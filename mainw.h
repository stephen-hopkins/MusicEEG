#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include <QTGui>
#include "headset.h"
#include <QTimer>
#include <phonon/audiooutput.h>
#include <phonon/seekslider.h>
#include <phonon/mediaobject.h>
#include <phonon/volumeslider.h>
#include <phonon/backendcapabilities.h>
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
    // these 2 are for 2 buttons, will be got rid of
    void on_startButton_clicked();
    void on_stopButton_clicked();

    // handling headset
    void trackStarted();
    void trackFinished();

    // music player
    void addFiles();
    void about();
    void stateChanged(Phonon::State newState, Phonon::State oldState);
    void tick(qint64 time);
    void sourceChanged(const Phonon::MediaSource &source);
    void metaStateChanged(Phonon::State newState, Phonon::State oldState);
    void aboutToFinish();
    void tableClicked(int row, int column);

signals:
    // handles headset/database
    void startRecording(QString, QString, QString);
    void logEmoState();
    void stopRecording();

private:
    Ui::MainW *ui;

    //  used for headset
    QTimer* headsetTimer;
    QString user;
    QString artist;
    QString track;

    // used for mediaplayer
    Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    QList<Phonon::MediaSource> sources;

    void setupActions();

};

#endif // MAINW_H
