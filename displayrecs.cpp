#include "displayrecs.h"
#include "ui_displayrecs.h"

DisplayRecs::DisplayRecs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayRecs)
{
    ui->setupUi(this);
}

DisplayRecs::~DisplayRecs()
{
    delete ui;
}

void DisplayRecs::updateTable(QMultiMap<float, QStringList> recs)
{
    ui->tableWidget->clearContents();
    QList<float> scores = recs.keys();
    int currentRow = 0;

    while (!scores.isEmpty()) {

        float currentScore = scores.takeLast();
        QList<QStringList> uATs = recs.values(currentScore);

        while (!uATs.isEmpty()) {
            ui->tableWidget->insertRow(currentRow);
            QStringList uAT = uATs.takeFirst();
            for (int n = 0 ; n < 4 ; n++) {
                QString text;
                if (n != 3) {
                    text = uAT[n];
                }
                else {
                    text = QString::number(currentScore);
                }
                QTableWidgetItem* newItem = new QTableWidgetItem(text);
                newItem->setFlags(Qt::ItemIsSelectable);
                ui->tableWidget->setItem(currentRow, n, newItem);
            }
            currentRow++;
        }

    }
    this->show();
}

void DisplayRecs::updateTable(QMultiMap<int, QStringList> recs)
{
    ui->tableWidget->clearContents();
    QList<int> scores = recs.keys();
    int currentRow = 0;

    while (!scores.isEmpty()) {

        int currentScore = scores.takeLast();
        QList<QStringList> uATs = recs.values(currentScore);

        while (!uATs.isEmpty()) {
            ui->tableWidget->insertRow(currentRow);
            QStringList uAT = uATs.takeFirst();
            for (int n = 0 ; n < 4 ; n++) {
                QString text;
                if (n != 3) {
                    text = uAT[n];
                }
                else {
                    text = QString::number(currentScore);
                }
                QTableWidgetItem* newItem = new QTableWidgetItem(text);
                newItem->setFlags(Qt::ItemIsSelectable);
                ui->tableWidget->setItem(currentRow, n, newItem);
            }
            currentRow++;
        }

    }
    this->show();
}

