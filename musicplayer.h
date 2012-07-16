#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#define TAGLIB_STATIC

#include <phonon/audiooutput.h>
#include <phonon/mediaobject.h>
#include <phonon/backendcapabilities.h>
#include <QList>
#include <QObject>
#include <QMessageBox>

#include "taglib/tag.h"
#include "taglib/fileref.h"

class MusicPlayer : public QObject
{
    Q_OBJECT
public:
    MusicPlayer();
    ~MusicPlayer();
    Phonon::AudioOutput* getAudioOutputPtr();   

public slots:
    void startPlaying(Phonon::MediaSource);
    void stopPlaying();
    void getMetaData(QList<Phonon::MediaSource>);

private slots:
    void handleNewMetadata();

signals:
    void newMetaData(QList<QStringList>);
    void trackFinished();
    void tick(qint64);

private:
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;

    QList<QStringList> processedMetaData;
    QList<Phonon::MediaSource> sources;

    void addNextFile();
};

#endif // MUSICPLAYER_H
