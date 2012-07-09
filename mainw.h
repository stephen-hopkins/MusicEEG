#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QTGui>
#include "headset.h"
#include <QTimer>
#include <phonon/volumeslider.h>
#include <phonon/MediaSource>
#include <QList>

namespace Ui {
class MainW;
}

class MainW : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainW(QWidget *parent = 0);
    ~MainW();
    
private slots:

    void startButtonPressed();
    void stopButtonPressed();
    void startTrack();

    // music player
    void addFiles();
    void about();
    void tableClicked(int row, int column);
    void updateTable(QList<QStringList>);

public slots:
    void tick(qint64 time);
    void trackFinished();

signals:
    // handles headset/database
    void startRecording(QString, QString, QString);
    void logEmoState();
    void stopRecording();
    void cancelRecording();

    // goes to mediaplayer
    void startPlaying(Phonon::MediaSource);
    void stopPlaying();
    void newSourceList(QList<Phonon::MediaSource>);

private:
    Ui::MainW *ui;

    //  used to logEmoState every second for headset
    QTimer* recurringTimer;
    QTimer* singleShotTimer;

    // track being played
    int currentTrack;

    // used for mediaplayer
    Phonon::SeekSlider *seekSlider;
    QList<Phonon::MediaSource> sources;

    void setupActions();

};

#endif // MAINW_H
