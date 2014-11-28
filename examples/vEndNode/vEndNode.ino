#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "printf.h"
#include "commonInterface.h"

#define DEBUG

#ifdef DEBUG
  #define GREEN  4
  #define YELLOW 5
  #define RED    6
#endif

RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The role of the current running sketch
role_e role;

unsigned long seqNum = 0;
int i =0;

const short sleep_cycles_per_transmission = 4;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;

void setup_watchdog(uint8_t prescalar);
void do_sleep(void);

void setup()
{
  
  #ifdef DEBUG
    pinMode(GREEN, OUTPUT);
    pinMode(YELLOW, OUTPUT);
    pinMode(RED, OUTPUT);
  #endif
  
  Serial.begin(57600);
  delay(20);
  printf_begin();
  delay(1000);
  
  setup_watchdog(wdt_1s);
  
  radio.begin();
  
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,15);
  radio.setPayloadSize(4);
  
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  
  radio.startListening();

  #ifdef DEBUG
    radio.printDetails();
  #endif
}

void loop()
{
  
  VDFrame fr;
  
  fr.header.destAddr = BS_MAC_ID;
  fr.header.srcAddr  = 0x01;//config.mac_addr;
  fr.header.type     = 1;
  fr.payload.data[0] = seqNum;
  fr.payload.data[1] = 24;
  seqNum++;

  printf("Now sending...");
  
  // Now, continue listening
  radio.startListening();
  
  int ca = 1;
  
  while(ca){  
    
    // Listen for a little
    radio.startListening();
    delayMicroseconds(128);
    radio.stopListening();
    
    if ( ! radio.testRPD() ) ca=0;
    
    digitalWrite(YELLOW, HIGH);
    
    };
    
  // First, stop listening so we can talk.
  radio.stopListening();
  
  bool ok = radio.write( &fr, sizeof(fr) );
  
  
  digitalWrite(YELLOW, LOW);
  // Now, continue listening
  radio.startListening();

  // Wait here until we get a response, or timeout (250ms)
  unsigned long started_waiting_at = millis();
  bool timeout = false;
  
  while ( ! radio.available() && ! timeout )
  if (millis() - started_waiting_at > 200 )
  timeout = true;

  // Describe the results
  if ( timeout )
  {
    if (i) {digitalWrite(RED, HIGH); i=0;}
    else if(0==i)  {digitalWrite(RED, LOW); i=1;}
    
  }
  else
  {
    if (i) {digitalWrite(GREEN, HIGH); i=0;}
    else if(0==i)  {digitalWrite(GREEN, LOW); i=1;}
  }

  radio.powerDown();
  while( sleep_cycles_remaining )
    do_sleep();
  sleep_cycles_remaining = sleep_cycles_per_transmission;
}

//
// Sleep helpers
//

// 0=16ms, 1=32ms,2=64ms,3=125ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec

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

ISR(WDT_vect)
{
  --sleep_cycles_remaining;
}

void do_sleep(void)
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
  sleep_enable();

  sleep_mode();                        // System sleeps here

  sleep_disable();                     // System continues execution here when watchdog timed out
}