#include <SPI.h>
#include "printf.h"
#include <commonInterface.h>

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The role of the current running sketch
role_e role;

void setup()
{
  role = role_base_station;
  Serial.begin(57600);
  delay(20);
  printf_begin();
  delay(1000);
  radio.begin();
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
}

void loop(void)
{
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
      // Spew it.  Include our time, because the ping_out millis counter is unreliable
      // due to it sleeping
      printf("%d %d %d %d %lu\n", frRx.header.srcAddr, frRx.header.type, frRx.payload.data[0], frRx.payload.data[1], frRx.payload.seqNum);
    }
  }
}