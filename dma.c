#include <ioCC2541.h>
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "TR.h"

typedef struct DMAConfigure {
  char srcH;
  char srcL;
  char desH;
  char desL;
  char lenH;
  char lenL;
  char mode;
  char misc;
} DMACONF;

struct DMATaskPending {
  int left;
  int head;
  char done;
};

__xdata struct DMATaskPending aDMA0;

__xdata DMACONF DMA0;

void resetDMARF()
{
  aDMA0.head = 0x1000;
  aDMA0.left = 0;
  aDMA0.done = 0;
}
void initDMARF( DMACONF *pconf )
{
  pconf->desH = 0x70;
  pconf->desL = 0xD9;
  pconf->mode = 0x20|11;
  pconf->misc = 0x4a; // enable intr
  RFFDMA1 = 0x16;//: Tx FIFO is read when its size equals RFTXFTHRS.
  resetDMARF();
}

void setDMARF( DMACONF *pconf, int source, char len )
{
  pconf->srcH = source >> 8;
  pconf->srcL = source & 0xff;
  pconf->lenH = 0;
  pconf->lenL = len&0xff;
  DMA0CFGH = (int)pconf >> 8;
  DMA0CFGL = (int)pconf & 0xff;
  DMAARM |= 1;
}


void setDMATask( char *src, int len )
{
  initDMARF( &DMA0 );
  aDMA0.left = len;
  aDMA0.head = (int)src;
  aDMA0.done = 2;
  if( aDMA0.left>0x40 )
  {
    setDMARF( &DMA0, aDMA0.head, 0x40 );
    aDMA0.head += 0x40;
    aDMA0.left -= 0x40;
  }
  else
  {
    setDMARF( &DMA0, aDMA0.head, aDMA0.left );
    aDMA0.head += aDMA0.left;
    aDMA0.left = 0;
  }
  DMAREQ |= 1;
  DMAIE = 1;
  EA = 1;
  
}

#pragma vector = DMA_VECTOR
__interrupt void DMA_ISR( void )
{
  if( (DMAIRQ&1) == 1 )
  {
    if( aDMA0.done == 2 )
    {
      //dump( (int)(&aDMA0),5);
      //dump( (int)(&DMA0),8);
      while( RFST!=0 );
      RFST = CMD_TX;
      aDMA0.done = 1;
    }
    if( aDMA0.left>0 )
    {
      if( aDMA0.left>0x40 )
      {
        setDMARF( &DMA0, aDMA0.head, 0x40 );
        aDMA0.head += 0x40;
        aDMA0.left -= 0x40;
      }
      else
      {
        setDMARF( &DMA0, aDMA0.head, aDMA0.left );
        aDMA0.head += aDMA0.left;
        aDMA0.left = 0;
      }
    }
    else
      aDMA0.done = 0;
    DMAIRQ &= 0xfe;
  }
  DMAIF = 0;
}

void DMAReport()
{
  char cTemp[32];
  sprintf(cTemp,"Left %d\r\n",aDMA0.left);
  uartSendString(cTemp);
  dump( (int)(&aDMA0),5);
  dump( (int)(&DMA0),8);
  printByte("DMAIF",DMAIF);
  printByte("DMAIRQ",DMAIRQ);
  printByte("DMAIE",DMAIE);
  printByte("DMAARM",DMAARM);
  printByte("DMA0CFGH",DMA0CFGH);
  printByte("DMA0CFGL",DMA0CFGL);
  printByte("DMAREQ",DMAREQ);
}


int DMALeft()
{
  return aDMA0.left;
}

int DMADone()
{
  return aDMA0.done;
}