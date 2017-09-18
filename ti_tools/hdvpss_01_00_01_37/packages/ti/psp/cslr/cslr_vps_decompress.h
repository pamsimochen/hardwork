/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_decompress.h
*
* Brief: This file contains the Register Description for vps_decompress
*
*********************************************************************/
#ifndef _CSLR_VPS_DECOMPRESS_H_
#define _CSLR_VPS_DECOMPRESS_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 STATUS;
    volatile Uint32 UNIT_SETTINGS;
} CSL_Vps_decompressRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_decompressRegs     *CSL_VpsDecompressRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* status */

#define CSL_VPS_DECOMPRESS_STATUS_ENABLE_MASK (0x00000001u)
#define CSL_VPS_DECOMPRESS_STATUS_ENABLE_SHIFT (0x00000000u)

#define CSL_VPS_DECOMPRESS_STATUS_NUM_IWT_MASK (0x00000006u)
#define CSL_VPS_DECOMPRESS_STATUS_NUM_IWT_SHIFT (0x00000001u)


/* unit_settings */

#define CSL_VPS_DECOMPRESS_UNIT_SETTINGS_RESERVED_MASK (0x00000080u)
#define CSL_VPS_DECOMPRESS_UNIT_SETTINGS_RESERVED_SHIFT (0x00000007u)

#define CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_PER_LINE_MASK (0x0000FF00u)
#define CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_PER_LINE_SHIFT (0x00000008u)

#define CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_SIZE_MASK (0x0000007Fu)
#define CSL_VPS_DECOMPRESS_UNIT_SETTINGS_UNIT_SIZE_SHIFT (0x00000000u)


#endif
