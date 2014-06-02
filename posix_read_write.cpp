#include "serial.h"
#include <iostream>
#include <stdint.h>

typedef unsigned char byte;

using namespace std;
using namespace HAL;

int main()
{
  Serial ser;
  if(ser.Open("/dev/ttySAC3",115200))
  {
    cout<<"port open\n";
    byte packet[]={0xAA,0xBB,0x03,0x01,0x01,0x03,0xAA,0x2D,0x7E,0x37};
    for(int i=0;i<sizeof(packet);i++)
      ser.WriteByte((char)packet[i]);
    //ser.WriteByte('k');
    //ser.WriteString(std::string("0xAA"));
    ser.Close();
  }
  else
  {
    cout<<"port could not open\n";
  }
  return 0;
}
