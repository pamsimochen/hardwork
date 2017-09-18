/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _ISSDRV_Imx104_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_Imx104_H_

#include <ti/psp/devices/iss_sensorDriver.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

#define CTRL_MODULE_BASE_ADDR	(0x48140000u)
#define GPIO_MODULE_BASE_ADDR   (0x4804c000u)

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

#define PINCNTL85				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0950)
#define PINCNTL86				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x0954)
#define PINCNTL88				(volatile UInt32*)(CTRL_MODULE_BASE_ADDR + 0x095C)


#define GIO_INPUT_OUTPUT_DIR                    (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0134)
#define GIO_ENABLE_DISABLE_WAKEUP               (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0044)
#define GIO_WRITE_DATA                          (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0194)
#define GIO_CLEAR_DATA                          (volatile UInt32*)(GPIO_MODULE_BASE_ADDR + 0x0190)

#define RSZ_SRC_FMT0             (volatile UInt32*)(0x52010428)

Int32 Iss_Imx104Init (  );

Fdrv_Handle Iss_Imx104Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams );


Int32 Iss_Imx104Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );
                           
Int32 Iss_Imx104Delete ( Fdrv_Handle handle, Ptr deleteArgs );                       
                           
Int32 Iss_Imx104DeInit (  );

Int32 Iss_Imx104PinMux (  );

void imx104_setgio(unsigned short addr, unsigned short reg);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

