#include "recommender.h"
#include <QSqlQuery>
#include <QMessageBox>

Recommender::Recommender(Database* d) : db(d)
{
    // get details from database
    QSqlQuery utQuery(db);
    utQuery.exec("SELECT UTid, User, Artist, Track, MeanEng, MeanExc, MeanFrus, MeanMed, ChaEng, ChaExc, ChaFrus, ChaMed, SDEng, SDExc, SDFrus, SDMed FROM UserTracks");
    if (!utQuery.isActive) {
        QMessageBox msgBox;
        msgBox.setText("Fatal Error : Cannot retrieve records from database");
        msgBox.exec();
    }

    int currentIndex = 0;
    QStringList emptyQStringList;
    QList<float> emptyFloatList;
    while (utQuery.next()) {

        int UTid = utQuery.value(0).toInt();

        // Enter empty lists so that QList indices same as UTids
        while (UTid > currentIndex) {
            details << emptyQStringList;
            stats << emptyFloatList;
            currentIndex++;
        }

        // Get user, artist, track details, followed by stats
        QStringList theseDetails;
        for (int n = 1 ; n < 4 ; n++) {
            theseDetails << utQuery.value(n).toString();
        }

        QList<float> theseStats;
        for (int n = 4 ; n < 16 ; n++) {
            theseStats << utQuery.value(n).toFloat();
        }

        // Add to appropriate lists, rinse and repeat
        details << theseDetails;
        stats << theseStats;
        currentIndex++;
        validUTIDs << UTid;
    }
}

Recommender::getSimilar(int UTid)
{
    QList<float> currentStats = stats[UTid];

}
