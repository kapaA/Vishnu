#include <SPI.h>
#include <commonInterface.h>

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
  delay(20);
  printf_begin();
  delay(1000);

  radio.begin();
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
    // Dump the payloads until we've gotten everything
    unsigned long got_time;
    bool done = false;
    while (!done)
    {
      // Fetch the payload, and see if this was the last one.
      done = radio.read( &got_time, sizeof(unsigned long) );

      // Spew it.  Include our time, because the ping_out millis counter is unreliable
      // due to it sleeping
      printf("Got payload %lu @ %lu...\n",got_time,millis());
    }
    // First, stop listening so we can talk
    radio.stopListening();

    // Send the final one back.
    radio.write( &got_time, sizeof(unsigned long) );
    printf("Sent response.\n\r");

    // Now, resume listening so we catch the next packets.
    radio.startListening();
  }
}