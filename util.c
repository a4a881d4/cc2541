#include <ioCC2541.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
/****************************************************************************
* 名    称: DelayMS()
* 功    能: 以毫秒为单位延时，系统时钟不配置时默认为16M(用示波器测量相当精确)
* 入口参数: msec 延时参数，值越大，延时越久
* 出口参数: 无
****************************************************************************/
void DelayMS(uint msec)
{ 
    uint i,j;
    
    for (i=0; i<msec; i++)
        for (j=0; j<535; j++);
}

/****************************************************************************
* 名    称: InitCLK()
* 功    能: 设置系统时钟源
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitCLK()
{
    CLKCONCMD &= ~0x40;             //设置系统时钟源为32MHZ晶振
    while(CLKCONSTA & 0x40);        //等待晶振稳定为32M
    CLKCONCMD &= ~0x47;             //设置系统主时钟频率为32MHZ   
}

/****************************************************************************
* 名    称: InitUart()
* 功    能: 串口初始化函数
* 入口参数: 无
* 出口参数: 无
****************************************************************************/
void InitUart()
{
    PERCFG = 0x00;                  //位置1 P0口
    P0SEL = 0x0c;                   //P0用作串口
    P2DIR &= ~0xc0;                 //P0优先作为UART0 
    U0CSR |= 0x80;                  //串口设置为UART方式
    U0GCR |= 14;
    U0BAUD |= 216;                  //波特率设为115200
    U0CSR |= 0x40;                  //UART接收器使能
    UTX0IF = 0;                     //UART0 TX中断标志初始置位0
}

/****************************************************************************
* 名    称: UartSendString()
* 功    能: 串口发送函数
* 入口参数: Data:发送缓冲区   len:发送长度
* 出口参数: 无
****************************************************************************/
void UartSendString(char *Data, int len)
{
    uint i;
    
    for(i=0; i<len; i++)
    {
        U0DBUF = *Data++;
        while(UTX0IF == 0);
        UTX0IF = 0;
    }
}

/****************************************************************************
* 名    称: uartSendString()
* 功    能: 串口发送函数
* 入口参数: Data:发送缓冲区, 0 for end
* 出口参数: 无
****************************************************************************/
void uartSendString( char *Data )
{
    while(*Data)
    {
        U0DBUF = *Data++;
        while(UTX0IF == 0);
        UTX0IF = 0;
    }
}

void printByte( char *str, char A )
{
  char strTemp[8];
  uartSendString( str );
  sprintf(strTemp,":0x%x\r\n",((int)A)&0xff);
  uartSendString(strTemp);
}

void dump( int addr, int len )
{
  int i;
  char cTemp[32];
  char *p = (char *)addr;
  sprintf(cTemp,"0x%x:\r\n",addr);
  uartSendString(cTemp);
  for( i=0;i<len;i++ )
  {
    sprintf(cTemp,"%x ",p[i]);
    uartSendString(cTemp);
  }
  uartSendString("\r\n");

}
