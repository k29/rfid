#include "serial.h"

int main()
{
  Serial s;
  if(s.Open("/dev/ttySAC3",19200))
  {
    cout<<"port open\n";
    s.Write("0xAA",1);
    s.Write("0xBB",1);
    s.Write("0x03",1);
    s.Write("0x01",1);
    s.Write("0x01",1);
    s.Write("0x03",1);
    s.Close();
  }
  else
  {
    cout<<"port could not open\n";
  }
  retrun 0;
}
