#include "database.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <iostream>
#include <QVariant>
#include <QStringList>

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
        init.prepare("CREATE TABLE UserTracks(UTid integer primary key autoincrement, User text, Artist text, Track text, Length integer, MeanEng real, MeanExc real, MeanFrus real, MeanMed real, ChaEng real, ChaExc real, ChaFrus real, ChaMed real, SDEng real, SDExc real, SDFrus real, SDMed real)");
        if (!init.exec()) {
            cerr << "Error creating UserTracks table in database";
        }
        init.prepare("CREATE TABLE Users(Uid integer primary key autoincrement, User text)");
        if (!init.exec()) {
            cerr << "Error creating Users table in database";
        }
        init.prepare("CREATE TABLE RawData(UTid integer, RDid integer primary key autoincrement, Engagement real, Excitement real, Frustration real, Meditation real)");
        if (!init.exec()) {
            cerr << "Error creating RawData table in database";
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
    return;
}

void Database::saveUserTrack(QString user, QString artist, QString track, QList< QList<float> > rawEmoData, QList< QList<float> > stats)
{
    // test input as expected
    if (rawEmoData[3].isEmpty()) {
        cerr << "Error: No Emo Data";
    }

    // Add entry into main table UserTracks
    QSqlQuery addData(db);
    addData.prepare(QString("INSERT INTO UserTracks VALUES(NULL, :user, :artist, :track, :length, :ave0, :ave1, :ave2, :ave3, :cha0, :cha1, :cha2, :cha3, :std0, :std1, :std2, :std3, NULL)"));
    addData.bindValue(":user", user);
    addData.bindValue(":artist", artist);
    addData.bindValue(":track", track);
    addData.bindValue(":length", rawEmoData[0].size());

    for (int n = 0 ; n < 4 ; n++) {
        QString ave = QString(":ave%1").arg(n);
        QString cha = QString(":cha%1").arg(n);
        QString std = QString(":std%1").arg(n);
        addData.bindValue(ave, stats[0][n]);
        addData.bindValue(cha, stats[1][n]);
        addData.bindValue(std, stats[2][n]);
    }

    if (!addData.exec()) {
        cerr << "Error adding new UserTrack to database" << endl;
    }

    int utID = addData.lastInsertId().toInt();

    // Insert raw data
    addData.prepare(QString("INSERT INTO RawData VALUES(%1, NULL, :engagement, :excitement, :frustration, :meditation)").arg(utID));

    while (!rawEmoData[0].isEmpty()) {
        addData.bindValue(":engagement", rawEmoData[0].takeFirst());
        addData.bindValue(":excitement", rawEmoData[1].takeFirst());
        addData.bindValue(":frustration", rawEmoData[2].takeFirst());
        addData.bindValue(":meditation", rawEmoData[3].takeFirst());
        if (!addData.exec()) {
            cerr << "Error adding record to RawData table" << endl;
        }
    }
    emit newUserTrackSaved(utID, user, artist, track, stats);

    return;
}

QStringList Database::getUsers()
{
    QStringList users;
    QSqlQuery usersQuery("SELECT User FROM Users");
    while (usersQuery.next()) {
        QString user = usersQuery.value(0).toString();
        users.append(user);
    }
    return users;
}

QSqlQuery Database::getAllRecords()
{
    QSqlQuery utQuery(db);
    utQuery.exec("SELECT UTid, User, Artist, Track, MeanEng, MeanExc, MeanFrus, MeanMed, ChaEng, ChaExc, ChaFrus, ChaMed, SDEng, SDExc, SDFrus, SDMed, Likes FROM UserTracks");
    return utQuery;
}

QSqlQuery Database::getAllUsers()
{
    QSqlQuery users(db);
    users.exec("SELECT Uid, User, LikeThreshold FROM Users");
    return users;
}

void Database::saveUserLike(int utID, bool userLike)
{
    QSqlQuery saveLike(db);
    int liked = 0;
    if (userLike) {
        liked = 1;
    }
    saveLike.prepare(QString("UPDATE UserTracks SET Likes= %1 WHERE UTid= %2").arg(liked).arg(utID));
    saveLike.exec();
}

void Database::amendUserThreshold(QString user, float newThreshold)
{
    QSqlQuery amend(db);
    amend.prepare(QString("UPDATE Users SET LikeThreshold= %1 WHERE User= %2").arg(newThreshold).arg(user));
    amend.exec();
}
