/*  ============================================================================
    Copyright (C) 2014,
    All rights reserved.
    ============================================================================
    This document contains proprietary information belonging to Achuthan.
    Passing on and copying of this document, use and
    communication of its contents is not permitted without prior written
    authorization.
    ============================================================================
    Revision Information:
        File name: networkLayer.c
        Version:   v0.0
        Date:      2014-11-02
    ============================================================================
*/

/*============================================================================*/
/*                           INCLUDE STATEMENTS
==============================================================================*/
#include "commonInterface.h"

GLOB_RET network_outgoing( pload *d);
GLOB_RET network_incoming( pload *d );

GLOB_RET mac_interface(char iKey, VDFrame *d);



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

  if(OUTGOING==iKey)
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

GLOB_RET network_outgoing( pload *d)
{
  GLOB_RET ret = GLOB_SUCCESS;
  
  VDFrame fr;
  
  fr.header.destAddr = BS_MAC_ID;
  fr.header.srcAddr  = config.mac_addr;
  fr.header.type     = config.frame_mode;
  
  memcpy(&fr.payload.data, d, sizeof(fr.payload.data));
  
  mac_interface(OUTGOING, &fr);
  
  return ret;
}

/*==============================================================================
** Function...: network_incoming
** Return.....: GLOB_RET
** Description: private function that handles all incoming packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET network_incoming( pload *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  return ret;
}