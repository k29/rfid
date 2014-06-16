#ifndef RFID_H
#define RFID_H

#include <QMainWindow>
#include "tag.h"
#include "serial.h"
#include <QComboBox>
#include <QStackedWidget>
#include <QStackedLayout>

namespace Ui {
class rfid;
}

class rfid : public QMainWindow
{
    Q_OBJECT

public:
    explicit rfid(QWidget *parent = 0);
    ~rfid();

private:
    Ui::rfid *ui;

    Tag_Actions tag;
    
    //flags
    bool init_flag;
    bool install_flag;
    bool use_flag;
    byte serialNumber[4];
    byte type;

    void initial_scanRfid();
    void port_open();
    void periodic_scanRfid();

};

#endif // RFID_H
