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

public slots:

    void saveUserTrack (QString, QString, QString,
                      QList<float>, QList<float>, QList<float>, QList<float>, QList<float>);

private:
    QSqlDatabase db;
    bool dbInitialised();
};

#endif // DATABASE_H
