#include "recommender.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <CMath>
#include <QVariant>
#include <iostream>
#include <QTime>


Recommender::Recommender(Database* d) : db(d)
{
    setupDetailsStatsUTIDs();
    setupThresholds();
}

/*
void Recommender::testing()
{
    // should be true false true
    bool tool1, tool2, tool3;
    tool1 = classify(5, 0.25);
    tool2 = classify(5, 0.35);
    tool3 = classify(16, 0.1);

    // should be 14, 9, 2
    int bint1, bint2, bint3;
    bint1 = noCorrectlyClassified("Steve", 0.1);
    bint2 = noCorrectlyClassified("Steve", 0.3);
    bint3 = noCorrectlyClassified("Steve", 0.7);

    // should be around 0.3
    float boat = calcLikesThreshold("Steve");
    std::cout << boat;
}
*/

void Recommender::setupDetailsStatsUTIDs()
{
    // get details from database
    QSqlQuery userTracks = db->getAllRecords();
    if (!userTracks.isActive()) {
        QMessageBox msgBox;
        msgBox.setText("Fatal Error : Cannot retrieve usertrack records from database");
        msgBox.exec();
    }

    int currentIndex = 0;
    QStringList emptyQStringList;
    QList<float> emptyFloatList;
    while (userTracks.next()) {

        int UTid = userTracks.value(0).toInt();

        // Enter empty lists so that QList indices same as UTids
        while (UTid > currentIndex) {
            details << emptyQStringList;
            stats << emptyFloatList;
            liked << false;
            currentIndex++;
        }

        // Get user, artist, track details, followed by stats
        QStringList theseDetails;
        for (int n = 1 ; n < 4 ; n++) {
            theseDetails << userTracks.value(n).toString();
        }

        QList<float> theseStats;
        for (int n = 4 ; n < 16 ; n++) {
            theseStats << userTracks.value(n).toFloat();
        }
        bool userLiked = userTracks.value(16).toBool();

        // Add to appropriate lists, rinse and repeat
        details << theseDetails;
        stats << theseStats;
        liked << userLiked;
        currentIndex++;
        validUTIDs << UTid;
    }
}

void Recommender::setupThresholds()
{
    QSqlQuery usersQuery = db->getAllUsers();

    if (!usersQuery.isActive()) {
        QMessageBox msgBox;
        msgBox.setText("Fatal Error : Cannot retrieve user records from database");
        msgBox.exec();
    }
    int currentIndex = 0;
    QString emptyQString;
    while (usersQuery.next()) {
        int Uid = usersQuery.value(0).toInt();
        while (Uid > currentIndex) {
            thresholds.append(0.0);
            users.append(emptyQString);
            currentIndex++;
        }

        QString user = usersQuery.value(1).toString();
        float likeThreshold = usersQuery.value(2).toFloat();
        if (likeThreshold == 0) {
            likeThreshold = calcLikesThreshold(user);
            emit newThreshold(user, likeThreshold);
        }
        thresholds.append(likeThreshold);
        users.append(user);
        currentIndex++;
    }
}

void Recommender::saveThresholds()
{
    QSqlQuery usersQuery = db->getAllUsers();

    while (usersQuery.next()) {
        int Uid = usersQuery.value(0).toInt();

        QString user = usersQuery.value(1).toString();
        float likeThreshold = usersQuery.value(2).toFloat();
        if (likeThreshold == 0) {
            likeThreshold = thresholds[Uid];
            emit newThreshold(user, likeThreshold);
        }
    }
}


QMultiMap<float, QStringList> Recommender::getSimilarTracks(int UTid)
{
    QMultiMap<float, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if ( (*i  != UTid) && (details[UTid][0] == details[*i][0]) ) {
            float diff = 0;
            for (int n = 0 ; n < 12 ; n++) {
                if (n < 8)
                    diff += abs(stats[UTid][n] - stats[*i][n]);
                else
                    diff += abs((3*stats[UTid][n]) - (3*stats[*i][n]));
            }
            QStringList userArtistTrack;
            userArtistTrack << details[*i][0] << details[*i][1] << details[*i][2];
            results.insert(diff, userArtistTrack);
        }
    }
    return results;
}

QMultiMap<float, int> Recommender::getSimilarUTIDs(int UTid)
{
    QMultiMap<float, int> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if ( (*i  != UTid) && (details[UTid][0] == details[*i][0]) ) {
            float diff = 0;
            for (int n = 0 ; n < 12 ; n++) {
                if (n < 8)
                    diff += abs(stats[UTid][n] - stats[*i][n]);
                else
                    diff += abs((3*stats[UTid][n]) - (3*stats[*i][n]));
            }
            results.insert(diff, *i);
        }
    }
    return results;
}

void Recommender::displaySimilarOwn(int row)
{
    int utID = validUTIDs[row];
    QMultiMap<float, QStringList> results = getSimilarTracks(utID);
    emit newRecs(results);
}

void Recommender::displaySimilarOthers(int row)
{
    int utID = validUTIDs[row];
    QList<int> sTDU = getSameTrackDiffUsers(utID);
    QMultiMap<float, QStringList> results;

    if (sTDU.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Error: track has not been played by any other users");
        msgBox.exec();
    }

    while (!sTDU.isEmpty()) {
        results += getSimilarTracks(sTDU.takeFirst());
    }

    emit newRecs(results);
}

QList<int> Recommender::getSameTrackDiffUsers(int utID)
{
    QList<int> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (*i != utID) {
            if ( (details[utID][1] == details[*i][1]) && (details[utID][2] == details[utID][2]) ) {
                if (details[utID][0] != details[*i][0]) {
                    results << *i;
                }
            }
        }
    }
    return results;
}

float Recommender::calcLikesThreshold(QString user)
{
    // create sorted list of scores (all 4 std devs added together)
    QMap<float, int> utIDsByStddev;
    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (details[*i][0] == user) {
            float totalStddevs = 0;
            for (int n = 8 ; n < 12 ; n++) {
                totalStddevs += stats[*i][n];
            }
            utIDsByStddev.insert(totalStddevs, *i);
        }
    }

    // add 0 and 1 to sorted list, and create second list of possible threshold values
    utIDsByStddev.insert(0.0, 0);
    utIDsByStddev.insert(1.0, 0);

    QMap<int,float> thresholdsByNoCorrect;

    QMap<float,int>::const_iterator it = utIDsByStddev.constBegin();
    it++;
    while (it != (utIDsByStddev.constEnd() - 1) ) {
        float thres = it.key() - ((it.key() - (it-1).key()) / 2);
        thresholdsByNoCorrect.insert(noCorrectlyClassified(user, thres), thres);
        it++;
    }

    QMap<int, float>::const_iterator last = thresholdsByNoCorrect.constEnd() - 1;
    return last.value();
}

int Recommender::noCorrectlyClassified(QString user, float threshold)
{
    int noCorrect = 0;
    for (int utID = 0 ; utID < details.size() ; utID++) {
        if (details[utID].isEmpty() || details[utID][0] != user) {
            continue;
        }
        else if (classify(utID, threshold) == liked[utID]) {
            noCorrect++;
        }
    }
    return noCorrect;
}

bool Recommender::classify(int utID, float threshold)
{
    float total = 0;
    for (int n = 8 ; n < 12 ; n++) {
        total += stats[utID][n];
    }
    return (total >= threshold);
}

QMultiMap<float, QStringList> Recommender::getRecommendations(QString user)
{
    // build list of UTIDs this user has liked
    QList<int> likedUTIDs;
    QList<int>::const_iterator utID;
    for (utID = validUTIDs.constBegin() ; utID != validUTIDs.constEnd() ; utID++) {
        if ( (details[*utID][0] == user) && liked[*utID]) {
            likedUTIDs.append(*utID);
        }
    }

    // for each liked UTID get similar from other users
    // add score (like x similarity) to list
    QMultiMap<float, QStringList> recs;
    QMultiMap<float, int> similar;

    for (utID = likedUTIDs.constBegin() ; utID != likedUTIDs.constEnd() ; utID++) {

        //calculate how much user liked this
        float likeScore = 0;
        for (int n = 8 ; n < 12 ; n++) {
            likeScore += stats[*utID][n];
        }

        // build list of same track diff users
        QList<int> stdu = getSameTrackDiffUsers(*utID);

        QList<int>::const_iterator stduI;

        for (stduI = stdu.constBegin() ; stduI != stdu.constEnd() ; stduI++) {

            // create qmap with similar UTids from other users
            similar.clear();
            similar = getSimilarUTIDs(*stduI);

            QMultiMap<float, int>::const_iterator similarI;
            for (similarI = similar.constBegin() ; similarI != similar.constEnd() ; similarI++) {
                int utid = similarI.value();
                QStringList artistTrack;
                artistTrack << details[utid][1] << details[utid][2];
                // skip if user already listened
                if (listenedToTrack(user, artistTrack[0], artistTrack[1])) {
                    continue;
                }
                float newScore = likeScore / similarI.key();
                recs.insert(newScore, artistTrack);
            }
        }
    }

    // check for duplicates, and if so average scores

    QMultiMap<float, QStringList> dupesRemoved;
    QMultiMap<float, QStringList>::const_iterator first;
    while (!recs.isEmpty()) {
        first = recs.constBegin();
        QStringList details = first.value();
        QList<float> scores = recs.keys(details);

        float totalScore = 0;
        int items = scores.size();
        while (!scores.isEmpty()) {
            float thisScore = scores.takeFirst();
            totalScore += thisScore;
            recs.remove(thisScore, details);
        }

        float aveScore = totalScore / items;
        dupesRemoved.insert(aveScore, details);
    }
    return dupesRemoved;
}

bool Recommender::listenedToTrack(QString user, QString artist, QString track)
{
    QList<int>::const_iterator utid;
    for (utid = validUTIDs.constBegin() ; utid != validUTIDs.constEnd() ; utid++) {
        if ( (details[*utid][1] == artist) && (details[*utid][2] == track) && (details[*utid][0] == user) ) {
            return true;
        }
    }
    return false;
}

void Recommender::addUser(int uID, QString newUser)
{
    while (users.size() < (uID - 1)) {
        users.append(QString());
        thresholds.append(0.0);
    }
    users.append(newUser);
    float aveThres = 0.0;
    int noUsers = 0;
    for (int n = 1 ; n < thresholds.size() ; n++) {
        aveThres += thresholds[n];
        if (thresholds[n] != 0.0) {
            noUsers++;
        }
    }
    aveThres /= noUsers;
    thresholds.append(aveThres);
    emit newThreshold(newUser, aveThres);
}


void Recommender::addNewTrack(int utID, QString user, QString artist, QString track, QList< QList<float> > thisstats)
{
    // update details, stats, validUTIDs
    validUTIDs.append(utID);
    while ( stats.size() < (utID - 1) ) {
        QStringList emptyQSL;
        details << emptyQSL;
        QList<float> emptyQLF;
        stats << emptyQLF;
        liked << false;
    }
    QStringList deets;
    deets << user << artist << track;
    details.append(deets);
    QList<float> statsToAdd;
    for (int type = 0 ; type < 3 ; type++) {
        for (int emo = 0 ; emo < 4 ; emo++) {
            statsToAdd << thisstats[type][emo];
        }
    }
    stats << statsToAdd;

    // calc if user liked track, and check
    int uID;
    for (uID = 1 ; users[uID] != user ; uID++) {}
    float threshold = thresholds[uID];
    float stddevTotal = 0;
    for (int n = 0 ; n < 4 ; n++) {
        stddevTotal += thisstats[2][n];
    }
    bool estimateLike = false;
    if (stddevTotal > threshold) {
        estimateLike = true;
    }

    QMessageBox msgBox;
    if (estimateLike) {
        msgBox.setText("I think you liked this track.  Is that correct?");
    }
    else {
        msgBox.setText("I think you didn't like this track.  Is that correct?");
    }
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    bool userLike;
    if (ret == QMessageBox::Yes) {
        userLike = estimateLike;
        liked << userLike;
    }
    else {
        userLike = !estimateLike;
        liked << userLike;
        thresholds[uID] = calcLikesThreshold(user);
        emit newThreshold(user, thresholds[uID]);

    }
    emit userLikeConfirmation(utID, userLike);
}

void Recommender::crossValidation()
{
    // build list of totals & likeds
    QList<float> totStdDevs;
    QList<bool> likes;

    QList<int>::const_iterator utid;
    for (utid = validUTIDs.constBegin() ; utid != validUTIDs.constEnd() ; utid++) {
        float total = 0;
        for (int n = 8 ; n < 12 ; n++) {
            total += stats[*utid][n];
        }
        totStdDevs.append(total);
        likes.append(liked[*utid]);
    }

    // split into 10 datasets
    QList<QList<float> > scoreDataset;
    QList<QList<bool> > likeDataset;

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());

    int fold = 0;
    scoreDataset.append(QList<float>());
    likeDataset.append(QList<bool>());
    while (!totStdDevs.isEmpty()) {
        int rand = qrand() % totStdDevs.size();
        if (scoreDataset[fold].size() == 5) {
            fold++;
            scoreDataset.append(QList<float>());
            likeDataset.append(QList<bool>());
        }
        scoreDataset[fold].append(totStdDevs.takeAt(rand));
        likeDataset[fold].append(likes.takeAt(rand));
    }

    int TP = 0;
    int FN = 0;
    int FP = 0;
    int TN = 0;

    // go through each fold, train on others, test
    for (fold = 0 ; fold < 10 ; fold++) {
        QList<float> trainingScores;
        QList<float> testScores;
        QList<bool> trainingLikes;
        QList<bool> testLikes;
        testScores = scoreDataset[fold];
        testLikes = likeDataset[fold];
        for (int n = 0 ; n < 10 ; n++) {
            if (n != fold) {
                trainingScores += scoreDataset[fold];
                trainingLikes += likeDataset[fold];
            }
        }

        float threshold = validCalcLikesThreshold(trainingScores, trainingLikes);

        for (int n = 0 ; n < testScores.size() ; n++) {
            if (testLikes[n]) {
                if (testScores[n] > threshold) {
                    TP++;
                }
                else {
                    FN++;
                }
            }
            else {
                if (testScores[n] > threshold) {
                    FP++;
                }
                else {
                    TN++;
                }
            }
        }
    }
    std::cout << "TP: " << TP << std::endl;
    std::cout << "FN: " << FN << std::endl;
    std::cout << "FP: " << FP << std::endl;
    std::cout << "TN: " << TN << std::endl;

    return;
}

float Recommender::validCalcLikesThreshold(QList<float> scores, QList<bool> likes)
{
    // create sorted list of scores
    QMultiMap<float, bool> scoresLikes;
    for (int n = 0 ; n < scores.size() ; n++) {
        scoresLikes.insert(scores[n], likes[n]);
    }

    // add 0 and 1 to sorted list, and create second list of possible threshold values
    scoresLikes.insert(0.0, false);
    scoresLikes.insert(1.0, false);

    QMap<int,float> noCorrectThresholds;

    QMultiMap<float,bool>::const_iterator score = scoresLikes.constBegin();
    score++;
    while (score != scoresLikes.constEnd()) {
        float thres = score.key() - ((score.key() - (score-1).key()) / 2);
        noCorrectThresholds.insert(validClassify(thres, scores, likes), thres);
        score++;
    }
    QMap<int, float>::const_iterator last = noCorrectThresholds.constEnd() - 1;
    return last.value();
}

int Recommender::validClassify(float thres, QList<float> scores, QList<bool> likes)
{
    int noCorrect = 0;
    for (int n = 0 ; n < scores.size() ; n++) {
        if ((scores[n] > thres) == likes[n]) {
            noCorrect++;
        }
    }
    return noCorrect;
}


