#include <ioCC2541.h>
#include "util.h"
#include "phy.h"
#include "TR.h"

char packet[] = { 0x10,
'1','2','3','4','5','6','7','8','9','0','a','b','c','d','e','f' };

void initRF()
{
  int i;
  for( i=0;i<0x11;i++ )
    RFD = packet[i];
  /*
  
  Bits 0每6: FREQ
Frequency to use.
0: 2379 MHz
... 1 MHz steps
116: 2495 MHz
117每126: Reserved
127: The LLE does not program frequency; it is to
PRF_CHAN
be set up by the MCU through the FREQCTRL
and MDMTEST1 registers.

  Bit 7: SYNTH_ON
0: Turn off synthesizer when task is done.
1: Leave synthesizer running after task is done.
  
  */
  BLE_CHAN = (char)( 2400 - 2379  )+0x80;
  /*

  Bits 0每1: MODE (operation mode)

*00: Basic mode, fixed length
01: Basic mode, variable length
10: Auto mode, 9-bit header
11: Auto mode, 10-bit header

  Bit 2: REPEAT (repeated operation)
0: Single operation
*1: Repeated operation

  Bit 3: START_CONF (start configuration)
0: Start each receive/transmit immediately
1: Start each receive/transmit on Timer 2 event 1

  Bits 4每5: STOP_CONF (stop configuration)
*00: No stop based on Timer 2.
01: End task after current packet is done on Timer
2 event 2 (end immediately in sync search or wait)
10: Stop transmit/receive immediately on Timer 2
event 2
11: End task on Timer 2 event 2 in first sync
search or clear channel assessment. No stop after
first sync search or clear channel assessment.

  Bit 6: TX_ON_CC_CONF
0: Listen until RSSI drops below given level, then
start Tx.
1: End task if RSSI is above given level

  Bit 7: REPEAT_CONF
For TX_ON_CC with REPEAT = 1:
0: Listen again on repeated operation and
retransmissions
1: Listen only before the first transmission, then
transmit every time
For RX with REPEAT = 1:
0: Recalibrate the synthesizer before listening for
new packets
1: Recalibrate the synthesizer only when the task
starts
  */
  BLE_CHANMAP = 0x04;

  /*
  Bit 0: AUTOFLUSH_IGN
Keep received packets with unexpected sequence
number in the Rx FIFO.
0: Keep
*1: Auto-flush

  Bit 1: AUTOFLUSH_CRC
Keep received packets with CRC error in the Rx
FIFO.
0: Keep
*1: Auto-flush

  Bit 2: AUTOFLUSH_EMPTY
Keep packets with no payload in the Rx FIFO.
0: Keep
*1: Auto-flush

  Bit 3: RX_STATUS_CONF
Rx FIFO channel information
0: Do not append RSSI and RES
*1: Append RSSI and RES

  Bits 4每5: RX_ADDR_CONF
Rx FIFO address and config byte configuration
00: Do not include address or config byte in Rx
FIFO
01: Include received address in Rx FIFO (1-byte
addresses only), but no config byte
10: Include config byte in Rx FIFO, but no address
byte
*11: Include received address (1-byte addresses
only) and config byte in Rx FIFO

  Bits 6每7: TX_ADDR_CONF
Tx FIFO address and config byte configuration
00: No address or config byte; read address from
PRF_ADDR_ENTRY0
*01: Include address byte in Tx FIFO, no config byte
10: Include config byte and use address index in
that byte to find address from
PRF_ADDR_ENTRYn
11: Read address from Tx FIFO followed by config
byte (where address information is ignored). Not
allowed for PRF_TASK_CONF.MODE = 00 or
01.
*/  
  BLE_FIFO_CONF = 0xbf;
  /*
  Bit 0: ADDR_LEN. Number of address bytes (0 or
1).

  Bit 1: AGC_EN
0: Do not use AGC
*1: Use AGC (Section 25.9.2.1)

  Bit 2: START_TONE
0: Ordinary transmission
1: Override extra preamble bytes with tone and
reduce synthesizer calibration time accordingly
(Section 25.9.2.2)
Bits 3-7: Reserved, always write 0.
  */
  BLE_CONF = 0x2;
  
  /*
  Number of CRC bytes. Permitted values: 0每4
  */
  BLE_MAXNACK = 0;
  
  MDMCTRL0 = 0x6;
  
  LLECTRL = 1;
  
  RFST = CMD_TX;
  
}

void sendPacket( char *data, int len )
{
  int i,j;
  
  RFIRQF1 = 0;
  while( RFST!=0 );
  RFST = 0x91;
  i = XREG(0x618A);
  i = XREG(0x618B);
  XREG(0x6001) = 0x4;
  XREG(0x600E) = 0;
  XREG(0x600F) = 0;
  XREG(0x618A) = 1;
  XREG(0x618B) = 1;
  XREG(0x61D1) = 0x10;
  XREG(0x61C6) = 0x33;
  RFD = len&0xff;
  for( i=0;i<len;i++ )
  {
    RFD = data[i];
  }
  RFST = CMD_TX;
  for( j=0;j<255;j++ )
  {
    if((XREG(0x61C5)&0x20)==0)
    {
      RFD = len&0xff;
      for( i=0;i<len;i++ )
      {
        RFD = data[i];
      }
    }
    else
    {
      while((XREG(0x61C5)&0x20)!=0);
    }
  }
  while((RFIRQF1&0x40)==0);
  printByte("ENDCAUSE",XREG(0x607f));

}
void dumpRF()
{
  printByte("CHAN",BLE_CHAN);   // BLE_LSI_CHAN
  printByte("CHANMAP",BLE_CHANMAP); // BLE_A_CHANMAP
  printByte("FIFO_CONF",BLE_FIFO_CONF); // BLE_FIFO_CONF
  printByte("CONF",BLE_CONF); // BLE_L_CONF
  printByte("MAXNACK",BLE_MAXNACK); // BLE_L_MAXNACK
  
  printByte("CRCINIT_0",BLE_CRCINIT_0); // BLE_L_CRCINIT
  printByte("CRCINIT_1",BLE_CRCINIT_1); // BLE_L_CRCINIT
  printByte("CRCINIT_2",BLE_CRCINIT_2); // BLE_L_CRCINIT
  
  printByte("SEQSTAT",BLE_SEQSTAT);  // BLE_L_SEQSTAT
  
  printByte("BLACKLIST",BLE_BLACKLIST); // BLE_S_BLACKLIST
  
  printByte("BACKOFFCNT_0",BLE_BACKOFFCNT_0);  // BLE_S_BACKOFFCNT
  printByte("BACKOFFCNT_1",BLE_BACKOFFCNT_1);  // BLE_S_BACKOFFCNT
  
  printByte("SCANCONF",BLE_SCANCONF); // BLE_S_CONF
  
  printByte("WINCONF",BLE_WINCONF);  // BLE_I_WIN_CONF
/*  
  printByte("WINOFFSET_0",BLE_WINOFFSET_0);  // BLE_I_WIN_OFFSET
  printByte("WINOFFSET_1",BLE_WINOFFSET_1);  // BLE_I_WIN_OFFSET
  
  printByte("WINMOD_0",BLE_WINMOD_0);  // BLE_I_WIN_MOD
  printByte("WINMOD_1",BLE_WINMOD_1);  // BLE_I_WIN_MOD
  
  printByte("MAX_LEN",BLE_MAX_LEN);  // BLE_MAX_LEN
  printByte("LEN_BIT_MASK",BLE_LEN_BIT_MASK);  // BLE_LEN_BIT_MASK
  
  printByte("WLPOLICY",BLE_WLPOLICY);  // BLE_ASI_WLPOLICY
  printByte("ADDRTYPE",BLE_ADDRTYPE);  // BLE_ASI_ADDRTYPE
  
  printByte("OWNADDR_0",BLE_OWNADDR_0);  // BLE_ASI_OWNADDR
  printByte("OWNADDR_1",BLE_OWNADDR_1);  // BLE_ASI_OWNADDR
  printByte("OWNADDR_2",BLE_OWNADDR_2);  // BLE_ASI_OWNADDR
  printByte("OWNADDR_3",BLE_OWNADDR_3);  // BLE_ASI_OWNADDR
  printByte("OWNADDR_4",BLE_OWNADDR_4);  // BLE_ASI_OWNADDR
  printByte("OWNADDR_5",BLE_OWNADDR_5);  // BLE_ASI_OWNADDR
  
  printByte("PEERADDR_0",BLE_PEERADDR_0);  // BLE_AI_PEERADDR
  printByte("PEERADDR_1",BLE_PEERADDR_1);  // BLE_AI_PEERADDR
  printByte("PEERADDR_2",BLE_PEERADDR_2);  // BLE_AI_PEERADDR
  printByte("PEERADDR_3",BLE_PEERADDR_3);  // BLE_AI_PEERADDR
  printByte("PEERADDR_4",BLE_PEERADDR_4);  // BLE_AI_PEERADDR
  printByte("PEERADDR_5",BLE_PEERADDR_5);  // BLE_AI_PEERADDR
  
  printByte("WLVALID",BLE_WLVALID);   // BLE_ASI_WLVALID
  printByte("WLADDRTYPE",BLE_WLADDRTYPE);  // BLE_ASI_WLADTYPE
  printByte("NTXDONE",BLE_NTXDONE);       // BLE_L_NTXDONE
  printByte("NTXACK",BLE_NTXACK);         // BLE_L_NTXACK
  printByte("NTXCTRLACK",BLE_NTXCTRLACK); // BLE_L_NTXCTRLACK
  */
  
  printByte("PRF_TX_DELAY",XREG(0x600E));
  printByte("PRF_TX_DELAY",XREG(0x600F));
  
}