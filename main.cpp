#include <QApplication>
#include "mainw.h"
#include <iostream>
#include "headset.h"
#include "windows.h"
#include "database.h"
#include "musicplayer.h"
#include <QMetaType>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainW mainWindow;
    mainWindow.show();
    return a.exec();
}










