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

    void displaySimilarOthers(int);
    void displaySimilarOwn(int);
    QMultiMap<float, QStringList> getRecommendations(QString user);

public slots:
    void addNewTrack(int utID, QString user, QString artist, QString track, QList< QList<float> > thisstats);
    void addUser(int uID, QString newUser);

private:

    Database* db;

    // details, indexed by UTid, User, Artist, Track
    QList<QStringList> details;

    // stats 0-3 Mean Engagement, Excitement, Frustration, Meditation.  4-7 Change, 8-11 Std Devs.
    QList< QList<float> > stats;
    QList<int> validUTIDs;
    QList<bool> liked;

    // thresholds, indexed by Uid
    QList<float> thresholds;
    QStringList users;

    void setupDetailsStatsUTIDs();
    void setupHighLows();
    void setupThresholds();
    QMultiMap<float, int> getSimilarUTIDs(int UTid);
    QMultiMap<float, QStringList> getSimilarTracks(int UTid);

    float calcLikesThreshold(QString user);
    int noCorrectlyClassified(QString user, float threshold);
    bool classify(int utID, float threshold);
    bool listenedToTrack(QString, QString, QString);
    QList<int> getSameTrackDiffUsers(int utID);

    void testing();

signals:
    void newRecs(QMultiMap<float, QStringList>);
   // void newRecs(QMultiMap<int, QStringList>);
    void userLikeConfirmation(int utID, bool userLike);
    void newThreshold(QString user, float newThreshold);

};

#endif // RECOMMENDER_H
