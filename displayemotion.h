#ifndef DISPLAYEMOTION_H
#define DISPLAYEMOTION_H

#include <QDialog>

namespace Ui {
class DisplayEmotion;
}

class DisplayEmotion : public QDialog
{
    Q_OBJECT
    
public:
    explicit DisplayEmotion(QWidget *parent = 0);
    ~DisplayEmotion();

public slots:
    void updateWindow(QString user, QString artist, QString track, QList< QList<float> > rawData, QList< QList<float> > stats);

signals:
    void pressedOK();

private:
    Ui::DisplayEmotion *ui;
};

#endif // DISPLAYEMOTION_H
