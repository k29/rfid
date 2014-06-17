#ifndef RFID_H
#define RFID_H

#include <QMainWindow>
#include "tag.h"
#include "serial.h"
#include "ui_rfid.h"

#include <QComboBox>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QMessageBox>

namespace Ui {
class rfid;
}

class rfid : public QMainWindow
{
    Q_OBJECT

public:
    explicit rfid(QWidget *parent = 0);
    ~rfid();

    Tag_Actions tag;

private:
    Ui::rfid *ui;

    QTimer *timer;
    QMessageBox msgBox;
    QStackedWidget stackedWidget;

    //flags
    bool init_flag;
    bool install_flag;
    bool use_flag;
    byte serialNumber[4];
    byte type;

    void initial_scanRfid();
    void port_open();
    void periodic_scanRfid();
    void update();
    void choose_options();

private slots:
    void onControlRFtransmit_clicked();
    void onSelectMifare_clicked();
    void onOptions_clicked();
    void onBack_clicked();

};

#endif // RFID_H
