#include "tag.h"

void Tag_Actions::checksum()
{
  byte s=packet[2];
  for(int i=3;i<packet[2]+2;i++)
    s^=packet[i];
  packet[packet[2]+2]=s;
}


void Tag_Actions::packet_reset()
{
  for(int i=0;i<30;i++)
    packet[i]=0;
  packet[0]=0xAA;
  packet[1]=0xBB;
}


void Tag_Actions::control_rf_transmit(bool state_switch)
{
  packet_reset();
  packet[2]=0x03; //length
  packet[3]=0x01; //command for RF transmit
  if(state_switch==true)
    packet[4]=0x01;
  else
    packet[4]=0x00;

  checksum();
  //packet[5]=0x03;
  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }
  cout<<"\nReading...\n";

  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }
  cout<<"Hence the packet received is:\n";
  for(int i=0;i<packet_received[2]+3;i++)
    printf("%x\t",packet_received[i]);

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::select_mifare_card()
{
  packet_reset();
  packet[2]=0x02;
  packet[3]=0x10;
  checksum();

  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }

  cout<<"\nReading...\n";
  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }
  cout<<"Hence the packet received is:\n";
  for(int i=0;i<packet_received[2]+3;i++)
    printf("%x\t",packet_received[i]);

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::read_data_block(char key_type, byte block, byte *key) //key: 'a' or 'b'
{
  packet[2]=0x0A; // length=10
  packet[3]=0x11; // command
  if(key_type=='a' || 'A')
    packet[4]=0x00;
  else if(key_type=='b' || 'B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<12;i++)
    packet[i]=key[i-6];
  checksum();

  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }

  cout<<"\nReading...\n";
  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }
  cout<<"Hence the packet received is:\n";
  for(int i=0;i<packet_received[2]+3;i++)
    printf("%x\t",packet_received[i]);

  cout<<"Thus the data in the block is: \n";
  for(int i=5;i<21;i++)
    printf("%x\n",packet_received[i]);

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::write_data_block(char key_type, byte block, byte *key, byte *data_write)
{
  packet[2]=0x1A;
  packet[3]=0x12;
  if(key_type=='a' || 'A')
    packet[4]=0x00;
  else if(key_type=='b' || 'B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<12;i++)
    packet[i]=key[i-6];
  for(int i=12;i<28;i++)
    packet[i]=data_write[i-12];
  checksum();

  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }

  cout<<"\nReading...\n";
  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }

  if(packet_received[4]==0x00)
    cout<<"Writing success!!";
  else
    cout<<"Oops!! Error in writing.";

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";

}


void Tag_Actions::init_value_block(char key_type, byte block, byte *key, byte *value)
{
    packet[2]=0x0E;
    packet[3]=0x13;
    if(key_type=='a' || 'A')
      packet[4]=0x00;
    else if(key_type=='b' || 'B')
      packet[4]=0x01;
    packet[5]=block;
    for(int i=6;i<12;i++)
      packet[i]=key[i-6];
    for(int i=12;i<16;i++)
      packet[i]=value[i-12];
    checksum();

    cout<<"Sending the packet: \n";
    for(int i=0;i<packet[2]+3;i++)
    {
      printf("%x\n",packet[i]);
      serial.WriteByte((char)packet[i]);
    }

    cout<<"\nReading...\n";
    bool flag=false;
    byte temp_packet_read;
    int i=0;
    while(1)
    {
      if(serial.Read(&temp_packet_read,1)>0)
      {
        printf("%x\n",temp_packet_read);
        packet_received[i]=temp_packet_read;
        flag=true;
      }
      if(i>2 && i==packet_received[2]+2)
        break;
    }

    if(packet_received[4]==0x00)
      cout<<"Init as value block success!!";
    else
      cout<<"Oops!! Error in writing.";

    cout<<"\n";
    if(!flag)
      cout<<"Nothing to Read...\n";
}


void Tag_Actions::read_value_block(char key_type, byte block, byte *key)
{
  packet[2]=0x0A;
  packet[3]=0x14;
  if(key_type=='a' || 'A')
    packet[4]=0x00;
  else if(key_type=='b' || 'B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<12;i++)
    packet[i]=key[i-6];
  checksum();

  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }

  cout<<"\nReading...\n";
  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }
  cout<<"Hence the packet received is:\n";
  for(int i=0;i<packet_received[2]+3;i++)
    printf("%x\t",packet_received[i]);

  cout<<"Thus the data in the block is: \n";
  for(int i=5;i<9;i++)
    printf("%x\n",packet_received[i]);


  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::increment_value(char key_type, byte block, byte *key, byte *value)
{
  packet[2]=0x0E;
  packet[3]=0x15;
  if(key_type=='a' || 'A')
    packet[4]=0x00;
  else if(key_type=='b' || 'B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<12;i++)
    packet[i]=key[i-6];
  for(int i=12;i<16;i++)
    packet[i]=value[i-12];
  checksum();

  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }

  cout<<"\nReading...\n";
  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }

  if(packet_received[4]==0x00)
    cout<<"Increment value block success!!";
  else
    cout<<"Oops!! Error in writing.";

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::decrement_value(char key_type, byte block, byte *key, byte *value)
{
  packet[2]=0x0E;
  packet[3]=0x16;
  if(key_type=='a' || 'A')
    packet[4]=0x00;
  else if(key_type=='b' || 'B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<12;i++)
    packet[i]=key[i-6];
  for(int i=12;i<16;i++)
    packet[i]=value[i-12];
  checksum();

  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+3;i++)
  {
    printf("%x\n",packet[i]);
    serial.WriteByte((char)packet[i]);
  }

  cout<<"\nReading...\n";
  bool flag=false;
  byte temp_packet_read;
  int i=0;
  while(1)
  {
    if(serial.Read(&temp_packet_read,1)>0)
    {
      printf("%x\n",temp_packet_read);
      packet_received[i]=temp_packet_read;
      flag=true;
    }
    if(i>2 && i==packet_received[2]+2)
      break;
  }

  if(packet_received[4]==0x00)
    cout<<"Decrement value block success!!";
  else
    cout<<"Oops!! Error in writing.";

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}
