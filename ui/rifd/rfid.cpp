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
    	cout<<"Port could not open\n";
    	return 1;
  	}
  	cout<<"Port open\n";
	initial_scanRfid(); // does the initial scan in a while loop and then calls a funciton which scans in a timer.
}

void rfid::initial_scanRfid()
{
	while(1)
	{
		tag.control_rf_transmit(true);
		tag.select_mifare_card();
		
	}
}