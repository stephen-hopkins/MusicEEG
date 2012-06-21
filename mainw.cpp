#include "mainw.h"
#include "ui_mainw.h"
#include "meexception.h"
#include <iostream>

MainW::MainW(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainW)
{
    ui->setupUi(this);
    QObject::connect(&headsetTimer, SIGNAL(timeout()),
                     this, SIGNAL(logEmoState()));
}

MainW::~MainW()
{
    delete ui;
}


void MainW::on_startButton_clicked()
{
     emit startRecording("Steve", "The XX", "Hello");
     emit logEmoState();
     headsetTimer.start(1000);
     return;
}

void MainW::on_stopButton_clicked()
{
    headsetTimer.stop();
    emit stopRecording();
}


