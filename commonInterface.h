
#ifdef ARDUINO
	#include "nRF24L01.h"
	#include "RF24.h"
	#include "printf.h"
	#include <RF24_config.h>
#endif

#include "commonVar.h"
#include "commonDef.h"




void systemConfig(void);
void readEeprom(void);
void writeEeprom(uint8_t add, uint8_t data);