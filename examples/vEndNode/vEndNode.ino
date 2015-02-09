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
 //#define DHT_SENSOR 

#define DHTPIN A2
//#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

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

const short sleep_cycles_per_transmission = 1; 
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;

// store the state of register ADCSRA
byte keep_ADCSRA;

#ifdef DHT_SENSOR
  // Initialize DHT sensor for internal 8mhz Arduino
  DHT dht(DHTPIN, DHTTYPE, 3);
#endif 


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
  
  #ifdef DEBUG
    //Start Serial communication
    Serial.begin(57600);
    delay(20);
    //Printf helper function
    printf_begin();
    delay(1000);
  #endif
  
  #ifdef DHT_SENSOR
    //Start the DHT sensor
    dht.begin();
  #endif
  
  
  //Setup watchdog to interrupt every 1 sec.
  setup_watchdog(wdt_1s);
  
  // Start the transceiver   
  radio.begin();
  // set the delay between retries & # of retries
  radio.setRetries(15,15);
  // fixed payload size of 9 bytes
  radio.setPayloadSize(9);
  //Open read/write pipelines
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  //Start the listening
  radio.startListening();

  #ifdef DEBUG
    radio.printDetails();
  #endif
  
  //pinMode(7, OUTPUT);
  
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
  seqNum++;
  
  // Data frame
  VDFrame fr;
  
  float t = 0, h = 0;
  
 // digitalWrite(7, HIGH); 
  //delay(210);
  
  #ifdef DHT_SENSOR
    dht.begin();
    delay(300);
    t = dht.readTemperature();
    h = dht.readHumidity();
  #endif
  
  //digitalWrite(7, LOW); 
  
  fr.header.destAddr = BS_MAC_ID;
  fr.header.srcAddr  = 0x04;//config.mac_addr;
  fr.header.type     = 1;
  fr.payload.data[0] = (uint8_t) h ;
  fr.payload.data[1] = (uint8_t) t ;
  fr.payload.seqNum = seqNum;

  #ifdef DEBUG
    printf("Now sending...\n");
    Serial.print("Humidity: ");
    Serial.print(fr.payload.data[0]);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(fr.payload.data[1]);
    Serial.print(" *C ");
    Serial.print("Cnt ");
    Serial.print(fr.payload.seqNum);
    Serial.println("  ");
  #endif
  
  // Send the payload
  bool ok = radio.write( &fr, sizeof(fr) );
  

  // turn off the radio and go to sleep
  radio.powerDown();
  while( sleep_cycles_remaining )
    do_sleep();
    sleep_cycles_remaining = sleep_cycles_per_transmission;
  radio.powerUp();
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
  #ifdef DEBUG
    printf("Now sleeping...\n");
  #endif
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