#include <ioCC2541.h>
#include <stdio.h>
#include <string.h>
#include "util.h"

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
} aDMA0;

DMACONF DMA0;

void initDMARF( DMACONF *pconf )
{
  pconf->desH = 0;
  pconf->desL = 0xD9;
  pconf->mode = 11;
  pconf->misc = 0x4a; // enable intr
  RFFDMA1 = 0x16;//: Tx FIFO is read when its size equals RFTXFTHRS.
  aDMA0.head = 0x1000;
  aDMA0.left = 0;
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
  IEN1 = 1;
}

#pragma vector = 0x43
__interrupt void DMA_ISR( void )
{
  if( (DMAIRQ&1) == 1 )
  {
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
    DMAIF &= 0xfe;
  }
}

void DMAReport()
{
  char cTemp[32];
  sprintf(cTemp,"Left %d\r\n",aDMA0.left);
  uartSendString(cTemp);
}

int DMALeft()
{
  return aDMA0.left;
}