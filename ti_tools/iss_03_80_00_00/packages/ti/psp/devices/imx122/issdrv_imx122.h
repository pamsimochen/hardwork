/** ==================================================================
 *  @file   issdrv_imx122.h
 *
 *  @path   /ti/psp/devices/imx122/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISSDRV_IMX122_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_IMX122_H_

#include <ti/psp/devices/iss_sensorDriver.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

#define CTRL_MODULE_BASE_ADDR	(0x48140000u)
#define GP1_MODULE_BASE_ADDR	(0x4804C000u)
#define GP3_MODULE_BASE_ADDR	(0x481AE000u)

//#define GP0_MODULE_BASE_ADDR	(0x48032000u)
//#define GP2_MODULE_BASE_ADDR	(0x481AC000u)

/* pinmux for CAM */
#define PINCNTL153				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0A60)
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

#define	PINCNTL85				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0950)
#define	PINCNTL86				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0954)
#define	PINCNTL87				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0958)
#define	PINCNTL88				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x095C)



#define	PINCNTL225				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0AB8)

//#define PINCNTL74				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0924)
//#define PINCNTL75				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0928)

//#define PINCNTL85				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0950)
//#define PINCNTL86				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0954)
//#define PINCNTL88				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x095C)

//#define GIO_INPUT_OUTPUT_DIR		(volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0134)
//#define GIO_ENABLE_DISABLE_WAKEUP	(volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0044)

#define GIO_GP1_GET_DATA			(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0138)
#define GIO_GP1_WRITE_DATA			(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0194)
#define GIO_GP1_CLEAR_DATA			(volatile UInt32*)(GP1_MODULE_BASE_ADDR + 0x0190)

/* ===================================================================
 *  @func     Iss_Imx122Init
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
Int32 Iss_Imx122Init();

/* ===================================================================
 *  @func     Iss_Imx122Create
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
Fdrv_Handle Iss_Imx122Create(UInt32 drvId,
                              UInt32 instanceId,
                              Ptr createArgs,
                              Ptr createStatusArgs,
                              const FVID2_DrvCbParams * fdmCbParams);

/* ===================================================================
 *  @func     Iss_Imx122Control
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
Int32 Iss_Imx122Control(Fdrv_Handle handle,
                         UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs);

/* ===================================================================
 *  @func     Iss_Imx122Delete
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
Int32 Iss_Imx122Delete(Fdrv_Handle handle, Ptr deleteArgs);

/* ===================================================================
 *  @func     Iss_Imx122DeInit
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
Int32 Iss_Imx122DeInit();

/* ===================================================================
 *  @func     Iss_Imx122PinMux
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
Int32 Iss_Imx122PinMux();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
