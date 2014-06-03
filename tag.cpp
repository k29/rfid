Tag_Actions::Tag_Actions()
{
  packet_reset();
}


void Tag_Actions::checksum(byte a[])
{
  byte s=a[2];
  for(int i=3;i<a[2]+1;i++)
    s^=i;
  a[a[2]+2]=s;
}


void Tag_Actions::packet_reset()
{
  packet[0]=0xAA;
  packet[1]=0xBB;
  for(int i=0;i<30;i++)
    packet[i]=0;
}


void Tag_Actions::control_rf_transmit(bool switch)
{
  packet_reset();
  packet[2]=0x03; //length
  packet[3]=0x01; //command for RF transmit
  if(switch==true)
    packet[4]=0x01;
  else
    packet[4]=0x00;
  checksum(packet);
  cout<<"Sending the packet: \n";
  for(int i=0;i<packet[2]+2;i++)
  {
    cout<<packet[i]<<"\t";
    serial.WriteByte((char)packet[i]);
  }
  cout<<"\nReading...";
  int attempt=100;
  bool flag=false;
  while(attempt--)
  {
    if(Read(packet_received,1)>0)
    {
      for(int i=0;i<sizeof(packet_received);i++)
        printf("%x\t",packet_received[i]);
      flag=true;
    }
  }
  cout<<"\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}


void Tag_Actions::select_mifare_card()
{
  packet_reset();
  packet[2]=0x02;
  packet[3]=0x10;
  checksum(packet);

  int attempt=100;
  bool flag=false;
  while(attempt--)
  {
    if(Read(packet_received,1)>0)
    {
      for(int i=0;i<sizeof(packet_received);i++)
        printf("%x\t",packet_received[i]);
      flag=true;
    }
  }
  cout<<"\n";
  cout<<"Serial number: ";
  for(int i=5;i<9;i++)
    cout<<packet_received[i];
  cout<<"\n";
  if(packet_received[9]==0x00)
    cout<<"Mifare Standard 1K(S50) card\n";
  else if(packet_received[9]==0x01)
    cout<<"Mifare Standard 4K(S70) card\n";
  else if(packet_received[9]==0x02)
    cout<<"Mifare ProX card\n";
  if(!flag)
    cout<<"Nothing to Read...\n";
}
