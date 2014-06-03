//TOD DO: reading only improved for the rf transmit function and not for others. though this new version is not
//tested either.

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

  // int attempt=100;
  // bool flag=false;
  // while(attempt--)
  // {
  //   if(serial.Read(packet_received,1)>0)
  //   {
  //     for(int i=0;i<sizeof(packet_received);i++)
  //       printf("%x\t",packet_received[i]);
  //     flag=true;
  //   }
  // }
  // cout<<"\n";
  // cout<<"Serial number: ";
  // for(int i=5;i<9;i++)
  //   cout<<packet_received[i];
  // cout<<"\n";
  // if(packet_received[9]==0x00)
  //   cout<<"Mifare Standard 1K(S50) card\n";
  // else if(packet_received[9]==0x01)
  //   cout<<"Mifare Standard 4K(S70) card\n";
  // else if(packet_received[9]==0x02)
  //   cout<<"Mifare ProX card\n";
  // if(!flag)
  //   cout<<"Nothing to Read...\n";
}


void Tag_Actions::read_data_block(char key_a_b, byte block, byte key) //key: 'a' or 'b'
{
  packet[2]=0x0C; // length=13
  packet[3]=0x11; // command
  if(key_a_b=='a || A')
    packet[4]=0x00;
  else if(key_a_b=='b || B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<13;i++)
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
  for(int i=5;i<18;i++)
    printf("%x\n",packet_received[i]);

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::write_data_block(char key_a_b, byte block, byte key, byte data_write)
{
  packet[2]=0x1A;
  packet[3]=0x12;
  if(key_a_b=='a || A')
    packet[4]=0x00;
  else if(key_a_b=='b || B')
    packet[4]=0x01;
  packet[5]=block;
  for(int i=6;i<13;i++)
    packet[i]=key[i-6];
  for(int i=13;i<30;i++)
    packet[i]=data_write[i-13];
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
    cout<<"Oops!! Erroor in writing.";

  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";

}
