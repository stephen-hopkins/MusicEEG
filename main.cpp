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
    Database database;
    Headset headset;
    MusicPlayer musicPlayer;
    MainW w(&database);

    w.setVolumeSlider(musicPlayer.getAudioOutputPtr());


    QObject::connect(&w, SIGNAL(startRecording(QString,QString,QString)),
                     &headset, SLOT(initialise(QString,QString,QString)));
    QObject::connect(&w, SIGNAL(logEmoState()),
                     &headset, SLOT(logEmoState()));
    QObject::connect(&w, SIGNAL(stopRecording()),
                     &headset, SLOT(writeData()));
    QObject::connect(&w, SIGNAL(cancelRecording()),
                     &headset, SLOT(discardData()));

    QObject::connect(&headset, SIGNAL(newUserTrack(QString,QString,QString,QList< QList<float> >)),
                     &database, SLOT(saveUserTrack(QString,QString,QString,QList< QList<float> >)));

    QObject::connect(&w, SIGNAL(newSourceList(QList<Phonon::MediaSource>)),
                     &musicPlayer, SLOT(getMetaData(QList<Phonon::MediaSource>)));
    QObject::connect(&musicPlayer, SIGNAL(newMetaData(QList<QStringList>)),
                     &w, SLOT(updateTable(QList<QStringList>)));
    QObject::connect(&w, SIGNAL(startPlaying(Phonon::MediaSource)),
                     &musicPlayer, SLOT(startPlaying(Phonon::MediaSource)));
    QObject::connect(&w, SIGNAL(stopPlaying()),
                     &musicPlayer, SLOT(stopPlaying()));
    QObject::connect(&musicPlayer, SIGNAL(trackFinished()),
                     &w, SLOT(trackFinished()));
    QObject::connect(&musicPlayer, SIGNAL(tick(qint64)),
                     &w, SLOT(tick(qint64)));

    w.show();
    return a.exec();
}










