#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QTGui>
#include "headset.h"
#include <QTimer>
#include <phonon/volumeslider.h>
#include <phonon/MediaSource>
#include <QList>
#include "database.h"
#include "musicplayer.h"
#include "displayemotion.h"

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
    void userSelectionMade(QString userSelection);

    // music player
    void addFiles();
    void about();
    void tableClicked(int row, int column);
    void updateTable(QList<QStringList>);

public slots:
    void tick(qint64 time);
    void trackFinished();
    void setVolumeSlider(Phonon::AudioOutput*);

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

    QTimer* headsetTimer;
    QTimer* delayTrackPlayTimer;
    Phonon::VolumeSlider *volumeSlider;

    int currentTrack;
    QList<Phonon::MediaSource> sources;
    QString user;

    Database* db;
    Headset* headset;
    MusicPlayer* musicPlayer;
    DisplayEmotion* displayEmotion;

    void setupActions();
    void setupUsers();
    void connectSignalsSlots();

};

#endif // MAINW_H
