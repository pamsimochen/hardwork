/** ==================================================================
 *  @file   issdrv_alg2APriv.h
 *
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/drivers/alg/2A/inc/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_CAPT_ALG_2A_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_CAPT_ALG_2A_H_

#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/iss/drivers/capture/issdrv_capture.h>
#include <ti/psp/iss/core/isp_msp.h>

#define ISSALG_CAPT_TSK_PRI_2A			(9)
#define ISSALG_CAPT_TSK_STACK_2A		(16 * KB)

#define NUM_MAX_FOCUS					(4)
#define AF_PAX_VT_CNT					(16)
#define AF_PAX_HZ_CNT					(6)

typedef enum
{
    AEWB_ID_NONE = 0,
    AEWB_ID_APPRO,
    AEWB_ID_TI,
    AEWB_ID_MAXNUM = 3

}AEWB_VENDOR;


typedef struct
{
    Task_Handle taskHndl;
    Semaphore_Handle sem;

    FVID2_Handle SensorHandle;
    FVID2_Handle cameraVipHandle;

    AEWB_VENDOR aewbVendor;
    UInt32      aewbMode;
	UInt32		aewbModeFlag;
    UInt32      aewbPriority;

    MSP_IspH3aCfgT           ispH3aCfg;
    MSP_IspConfigProcessingT *pIssConfig;
    MSP_COMPONENT_TYPE       *pIspHandle;
    Void                     *pAlgHndl;

    UInt32 h3aBufAddr[2];
    UInt32 fullH3ABufAddr;
    UInt32 curH3aBufIdx;
    UInt32 h3aBufSize;
    Void   *pH3AMem;

    UInt32 aewbNumWinH;
    UInt32 aewbNumWinV;
    UInt32 aewbNumPix;

    volatile UInt32 exitFlag;

    Int32 AutoIris;
    Int32 saturation;
    Int32 sharpness;
    Int32 brightness;
    Int32 contrast;
    Int32 blc;
    Int32 AWBMode;
    Int32 AEMode;
    Int32 Env;
    Int32 Binning;
    Int32 FrameRate;

    UInt8  *dcc_Default_Param;
    Int32  dcc_init_done;
    UInt32 dccSize;
    UInt32 AFValue;
    UInt32 AEWBValue1;
    UInt32 AEWBValue2;
}Iss_2AObj;

typedef struct
{
	UInt32 focusValue;
	UInt32 sumValue;
	UInt8 x;
	UInt8 y;

} Iss_2AKMaxParams;

typedef struct
{
	UInt32 focusValue; 						// average focus value among NUM_MAX_FOCUS paxels that have the highest focus values
	UInt32 lumValue; 						// average luminance value among NUM_MAX_FOCUS paxels that have the highest focus values
	UInt32 globalLumValue;
	UInt32 maxFocusValues[NUM_MAX_FOCUS]; 	// x coordinate of the paxels which have the maximum focus value, in decreasing order
	UInt32 maxLumValues[NUM_MAX_FOCUS];
	UInt8 locMax_x[NUM_MAX_FOCUS]; 			// x coordinate of the paxels which have the maximum focus value, in decreasing order
	UInt8 locMax_y[NUM_MAX_FOCUS]; 			// y coordinate of the paxels which have the maximum focus value, in decreasing order
	UInt16 offsetRowAfPaxData[32];

	UInt8 *pCurAFDataAddr;
	UInt32 firstTime;

} Iss_2AFocusStatistics;

/* Function prototypes */

/* ===================================================================
 *  @func     IssAlg_capt2AInit
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
Int32 IssAlg_capt2AInit(Iss_2AObj *pObj, UInt32 bSetDefaultFlag);
/* ===================================================================
 *  @func     IssAlg_capt2ADeInit
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
Int32 IssAlg_capt2ADeInit(Iss_2AObj *pObj);
/* ===================================================================
 *  @func     IssAlg_capt2AIsrCallBack
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
Int32 IssAlg_capt2AIsrCallBack(Iss_2AObj *pObj);

/* Set functions */
/* ===================================================================
 *  @func     IssAlg_capt2ASetColor
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
Int32 IssAlg_capt2ASetColor(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetAEWBVendor
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
Int32 IssAlg_capt2ASetAEWBVendor(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetAEWBMode
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
Int32 IssAlg_capt2ASetAEWBMode(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetIris
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
Int32 IssAlg_capt2ASetIris(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetFrameRate
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
Int32 IssAlg_capt2ASetFrameRate(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetAEWBPri
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
Int32 IssAlg_capt2ASetAEWBPri(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetSharpness
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
Int32 IssAlg_capt2ASetSharpness(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetBlc
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
Int32 IssAlg_capt2ASetBlc(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetAwbMode
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
Int32 IssAlg_capt2ASetAwbMode(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetAeMode
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
Int32 IssAlg_capt2ASetAeMode(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetEnv
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
Int32 IssAlg_capt2ASetEnv(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetBinning
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
Int32 IssAlg_capt2ASetBinning(Fdrv_Handle handle,Ptr cmdArgs);
/* ===================================================================
 *  @func     IssAlg_capt2ASetDccPrm
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
Int32 IssAlg_capt2ASetDccPrm(Fdrv_Handle handle,Ptr cmdArgs);

/* Get functions */
/* ===================================================================
 *  @func     IssAlg_capt2AGetAEWBVendor
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
AEWB_VENDOR IssAlg_capt2AGetAEWBVendor();

Int32 ALG_afRun(void *h3aDataVirtAddr);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif // _ISS_CAPT_ALG_2A_H_
