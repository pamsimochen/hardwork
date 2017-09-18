/** ==================================================================
 *  @file   issdrv_imx122Priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/imx122/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_IMX122_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_IMX122_PRIV_H_

#include <string.h>
#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/imx122/issdrv_imx122.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

/* Driver object state - NOT IN USE */
#define ISS_IMX122_OBJ_STATE_UNUSED   (0)

/* Driver object state - IN USE and IDLE */
#define ISS_IMX122_OBJ_STATE_IDLE     (1)

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

} Iss_Imx122Obj;

/*
 * Glabal driver object */
typedef struct {
    FVID2_DrvOps fvidDrvOps;                               /* FVID2 driver
                                                            * ops */

    Semaphore_Handle lock;                                 /* global driver
                                                            * lock */

    Iss_Imx122Obj Imx122Obj[ISS_DEVICE_MAX_HANDLES];
    /*
     * handle objects
     */

} Iss_Imx122CommonObj;

extern Iss_Imx122CommonObj gIss_Imx122CommonObj;

/* ===================================================================
 *  @func     imx122_spiRead                                              
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
int imx122_spiRead(UInt8 *regAddr, UInt8 *regValue, Uint8 count);

/* ===================================================================
 *  @func     imx122_spiWrite                                               
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
int imx122_spiWrite(UInt8 *regAddr, UInt8 *regValue, Uint8 count);


/* ===================================================================
 *  @func     Iss_Imx122GetChipId
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
Int32 Iss_Imx122GetChipId(Iss_Imx122Obj * pObj,
                           Iss_SensorChipIdParams * pPrm,
                           Iss_SensorChipIdStatus * pStatus);

/* ===================================================================
 *  @func     Iss_Imx122Reset
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
Int32 Iss_Imx122Reset(Iss_Imx122Obj * pObj);

/* ===================================================================
 *  @func     Iss_Imx122Start
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
Int32 Iss_Imx122Start(Iss_Imx122Obj * pObj);

/* ===================================================================
 *  @func     Iss_Imx122Stop
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
Int32 Iss_Imx122Stop(Iss_Imx122Obj * pObj);

/* ===================================================================
 *  @func     Iss_Imx122RegWrite
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
Int32 Iss_Imx122RegWrite(Iss_Imx122Obj * pObj,
                          Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Imx122RegRead
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
Int32 Iss_Imx122RegRead(Iss_Imx122Obj * pObj,
                         Iss_VideoDecoderRegRdWrParams * pPrm);

/* ===================================================================
 *  @func     Iss_Imx122LockObj
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
Int32 Iss_Imx122LockObj(Iss_Imx122Obj * pObj);

/* ===================================================================
 *  @func     Iss_Imx122UnlockObj
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
Int32 Iss_Imx122UnlockObj(Iss_Imx122Obj * pObj);

/* ===================================================================
 *  @func     Iss_Imx122Lock
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
Int32 Iss_Imx122Lock();

/* ===================================================================
 *  @func     Iss_Imx122Unlock
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
Int32 Iss_Imx122Unlock();

/* ===================================================================
 *  @func     Iss_Imx122AllocObj
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
Iss_Imx122Obj *Iss_Imx122AllocObj();

/* ===================================================================
 *  @func     Iss_Imx122FreeObj
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
Int32 Iss_Imx122FreeObj(Iss_Imx122Obj * pObj);

/* ===================================================================
 *  @func     Iss_Imx122PowerDown
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
Int32 Iss_Imx122PowerDown(Iss_Imx122Obj * pObj, UInt32 powerDown);

/* ===================================================================
 *  @func     Iss_Imx122OutputEnable
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
Int32 Iss_Imx122OutputEnable(Iss_Imx122Obj * pObj, UInt32 enable);

/* ===================================================================
 *  @func     Imx122_GainTableMap
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
static UInt8 Imx122_GainTableMap(int sensorGain);

/* ===================================================================
 *  @func     Iss_Imx122UpdateExpGain
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
Int32 Iss_Imx122UpdateExpGain(Iss_Imx122Obj * pObj, Ptr createArgs);

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

/*
 * Update ITT Values */
/* ===================================================================
 *  @func     Iss_Imx122UpdateItt
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
Int32 Iss_Imx122UpdateItt(Iss_Imx122Obj * pObj,
                           Itt_RegisterParams * gItt_RegisterParams);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_IMX122_PRIV_H_
                                                            */
