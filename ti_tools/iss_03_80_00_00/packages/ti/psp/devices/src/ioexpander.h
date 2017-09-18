/** ==================================================================
 *  @file   ioexpander.h                                                  
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
 * \file    ioexpander.h
 *
 * \brief   IO Expander Header File
 *
 * This header file contains the Macros definitions for GPIO expander
 * 
 *
 * \author  0043
 *
 * \version 01a,25aug,2010 Created.
 */

#ifndef _DM814x_ioexpander_H_
#define _DM814x_ioexpander_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* 
     *====================
     * Includes
     *====================
     */
#include "DM814x_i2c.h"

/**
@addtogroup DM814x EVM GPIO Expander
@{
*/

    /* 
     *====================
     * Defines
     *====================
     */
#define GPIO_EXP_I2C_BUS_NUM	0x0
#define GPIO_EXP_SLAVE_ADDR		0x20

    /* 
     *====================
     * Function declaration
     *====================
     */
    UINT16 sw_get();
    STATUS led_set_num(UINT8 num, UINT8 state);
    STATUS led_set_pat(UINT8 pat);

    /* @} *//* End of DM814x EVM GPIO Expander */

#ifdef __cplusplus
}                                                          /* End of extern C 
                                                            */
#endif                                                     /* #ifdef
                                                            * __cplusplus */
#endif                                                     /* End of
                                                            * _DM814x_EVM_H_ */
