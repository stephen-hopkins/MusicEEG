#include "database.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <iostream>

using namespace std;

Database::Database()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("Profiles.sqlite");
    db.open();
    if (!dbInitialised()) {
        QSqlQuery init(db);
        if (!init.exec("CREATE TABLE UserTracks(UTid integer primary key autoincrement, User text, Artist text, Album text, Length integer)")) {
            cerr << "Error initialising database";
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
