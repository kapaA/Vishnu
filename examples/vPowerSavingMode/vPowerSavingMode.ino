#include <SPI.h>
#include <EEPROM.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "printf.h"
#include "commonInterface.h"

#define SCPT 1 // corresponds to 24 min sleep cycle

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The role of the current running sketch
role_e role;

void setup_watchdog(uint8_t prescalar);
void do_sleep(void);
void work_time(void);

uint8_t sendFrame(pload *data);


const short sleep_cycles_per_transmission = SCPT;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;


configuration config;
static uint8_t configFlags = 0;

void setup()
{
  readEeprom();
  systemConfig();
  
  // turn off brown-out enable in software
  MCUCR = _BV (BODS) | _BV (BODSE);
  MCUCR = _BV (BODS);

  role = role_end_node;

  Serial.begin(57600);
  delay(20);
  printf_begin();
  delay(1000);

  radio.begin();
  
  // enable dynamic payloads
  // radio.enableDynamicPayloads();
  // optionally, increase the delay between retries & # of retries
  //radio.setRetries(15,15);

  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  
  // Prepare sleep parameters
  setup_watchdog(wdt_1s);
}

void systemConfig(void)
{
  if ((configFlags&VFLAG_FRAME_MODE)==0)
  {
    config.frame_mode = FRAME_SHORT_MODE;
  }
  if((configFlags&VFLAG_ROLE)==0 )
  {
    config.rolde_id = 0xff;
  }
  if ((configFlags&VFLAG_MAC_ADD)==0)
  {
    config.mac_addr = UNDEFINED_MAC_ID;
  }
}

void readEeprom(void)
{
  config.frame_mode = EEPROM.read(EEPROM_FRAME_MODE);
  config.rolde_id   = EEPROM.read(EEPROM_ROLE);
  config.mac_addr   = EEPROM.read(EEPROM_MAC_ADD);
  
  if (FRAME_SHORT_MODE == config.frame_mode ||
  FRAME_LONGE_MODE == config.frame_mode)
  {
    configFlags = configFlags|VFLAG_FRAME_MODE;
  }
  
  if( UNDEFINED_MAC_ID < config.mac_addr && config.mac_addr <= BS_MAC_ID)
  {
    configFlags = configFlags|VFLAG_MAC_ADD;
  }
  
  if (role_end_node == config.rolde_id || role_base_station == config.rolde_id )
  {
    configFlags = configFlags|VFLAG_ROLE;
  }
  
}

void loop()
{
  pload pl;
  uint8_t i;
  
  pl.data[0] = 24;
  pl.data[1] = 64;
  
  
  i = sizeof(pl);
  printf("size %d\n",i);
  
  work_time();

  while( sleep_cycles_remaining )
  do_sleep();

  sleep_cycles_remaining = sleep_cycles_per_transmission;
  
  
  
  sendFrame(&pl);
}

uint8_t sendFrame(pload *data)
{
  network_interface(OUTGOING, data);
}


void work_time(void)
{
  // First, stop listening so we can talk.
  radio.stopListening();
  // Take the time, and send it.  This will block until complete
  unsigned long time = millis();
  printf("Now sending %lu...\n",time);
  radio.write( &time, sizeof(unsigned long) );
  
  // Now, listening for response
  radio.startListening();
  
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
  else
  {
    // Grab the response, compare, and send to debugging spew
    unsigned long got_time;
    radio.read( &got_time, sizeof(unsigned long) );

    // Spew it
    printf("Got response %lu, round-trip delay: %lu\n\r",got_time,millis()-got_time);
  }
  
  
  // Power down the radio.
  radio.powerDown();
}

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