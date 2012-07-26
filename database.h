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
    QSqlQuery getAllRecords();

public slots:

    void saveUserTrack (QString user, QString artist, QString track, QList< QList<float> > rawEmoData, QList< QList<float> > stats);
    void saveNewUser(QString user);

private:
    QSqlDatabase db;
    bool dbInitialised();

};

#endif // DATABASE_H
