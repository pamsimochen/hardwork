/** ==================================================================

 *  @file   issdrv_alg2AApi.c.c

 *

 *  @path   /ti/psp/iss/drivers/capture/src/

 *

 *  @desc   This  File contains.

 * ===================================================================

 *  Copyright (c) Texas Instruments Inc 2011, 2012

 *

 *  Use of this software is controlled by the terms and conditions found

 *  in the license agreement under which this software has been supplied

 * ===================================================================*/





#include <xdc/std.h>

#include <stdlib.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_alg2APriv.h>

#include <ti/psp/iss/core/inc/iss_drv_common.h>

#include <ti/psp/examples/utility/vpsutils_mem.h>

#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>

#include <ti/psp/devices/iss_sensorDriver.h>

#include <ti/psp/iss/alg/aewb/appro2a/inc/alg_appro_aewb.h>

#include <ti/psp/iss/alg/dcc/inc/idcc.h>
#include <ti/psp/iss/drivers/capture/src/issdrv_capturePriv.h>



extern ti2a_output ti2a_output_params;

extern ti2aControlParams_t gTi2aControlParams;

extern const ipipe_gbce_cfg_t gbce_iss_default_params;



Iss_2AObj *gpIssAlg2AObj;

Iss_2AFocusStatistics gFocusStatistics = {.firstTime = 1};

/* Stack for 2A task */



#pragma DATA_ALIGN(gIssAlg_captTskStack2A, 32)

#pragma DATA_SECTION(gIssAlg_captTskStack2A, ".bss:taskStackSection")

UInt8 gIssAlg_captTskStack2A[ISSALG_CAPT_TSK_STACK_2A];

UInt32 rgb_3d_lut_table[729];
const uint32 iss_3d_lut_r_table_6500K[] = {
	#include "./3DLUT/Camera_3DLUT_6500K_R.txt"
};
const uint32 iss_3d_lut_g_table_6500K[] = { 
	#include "./3DLUT/Camera_3DLUT_6500K_G.txt" 
};
const uint32 iss_3d_lut_b_table_6500K[] = { 
	#include "./3DLUT/Camera_3DLUT_6500K_B.txt" 
};

const uint32 iss_3d_lut_r_table_5000K[] = {
	#include "./3DLUT/Camera_3DLUT_5000K_R.txt"
};
const uint32 iss_3d_lut_g_table_5000K[] = { 
	#include "./3DLUT/Camera_3DLUT_5000K_G.txt" 
};
const uint32 iss_3d_lut_b_table_5000K[] = { 
	#include "./3DLUT/Camera_3DLUT_5000K_B.txt" 
};

const uint32 iss_3d_lut_r_table_A[] = { 
	#include "./3DLUT/Camera_3DLUT_A_R.txt" 
};
const uint32 iss_3d_lut_g_table_A[] = { 
	#include "./3DLUT/Camera_3DLUT_A_G.txt" 
};
const uint32 iss_3d_lut_b_table_A[] = { 
	#include "./3DLUT/Camera_3DLUT_A_B.txt" 
};

const uint32 iss_3d_lut_r_table_TL84[] = { 
	#include "./3DLUT/Camera_3DLUT_TL84_R.txt" 
};
const uint32 iss_3d_lut_g_table_TL84[] = { 
	#include "./3DLUT/Camera_3DLUT_TL84_G.txt" 
};
const uint32 iss_3d_lut_b_table_TL84[] = { 
	#include "./3DLUT/Camera_3DLUT_TL84_B.txt" 
};
//#define TWOA_PROFILE

#ifdef TWOA_PROFILE
    extern UInt32 Utils_getCurTimeInMsec();
#endif

/* H3A ISR call back function */



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
Int32 IssAlg_capt2AIsrCallBack(Iss_2AObj *pObj)

{

    /* save the full H3A buf address */

    pObj->fullH3ABufAddr = pObj->h3aBufAddr[pObj->curH3aBufIdx];



    /* post the 2A sem */

    Semaphore_post(pObj->sem);



    /* switch the H3A buffer */

    pObj->curH3aBufIdx = pObj->curH3aBufIdx?0:1;



    h3a_config_aewb_op_addr(pObj->h3aBufAddr[pObj->curH3aBufIdx]);

    h3a_config_af_op_addr(pObj->h3aBufAddr[pObj->curH3aBufIdx] + pObj->h3aBufSize);

    return FVID2_SOK;

}



/****************************************************************************************

                                SET FUNCTIONS

****************************************************************************************/



/* Set color */



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
Int32 IssAlg_capt2ASetColor(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;

    Iss_CaptColor *pColorPrm = (Iss_CaptColor*)cmdArgs;



    pObj->saturation = pColorPrm->saturation;

    pObj->contrast   = pColorPrm->contrast;

    pObj->brightness = pColorPrm->brightness;



    return FVID2_SOK;

}



/* Set AEWB Vendor */



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
Int32 IssAlg_capt2ASetAEWBVendor(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->aewbVendor = (AEWB_VENDOR)(*((UInt32*)cmdArgs));



    return FVID2_SOK;

}



/* Set AEWB mode */



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
Int32 IssAlg_capt2ASetAEWBMode(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->aewbMode = *((UInt32*)cmdArgs);

    pObj->aewbModeFlag = 1;

    return FVID2_SOK;

}



/* Set Iris */



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
Int32 IssAlg_capt2ASetIris(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->AutoIris = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set Frame Rate */



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
Int32 IssAlg_capt2ASetFrameRate(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->FrameRate = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set AEWB Priority */



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
Int32 IssAlg_capt2ASetAEWBPri(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->aewbPriority = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set sharpness */



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
Int32 IssAlg_capt2ASetSharpness(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->sharpness = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set BLC */



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
Int32 IssAlg_capt2ASetBlc(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->blc = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set AWB Mode */



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
Int32 IssAlg_capt2ASetAwbMode(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->AWBMode = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set AE mode */



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
Int32 IssAlg_capt2ASetAeMode(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->AEMode = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set ENV */



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
Int32 IssAlg_capt2ASetEnv(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->Env = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set Binning */



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
Int32 IssAlg_capt2ASetBinning(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    pObj->Binning = *((UInt32*)cmdArgs);



    return FVID2_SOK;

}



/* Set DCC param */



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
Int32 IssAlg_capt2ASetDccPrm(Fdrv_Handle handle,Ptr cmdArgs)

{

    Iss_2AObj *pObj = gpIssAlg2AObj;

    Iss_CaptDccPrm *pDccPrm = (Iss_CaptDccPrm*)cmdArgs;



    pObj->dcc_Default_Param = pDccPrm->dcc_Default_Param;

    pObj->dcc_init_done     = pDccPrm->dcc_init_done;

    pObj->dccSize           = pDccPrm->dccSize;



    return FVID2_SOK;

}



/****************************************************************************************

                                GET FUNCTIONS

****************************************************************************************/



/* Get AEWB vendor */



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
AEWB_VENDOR IssAlg_capt2AGetAEWBVendor()

{

    Iss_2AObj *pObj = gpIssAlg2AObj;



    return (pObj->aewbVendor);

}



/****************************************************************************************

                        TI 2A functions

*****************************************************************************************/



void TI_2A_UpdateDynamicParams1(Iss_2AObj *pObj)

{



    static int flickerFreq = 0;



    if (gTi2aControlParams.brightness != pObj->brightness ||

        gTi2aControlParams.contrast != pObj->contrast)

    {

        gTi2aControlParams.brightness = pObj->brightness;

        gTi2aControlParams.contrast = pObj->contrast;

        gTi2aControlParams.update |= TI2A_UPDATE_CNT_BRGT;

    }



    if (gTi2aControlParams.saturation != pObj->saturation)

    {

        gTi2aControlParams.saturation = pObj->saturation;

        gTi2aControlParams.update |= TI2A_UPDATE_SATURATION;

    }



    if (gTi2aControlParams.sharpness != pObj->sharpness)

    {

        gTi2aControlParams.sharpness = pObj->sharpness;

        gTi2aControlParams.update |= TI2A_UPDATE_SHARPNESS;

    }



    if (gTi2aControlParams.day_night == pObj->aewbPriority)

    {

        gTi2aControlParams.day_night = !(pObj->aewbPriority);

        gTi2aControlParams.update |= TI2A_UPDATE_AE_DAY_NIGHT;

    }



    if (gTi2aControlParams.blc != pObj->blc)

    {

        gTi2aControlParams.blc = pObj->blc;

        gTi2aControlParams.update |= TI2A_UPDATE_BLC;

    }



    if (pObj->Env != flickerFreq)

    {

        flickerFreq = pObj->Env;

        switch (flickerFreq)

        {                                                  /* <TBR: Uday>Sel

                                                            * between 50/60Hz

                                                            * * flicker */

            case 0:

                gTi2aControlParams.flickerFreq = 60;

                break;

            case 1:

                gTi2aControlParams.flickerFreq = 50;

                break;

            default:

                gTi2aControlParams.flickerFreq = 0;

                break;

        }

        gTi2aControlParams.update |= TI2A_UPDATE_CONTROL_PARAMS_2A;

    }


    if(pObj->aewbModeFlag == 1){
        gTi2aControlParams.aewbType = pObj->aewbMode;
        pObj->aewbModeFlag = 0;
    // 0=off, 1=AE,
    }
    // 2=AWB, 3=AE+AWB

    gTi2aControlParams.wbSceneMode = pObj->AWBMode;

}

#define NUM_RGB2RGB_MATRIXES 4
static int RGB2RGB_stablize(int curr_RGB2RGBIndex, int reset)
{
#define AWB_AVG_BUF_LENGTH 12
    static int history_index[AWB_AVG_BUF_LENGTH];
    static int awb_count = 1;
    static int prev_RGB2RGBIndex = 0;

    int i;
    int max;
    int max_index = 0;
    int history_length;
    int index_histogram[NUM_RGB2RGB_MATRIXES];

    if (reset == 1) {
        awb_count = 1;
        prev_RGB2RGBIndex = 0;
    }

    if (awb_count < AWB_AVG_BUF_LENGTH) {
        history_length = awb_count;
        awb_count++;
    } else {
        history_length = AWB_AVG_BUF_LENGTH;
    }

	if (history_length == AWB_AVG_BUF_LENGTH) {
        for (i = 0; i < history_length - 1; i++) {
            history_index[i] = history_index[i + 1];
        }
    }
    history_index[history_length - 1] = curr_RGB2RGBIndex;

    for (i = 0; i < NUM_RGB2RGB_MATRIXES; i++) {
        index_histogram[i] = 0;
    }
    for (i = 0; i < history_length; i++) {
        index_histogram[history_index[i]]++;
    }

    max = 0;
    for (i = 0; i < NUM_RGB2RGB_MATRIXES; i++) {
        if (index_histogram[i] > max) {
            max = index_histogram[i];
            max_index = i;
        }
    }

	if (history_length < AWB_AVG_BUF_LENGTH) {
        curr_RGB2RGBIndex = max_index;
    } else {
        if (max * 10 >= (AWB_AVG_BUF_LENGTH * 8)) {
            curr_RGB2RGBIndex = max_index;
        } else {
            curr_RGB2RGBIndex = prev_RGB2RGBIndex;
        }
    }
    prev_RGB2RGBIndex = curr_RGB2RGBIndex;
    return (curr_RGB2RGBIndex);
}


int get_index_of_matrixes(int colorTemp,  int reset)
{
    int i, diff, next_diff;
    static int prev_i = -1;
    static int color_temp[] = {2850, 4250, 5000, 6500};

    for (i = 0; i < NUM_RGB2RGB_MATRIXES - 1 && color_temp[i+1] > 0; i++)
    {
        diff = colorTemp - color_temp[i];
        next_diff = color_temp[i+1] - colorTemp;
        if ((next_diff >= 0 && diff >= 0) || diff < 0)
        {
            if (prev_i == i) diff -= 200;
            if (prev_i == i+1) next_diff -= 200;
            if (next_diff < diff)
            {
                i++;
            }
            break;
        }
    }
    i = RGB2RGB_stablize(i, reset);
	if (prev_i != i || reset)
    {
        prev_i = i;
    }
	return i;
}


/* TI 2A Process Call */


/* ===================================================================
 *  @func     IssAlg_capt2AProcessTI
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
extern int16 gamma_iss_default_table[];
Int32 IssAlg_capt2AProcessTI(Iss_2AObj *pObj)

{
    Int32 status = FVID2_SOK;
    static UInt32 dccFrmCnt = 0;

    TI_2A_UpdateDynamicParams1(pObj);
    status = ALG_aewbRun((Void*)pObj->fullH3ABufAddr);
    ALG_aewbControl();
    if (status == FVID2_SOK)
    {
        // AE write to SENSOR
        status =
            FVID2_control(pObj->SensorHandle, IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN,
                          &ti2a_output_params, NULL);
        if (status != FVID2_SOK)
        {
            Vps_rprintf("\nAE write to sensor failed!\n");
            return status;
        }

        pObj->AEWBValue1 = ti2a_output_params.sensorExposure/100;  //10 bit
        pObj->AEWBValue2 = (ti2a_output_params.sensorGain / 100);
        pObj->AEWBValue2 *= (ti2a_output_params.ipipe_awb_gain.dGain *40/1024); //20 bit
        // AWB write to ISP

        status =
            Iss_captControl(pObj->cameraVipHandle, IOCTL_ISS_ALG_2A_UPDATE,
                            &ti2a_output_params, NULL);

        if (status != FVID2_SOK)
        {
            Vps_rprintf("AWB write to ISP failed!\n");
            return status;
        }
    } else {
        Vps_rprintf("2A ALG run failed!\n");
    }

    /* Added for DCC Parser */
    if (pObj->aewbVendor == AEWB_ID_TI)
    {
        if (((dccFrmCnt % 32) == 0) && (pObj->dcc_init_done == TRUE))
        {
            dcc_parser_input_params_t input_params;
            dcc_parser_output_params_t output_params;

            extern iss_drv_config_t *iss_drv_config;
            static int first_time = 0;
            if(first_time == 0)
            {
              extern volatile int new_dcc_data_available;
              new_dcc_data_available = 1;

              iss_drv_config->isif_cfg.lsc_params = (isif_2dlsc_cfg_t*)gIss_captCommonObj.pIssConfig->ptLsc2D;

              first_time = 1;
            }
            output_params.iss_drv_config   = iss_drv_config;
            input_params.dcc_buf           = (Int8 *)pObj->dcc_Default_Param;
            input_params.dcc_buf_size      = pObj->dccSize;
            input_params.color_temparature = ti2a_output_params.colorTemparaure;
            input_params.exposure_time     = ti2a_output_params.sensorExposure;
            input_params.analog_gain       = (ti2a_output_params.sensorGain / 1000);
            input_params.analog_gain       *= (ti2a_output_params.ipipe_awb_gain.dGain *4/1024);
            output_params.pVnfParam   = NULL;
            output_params.pGlbceParams   = NULL;
			output_params.pMctnfParam   = NULL;

            dcc_update(&input_params, &output_params);

            output_params.iss_drv_config->ipipe_cfg.rgb_yuv_params->offset[0] =
                gTi2aControlParams.brightness - 128;
            output_params.iss_drv_config->ipipe_cfg.rgb_yuv_params->contrast =
                (gTi2aControlParams.contrast >> 3);
            output_params.iss_drv_config->ipipe_cfg.filter_flag |=
                (IPIPE_RGB_TO_YUV_FLAG);

#if defined(IMGS_MICRON_MT9M034) | defined(IMGS_SONY_IMX104) | defined(IMGS_SONY_IMX136) |  defined(IMGS_SONY_IMX140)
			/* shaprness adjustment sample code*/
            if( (gTi2aControlParams.sharpness > 0 || gTi2aControlParams.sharpness < 256) &&
                    (ti2a_output_params.sensorGain <= 16000))
            {
                output_params.iss_drv_config->ipipe_cfg.ee_param->enable = 1;
                output_params.iss_drv_config->ipipe_cfg.ee_param->halo_reduction = IPIPE_HALO_REDUCTION_ENABLE;
                output_params.iss_drv_config->ipipe_cfg.ee_param->sel = 1;

                output_params.iss_drv_config->ipipe_cfg.ee_param->gain = gTi2aControlParams.sharpness/2 ;
                output_params.iss_drv_config->ipipe_cfg.ee_param->hpf_low_thr = 100 ;
                output_params.iss_drv_config->ipipe_cfg.ee_param->hpf_high_thr = 30 ;
                output_params.iss_drv_config->ipipe_cfg.ee_param->hpf_gradient_gain =
                         output_params.iss_drv_config->ipipe_cfg.ee_param->gain >> 1 +
                                output_params.iss_drv_config->ipipe_cfg.ee_param->gain ;
                output_params.iss_drv_config->ipipe_cfg.ee_param->hpf_gradient_offset = 24;
                output_params.iss_drv_config->ipipe_cfg.filter_flag |= (IPIPE_EE_FLAG);
            } else {
                output_params.iss_drv_config->ipipe_cfg.ee_param->enable = 0;
                output_params.iss_drv_config->ipipe_cfg.filter_flag |= (IPIPE_EE_FLAG);
            }
#endif

			/*3DLut sample code*/
			int i3dlut = 0;
			static int indexPrev = -1;
			int index = get_index_of_matrixes(ti2a_output_params.colorTemparaure, 0);

			if(index != indexPrev)
			{
				Vps_printf("CT: %d, 3dlut index:%d \n", ti2a_output_params.colorTemparaure, index);
				if(index == 3)
				{
					for(i3dlut = 0 ; i3dlut< 729 ; i3dlut ++) {
						rgb_3d_lut_table[i3dlut] = (iss_3d_lut_r_table_6500K[i3dlut] & 0x3FF) << 20;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_g_table_6500K[i3dlut]& 0x3FF) << 10;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_b_table_6500K[i3dlut]& 0x3FF);
					}
				} else if(index == 2) {
					for(i3dlut = 0 ; i3dlut< 729 ; i3dlut ++) {
						rgb_3d_lut_table[i3dlut] = (iss_3d_lut_r_table_5000K[i3dlut] & 0x3FF) << 20;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_g_table_5000K[i3dlut]& 0x3FF) << 10;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_b_table_5000K[i3dlut]& 0x3FF);
					}
				} else if(index == 1 ) {
					for(i3dlut = 0 ; i3dlut< 729 ; i3dlut ++) {
						rgb_3d_lut_table[i3dlut] = (iss_3d_lut_r_table_TL84[i3dlut] & 0x3FF) << 20;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_g_table_TL84[i3dlut]& 0x3FF) << 10;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_b_table_TL84[i3dlut]& 0x3FF);
					}
				} else if(index == 0) { 
					for(i3dlut = 0 ; i3dlut< 729 ; i3dlut ++) {
						rgb_3d_lut_table[i3dlut] = (iss_3d_lut_r_table_A[i3dlut] & 0x3FF) << 20;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_g_table_A[i3dlut]& 0x3FF) << 10;
						rgb_3d_lut_table[i3dlut] |= (iss_3d_lut_b_table_A[i3dlut]& 0x3FF);
					}
				}

				MSP_IspProcCfgT tIspParamCfg;
				tIspParamCfg.ptIssConfig  = gIss_captCommonObj.pModuleInstance->tConfigParams.ptIssConfig;
				tIspParamCfg.ptIssConfig->pt3Dcc->enable = 0; /* enable 3dlut:1 */
				tIspParamCfg.ptIssConfig->pt3Dcc->pInBiffData = &rgb_3d_lut_table;
				tIspParamCfg.ptIssConfig->eFProcIpipeValidity =(MSP_PROC_IPIPE_VALID_ID)(MSP_PROC_IPIPE_VALID_3DCC);
				MSP_ISP_config(gIss_captCommonObj.pModuleInstance->hIspHandle,
               		    (MSP_INDEXTYPE) MSP_ISP_CFG_IPIPE, &tIspParamCfg);
			}
			indexPrev = index;

            if(gTi2aControlParams.saturation != 128)

            {

              int K = ((gTi2aControlParams.saturation * 1024) / 128);





              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[0] +=

              ((717 * (K - 1024)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[1] +=

              ((601 * (1024 - K)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[2] +=

              ((117 * (1024 - K)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[3] +=

              ((601 * (1024 - K)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[4] +=

              ((717 * (K - 1024)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[5] +=

              ((117 * (1024 - K)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[6] +=

                ((306 * (1024 - K)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[7] +=

              ((311 * (1024 - K)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.rgb_rgb1_params->mul_off[8] +=

              ((717 * (K - 1024)) * 256 )/(1024 * 1024);



              output_params.iss_drv_config->ipipe_cfg.filter_flag |=

                    (IPIPE_RGB_RGB_1_FLAG);

            }

#if 0
            /*GBCE/GAMMA adjustment Sample Code*/
            output_params.iss_drv_config->ipipe_cfg.gbce_params = (ipipe_gbce_cfg_t* )&gbce_iss_default_params;
            output_params.iss_drv_config->ipipe_cfg.filter_flag |= (IPIPE_GBCE_FLAG);

            static ipipe_gamma_cfg_t gamma_cfg = 
            {IPIPE_GAMMA_TBL_512, 0, IPIPE_GAMMA_BYPASS_DISABLE, IPIPE_GAMMA_BYPASS_DISABLE, IPIPE_GAMMA_BYPASS_DISABLE, NULL, NULL, NULL};
            gamma_cfg.red_table = gamma_cfg.green_table = gamma_cfg.blue_table = (int16 *)gamma_iss_default_table;
            ipipe_config_gamma(&gamma_cfg);
#endif

            ipipe_config_filters(&output_params.iss_drv_config->ipipe_cfg);

            output_params.iss_drv_config->ipipe_cfg.filter_flag = 0;

            output_params.iss_drv_config->isif_cfg.feature_flag &= ~ISIF_SDRAM_OP_FLAG;
            isif_config(&output_params.iss_drv_config->isif_cfg);

            output_params.iss_drv_config->isif_cfg.feature_flag = 0;

        }

    }



    dccFrmCnt++;



    return FVID2_SOK;

}



/****************************************************************************************

                        APPRO 2A functions

*****************************************************************************************/



/* ===================================================================
 *  @func     copy4x32bit
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
static inline void copy4x32bit(Uint32 * pDest, Uint32 * pSource)

{

    *pDest++ = *pSource++;

    *pDest++ = *pSource++;

    *pDest++ = *pSource++;

    *pDest++ = *pSource++;

}



/* reorder H3A data */

void reorderH3AData1(Uint32 * pDest, Uint32 * pSource, Uint32 nPaxX,

                     Uint32 nPaxY)

{

#define SKIP_BLACK_ROW

    int nIdx1;



    int nIdx2;



    int nPaxRemain = nPaxX;



    int nBytesInThisRow = 0;



    Uint32 *pDestSumPtr = pDest;



    Uint32 *pDestUnSatPtr = pDest + 4;



#ifdef SKIP_BLACK_ROW

    nPaxY++;

#endif                                                     // SKIP_BLACK_ROW

    for (nIdx1 = 0; nIdx1 < (nPaxX * nPaxY); nIdx1++)



    {

        // Copy Paxel

        copy4x32bit(pDestSumPtr, pSource);

        pDestSumPtr += 5;

        pSource += 4;

        nBytesInThisRow += 16;

        // Copy Packet end data (unsaturated pixels)

        if (nIdx1 % 8 == 7)

        {

            for (nIdx2 = 0; nIdx2 < 4; nIdx2++)

            {

                *pDestUnSatPtr = *pSource & 0xFFFF;

                pDestUnSatPtr += 5;

                *pDestUnSatPtr = ((*pSource) >> 16) & 0xFFFF;

                pDestUnSatPtr += 5;

                pSource++;

            }

            nBytesInThisRow += 16;

        }

        nPaxRemain--;

        if (!nPaxRemain)

        {

#ifdef SKIP_BLACK_ROW

            if ((nPaxX - 1) == nIdx1)

            {

                pDestSumPtr = pDest;

                pDestUnSatPtr = pDest + 4;

            }

#endif                                                     // SKIP_BLACK_ROW

            // End of row

            if (nBytesInThisRow % 32)

            {

                pSource += 4;

            }

            nPaxRemain = nPaxX;

            nBytesInThisRow = 0;

        }



    }



    for (nIdx2 = 0; nIdx2 < (1 + (nPaxX * nPaxY) % 8) / 2; nIdx2++)

    {

        *pDestUnSatPtr = *pSource & 0xFFFF;

        pDestUnSatPtr += 5;

        *pDestUnSatPtr = ((*pSource) >> 16) & 0xFFFF;

        pDestUnSatPtr += 5;

        pSource++;

    }



}



/***************************************************************************************/

/* ===================================================================
 *  @func     IssAlg_capt2ADynamicParmAppro
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
Int32 IssAlg_capt2ADynamicParmAppro(Iss_2AObj *pObj)
{
    Int32 status = FVID2_SOK;
    static int FlgFrame = -1;
    static int FlgFlicker = -1;
    Int32 SetVal1 = 1;
    Int32 SetVal2 = 0;
    Int32 IsDbgOn = 0;
    static appro2a_output *pApproOutPrm = NULL;
    static ipipe_cfg_t * pIpipe_config = NULL;
    static isif_cfg_t * pIsif_config = NULL;

    Appro2ACtrl(APPRO_CMD_AUTO_IRIS, &pObj->AutoIris);
    Appro2ACtrl(APPRO_CMD_SATURATION,&pObj->saturation);
    Appro2ACtrl(APPRO_CMD_SHARPNESS, &pObj->sharpness);
    Appro2ACtrl(APPRO_CMD_BRIGHTNESS,&pObj->brightness);
    Appro2ACtrl(APPRO_CMD_CONTRAST,  &pObj->contrast);
    Appro2ACtrl(APPRO_CMD_BLC,       &pObj->blc);

    switch( pObj->aewbMode )
    {
        case 0:
            if( IsDbgOn ) Vps_printf("AWB Disable \n");
            SetVal1 = 0;
            SetVal2 = 0;
        break;
        case 1:
            if( IsDbgOn ) Vps_printf("AE Only \n");
            SetVal1 = 1;
            SetVal2 = 1;
        break;
        case 2:
            if( IsDbgOn ) Vps_printf("AWB Only \n");
            SetVal1 = 1;
            SetVal2 = 2;
        break;
        case 3:
            if( IsDbgOn ) Vps_printf("AEWB Enable \n");
            SetVal1 = 1;
            SetVal2 = 0;
        break;
        default :
            if( IsDbgOn ) Vps_printf("AEWB Default \n");
            SetVal1 = 1;
            SetVal2 = 0;

        break;
    }
    Appro2ACtrl(APPRO_CMD_AEWB_ENABLE, &SetVal1);
    Appro2ACtrl(APPRO_CMD_AEWB_TYPE,   &SetVal2);

    if( pObj->aewbPriority == 0 )
    {
        if( IsDbgOn ) Vps_printf("PRIORITY_FRAMERATE \n");
        Appro2ACtrl(APPRO_CMD_AE_FRAME_PRIO,   NULL);
    }
    else if( pObj->aewbPriority == 1)
    {
        if( IsDbgOn ) Vps_printf("PRIORITY_EXPOSURE \n");
        Appro2ACtrl(APPRO_CMD_AE_QUALITY_PRIO,   NULL);
    }else{
        if( IsDbgOn ) Vps_printf("PRIORITY_Default \n");
        Appro2ACtrl(APPRO_CMD_AE_QUALITY_PRIO,   NULL);
    }

    //pObj->AWBMode = 0;//0:auto 1:d65 2:d55 3:fluorescent 4:incandescent
    if( IsDbgOn ) Vps_printf("AWB MODE = %d \n",pObj->AWBMode);
    Appro2ACtrl(APPRO_CMD_AWB_MODE,   &pObj->AWBMode);


    if ((FlgFrame != pObj->FrameRate)||(FlgFlicker!=pObj->Env))
    {
        int FrameTime = 0;
        int frameRate = 30;

        if(FlgFlicker!=pObj->Env)
        {
            FlgFlicker = pObj->Env;
            Appro2ACtrl(APPRO_CMD_AE_FLICKER, &FlgFlicker);
        }

        FlgFrame = pObj->FrameRate;

        if ((FlgFrame == 30) || (FlgFrame == 60))
        {
            if(FlgFrame == 60 )
            {
                if( FlgFlicker == 0 )
                    frameRate = 60;
                else
                    frameRate = 50;
            }else{
                if( FlgFlicker == 0 )
                    frameRate = 30;
                else
                    frameRate = 25;
            }
            status =
                FVID2_control(pObj->SensorHandle,
                              IOCTL_ISS_SENSOR_FRAME_RATE_SET, &frameRate,
                              &FrameTime);
            if (status != FVID2_SOK)
            {
                Vps_rprintf("\nAE write to sensor failed!\n");
                return FVID2_EFAIL;
            }

            if( IsDbgOn ) Vps_rprintf("\nFrameTime = %d\n", FrameTime);

            Appro2ACtrl(APPRO_CMD_FRAME_TIME, &FrameTime);
        }
    }

    if( pApproOutPrm == NULL )
    {

        Appro2ACtrl(APPRO_CMD_GET_OUTPUT, &pApproOutPrm);
        pIpipe_config = (ipipe_cfg_t * )(pApproOutPrm->ipipe);
        pIsif_config = (isif_cfg_t * )(pApproOutPrm->isif);
        Appro2ACtrl(APPRO_CMD_SET_ENGMODE, APPRO_2AEngModeParm());
    }

    if( pApproOutPrm != NULL )
    {
        if( pIpipe_config->filter_flag != 0 &&  IsDbgOn  )
        {
            Vps_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
            Vps_printf("pIpipe_config->filter_flag = 0x%X\n", pIpipe_config->filter_flag);
            Vps_printf("pIsif_config->feature_flag = 0x%X\n", pIsif_config->feature_flag);
            Vps_printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
        }

        ipipe_config_filters(pIpipe_config);

        pIpipe_config->filter_flag = 0;

        pIsif_config->feature_flag &= ~ISIF_SDRAM_OP_FLAG;
        isif_config(pIsif_config);

        pIsif_config->feature_flag = 0;

        if( pApproOutPrm->updateMisc )
        {


            if( pApproOutPrm->updateMisc & UPDATE_MISC_PWM_VIDEO )
            {
                Iss_SensorPwmParm PwmParm;
                PwmParm.Id = ISS_SENSOR_PWM_VIDEO;
                PwmParm.period = pApproOutPrm->PwmVideoPeriod;
                PwmParm.duty = pApproOutPrm->PwmVideoDuty;
                status =   FVID2_control(pObj->SensorHandle,
                              IOCTL_ISS_SENSOR_PWM_CONFIG, &PwmParm,NULL);
                if (status != FVID2_SOK)
                {
                    Vps_printf("\n ISS_SENSOR_PWM_VIDEO failed!\n");
                }

            }

            if( pApproOutPrm->updateMisc & UPDATE_MISC_PWM_DC )
            {
                Iss_SensorPwmParm PwmParm;
                PwmParm.Id = ISS_SENSOR_PWM_DC;
                PwmParm.period = pApproOutPrm->PwmDCPeriod;
                PwmParm.duty = pApproOutPrm->PwmDCDuty;
                status =   FVID2_control(pObj->SensorHandle,
                              IOCTL_ISS_SENSOR_PWM_CONFIG, &PwmParm,NULL);
                if (status != FVID2_SOK)
                {
                    Vps_printf("\n ISS_SENSOR_PWM_VIDEO failed!\n");
                }

            }

            pApproOutPrm->updateMisc = 0;

        }
    }


    return 0;
}



/* APPRO 2A Process */



/* ===================================================================
 *  @func     IssAlg_capt2AProcessAppro
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
Int32 IssAlg_capt2AProcessAppro(Iss_2AObj *pObj)

{
    static appro2a_output *pApproOutPrm = NULL;
    Int32 status = FVID2_SOK;



    /* reorder data */

    reorderH3AData1((Uint32*)pObj->pH3AMem,(Uint32*)pObj->fullH3ABufAddr,

                    (Uint32)pObj->aewbNumWinH,

                    (Uint32)pObj->aewbNumWinV);



    Appro2ACalc(pObj->aewbNumWinV,pObj->aewbNumWinH,

                pObj->aewbNumPix,pObj->pH3AMem);


    if (status == FVID2_SOK)

    {


        if( pApproOutPrm == NULL )
        {

            Appro2ACtrl(APPRO_CMD_GET_OUTPUT, &pApproOutPrm);
        }

        // AE write to SENSOR
        if( pApproOutPrm != NULL )
        {
            if((pApproOutPrm->sensor.update & UPDATE_SENSOR_GAIN) ||
                (pApproOutPrm->sensor.update & UPDATE_SENSOR_SHUTTER))
            {

                ti2a_output_params.sensorGain       = pApproOutPrm->sensor.Gain;
                ti2a_output_params.sensorExposure   = pApproOutPrm->sensor.Shutter;
                ti2a_output_params.mask             = pApproOutPrm->sensor.update;

                status =

                    FVID2_control(pObj->SensorHandle,

                                  IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN,

                                  &ti2a_output_params,NULL);

                if (status != FVID2_SOK)

                {

                    Vps_rprintf("\nAE write to sensor failed!\n");

                    return FVID2_EFAIL;

                }

            }

            pApproOutPrm->sensor.update = 0;
        }
#if 0
        // AWB write to ISP
        status =
            Iss_captControl(pObj->cameraVipHandle,IOCTL_ISS_ALG_2A_UPDATE,

                            &ti2a_output_params,NULL);

        if (status != FVID2_SOK)

        {

            Vps_rprintf("AWB write to ISP failed!\n");

            return FVID2_EFAIL;

        }
#else
        // AEWB write to ISP
        IssAlg_capt2ADynamicParmAppro(pObj);
#endif
    }

    else

    {

        Vps_rprintf("2A ALG run failed!\n");

    }



    return status;

}



/* 2A process function */



/* ===================================================================
 *  @func     IssAlg_capt2AProcess
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
Int32 IssAlg_capt2AProcess(Iss_2AObj *pObj)

{
#ifdef TWOA_PROFILE
    Uint32 start, end;
    start = Utils_getCurTimeInMsec();
#endif

    if(pObj->aewbVendor == AEWB_ID_TI)

    {

        IssAlg_capt2AProcessTI(pObj);

    }

    else if(pObj->aewbVendor == AEWB_ID_APPRO)

         {

             IssAlg_capt2AProcessAppro(pObj);

         }

#ifdef TWOA_PROFILE
    end = Utils_getCurTimeInMsec();
    Vps_printf("2A:Process time = %d msec\n",(end - start));
#endif

    return FVID2_SOK;

}



/* 2A task Main function */



/* ===================================================================
 *  @func     IssAlg_captTsk2A
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
Void IssAlg_captTsk2A(UArg arg0,UArg arg1)

{

    Iss_2AObj *pObj = (Iss_2AObj*)arg0;

    static int count = 0;


    /* allocate H3A temp memory */

    pObj->pH3AMem = memalign(64,20 * pObj->aewbNumWinV * pObj->aewbNumWinH);

    if (pObj->pH3AMem == NULL)
    {

        Vps_rprintf ( " %s:%d: pObj->pH3AMem is NULL.. memalign failed !!!\n", __FUNCTION__,

                        __LINE__ );
    }

    /* Create AEWB algorithm instance */

    pObj->pAlgHndl = (Void*)ALG_aewbCreate(pObj->aewbNumWinH, pObj->aewbNumWinV,pObj->aewbNumPix);



    if(pObj->pAlgHndl == NULL)

    {

        Vps_rprintf ( " %s:%d: 2A alg instance create failed !!!\n", __FUNCTION__,

                        __LINE__ );

        GT_assert(GT_DEFAULT_MASK,pObj->pAlgHndl != NULL);

    }



#ifdef ISS_CAPT_DEBUG

    Vps_rprintf ( " %s:%d: 2A task Main function Entered !!!\n", __FUNCTION__,

                    __LINE__ );

#endif



    while(pObj->exitFlag == FALSE)

    {

        /* wait for 2A sem */

        Semaphore_pend(pObj->sem,BIOS_WAIT_FOREVER);

        if(pObj->exitFlag == FALSE)
        {
            /* Got new H3A data apply 2A algorithm */
            IssAlg_capt2AProcess(pObj);

            /*
             *  Save the current AF H3A buffer address to be used for Focus value
             *  calculation for VA Defocus algorithm.
             */
            gFocusStatistics.pCurAFDataAddr = (Void*)(pObj->fullH3ABufAddr + pObj->h3aBufSize);

            count ++;
            if ((count % 60) == 0)
            {
                pObj->AFValue = ALG_afRun((Void*)(pObj->fullH3ABufAddr + pObj->h3aBufSize));
            }

        }

    }

    /* Delete the 2A alg instance */

    ALG_aewbDelete(pObj->pAlgHndl);

    //Appro2ARelease();

    /* free H3A temp memory */

    free(pObj->pH3AMem);



#ifdef ISS_CAPT_DEBUG

    Vps_rprintf ( " %s:%d: 2A task Main function Exited !!!\n", __FUNCTION__,

                    __LINE__ );

#endif
}



/* Set 2A default params */



/* ===================================================================
 *  @func     IssAlg_capt2ASetDefault
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
Int32 IssAlg_capt2ASetDefault(Iss_2AObj *pObj)

{

    pObj->exitFlag   = FALSE;

    pObj->AutoIris   = 0;

    pObj->saturation = 0;

    pObj->sharpness  = 0;

    pObj->brightness = 0;

    pObj->contrast   = 0;

    pObj->blc        = 0;

    pObj->AWBMode    = 0;

    pObj->AEMode     = 0;

    pObj->Env        = 0;

    pObj->Binning    = 0;

    pObj->FrameRate  = 0;



    pObj->dcc_Default_Param = NULL;

    pObj->dcc_init_done     = FALSE;

    pObj->dccSize           = 0;



    return FVID2_SOK;

}



/* 2A Init */



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
Int32 IssAlg_capt2AInit(Iss_2AObj *pObj, UInt32 bSetDefaultFlag)

{

    Task_Params tskParams;

    Semaphore_Params semParams;



    gpIssAlg2AObj = pObj;


    if(bSetDefaultFlag)
        IssAlg_capt2ASetDefault(pObj);

    if (pObj->taskHndl==NULL)
    {

        /* Create 2A task */

        Task_Params_init(&tskParams);



        tskParams.priority  = ISSALG_CAPT_TSK_PRI_2A;

        tskParams.stack     = gIssAlg_captTskStack2A;

        tskParams.stackSize = sizeof(gIssAlg_captTskStack2A);

        tskParams.arg0      = (UArg)pObj;



        pObj->taskHndl = Task_create(IssAlg_captTsk2A,

                                    &tskParams,

                                    NULL);



        if(pObj->taskHndl == NULL)

        {

            Vps_rprintf ( " %s:%d: 2A task create failed !!!\n", __FUNCTION__,

                            __LINE__ );

            GT_assert(GT_DEFAULT_MASK,pObj->taskHndl != NULL);

        }



        /* create 2A sem */

        Semaphore_Params_init(&semParams);

        semParams.mode = Semaphore_Mode_BINARY;



        pObj->sem = Semaphore_create(0u,&semParams,NULL);



        if(pObj->sem == NULL)

        {

            Vps_rprintf ( " %s:%d: 2A task sem create failed !!!\n", __FUNCTION__,

                        __LINE__ );

            GT_assert(GT_DEFAULT_MASK,pObj->sem != NULL);

        }



        /* Allocate memory for the H3A buffers */

        pObj->h3aBufAddr[0] = (UInt32)memalign(64,pObj->h3aBufSize * 2); // AEWB + AF

        pObj->h3aBufAddr[1] = (UInt32)memalign(64,pObj->h3aBufSize * 2); // AEWB + AF

    }

    pObj->curH3aBufIdx  = 0;



    /* H3A module config */

    pObj->ispH3aCfg.ptIssConfig = pObj->pIssConfig;

    pObj->ispH3aCfg.pAewbBuff   = (MSP_PTR)pObj->h3aBufAddr[0];

    pObj->ispH3aCfg.pAfBuff     = (MSP_PTR)(pObj->h3aBufAddr[0] + pObj->h3aBufSize);



    MSP_ISP_config(pObj->pIspHandle,(MSP_INDEXTYPE)MSP_ISP_CFG_H3A,&pObj->ispH3aCfg);



#ifdef ISS_CAPT_DEBUG

    Vps_rprintf ( " %s:%d: 2A task Init Done !!!\n", __FUNCTION__,

                    __LINE__ );

#endif



    return FVID2_SOK;

}



/* 2A De Init */



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
Int32 IssAlg_capt2ADeInit(Iss_2AObj *pObj)

{
    /* Set exit flag */
    pObj->exitFlag = TRUE;

    /* post the 2A semaphore so that 2A task comes out of sem wait */
    Semaphore_post(pObj->sem);

    /* wait for 2A task to exit the loop */
    if(pObj->taskHndl != NULL)
    {
        while(Task_getMode(pObj->taskHndl) != Task_Mode_TERMINATED)
        {
            /* wait for 100 msec */
            Task_sleep(100);
        }
    }

    /* Delete the 2A task */
    if(pObj->taskHndl != NULL)
    {
        Task_delete(&pObj->taskHndl);
    }

    if(pObj->sem != NULL)
    {
        Semaphore_delete(&pObj->sem);
    }

    /* Stop H3A module */
    if(ISP_SUCCESS != issDrvH3aStop()) {
        Vps_printf("issDrvH3aStop failed\n");
    }

    /* free H3A buffers */
    free((void*)pObj->h3aBufAddr[0]);
    free((void*)pObj->h3aBufAddr[1]);

    pObj->taskHndl = NULL;

#ifdef ISS_CAPT_DEBUG
    Vps_rprintf ( " %s:%d: 2A task DeInit Done !!!\n", __FUNCTION__, __LINE__ );
#endif



    return FVID2_SOK;

}

Int32 ALG_afRun(void *h3aDataVirtAddr)
{
    MSP_H3aAfOutVfDisableOverlay *pAfPaxData;
    unsigned short i,j;
    int focus_value;
    Uint8 *curAfAddr;
    Uint32 accAfVal[9] = {0,0,0,0,0,0,0,0,0};
    //float avgAfVal[9];
    int weighti, weightj, fweight;
    int af_pax_vt_cnt, af_pax_hz_cnt;

    af_pax_vt_cnt = 16;
    af_pax_hz_cnt = 6;

    curAfAddr = (Uint8* ) h3aDataVirtAddr;

    for(i=1;i<af_pax_vt_cnt-1; i++) {
        for(j=1;j<af_pax_hz_cnt-1; j++) {

            pAfPaxData = (MSP_H3aAfOutVfDisableOverlay *)curAfAddr;

            weighti = (i > (af_pax_vt_cnt-1 - i))?(af_pax_vt_cnt-1 - i):i;
            weightj = (j > (af_pax_hz_cnt-1 - j))?(af_pax_hz_cnt-1 - j):j;
            fweight = weighti*weightj;

            accAfVal[0] += fweight*pAfPaxData->hfvSum_0;
            accAfVal[1] += fweight*pAfPaxData->hfv1_0;
            accAfVal[2] += fweight*pAfPaxData->hfv2_0;
            accAfVal[3] += fweight*pAfPaxData->hfvSum_1;
            accAfVal[4] += fweight*pAfPaxData->hfv1_1;
            accAfVal[5] += fweight*pAfPaxData->hfv2_1;
            accAfVal[6] += fweight*pAfPaxData->hfvSum_2;
            accAfVal[7] += fweight*pAfPaxData->hfv1_2;
            accAfVal[8] += fweight*pAfPaxData->hfv2_2;

            curAfAddr += sizeof(MSP_H3aAfOutVfDisableOverlay);

        }
    }

/*   for(i=0;i<9;i++){
      avgAfVal[i] = (float)accAfVal[i]/(af_pax_vt_cnt*af_pax_hz_cnt);
      //if((i==4)||(i==5)) // 4 & 5 indices for green value in RGB space
      //    OSA_printf("accAfVal[%d]: %d \t avgAfVal[%d]: %f \n", i, accAfVal[i], i, avgAfVal[i]);
   }*/

   focus_value = accAfVal[4];

   return focus_value;
}

/************************* FOCUS VALUE FOR TAMPER ALGORITHM ***********************/

Void IssAlg_capt2AGetFocusInfo(UInt8 **pAFAddr,UInt16 *pVtPaxCnt,UInt16 *pHzPaxCnt)
{
    *pAFAddr   = gFocusStatistics.pCurAFDataAddr;
    *pVtPaxCnt = AF_PAX_VT_CNT;
    *pHzPaxCnt = AF_PAX_HZ_CNT;
}

Void IssAlg_capt2AInitKmax(Iss_2AFocusStatistics *pStatistics)
{
    UInt8 k;

    pStatistics->focusValue = pStatistics->lumValue = pStatistics->globalLumValue = 0;

    for (k = 0; k < NUM_MAX_FOCUS; k++)
    {
        pStatistics->maxFocusValues[k] = 0;
        pStatistics->maxLumValues[k]   = 0;
        pStatistics->locMax_x[k]       = 0;
        pStatistics->locMax_y[k]       = 0;
    }
}

Void IssAlg_capt2AShiftKmax(Iss_2AFocusStatistics *pStatistics,Int8 k)
{
    Int8 i;

    for(i = (NUM_MAX_FOCUS - 2); i >= k ; i--)
    {
        pStatistics->locMax_x[i + 1]       = pStatistics->locMax_x[i];
        pStatistics->locMax_y[i + 1]       = pStatistics->locMax_y[i];
        pStatistics->maxFocusValues[i + 1] = pStatistics->maxFocusValues[i];
        pStatistics->maxLumValues[i + 1]   = pStatistics->maxLumValues[i];
    }
}

Void IssAlg_capt2AUpdateKmax(Iss_2AFocusStatistics *pStatistics,Int8 k,Iss_2AKMaxParams *kmaxp)
{
    if(k == NUM_MAX_FOCUS)
    {
        return;
    }

    if(kmaxp->focusValue > pStatistics->maxFocusValues[k])
    {
        IssAlg_capt2AShiftKmax(pStatistics,k);
        pStatistics->locMax_x[k]       = kmaxp->x;
        pStatistics->locMax_y[k]       = kmaxp->y;
        pStatistics->maxFocusValues[k] = kmaxp->focusValue;
        pStatistics->maxLumValues[k]   = kmaxp->sumValue;
    }
    else
    {
        IssAlg_capt2AUpdateKmax(pStatistics,(k + 1),kmaxp);
    }
}

Int32 IssAlg_capt2AFocusReset(Iss_2AFocusStatistics *pStatistics)
{
    MSP_H3aAfOutVfDisableOverlay *pAfPaxData;
    UInt16 i,j;
    UInt8 *curAfAddr,*afAddr;
    UInt16 af_pax_vt_cnt, af_pax_hz_cnt;
    Iss_2AKMaxParams kmaxp;

    IssAlg_capt2AGetFocusInfo(&afAddr,&af_pax_vt_cnt,&af_pax_hz_cnt);

    IssAlg_capt2AInitKmax(pStatistics);

    curAfAddr = afAddr + sizeof(MSP_H3aAfOutVfDisableOverlay);

    for(i = 0;i < af_pax_vt_cnt; i++)
    {
        pStatistics->offsetRowAfPaxData[i] = (curAfAddr - afAddr);

        for(j = 0;j < (af_pax_hz_cnt - 2); j++)
        {
            pAfPaxData       = (MSP_H3aAfOutVfDisableOverlay *)curAfAddr;
            kmaxp.focusValue = (pAfPaxData->hfv1_1 << 16)/pAfPaxData->hfvSum_1;
            kmaxp.sumValue   = pAfPaxData->hfvSum_1;
            kmaxp.x          = j;
            kmaxp.y          = i;

            IssAlg_capt2AUpdateKmax(pStatistics,0,&kmaxp);

            pStatistics->globalLumValue += pAfPaxData->hfvSum_1;

            curAfAddr += sizeof(MSP_H3aAfOutVfDisableOverlay);
        }

        curAfAddr += sizeof(MSP_H3aAfOutVfDisableOverlay);
        curAfAddr += sizeof(MSP_H3aAfOutVfDisableOverlay);
    }

    pStatistics->focusValue = 0;
    pStatistics->lumValue   = 0;

    for (i = 0;i < NUM_MAX_FOCUS;i++)
    {
        pStatistics->focusValue += pStatistics->maxFocusValues[i];
        pStatistics->lumValue   += pStatistics->maxLumValues[i];
    }

    pStatistics->focusValue /= NUM_MAX_FOCUS;
    pStatistics->lumValue /= NUM_MAX_FOCUS;

    pStatistics->globalLumValue /= (af_pax_vt_cnt)*(af_pax_hz_cnt - 2);

    return FVID2_SOK;
}

Int32 IssAlg_capt2AFocus(Iss_2AFocusStatistics *pStatistics,UInt32 *pFocusValue)
{
    MSP_H3aAfOutVfDisableOverlay *pAfPaxData;
    UInt16 i;
    UInt8 *afAddr;
    UInt16 af_pax_vt_cnt, af_pax_hz_cnt;

    IssAlg_capt2AGetFocusInfo(&afAddr,&af_pax_hz_cnt,&af_pax_vt_cnt);

    /* Find average of target locations' focus value */
    *pFocusValue = 0;

    for(i = 0;i < NUM_MAX_FOCUS;i++)
    {
        pAfPaxData= (MSP_H3aAfOutVfDisableOverlay*)(afAddr + pStatistics->offsetRowAfPaxData[pStatistics->locMax_y[i]] + pStatistics->locMax_x[i] * sizeof(MSP_H3aAfOutVfDisableOverlay));
        *pFocusValue += (pAfPaxData->hfv1_1 << 16)/pAfPaxData->hfvSum_1;
    }

    *pFocusValue /= NUM_MAX_FOCUS;

    return FVID2_SOK;
}


/* Focus value calculation for VA defocus algorithm */
Int32 IssAlg_capt2AGetFocus(Ptr pCmdArgs)
{
    UInt32 focusValue;
    Iss_CaptFocusValue *pFocusValue = (Iss_CaptFocusValue*)pCmdArgs;

    if((pFocusValue->resetFlag == 1) || (gFocusStatistics.firstTime == 1))
    {
        /* AF Reset */
        IssAlg_capt2AFocusReset(&gFocusStatistics);

        gFocusStatistics.firstTime = 0;
    }

    IssAlg_capt2AFocus(&gFocusStatistics,&focusValue);

    pFocusValue->focusValue = focusValue;

    return FVID2_SOK;
}
