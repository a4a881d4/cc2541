#include <ioCC2541.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
/****************************************************************************
* ��    ��: DelayMS()
* ��    ��: �Ժ���Ϊ��λ��ʱ��ϵͳʱ�Ӳ�����ʱĬ��Ϊ16M(��ʾ���������൱��ȷ)
* ��ڲ���: msec ��ʱ������ֵԽ����ʱԽ��
* ���ڲ���: ��
****************************************************************************/
void DelayMS(uint msec)
{ 
    uint i,j;
    
    for (i=0; i<msec; i++)
        for (j=0; j<535; j++);
}

/****************************************************************************
* ��    ��: InitCLK()
* ��    ��: ����ϵͳʱ��Դ
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void InitCLK()
{
    CLKCONCMD &= ~0x40;             //����ϵͳʱ��ԴΪ32MHZ����
    while(CLKCONSTA & 0x40);        //�ȴ������ȶ�Ϊ32M
    CLKCONCMD &= ~0x47;             //����ϵͳ��ʱ��Ƶ��Ϊ32MHZ   
}

/****************************************************************************
* ��    ��: InitUart()
* ��    ��: ���ڳ�ʼ������
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void InitUart()
{
    PERCFG = 0x00;                  //λ��1 P0��
    P0SEL = 0x0c;                   //P0��������
    P2DIR &= ~0xc0;                 //P0������ΪUART0 
    U0CSR |= 0x80;                  //��������ΪUART��ʽ
    U0GCR |= 14;
    U0BAUD |= 216;                  //��������Ϊ115200
    U0CSR |= 0x40;                  //UART������ʹ��
    UTX0IF = 0;                     //UART0 TX�жϱ�־��ʼ��λ0
}

/****************************************************************************
* ��    ��: UartSendString()
* ��    ��: ���ڷ��ͺ���
* ��ڲ���: Data:���ͻ�����   len:���ͳ���
* ���ڲ���: ��
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
* ��    ��: uartSendString()
* ��    ��: ���ڷ��ͺ���
* ��ڲ���: Data:���ͻ�����, 0 for end
* ���ڲ���: ��
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
