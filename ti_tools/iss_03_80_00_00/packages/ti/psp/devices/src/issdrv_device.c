/** ==================================================================
 *  @file   issdrv_device.c
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

#include <ti/psp/devices/src/vpsdrv_devicePriv.h>
#include "issdrv_devicePriv.h"

#ifdef IMGS_MICRON_MT9J003
#include "ti/psp/devices/mt9j003/issdrv_mt9j003.h"
#endif
#ifdef IMGS_MICRON_AR0331
#include "ti/psp/devices/ar0331/issdrv_ar0331.h"
#endif
#ifdef IMGS_PANASONIC_MN34041
#include "ti/psp/devices/mn34041/issdrv_mn34041.h"
#endif
#ifdef IMGS_SONY_IMX035
#include "ti/psp/devices/imx035/issdrv_imx035.h"
#endif
#ifdef IMGS_OMNIVISION_OV2715
#include "ti/psp/devices/ov2715/issdrv_ov2715.h"
#endif
#ifdef IMGS_SONY_IMX036
#include "ti/psp/devices/imx036/issdrv_imx036.h"
#endif
#ifdef IMGS_OMNIVISION_OV9712
#include "ti/psp/devices/ov9712/issdrv_ov9712.h"
#endif
#ifdef IMGS_OMNIVISION_OV10630
#include "ti/psp/devices/ov10630/issdrv_ov10630.h"
#endif
#ifdef IMGS_MICRON_MT9P031
#include "ti/psp/devices/mt9p031/issdrv_mt9p031.h"
#endif
#ifdef IMGS_MICRON_MT9D131
#include "ti/psp/devices/mt9d131/issdrv_mt9d131.h"
#endif
#ifdef IMGS_MICRON_MT9M034
#include "ti/psp/devices/mt9m034/issdrv_mt9m034.h"
#endif
#ifdef IMGS_TAXAS_TVP514X
#include "ti/psp/devices/tvp514x/issdrv_tvp514x.h"
#endif
#ifdef IMGS_MICRON_AR0330
#include "ti/psp/devices/ar0330/issdrv_ar0330.h"
#endif
#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
#include "ti/psp/devices/mt9m034_dual/issdrv_mt9m034.h"
#endif
#ifdef IMGS_OMNIVISION_OV7740
#include "ti/psp/devices/ov7740/issdrv_ov7740.h"
#endif
#ifdef IMGS_SONY_IMX136
#ifdef APPRO_SENSOR_VENDOR
#include "ti/psp/devices/imx136_APPRO/issdrv_imx136.h"
#else
#include "ti/psp/devices/imx136_LIMG/issdrv_imx136.h"
#endif
#endif
#ifdef IMGS_SONY_IMX104
#include "ti/psp/devices/imx104/issdrv_imx104.h"
#endif
#ifdef IMGS_ALTASENS_AL30210
#include "ti/psp/devices/al30210/issdrv_al30210.h"
#endif
#ifdef IMGS_OMNIVISION_OV2710
#include "ti/psp/devices/ov2710/issdrv_ov2710.h"
#endif
#ifdef IMGS_SONY_IMX122
#include "ti/psp/devices/imx122/issdrv_imx122.h"
#endif
#ifdef IMGS_SONY_IMX140
#include "ti/psp/devices/imx140/issdrv_imx140.h"
#endif

#ifdef CBB_PLATFORM
#define REG32	*(volatile unsigned int*)
static UInt32 iss_gpio_get_base_address(Int32 bank)
{
	unsigned int address;

	if(bank==0)			address = 0x48032000;
	else if(bank==1)	address = 0x4804C000;
	else if(bank==2)	address = 0x481AC000;
	else if(bank==3)	address = 0x481AE000;
	else				return 0;

	return address;
}

Int32 iss_gpio_write_data(UInt32 band, UInt32 io_no, UInt32 data)
{
	UInt32 gpio_base;

	gpio_base = iss_gpio_get_base_address(band);
	if(!gpio_base)
		return FVID2_EFAIL;

	if(data)	REG32(gpio_base+0x194) = 1<<io_no;
	else		REG32(gpio_base+0x190) = 1<<io_no;

	return FVID2_SOK;
}
#endif

/* ===================================================================
 *  @func     Iss_deviceInit
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
Int32 Iss_deviceInit(Vps_DeviceInitParams * pPrm)
{
    Int32 status = FVID2_SOK;

    Iss_PlatformId platformId = Iss_platformGetId();

    Iss_PlatformBoardId boardId;

#ifdef POWER_OPT_DSS_OFF
    memset(&gVps_deviceObj, 0, sizeof(gVps_deviceObj));
#endif
    platformId = ISS_PLATFORM_ID_EVM_TI814x;
    if ((platformId == ISS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= ISS_PLATFORM_ID_MAX))
    {
        status = FVID2_EFAIL;
    }
#ifdef POWER_OPT_DSS_OFF

    if ((FVID2_SOK == status) && (TRUE == pPrm->isI2cInitReq))
    {
        status = Vps_deviceI2cInit(pPrm);
#else
    if (FVID2_SOK == status)
    {
#endif
        if (FVID2_SOK == status)
        {
#ifdef TI_8107_BUILD
#ifdef _IPNC_HW_PLATFORM_EVM_

		UInt8       regValue[2];
		status = Vps_deviceRawRead8(Vps_platformGetI2cInstId(), 0x20, regValue, 2u);
		regValue[0] = 0xBF;
		regValue[1] = 0xFF;
		Vps_deviceRawWrite8(Vps_platformGetI2cInstId(), 0x20, regValue, 2u);

		status = Vps_deviceRawRead8(Vps_platformGetI2cInstId(), 0x20, regValue, 2u);
#endif
#endif
#ifdef IMGS_MICRON_AR0331
            Iss_Ar0331PinMux();
#endif
#ifdef IMGS_PANASONIC_MN34041
            Iss_Mn34041PinMux();
#endif
#ifdef IMGS_SONY_IMX035
            Iss_Imx035PinMux();
#endif
#ifdef IMGS_OMNIVISION_OV2715
            Iss_Ov2715PinMux();
#endif
#ifdef IMGS_SONY_IMX036
            Iss_Imx036PinMux();
#endif
#ifdef IMGS_OMNIVISION_OV9712
            Iss_Ov9712PinMux();
#endif
#ifdef IMGS_OMNIVISION_OV10630
            Iss_Ov10630PinMux();
#endif
#ifdef IMGS_MICRON_MT9P031
            Iss_Mt9p031PinMux();
#endif
#ifdef IMGS_MICRON_MT9D131
            Iss_Mt9d131PinMux();
#endif
#ifdef IMGS_MICRON_MT9M034
            Iss_Mt9m034PinMux();
#endif
#ifdef IMGS_TAXAS_TVP514X
            Iss_Tvp514xPinMux();
#endif
#ifdef IMGS_MICRON_MT9J003
            //Iss_Mt9j003PinMux();
#endif
#ifdef IMGS_MICRON_AR0330
            Iss_Ar0330PinMux();
#endif
#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
            Iss_Mt9m034PinMux();
#endif
#ifdef IMGS_OMNIVISION_OV7740
            Iss_Ov7740PinMux();
#endif
#ifdef IMGS_SONY_IMX136
            Iss_Imx136PinMux();
#endif
#ifdef IMGS_SONY_IMX140
            Iss_Imx140PinMux();
#endif
#ifdef IMGS_SONY_IMX122
            Iss_Imx122PinMux();
#endif
#ifdef IMGS_SONY_IMX104
            Iss_Imx104PinMux();
#endif
#ifdef IMGS_ALTASENS_AL30210
            Iss_al30210PinMux();
#endif
#ifdef IMGS_OMNIVISION_OV2710
            Iss_Ov2710PinMux();
#endif

            boardId = Iss_platformGetBoardId();
            boardId = ISS_PLATFORM_BOARD_VCAM;
            if (boardId >= ISS_PLATFORM_BOARD_MAX)
            {
                status |= FVID2_EFAIL;
            }

            else if (ISS_PLATFORM_BOARD_VCAM == boardId)
            {
#ifdef IMGS_MICRON_MT9J003
                status |= Iss_Mt9j003Init();
#endif
#ifdef IMGS_MICRON_AR0331
                status |= Iss_Ar0331Init();
#endif
#ifdef IMGS_PANASONIC_MN34041
                status |= Iss_Mn34041Init();
#endif
#ifdef IMGS_SONY_IMX035
                status |= Iss_Imx035Init();
#endif
#ifdef IMGS_OMNIVISION_OV2715
                status |= Iss_Ov2715Init();
#endif
#ifdef IMGS_IMGS_SONY_IMX036
                status |= Iss_Imx036Init();
#endif
#ifdef IMGS_OMNIVISION_OV9712
                status |= Iss_Ov9712Init();
#endif
#ifdef IMGS_OMNIVISION_OV10630
                status |= Iss_Ov10630Init();
#endif
#ifdef IMGS_MICRON_MT9P031
                status |= Iss_Mt9p031Init();
#endif
#ifdef IMGS_MICRON_MT9D131
                status |= Iss_Mt9d131Init();
#endif
#ifdef IMGS_MICRON_MT9M034
                status |= Iss_Mt9m034Init();
#endif
#ifdef IMGS_TAXAS_TVP514X
                status |= Iss_Tvp514xInit();
#endif
#ifdef IMGS_MICRON_AR0330
                status |= Iss_Ar0330Init();
#endif
#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
                status |= Iss_Mt9m034Init();
#endif
#ifdef IMGS_OMNIVISION_OV7740
                status |= Iss_Ov7740Init();
#endif
#ifdef IMGS_SONY_IMX136
                status |= Iss_Imx136Init();
#endif
#ifdef IMGS_SONY_IMX140
                status |= Iss_Imx140Init();
#endif
#ifdef IMGS_SONY_IMX122
                status |= Iss_Imx122Init();
#endif
#ifdef IMGS_SONY_IMX104
                status |= Iss_Imx104Init();
#endif
#ifdef IMGS_ALTASENS_AL30210
                status |= Iss_al30210Init();
#endif
#ifdef IMGS_OMNIVISION_OV2710
                status |= Iss_Ov2710Init();
#endif

            }
            else
            {
                status = FVID2_EFAIL;
            }
        }
    }

    return (status);
}

/* ===================================================================
 *  @func     Iss_deviceDeInit
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
Int32 Iss_deviceDeInit()
{
    Int32 status = FVID2_SOK;

    Iss_PlatformId platformId = Iss_platformGetId();

    Iss_PlatformBoardId boardId = Iss_platformGetBoardId();

    if ((platformId == ISS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= ISS_PLATFORM_ID_MAX) ||
        (boardId >= ISS_PLATFORM_BOARD_MAX))
    {
        status = FVID2_EFAIL;
    }
    else if (ISS_PLATFORM_BOARD_VCAM == boardId)
    {
#ifdef IMGS_MICRON_MT9J003
        status |= Iss_Mt9j003DeInit();
#endif
#ifdef IMGS_MICRON_AR0331
        status |= Iss_Ar0331DeInit();
#endif
#ifdef IMGS_PANASONIC_MN34041
        status |= Iss_Mn34041DeInit();
#endif
#ifdef IMGS_SONY_IMX035
        status |= Iss_Imx035DeInit();
#endif
#ifdef IMGS_OMNIVISION_OV2715
        status |= Iss_Ov2715DeInit();
#endif
#ifdef IMGS_IMGS_SONY_IMX036
        status |= Iss_Imx036DeInit();
#endif
#ifdef IMGS_OMNIVISION_OV9712
        status |= Iss_Ov9712DeInit();
#endif
#ifdef IMGS_OMNIVISION_OV10630
        status |= Iss_Ov10630DeInit();
#endif
#ifdef IMGS_MICRON_MT9P031
        status |= Iss_Mt9p031DeInit();
#endif
#ifdef IMGS_MICRON_MT9D131
        status |= Iss_Mt9d131DeInit();
#endif
#ifdef IMGS_MICRON_MT9M034
        status |= Iss_Mt9m034DeInit();
#endif
#ifdef IMGS_TAXAS_TVP514X
        status |= Iss_Tvp514xDeInit();
#endif
#ifdef IMGS_MICRON_AR0330
        status |= Iss_Ar0330DeInit();
#endif
#ifdef IMGS_MICRON_MT9M034_DUAL_HEAD_BOARD
        status |= Iss_Mt9m034DeInit();
#endif
#ifdef IMGS_OMNIVISION_OV7740
        status |= Iss_Ov7740DeInit();
#endif
#ifdef IMGS_SONY_IMX136
        status |= Iss_Imx136DeInit();
#endif
#ifdef IMGS_SONY_IMX140
        status |= Iss_Imx140DeInit();
#endif
#ifdef IMGS_SONY_IMX122
        status |= Iss_Imx122DeInit();
#endif
#ifdef IMGS_SONY_IMX104
        status |= Iss_Imx104DeInit();
#endif
#ifdef IMGS_ALTASENS_AL30210
        status |= Iss_al30210DeInit();
#endif
#ifdef IMGS_OMNIVISION_OV2710
        status |= Iss_Ov2710DeInit();
#endif
    }
    else
    {
        status |= FVID2_EFAIL;
    }
#ifdef POWER_OPT_DSS_OFF
    Vps_deviceI2cDeInit();
#endif
    return status;
}
