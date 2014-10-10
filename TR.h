#ifndef __TR_H
#define __TR_H
void dumpRF();

#define CMD_TX 0x9

void initRF();
void sendPacket( char *data, int len );
#endif
