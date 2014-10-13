#ifndef __DMA_H
#define __DMA_H

void setDMATask( char *src, int len );
void DMAReport();
int DMALeft();
int DMADone();
void resetDMARF();

#endif
