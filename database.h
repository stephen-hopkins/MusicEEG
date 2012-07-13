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

    void saveUserTrack (QString user, QString artist, QString track, QList< QList<float> > rawEmoData);
    void saveNewUser(QString user);

private:
    QSqlDatabase db;
    bool dbInitialised();
    float calcAverage(QList<float> input);
};

#endif // DATABASE_H
