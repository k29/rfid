//TODO: Add to the main timer the check of verification of serial number every 10 secs.
//TODO: Add the time expiry


#ifndef RFID_H
#define RFID_H

#include <QMainWindow>
#include <QComboBox>
#include <QStackedWidget>
#include <QStackedLayout>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <QDateTime>

#include "tag.h"
#include "serial.h"
#include "ui_rfid.h"

#define NO_OF_USES 2
#define EXPIRY_DATE 3 //in months


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
    QDateTime dt;

    //flags
    bool init_flag;
    bool install_flag;
    bool use_flag;

    byte serialNumber[4];
    byte type;
    byte new_key_A[6];
    byte rev;

    bool port_open();
    void get_encrypt_key();
    byte reverse(byte);
    void variable_reset();
    int communication_init();
    bool update_use_condition();
    bool init();

    byte transport_config[6];
    byte default_key_A[6];
    byte default_key_B[6];



public:
    int start_rfid_verification();
};

#endif // RFID_H






