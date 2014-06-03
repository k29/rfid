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
  for(int i=2;i<30;i++)
    packet[i]=0;
}


void Tag_Actions::control_rf_transmit(bool switch)
{
  packet[2]=0x03; //length
  packet[3]=0x01; //command for RF transmit
  if(switch==true)
    packet[4]=0x01;
  else
    packet[4]=0x00;
  checksum(packet);
}
