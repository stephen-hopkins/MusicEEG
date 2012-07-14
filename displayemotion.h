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
    void setRawData(QList< QList<float> > rawEmoData);
    void setAverages(QList<float>);
    void setChanges(QList<float>);

public slots:
    void updateWindow(QString user, QString artist, QString track, QList< QList<float> > rawData, QList<float> averages, QList<float> changes);
    
private:
    Ui::DisplayEmotion *ui;
};

#endif // DISPLAYEMOTION_H
