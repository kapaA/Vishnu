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



// The various roles supported by this sketch
typedef enum { role_end_node = 1, role_base_station = 0 } role_e;

// MAC address ID
#define BS_MAC_ID 0x3F
#define UNDEFINED 0x00

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

