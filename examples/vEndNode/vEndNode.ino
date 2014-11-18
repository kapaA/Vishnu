#include <SPI.h>
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
  
  // First, stop listening so we can talk.
  radio.stopListening();

  printf("Now sending...");
  
  bool ok = radio.write( &fr, sizeof(fr) );
  
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

  // Try again 1s later
  delay(1000);
}