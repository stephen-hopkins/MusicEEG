#include "recommender.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <CMath>
#include <QVariant>
#include <iostream>

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
        msgBox.setText("Fatal Error : Cannot retrieve records from database");
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
        msgBox.setText("Fatal Error : Cannot retrieve records from database");
        msgBox.exec();
    }
    int currentIndex = 0;
    while (usersQuery.next()) {
        int Uid = usersQuery.value(0).toInt();
        while (Uid > currentIndex) {
            thresholds.append(0.0);
            currentIndex++;
        }
        QString user = usersQuery.value(1).toString();
        thresholds.append(calcLikesThreshold(user));
        currentIndex++;
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

