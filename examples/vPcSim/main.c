
#include <stdio.h>
#include "..\..\commonInterface.h"

#define SCPT 1 // corresponds to 24 min sleep cycle

// The role of the current running sketch
role_e role;

void do_sleep(void);
void work_time(void);

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
  if (! configFlags&VFLAG_FRAME_MODE)
  {
    
  }

  
}

void readEeprom(void)
{
  config.frame_mode = 0;
  config.rolde_id   = 5;
  config.mac_addr   = 0;
  
  if (FRAME_SHORT_MODE == config.frame_mode ||
  FRAME_LONGE_MODE == config.frame_mode)
  {
    configFlags = configFlags|VFLAG_FRAME_MODE;
  }
  
  if( UNDEFINED < config.mac_addr && config.mac_addr <= BS_MAC_ID)
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

void work_time(void)
{


}

void do_sleep(void)
{
  
}

int main(void)
{
	readEeprom();
}