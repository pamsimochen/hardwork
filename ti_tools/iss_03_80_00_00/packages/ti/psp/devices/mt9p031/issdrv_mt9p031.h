/** ==================================================================
 *  @file   issdrv_mt9p031.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/mt9p031/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISSDRV_MT9P031_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_MT9P031_H_

#include <ti/psp/devices/iss_sensorDriver.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

#define CTRL_MODULE_BASE_ADDR	(0x48140000u)

/* pinmux for CAM */
#define	PINCNTL156				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A6C)
#define	PINCNTL157				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A70)
#define	PINCNTL158				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A74)
#define	PINCNTL159				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A78)
#define	PINCNTL160				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A7C)
#define	PINCNTL161				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A80)
#define	PINCNTL162				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A84)
#define	PINCNTL163				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A88)
#define	PINCNTL164				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A8C)
#define	PINCNTL165				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A90)
#define	PINCNTL166				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A94)
#define	PINCNTL167				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A98)
#define	PINCNTL168				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A9C)
#define	PINCNTL169				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AA0)
#define	PINCNTL170				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AA4)
#define	PINCNTL171				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AA8)
#define	PINCNTL172				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AAC)
#define	PINCNTL173				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB0)
#define	PINCNTL174				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB4)
#define	PINCNTL175				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB8)

#define PINCNTL74				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0924)
#define PINCNTL75				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0928)

/* ===================================================================
 *  @func     Iss_Mt9p031Init                                               
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
Int32 Iss_Mt9p031Init();

/* ===================================================================
 *  @func     Iss_Mt9p031Create                                               
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
Fdrv_Handle Iss_Mt9p031Create(UInt32 drvId,
                              UInt32 instanceId,
                              Ptr createArgs,
                              Ptr createStatusArgs,
                              const FVID2_DrvCbParams * fdmCbParams);

/* ===================================================================
 *  @func     Iss_Mt9p031Control                                               
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
Int32 Iss_Mt9p031Control(Fdrv_Handle handle,
                         UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs);

/* ===================================================================
 *  @func     Iss_Mt9p031Delete                                               
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
Int32 Iss_Mt9p031Delete(Fdrv_Handle handle, Ptr deleteArgs);

/* ===================================================================
 *  @func     Iss_Mt9p031DeInit                                               
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
Int32 Iss_Mt9p031DeInit();

/* ===================================================================
 *  @func     Iss_Mt9p031PinMux                                               
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
Int32 Iss_Mt9p031PinMux();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
