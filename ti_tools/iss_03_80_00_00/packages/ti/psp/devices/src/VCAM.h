/** ==================================================================
 *  @file   VCAM.h                                                  
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
#define GPIO_EXP_SLAVE_ADDR		0x27

#define PAR_CAM_I2C_BUS_NUM		0x2
#define PAR_CAM_SLAVE_ADDR		(0x90>>1)

#define HISPI_CAM_I2C_BUS_NUM	0x2
    // #define HISPI_CAM_SLAVE_ADDR (0x20>>1)
#define HISPI_CAM_SLAVE_ADDR	(0x6C>>1)

    /* 
     *====================
     * Function declaration
     *====================
     */
    STATUS io_exp_set(UINT8 num, UINT8 state);
    UINT16 io_exp_rd();
    UINT16 parallel_sensor_read(UINT8 u8BusNum, UINT16 u16SlaveAddr, UINT8 reg);
    UINT8 *parallel_sensor_write(UINT8 u8BusNum, UINT16 u16SlaveAddr, UINT8 reg,
                                 UINT16 val);
    UINT16 HISPI_sensor_read(UINT8 u8BusNum, UINT16 u16SlaveAddr, UINT16 reg,
                             UINT8 req_num);
    UINT8 *HISPI_sensor_write(UINT8 u8BusNum, UINT16 u16SlaveAddr, UINT16 reg,
                              UINT8 * val, UINT8 valBufLen);
    INT16 parallel_camera_test();
    INT16 HISPI_camera_test();

#ifdef __cplusplus
}                                                          /* End of extern C 
                                                            */
#endif                                                     /* #ifdef
                                                            * __cplusplus */
#endif                                                     /* End of
                                                            * _DM814x_EVM_H_ */
