/*  ============================================================================
    Copyright (C) 2015 Achuthan Paramanathan.
    RF24 lib. provided by 2011 J. Coliz <maniacbug@ymail.com>
    ============================================================================
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.
    ============================================================================
    Revision Information:
        File name: vNeuron.ino
        Version:   v0.0
        Date:      04-03-2015
    ==========================================================================*/


/*============================================================================*/
/*                           INCLUDE STATEMENTS                               */
/*============================================================================*/

#include <SPI.h>
#include <commonInterface.h>

#include "SimpleTimer.h"
#include "ringBuffer.h"
#include "printf.h"


/*============================================================================*/
/*                           PRIVATE DIFINES                                  */
/*============================================================================*/

#define NPS_INTERVAL 1000 // Neuron Pulse Signal Interval


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

uint8_t src_macID  = 1;
uint8_t dest_macID = 2;

SimpleTimer npsTimer; // The Neuron Pulse Signal timer

uint16_t frameDubManager[100];

bufPar outBufPar;
bufPar inBufPar;

unsigned long seqNum = 0;

struct statistics
{
  uint8_t failed_tx;
  uint8_t successful_tx;
  
  uint8_t carrier_sensed;
  
  uint8_t total_tx;
  uint8_t total_rx;
  
  uint8_t fromNode[40];
  
  uint8_t macCDCnt;
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
  radio.setChannel(9);
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
  
  // the function is called every routine time interval
  npsTimer.setInterval(NPS_INTERVAL, neuronPulsGenerator);
  
  attachInterrupt(0, check_radio, FALLING);
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
  VDFrame frRx;
  npsTimer.run();
  
  if (!buffer_empty(&inBufPar))
  {
    read_buffer(&frRx, &inBufPar);
    printf("%d %d %d %lu %d %d\n",
    frRx.header.srcAddr, frRx.header.destAddr, frRx.header.type,
    frRx.payload.seqNum, frRx.payload.data[0], frRx.payload.data[1] );
  }
}


/*==============================================================================
** Function...: sendPayload
** Return.....: void
** Description: Send out a frame using LBT
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
int sendPayload(VDFrame fr)
{
  bool ok;
  
  radio.startListening();
  delayMicroseconds(128);
  radio.stopListening();
  while(radio.testCarrier())
  {
    #ifdef LED_DEBUG
    digitalWrite(YELLOW, HIGH);
    #endif
    radio.startListening();
    delayMicroseconds(128); // # 128uS at least to detect any carrier
    radio.stopListening();
    
    stats.carrier_sensed++;
  }
  #ifdef LED_DEBUG
  digitalWrite(YELLOW, LOW);
  #endif
  
  stats.total_tx++;
  
  ok = radio.write( &fr, sizeof(fr) );
  
  if(ok)
  {
    #ifdef LED_DEBUG
    digitalWrite(GREEN, !digitalRead(GREEN));
    #endif
  }
  else
  {
    digitalWrite(RED, !digitalRead(RED));
  }
}


/*==============================================================================
** Function...: neuronPulsGenerator
** Return.....: void
** Description: Sends out pulse to the central unit
** Created....: 18.2.2015 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void neuronPulsGenerator(void)
{
  VDFrame frTx;
  char s[100];
  int battVolts;
  
  battVolts=getBandgap();
  
  frTx.header.destAddr = dest_macID;
  frTx.header.srcAddr  = src_macID;
  frTx.header.type     = 0;
  frTx.payload.data[0] = battVolts/10;
  frTx.payload.data[1] = stats.carrier_sensed;
  frTx.payload.seqNum  = seqNum++;
  
  detachInterrupt(0);
  // Send the payload
  sendPayload(frTx);
  radio.stopListening();
  delayMicroseconds(128);
  radio.startListening();
  attachInterrupt(0, check_radio, FALLING);
  
  
}


/*==============================================================================
** Function...: check_radio
** Return.....: void
** Description: Interrupt routine handling incomming frames
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void check_radio(void)
{
  
  // if there is data ready
  if ( radio.available() )
  { 
    VDFrame frRx;
    bool done = false;
    stats.total_rx++;
    
    while (!done)
    { 
      // Fetch the frame
      done = radio.read( &frRx, sizeof(frRx) );
      
      // Keep some statistic about from whom we get frame from
      stats.fromNode[frRx.header.srcAddr]++;
      
      // Is this frame for me ?
      if(frRx.header.destAddr == src_macID)
      {
        // Is this frame same as the previous frame ?
        if(frRx.payload.seqNum^frameDubManager[frRx.header.srcAddr])
        {
          frameDubManager[frRx.header.srcAddr] = frRx.payload.seqNum;
          
          // Is inbuffer full?
          if (! buffer_full(&inBufPar))
          {
            write_buffer(&frRx, &inBufPar);
            //printf("W: %d %d %d\n",inBufPar.read_pointer, inBufPar.write_pointer, inBufPar.data_size);
          }
          else
          {
            printf("Buffer full\n");
          }
          
        }
        
      }
      
    } 
  } 
}


/*==============================================================================
** Function...: getBandgap
** Return.....: int
** Description: Reads the battery level
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
int getBandgap(void) // Returns actual value of Vcc (x 100)
{
  
  #if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // For mega boards
  const long InternalReferenceVoltage = 1115L;  // Adjust this value to your boards specific internal BG voltage x1000
  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc reference
  // MUX4 MUX3 MUX2 MUX1 MUX0  --> 11110 1.1V (VBG)         -Selects channel 30, bandgap voltage, to measure
  ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR)| (0<<MUX5) | (1<<MUX4) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  
  #else
  // For 168/328 boards
  const long InternalReferenceVoltage = 1056L;  // Adjust this value to your boards specific internal BG voltage x1000
  // REFS1 REFS0          --> 0 1, AVcc internal ref. -Selects AVcc external reference
  // MUX3 MUX2 MUX1 MUX0  --> 1110 1.1V (VBG)         -Selects channel 14, bandgap voltage, to measure
  ADMUX = (0<<REFS1) | (1<<REFS0) | (0<<ADLAR) | (1<<MUX3) | (1<<MUX2) | (1<<MUX1) | (0<<MUX0);
  
  #endif
  delay(50);  // Let mux settle a little to get a more stable A/D conversion
  // Start a conversion
  ADCSRA |= _BV( ADSC );
  // Wait for it to complete
  while( ( (ADCSRA & (1<<ADSC)) != 0 ) );
  // Scale the value
  int results = (((InternalReferenceVoltage * 1024L) / ADC) + 5L) / 10L; // calculates for straight line value
  return results;
  
}
