



/*

sec 1 - blocks 4,5,6.(7th for keys)        ----      encrypt(serial)
sec 2 - blocks 8,9,10.(11th for keys)     ----      no_of_use, machine serial.
sec 3 -	blocks 12,13,14.(15th for keys)		    manufacturing_date,date_of_last_use.
sec 4 - blocks 16,17,18(19th for keys)    ----      installation_date,unusable_date
sec 5 - blocks 20,21,22(23th for keys)    ----     machine name, pateint 1st name , 2nd name.
sec 6 - blocks 24,25,26(27th for keys)    ----     doc 1st name, 2nd name.


-INITIALIZE (Datt factory)
read serial
write keyA1(Sec1), keyA2(Sec2), keyA3(Sec3),keyA4(Sec4),keyA5(sec5),keyA6(sec6).
block 4 (encrypt(serial)==same)
block 8 (initialize value block to 100)
block 12,13(1st 8 bits)(manufacturing date and time)


-INSTALL IN MACHINE(also check every min.)
read serial
using keyA1, verify & encrypt(serial)==read.block(4)
block 16,17(1st 8 bits)  (installation date and time)
block 20(machine name -> write)
block 9,10(machine serial -> write)
save installed serial no. of tag to machine
if Correct, Proceed and Write(null) and change ACCESS BIT for sector1 => Block(4)


-USE IN MACHINE AND RESUSE
verify installed serial==bottle serial
(if not true , try INSTALL IN MACHINE-step)
if not there(write block 21 and 22 patient name ---- 24 and 25 doctor name.)
else verify doc and patient name.
verify block8 value >0
date_of_use(keep overwriting)-block13(last 8bytes),14.
if not, go to unusable.
else decrement value in block 8.



-unusable
if block8<=0
write time stamp - block17(last 8bytes),block 18.
make all access bits to read only.
also if 100% trigger.


*/

// sending packets for 1.on/off  2.auto read serial no.   3.read/write data


#include<stdio.h>
#include<stdlib.h>
#include<ftdi.h>
#include <unistd.h>
#include<inttypes.h>
#include<time.h>
#include<math.h>

#define key_type_used 0		// 0=key type A  and 1=key type B //
#define block_used 11		// block to be accessed //



/*
sec 1 - blocks 4,5,6.(7th for keys)        ----      encrypt(serial)
sec 2 - blocks 8,9,10.(11th for keys)     ----      no_of_use, machine serial.
sec 3 -	blocks 12,13,14.(15th for keys)		    manufacturing_date,date_of_last_use.
sec 4 - blocks 16,17,18(19th for keys)    ----      installation_date,unusable_date
sec 5 - blocks 20,21,22(23th for keys)    ----     machine name, pateint 1st name , 2nd name.
sec 6 - blocks 24,25,26(27th for keys)    ----     doc 1st name, 2nd name.
*/

#define no_of_use 100

// SECTOR 1 //
#define block_encrypt 04

// SECTOR 2 //
#define block_no_use 8
#define block_mach_serial1 9
#define block_mach_serial2 10


// SECTOR 3 //
#define block_manu_date 12
#define block_manu_and_last_date 13
#define block_last_date 14

// SECTOR 4 //
#define block_install_date 16
#define block_install_and_unusable_date 17
#define block_unusable_date 18


// SECTOR 5 //
#define block_mach_name 20
#define block_pat_name1 21
#define block_pat_name2 22


// SECTOR 6 //
#define block_doc_name1 24
#define block_doc_name2 25




#define default_access 0xff078069			//transport config//
#define nonread_nonwrite_access 0x88f0f769
#define read_only_access 0x00f0ff69			//destroy the rfid card//
#define default_keyA 0xffffffffffff			// 6byte //
#define default_keyB 0xffffffffffff			// 6byte //

#define keyA 0xffffffffffff			// 6byte //
#define keyB 0xffffffffffff			// 6byte //






#define keyA1 0xffffffffffff			// 6byte //
#define keyA2 0xffffffffffff			// 6byte //
#define keyA3 0xffffffffffff			// 6byte //
#define keyA4 0xffffffffffff			// 6byte //
#define keyA5 0xffffffffffff			// 6byte //
#define keyA6 0xffffffffffff			// 6byte //

#define keyB1 0xffffffffffff			// 6byte //
#define keyB2 0xffffffffffff			// 6byte //
#define keyB3 0xffffffffffff			// 6byte //
#define keyB4 0xffffffffffff			// 6byte //
#define keyB5 0xffffffffffff			// 6byte //
#define keyB6 0xffffffffffff			// 6byte //






#define DATA1 0xffffffffffffffff	//higher bit
#define DATA2 0xbbbbbbbbbbbbbbaa	//lower bit		// 16 bytes //



#define usb_serial "A900fDhH"
#define description FT232R
typedef uint8_t byte;

byte head_packet[2]={0xaa,0xbb};

byte packet[30];
byte packet_recieved[30];

struct ftdi_context ftdi;

int attempts=0;


int i,a=0;
void ftdi_bootup()
{

	if(ftdi_init(&ftdi)!=0)
	{
		printf("ftdi - could not be opened.\nr");
	}
	else
		printf("booting up.\n");


//	int ftdi_usb_open_desc	(	struct ftdi_context * 	ftdi,int vendor,int product,const char * description,const char * serial )

	if(ftdi_usb_open_desc(&ftdi,0x0403, 0x6001,NULL/*description*/,usb_serial)!=0)
	{
		printf("ftdi - error.\n");
	}
	else
	{
		printf("ftdi opened.\n");
		ftdi_set_baudrate(&ftdi,19200);
	}

}



void ftdi_close()
{

	if(ftdi_usb_close(&ftdi)!=0)
	{
		printf("\nusb could not be closed.\n");

	}
	else
		printf("\nusb closed.\n");

	 ftdi_deinit(&ftdi);

}


//   Checksum    :     	Exclusive ORed result from Len to Data inclusively, 1 byte.            //


byte checksum(int packet_no)
{
	int temp;
	char str[3];
	byte result=0;
	temp=packet_no-1;
	//result=packet[temp];
	//temp--;
	while(temp>1)
	{
		result^=packet[temp];
		temp--;
	}


	return result;

}

byte length(int packet_no)
{
	return(packet_no-2);
}


void switch_packet()
{
	//	switching on the rf transmit		//
	int long switching_on_flag=1;
	printf("\nswitching on:\n");
	//packet[2]=length calculation at end
	packet[3]=0x01;		//command for switching on/off.
	packet[4]=0x01;		//turn on.
	packet[2]=length(5);	//Len:Byte length counting from Command to Checksum inclusively, 1 byte.
	packet[5]=checksum(5);



	// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data.:");
	for(i=0;i<6;i++)
		printf("%x\t",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,6);

	if(a>0)
		printf("bytes written(no error):%d\n",a);

	else
	{
//		printf("error.\n");
	}

	printf("DATA recieved:\n");
	attempts=0;
 	while(attempts<100)
	{	attempts++;
		if(ftdi_read_data(&ftdi,packet_recieved,6)>0)
		{
			printf("bytes rd(no error):%d\n",a);
			for(i=0;i<6;i++)
			{	if(i==4)
				{	printf("(err)");
					switching_on_flag=packet_recieved[i];
				}
				printf("%x\t",packet_recieved[i]);
			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
//			printf("NO DAta\n");
		}
	}
	if(switching_on_flag!=0)
	{
		printf("\nNO module connected ERROR!!!\n");
		exit(0);
	}

}

long int card_select_packet()
{
	//	Card Select.		//
	long int switching_on_flag=1;
	printf("\ncard select\n");
	packet[3]=0x10;
	packet[2]=length(4);
	packet[4]=checksum(4);

		// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data.:");
	for(i=0;i<5;i++)
		printf("%x\t",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,5);
	if(a>0)
		printf("bytes written(no error):%d\n",a);
	else
	{
		//printf("error.\n");
	}

	printf("DATA recieved:");
	attempts=0;
	while(attempts<100)
	{	attempts++;
		if(ftdi_read_data(&ftdi,packet_recieved,11)>0)
		{
			printf("bytes rd(no error):%d\n",a);
			for(i=0;i<11;i++)
			{	if(i==4)
				{	printf("(err)");
					switching_on_flag=packet_recieved[i];
				}
				printf("%x  ",packet_recieved[i]);
			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
//			printf("NO DAta\n");
		}
	}
		if(switching_on_flag!=0)
	{
		printf("\nNO RFID CARD ERROR!!!\n");
		exit(0);
	}
	long int serial=0;
	int j=0;
	for(i=5;i<9;i++)
	{
		serial=(serial)*256;
		serial+=(packet_recieved[i]);
/*		printf("key_a=%lx\n",(long int)serial);			*/

	}

	if(switching_on_flag!=0)
	{
		printf("\nNO RFID CARD ERROR!!!\n");
		exit(0);
	}

	return serial;



}

long int data_recieved[16];

void read_packet_define(int block,int key_type,double key_A,double key_B)
{
	packet[3]=0x11;
	packet[4]=key_type;
	packet[5]=block;

/*	double key_A=keyA;*/
/*	double key_B=keyB;*/
	int i=11;
	if(key_type_used==0)
	{	while(i>5)
		{

			packet[i]=((long int)key_A)%256;
/*			printf("key_a=%e\n",key_A);			*/
			key_A=((long int)key_A)/256;
			i--;

		}
	}

	else if(key_type_used==1)
	{	while(i>5)
		{

			packet[i]=((long int)key_B)%256;
/*			printf("key_a=%e\n",key_B);			*/
			key_B=((long int)key_B)/256;
			i--;

		}

	}
	packet[2]=length(12);
	packet[12]=checksum(12);


	// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data(reading).:");
	for(i=0;i<13;i++)
		printf("%x\t",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,13);
	if(a>0)
		printf("bytes written(no error):%d\n",a);
	else
	{
//		printf("error.\n");
	}

	printf("DATA recieved:");
	attempts=0;
	int j=0;
	while(attempts<100)
	{	attempts++;
		a=ftdi_read_data(&ftdi,packet_recieved,22);
		if(a>0)
		{	printf("bytes rd(no error):%d\n",a);
			for(i=0;i<22;i++)
			{	if(i==4)
					printf("(err)");
				else if(i>4 && i<21)
				{
					data_recieved[j]=packet_recieved[i];
					j++;
				}
				printf("%x  ",packet_recieved[i]);

			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
//			printf("error.\n");
		}
	}
	printf("\nDOne packet reading.\n");


}



int write_packet_define(unsigned long int data1,unsigned long int data2,int block,int key_type,double key_A,double key_B)
{
	packet[3]=0x12;
	packet[4]=key_type;
	packet[5]=block;
	// KEY //
/*	double key_A=keyA;*/
/*	double key_B=keyB;*/
	int i=11;


/*	unsigned long int data=DATA_WRITE;*/

	if(key_type==0)
	{	while(i>5)
		{

			packet[i]=((long int)key_A)%256;
/*			printf("key_a=%e\n",key_A);			*/
			key_A=((long int)key_A)/256;
			i--;

		}
	}

	else if(key_type==1)
	{	while(i>5)
		{

			packet[i]=((long int)key_B)%256;
/*			prinr)fftf("key_a=%e\n",key_B);			*/
			key_B=((long int)key_B)/256;
			i--;

		}

	}

	// DATA //

/*	long double data=DATA;*/

/*	unsigned long int data1=DATA1;*/
/*	unsigned long int data2=DATA2;*/

	i=27;
	while(i>11)
	{	if(i<20)
		{
			packet[i]=((unsigned long int)data1)%256;
			printf("data1=%lx\n",data1);
			data1=((unsigned long int)data1)/256;

		}
		else
		{	packet[i]=((unsigned long int)data2)%256;
			printf("data2=%lx\n",data2);
			data2=((unsigned long int)data2)/256;
		}
		i--;

	}

	for(i=12;i<28;i++)
		printf("packets- %x \n",packet[i]);

	 packet[2]=length(28);
	packet[28]=checksum(28);




	// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data.:");
	for(i=0;i<29;i++)
		printf("%x  ",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,29);
	if(a>0)
		printf("bytes written(no error):%d\n",a);
	else
	{
//		printf("error.\n");
	}

	printf("DATA recieved:");
	attempts=0;
	while(attempts<200)
	{	attempts++;
		a=ftdi_read_data(&ftdi,packet_recieved,6);
		if(a>0)
		{
			printf("bytes rd(no error):%d\n",a);
			for(i=0;i<6;i++)
			{
				if(i==4)
					printf("(err)");
				printf("%x  ",packet_recieved[i]);
			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
	//		printf("error.\n");
		}
	}

	return(0);
}


void initialize_value_block(int key_type,int block,double key_A,double key_B,int value)
{	packet[3]=0x13;
	packet[4]=key_type;
	packet[5]=block;

/*	double key_A=keyA;*/
/*	double key_B=keyB;*/
	int i=11;
	if(key_type==0)
	{	while(i>5)
		{

			packet[i]=((long int)key_A)%256;
/*			printf("key_a=%e\n",key_A);			*/
			key_A=((long int)key_A)/256;
			i--;

		}
	}

	else if(key_type==1)
	{	while(i>5)
		{

			packet[i]=((long int)key_B)%256;
/*			printf("key_a=%e\n",key_B);			*/
			key_B=((long int)key_B)/256;
			i--;

		}

	}


	packet[2]=length(13);
	packet[12]=value;
	packet[13]=checksum(13);


	// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data.:");
	for(i=0;i<14;i++)
		printf("%x\t",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,14);
	if(a>0)
		printf("bytes written(no error):%d\n",a);
	else
	{
//		printf("error.\n");
	}

	printf("DATA recieved:");
	attempts=0;
	while(attempts<100)
	{	attempts++;
		a=ftdi_read_data(&ftdi,packet_recieved,6);
		if(a>0)
		{	printf("bytes rd(no error):%d\n",a);
			for(i=0;i<6;i++)
			{	if(i==4)
					printf("(err)");
				printf("%x  ",packet_recieved[i]);

			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
//			printf("error.\n");
		}
	}





}

int long present_no_of_use=0;

void reading_value_block(int key_type,int block,double key_A,double key_B)
{	packet[3]=0x14;
	packet[4]=key_type;
	packet[5]=block;

/*	double key_A=keyA;*/
/*	double key_B=keyB;*/
	int i=11;
	if(key_type==0)
	{	while(i>5)
		{

			packet[i]=((long int)key_A)%256;
/*			printf("key_a=%e\n",key_A);			*/
			key_A=((long int)key_A)/256;
			i--;

		}
	}

	else if(key_type==1)
	{	while(i>5)
		{

			packet[i]=((long int)key_B)%256;
/*			printf("key_a=%e\n",key_B);			*/
			key_B=((long int)key_B)/256;
			i--;

		}

	}


	packet[2]=length(12);
	packet[12]=checksum(12);


	// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data.:");
	for(i=0;i<13;i++)
		printf("%x\t",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,13);
	if(a>0)
		printf("bytes written(no error):%d\n",a);
	else
	{
//		printf("error.\n");
	}

	printf("DATA recieved:");
	attempts=0;
	while(attempts<100)
	{	attempts++;
		a=ftdi_read_data(&ftdi,packet_recieved,7);
		if(a>0)
		{	printf("bytes rd(no error):%d\n",a);
			for(i=0;i<7;i++)
			{	if(i==4)
					printf("(err)");
				else if(i==5)
					present_no_of_use=packet_recieved[i];
				printf("%x  ",packet_recieved[i]);

			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
//			printf("error.\n");
		}
	}
	printf("\n");
}


void write_keys_packet_define(unsigned long int key_A_new,unsigned long int key_B_new,unsigned long int access_bits,int block,int key_type,double key_A,double key_B)
{

/*	write_keys_packet_define(keyA,keyB,block_used,key_type_used,default_keyA,default_keyB);*/
	packet[3]=0x12;
	packet[4]=key_type;
	packet[5]=block;
	// KEY //
/*	double key_A=keyA;*/
/*	double key_B=keyB;*/
	int i=11;


/*	unsigned long int data=DATA_WRITE;*/

	if(key_type==0)
	{	while(i>5)
		{

			packet[i]=((long int)key_A)%256;
/*			printf("key_a=%e\n",key_A);			*/
			key_A=((long int)key_A)/256;
			i--;

		}
	}

	else if(key_type==1)
	{	while(i>5)
		{

			packet[i]=((long int)key_B)%256;
/*			printf("key_a=%e\n",key_B);			*/
			key_B=((long int)key_B)/256;
			i--;

		}

	}

	// DATA //

/*	long double data=DATA;*/

/*	unsigned long int data1=DATA1;*/
/*	unsigned long int data2=DATA2;*/

	i=27;
	while(i>11)
	{	if(i>11 && i<18)
		{
			packet[i]=((unsigned long int)key_A_new)%256;
/*			printf("data1=%lx\n",data1);			*/
			key_A_new=((unsigned long int)key_A_new)/256;

		}
		else if(i<28 && i>21)
		{	packet[i]=((unsigned long int)key_B_new)%256;
/*			printf("data2=%lx\n",data2);			*/
			key_B_new=((unsigned long int)key_B_new)/256;
		}
		else
		{
			packet[i]=((unsigned long int)access_bits)%256;
		//	printf("data2=%lx\n",access_bits);
			access_bits=((unsigned long int)access_bits)/256;


		}
		i--;

	}

//	for(i=12;i<28;i++)
	//	printf("packets- %x \n",packet[i]);

	 packet[2]=length(28);
	packet[28]=checksum(28);




	// 	SENDING AND RECIEVING //

	//ftdi_write_data(ftdi_pointer,packet,length);
	printf("writting data.:");
	for(i=0;i<29;i++)
		printf("%x  ",packet[i]);

	printf("\n");
	a=ftdi_write_data(&ftdi,packet,29);
	if(a>0)
		printf("bytes written(no error):%d\n",a);
	else
	{
//		printf("error.\n");
	}

	printf("DATA recieved:");
	attempts=0;
	while(attempts<200)
	{	attempts++;
		a=ftdi_read_data(&ftdi,packet_recieved,6);
		if(a>0)
		{	printf("bytes rd(no error):%d\n",a);
			for(i=0;i<6;i++)
			{
				if(i==4)
					printf("(err)");
				printf("%x  ",packet_recieved[i]);
			}
			ftdi_usb_purge_buffers(&ftdi);
			break;
		}
		else
		{
//			printf("error.\n");
		}
	}
}


char string[17];
int size_of_encrypted_data=0;

char *encryption(long int data_for_encrption)
{
	printf("data_for_encrption=%lx\n\n",data_for_encrption);



	size_of_encrypted_data=5;
	for(i=size_of_encrypted_data-1;i>=0;i--)
	{	string[i]=((unsigned long int)data_for_encrption)%256;
	/*	printf("data1=%lx\n",data1);			*/
		data_for_encrption=((unsigned long int)data_for_encrption)/256;
	}

//	string[17]=(char)data_for_encrption;//'\0';
//	string[18]='\0';
//	printf("\n\n\n%sstring\n\n\n",string);
//	printf("1\n");
	return string;


}

char* time_stamp()
{
	char *date;
	time_t timer;
	timer=time(NULL);  //time function returns current time //
	date=asctime(localtime(&timer));  //Convert tm structure to string //
	printf("Current Date:%s", date);

	return date;
}



void initialize(long int serial)
{

	//write serials for sectors// //changing the sector keys//
	write_keys_packet_define(keyA1,keyB1,default_access,7/*07block_to_write*/,0/*key_type_used*/,default_keyA,default_keyB);
	write_keys_packet_define(keyA2,keyB2,default_access,11/*11block_to_write*/,0/*key_type_used*/,default_keyA,default_keyB);
	write_keys_packet_define(keyA3,keyB3,default_access,15/*15block_to_write*/,0/*key_type_used*/,default_keyA,default_keyB);
	write_keys_packet_define(keyA4,keyB4,default_access,19/*19block_to_write*/,0/*key_type_used*/,default_keyA,default_keyB);
	write_keys_packet_define(keyA5,keyB5,default_access,23/*23block_to_write*/,0/*key_type_used*/,default_keyA,default_keyB);
	write_keys_packet_define(keyA6,keyB6,default_access,27/*27block_to_write*/,0/*key_type_used*/,default_keyA,default_keyB);


	printf("\n\n\nKEYS changed.\n\n\n");

	//Read serials for secctors// //reading changed keys//
	read_packet_define(7,0,default_keyA,default_keyB);
	read_packet_define(11,0,default_keyA,default_keyB);
	read_packet_define(15,0,default_keyA,default_keyB);
	read_packet_define(19,0,default_keyA,default_keyB);
	read_packet_define(23,0,default_keyA,default_keyB);
	read_packet_define(27,0,default_keyA,default_keyB);

//	sleep(100000);
/*	char *encrypted_data;*/

/*	encrypted_data=encryption(serial);*/
/*	printf("encrypted_data:%s", encrypted_data);*/
/*	*/

	char *encrypted_data;

	encrypted_data=encryption(serial);
	printf("encrypted_data:%s\n", encrypted_data);
//	sleep(100000);
	long int gen=0;
	for(i=0;i<size_of_encrypted_data;i++)
	{	gen=(gen)*256;
		gen+=(encrypted_data[i]);
		//	printf("gen=%lx\n",gen);
	}
	printf("gen=%lx\n",gen);


	//writing encrypted data to block 4 //

	long int enc_data_1=0,enc_data_2=0;
	enc_data_1=0;
	enc_data_2=gen;

	printf("\n%lx\n%lx\n",enc_data_1,enc_data_2);

//	sleep(100000);
//	enc_data1=gen;



	write_packet_define(enc_data_1,enc_data_2,block_encrypt,key_type_used,keyA1,keyB1);

	//making access as non-readable and non-writable for encrypted_key block//
	//unhash will block the encrypted block as nonread_nonwrite_access//
//	write_keys_packet_define(keyA1,keyB1,nonread_nonwrite_access,7/*07block_to_write*/,0/*key_type_used*/,keyA1,keyB1);

//	printf("aaaaaaaa");
//	sleep(10000000);


	//INITIALIZING A VALUE BLOCK OF 100//

	initialize_value_block(key_type_used,block_no_use,keyA2,keyB2,no_of_use);

	printf("\nReading Block value\n");


	reading_value_block(key_type_used,block_no_use,keyA2,keyB2);

	//SYSTEM DATE AND TIME//

	char *stamp;
	stamp=time_stamp();
//	sleep(1000000);
	printf("\n111111\n");
	long int time_data1=0,time_data2=0,time_data3=0;

	//WRITING ASCII value of TIMESTAMP DATA to RFID//

	for(i=0;i<24;i++)
	{
		if(i<8)
		{	time_data1=(time_data1)*256;
			time_data1+=(stamp[i]);
		//		printf("time_data1=%lx\n",(long int)time_data1);
		}
		else if(i>7 && i<16)
		{	time_data2=(time_data2)*256;
			time_data2+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}
		else if(i>15 && i<24)
		{	time_data3=(time_data3)*256;
			time_data3+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}

	printf("\ntimestamp %lx , %lx , %lx\n",time_data1,time_data2,time_data3);

	}


	write_packet_define(time_data1,time_data2,block_manu_date,key_type_used,keyA,keyB);
	write_packet_define(time_data3,0,block_manu_and_last_date,key_type_used,keyA,keyB);




}

void machine_install(long int serial)
{

	//CHECKING FOR ENCRYPTED DATA//

	char *encrypted_data;
	int k,flag=0,position=0;

	encrypted_data=encryption(serial);
	printf("encrypted_data:%s\n", encrypted_data);
	long int gen=0,check_encrypted=0;
	for(i=0;i<size_of_encrypted_data;i++)
	{	gen=(gen)*256;
		gen+=(encrypted_data[i]);
		//	printf("gen=%lx\n",gen);
	}
	printf("gen=%lx\n",gen);

	read_packet_define(block_encrypt,key_type_used,keyA1,keyB1);

	printf("\n");
	for(k=0;k<16;k++)
	{	if(data_recieved[k]!=0 && flag==0)
		{	flag=1;
			position=k;
		}
		if(flag==1)
		{	check_encrypted*=256;
			check_encrypted+=(data_recieved[k]);

		}
		printf("%lx",data_recieved[k]);
	}
//		printf("check_encrypted=%lx\n",check_encrypted);
//	for(k=position)

	if(gen==check_encrypted)
	{
		printf("\n\nencrypted data checked, card is ok!!!\n\n");

	}
	else
	{
		printf("\n\nencrypted data does not match, false card!!!\n\n");
		sleep(10);
		exit(0);
	}

	printf("\n");


	//SYSTEM DATE AND TIME at installation//

	char *stamp;
	stamp=time_stamp();
//	sleep(1000000);
	printf("\n111111\n");
	long int time_data1=0,time_data2=0,time_data3=0;

	//WRITING ASCII value of TIMESTAMP DATA to RFID//

	for(i=0;i<24;i++)
	{
		if(i<8)
		{	time_data1=(time_data1)*256;
			time_data1+=(stamp[i]);
		//		printf("time_data1=%lx\n",(long int)time_data1);
		}
		else if(i>7 && i<16)
		{	time_data2=(time_data2)*256;
			time_data2+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}
		else if(i>15 && i<24)
		{	time_data3=(time_data3)*256;
			time_data3+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}

	printf("\ntimestamp %lx , %lx , %lx\n",time_data1,time_data2,time_data3);

	}

	write_packet_define(time_data1,time_data2,block_install_date,key_type_used,keyA4,keyB4);
	write_packet_define(time_data3,0,block_install_and_unusable_date,key_type_used,keyA4,keyB4);

	//WRITING MACHINE NAME ON RFID//

	char machine_name[] = "write machine name";

	long int machine_name1=0,machine_name2=0;

	printf("\nmachine name  = %s\n",machine_name);

	//Saving ascii values for machine name//

	for(i=0;i<16;i++)
	{
		if(i<8)
		{	machine_name1=(machine_name1)*256;
			machine_name1+=(machine_name[i]);
		//		printf("time_data1=%lx\n",(long int)time_data1);
		}
		else if(i>7 && i<16)
		{	machine_name2=(machine_name2)*256;
			machine_name2+=(machine_name[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}

	}

	write_packet_define(machine_name1,machine_name2,block_mach_name,key_type_used,keyA5,keyB5);

	//WRITING MACHINE machine serial ON RFID//

	char machine_serial[] = "write serial here";

	long int machine_serial1=0,machine_serial2=0,machine_serial3=0,machine_serial4=0;

	printf("\nmachine serial  = %s\n",machine_serial);



	//WRITING ASCII value of MACHINE SERIAL to RFID//


	for(i=0;machine_serial[i]!='\0';i++)
	{
		if(i<8)
		{	machine_serial1=(machine_serial1)*256;
			machine_serial1+=(machine_serial[i]);
		//		printf("time_data1=%lx\n",(long int)time_data1);
		}
		else if(i>7 && i<16)
		{	machine_serial2=(machine_serial2)*256;
			machine_serial2+=(machine_serial[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}
		else if(i>15 && i<24)
		{	machine_serial3=(machine_serial3)*256;
			machine_serial3+=(machine_serial[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}
		else if(i>23 && i<32)
		{	machine_serial4=(machine_serial4)*256;
			machine_serial4+=(machine_serial[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}

	printf("\nmachine_serial %lx , %lx , %lx ,%lx\n",machine_serial1,machine_serial2,machine_serial3,machine_serial4);

	}

	write_packet_define(machine_serial1,machine_serial2,block_mach_serial1,key_type_used,keyA2,keyB2);
	write_packet_define(machine_serial3,machine_serial4,block_mach_serial2,key_type_used,keyA2,keyB2);


	//SAVE INSTALLED SERIAL NO. of RFID TAG to machine//
	//long int serial variable to write in machine
/////////////////////////////////////////////////////////////////////////////////



	//Change access bits for block 4 // sector 1//

}
void unusable()
{
	//UNUSABLE SYSTEM DATE AND TIME at installation//
	int long previous_time_data;
	char *stamp;
	stamp=time_stamp();
//	sleep(1000000);
	printf("\n111111\n");
	long int time_data1=0,time_data2=0,time_data3=0;

	//WRITING ASCII value of unusable TIMESTAMP DATA to RFID//

	for(i=0;i<24;i++)
	{
		if(i<8)
		{	time_data1=(time_data1)*256;
			time_data1+=(stamp[i]);
		//		printf("time_data1=%lx\n",(long int)time_data1);
		}
		else if(i>7 && i<16)
		{	time_data2=(time_data2)*256;
			time_data2+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}
		else if(i>15 && i<24)
		{	time_data3=(time_data3)*256;
			time_data3+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}

	printf("\ntimestamp %lx , %lx , %lx\n",time_data1,time_data2,time_data3);

	}
	read_packet_define(block_install_and_unusable_date,key_type_used,keyA1,keyB1);
/*	data_recieved[16];*/

	for(i=0;i<8;i++)
	{	previous_time_data=(previous_time_data)*256;
			previous_time_data+=(data_recieved[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
	}
	write_packet_define(previous_time_data,time_data1,block_install_and_unusable_date,key_type_used,keyA4,keyB4);
	write_packet_define(time_data2,time_data3,block_unusable_date,key_type_used,keyA4,keyB4);

	//MAKE ALL ACCESS BITS TO READ ONLY //
	//write serials for sectors// //changing the sector keys//
	//dont unhash it will spoil the rfid card //
//	write_keys_packet_define(keyA1,keyB1,read_only_access,7/*07block_to_write*/,0/*key_type_used*/,keyA1,keyB1);
//	write_keys_packet_define(keyA2,keyB2,read_only_access,11/*11block_to_write*/,0/*key_type_used*/,keyA2,keyB2);
//	write_keys_packet_define(keyA3,keyB3,read_only_access,15/*15block_to_write*/,0/*key_type_used*/,keyA3,keyB3);
//	write_keys_packet_define(keyA4,keyB4,read_only_access,19/*19block_to_write*/,0/*key_type_used*/,keyA4,keyB4);
//	write_keys_packet_define(keyA5,keyB5,read_only_access,23/*23block_to_write*/,0/*key_type_used*/,keyA5,keyB5);
//	write_keys_packet_define(keyA6,keyB6,read_only_access,27/*27block_to_write*/,0/*key_type_used*/,keyA6,keyB6);


}
void use_in_machine_reuse(long int serial)
{

	//verify installed serial==bottle serial
	//serial var has bottle serial//

	//(if not true , try INSTALL IN MACHINE-step)
//	if(machine_serial!=serial)
//	machine_install(serial);

	//if not there(write block 21 and 22 patient name ---- 24 and 25 doctor name.)
	//else verify doc and patient name.



	//verify block8 value >0
	//if not, go to unusable.
	//else decrement value in block 8.
	present_no_of_use=0;
	reading_value_block(key_type_used,block_no_use,keyA2,keyB2);
	if(present_no_of_use>0)
	{
		present_no_of_use--;
		initialize_value_block(key_type_used,block_no_use,keyA2,keyB2,present_no_of_use);
	}
	else if(present_no_of_use<0)
	{
		unusable();

	}

	//date_of_use(keep overwriting)-block13(last 8bytes),14.
	//WRITING SYSTEM DATE AND TIME at every use//

	int long previous_time_data;
	char *stamp;
	stamp=time_stamp();
//	sleep(1000000);
	printf("\n111111\n");
	long int time_data1=0,time_data2=0,time_data3=0;

	//WRITING ASCII value of unusable TIMESTAMP DATA to RFID//

	for(i=0;i<24;i++)
	{
		if(i<8)
		{	time_data1=(time_data1)*256;
			time_data1+=(stamp[i]);
		//		printf("time_data1=%lx\n",(long int)time_data1);
		}
		else if(i>7 && i<16)
		{	time_data2=(time_data2)*256;
			time_data2+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}
		else if(i>15 && i<24)
		{	time_data3=(time_data3)*256;
			time_data3+=(stamp[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
		}

	printf("\ntimestamp %lx , %lx , %lx\n",time_data1,time_data2,time_data3);

	}
	read_packet_define(block_install_and_unusable_date,key_type_used,keyA1,keyB1);
//	data_recieved[16];

	for(i=0;i<8;i++)
	{	previous_time_data=(previous_time_data)*256;
			previous_time_data+=(data_recieved[i]);
		/*		printf("key_a=%lx\n",(long int)serial);			*/
	}

	write_packet_define(previous_time_data,time_data1,block_manu_and_last_date,key_type_used,keyA3,keyB3);
	write_packet_define(time_data2,time_data3,block_last_date,key_type_used,keyA3,keyB3);


	//CHECK 100% TRIGGER IF YES CALL UNUSABLE//


}

int main()
{

	ftdi_bootup();
	packet[0]=head_packet[0];
	packet[1]=head_packet[1];

	int option=0;

	printf("\n1.INITIALIZE(Dutt Factory)\n2.INSTALL IN MACHINE\n3.USE IN MACHINE AND RESUSE\nSelect the options:",option);
	scanf("%d",&option);
	printf("\n");
	// switched on //
	switch_packet();

	// card serial read //
	long int serial_no;

	serial_no=card_select_packet();
	printf("\n\nSERIAL:%lx\n",serial_no);
//	sleep(100000000);


	//INITIALIZE (Datt factory)//

	//read serial
	//write keyA1(Sec1), keyA2(Sec2), keyA3(Sec3),keyA4(Sec4),keyA5(sec5),keyA6(sec6).
	//block 4 (encrypt(serial)==same)
	//block 8 (initialize value block to 100)
	//block 12,13(1st 8 bits)(manufacturing date and time)
	if(option==1)
	{
		initialize(serial_no);
	}



	//-INSTALL IN MACHINE(also check every min.)
	//read serial
	//using keyA1, verify & encrypt(serial)==read.block(4)
	//block 16,17(1st 8 bits)  (installation date and time)
	//block 20(machine name -> write)
	//block 9,10(machine serial -> write)
	//save installed serial no. of tag to machine
	//if Correct, Proceed and Write(null) and change ACCESS BIT for sector1 => Block(4)
	else if(option==2)
	{
		machine_install(serial_no);
	}



		//-USE IN MACHINE AND RESUSE
	//verify installed serial==bottle serial
	//(if not true , try INSTALL IN MACHINE-step)
	//if not there(write block 21 and 22 patient name ---- 24 and 25 doctor name.)
	//else verify doc and patient name.
	//verify block8 value >0
	//date_of_use(keep overwriting)-block13(last 8bytes),14.
	//if not, go to unusable.
	//else decrement value in block 8.

		//-unusable
	//if block8<=0
	//write time stamp - block17(last 8bytes),block 18.
	//make all access bits to read only.
	//also if 100% trigger.
	else if(option==3)
	{
		use_in_machine_reuse(serial_no);
	}



//	read_packet_define(11,key_type_used,default_keyA,default_keyB);


	// installation in machine //
	// use and reuse in machine //
	// unsusable //
	printf("\nend of program.\n");
/*	sleep(10000);*/

/*	int a;*/
/*	printf("\n\n1.read data or 2. write data:");*/
/*	scanf("%d",&a);*/

/*	if(a==1)*/
/*	{	read_packet_define(block_used,key_type_used,keyA,keyB);*/

/*	}*/
/*	else if(a==2)*/
/*	{	*/
/*		write_packet_define(DATA1,DATA2,block_used,key_type_used,keyA,keyB);*/

/*		write_packet_define(DATA1,DATA2);*/
/*	*/
/*	}*/

	// for writing keys // impo

/*	write_keys_packet_define(keyA,keyB,default_access,block_used,key_type_used,default_keyA,default_keyB);*/


	//system date and time //


//	char *stamp;
//	stamp=time_stamp();

	ftdi_close();

}
