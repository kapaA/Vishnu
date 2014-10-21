#include <SPI.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "commonInterface.h"

#define SCPT 180 // corresponds to 24 min sleep cycle

// Set up nRF24L01 radio on SPI bus plus pins 9 & 10
RF24 radio(9,10);

// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };

// The various roles supported by this sketch
typedef enum { role_end_node = 1, role_base_station } role_e;	
	
// The role of the current running sketch
role_e role;

void setup_watchdog(uint8_t prescalar);
void do_sleep(void);
void work_time(void);

const short sleep_cycles_per_transmission = SCPT;
volatile short sleep_cycles_remaining = sleep_cycles_per_transmission;

void setup()
{		
	// turn off brown-out enable in software
	MCUCR = _BV (BODS) | _BV (BODSE);
	MCUCR = _BV (BODS);
	
	role = role_end_node;
	
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
	
	
	// Prepare sleep parameters
	setup_watchdog(wdt_8s);
}

void loop()
{
	work_time();
	
	while( sleep_cycles_remaining )
		do_sleep();
		
	sleep_cycles_remaining = sleep_cycles_per_transmission;
}

void work_time(void)
{
	// First, stop listening so we can talk.
	radio.stopListening();
	// Take the time, and send it.  This will block until complete
	unsigned long time = millis();
	printf("Now sending %lu...",time);
	radio.write( &time, sizeof(unsigned long) );
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