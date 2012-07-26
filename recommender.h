#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include "database.h"
#include <QMultiMap>

class Recommender
{
public:
    Recommender(Database* d);
    QMultiMap<float, QStringList> getSimilar(int UTid);
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
