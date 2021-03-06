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

#ifndef __COMON_VAR_H__
#define __COMON_VAR_H__

#define BUFFER_SIZE 40

#ifndef ARDUINO
typedef unsigned __int8 uint8_t;
typedef unsigned __int32 uint32_t;
#endif

typedef enum { wdt_16ms = 0, wdt_32ms, wdt_64ms, wdt_128ms, wdt_250ms, wdt_500ms, wdt_1s, wdt_2s, wdt_4s, wdt_8s } wdt_prescalar_e;


/**
 * Typedef struct containing the payload structure
 * Size: 7 bytes
*/
typedef struct{
  uint8_t data[2];        // 3 bytes
  unsigned long seqNum;    // 4 bytes
}  pload;  

/**
 * Typedef struct containing the header structure
 * Size: 3 bytes
*/
typedef struct{
  uint8_t  type;    // 8 bits
  uint8_t srcAddr;    // 8 bits
  uint8_t destAddr;    // 8 bits
}  head;  
  
/**
* Typedef struct containing Vishnu  Data frame structure
* Size: One frame is 10 bytes long
*/
typedef struct
{
  head header;    // 3 bytes 
  pload payload;  // 7 bytes
} VDFrame;



/**
* Typedef struct containing configuration info
*/
typedef struct
{
  uint8_t rolde_id;
  uint8_t frame_mode;
  uint8_t mac_addr;
} configuration;


typedef struct
{
  unsigned volatile int read_pointer;
  unsigned volatile int write_pointer;
  unsigned volatile int data_size;
  VDFrame buffer[BUFFER_SIZE];
}bufPar;


/*
** ==========================================================================
**                       Extern Global variables
** ==========================================================================
*/


extern configuration config;

extern bufPar outBufPar;
extern bufPar inBufPar;

#endif

