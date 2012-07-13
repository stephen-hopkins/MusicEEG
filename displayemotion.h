#ifndef DISPLAYEMOTION_H
#define DISPLAYEMOTION_H

#include <QDialog>

namespace Ui {
class displayEmotion;
}

class displayEmotion : public QDialog
{
    Q_OBJECT
    
public:
    explicit displayEmotion(QWidget *parent = 0);
    ~displayEmotion();
    void setRawData(QList< QList<float> > rawEmoData);
    void setAverages(QList<float>);
    void setChanges(QList<float>);
    
private:
    Ui::displayEmotion *ui;
};

#endif // DISPLAYEMOTION_H
