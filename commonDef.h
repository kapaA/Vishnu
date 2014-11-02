/*  =========================================================================
    Copyright (C) 2012 - 2014 Mobile Devices AAU.
    All rights reserved.
    =========================================================================
    This document contains proprietary information belonging to Achuthan.
    Passing on and copying of this document, use and
    communication of its contents is not permitted without prior written
    authorization.
    =========================================================================
    Revision Information:
        File name: commonDef.h
        Version:   v0.0
        Date:      2014-10-31
    =========================================================================
*/

#ifndef __COMON_DEF_H__
#define __COMON_DEF_H__

/**
 * Enumeration containing different return types
*/
typedef enum
{
  KEEP_BUFFER_IGNORE            = 23,
  KEEP_BUFFER_FULL              = 22,
  IN_BUFFER_IGNORE              = 21,
  IN_BUFFER_EMPTY               = 20,
  IN_BUFFER_FULL                = 19,
  OUT_BUFFER_EMPTY              = 18,
  OUT_BUFFER_FULL               = 17,

  APP_CONF_NOT_SET              = 16,
  APP_CONF_SUCCESS              = 15,
  APP_CONF_FAILURE              = 14,

  CODING_FAILURE                = 13,
  CODING_BELOW_HOLDING          = 12,
  CODING_FWD                    = 11,
  CODING_SUCCESS                = 10,
  CODING_BUFFER_FULL            = 9,
  CODING_BUFFER_EMPTY           = 8,
  DATA_LINK_ACK_TIMEOUT         = 7, /* Payload sent but did not receive ack */
  DATA_LINK_GOT_ACK             = 6, /* Payload sent and received ack */
  DATA_LINK_CTS_TIMEOUT         = 5, /* RTS sent but did not receive cts*/
  DATA_LINK_GOT_CTS             = 4, /* RTS sent and received cts*/
  DATA_LINK_DIFS_WAIT_OK        = 3, /* Waited DIFS without any CD*/
  DATA_LINK_BACKOFF_OK          = 2, /* Did backoff without CD*/
  DATA_LINK_CARRIER_DETECTED    = 1, /* Carrier detected */

  GLOB_SUCCESS                  = 0,  /* The prosses was prossed with success */
  GLOB_FAILURE                  = -1, /* The prosses was prossed with some or more error */
  GLOB_ERROR_OUT_OF_RANGE_PARAM = -2, /* The parameter passed to the function is outside the valid range */
  GLOB_ERROR_INVALID_MESSAGE    = -3, /* The given message does not identify a valid object */
  GLOB_ERROR_INVALID_PARAM      = -4, /* The parameter passed to the function is invalid*/
  GLOB_ERROR_OUT_OF_HANDLES     = -5, /* There is no free handle available*/
  GLOB_ERROR_INIT               = -6, /* Initialisation when wrong */
  GLOB_WARNING_PARAM_NOT_SET    = -20, /* There requiret parameter is not set */
}GLOB_RET;

// The various roles supported by this sketch
typedef enum { role_end_node = 1, role_base_station  } role_e;

// MAC address ID
#define BS_MAC_ID        0x3F
#define UNDEFINED_MAC_ID 0x00

// EEPROM
#define EEPROM_ROLE       0
#define EEPROM_MAC_ADD    1
#define EEPROM_FRAME_MODE 2
#define EEPROM_SLEEP_TIME 3

// Config flags
#define VFLAG_ROLE       0x01
#define VFLAG_MAC_ADD    0x02
#define VFLAG_FRAME_MODE 0x04

// config defines
#define FRAME_SHORT_MODE 1
#define FRAME_LONGE_MODE 2

//Frame size
#define SHORT_FRAME_SIZE 2
#define LONGE_FRAME_SIZE 29

//Interface iKey
#define INCOMING 0
#define OUTGOING 1

#endif
