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
  if(tag.serial.open("/dev/ttySAC1",19200))
    cout<<"Port Open\n";
  else
    cout<<"Could not open port\n";

  }
  return 0;
}
