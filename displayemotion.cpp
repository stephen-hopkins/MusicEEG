#include "displayemotion.h"
#include "ui_displayemotion.h"

#include <iostream>


DisplayEmotion::DisplayEmotion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayEmotion)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()),
            this, SIGNAL(pressedOK()));

    ui->chart->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(9);
    ui->chart->legend->setFont(legendFont);
    ui->chart->legend->setPositionStyle(QCPLegend::psBottomRight);
    ui->chart->legend->setBrush(QBrush(QColor(255,255,255,230)));

    ui->chart->addGraph();
    ui->chart->graph(0)->setPen(QPen(Qt::blue));
    ui->chart->graph(0)->setName("Engagement");
    ui->chart->addGraph();
    ui->chart->graph(1)->setPen(QPen(Qt::red));
    ui->chart->graph(1)->setName("Excitement");
    ui->chart->addGraph();
    ui->chart->graph(2)->setPen(QPen(Qt::green));
    ui->chart->graph(2)->setName("Frustration");
    ui->chart->addGraph();
    ui->chart->graph(3)->setPen(QPen(Qt::yellow));
    ui->chart->graph(3)->setName("Meditation");
}

DisplayEmotion::~DisplayEmotion()
{
    delete ui;
}

void DisplayEmotion::updateWindow(QString user, QString artist, QString track, QList< QList<float> > rawData, QList<float> averages, QList<float> changes)
{

    // Update chart
    QVector<float> engagementF = rawData[0].toVector();
    QVector<float> excitementF = rawData[1].toVector();
    QVector<float> frustrationF = rawData[2].toVector();
    QVector<float> meditationF = rawData[3].toVector();

    int noDataPoints = engagementF.size();

    QVector<double> engagement(noDataPoints);
    QVector<double> excitement(noDataPoints);
    QVector<double> frustration(noDataPoints);
    QVector<double> meditation(noDataPoints);

    for (int n = 0 ; n < noDataPoints ; n++) {
        engagement[n] = engagementF[n];
        excitement[n] = excitementF[n];
        frustration[n] = frustrationF[n];
        meditation[n] = meditationF[n];
    }

    QVector<double> x(noDataPoints);
    for (int n = 0 ; n < noDataPoints ; n++) {
        x[n] = n;
    }
    ui->chart->graph(0)->setData(x, engagement);
    ui->chart->graph(1)->setData(x, excitement);
    ui->chart->graph(2)->setData(x, frustration);
    ui->chart->graph(3)->setData(x, meditation);
    ui->chart->rescaleAxes();

    QString title = QString("User: %1 Track: %2 by %3" ).arg(user, track, artist);
    ui->chart->setTitle(title);
    ui->chart->replot();

    // Update table
    for (int n = 0 ; n < 4 ; n++) {
        QTableWidgetItem* ave = new QTableWidgetItem(QString::number(averages[n]));
        QTableWidgetItem* cha = new QTableWidgetItem(QString::number(changes[n]));
        ui->summaryTable->setItem(n, 0, ave);
        ui->summaryTable->setItem(n, 1, cha);
    }
    QString filename = QString("%1%2%3.jpg").arg(user, artist, track);
    filename.remove(" ");
    ui->chart->saveJpg(filename);

    this->show();
}



void DisplayEmotion::on_okButton_clicked()
{
    this->hide();
    emit pressedOK();
}
