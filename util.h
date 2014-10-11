#ifndef __UTIL_H
#define __UTIL_H

typedef unsigned char uchar;
typedef unsigned int  uint;

void DelayMS(uint msec);
void InitCLK();
void InitUart();
void UartSendString(char *Data, int len);
void uartSendString(char *Data);
void printByte( char *str, char A );
void dump( int addr, int len );

#endif
