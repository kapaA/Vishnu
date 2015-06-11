/*  ============================================================================
    Copyright (C) 2015 Achuthan Paramanathan.
    RF24 lib. provided by 2011 J. Coliz <maniacbug@ymail.com>
    ============================================================================
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.
    ============================================================================
    Revision Information:
        File name: base.ino
        Version:   v0.0
        Date:      04-03-2015
    ==========================================================================*/

/*============================================================================*/
/*                           INCLUDE STATEMENTS                               */
/*============================================================================*/
#include <SPI.h>
#include "printf.h"
#include <commonInterface.h>
#include "SimpleTimer.h"


/*============================================================================*/
/*                           PRIVATE DIFINES                                  */
/*============================================================================*/

//#define PRINT_ONE_SEC

/*============================================================================*/
/*                           Hardware configuration                           */
/*============================================================================*/
// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


/*============================================================================*/
/*                           LOCAL VARIABLES                                  */
/*============================================================================*/
SimpleTimer printTimer;

uint16_t frameDubManager[100];

struct statistics
{
  uint16_t failed_tx;
  uint16_t successful_tx;
  
  uint16_t total_tx;
  uint16_t total_rx;
  
  uint16_t fromNode[40];
  
  uint16_t macCDCnt;
};

statistics stats;


/*==============================================================================
** Function...: setup
** Return.....: void
** Description: Initial setup
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void setup()
{
  Serial.begin(115200);
  delay(20);
  printf_begin();
  delay(1000);
  
  radio.begin();
  // Set the channel
  radio.setChannel(90);
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,1);
  // optionally, reduce the payload size.  seems to
  // improve reliability
  radio.setPayloadSize(9);
  // Open the pipe for reading/writing
  radio.openWritingPipe(pipes[1]);
  radio.openReadingPipe(1,pipes[0]);
  radio.startListening();
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
  
  #ifdef PRINT_ONE_SEC
    // the function is called every routineTime interval
    printTimer.setInterval(1000, printOut);
  #endif
}


/*==============================================================================
** Function...: loop
** Return.....: void
** Description: main function
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void loop(void)
{
  #ifdef PRINT_ONE_SEC
    printTimer.run();
  #endif
  
  // if there is data ready
  if ( radio.available() )
  {
    VDFrame frRx;
    // Dump the payloads until we've gotten everything
    unsigned long got_time;
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &frRx, sizeof(frRx) );
      
      if(frRx.payload.seqNum^frameDubManager[frRx.header.srcAddr])
      {
        stats.fromNode[frRx.header.srcAddr]++;
        stats.total_rx++;
        frameDubManager[frRx.header.srcAddr] = frRx.payload.seqNum;
        
        #ifndef PRINT_ONE_SEC
          printf("%d %d %d %lu %d %d\n",
          frRx.header.srcAddr, frRx.header.destAddr, frRx.header.type,
          frRx.payload.seqNum, frRx.payload.data[0], frRx.payload.data[1] );
        #endif
      }
    }
  }
}


/*==============================================================================
** Function...: printOut
** Return.....: void
** Description: main function
** Created....: 18.2.2015 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void printOut(void)
{
  char s[100];
  
  
  sprintf(s," %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d | %d", 
     stats.fromNode[1],  stats.fromNode[2],  stats.fromNode[3],  stats.fromNode[4],
     stats.fromNode[5],  stats.fromNode[6],  stats.fromNode[7],  stats.fromNode[8],
     stats.fromNode [9], stats.fromNode[10], stats.fromNode[11], stats.fromNode[12],
    stats.fromNode[13], stats.fromNode[14], stats.fromNode[15], stats.fromNode[16],
    stats.total_rx);
  Serial.println(s);
  
  
  stats.fromNode[1] = 0;
  stats.fromNode[2] = 0;
  stats.fromNode[3] = 0;
  stats.fromNode[4] = 0;
  stats.fromNode[5] = 0;
  stats.fromNode[6] = 0;
  stats.fromNode[7] = 0;
  stats.fromNode[8] = 0;
  stats.fromNode[9] = 0;
  stats.fromNode[10] = 0;
  stats.fromNode[11] = 0;
  stats.fromNode[12] = 0;
  stats.fromNode[13] = 0;
  stats.fromNode[14] = 0;
  stats.fromNode[15] = 0;
  stats.fromNode[16] = 0;
  
  stats.total_rx = 0;
}