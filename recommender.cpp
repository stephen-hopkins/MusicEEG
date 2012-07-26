#include "recommender.h"
#include <QSqlQuery>
#include <QMessageBox>
#include <CMath>
#include <QVariant>

Recommender::Recommender(Database* d) : db(d)
{
    setupDetailsStatsUTIDs();
    setupHighLows();
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
        QMap<float, int> utIDbyScore;
        QList<int>::const_iterator utID;
        /*
        for (utID = validUTIDs.begin() ; utID != validUTIDs.end() ; utID++) {
            utIDbyScore.insert(stats[*utID][metric], *utID );
        }*/
        for (int poo = 0 ; poo < noRecords ; poo++) {
            utIDbyScore.insert(stats[validUTIDs[poo]][metric], validUTIDs[poo]);
        }

        //tag top and bottom third
        QList<int> orderedUTids = utIDbyScore.values();
        for (int n = 0 ; n < noRecords / 3 ; n++) {
            highlows[orderedUTids[n]][metric] = 'H';
        }
        for (int n = noRecords - 1 ; n >=  noRecords-(noRecords/3) ; n--) {
            highlows[orderedUTids[n]][metric] = 'L';
        }
    }
}

QMultiMap<float, QStringList> Recommender::getSimilar(int UTid)
{
    QMultiMap<float, QStringList> results;

    QList<int>::const_iterator i;
    for (i = validUTIDs.begin() ; i != validUTIDs.end() ; i++) {
        if (*i  != UTid) {
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


