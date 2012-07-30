#ifndef DISPLAYRECS_H
#define DISPLAYRECS_H

#include <QDialog>
#include <QMultiMap>

namespace Ui {
class DisplayRecs;
}

class DisplayRecs : public QDialog
{
    Q_OBJECT
    
public:
    explicit DisplayRecs(QWidget *parent = 0);
    ~DisplayRecs();

public slots:
    void updateTable(QMultiMap<int, QStringList> recs);
    void updateTable(QMultiMap<float, QStringList> recs);
    
private:
    Ui::DisplayRecs *ui;
};

#endif // DISPLAYRECS_H
