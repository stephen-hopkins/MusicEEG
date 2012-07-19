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
    QList<QStringList> getMetaData(QList<Phonon::MediaSource>);


public slots:
    void startPlaying(Phonon::MediaSource);
    void stopPlaying();
    void handleErrors(Phonon::State newstate);

signals:
    void trackFinished();
    void tick(qint64);
    void errorPlayingTrack();

private:
    Phonon::MediaObject* mediaObject;
    Phonon::AudioOutput* audioOutput;

};

#endif // MUSICPLAYER_H
