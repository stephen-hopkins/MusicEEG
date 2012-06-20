#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include "headset.h"

namespace Ui {
class MainW;
}

class MainW : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainW(Headset*, QWidget *parent = 0);
    ~MainW();
    

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

private:
    Ui::MainW *ui;
    Headset* headset;
};

#endif // MAINW_H
