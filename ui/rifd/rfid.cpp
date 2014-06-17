#include "rfid.h"
#include "ui_rfid.h"
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
    //port_open();
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
        printf("port could not open!!");
        msgBox.setText("Error, Port could not open...trying again!!");
        msgBox.exec();
    }
    else
    {
        qDebug()<<"Port open\n";
        printf("port open!!");
        initial_scanRfid(); // does the initial scan in a while loop and then calls a funciton which scans in a timer.
    }
}

void rfid::initial_scanRfid()
{
    while(1)
    {
        tag.control_rf_transmit(true);
        if(!tag.packet_received[4]==0x00)
        {
            msgBox.setText("Error, Control Transmit error. Try Again");
            msgBox.exec();
            qDebug()<<" control tranmit error!! try again";
            continue;
        }

        tag.select_mifare_card();
        for(int i=0;i<4;i++)
            serialNumber[i]=tag.packet_received[i+5];

        type=tag.packet_received[9];
        if(type==0x00)
        {
           qDebug()<<"Mifare Standard 1K(S50) card\n";
           break;
        }
        else if(type==0x01)
        {
            qDebug()<<"Mifare Standard 4K(S70) card\n";
            break;
        }
        else if(type==0x02)
        {
           qDebug()<<"Mifare ProX card\n";
           break;
        }
    }
    periodic_scanRfid();

    qDebug()<<"going to options widget!!";
    ui->stackedWidget->setCurrentWidget(ui->Options);
    choose_options();
}

void rfid::periodic_scanRfid()
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

void rfid::choose_options()
{
    qDebug()<<"yay!!";
    this->close();
    msgBox.setText("yay!! done reached final stage!!");
    //if(init_flag==false && install_flag==false && use_flag)
}


void rfid::onControlRFtransmit_clicked()
{

}


void rfid::onSelectMifare_clicked()
{

}
