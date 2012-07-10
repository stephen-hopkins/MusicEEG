#include "mainw.h"
#include "ui_mainw.h"
#include <iostream>

using namespace std;

MainW::MainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainW)
{
    ui->setupUi(this);
    user = "Steve";

    headsetTimer = new QTimer();
    delayTrackPlayTimer = new QTimer();
    delayTrackPlayTimer->setSingleShot(true);
    connect(headsetTimer, SIGNAL(timeout()),
                     this, SIGNAL(logEmoState()));
    connect(delayTrackPlayTimer, SIGNAL(timeout()),
            this, SLOT(startTrack()));

    setupActions();
    ui->timeLcd->display("00:00");
    currentTrack = 0;
}

MainW::~MainW()
{
    delete ui;
    delete headsetTimer;
    delete delayTrackPlayTimer;
}

void MainW::addFiles()
 {
     QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),
         QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

     if (files.isEmpty())
         return;

     foreach (QString string, files) {
             Phonon::MediaSource source(string);

         sources.append(source);
     }
     if (!sources.isEmpty())
         emit newSourceList(sources);
     currentTrack = 0;
 }

void MainW::about()
 {
     QMessageBox::information(this, tr("About Music Player"),
         tr("The Music Player example shows how to use Phonon - the multimedia"
            " framework that comes with Qt - to create a simple music player."));
 }

void MainW::tick(qint64 time)
 {
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
     ui->timeLcd->display(displayTime.toString("mm:ss"));
 }

void MainW::tableClicked(int row, int /* column */)
 {
     emit stopPlaying();
     emit cancelRecording();

     if (row >= sources.size())
         return;

     ui->musicTable->selectRow(row);
     ui->timeLcd->display("00:00");
     currentTrack = row;
     emit startButtonPressed();
 }

void MainW::setupActions()
 {
    ui->actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->actionNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    connect(ui->actionPlay, SIGNAL(triggered()),
            this, SLOT(startButtonPressed()));
    connect(ui->actionStop, SIGNAL(triggered()),
            this, SLOT(stopButtonPressed()));
    connect(ui->actionAddFiles, SIGNAL(triggered()),
            this, SLOT(addFiles()));
    connect(ui->actionExit, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()),
            this, SLOT(about()));
    connect(ui->actionAboutQT, SIGNAL(triggered()),
            qApp, SLOT(aboutQt()));
    connect(ui->musicTable, SIGNAL(cellPressed(int,int)),
            this, SLOT(tableClicked(int,int)));
     return;
 }

void MainW::startButtonPressed()
{
    QString artist = ui->musicTable->itemAt(currentTrack,0)->text();
    QString track = ui->musicTable->itemAt(currentTrack,1)->text();
    emit startRecording(user, artist, track);
    delayTrackPlayTimer->start(5000);
}

void MainW::startTrack()
{
    emit logEmoState();
    emit startPlaying(sources[currentTrack]);
    headsetTimer->start(1000);
}

void MainW::trackFinished()
{
    headsetTimer->stop();
    emit stopRecording();
    if (currentTrack < sources.size()) {
        currentTrack++;
        emit startButtonPressed();
    }
}

void MainW::stopButtonPressed()
{
    headsetTimer->stop();
    emit cancelRecording();
    emit stopPlaying();
}

void MainW::updateTable(QList<QStringList> metaData)
{   
    if (metaData[0].isEmpty()) {
        cout << "Error: metaData is empty";
    }

    QTableWidgetItem *artistItem = new QTableWidgetItem(metaData[0].takeFirst());
    artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *titleItem = new QTableWidgetItem(metaData[1].takeFirst());
    titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *albumItem = new QTableWidgetItem(metaData[2].takeFirst());
    albumItem->setFlags(albumItem->flags() ^ Qt::ItemIsEditable);
    QTableWidgetItem *yearItem = new QTableWidgetItem(metaData[3].takeFirst());
    yearItem->setFlags(yearItem->flags() ^ Qt::ItemIsEditable);

    int currentRow = ui->musicTable->rowCount();
    ui->musicTable->insertRow(currentRow);
    ui->musicTable->setItem(currentRow, 0, artistItem);
    ui->musicTable->setItem(currentRow, 1, titleItem);
    ui->musicTable->setItem(currentRow, 2, albumItem);
    ui->musicTable->setItem(currentRow, 3, yearItem);

    ui->musicTable->resizeColumnsToContents();
    if (ui->musicTable->columnWidth(0) > 300)
        ui->musicTable->setColumnWidth(0, 300);
    return;
}

void MainW::setVolumeSlider(Phonon::AudioOutput* audio)
{
    ui->volumeSlider->setAudioOutput(audio);
}
