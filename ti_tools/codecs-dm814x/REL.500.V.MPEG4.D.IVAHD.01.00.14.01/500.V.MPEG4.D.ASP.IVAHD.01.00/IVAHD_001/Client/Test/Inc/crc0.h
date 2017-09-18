/*
********************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
********************************************************************************
*/



/**
*******************************************************************************
 * @file <crc0.h>
 *
 * @brief  A header file describing the various CRC standards & function
 *         prototypes.
 *
 * @author: Ramakrishna
 *
 * @version 0.0 (Jan 2008)  : Base version  [Kumar]
 * @version 0.2 (August 2011) : Code commenting and cleanup
 *                           [Ashish]
 ******************************************************************************
*/

#ifndef _crc_h
#define _crc_h

/****************************************************************************
*   INCLUDE FILES
*****************************************************************************/
/* -------------------- system and platform files ------------------------- */
#include <stdio.h>

/****************************************************************************
*   EXTERNAL REFERENCES NOTE : only use if not found in header file
*****************************************************************************/
/* ------------------------ data declarations ----------------------------- */
/* ----------------------- function prototypes ---------------------------- */

/****************************************************************************
*   PUBLIC DECLARATIONS Defined here, used elsewhere
*****************************************************************************/
/* ----------------------- data declarations ------------------------------ */
/* ---------------------- function prototypes ----------------------------- */

/******************************************************************************
*  MACROS
******************************************************************************/

/**
 * Select the CRC standard from the list that follows.
 * CRC_CCITT,CRC32 and CRC16
*/
#define CRC32
//#define FALSE   0
//#define TRUE    !FALSE

#if defined(CRC_CCITT)

  typedef unsigned short      crc;
  #define CRC_NAME            "CRC-CCITT"
  #define POLYNOMIAL          0x1021
  #define INITIAL_REMAINDER   0xFFFF
  #define FINAL_XOR_VALUE     0x0000
  #define REFLECT_DATA        FALSE
  #define REFLECT_REMAINDER   FALSE
  #define CHECK_VALUE         0x29B1

#elif defined(CRC16)

  typedef unsigned short      crc;
  #define CRC_NAME            "CRC-16"
  #define POLYNOMIAL          0x8005
  #define INITIAL_REMAINDER   0x0000
  #define FINAL_XOR_VALUE     0x0000
  #define REFLECT_DATA        TRUE
  #define REFLECT_REMAINDER   TRUE
  #define CHECK_VALUE         0xBB3D

#elif defined(CRC32)

  typedef unsigned long       crc;
  #define CRC_NAME            "CRC-32"
  #define POLYNOMIAL          0x04C11DB7
  #define INITIAL_REMAINDER   0xFFFFFFFF
  #define FINAL_XOR_VALUE     0xFFFFFFFF
  #define REFLECT_DATA        TRUE
  #define REFLECT_REMAINDER   TRUE
  #define CHECK_VALUE         0xCBF43926

#else
  #error "One of CRC_CCITT, CRC16, or CRC32 must be #define'd."
#endif

/**
********************************************************************************
 *  @fn     crcInit
 *  @brief  Populate the partial CRC lookup table.
 *
 *  @param[in] None
 *
 *  @return None
********************************************************************************
*/
void  crcInit(void);

/**
********************************************************************************
 *  @fn     crcSlow
 *  @brief  Compute the CRC of a given message.
 *
 *  @param[in] message : Input buffer for which CRC to be caclculated
 *
 *  @param[in] nBytes  : Number of Bytes
 *
 *  @return CRC of the message
********************************************************************************
*/
crc   crcSlow(unsigned char const message[], int nBytes);

/**
********************************************************************************
 *  @fn     crcFast
 *  @brief  Compute the CRC of a given message.
 *
 *  @param[in] message : Input buffer for which CRC to be caclculated
 *
 *  @param[in] nBytes  : Number of Bytes
 *
 *  @return CRC of the message
********************************************************************************
*/
crc   crcFast(unsigned char const message[], int nBytes);

#endif /* _crc_h */


