#include "musicplayer.h"
#include <QMessageBox>
#include <iostream>

using namespace std;

MusicPlayer::MusicPlayer()
{
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);

    connect(mediaObject, SIGNAL(tick(qint64)),
            this, SIGNAL(tick(qint64)));
    connect(mediaObject, SIGNAL(finished()),
            this, SIGNAL(trackFinished()));

    Phonon::createPath(mediaObject, audioOutput);
}

MusicPlayer::~MusicPlayer()
{
    delete audioOutput;
    delete mediaObject;
}

void MusicPlayer::startPlaying(Phonon::MediaSource track)
{
    mediaObject->enqueue(track);
    mediaObject->play();
}


void MusicPlayer::stopPlaying()
{
    mediaObject->stop();
}

QList<QStringList> MusicPlayer::getMetaData(QList<Phonon::MediaSource> sources)
{
    QList<QStringList> metaData;

    while (!sources.isEmpty()) {

        TagLib::FileRef f(sources.takeFirst().fileName().toStdString().c_str());
        TagLib::Tag* tag = f.tag();
        QString title = tag->title().toCString();
        QString artist = tag->artist().toCString();
        QString album = tag->album().toCString();
        QString year = QString("%1").arg(tag->year());

        QStringList thisMetadata;
        thisMetadata << artist << title << album << year;
        metaData.append(thisMetadata);
    }
    return metaData;
}

Phonon::AudioOutput* MusicPlayer::getAudioOutputPtr()
{
    return audioOutput;
}


