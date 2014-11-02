
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\commonInterface.h"

#define SCPT 1 // corresponds to 24 min sleep cycle

// The role of the current running sketch
role_e role;

void do_sleep(void);
void work_time(void);
uint8_t sendFrame(uint8_t frameMode, void *data, uint8_t dType);

const short sleep_cycles_per_transmission = 1;
volatile short sleep_cycles_remaining = 1;

configuration config;
static uint8_t configFlags = 0;

void setup()
{
  readEeprom();
  systemConfig();
  
  role = role_end_node;

}

void systemConfig(void)
{
  if ((configFlags&VFLAG_FRAME_MODE)==0)
  {
    config.frame_mode = FRAME_SHORT_MODE;
  }
  if((configFlags&VFLAG_ROLE)==0 )
  {
    config.rolde_id = role_undefined;
  }
  if ((configFlags&VFLAG_MAC_ADD)==0)
  {
	  config.mac_addr = UNDEFINED_MAC_ID;
  }
}

void readEeprom(void)
{
  config.frame_mode = FRAME_SHORT_MODE;
  config.rolde_id   = role_end_node;
  config.mac_addr   = 60;
  
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
  work_time();

  while( sleep_cycles_remaining )
  do_sleep();

  sleep_cycles_remaining = sleep_cycles_per_transmission;
}

uint8_t sendFrame(uint8_t frameMode, void *data, uint8_t dType)
{
  if(FRAME_SHORT_MODE == frameMode)
  {
    uint8_t s = sizeof(&data);
    if(s>5)
    {
      return 1;
    }
    else
    {
      VSDFrame fr;
      fr.srcAddr  = config.mac_addr;
      fr.destAddr = BS_MAC_ID;
      fr.type = dType;
      memcpy(&fr.data, data, sizeof(data));
	  printf("Sending short data size[%d]\n",sizeof(fr));
    }
    
  }
  else if ( FRAME_LONGE_MODE == frameMode)
  {
    uint8_t s = sizeof(data);
    if(s>LONGE_FRAME_SIZE)
    {
      return 1;
    }
    else
    {
      VLDFrame fr;
      fr.srcAddr  = config.mac_addr;
      fr.destAddr = BS_MAC_ID;
      fr.type = dType;
	  memcpy(&fr.payload.data, data, LONGE_FRAME_SIZE);
	  printf("Sending longe data\n");
      
    }
  }
  
  return 0;
}

void work_time(void)
{


}

void do_sleep(void)
{
  
}

int main(void)
{
	uint8_t d[2];

	d[0] = 24;
	d[1] = 56;
	readEeprom();
	systemConfig();

	sendFrame(FRAME_SHORT_MODE, &d, 1);
}