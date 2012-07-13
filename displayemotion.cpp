#include "displayemotion.h"
#include "ui_displayemotion.h"

displayEmotion::displayEmotion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::displayEmotion)
{
    ui->setupUi(this);
}

displayEmotion::~displayEmotion()
{
    delete ui;
}
