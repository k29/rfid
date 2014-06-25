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

#define no_of_use 10



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
    byte new_key_A[6];
    byte rev;

    void initial_scanRfid();
    void port_open();
    void periodic_scanRfid();
    void update();
    void choose_options();
    void get_encrypt_key();
    byte reverse(byte);
    void variable_reset();

    byte transport_config[6];
    byte default_key_A[6];
    byte default_key_B[6];

private slots:
    void onControlRFtransmit_clicked();
    void onSelectMifare_clicked();
    void onOptions_clicked();
    void onBack_clicked();
    void onInit_clicked();
    void onInstall_clicked();
    void onUse_clicked();
    void onRead_clicked();


};

#endif // RFID_H
