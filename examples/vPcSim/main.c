
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\..\commonInterface.h"

#define SCPT 1 // corresponds to 24 min sleep cycle

// The role of the current running sketch
role_e role;

void do_sleep(void);
void work_time(void);
uint8_t sendFrame(pload *data);

const short sleep_cycles_per_transmission = 1;
volatile short sleep_cycles_remaining = 1;

configuration config;
static uint8_t configFlags = 0;

GLOB_RET network_outgoing(pload *d);
GLOB_RET network_incoming(pload *d);

GLOB_RET mac_interface(char iKey, pload *d);
GLOB_RET mac_outgoing(pload *d);
GLOB_RET mac_incoming(pload *d);

GLOB_RET phy_interface(char iKey, VDFrame *f);
GLOB_RET phy_outgoing(VDFrame *d);
GLOB_RET phy_incoming(VDFrame *d);


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
    config.rolde_id = 1;
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

uint8_t sendFrame(pload *data)
{
  network_interface(OUTGOING, data);
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
  pload p;

  p.data[0] = 24;
  p.data[1] = 25;
  readEeprom();
  systemConfig();

  sendFrame(&p);
}








/*==============================================================================
** Function...: network_interface
** Return.....: GLOB_RET
** Description: network layer interface. All call must go through this interface
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET network_interface(char iKey, pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  if (OUTGOING == iKey)
  {
    ret = network_outgoing(f);
  }
  else if (INCOMING == iKey)
  {
    ret = network_incoming(f);
  }
  else
  {
    ret = GLOB_ERROR_INVALID_PARAM;
  }
  return ret;
}

/*==============================================================================
** Function...: network_outgoing
** Return.....: GLOB_RET
** Description: private function that handles all outgoing packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET network_outgoing(pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  mac_interface(OUTGOING, f);

  return ret;
}

/*==============================================================================
** Function...: network_incoming
** Return.....: GLOB_RET
** Description: private function that handles all incoming packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET network_incoming(pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  return ret;
}

/*==============================================================================
** Function...: mac_interface
** Return.....: GLOB_RET
** Description: mac layer interface. All call must go through this interface
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET mac_interface(char iKey, pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  if (OUTGOING == iKey)
  {
    ret = mac_outgoing(f);
  }
  else if (INCOMING == iKey)
  {
    ret = mac_incoming(f);
  }
  else
  {
    ret = GLOB_ERROR_INVALID_PARAM;
  }
  return ret;
}

/*==============================================================================
** Function...: mac_outgoing
** Return.....: GLOB_RET
** Description: private function that handles all outgoing packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET mac_outgoing(pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  return ret;
}

/*==============================================================================
** Function...: mac_incoming
** Return.....: GLOB_RET
** Description: private function that handles all incoming packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET mac_incoming(pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  return ret;
}


/*==============================================================================
** Function...: phy_interface
** Return.....: GLOB_RET
** Description: mac layer interface. All call must go through this interface
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET phy_interface(char iKey, VDFrame *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  if (OUTGOING == iKey)
  {
    ret = phy_outgoing(f);
  }
  else if (INCOMING == iKey)
  {
    ret = phy_incoming(f);
  }
  else
  {
    ret = GLOB_ERROR_INVALID_PARAM;
  }
  return ret;
}

/*==============================================================================
** Function...: phy_outgoing
** Return.....: GLOB_RET
** Description: private function that handles all outgoing packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET phy_outgoing(VDFrame *f)
{
  GLOB_RET ret = GLOB_SUCCESS;


}

/*==============================================================================
** Function...: mac_incoming
** Return.....: GLOB_RET
** Description: private function that handles all incoming packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET phy_incoming(VDFrame *f)
{

}