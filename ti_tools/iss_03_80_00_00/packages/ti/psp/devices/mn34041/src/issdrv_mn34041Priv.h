/** ==================================================================
 *  @file   issdrv_mn34041Priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/mn34041/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_MN34041_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_MN34041_PRIV_H_

#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/mn34041/issdrv_mn34041.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

/* Driver object state - NOT IN USE */
#define ISS_MN34041_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_MN34041_OBJ_STATE_IDLE     (1)

/*
 * Driver handle object */
typedef struct {

    UInt32 state;                                          /* handle state */

    UInt32 handleId;                                       /* handle ID,
                                                            * 0..ISS_DEVICE_MAX_HANDLES-1
                                                            */

    Semaphore_Handle lock;                                 /* handle lock */

    Iss_SensorCreateParams createArgs;                     /* create time
                                                            * arguments */

    UInt8 regCache[2][256];                                /* register read
                                                            * cache */

} Iss_Mn34041Obj;

/*
 * Glabal driver object */
typedef struct {
    FVID2_DrvOps fvidDrvOps;                               /* FVID2 driver
                                                            * ops */

    Semaphore_Handle lock;                                 /* global driver
                                                            * lock */

    Iss_Mn34041Obj Mn34041Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Mn34041CommonObj;

extern Iss_Mn34041CommonObj gIss_Mn34041CommonObj;

void mn34_041_setgio(unsigned short addr, unsigned short reg);
/* ===================================================================
 *  @func     Iss_Mn34041GetChipId
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
Int32 Iss_Mn34041GetChipId(Iss_Mn34041Obj * pObj,
                           Iss_SensorChipIdParams * pPrm,
                           Iss_SensorChipIdStatus * pStatus);

/* ===================================================================
 *  @func     Iss_Mn34041Reset
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
Int32 Iss_Mn34041Reset(Iss_Mn34041Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mn34041Start
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
Int32 Iss_Mn34041Start(Iss_Mn34041Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mn34041Stop
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
Int32 Iss_Mn34041Stop(Iss_Mn34041Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mn34041RegWrite
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
Int32 Iss_Mn34041RegWrite(Iss_Mn34041Obj * pObj,
                          Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Mn34041RegRead
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
Int32 Iss_Mn34041RegRead(Iss_Mn34041Obj * pObj,
                         Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Mn34041LockObj
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
Int32 Iss_Mn34041LockObj(Iss_Mn34041Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mn34041UnlockObj
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
Int32 Iss_Mn34041UnlockObj(Iss_Mn34041Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mn34041Lock
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
Int32 Iss_Mn34041Lock();

/* ===================================================================
 *  @func     Iss_Mn34041Unlock
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
Int32 Iss_Mn34041Unlock();

/* ===================================================================
 *  @func     Iss_Mn34041AllocObj
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
Iss_Mn34041Obj *Iss_Mn34041AllocObj();

/* ===================================================================
 *  @func     Iss_Mn34041FreeObj
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
Int32 Iss_Mn34041FreeObj(Iss_Mn34041Obj * pObj);

/* ===================================================================
 *  @func     Iss_Mn34041PowerDown
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
Int32 Iss_Mn34041PowerDown(Iss_Mn34041Obj * pObj, UInt32 powerDown);

/* ===================================================================
 *  @func     Iss_Mn34041OutputEnable
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
Int32 Iss_Mn34041OutputEnable(Iss_Mn34041Obj * pObj, UInt32 enable);

/* ===================================================================
 *  @func     Mn34041_GainTableMap
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
static int Mn34041_GainTableMap(int sensorGain);
static Uint16 Mn34041_DGainTableMap(int sensorGain);

/* ===================================================================
 *  @func     Iss_Mn34041UpdateExpGain
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
Int32 Iss_Mn34041UpdateExpGain(Iss_Mn34041Obj * pObj, Ptr createArgs);

/* ===================================================================
 *  @func     Transplant_DRV_imgsSetEshutter
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
int Transplant_DRV_imgsSetEshutter();

/* ===================================================================
 *  @func     Mn34041_GainTableMap
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
static int Mn34041_GainTableMap(int again)
{
    int theRetGain;

    if (again >= 8000)
    {
        theRetGain = 0xC0C0;
    }
    else if (again >= 4000)
    {
        theRetGain = 0xC080;
    }
    else if (again >= 2000)
    {
        theRetGain = 0x8080;
    }
    else
    {
        theRetGain = 0x0080;
    }

    return theRetGain;
}

static Uint16 Mn34041_DGainTableMap(int again)
{
    int theRetGain;

    if (again >= 8000) /**/
    {
		theRetGain = again / 8;
    }
    else if (again >= 4000)  /**/
    {
		theRetGain = again / 4;
    }
    else if (again >= 2000) /**/
    {
        theRetGain = again / 2;
    }
    else  /**/
    {
        theRetGain = again;
    }

	if(theRetGain >= 4000) theRetGain = 0x0100;
	else if(theRetGain >= 3900) theRetGain = 0x00FE;
	else if(theRetGain >= 3800) theRetGain = 0x00FC;
	else if(theRetGain >= 3700) theRetGain = 0x00FA;
	else if(theRetGain >= 3600) theRetGain = 0x00F7;
	else if(theRetGain >= 3500) theRetGain = 0x00F4;
	else if(theRetGain >= 3400) theRetGain = 0x00F1;
	else if(theRetGain >= 3300) theRetGain = 0x00EF;
	else if(theRetGain >= 3200) theRetGain = 0x00EC;
	else if(theRetGain >= 3100) theRetGain = 0x00E9;
	else if(theRetGain >= 3000) theRetGain = 0x00E5;
	else if(theRetGain >= 2900) theRetGain = 0x00E2;
	else if(theRetGain >= 2800) theRetGain = 0x00DF;
	else if(theRetGain >= 2700) theRetGain = 0x00DC;
	else if(theRetGain >= 2600) theRetGain = 0x00D9;
	else if(theRetGain >= 2500) theRetGain = 0x00D6;
	else if(theRetGain >= 2400) theRetGain = 0x00D1;
	else if(theRetGain >= 2300) theRetGain = 0x00CD;
	else if(theRetGain >= 2200) theRetGain = 0x00C9;
	else if(theRetGain >= 2100) theRetGain = 0x00C5;
	else if(theRetGain >= 2000) theRetGain = 0x00C0;
	else if(theRetGain >= 1900) theRetGain = 0x00BC;
	else if(theRetGain >= 1800) theRetGain = 0x00B7;
	else if(theRetGain >= 1700) theRetGain = 0x00B2;
	else if(theRetGain >= 1600) theRetGain = 0x00AC;
	else if(theRetGain >= 1500) theRetGain = 0x00A6;
	else if(theRetGain >= 1400) theRetGain = 0x00A0;
	else if(theRetGain >= 1300) theRetGain = 0x0099;
	else if(theRetGain >= 1200) theRetGain = 0x0091;
	else if(theRetGain >= 1100) theRetGain = 0x0089;
	else theRetGain = 0x80;

    return theRetGain;
}

Int32 Iss_Mn34041FrameRateSet(Iss_Mn34041Obj * pObj, Ptr createArgs, Ptr cmdStatusArgs);
Uint32 Iss_Mn34041PwmVideoSet(Uint32 load_value, Uint32 match_value);
Uint32 Iss_Mn34041PwmDcSet(Uint32 load_value, Uint32 match_value);
Int32 Iss_Mn34041FrameRateSet(Iss_Mn34041Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs);
Int32 Iss_Mn34041UpdateItt(Iss_Mn34041Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_MN34041_PRIV_H_
                                                            */
