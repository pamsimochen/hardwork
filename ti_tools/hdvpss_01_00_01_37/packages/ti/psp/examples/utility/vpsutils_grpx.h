/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSUTILS_API
    \defgroup VPSUTILS_GRPX_API Graphics APIs
    @{
*/

/**
 *  \file vpsutils_grpx.h
 *
 *  \brief Graphics API
*/


#ifndef _VPSUTILS_GRPX_H_
#define _VPSUTILS_GRPX_H_

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/**
 *  \brief  This function to get the pitch
 *
 *  \param  width      [IN] width of region
 *  \param  bpp        [IN] bits per pixel
 *
 *  \return FVID2_SOK on success, else failure
 *
 */
UInt32 VpsUtils_getPitch(UInt32 width, UInt32 bpp);


/**
 *  \brief  This function to generate data pattern based on size and data format
 *
 *  \param  addr       [IN] Pointer to buffer where to store the pattern
 *  \param  dataFormat [IN] data format
 *  \param  width      [IN] width of region
 *  \param  height     [IN] region height
 *  \param  reversed   [IN] reversed pattern or not
 *  \param  alpha      [IN] alpha value
 */
Void VpsUtils_grpxGenPattern(UInt8 *addr,
                             UInt32 dataFormat,
                             UInt32 width,
                             UInt32 height,
                             UInt8 reversed,
                             UInt8 alpha);

#endif

/*@}*/
