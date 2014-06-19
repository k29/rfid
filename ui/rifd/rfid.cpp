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

    //init the keys
    transport_config[0]=0xff;
    transport_config[1]=0x07;
    transport_config[2]=0x80;
    transport_config[3]=0x29;
    for(int i=0;i<6;i++)
    {
        default_key_A[i]=0xff;
        default_key_B[i]=0xff;
    }
    variable_reset();
}
rfid::~rfid()
{
    delete ui;
}


void rfid::variable_reset()
{
    for(int i=0;i<sizeof(new_key_A);i++)
    {
        new_key_A[i]=0xff;
    }
    for(int i=0;i<sizeof(serialNumber[i]);i++)
    {
        serialNumber[i]=0xff;
    }


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
        qDebug()<<serialNumber[i];
        printf("%x\t",serialNumber[i]);
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
    choose_options();
}


void rfid::onBack_clicked()
{
    this->close();
    login l;
    l.enter_three();
}


void rfid::choose_options()
{

}

byte rfid::reverse(byte num)
{
    rev=0x00;
    byte digit=0x00;
    while(num!=0x00)
    {
        digit=num%0x10;
        rev=rev*0x10+digit;
        num=num/0x10;
    }
    return rev;
}

void rfid::get_encrypt_key()
{
    for(int i=0;i<4;i++)
        new_key_A[i]=reverse(serialNumber[i]);
    new_key_A[4]=serialNumber[0]+serialNumber[2];
    new_key_A[5]=serialNumber[1]+serialNumber[3];
    for(int i=0;i<6;i++)
        qDebug()<<"new_key_A"<<new_key_A[i];

}

void rfid::onInit_clicked()
{
    /////////////////////////////////////
    /* defining the sector trailer for sector 0,1,2,3.... new_key_A(a function of the serial number),access_conditions(transport),new_key_B(useless)*/
    ////////////////////////////////////


    get_encrypt_key();
    qDebug()<<"new key A: "<<new_key_A;  // 0xe9, 0x08, 0x86, 0xae, 0x06, 0x6a (for the blue tag)
    byte new_key_B[]={0xff,0xff,0xff,0xff,0xff,0xff};
    byte data_write_sectorTrailer[16];
    for(int i=0;i<6;i++)
        data_write_sectorTrailer[i]=new_key_A[i];
    data_write_sectorTrailer[6]=0xff;
    data_write_sectorTrailer[7]=0x07;
    data_write_sectorTrailer[8]=0x80;
    data_write_sectorTrailer[9]=0x29;
    for(int i=10;i<16;i++)
        data_write_sectorTrailer[i]=new_key_B[i-10];
    qDebug()<<data_write_sectorTrailer;
    ui->textEdit->setText("writing data...\n");

    qDebug()<<"writing to sector trailer 0";
    if(tag.write_data_block(0,3,default_key_A,data_write_sectorTrailer))
    {
        qDebug()<<"Write, sector trailer 0: SUCCESS";
        ui->textEdit->append("Write, sector trailer 0: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, sector trailer 0: FAIL";
        ui->textEdit->append("Write, sector trailer 0: FAIL\n");
    }

    qDebug()<<"writing to sector trailer 1";
    if(tag.write_data_block(0,7,default_key_A,data_write_sectorTrailer))
    {
        qDebug()<<"Write, sector trailer 1: SUCCESS";
        ui->textEdit->append("Write, sector trailer 1: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, sector trailer 1: FAIL";
        ui->textEdit->append("Write, sector trailer 1: FAIL\n");
    }

    qDebug()<<"writing to sector trailer 2";
    if(tag.write_data_block(0,11,default_key_A,data_write_sectorTrailer))
    {
        qDebug()<<"Write, sector trailer 2: SUCCESS";
        ui->textEdit->append("Write, sector trailer 2: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, sector trailer 2: FAIL";
        ui->textEdit->append("Write, sector trailer 2: FAIL\n");
    }

    qDebug()<<"writing to sector trailer 3";
    if(tag.write_data_block(0,15,default_key_A,data_write_sectorTrailer))
    {
        qDebug()<<"Write, sector trailer 3: SUCCESS";
        ui->textEdit->append("Write, sector trailer 3: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, sector trailer 3: FAIL";
        ui->textEdit->append("Write, sector trailer 3: FAIL\n");
    }

    qDebug()<<"writing to sector trailer 4";
    if(tag.write_data_block(0,19,default_key_A,data_write_sectorTrailer))
    {
        qDebug()<<"Write, sector trailer 4: SUCCESS";
        ui->textEdit->append("Write, sector trailer 4: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, sector trailer 4: FAIL";
        ui->textEdit->append("Write, sector trailer 4: FAIL\n");
    }



    //////////////////////////////////////
    /*Storing Flags in block 1..... init_flag, install_flag, use_flag,0x00,0x02,0x09,Grey orange...*/
    /////////////////////////////////////



    qDebug()<<"writing to Block 1";
    byte data_write_block1[]={true,false,false,0x00,0x02,0x09,'g','r','e','y','o','r','a','n','g','e'};
    if(tag.write_data_block(0,1,new_key_A,data_write_block1))
    {
        qDebug()<<"Write, Block 1: SUCCESS";
        ui->textEdit->append("Write, Block 1: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, Block 1: FAIL";
        ui->textEdit->append("Write, Block 1: FAIL\n");
    }



    /////////////////////////////////////
    /*Storing serial key in block 2*/
    ////////////////////////////////////


    qDebug()<<"writing to block 2";
    byte *data_write_block2=serialNumber;
    if(tag.write_data_block(0,2,new_key_A,data_write_block2))
    {
        qDebug()<<"Write, Block 2: SUCCESS";
        ui->textEdit->append("Write, Block 2: SUCCESS\n");
    }
    else
    {
        qDebug()<<"Write, Block 2: FAIL";
        ui->textEdit->append("Write, Block 2: FAIL\n");
    }
}

void rfid::onRead_clicked()
{
    get_encrypt_key();
    ui->stackedWidget->setCurrentWidget(ui->Read);

    qDebug()<<"Reading form Block 1";
    if(tag.read_data_block(0,1,new_key_A))
        qDebug()<<"Read, Block 1: SUCCESS";
    else
        qDebug()<<"Read, Block 1: FAIL";

    qDebug()<<"Reading form Block 2";
    if(tag.read_data_block(0,2,new_key_A))
        qDebug()<<"Read, Block 2: SUCCESS";
    else
        qDebug()<<"Read, Block 2: FAIL";

    qDebug()<<"Reading form Block 3";
    if(tag.read_data_block(0,3,new_key_A))
        qDebug()<<"Read, Block 3: SUCCESS";
    else
        qDebug()<<"Read, Block 3: FAIL";

    qDebug()<<"Reading form Block 4";
    if(tag.read_data_block(0,4,new_key_A))
        qDebug()<<"Read, Block 4: SUCCESS";
    else
        qDebug()<<"Read, Block 4: FAIL";

    qDebug()<<"Reading form Block 5";
    if(tag.read_data_block(0,5,new_key_A))
        qDebug()<<"Read, Block 5: SUCCESS";
    else
        qDebug()<<"Read, Block 5: FAIL";

    qDebug()<<"Reading form Block 6";
    if(tag.read_data_block(0,6,new_key_A))
        qDebug()<<"Read, Block 6: SUCCESS";
    else
        qDebug()<<"Read, Block 6: FAIL";


}
