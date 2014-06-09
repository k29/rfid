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
}

rfid::~rfid()
{
    delete ui;
}
