#include "serial.h"
#include "tag.h"
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef unsigned char byte;

using namespace std;
using namespace HAL;

int main()
{
  Tag_Actions tag;
  if(!tag.serial.Open("/dev/ttySAC1",19200))
  {
    cout<<"Port could not open\n";
    return 1;
  }
  else
  cout<<"Port open\n";
  tag.control_rf_transmit(true);
  //tag.select_mifare_card();
  return 0;
}
