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

namespace Ui {
class MainW;
}

class MainW : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainW(Database* db, QWidget *parent = 0);
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

    //  used to logEmoState every second for headset
    QTimer* headsetTimer;

    // used to provide 5 secon delay before starting to play track
    QTimer* delayTrackPlayTimer;

    // used for mediaplayer
    Phonon::VolumeSlider *volumeSlider;

    int currentTrack;
    QList<Phonon::MediaSource> sources;

    QString user;
    Database* db;

    void setupActions();
    void setupUsers();

};

#endif // MAINW_H
