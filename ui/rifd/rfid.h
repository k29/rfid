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
    //flags
    bool init_flag;
    bool install_flag;
    bool use_flag;

};

#endif // RFID_H
