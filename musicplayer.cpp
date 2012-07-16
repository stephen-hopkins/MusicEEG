#include "musicplayer.h"
#include <QMessageBox>
#include <iostream>

using namespace std;

MusicPlayer::MusicPlayer()
{
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);

    connect(mediaObject, SIGNAL(tick(qint64)),
            this, SIGNAL(tick(qint64)));
    connect(mediaObject, SIGNAL(finished()),
            this, SIGNAL(trackFinished()));
    connect(metaInformationResolver, SIGNAL(metaDataChanged()),
            this, SLOT(handleNewMetadata()));

    Phonon::createPath(mediaObject, audioOutput);
    QStringList emptyQStringList;
    for (int i = 0 ; i < 4 ; i++) {
        processedMetaData.insert(i, emptyQStringList);
    }


}

MusicPlayer::~MusicPlayer()
{
    delete audioOutput;
    delete mediaObject;
    delete metaInformationResolver;
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

void MusicPlayer::getMetaData(QList<Phonon::MediaSource> inputSources)
{
    for (int i = 0 ; i < 4 ; i++) {
        processedMetaData[i].clear();
    }
    if (inputSources.isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Error: No files selected");
        msgBox.exec();
    }

    sources = inputSources;

    TagLib::FileRef f(sources[0].fileName().toStdString().c_str());
    TagLib::Tag* tag = f.tag();
    QString title = tag->title().toCString();
    QString artist = tag->artist().toCString();
    QString album = tag->album().toCString();
    QString year = tag->year();



    addNextFile();
}

void MusicPlayer::addNextFile()
{
    if (sources.isEmpty()) {
        return;
    }
    else {
        metaInformationResolver->setCurrentSource(sources.last());
        return;
    }
}

void MusicPlayer::handleNewMetadata()
{
    QMap<QString, QString> rawMetaData = metaInformationResolver->metaData();

    QString title = rawMetaData.value("TITLE");

    if (title == "") {
        title = metaInformationResolver->currentSource().fileName();
    }

    processedMetaData[0].append(rawMetaData.value("ARTIST"));
    processedMetaData[1].append(title);
    processedMetaData[2].append(rawMetaData.value("ALBUM"));
    processedMetaData[3].append(rawMetaData.value("DATE"));
    sources.removeLast();
    if (sources.isEmpty()) {
        emit newMetaData(processedMetaData);
    }
    else {
        addNextFile();
    }
}

Phonon::AudioOutput* MusicPlayer::getAudioOutputPtr()
{
    return audioOutput;
}


