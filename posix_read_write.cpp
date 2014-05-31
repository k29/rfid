#include "serial.h"
#include <iostream>
#include <stdint.h>

typedef uint8_t byte;

using namespace std;
using namespace HAL;
int main()
{
  Serial s;
  if(s.Open("/dev/ttySAC3",19200))
  {
    cout<<"port open\n";
    byte packet[6];
    packet[0]=0xAA;
    s.Write(packet,1);
    packet[1]=0xBB;
    s.Write(packet+1,1);
    packet[2]=0x03;
    s.Write(packet+2,1);
    packet[3]=0x01;
    s.Write(packet+3,1);
    packet[4]=0x01;
    s.Write(packet+4,1);
    packet[5]=0x03;
    s.Write(packet+5,1);


    s.Close();
  }
  else
  {
    cout<<"port could not open\n";
  }
  return 0;
}
