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
/**********************************************************************
    Resetting all the variables and keys here, since this is the only way to access the rfid verification,
***********************************************************************/

    //FLAGS
    init_flag=false;
    install_flag=false;
    use_flag=false;

    //KEYS
    transport_config[0]=0xff;
    transport_config[1]=0x07;
    transport_config[2]=0x80;
    transport_config[3]=0x29;
    for(int i=0;i<6;i++)
    {
        default_key_A[i]=0xff;
        default_key_B[i]=0xff;
    }
 
    for(int i=0;i<sizeof(new_key_A);i++)
    {
        new_key_A[i]=0xff;
    }

    //SERIAL NUMBER
    for(int i=0;i<sizeof(serialNumber[i]);i++)
    {
        serialNumber[i]=0xff;
    }
}

rfid::~rfid()
{
    delete ui;
}

bool rfid::port_open()
{
    if(!tag.serial.Open("/dev/ttySAC1",19200))
    {
        qDebug()<<"\nPort could not open\n";
        return false;
    }
    else
    {
        qDebug()<<"\nPort open!!!";
        return true;
    }
}


bool rfid::init()
{

}

int rfid::communication_init()
{
    /********************************************
                    PORT OPEN
    *********************************************/
    qDebug()<<"\n\n\n\nIn Port Open";
    if(!port_open())
        return 1;


    /********************************************
                CONTROL RF TRANSMIT
    *********************************************/
    qDebug()<<"\n\n\n\nIN control RF transmit";
    tag.control_rf_transmit(true);
    if(!tag.packet_received[4]==0x00)
    {
        qDebug()<<"Control RF tranmit error!!";
        return 2;
    }
    qDebug()<<"Control RF transmit true set!!";


    /********************************************
                SELECT MIFARE CARD
    *********************************************/
    qDebug()<<"\n\n\n\nIn select Mifare Card";
    tag.select_mifare_card();
    if(!tag.packet_received[4]==0x00)
    {
        qDebug()<<"Select mifare card error";
        return 3;
    }
    qDebug()<<"Comminication with card successfull";
       
    //Storing information from the select_mifare_card
    for(int i=0;i<4;i++)
    {
        serialNumber[i]=tag.packet_received[i+5];
        qDebug()<<serialNumber[i];
        printf("%x\t",serialNumber[i]);
    }

    type=tag.packet_received[9];
    if(type==0x00)
       qDebug()<<"Mifare Standard 1K(S50) card\n";
    else if(type==0x01)
        qDebug()<<"Mifare Standard 4K(S70) card\n";
    else if(type==0x02)
       qDebug()<<"Mifare ProX card\n";

   return 4; //success
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

int rfid::start_rfid_verification()
{
    /********************************************
                COMMUNICATION INIT
    *********************************************/
    int communication_init_status=communication_init();
    if(communication_init_status==1)
        return 1;
    if(communication_init_status==2)
        return 2;
    if(communication_init_status==3)
        return 3;
    qDebug()<<"Comminication init successfull";

   /********************************************
                CHOOSING OPTIONS
    *********************************************/
    qDebug()<<"\n\n\n\nIn choosing options";

    //Using Default keys
    if(tag.read_data_block(0,1,default_key_A))
    {
        qDebug()<<"Reading using default keys successfull, hence setting the Init active";
        init();
    }
    qDebug()<<"Failed to read using Default keys(Device Init done), using new keys";

    communication_init_status=communication_init();
    if(communication_init_status==1)
    return 1;
    if(communication_init_status==2)
        return 2;
    if(communication_init_status==3)
        return 3;
    qDebug()<<"Comminication init successfull again";

    get_encrypt_key();
    bool correct_init_flag=false;
    if(tag.read_data_block(0,1,new_key_A))
    {
        qDebug()<<"Read using new keys!! Success!!Verfying block 1 and 2 format!!";
        if(tag.packet_received[8]==0x00 && tag.packet_received[9]==0x02 && tag.packet_received[10]==0x09
                    && tag.packet_received[14]=='y' && tag.packet_received[18]=='n')
        {
            if(tag.read_data_block(0,2,new_key_A))
            {
                for(int i=0;i<4;i++)
                    if(tag.packet_received[i+5]==serialNumber[i])
                    {
                        correct_init_flag=true;
                    }
                    else
                        return 7; //serial number does not match
            }
            else
                return 6; //reading from block 2 failed using new keys
        }
        else
            return 5; //pattern of block 1 does not match

    }
    else
        return 4; //read using new keys failed

    if(correct_init_flag)
    {
        if(tag.read_data_block(0,1,new_key_A))
        {
            if(tag.packet_received[7]==true) //is use flag true
            {
                qDebug()<<"ready to use...";
                update_use_condition();
                return 8;
            }
            else
                return 9;
        }
        else
            return 4;
    }

}


bool rfid::init()
{
    //BLOCK 1: flag and some special format
    //BLOCK 2: Serial Number
    //BLOCK 3: Init date and expiry
    //BLOCK 4: No of uses

    /*************************************************************************************************
    SECTOR TRAILER for sector 0,1,2,3.... 
    new_key_A(a function of the serial number),access_conditions(transport),new_key_B(useless)
    **************************************************************************************************/
    qDebug()<<"\n\n\n\nIn Init..."
    get_encrypt_key();
    byte new_key_B[]={0xff,0xff,0xff,0xff,0xff,0xff};
    byte data_write_sectorTrailer[16];
    for(int i=0;i<6;i++)
        data_write_sectorTrailer[i]=new_key_A[i];  //new key A

    for(int i=6;i<10;i++)
        data_write_sectorTrailer[i]=transport_config[i-6];  //Transport Config
 
    for(int i=10;i<16;i++)
       data_write_sectorTrailer[i]=new_key_B[i-10];  //new key B

    qDebug()<<"data write sector trailer: \n"<<data_write_sectorTrailer;

    qDebug()<<"writing to sector trailer 0";
    if(tag.write_data_block(0,3,default_key_A,data_write_sectorTrailer))
        qDebug()<<"Write, sector trailer 0: SUCCESS";
    else
    {
        qDebug()<<"Write, sector trailer 0: FAIL";
        return false;
    }
    
    qDebug()<<"writing to sector trailer 1";
    if(tag.write_data_block(0,7,default_key_A,data_write_sectorTrailer))
        qDebug()<<"Write, sector trailer 1: SUCCESS";
    else
    {
        qDebug()<<"Write, sector trailer 1: FAIL";
        return false;
    }
    
    qDebug()<<"writing to sector trailer 2";
    if(tag.write_data_block(0,11,default_key_A,data_write_sectorTrailer))
        qDebug()<<"Write, sector trailer 2: SUCCESS";
    else
    {
        qDebug()<<"Write, sector trailer 2: FAIL";
        return false;
    }
    
    qDebug()<<"writing to sector trailer 3";
    if(tag.write_data_block(0,15,default_key_A,data_write_sectorTrailer))
        qDebug()<<"Write, sector trailer 3: SUCCESS";
    else
    {
        qDebug()<<"Write, sector trailer 3: FAIL";
        return false;
    }
    
    qDebug()<<"writing to sector trailer 4";
    if(tag.write_data_block(0,19,default_key_A,data_write_sectorTrailer))
        qDebug()<<"Write, sector trailer 4: SUCCESS";
    else
    {
        qDebug()<<"Write, sector trailer 4: FAIL";
        return false;
    }
    

    /********************************************************************
    BLOCK 1:
    init_flag, install_flag, use_flag,0x00,0x02,0x09,Grey orange...
    ********************************************************************/


    qDebug()<<"Writing to Block 1";
    init_flag=true;
    install_flag=false;
    use_flag=true; //the device is ready to use.... and the no of uses value is set to 1.
    byte data_write_block1[]={init_flag,install_flag,use_flag,0x00,0x02,0x09,'g','r','e','y','o','r','a','n','g','e'};
    if(tag.write_data_block(0,1,new_key_A,data_write_block1))
        qDebug()<<"Write, Block 1: SUCCESS";
    else
    {
        qDebug()<<"Write, Block 1: FAIL";
        return false;
    }
    

    /********************************************************************
    BLOCK 2:
    Serial Number
    ********************************************************************/

    qDebug()<<"Writing to block 2";
    byte *data_write_block2=serialNumber;
    if(tag.write_data_block(0,2,new_key_A,data_write_block2))
        qDebug()<<"Write, Block 2: SUCCESS";
    else
    {
        qDebug()<<"Write, Block 2: FAIL";
        return false;
    }


    /********************************************************************
    BLOCK 4:
    Expiry date
    ********************************************************************/
    




    /********************************************************************
    BLOCK 5:
    No of uses
    ********************************************************************/
    qDebug()<<"Writing to block 5";
    byte data_write_block5[]={0x01}; //no of uses initialised to 1
    if(tag.write_data_block(0,5,new_key_A,data_write_block5))
        qDebug()<<"Write, Block 5: SUCCESS";
    else
    {
        qDebug()<<"Write, Block 5: FAIL";
        return false;
    }

    return true;

}

void rfid::update_use_condition()
{

}