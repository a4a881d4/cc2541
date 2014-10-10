#include <ioCC2541.h>
#include "util.h"
#include "TR.h"

void main( void )
{
  int i,k;
  char buf[128];
  InitCLK();
  InitUart();
  uartSendString("Hello World\r\n");
  initRF();
  dumpRF();
  printByte("RFERRF",RFERRF);
  printByte("RFIRQF0",RFIRQF0);
  printByte("RFIRQF1",RFIRQF1);
  k=0;
  for( i=0;i<96;i++ )
      buf[i]=0x30+i;
  while(1)
  {
    sendPacket(buf,96);
    printByte("Done",k&0xff);
    k++;
    if( (k&0xf)==0 )
    {
      printByte("PRF_TX_DELAY",XREG(0x600E));
      printByte("PRF_TX_DELAY",XREG(0x600F));
    }
  }
}