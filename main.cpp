#include <QApplication>
#include "mainw.h"
#include <iostream>
#include "headset.h"
#include "windows.h"
#include "database.h"
#include "meexception.h"

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Database database;
    Headset headset(&database);
    MainW w(&headset);

    w.show();
    return a.exec();
}










