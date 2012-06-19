#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>

class Database
{
public:
    Database();
    ~Database();
    newUserTrack(QString user, QString artist, QString album, int length);
    newTrack(QList<float> engagement, QList<float> excitementST, QList<float> excitementLT, QList<float> frustration, QList<float> meditation);

private:
    QSqlDatabase db;
    bool dbInitialised();
};

#endif // DATABASE_H
