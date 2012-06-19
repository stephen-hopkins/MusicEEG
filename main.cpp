#include <QApplication>
#include "mainw.h"
#include <iostream>
#include "headset.h"
#include "windows.h"
#include "database.h"

using namespace std;

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainW w;
    Database database;

    w.show();

    return a.exec();


    return 0;
}










