#include "database.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <iostream>
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
        init.prepare("CREATE TABLE UserTracks(UTid integer primary key autoincrement, User text, Artist text, Track text, Length integer, AveEngagement real, AveExcitementST real, AveExcitementLT real, AveFrustration real, AveMeditation real, ChaEngagement real, ChaExcitementST real, ChaExcitmentLT real, ChaFrustration real, ChaMeditation real)");
        if (!init.exec()) {
            cerr << "Error creating UserTracks table in database";
        }
        init.prepare("CREATE TABLE Users(Uid integer primary key autoincrement, User text)");
        if (!init.exec()) {
            cerr << "Error creating Users table in database";
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

void Database::saveNewUser(QString user)
{
    if (user.isEmpty()) {
        cerr << "Error: User name blank";
    }
    else {
        QSqlQuery addUser(db);
        addUser.prepare(QString("INSERT INTO Users VALUES(NULL, '%1')").arg(user));
        if (!addUser.exec()) {
            cerr << "Error adding new User to Database";
        }
    }
}

void Database::saveUserTrack(QString user, QString artist, QString track, QList< QList<float> > rawEmoData)
{
    // test input as expected
    if (rawEmoData[4].isEmpty()) {
        cerr << "Error: No Emo Data";
    }

    // calculate averages & change in averages
    QList<float> averages;
    QList<float> changes;

    for (int n = 0 ; n < 5 ; n++) {
        averages[n] = calcAverage(rawEmoData[n]);
    }

    for (int n = 0 ; n < 5 ; n++) {
        float avebeginning = calcAverage(rawEmoData[n].mid(0, 30));
        float aveend = calcAverage(rawEmoData[n].mid((rawEmoData[n].length())-30, 30 ));
        changes[n] = aveend - avebeginning;
    }


    // Add entry into main table UserTracks
    QSqlQuery addData(db);
    addData.prepare(QString("INSERT INTO UserTracks VALUES(NULL, :user, :artist, :track, :length, :ave0, :ave1, :ave2, :ave3, :ave4, :cha0, :cha1, :cha2, :cha4, :cha4"));
    addData.bindValue(":user", user);
    addData.bindValue(":artist", artist);
    addData.bindValue(":track", track);
    addData.bindValue(":length", rawEmoData[0].size());

    for (int n = 0 ; n < 5 ; n++) {
        QString ave = QString(":ave%1").arg(n);
        QString cha = QString(":cha%1").arg(n);
        addData.bindValue(ave, averages[n]);
        addData.bindValue(cha, changes[n]);
    }

    if (!addData.exec()) {
        cerr << "Error adding new UserTrack to database";
    }

    QString utID = addData.lastInsertId().toString();

    // Create new table to hold emotion data
    addData.prepare(QString("CREATE TABLE UTid%1(Second integer primary key autoincrement, Engagement real, ExcitementST real, ExcitementLT real, Frustration real, Meditation real)").arg(utID));
    if (!addData.exec()) {
        cerr << "Error adding new table to hold raw emotion data";
    }

    addData.prepare(QString("INSERT INTO UTid%1 VALUES(NULL, :engagement, :excitementST, :excitementLT, :frustration, :meditation)").arg(utID));
    while (!rawEmoData[0].isEmpty()) {
        addData.bindValue(":engagement", rawEmoData[0].takeFirst());
        addData.bindValue(":excitementST", rawEmoData[1].takeFirst());
        addData.bindValue(":excitementLT", rawEmoData[2].takeFirst());
        addData.bindValue(":frustration", rawEmoData[3].takeFirst());
        addData.bindValue(":meditation", rawEmoData[4].takeFirst());
        if (!addData.exec()) {
            cerr << "Error adding record to UTid table" << endl;
        }
    }
    return;
}

float Database::calcAverage(QList<float> input)
{
   float total = 0;
   int items = input.size();
   while (!input.isEmpty()) {
       total += input.takeFirst();
   }
   return total / items;
}

