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

    // debugging

    int noRecords = validUTIDs.size();
    int highestUTID = validUTIDs[noRecords-1];

    std::cout << "highlows: " << std::endl;

    for (int ut = 0 ; ut < highestUTID+1 ; ut++) {
        for (int metric = 0 ; metric < 12 ; metric++) {
            std::cout << highlows[ut][metric] << " ";
        }
        std::cout << std::endl;
    }

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

        // Add to appropriate lists, rinse and repeat
        details << theseDetails;
        stats << theseStats;
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

QMultiMap<float, QStringList> Recommender::getRecommendationsCont(int UTid)
{
    QList<int> sameTrackDiffUsers;
    QMultiMap<float, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (*i != UTid) {
            if ( (details[UTid][1] == details[*i][1]) && (details[UTid][2] == details[UTid][2]) ) {
                if (details[UTid][0] != details[*i][0]) {
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

    return results;
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

QMultiMap<int, QStringList> Recommender::getRecommendationsDisc(int UTid)
{
    QList<int> sameTrackDiffUsers;
    QMultiMap<int, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (*i != UTid) {
            if ( (details[UTid][1] == details[*i][1]) && (details[UTid][2] == details[UTid][2]) ) {
                if (details[UTid][0] != details[*i][0]) {
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
    return results;
}

