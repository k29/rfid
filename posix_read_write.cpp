//Read write serial data to the rfid reader. Make for the arm using make arm for other just make.

#include "serial.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>


typedef unsigned char byte;

using namespace std;
using namespace HAL;

int main()
{
  Serial s;
  if(s.Open("/dev/ttySAC1",19200))
  {
    cout<<"port open\n";
    byte packet[]={0xAA,0xBB,0x03,0x01,0x01,0x03};
    for(int i=0;i<sizeof(packet);i++)
    {
      cout<<"writing...\n";
      s.WriteByte((char)packet[i]);
    }

    byte packet_read[1];

    int attempt=100;
    bool flag=false;
    while(attempt--)
    {
      if(s.Read(packet_read,1)>0)
      {
        for(int i=0;i<sizeof(packet_read);i++)
          printf("read packet %x\n",packet_read[i]);
        flag=true;
      }

    }
    if(!flag)
      cout<<"nothing to read...\n";
    cout<<"closing port...\n";
    s.Close();
  }
  else
  {
    cout<<"port could not open\n";
  }
  return 0;
}
