#ifndef HEADSET_H
#define HEADSET_H

#include <QObject>
#include "edk.h"
#include <QList>
#include <QString>

class Headset : public QObject
{
    Q_OBJECT
public:
    Headset();
    ~Headset();

public slots:
    void initialise(QString u, QString a, QString t);
    void logEmoState();
    void writeData();
    void discardData();

signals:
    void newUserTrack(QString user, QString artist, QString track, QList< QList<float> > rawEmoData, QList< QList<float> > stats);


private:
    QString user;
    QString artist;
    QString track;
    QList<float> engagement;
    QList<float> excitement;
    QList<float> frustration;
    QList<float> meditation;

    EmoEngineEventHandle event;
    EmoStateHandle emoState;
    unsigned int userID;

    bool trackPlaying;
    void logEmoState(EmoStateHandle);
    float calcAverage(QList<float> input);

};

#endif // HEADSET_H
