#include <ioCC2541.h>
#include "util.h"
#include "TR.h"
#include "dma.h"

__xdata char xbuf[256];
void main( void )
{
  int i,k;
  
  InitCLK();
  InitUart();
  uartSendString("Hello World\r\n");
  initRF();
  dumpRF();
  printByte("RFERRF",RFERRF);
  printByte("RFIRQF0",RFIRQF0);
  printByte("RFIRQF1",RFIRQF1);
  printByte("BSP",XREG(0x61e9));
  XREG(0x61e9)=0;
  k=0;
  for( i=0;i<256;i++ )
    xbuf[i]=i;
  xbuf[0] = 255;
  XREG(0x6195) = 0x4b;
  XREG(0x6196) = 0x3e;
  XREG(0x6197) = 0x37;
  XREG(0x6198) = 0x50;
  
  resetDMARF();
  while(1)
  {
    if( DMADone()==0 )
      sendPacket(xbuf,256);
    DelayMS(10);
    uartSendString("*");
    k++;
    if( (k&0xff)==0 )
    {
      DMAReport();
      printByte("TXFIFOCNT",_XREG619C);
      printByte("TXFIRST_PTR",XREG(0x61a1));
      printByte("TXLAST_PTR",XREG(0x61a2));
      uartSendString("Done\r\n");
    }
  }
}