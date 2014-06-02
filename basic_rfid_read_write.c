//ftdi read and write code.

#include <stdio.h>
#include <stdlib.h>
#include <ftdi.h>
#include <unistd.h>
#include <inttypes.h>
#include <time.h>
#include <math.h>

#define usb_serial "A900fDlf"//"A900fDhH"
#define description FT232R
typedef uint8_t byte;

struct ftdi_context ftdi;

byte packet[30];


void ftdi_bootup()
{
  if(ftdi_init(&ftdi)!=0)
    printf("ftdi - could not be opened.\nr");
  else
    printf("booting up.\n");

  if(ftdi_usb_open_desc(&ftdi,0x0403, 0x6001,NULL/*description*/,usb_serial)!=0)
    printf("ftdi - error.\n");
  else
  {
    printf("ftdi opened.\n");
    ftdi_set_baudrate(&ftdi,115200);
  }
}

void ftdi_close()
{
  if(ftdi_usb_close(&ftdi)!=0)
    printf("\nusb could not be closed.\n");
  else
    printf("\nusb closed.\n");
  ftdi_deinit(&ftdi);
}


int main()
{
  ftdi_bootup();
  //printf("sending data to the rfid card\n");
  //byte pack[]={0xaa,0xbb,0x03,0x01,0x01,0x03};
  //ftdi_write_data(&ftdi,pack,6);
  //printf("data written");
  byte packet_recieved[6];
  int i,attempts=100;
  //usleep(5000);
  do
  {
    while(ftdi_read_data(&ftdi,packet_recieved,1)>0)
    {
        printf("data reading...\n");
        for(i=0;i<1;i++)
          printf("%x  ",packet_recieved[i]);
    }
  }while(1);

  ftdi_close();
}
