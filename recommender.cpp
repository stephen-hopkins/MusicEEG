#include "recommender.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <CMath>
#include <QVariant>
#include <iostream>

Recommender::Recommender(Database* d) : db(d)
{
    setupDetailsStatsUTIDs();
    setupHighLows();
    //setupThresholds();

    testing();

}

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

        int bint = userTracks.value(16).toInt();
        bool userLiked = userTracks.value(16).toBool();

        // Add to appropriate lists, rinse and repeat
        details << theseDetails;
        stats << theseStats;
        liked << userLiked;
        currentIndex++;
        validUTIDs << UTid;
    }
}

void Recommender::setupHighLows()
{
    // set up empty 2D array of correct size
    int noRecords = validUTIDs.size();
    int highestUTID = validUTIDs[noRecords-1];
    highlows = new char*[highestUTID+1];
    for (int n = 0 ; n < highestUTID+1 ; n++) {
        highlows[n] = new char[12];
    }

    // go though each metric, adding to QMap, and tagging appropriately
    for (int metric = 0 ; metric < 12 ; metric++) {
        QMultiMap<float, int> utIDbyScore;
        QList<int>::const_iterator utID;
        for (utID = validUTIDs.begin() ; utID != validUTIDs.end() ; utID++) {
            utIDbyScore.insert(stats[*utID][metric], *utID );
        }

        //tag top and bottom third
        QList<int> orderedUTids = utIDbyScore.values();
        for (int n = 0 ; n < noRecords / 3 ; n++) {
            highlows[orderedUTids[n]][metric] = 'L';
        }
        for (int n = noRecords - 1 ; n >=  noRecords-(noRecords/3) ; n--) {
            highlows[orderedUTids[n]][metric] = 'H';
        }
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

QMultiMap<float, QStringList> Recommender::getSimilarByScores(int UTid)
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

void Recommender::getRecommendationsOwnCont(int row)
{
    int utID = validUTIDs[row];
    QMultiMap<float, QStringList> results = getSimilarByScores(utID);
    emit newRecs(results);
}

void Recommender::getRecommendationsCont(int row)
{
    int utID = validUTIDs[row];
    QList<int> sameTrackDiffUsers;
    QMultiMap<float, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (*i != utID) {
            if ( (details[utID][1] == details[*i][1]) && (details[utID][2] == details[utID][2]) ) {
                if (details[utID][0] != details[*i][0]) {
                    sameTrackDiffUsers << *i;
                }
            }
        }
    }

    if (sameTrackDiffUsers.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Error: track has not been played by any other users");
        msgBox.exec();
    }

    while (!sameTrackDiffUsers.isEmpty()) {
        results += getSimilarByScores(sameTrackDiffUsers.takeFirst());
    }

    emit newRecs(results);
}

QMultiMap<int, QStringList> Recommender::getSimilarByHYs(int UTid)
{
    QMultiMap<int, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if ( (*i != UTid) && (details[UTid][0] == details[*i][0]) ) {
            int diff = 0;
            for (int metric = 0 ; metric < 12 ; metric++) {
                if (highlows[*i][metric] == highlows[UTid][metric])
                    diff++;
                else if (highlows[*i][metric] == 'H' && highlows[UTid][metric] == 'L')
                    diff--;
                else if (highlows[*i][metric] == 'L' && highlows[UTid][metric] == 'H')
                    diff--;
            }
            QStringList userArtistTrack;
            userArtistTrack << details[*i][0] << details[*i][1] << details[*i][2];
            results.insert(diff, userArtistTrack);
        }
    }
    return results;
}

void Recommender::getRecommendationsOwnDisc(int row)
{
    int utID = validUTIDs[row];
    QMultiMap<int, QStringList> results = getSimilarByHYs(utID);
    emit newRecs(results);
}

void Recommender::getRecommendationsDisc(int row)
{
    int utID = validUTIDs[row];
    QList<int> sameTrackDiffUsers;
    QMultiMap<int, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (*i != utID) {
            if ( (details[utID][1] == details[*i][1]) && (details[utID][2] == details[utID][2]) ) {
                if (details[utID][0] != details[*i][0]) {
                    sameTrackDiffUsers << *i;
                }
            }
        }
    }

    if (sameTrackDiffUsers.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Error: track has not been played by any other users");
        msgBox.exec();
    }

    while (!sameTrackDiffUsers.isEmpty()) {
        results += getSimilarByHYs(sameTrackDiffUsers.takeFirst());
    }
    emit newRecs(results);
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

/* OLD CODE
    // set threshold to initial value of 0.3
    float threshold = 0.05;
    int currentCorrect = noCorrectlyClassified(user, threshold);

    return calcLikesThresholdHelper(user, utIDsByStddev, false, false, threshold, currentCorrect);

    */

float Recommender::calcLikesThresholdHelper(QString user, QMultiMap<float, int> utIDsByStddev, bool higher, bool lower, float threshold, int currentCorrect)
{
    // calc what thresholds would have to be to change classifications by 1
    // if moved higher, check if going higher improves classifications
    // if moved lower, check if going lower improves classifications
    // if neither return current

    QList<float> stddevs = utIDsByStddev.keys();


    // check correct if threshold one lower
    if (!higher && (threshold > stddevs.first()) ) {
        QList<float>::const_iterator below;
        for (below = stddevs.begin() ; (*below < threshold) && (below != stddevs.end()) ; below++) {
            float hello = *below;
            std::cout << hello;
        }
        below--;

        //debug
        float deboat = *below;

        float lowerThreshold = (*below + *(below-1)) / 2;
        int belowCorrect = noCorrectlyClassified(user, lowerThreshold);
        if (belowCorrect > currentCorrect) {
            return calcLikesThresholdHelper(user, utIDsByStddev, false, true, lowerThreshold, belowCorrect);
        }
    }

    // check correct if threshold one higher
    if (!lower && (threshold < stddevs.last())) {
        QList<float>::const_iterator above;
        for (above = stddevs.end() - 1 ; (*above > threshold) && (above != stddevs.begin()) ; above--) {}
        if (above != stddevs.begin()) {
            above ++;
        }
        float higherThreshold = (*above + *(above+1)) / 2;
        int aboveCorrect = noCorrectlyClassified(user, higherThreshold);
        if (aboveCorrect < currentCorrect) {
            return calcLikesThresholdHelper(user, utIDsByStddev, true, false, higherThreshold, aboveCorrect);
        }
    }
    return threshold;
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

