#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include "database.h"
//#include <QList>
//#include <QString>

class Recommender
{
public:
    Recommender(Database* d);
private:
    Database* db;
    QList<QStringList> details;
    QList< QList<float> > stats;
    QList<int> validUTIDs;

};

#endif // RECOMMENDER_H
