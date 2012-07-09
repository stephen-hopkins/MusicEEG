#include <QApplication>
#include "mainw.h"
#include <iostream>
#include "headset.h"
#include "windows.h"
#include "database.h"
#include <QMetaType>

Q_DECLARE_METATYPE(QList<float>)

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Database database;
    Headset headset;
    MainW w;

    qRegisterMetaType<QList<float> >();


    QObject::connect(&w, SIGNAL(startRecording(QString,QString,QString)),
                     &headset, SLOT(initialise(QString,QString,QString)));
    QObject::connect(&w, SIGNAL(logEmoState()),
                     &headset, SLOT(logEmoState()));
    QObject::connect(&w, SIGNAL(stopRecording()),
                     &headset, SLOT(writeData()));
    QObject::connect(&w, SIGNAL(cancelRecording()),
                     &headset, SLOT(discardData()));

    QObject::connect(&headset, SIGNAL(newUserTrack(QString,QString,QString,QList<float>,QList<float>,QList<float>,QList<float>,QList<float>)),
                     &database, SLOT(saveUserTrack(QString,QString,QString,QList<float>,QList<float>,QList<float>,QList<float>,QList<float>)), Qt::QueuedConnection);

    w.show();
    return a.exec();
}










