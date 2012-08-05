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

    // stats 0-3 Mean Engagement, Excitement, Frustration, Meditation.  4-7 Change, 8-11 Std Devs.
    QList< QList<float> > stats;
    QList<int> validUTIDs;
    QList<bool> liked;

    // indexed by UTId
    char** highlows;

    // thresholds, indexed by Uid
    QList<float> thresholds;

    void setupDetailsStatsUTIDs();
    void setupHighLows();
    void setupThresholds();
    QMultiMap<float, QStringList> getSimilarByScores(int UTid);
    QMultiMap<int, QStringList> getSimilarByHYs(int UTid);
    float calcLikesThreshold(QString user);
    float calcLikesThresholdHelper(QString user, QMultiMap<float, int> utIDsByStddev, bool higher, bool lower, float threshold, int currentCorrect);
    int noCorrectlyClassified(QString user, float threshold);
    bool classify(int utID, float threshold);

    void testing();

signals:
    void newRecs(QMultiMap<float, QStringList>);
    void newRecs(QMultiMap<int, QStringList>);

};

#endif // RECOMMENDER_H
