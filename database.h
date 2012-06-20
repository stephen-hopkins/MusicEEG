#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QList>

class Database
{
public:
    Database();
    ~Database();
    void newUserTrack (QString, QString, QString,
                      QList<float>, QList<float>, QList<float>, QList<float>, QList<float>);

private:
    QSqlDatabase db;
    bool dbInitialised();
};

#endif // DATABASE_H
