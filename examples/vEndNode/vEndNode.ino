/*  ============================================================================
    Copyright (C) 2014 - 2016 Achuthan
    All rights reserved.
    ============================================================================
    This document contains proprietary information belonging to Achuthan.
    Passing on and copying of this document, use and
    communication of its contents is not permitted without prior written
    authorization.
    ============================================================================
    Revision Information:
        File name: vEndNode.ino
        Version:   v0.1
        Date:      2014-11-28
    ============================================================================
*/

/*
** =============================================================================
**                   INCLUDE STATEMENTS
** =============================================================================
*/
#include "DHT.h"
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "printf.h"
#include "commonInterface.h"


/*
** =============================================================================
**                   DEFINES
** =============================================================================
*/

#define DEBUG

#define DHTPIN 3
#define DHTTYPE DHT21   // DHT 21 (AM2301)

/*
** =============================================================================
**                   LOCAL VARIABELS  
** =============================================================================
*/

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The role of the current running sketch
role_e role;

unsigned long seqNum = 0;

const short sleep_cycles_per_transmission = 2;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;

// store the state of register ADCSRA
byte keep_ADCSRA;

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

/*
** =============================================================================
**                   LOCAL EXPORTED FUNCTION DECLARATIONS
** =============================================================================
*/
void setup_watchdog(uint8_t prescalar);
void do_sleep(void);


/*==============================================================================
** Function...: setup
** Return.....: void
** Description: Initial setup
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void setup()
{
  
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS);
  
  //Start Serial communication
  Serial.begin(57600);
  delay(20);
  //Start the DHT sensor
  dht.begin();
  //Printf helper function
  printf_begin();
  delay(1000);
  
  //Setup watchdog to interrupt every 1 sec.
  setup_watchdog(wdt_1s);
  
  // Start the transceiver   
  radio.begin();
  // set the delay between retries & # of retries
  radio.setRetries(15,15);
  // fixed payload size of 5 bytes
  radio.setPayloadSize(5);
  //Open read/write pipelines
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  //Start the listening
  radio.startListening();

  #ifdef DEBUG
    radio.printDetails();
  #endif
}


/*==============================================================================
** Function...: loop
** Return.....: void
** Description: main function
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void loop()
{
  // Data frame
  VDFrame fr;
  
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  fr.header.destAddr = BS_MAC_ID;
  fr.header.srcAddr  = 0x03;//config.mac_addr;
  fr.header.type     = 1;
  fr.payload.data[0] = (uint8_t) h ;
  fr.payload.data[1] = (uint8_t) t ;
  seqNum++;

  #ifdef DEBUG
    printf("Now sending...\n");
    Serial.print("Humidity: ");
    Serial.print(fr.payload.data[0]);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(fr.payload.data[1]);
    Serial.println(" *C ");
  #endif
  
  // First, stop listening so we can talk.
  radio.stopListening();
  
  // Send the payload
  bool ok = radio.write( &fr, sizeof(fr) );
  
  
  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  while ( ! radio.available() && ! timeout )
  if (millis() - started_waiting_at > 250 )
  timeout = true;

  // Describe the results
  if ( timeout )
  {
    printf("Failed, response timed out.\n\r");
  }
  
  
  // turn off the radio and go to sleep
  radio.powerDown();
  while( sleep_cycles_remaining )
    do_sleep();
  sleep_cycles_remaining = sleep_cycles_per_transmission;
}


/*==============================================================================
** Function...: setup
** Return.....: void
** Description: watchdog  setup. 
**              Arguments: 
**              0=16ms, 1=32ms,2=64ms,3=125ms,4=250ms,5=500ms
**              6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void setup_watchdog(uint8_t prescalar)
{
  prescalar = min(9,prescalar);
  uint8_t wdtcsr = prescalar & 7;
  if ( prescalar & 8 )
  wdtcsr |= _BV(WDP3);

  MCUSR &= ~_BV(WDRF);
  WDTCSR = _BV(WDCE) | _BV(WDE);
  WDTCSR = _BV(WDCE) | wdtcsr | _BV(WDIE);
}


/*==============================================================================
** Function...: ISR
** Return.....: void
** Description: Watchdog timer interrupt service
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
ISR(WDT_vect)
{
  --sleep_cycles_remaining;
}


/*==============================================================================
** Function...: do_sleep
** Return.....: void
** Description: prepare sleep
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void do_sleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();
  
  keep_ADCSRA  = ADCSRA;
  // disable ADC
  ADCSRA = 0;

  sleep_mode();                // System sleeps here

  // enable ADC
  ADCSRA = keep_ADCSRA;
  
  sleep_disable();  // System continues execution here when watchdog timed out
}