#include "mainw.h"
#include "ui_mainw.h"

MainW::MainW(Headset* h, QWidget *parent) :
    headset(h), QMainWindow(parent),
    ui(new Ui::MainW)
{
    ui->setupUi(this);
}

MainW::~MainW()
{
    delete ui;
}


void MainW::on_startButton_clicked()
{
    headset->setUserArtistTrack("Steve", "The XX", "Hello");
    headset->start();
}

void MainW::on_stopButton_clicked()
{
    headset->trackFinished();
}
