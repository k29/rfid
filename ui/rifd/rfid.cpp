#include "rfid.h"
#include "ui_rfid.h"

rfid::rfid(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rfid)
{
    ui->setupUi(this);

    //init all flags to zero
    init_flag=false;
    install_flag=false;
    use_flag=false;

    initial_scanRfid(); // does the initial scan in a while loopa and then calls a funciton which scans in a timer.
}

rfid::~rfid()
{
    delete ui;
}
