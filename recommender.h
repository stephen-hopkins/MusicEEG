#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include "database.h"
#include <QMultiMap>

class Recommender
{
public:
    Recommender(Database* d);
    QMultiMap<float, QStringList> getSimilarByScores(int UTid);
    QMultiMap<int, QStringList> getSimilarByHYs(int UTid);
    QMultiMap<float, QStringList> getRecommendationsCont(int UTid);
    QMultiMap<int, QStringList> getRecommendationsDisc(int UTid);

private:

    Database* db;
    QList<QStringList> details;
    QList< QList<float> > stats;
    QList<int> validUTIDs;
    char** highlows;

    void setupDetailsStatsUTIDs();
    void setupHighLows();

};

#endif // RECOMMENDER_H
