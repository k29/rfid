void Tag_Actions::checksum(byte a[])
{
  byte s=a[2];
  for(int i=3;i<a[2]+1;i++)
    s^=i;
  a[a[2]+2]=s;
}
