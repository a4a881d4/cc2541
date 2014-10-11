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
  k=0;
  for( i=0;i<256;i++ )
    xbuf[i]=i;
  xbuf[0] = 255;
  while(1)
  {
    sendPacket(xbuf,256);
    DelayMS(1200);
    DMAReport();
    printByte("TXFIFOCNT",_XREG619C);
    
    printByte("TXFIRST_PTR",XREG(0x61a1));
    printByte("TXLAST_PTR",XREG(0x61a2));
    
    k++;
    if( (k&0xff)==0 )
    {
      uartSendString("Done\r\n");
    }
  }
}