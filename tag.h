#ifndef __TAG_H__
#define __TAG_H__

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial.h"

typedef unsigned char byte;
using namespace HAL;
using namespace std;

class Tag_Actions
{
  private:
    byte packet[30];
    byte packet_received[30];

    void checksum(); //XOR from len to data i.e packet[2] to packet[len+1]
    void packet_reset(); //initialises packet header and sets the rest to zero;

  public:
    Tag_Actions()
    {
        packet_reset();
    }

    Serial serial;

    void control_rf_transmit(bool state_switch);
    void select_mifare_card();
    // void read_data_block();
    // void write_data_block();
    // void inti_value_block();
    // void read_value_block();
    // void increment_value();
    // void decrement_value();

};






#endif
