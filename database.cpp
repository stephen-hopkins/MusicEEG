#include "database.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <iostream>
#include "meexception.h"
#include <QVariant>
#include <QString>

using namespace std;

Database::Database()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Profiles.sqlite");

    if (!db.open()) {
        cerr << "Error opening database";
    }

    if (!dbInitialised()) {
        QSqlQuery init(db);
        if (!init.exec("CREATE TABLE UserTracks(UTid integer primary key autoincrement, User text, Artist text, Album text, Length integer)")) {
            cerr << "Error creating initial table in database";
        }
    }
}

Database::~Database()
{
    db.close();
}

bool Database::dbInitialised()
{
    QSqlRecord userTracks = db.record("UserTracks");
    if (userTracks.isEmpty()) {
        return false;
    }
    else {
        return true;
    }
}

void Database::saveUserTrack(QString user, QString artist, QString track,
                            QList<float> engagement, QList<float> excitementST, QList<float> excitementLT, QList<float> frustration, QList<float> meditation)
{
    // test that all lists of equal length
    int length = (engagement.size() + excitementST.size() + excitementLT.size() + frustration.size() + meditation.size()) / 5;
    if (length != engagement.size()) {
        cerr << "Emotion lists not of equal size";
    }

    // Add entry into main table UserTracks
    QSqlQuery addData(db);
    QString hello = QString("INSERT INTO UserTracks VALUES(NULL, '%1', '%2', '%3', '%4')").arg(user).arg(artist).arg(track).arg(length);
    cout << hello.toStdString();
    addData.prepare(QString("INSERT INTO UserTracks VALUES(NULL, '%1', '%2', '%3', '%4')").arg(user).arg(artist).arg(track).arg(length));
    if (!addData.exec()) {
        cerr << "Error adding new UserTrack to database";
    }
    QString utID = addData.lastInsertId().toString();

    // Create new table to hold emotion data
    addData.prepare(QString("CREATE TABLE UTid%1(Second integer primary key autoincrement, Engagement real, ExcitementST real, ExcitementLT real, Frustration real, Meditation real)").arg(utID));
    if (!addData.exec()) {
        cerr << "Error adding new table to hold emotion data";
    }

    addData.prepare(QString("INSERT INTO UTid%1 VALUES(NULL, :engagement, :excitementST, :excitementLT, :frustration, :meditation)").arg(utID));
    while (!engagement.isEmpty()) {
        addData.bindValue(":engagement", engagement.takeFirst());
        addData.bindValue(":excitementST", excitementST.takeFirst());
        addData.bindValue(":excitementLT", excitementLT.takeFirst());
        addData.bindValue(":frustration", frustration.takeFirst());
        addData.bindValue(":meditation", meditation.takeFirst());
        if (!addData.exec()) {
            cerr << "Error adding record to UTid table" << endl;
        }
    }
    return;
}


