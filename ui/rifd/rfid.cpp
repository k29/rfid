#include "rfid.h"
#include "ui_rfid.h"
#include "login.h"
#include <QTimer>
#include <QDebug>
#include <QFont>

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

    QFont font;
    font.setPixelSize(35);

    ui->label->setFont(font);
    ui->label_scanStatus->setFont(font);
    ui->label_serial->setFont(font);
    ui->label_type->setFont(font);

    ui->pushButton->setFont(font);
    ui->pushButton_controlRFtransmit->setFont(font);
    ui->pushButton_init->setFont(font);
    ui->pushButton_install->setFont(font);
    ui->pushButton_options->setFont(font);
    ui->pushButton_read->setFont(font);
    ui->pushButton_reuse->setFont(font);
    ui->pushButton_selectMifare->setFont(font);

    ui->lineEdit->setFont(font);
    ui->lineEdit_serial->setFont(font);
    ui->label_type->setFont(font);

    ui->textEdit->setFont(font);
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
        qDebug()<<" control tranmit error!! try again"; //addd message box here for the actual pop up
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
    //use default keys to read block 1 and read the flag, is false then init active.
    //else control transmit true and select mifare card and then use new keys to activate either initialise ot reuse.



    /***************Using default keys****************/
    if(tag.read_data_block(0,1,default_key_A))
    {
        if(tag.packet_received[0]!=true && tag.packet_received[3]!=0x00 &&
                tag.packet_received[4]!=0x02 && tag.packet_received[5]!=0x09) //if not initialsed
        {
            ui->pushButton_init->setEnabled(true);
            ui->pushButton_install->setEnabled(false);
            ui->pushButton_reuse->setEnabled(false);
        }
        else
        {
            qDebug()<<"Error: keys not set but block 1 is correct!!";
        }
    }



    /********************resetting***********************/
    tag.control_rf_transmit(true);
    tag.select_mifare_card();


    /*******************Using new_key_A******************/
    
    //the following ensures that the format of the first two blocks are same as init!!
    bool correct_init_flag=false;
    bool correct_serial=true;
    if(tag.read_data_block(0,1,new_key_A))
    {
        if(tag.packet_received[3]==0x00 && tag.packet_received[4]==0x02 && tag.packet_received[5]==0x09
                && tag.packet_received[9]=='y' && tag.packet_received[13]=='n')
        {
            if(tag.read_data_block(0,2,new_key_A))
            {
                for(int i=0;i<4;i++)
                    if(!tag.packet_received[i]==serialNumber[i])
                    {
                        correct_serial=false;
                        break;
                    }
                    else
                        continue;
                if(correct_serial==true)
                    correct_init_flag=true;
            }
        }
    } 
    //if correct_init_flag is true it ensures that the two blocks are all cool!! 
    //now bothering about enabling the correct push button
    
    //following code bothers about that:
    //note that by default all are disabled!!
    if(tag.read_data_block(0,1,new_key_A))
    {
        if(tag.packet_received[0]==false)
        {
            ui->pushButton_init->setEnabled(true);
            ui->pushButton_install->setEnabled(false);
            ui->pushButton_reuse->setEnabled(false);
        }
        if(tag.packet_received[0]==true && tag.packet_received[1]==false && tag.packet_received[2]==false)
        {
            ui->pushButton_init->setEnabled(false);
            ui->pushButton_install->setEnabled(true);
            ui->pushButton_reuse->setEnabled(false);
        }
        if(tag.packet_received[0]==true && tag.packet_received[1]==true && tag.packet_received[2]==true)
        {
            ui->pushButton_init->setEnabled(false);
            ui->pushButton_install->setEnabled(false);
            ui->pushButton_reuse->setEnabled(true);
        }
    }
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
    //write flag and format in Block 1.
    //write the serial number in Block 2.
    //write init date, and valid after init date in block 4.
    
    
    
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
    init_flag=true;
    install_flag=false;
    use_flag=flag;
    byte data_write_block1[]={init_flag,install_flag,use_flag,0x00,0x02,0x09,'g','r','e','y','o','r','a','n','g','e'};
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

    //TODO: WRITE THE INIT DATE AND INIT EXPIRY DATE
}


void rfid::onInstall_clicked()
{
    //reading of the serial and genration of the new key is done in choose_options()
    //authenticate the sector 1 and 2 using the new key and check if the format matches and that the serial is correct.

    bool correct_init_flag=false;
    bool correct_serial=true;
    if(tag.read_data_block(0,1,new_key_A))
    {
        if(tag.packet_received[3]==0x00 && tag.packet_received[4]==0x02 && tag.packet_received[5]==0x09
                && tag.packet_received[9]=='y' && tag.packet_received[13]=='n')
        {
            if(tag.read_data_block(0,2,new_key_A))
            {
                for(int i=0;i<4;i++)
                    if(!tag.packet_received[i]==serialNumber[i])
                    {
                        correct_serial=false;
                        break;
                    }
                    else
                        continue;
                if(correct_serial==true)
                    correct_init_flag=true;
            }
        }
    }
    //if correct_init_flag is true it ensures that the two blocks are all cool!!


    //write the install machine info. into the tag:

    //block 5 install date
    QDate dateInstall;
    byte *data_write_block5;
    data_write_block5=new byte;
    data_write_block5=&dateInstall;
    if(tag.write_data_block(0,5,new_key_A,data_write_block5))
    {
        qDebug()<<"Write, Block 5: SUCCESS";
        ui->textEdit->append("Write, Block 5: SUCCESS\n");
    }

    //block 6 no_of_use initialised to 0, to be incremented once the therapy button is clicked.
    byte data_write_block6[16];
    data_write_block6[0]=0

    //install flag and use flag is set to true:










    //set use flag as true


}



void rfid::onUse_clicked()
{
    
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
