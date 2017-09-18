/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_ths.c
 *
 *  \brief I2C driver
 *  This file implements functionality for the THS7375 and THS8360.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/devices/vps_thsfilters.h>
#include <ti/psp/devices/thsfilters/vpsdrv_thsfilters.h>
#include <ti/sysbios/knl/Semaphore.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Maximum ports supported by the PCF8575 */
#define VPS_PCF8575_MAX_PORTS           (2u)

/* Macros for accessing for ports */
#define VPS_PCF8575_PORT0               (0u)
#define VPS_PCF8575_PORT1               (1u)

#ifdef TI_816X_BUILD
/* Macros for PCF8575 Pins */
#define VPS_PCF8575_PIN0                (0x1)
#define VPS_PCF8575_PIN1                (0x2)
#define VPS_PCF8575_PIN2                (0x4)
#define VPS_PCF8575_PIN3                (0x8)
#define VPS_PCF8575_PIN4                (0x10)
#define VPS_PCF8575_PIN5                (0x20)
#define VPS_PCF8575_PIN6                (0x40)
#define VPS_PCF8575_PIN7                (0x80)

#define VPS_PCF8575_PIN10               (0x1)
#define VPS_PCF8575_PIN11               (0x2)
#else

/* Macros for PCF8575 Pins */
#define VPS_PCF8575_PIN0                (0x10)
#define VPS_PCF8575_PIN1                (0x20)
#define VPS_PCF8575_PIN2                (0x4)
#define VPS_PCF8575_PIN3                (0x8)
#define VPS_PCF8575_PIN4                (0x2)
#define VPS_PCF8575_PIN5                (0x1)
#define VPS_PCF8575_PIN6                (0x40)
#define VPS_PCF8575_PIN7                (0x80)

#define VPS_PCF8575_PIN10               (0x1)
#define VPS_PCF8575_PIN11               (0x2)
#endif

#define VPS_THS7375_MASK                (VPS_PCF8575_PIN10 | VPS_PCF8575_PIN11)

#define VPS_THS7360_SD_MASK             (VPS_PCF8575_PIN2 | VPS_PCF8575_PIN5)

#define VPS_THS7360_SF_MASK             (VPS_PCF8575_PIN0 |                    \
                                         VPS_PCF8575_PIN1 |                    \
                                         VPS_PCF8575_PIN3 |                    \
                                         VPS_PCF8575_PIN4)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void Vps_ThsGetLock();
static Void Vps_ThsReleaseLock();

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Static array to keep track of each pin value for each port */
static UInt8 VpsPcf8575Port[VPS_PCF8575_MAX_PORTS] = {0x0u, 0x0u};

/* Semaphore to protect port array */
static Semaphore_Handle VpsPcfPortLock;

static UInt32 VpsThsI2cInst = VPS_DEVICE_I2C_INST_ID_1;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/* \brief Function to initialize THS7375 and THS7360. It also creates
          semaphore to protect pcf8575 port array */
Int32 Vps_thsFiltersInit(UInt32 i2cInstId)
{
    Int32 retVal = FVID2_SOK;
    Semaphore_Params semParams;

    /* Create global semaphore to protect pcf port array */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    VpsPcfPortLock = Semaphore_create(1u, &semParams, NULL);
    if (NULL == VpsPcfPortLock)
    {
        retVal = FVID2_EALLOC;
    }

#ifdef TI_816X_BUILD
    VpsPcf8575Port[VPS_PCF8575_PORT0] = 0x0;
    VpsPcf8575Port[VPS_PCF8575_PORT1] = 0x0;
#else
    VpsPcf8575Port[VPS_PCF8575_PORT0] = 0xFF;
    VpsPcf8575Port[VPS_PCF8575_PORT1] = 0xFF;
#endif

    VpsThsI2cInst = i2cInstId;

    return (retVal);
}



/* \brief Function to deinitialize THS7375 and THS7360 */
Int32 Vps_thsFiltersDeInit()
{
    /* Delete semaphore */
    if (NULL != VpsPcfPortLock)
    {
        Semaphore_delete(&VpsPcfPortLock);
    }

    return (FVID2_SOK);
}



/* \brief Function to set THS7375 parameters through PCF8575 I2C expander */
Int32 Vps_ths7375SetParams(Vps_ThsFilterCtrl ctrl)
{
    Int32 retVal;

    Vps_ThsGetLock();

    /* Clear out THS7375 */
    VpsPcf8575Port[VPS_PCF8575_PORT1] &= ~VPS_THS7375_MASK;

    VpsPcf8575Port[VPS_PCF8575_PORT1] |= (ctrl & VPS_THS7375_MASK);

    retVal = Vps_deviceRawWrite8(VpsThsI2cInst,
                                 VPS_PCF8575_I2C_ADDR,
                                 VpsPcf8575Port,
                                 VPS_PCF8575_MAX_PORTS);
    Vps_ThsReleaseLock();

    return (retVal);
}



/* \brief Function to set THS7360 SD parameters through PCF8575 I2C
          expander. This settings affects the Composite and
          SVideo outputs only. */
Int32 Vps_ths7360SetSdParams(Vps_ThsFilterCtrl ctrl)
{
    Int32 retVal = FVID2_SOK;
    UInt8 val;

    Vps_ThsGetLock();

    VpsPcf8575Port[VPS_PCF8575_PORT0] &= ~(VPS_THS7360_SD_MASK);
    switch (ctrl)
    {
        case VPS_THSFILTER_ENABLE_MODULE:
            val = ~(VPS_THS7360_SD_MASK);
            break;
        case VPS_THSFILTER_BYPASS_MODULE:
            val = VPS_PCF8575_PIN2;
            break;
        case VPS_THSFILTER_DISABLE_MODULE:
            val = VPS_THS7360_SD_MASK;
            break;
        default:
            retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        VpsPcf8575Port[VPS_PCF8575_PORT0] |= (val & VPS_THS7360_SD_MASK);
        retVal = Vps_deviceRawWrite8(VpsThsI2cInst,
                                     VPS_PCF8575_I2C_ADDR,
                                     VpsPcf8575Port,
                                     VPS_PCF8575_MAX_PORTS);
    }

    Vps_ThsReleaseLock();

    return (retVal);
}



/* \brief Function to set THS7360 Sf (Selectable Filter) parameters
          through PCF8575 I2C expander. This settings affects
          component outputs only. This is used to select filter to
          the SD/ED/HD/True HD modes. */
Int32 Vps_ths7360SetSfParams(Vps_Ths7360SfCtrl ctrl)
{
    Int32 retVal = FVID2_SOK;
    UInt8 val;

    Vps_ThsGetLock();

    VpsPcf8575Port[VPS_PCF8575_PORT0] &= ~(VPS_THS7360_SF_MASK);
    switch(ctrl)
    {
        case VPS_THS7360_DISABLE_SF:
            val = VPS_PCF8575_PIN4;
            break;
        case VPS_THS7360_BYPASS_SF:
            val = VPS_PCF8575_PIN3;
            break;
        case VPS_THS7360_SF_SD_MODE:
            val = ~(VPS_THS7360_SF_MASK);
            break;
        case VPS_THS7360_SF_ED_MODE:
            val = VPS_PCF8575_PIN0;
            break;
        case VPS_THS7360_SF_HD_MODE:
            val = VPS_PCF8575_PIN1;
            break;
        case VPS_THS7360_SF_TRUE_HD_MODE:
            val = VPS_PCF8575_PIN0|VPS_PCF8575_PIN1;
            break;
        default:
            retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        VpsPcf8575Port[VPS_PCF8575_PORT0] |= (val & VPS_THS7360_SF_MASK);

        retVal = Vps_deviceRawWrite8(VpsThsI2cInst,
                                     VPS_PCF8575_I2C_ADDR,
                                     VpsPcf8575Port,
                                     VPS_PCF8575_MAX_PORTS);
    }

    Vps_ThsReleaseLock();

    return (retVal);
}


static Void Vps_ThsGetLock()
{
    Semaphore_pend(VpsPcfPortLock, BIOS_WAIT_FOREVER );
}

static Void Vps_ThsReleaseLock()
{
    Semaphore_post (VpsPcfPortLock);
}
