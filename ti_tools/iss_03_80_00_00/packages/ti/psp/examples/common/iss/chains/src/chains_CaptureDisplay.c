/** ==================================================================
 *  @file   Chains_CameraMt9j003.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "chains.h"

Void Chains_CameraMt9j003(Chains_Ctrl * chainsCfg)
{
    CameraLink_CreateParams cameraPrm;

    NullLink_CreateParams nullPrm;
    DisplayLink_CreateParams displayPrm;

    CameraLink_VipInstParams *pCameraInstPrm;

    CameraLink_OutParams *pCameraOutPrm;

    UInt32 displayId, cameraId;
    UInt32 nullId;

    UInt32 vipInstId;

    UInt32 numFlipCh;

    UInt32 numOutputs;

    cameraId = SYSTEM_LINK_ID_CAMERA;
    nullId = SYSTEM_LINK_ID_NULL_1;

    if (chainsCfg->cameraSingleChOutYuv422SP)
        numOutputs = 2;
    else
        numOutputs = 2;

    cameraPrm.numVipInst = 1;

    switch (chainsCfg->displayId[0])
    {
        default:
        case CHAINS_DISPLAY_ID_HDMI_ONCHIP:
            displayId = SYSTEM_LINK_ID_DISPLAY_0;
            break;
        case CHAINS_DISPLAY_ID_HDMI_OFFCHIP:
            displayId = SYSTEM_LINK_ID_DISPLAY_1;
            break;
    }

    numFlipCh = cameraPrm.numVipInst;
    if (numOutputs > 1)
        numFlipCh *= 2;

    /* In case of 422SP, we would require scaler, as display would not take
     * in 422SP as input format */

    cameraPrm.tilerEnable = chainsCfg->tilerEnable;

	for (vipInstId = 0; vipInstId < cameraPrm.numVipInst; vipInstId++)
	{
		pCameraInstPrm = &cameraPrm.vipInst[vipInstId];
		pCameraInstPrm->vipInstId =
			(ISS_CAPT_INST_VP + vipInstId * 2) % ISS_CAPT_INST_MAX;
		pCameraInstPrm->SensorId = FVID2_ISS_SENSOR_MT9J003_DRV;
		if (chainsCfg->cameraSingleChOutYuv420)
			pCameraInstPrm->inDataFormat = FVID2_DF_YUV420SP_UV;
		else if (chainsCfg->cameraSingleChOutYuv422SP)
			pCameraInstPrm->inDataFormat = FVID2_DF_YUV422SP_UV;
		else
		pCameraInstPrm->inDataFormat = FVID2_DF_YUV422I_UYVY; 	//FVID2_DF_YUV422I_VYUY;
		
		pCameraInstPrm->standard = FVID2_STD_1080P_60;
		pCameraInstPrm->numOutput = numOutputs;

		pCameraOutPrm = &pCameraInstPrm->outParams[0];
		if (chainsCfg->cameraSingleChOutYuv420)
			pCameraOutPrm->dataFormat = FVID2_DF_YUV420SP_UV;
		else if (chainsCfg->cameraSingleChOutYuv422SP)
			pCameraOutPrm->dataFormat = FVID2_DF_YUV422SP_UV;
		else
			pCameraOutPrm->dataFormat = FVID2_DF_YUV422I_UYVY; 	//FVID2_DF_YUV422I_VYUY;

		pCameraOutPrm->scEnable = FALSE;
		pCameraOutPrm->scOutWidth = 1920;
		pCameraOutPrm->scOutHeight = 1080;
		pCameraOutPrm->outQueId = 0;
		if(numOutputs == 2){
			pCameraOutPrm = &pCameraInstPrm->outParams[1];
			if (chainsCfg->cameraSingleChOutYuv420)
				pCameraOutPrm->dataFormat = FVID2_DF_YUV420SP_UV;
			else if (chainsCfg->cameraSingleChOutYuv422SP)
				pCameraOutPrm->dataFormat = FVID2_DF_YUV422SP_UV;
			else
				pCameraOutPrm->dataFormat = FVID2_DF_YUV422I_UYVY; 	//FVID2_DF_YUV422I_VYUY;
				
			pCameraOutPrm->scEnable = FALSE;
			pCameraOutPrm->scOutWidth = 720;
			pCameraOutPrm->scOutHeight = 480;
			pCameraOutPrm->outQueId = 1;
		}	
	}

    cameraPrm.outQueParams[0].nextLink = displayId;
	if(numOutputs == 2)
    cameraPrm.outQueParams[1].nextLink = nullId;

    displayPrm.inQueParams.prevLinkId = cameraId;

    displayPrm.inQueParams.prevLinkQueId = 0;
    displayPrm.displayRes = chainsCfg->displayRes;
	if(numOutputs == 2){
		nullPrm.numInQue = 1;
		nullPrm.inQueParams[0].prevLinkId = cameraId;
		nullPrm.inQueParams[0].prevLinkQueId = 1;
	}
    System_displayCtrlInit(chainsCfg->displayRes, chainsCfg->sdDisplayRes);
    System_linkCreate(cameraId, &cameraPrm);
	if(numOutputs == 2)
    System_linkCreate(nullId, &nullPrm);
    System_linkCreate(displayId, &displayPrm);
    System_memPrintHeapStatus();

    if (chainsCfg->exeTimeInSecs)
    {
        System_linkStart(displayId);
		if(numOutputs == 2)
        System_linkStart(nullId);
        System_linkStart(cameraId);

        while (1)
        {
            Task_sleep(1100 * 1000);
        }
    }

    System_linkDelete(cameraId);
	if(numOutputs == 2)
	System_linkDelete(nullId);
    System_linkDelete(displayId);

    System_displayCtrlDeInit();
}
