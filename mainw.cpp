#include "mainw.h"
#include "ui_mainw.h"
#include <iostream>
#include <QInputDialog>
#include <QSqlQuery>

using namespace std;

MainW::MainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainW)
{
    ui->setupUi(this);

    db = new Database();
    headset = new Headset();
    musicPlayer = new MusicPlayer();
    displayEmotion = new DisplayEmotion();
    recommender = new Recommender(db);

    user = "NO USER SELECTED";
    currentTrack = 0;
    recordingMode = true;

    headsetTimer = new QTimer();

    setupActions();
    setupComboBox();
    connectSignalsSlots();

    ui->timeLcd->display("00:00");
    setVolumeSlider(musicPlayer->getAudioOutputPtr());
}

void MainW::connectSignalsSlots()
{
    connect(headsetTimer, SIGNAL(timeout()),
            this, SIGNAL(logEmoState()));

    connect(this, SIGNAL(startRecording(QString,QString,QString)),
            headset, SLOT(initialise(QString,QString,QString)));
    connect(this, SIGNAL(logEmoState()),
            headset, SLOT(logEmoState()));
    connect(this, SIGNAL(stopRecording()),
            headset, SLOT(writeData()));
    connect(this, SIGNAL(cancelRecording()),
            headset, SLOT(discardData()));

    connect(headset, SIGNAL(newUserTrack(QString,QString,QString,QList< QList<float> >, QList< QList<float> >)),
            db, SLOT(saveUserTrack(QString,QString,QString,QList< QList<float> >, QList< QList<float> >)));
    connect(headset, SIGNAL(newUserTrack(QString,QString,QString,QList<QList<float> >,QList< QList<float> >)),
            displayEmotion, SLOT(updateWindow(QString,QString,QString,QList<QList<float> >,QList< QList<float> >)));

    connect(this, SIGNAL(startPlaying(Phonon::MediaSource)),
            musicPlayer, SLOT(startPlaying(Phonon::MediaSource)));
    connect(this, SIGNAL(stopPlaying()),
            musicPlayer, SLOT(stopPlaying()));
    connect(musicPlayer, SIGNAL(trackFinished()),
            this, SLOT(trackFinished()));
    connect(musicPlayer, SIGNAL(tick(qint64)),
            this, SLOT(tick(qint64)));
    connect(musicPlayer, SIGNAL(errorPlayingTrack()),
            this, SLOT(skipTrack()));

    connect(displayEmotion, SIGNAL(pressedOK()),
            this, SLOT(continuePlaying()));

}


MainW::~MainW()
{
    delete ui;
    delete headsetTimer;
    delete db;
    delete headset;
    delete musicPlayer;
    delete displayEmotion;
    delete recommender;
}

void MainW::setupComboBox()
{
    if (recordingMode == true) {
        QStringList users = db->getUsers();
        users.append("Add New User");
        ui->comboBox->insertItems(0, users);
        user = ui->comboBox->currentText();
    }
    else {
        QStringList recOptions;
        recOptions << "Own Cont" << "Other Cont" << "Own Disc" << "Other Disc";
        ui->comboBox->insertItems(0, recOptions);
    }
}

void MainW::addFiles()
 {
    if (recordingMode == false) {
        recordingMode = true;
        setupMusicTable();
    }
    sources.clear();
    QStringList files = QFileDialog::getOpenFileNames(this, tr("Select Music Files"),
                                                      QDesktopServices::storageLocation(QDesktopServices::MusicLocation), "MP3 Files (*.mp3)");

    if (files.isEmpty())
        return;

    foreach (QString string, files) {
        Phonon::MediaSource source(string);
        sources.append(source);
    }
    currentTrack = 0;
    updateTable(musicPlayer->getMetaData(sources));
    ui->actionPlay->setEnabled(true);
    ui->actionStop->setEnabled(false);
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
    if (recordingMode == true) {
     emit stopPlaying();
     emit cancelRecording();

     if (row >= sources.size())
         return;

     currentTrack = row;
     startButtonPressed();
    }
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
    connect(ui->actionShowRecords, SIGNAL(triggered()),
            this, SLOT(showRecords()));
     return;
 }

void MainW::startButtonPressed()
{
    QString artist = ui->musicTable->item(currentTrack,0)->text();
    QString track = ui->musicTable->item(currentTrack,1)->text();

    ui->musicTable->selectRow(currentTrack);
    ui->timeLcd->display("00:00");
    ui->actionPlay->setEnabled(false);
    ui->actionPlay->setEnabled(true);

    emit startRecording(user, artist, track);
    emit logEmoState();
    emit startPlaying(sources[currentTrack]);
    headsetTimer->start(1000);
}

void MainW::trackFinished()
{
    headsetTimer->stop();
    emit stopRecording();
}

void MainW::continuePlaying()
{
    if (currentTrack + 1 < sources.size()) {
        currentTrack++;
        startButtonPressed();
    }
}

void MainW::stopButtonPressed()
{
    headsetTimer->stop();
    ui->actionPlay->setEnabled(true);
    ui->actionStop->setEnabled(false);
    emit cancelRecording();
    emit stopPlaying();
}

void MainW::updateTable(QList<QStringList> metaData)
{   
    // clear current table
    while (ui->musicTable->rowCount() != 0) {
        ui->musicTable->removeRow(0);
    }

    while (!metaData.isEmpty()) {

        QStringList thisMetadata = metaData.takeFirst();

        QTableWidgetItem *artistItem = new QTableWidgetItem(thisMetadata[0]);
        artistItem->setFlags(artistItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *titleItem = new QTableWidgetItem(thisMetadata[1]);
        titleItem->setFlags(titleItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *albumItem = new QTableWidgetItem(thisMetadata[2]);
        albumItem->setFlags(albumItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *yearItem = new QTableWidgetItem(thisMetadata[3]);
        yearItem->setFlags(yearItem->flags() ^ Qt::ItemIsEditable);

        int currentRow = ui->musicTable->rowCount();
        ui->musicTable->insertRow(currentRow);
        ui->musicTable->setItem(currentRow, 0, artistItem);
        ui->musicTable->setItem(currentRow, 1, titleItem);
        ui->musicTable->setItem(currentRow, 2, albumItem);
        ui->musicTable->setItem(currentRow, 3, yearItem);

    }

    ui->musicTable->resizeColumnsToContents();

    if (ui->musicTable->columnWidth(0) > 300) {
        ui->musicTable->setColumnWidth(0, 300);
    }
}

void MainW::userSelectionMade(QString userSelection)
{
    if (userSelection == "Add New User") {
        QStringList users = db->getUsers();
        bool validUsernameChosen = false;

        while (!validUsernameChosen) {
            bool ok;
            QString newUser = QInputDialog::getText(this, "New User", "Enter desired username:", QLineEdit::Normal, QDir::home().dirName(), &ok);
            if (ok && !newUser.isEmpty() && !users.contains(newUser, Qt::CaseInsensitive)) {
                user = newUser;
                db->saveNewUser(newUser);
                int itemsPre = ui->comboBox->count();
                ui->comboBox->removeItem(itemsPre - 1);
                ui->comboBox->addItem(newUser);
                ui->comboBox->addItem("Add New User");
                ui->comboBox->setCurrentIndex(itemsPre - 1);
                validUsernameChosen = true;
            }
        }
    }
    else {
        user = userSelection;
    }
}

void MainW::setVolumeSlider(Phonon::AudioOutput* audio)
{
    ui->volumeSlider->setAudioOutput(audio);
}

void MainW::skipTrack()
{
    stopButtonPressed();
    QMessageBox msgBox;
    QString text = QString("Cannot play file: %1").arg(sources[currentTrack].fileName());
    msgBox.setText(text);
    msgBox.exec();
    continuePlaying();
}

void MainW::showRecords()
{
    recordingMode = false;
    setupMusicTable();
    ui->actionPlay->setEnabled(false);
    ui->actionPlay->setEnabled(false);

    QSqlQuery userTracks = db->getAllRecords();
    if (!userTracks.isActive()) {
        QMessageBox msgBox;
        msgBox.setText("Fatal Error : Cannot retrieve records from database");
        msgBox.exec();
    }

    int row = 0;
    while (userTracks.next()) {
        ui->musicTable->insertRow(row);
        for (int n = 1 ; n < 16 ; n++) {
            QTableWidgetItem* newItem = new QTableWidgetItem(userTracks.value(n).toString());
            newItem->setFlags(Qt::ItemIsSelectable);
            ui->musicTable->setItem(row, n-1, newItem);
        }
        row++;
    }
}

void MainW::setupMusicTable() {

    ui->musicTable->clear();
    QStringList headers;

    if (recordingMode == true) {
        ui->musicTable->setColumnCount(4);
        ui->musicTable->setRowCount(0);
        headers << "Artist" << "Track" << "Album" << "Year";

    }
    else {
        ui->musicTable->setColumnCount(15);
        ui->musicTable->setRowCount(0);
        headers << "User" << "Artist" << "Track" << "MeanEng" << "MeanExc" << "MeanFrus" << "MeanMed";
        headers << "ChaEng" << "ChaExc" << "ChaFrus" << "ChaMed" << "SDEng" << "SDExc" << "SDFrus" << "SDMed";
    }

    ui->musicTable->setHorizontalHeaderLabels(headers);
}

