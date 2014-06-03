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
  Serial serial;
  Tag_Actions tag;
  if(serial.open("/dev/ttySAC1",19200))
  {
    cout<<"Port Open\n";

    //perform actions here...
    
  }
  return 0;
}
