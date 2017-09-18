/** ==================================================================
 *  @file   DM814x_EVM.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
// << PUT CORRECT COPYRIGHT NOTICE HERE >>

/**
 * \file    DM814x_EVM.h
 *
 * \brief   Defines the SoC specific memory Maps
 *
 * This header file contains the Macros definitions for SoC memory maps
 * 
 *
 * \author  0043
 *
 * \version 01a,13aug,2010 Created.
 */

#ifndef _DM814x_EVM_H_
#define _DM814x_EVM_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* 
     *====================
     * Includes
     *====================
     */
#include "DM814x_types.h"

/**
@addtogroup DM814x EVM
@{
*/

    /* 
     *====================
     * Defines
     *====================
     */

    /* 
     *====================
     * Function declaration
     *====================
     */

/** **************************************************************************
 * \n \brief Micro-second delay routine
 *
 * Utility routine for micro-seconds
 * 
 * \param  u32usec	[IN] 	Delay in micro-seconds
 *
 * \return
 * \n      return SUCCESS for success  - Description
 * \n      return FAILED for error   - Description
 */
    extern void DM814x_usecDelay(UINT32 u32usec);
/** **************************************************************************
 * \n \brief Routine to initialize the DM814x EVM board
 *
 * This routine initializes the DM814x EVM Base Board
 *
 * \return
 * \n      return SUCCESS for success  - Description
 * \n      return FAILED for error   - Description
 */
    extern STATUS DM814x_EVM_Init(void);

    /* @} *//* End of EVM */

#ifdef __cplusplus
}                                                          /* End of extern C 
                                                            */
#endif                                                     /* #ifdef
                                                            * __cplusplus */
#endif                                                     /* End of
                                                            * _DM814x_EVM_H_ */
