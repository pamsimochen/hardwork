/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_compress.h
*
* Brief: This file contains the Register Description for vps_compress
*
*********************************************************************/
#ifndef _CSLR_VPS_COMPRESS_H_
#define _CSLR_VPS_COMPRESS_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 STATUS;
    volatile Uint32 UNIT_SETTINGS;
} CSL_Vps_compressRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_compressRegs       *CSL_VpsCompressRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* status */

#define CSL_VPS_COMPRESS_STATUS_ENABLE_MASK (0x00000001u)
#define CSL_VPS_COMPRESS_STATUS_ENABLE_SHIFT (0x00000000u)

#define CSL_VPS_COMPRESS_STATUS_NUM_WT_MASK (0x00000006u)
#define CSL_VPS_COMPRESS_STATUS_NUM_WT_SHIFT (0x00000001u)


/* unit_settings */

#define CSL_VPS_COMPRESS_UNIT_SETTINGS_RESERVED_MASK (0xFFFE0000u)
#define CSL_VPS_COMPRESS_UNIT_SETTINGS_RESERVED_SHIFT (0x00000011u)

#define CSL_VPS_COMPRESS_UNIT_SETTINGS_UNIT_BIT_SIZE_MASK (0x0001FF80u)
#define CSL_VPS_COMPRESS_UNIT_SETTINGS_UNIT_BIT_SIZE_SHIFT (0x00000007u)

#define CSL_VPS_COMPRESS_UNIT_SETTINGS_UNIT_SIZE_MASK (0x0000007Fu)
#define CSL_VPS_COMPRESS_UNIT_SETTINGS_UNIT_SIZE_SHIFT (0x00000000u)


#endif
