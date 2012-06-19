#ifndef HEADSET_H
#define HEADSET_H

#include <QThread>
#include "edk.h"
#include <QList>
#include <QString>

class Headset : public QThread
{

public:
    Headset();
    ~Headset();
    void run();
    void trackFinished();
    void setUserArtistTrack(QString, QString, QString);

private:
    QString user;
    QString artist;
    QString track;
    QList<float> engagement;
    QList<float> excitementST;
    QList<float> excitementLT;
    QList<float> frustration;
    QList<float> meditation;

    bool trackPlaying;
    void logEmoState(EmoStateHandle);

};

#endif // HEADSET_H
