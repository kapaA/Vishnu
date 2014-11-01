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
        File name: commonVar.h
        Version:   v0.0
        Date:      2014-10-31
    =========================================================================
*/
#ifndef ARDUINO
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;
#endif

typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;


/**
 * Typedef struct containing the payload structure
 * Size: 28 bytes
*/
typedef struct{
  uint8_t reserved[29];         // 29 bytes
}  pload;  
  
/**
* Typedef struct containing Vishnu Large Data frame structure
* Size: One frame is 32 bytes
*/
typedef struct
{
  uint8_t  type    :4;    // 4 bits
  uint8_t srcAddr  :6;    // 6 bits
  uint8_t destAddr :6;    // 6 bits
  uint8_t seqNr;          // 1 byte
  pload payload;          // 29 bytes
} VLDFrame;

/**
* Typedef struct containing Vishnu Short Data frame structure
* Size: One frame is 4 bytes
*/
typedef struct
{
  uint8_t  type    :4;    // 4 bits
  uint8_t srcAddr  :6;    // 6 bits
  uint8_t destAddr :6;    // 6 bits
  uint8_t data;           // 2 byte
} VSDFrame;

/**
* Typedef struct containing configuration info
*/
typedef struct
{
  uint8_t rolde_id;
  uint8_t frame_mode;
  uint8_t mac_addr;
} configuration;