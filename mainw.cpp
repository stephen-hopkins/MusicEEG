#include "mainw.h"
#include "ui_mainw.h"

MainW::MainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainW)
{
    ui->setupUi(this);
    headset = new Headset;
}

MainW::~MainW()
{
    delete ui;
    delete headset;
}


void MainW::on_startButton_clicked()
{
    headset->start();
}

void MainW::on_stopButton_clicked()
{
    headset->trackFinished();
}
