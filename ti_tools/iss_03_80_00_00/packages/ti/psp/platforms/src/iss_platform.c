/** ==================================================================
 *  @file   iss_platform.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file iss_platform.c
 *
 *  \brief Interface file to the platform specific functions abstraction APIs.
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/iss/iss.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/devices/iss_sensorDriver.h>
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#include <ti/psp/platforms/ti814x/iss_platformTI814x.h>
#endif
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

// #define PLATFORM_ZEBU

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

// #define ISS_PLATFORM_RESET_VIDEO_DECODER

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

typedef struct {
    FVID2_DrvOps fvidDrvOps;
    /**< FVID2 driver ops */
    Semaphore_Handle glock;
    /**< current pixel clock settings */
    UInt32 openCnt;
    /**< open count */
    Iss_PlatformId platformId;
    /**< Platform ID. For valid values see #Iss_PlatformId. */
    UInt32 isPlatformEvm;
    /**< Flag to determine whether platform is EVM or Simulator. */
    Iss_PlatformBoardId platBoardId;
    /**< Platform Board ID. For valid values see #Iss_PlatformBoardId. */
    Iss_PlatformCpuRev cpuRev;
    /**< CPU revision. */
    Iss_PlatformBoardRev baseBoardRev;
    /**< Base Board revision. */
    Iss_PlatformBoardRev dcBoardRev;
    /**< Daughter card board revision. */
} Iss_platformCommonObj;

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

Iss_platformCommonObj gIssPlatformCommonObj;

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     Iss_platformInit                                               
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
Int32 Iss_platformInit(Iss_PlatformInitParams * initParams)
{
    Int32 retVal = FVID2_SOK;

    Iss_PlatformInitParams localInitParams;

    memset(&gIssPlatformCommonObj, 0, sizeof(gIssPlatformCommonObj));

    gIssPlatformCommonObj.platformId = ISS_PLATFORM_ID_UNKNOWN;
    gIssPlatformCommonObj.platBoardId = ISS_PLATFORM_BOARD_MAX;

    /* Platform is EVM */
    gIssPlatformCommonObj.isPlatformEvm = TRUE;

    gIssPlatformCommonObj.platformId = ISS_PLATFORM_ID_EVM_TI814x;
    /* If NULL is passed pinMux will be initialized by default */
    if (NULL == initParams)
    {
        initParams = &localInitParams;
        initParams->isPinMuxSettingReq = TRUE;
    }
    retVal = Iss_platformTI814xInit(initParams);

    /* Get the CPU and board revisions */
    gIssPlatformCommonObj.cpuRev = Iss_platformTI814xGetCpuRev();
    gIssPlatformCommonObj.baseBoardRev = ISS_PLATFORM_BOARD_REV_MAX;
    gIssPlatformCommonObj.dcBoardRev = ISS_PLATFORM_BOARD_REV_MAX;

    return (retVal);
}

/* ===================================================================
 *  @func     Iss_platformDeInit                                               
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
Int32 Iss_platformDeInit(void)
{
    Int32 retVal = FVID2_SOK;

    retVal = Iss_platformTI814xDeInit();
    return (retVal);
}

/* Init EVM related sub-systems like I2C instance */
/* ===================================================================
 *  @func     Iss_platformDeviceInit                                               
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
Int32 Iss_platformDeviceInit(Iss_PlatformDeviceInitParams * initPrms)
{
    Int32 retVal = FVID2_SOK;

    Iss_PlatformDeviceInitParams localInitPrms;

    if (NULL == initPrms)
    {
        initPrms = &localInitPrms;
        localInitPrms.isI2cInitReq = TRUE;
    }
    retVal |= Iss_platformTI814xDeviceInit(initPrms);

    return (retVal);
}

/* De-Init EVM related sub-systems */
/* ===================================================================
 *  @func     Iss_platformDeviceDeInit                                               
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
Int32 Iss_platformDeviceDeInit(void)
{
    Int32 retVal = FVID2_SOK;

    /* Unregister FVID2 driver */
    // FVID2_unRegisterDriver(&gIssPlatformCommonObj.fvidDrvOps);
    /* Delete semaphore's */
    // Semaphore_delete( &gIssPlatformCommonObj.glock);

    retVal = Iss_platformTI814xDeviceDeInit();
    return (retVal);
}

/* ===================================================================
 *  @func     Iss_platformGetI2cInstId                                               
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
UInt32 Iss_platformGetI2cInstId(void)
{
    UInt32 i2cInst = 0u;

    i2cInst = Iss_platformTI814xGetI2cInstId();
    return (i2cInst);
}

/* ===================================================================
 *  @func     Iss_platformGetSensorI2cAddr                                               
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
UInt8 Iss_platformGetSensorI2cAddr(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt8 devAddr = 0u;

    devAddr = Iss_platformTI814xGetSensorI2cAddr(vidDecId, vipInstId);
    return (devAddr);
}


/* ===================================================================
 *  @func     Iss_platformGetCpuRev                                               
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
Iss_PlatformCpuRev Iss_platformGetCpuRev(void)
{
    return (gIssPlatformCommonObj.cpuRev);
}

/* ===================================================================
 *  @func     Iss_platformGetBaseBoardRev                                               
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
Iss_PlatformBoardRev Iss_platformGetBaseBoardRev(void)
{
    if (ISS_PLATFORM_BOARD_REV_MAX == gIssPlatformCommonObj.baseBoardRev)
    {
        gIssPlatformCommonObj.baseBoardRev =
            Iss_platformTI814xGetBaseBoardRev();
    }
    return (gIssPlatformCommonObj.baseBoardRev);
}

/* ===================================================================
 *  @func     Iss_platformGetDcBoardRev                                               
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
Iss_PlatformBoardRev Iss_platformGetDcBoardRev(void)
{
    if (ISS_PLATFORM_BOARD_REV_MAX == gIssPlatformCommonObj.dcBoardRev)
    {
        gIssPlatformCommonObj.dcBoardRev = Iss_platformTI814xGetDcBoardRev();
    }

    return (gIssPlatformCommonObj.dcBoardRev);
}

/* ===================================================================
 *  @func     Iss_platformGetId                                               
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
Iss_PlatformId Iss_platformGetId(void)
{
    return (gIssPlatformCommonObj.platformId);
}

/* ===================================================================
 *  @func     Iss_platformIsEvm                                               
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
UInt32 Iss_platformIsEvm(void)
{
    return (gIssPlatformCommonObj.isPlatformEvm);
}

/* ===================================================================
 *  @func     Iss_platformIsSim                                               
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
UInt32 Iss_platformIsSim(void)
{
    return (gIssPlatformCommonObj.isPlatformEvm ? FALSE : TRUE);
}

/* ===================================================================
 *  @func     Iss_platformSelectVideoDecoder                                               
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
/* ===================================================================
 *  @func     Iss_platformSelectSensor                                               
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
Int32 Iss_platformSelectSensor(UInt32 vidDecId, UInt32 vipInstId)
{
    Int32 status = FVID2_SOK;

    status = Iss_platformTI814xSelectSensor(vidDecId, vipInstId);
    return status;
}

/* ===================================================================
 *  @func     Iss_platformGetBoardId                                               
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
Iss_PlatformBoardId Iss_platformGetBoardId(void)
{

    gIssPlatformCommonObj.platBoardId = ISS_PLATFORM_BOARD_VCAM;
    return (gIssPlatformCommonObj.platBoardId);

}
