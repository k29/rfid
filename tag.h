#ifndef __TAG_H__
#define __TAG_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial.h"

typedef uint8_t byte;

class Tag_Actions
{
  private:
    byte packet[30];
    byte packet_received[30];

    void checksum(byte a[]); //XOR from len to data i.e packet[2] to packet[len+1]

  public:
    Tag_Actions()
    {
      packet[0]=0xAA;
      packet[1]=0xBB;
    }

    void control_rf_transmit();


}






#endif
