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

    port_open();
}

rfid::~rfid()
{
    delete ui;
}

void rfid::port_open()
{
	Tag_Actions tag;
  	if(!tag.serial.Open("/dev/ttySAC1",19200))
  	{
    	qDebug()<<"Port could not open\n";
    	return 1;
  	}
  	qDebug()<<"Port open\n";
	initial_scanRfid(); // does the initial scan in a while loop and then calls a funciton which scans in a timer.
}

void rfid::initial_scanRfid()
{
	while(1)
	{
		tag.control_rf_transmit(true);
        if(!tag.packet_received[4]==0x00)
        {
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
}

void rfid::periodic_scanRfid()
{
    
}
