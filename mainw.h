#ifndef MAINW_H
#define MAINW_H

#include <QMainWindow>
#include "headset.h"
#include <QTimer>

namespace Ui {
class MainW;
}

class MainW : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainW(QWidget *parent = 0);
    ~MainW();
    
private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();

signals:
    void startRecording(QString, QString, QString);
    void logEmoState();
    void stopRecording();

private:
    Ui::MainW *ui;
    QTimer headsetTimer;

};

#endif // MAINW_H
