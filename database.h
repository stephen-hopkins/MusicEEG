#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>
#include <QObject>

class Database : public QObject
{
    Q_OBJECT

public:
    Database();
    ~Database();
    QStringList getUsers();

public slots:

    void saveUserTrack (QString user, QString artist, QString track, QList< QList<float> > rawEmoData, QList<float> averages, QList<float> changes);
    void saveNewUser(QString user);

private:
    QSqlDatabase db;
    bool dbInitialised();

};

#endif // DATABASE_H
