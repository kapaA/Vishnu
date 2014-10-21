#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "commonInterface.h"

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };
  
// The various roles supported by this sketch
typedef enum { role_end_node = 1, role_base_station } role_e;
  
// The role of the current running sketch
role_e role;

void setup()
{
  role = role_base_station;
  
  Serial.begin(57600);
  delay(2000);
  
  radio.begin();
  // enable dynamic payloads
  radio.enableDynamicPayloads();
  // optionally, increase the delay between retries & # of retries
  radio.setRetries(15,1);
  // Open the pipe for reading/writing
  if ( role == role_end_node )
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }

  radio.startListening();
  
  // Dump the configuration of the rf unit for debugging
  radio.printDetails();
}

void loop(void)
{
  // if there is data ready
  if ( radio.available() )
  {
    // Dump the payloads until we've gotten everything
    uint8_t len;
    unsigned long got_time;
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      len = radio.getDynamicPayloadSize();
      done = radio.read( &got_time, len);
      
      // Spew it
      printf("Got payload %lu @ %lu...",got_time,millis());
    }    
  }
}  