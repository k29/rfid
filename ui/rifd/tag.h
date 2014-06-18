#ifndef TAG_H
#define TAG_H

//contains all the actions that can be used on the tag using MF-800

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "serial.h"
#include <QDebug>

typedef unsigned char byte;
using namespace HAL;
using namespace std;

class Tag_Actions
{
  private:
    
    char key_type;
    byte block;
    byte key[6];
    byte data_write[16];
    byte data_read[16];
    byte value[4];

    void checksum(); //XOR from len to data i.e packet[2] to packet[len+1]
    bool isChecksum();
    void packet_reset(); //initialises packet header and sets the rest to zero;
    void Read_rfid();
    void packet_received_reset();

  public:
    Tag_Actions()
    {
        packet_reset();
        packet_received_reset();
    }

    byte packet[30];
    byte packet_received[30];

    Serial serial;

    void control_rf_transmit(bool state_switch);
    void select_mifare_card();
    void read_data_block(char key_type, byte block, byte *key);
    void write_data_block(char key_type, byte block, byte *key, byte *data_write);
    void init_value_block(char key_type, byte block, byte *key, byte *value);
    void read_value_block(char key_type, byte block, byte *key);
    void increment_value(char key_type, byte block, byte *key, byte *value);
    void decrement_value(char key_type, byte block, byte *key, byte *value);

};







#endif // TAG_H
