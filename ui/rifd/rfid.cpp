#include "rfid.h"
#include "ui_rfid.h"
#include "login.h"
#include <QTimer>
#include <QDebug>

rfid::rfid(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rfid)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    //init all flags to zero
    init_flag=false;
    install_flag=false;
    use_flag=false;
    ui->stackedWidget->setCurrentWidget(ui->Scan);
}

rfid::~rfid()
{
    delete ui;
}

void rfid::port_open()
{
    if(!tag.serial.Open("/dev/ttySAC1",19200))
    {
        qDebug()<<"Port could not open\n";
        msgBox.setText("Error, Port could not open...try again!!");
        msgBox.exec();
    }
    else
        qDebug()<<"port open!!!";
}


void rfid::periodic_scanRfid()  //to be done once the therapy starts!!
{
    timer=new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(5000); //checking every 5 secs whether the same card is attached or not.
}

void rfid::update()
{
    tag.control_rf_transmit(true);
    tag.select_mifare_card();
    for(int i=0;i<4;i++)
        if(serialNumber[i]==tag.packet_received[i+5])
            qDebug()<<"card same!!";
        else
        {
            qDebug()<<"card not same, exiting";
            msgBox.setText("Error, card not same!!Exiting");
        }

}

void rfid::onControlRFtransmit_clicked()
{
    port_open();
    tag.control_rf_transmit(true);
    if(!tag.packet_received[4]==0x00)
    {
        ui->label_scanStatus->setText("RF Transmit successfull");
        qDebug()<<" control tranmit error!! try again";
    }
    else
    {
        qDebug()<<"control RF transmit true set!!";
        ui->pushButton_selectMifare->setEnabled(true);
        ui->label_scanStatus->setText("RF Transmit successfull");
     }

}


void rfid::onSelectMifare_clicked()
{
    tag.select_mifare_card();
    if(!tag.packet_received[4]==0x00)
    {
        ui->label_scanStatus->setText("Tag Reading Failed!! ");
        qDebug()<<"Select mifare card error, try again";
        ui->label_serial->setEnabled(false);
        ui->label_type->setEnabled(false);
        ui->lineEdit_serial->setEnabled(false);
        ui->lineEdit_type->setEnabled(false);
    }
    else
    {
        qDebug()<<"comminication with card successfull";
        ui->label_serial->setEnabled(true);
        ui->label_type->setEnabled(true);
        ui->lineEdit_serial->setEnabled(true);
        ui->lineEdit_type->setEnabled(true);
        ui->label_scanStatus->setText("comminication with card successfull");
     }

    QString serial_string;
    for(int i=0;i<4;i++)
    {
        serialNumber[i]=tag.packet_received[i+5];
        serial_string.append(serialNumber[i]);
    }

    ui->lineEdit_serial->setText(serial_string);

    type=tag.packet_received[9];
    if(type==0x00)
    {
       qDebug()<<"Mifare Standard 1K(S50) card\n";
       ui->lineEdit_type->setText("Mifare Standard 1K(S50) card");
    }
    else if(type==0x01)
    {
        qDebug()<<"Mifare Standard 4K(S70) card\n";
        ui->lineEdit_type->setText("Mifare Standard 4K(S70) card");
    }
    else if(type==0x02)
    {
       qDebug()<<"Mifare ProX card\n";
       ui->lineEdit_type->setText("Mifare ProX card");
    }
    ui->pushButton_options->setEnabled(true);
}


void rfid::onOptions_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->Options);
}


void rfid::onBack_clicked()
{
    this->close();
    login l;
    l.enter_three();
}
