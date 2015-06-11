/*  ============================================================================
Copyright (C) 2015 Achuthan Paramanathan.
RF24 lib. provided by 2011 J. Coliz <maniacbug@ymail.com>
============================================================================
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation.
============================================================================
Revision Information:
File name: node.ino
Version:   v0.0
Date:      04-03-2015
==========================================================================*/

/*============================================================================*/
/*                           INCLUDE STATEMENTS                               */
/*============================================================================*/
#include "DHT.h"
#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "printf.h"
#include "commonInterface.h"


/*============================================================================*/
/*                           PRIVATE DIFINES                                  */
/*============================================================================*/
//#define DEBUG
//#define LED_DEBUG
#define SLEEP_MODE
#define DHT_SENSOR


#define DHT_ON_PIN 3

#define DHTPIN A2
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


/*============================================================================*/
/*                           Hardware configuration                           */
/*============================================================================*/
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// store the state of register ADCSRA
byte keep_ADCSRA;

#ifdef DHT_SENSOR
// Initialize DHT sensor for internal 8mhz Arduino
DHT dht(DHTPIN, DHTTYPE, 3);
#endif


/*============================================================================*/
/*                           LOCAL VARIABLES                                  */
/*============================================================================*/
unsigned long seqNum = 0;

const short sleep_cycles_per_transmission = 1;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;

struct statistics
{
  uint16_t failed_tx;
  uint16_t successful_tx;
};

statistics stats;

int battVolts;

/*============================================================================*/
/*                   LOCAL EXPORTED FUNCTION DECLARATIONS                     */
/*============================================================================*/
void setup_watchdog(uint8_t prescalar);
void do_sleep(void);
int sendPayload(VDFrame fr);


/*==============================================================================
** Function...: setup
** Return.....: void
** Description: Initial setup
** Created....: 28.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/
void setup()
{

  
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
  setup_watchdog(wdt_8s);
  // Start the transceiver
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
  // Set the channel
  radio.setChannel(90);
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

  #ifdef LED_DEBUG
  //LED pin configuration
  pinMode(GREEN, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  #endif
  
  pinMode(DHT_ON_PIN,OUTPUT);
  
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
  float t = 0, h = 0;
  // Data frame
  VDFrame fr;
  digitalWrite(DHT_ON_PIN, HIGH);
  delay(200);
  
  seqNum++;

  battVolts=getBandgap();  //Determins what actual Vcc is, (X 100), based on known bandgap voltage
  
  #ifdef DHT_SENSOR
  dht.begin();
  delay(300);
  t = dht.readTemperature();
  h = dht.readHumidity();
  #endif
  
  fr.header.destAddr = BS_MAC_ID;
  fr.header.srcAddr  = 3;
  fr.header.type     = 6;
  fr.payload.data[0] = battVolts/10;
  fr.payload.data[1] = (uint8_t) t;
  fr.payload.seqNum = seqNum;

  // Send the payload
  sendPayload(fr);

  #ifdef SLEEP_MODE
  pinMode(DHT_ON_PIN,INPUT);
  digitalWrite(DHT_ON_PIN, LOW);
  // turn off the radio and go to sleep
  radio.powerDown();
  while( sleep_cycles_remaining )
  {
    do_sleep();
  }    
  sleep_cycles_remaining = sleep_cycles_per_transmission;
  radio.powerUp();
  pinMode(DHT_ON_PIN,OUTPUT);
  #endif
}


/*==============================================================================
** Function...: sendPayload
** Return.....: void
** Description: main function
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
  }
  #ifdef LED_DEBUG
  digitalWrite(YELLOW, LOW);
  #endif
  
  
  ok = radio.write( &fr, sizeof(fr) );
  
  if(ok)
  {
    #ifdef LED_DEBUG
    digitalWrite(GREEN, !digitalRead(GREEN));
    #endif
    stats.successful_tx++;
  }
  else
  {
    digitalWrite(RED, !digitalRead(RED));
    stats.failed_tx++;
  }
  
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
  
    // turn off brown-out enable in software
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS);
    
  keep_ADCSRA  = ADCSRA;
  // disable ADC
  ADCSRA = 0;

  sleep_cpu();                // System sleeps here

  // enable ADC
  ADCSRA = keep_ADCSRA;
  
  sleep_disable();  // System continues execution here when watchdog timed out
}

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
