/** ==================================================================
 *  @file   issutils_grpx.h                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    \ingroup ISSUTILS_API
    \defgroup ISSUTILS_GRPX_API Graphics APIs
    @{
*/

/**
 *  \file issutils_grpx.h
 *
 *  \brief Graphics API
*/

#ifndef _ISSUTILS_GRPX_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSUTILS_GRPX_H_

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
/* ===================================================================
 *  @func     IssUtils_getPitch                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 IssUtils_getPitch(UInt32 width, UInt32 bpp);

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
/* ===================================================================
 *  @func     IssUtils_grpxGenPattern                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void IssUtils_grpxGenPattern(UInt8 * addr,
                             UInt32 dataFormat,
                             UInt32 width,
                             UInt32 height, UInt8 reversed, UInt8 alpha);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
