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
#include "recommender.h"
#include "displayrecs.h"

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
    void userSelectionMade(QString userSelection);

    // music player
    void addFiles();
    void about();
    void tableClicked(int row, int column);

public slots:
    void tick(qint64 time);
    void trackFinished();
    void setVolumeSlider(Phonon::AudioOutput*);
    void continuePlaying();
    void skipTrack();
    void showRecords();
    void showRecs();

signals:
    // handles headset/database
    void startRecording(QString, QString, QString);
    void logEmoState();
    void stopRecording();
    void cancelRecording();
    void newUser(QString);

    // goes to mediaplayer
    void startPlaying(Phonon::MediaSource);
    void stopPlaying();

private:
    Ui::MainW *ui;

    QTimer* headsetTimer;
    Phonon::VolumeSlider* volumeSlider;

    int currentTrack;
    QList<Phonon::MediaSource> sources;
    QString user;
    bool recordingMode;

    Database* db;
    Headset* headset;
    MusicPlayer* musicPlayer;
    DisplayEmotion* displayEmotion;
    Recommender* recommender;
    DisplayRecs* displayRecs;

    void setupActions();
    void setupComboBox();
    void connectSignalsSlots();
    void updateTable(QList<QStringList>);
    void setupMusicTable();
    void startupGetUser();

};

#endif // MAINW_H
