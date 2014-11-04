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
        File name: macLayer.c
        Version:   v0.0
        Date:      2014-11-02
    ============================================================================
*/

/*============================================================================*/
/*                           INCLUDE STATEMENTS
==============================================================================*/
#include "commonInterface.h"

GLOB_RET mac_outgoing( VDFrame *d);
GLOB_RET mac_incoming( VDFrame *d );

GLOB_RET phy_interface(char iKey, VDFrame *f);

/*==============================================================================
** Function...: mac_interface
** Return.....: GLOB_RET
** Description: mac layer interface. All call must go through this interface
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET mac_interface(char iKey, VDFrame *f)
{
    GLOB_RET ret = GLOB_SUCCESS;

    if(OUTGOING==iKey)
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

GLOB_RET mac_outgoing( VDFrame *f)
{
  GLOB_RET ret = GLOB_SUCCESS;
  
  phy_interface(OUTGOING, f);
  
  return ret;
}

/*==============================================================================
** Function...: mac_incoming
** Return.....: GLOB_RET
** Description: private function that handles all incoming packets
** Created....: 02.11.2014 by Achuthan
** Modified...: dd.mm.yyyy by nn
==============================================================================*/

GLOB_RET mac_incoming( VDFrame *f)
{
  GLOB_RET ret = GLOB_SUCCESS;

  return ret;
}