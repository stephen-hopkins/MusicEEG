#ifndef RECOMMENDER_H
#define RECOMMENDER_H

#include "database.h"
#include <QMultiMap>
#include <QObject>
#include <QStringList>

class Recommender : public QObject
{
    Q_OBJECT
public:

    Recommender(Database* d);

    void getRecommendationsCont(int);
    void getRecommendationsDisc(int);
    void getRecommendationsOwnCont(int);
    void getRecommendationsOwnDisc(int);

private:

    Database* db;

    // details, indexed by UTid, User, Artist, Track
    QList<QStringList> details;

    // stats Mean Engagement, Excitement, Frustration, Meditation.  Then change, then Std Devs.
    QList< QList<float> > stats;

    QList<int> validUTIDs;
    char** highlows;

    void setupDetailsStatsUTIDs();
    void setupHighLows();

    QMultiMap<float, QStringList> getSimilarByScores(int UTid);
    QMultiMap<int, QStringList> getSimilarByHYs(int UTid);

signals:
    void newRecs(QMultiMap<float, QStringList>);
    void newRecs(QMultiMap<int, QStringList>);

};

#endif // RECOMMENDER_H
