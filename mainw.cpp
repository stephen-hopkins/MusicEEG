#include "mainw.h"
#include "ui_mainw.h"
#include "meexception.h"
#include <iostream>

MainW::MainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainW)
{
    ui->setupUi(this);
    user = "Steve";

    headsetTimer = new QTimer();
    connect(headsetTimer, SIGNAL(timeout()),
                     this, SIGNAL(logEmoState()));



    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);

    mediaObject->setTickInterval(1000);

    connect(mediaObject, SIGNAL(tick(qint64)),
            this, SLOT(tick(qint64)));
    connect(mediaObject, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(stateChanged(Phonon::State,Phonon::State)));
    connect(metaInformationResolver, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SLOT(metaStateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject, SIGNAL(currentSourceChanged(Phonon::MediaSource)),
            this, SLOT(sourceChanged(Phonon::MediaSource)));
    connect(mediaObject, SIGNAL(aboutToFinish()),
            this, SLOT(aboutToFinish()));
    connect(mediaObject, SIGNAL(finished()),
            this, SLOT(trackFinished()));

    Phonon::createPath(mediaObject, audioOutput);

    setupActions();
    ui->timeLcd->display("00:00");

}

MainW::~MainW()
{
    delete ui;
    delete headsetTimer;
    delete audioOutput;
    delete mediaObject;
    delete metaInformationResolver;
}


void MainW::on_startButton_clicked()
{
     emit startRecording("Steve", "The XX", "Hello");
     emit logEmoState();
     headsetTimer->start(1000);
     return;
}

void MainW::on_stopButton_clicked()
{
    headsetTimer->stop();
    emit stopRecording();
}

void MainW::addFiles()
 {
     QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),
         QDesktopServices::storageLocation(QDesktopServices::MusicLocation));

     if (files.isEmpty())
         return;

     int index = sources.size();
     foreach (QString string, files) {
             Phonon::MediaSource source(string);

         sources.append(source);
     }
     if (!sources.isEmpty())
         metaInformationResolver->setCurrentSource(sources.at(index));

 }

void MainW::about()
 {
     QMessageBox::information(this, tr("About Music Player"),
         tr("The Music Player example shows how to use Phonon - the multimedia"
            " framework that comes with Qt - to create a simple music player."));
 }

void MainW::stateChanged(Phonon::State newState, Phonon::State /* oldState */)
 {
     switch (newState) {
         case Phonon::ErrorState:
             if (mediaObject->errorType() == Phonon::FatalError) {
                 QMessageBox::warning(this, tr("Fatal Error"),
                 mediaObject->errorString());
             } else {
                 QMessageBox::warning(this, tr("Error"),
                 mediaObject->errorString());
             }
             break;
         case Phonon::PlayingState:
                 ui->actionPlay->setEnabled(false);
                 ui->actionStop->setEnabled(true);
                 break;
         case Phonon::StoppedState:
                 ui->actionStop->setEnabled(false);
                 ui->actionPlay->setEnabled(true);
                 ui->timeLcd->display("00:00");
                 break;
         case Phonon::BufferingState:
                 break;
         default:
             ;
     }
 }

void MainW::tick(qint64 time)
 {
     QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
     ui->timeLcd->display(displayTime.toString("mm:ss"));
 }

void MainW::tableClicked(int row, int /* column */)
 {
     bool wasPlaying = mediaObject->state() == Phonon::PlayingState;

     mediaObject->stop();
     mediaObject->clearQueue();

     if (row >= sources.size())
         return;

     mediaObject->setCurrentSource(sources[row]);

     if (wasPlaying)
         mediaObject->play();
     else
         mediaObject->stop();
 }

void MainW::sourceChanged(const Phonon::MediaSource &source)
 {
     ui->musicTable->selectRow(sources.indexOf(source));
     ui->timeLcd->display("00:00");
 }

void MainW::metaStateChanged(Phonon::State newState, Phonon::State /* oldState */)
 {
     if (newState == Phonon::ErrorState) {
         QMessageBox::warning(this, tr("Error opening files"),
             metaInformationResolver->errorString());
         while (!sources.isEmpty() &&
                !(sources.takeLast() == metaInformationResolver->currentSource())) {}  /* loop */;
         return;
     }

     if (newState != Phonon::StoppedState && newState != Phonon::PausedState)
         return;

     if (metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
             return;

     QMap<QString, QString> metaData = metaInformationResolver->metaData();

     QString title = metaData.value("TITLE");
     if (title == "")
         title = metaInformationResolver->currentSource().fileName();

     QTableWidgetItem *titleItem = new QTableWidgetItem(title);
     titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
     QTableWidgetItem *artistItem = new QTableWidgetItem(metaData.value("ARTIST"));
     artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
     QTableWidgetItem *albumItem = new QTableWidgetItem(metaData.value("ALBUM"));
     albumItem->setFlags(albumItem->flags() ^ Qt::ItemIsEditable);
     QTableWidgetItem *yearItem = new QTableWidgetItem(metaData.value("DATE"));
     yearItem->setFlags(yearItem->flags() ^ Qt::ItemIsEditable);

     track = title;
     artist = artistItem->text();

     int currentRow = ui->musicTable->rowCount();
     ui->musicTable->insertRow(currentRow);
     ui->musicTable->setItem(currentRow, 0, titleItem);
     ui->musicTable->setItem(currentRow, 1, artistItem);
     ui->musicTable->setItem(currentRow, 2, albumItem);
     ui->musicTable->setItem(currentRow, 3, yearItem);

     if (ui->musicTable->selectedItems().isEmpty()) {
         ui->musicTable->selectRow(0);
         mediaObject->setCurrentSource(metaInformationResolver->currentSource());
     }

     Phonon::MediaSource source = metaInformationResolver->currentSource();
     int index = sources.indexOf(metaInformationResolver->currentSource()) + 1;
     if (sources.size() > index) {
         metaInformationResolver->setCurrentSource(sources.at(index));
     }
     else {
         ui->musicTable->resizeColumnsToContents();
         if (ui->musicTable->columnWidth(0) > 300)
             ui->musicTable->setColumnWidth(0, 300);
     }
 }

void MainW::aboutToFinish()
 {
     int index = sources.indexOf(mediaObject->currentSource()) + 1;
     if (sources.size() > index) {
         mediaObject->enqueue(sources.at(index));
     }
 }

void MainW::setupActions()
 {
    ui->actionPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->actionStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->actionNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));

    connect(ui->actionPlay, SIGNAL(triggered()), mediaObject, SLOT(play()));
    connect(ui->actionPlay, SIGNAL(triggered()), this, SLOT(trackStarted()));
    connect(ui->actionStop, SIGNAL(triggered()), mediaObject, SLOT(stop()));
    connect(ui->actionAddFiles, SIGNAL(triggered()), this, SLOT(addFiles()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(ui->actionAboutQT, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    QStringList headers;
    headers << tr("Title") << tr("Artist") << tr("Album") << tr("Year");
    ui->musicTable->setHorizontalHeaderLabels(headers);
    connect(ui->musicTable, SIGNAL(cellPressed(int,int)),
            this, SLOT(tableClicked(int,int)));

     ui->volumeSlider->setAudioOutput(audioOutput);
 }

void MainW::trackStarted()
{
    emit startRecording(user, artist, track);
    emit logEmoState();
    headsetTimer->start(1000);
    return;
}

void MainW::trackFinished()
{
    headsetTimer->stop();
    emit stopRecording();
}
