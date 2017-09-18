/** ==================================================================
 *  @file   alg_fd.c                                                  
 *                                                                    
 *  @path    /proj/vsi/users/venu/DM812x/IPNetCam_rel_1_8/ti_tools/iss_02_bkup/packages/ti/psp/iss/alg/fd/src/                                                 
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <stdlib.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/iss/hal/iss/simcop/common/cslr.h>
#include <ti/psp/iss/hal/fd/inc/_csl_fd.h>
#include <ti/psp/iss/hal/iss/isp/isp5_utils/isp5_sys_types.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/iss/core/msp_types.h>
#include <ti/sysbios/knl/Task.h>

#include "alg_fd.h"

extern CSL_FdHandle Fdhndl;
Semaphore_Handle    gFdSem;

#define POLL

/* FD ISR */
/* ===================================================================
 *  @func     app_fd_callBack                                               
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
Void app_fd_callBack(ISS_RETURN status,uint32 arg1,void *arg2)
{
	CSL_FdHwSetupCtrl handle;
	CSL_Status        cslStatus = CSL_SOK;

    /* post the semaphore */
    Semaphore_post(gFdSem);

    handle.InterruptEnable = CSL_FD_HW_INT_ENABLE;

    cslStatus = _CSL_fdInt0EnableClear(Fdhndl,handle.InterruptEnable);
    if(cslStatus != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdInt0EnableClear failed - %d\n",cslStatus);
	}
}

/* FD init */
/* ===================================================================
 *  @func     app_fd_init                                               
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
CSL_Status app_fd_init(MSP_FD_CREATE_PARAMS *pApp_params)
{
    CSL_FdHwSetupCtrl handle;
    CSL_Status        status = CSL_SOK;
    
#ifndef POLL	
	ISS_RETURN        retval_iss;
    Semaphore_Params  semParams;
#endif	

	/*Get FD CSL handle*/
    Fdhndl = (CSL_FdObj*)malloc(sizeof(CSL_FdObj));
    if(Fdhndl == NULL)
    {
	    Vps_rprintf("FD:mem alloc for handle failed\n");
	    goto EXIT;
	}

	Fdhndl->openMode = CSL_EXCLUSIVE;
	Fdhndl->perNum   = CSL_FD_0;    ///hardcoded
	status = CSL_fdOpen(Fdhndl, Fdhndl->perNum,Fdhndl->openMode);
	if(status != CSL_SOK)
	{
	    Vps_rprintf("FD:CSL_fdOpen failed - %d\n",status);
	    goto EXIT;
	}

	/*we disable start bit as we are using HWSEQ*/
	handle.Soft_Reset_bit = CSL_FD_HW_SRST_ENABLE;
	handle.Face_Direction = pApp_params->ulDirection;
	handle.ImageStartX    = pApp_params->ptFdRoi->ulInputImageStartX;
	handle.ImageStartY    = pApp_params->ptFdRoi->ulInputImageStartY;
	handle.SizeX          = pApp_params->ptFdRoi->ulSizeX;
	handle.SizeY          = pApp_params->ptFdRoi->ulSizeY;
	handle.MinfaceSize    = pApp_params->ulMinFaceSize;
	handle.ThresholdValue = pApp_params->ulThresholdValue;

    /*configure the FD module*/
    status = CSL_fdHwSetup(Fdhndl,&handle);
	if(status != CSL_SOK)
	{
	    Vps_rprintf("FD:CSL_fdHwSetup failed - %d\n",status);
	    goto EXIT;
	}

#ifndef POLL	
    /* create FD semaphore */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;

    gFdSem = Semaphore_create(0u,&semParams,NULL);
    if(gFdSem == NULL)
    {
	    Vps_rprintf("FD:Semaphore_create failed\n");
	    goto EXIT;
	}

    /* register the ISR */
    retval_iss = iss_register_interrupt_handler(ISS_INTERRUPT_FD,app_fd_callBack,NULL,NULL);
    if(retval_iss != ISP_SUCCESS)
    {
	    Vps_rprintf("FD:FD ISR register failed\n");
	}
#endif	

EXIT :
    return status;
}

/* FD de-init */
/* ===================================================================
 *  @func     app_fd_deinit                                               
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
CSL_Status app_fd_deinit()
{
#ifndef POLL	
    ISS_RETURN retval_iss;

    /* delete FD sem */
    Semaphore_delete(&gFdSem);

    /* unregister the FD ISR */
    retval_iss = iss_unregister_interrupt_handler(ISS_INTERRUPT_FD);
    if(retval_iss != ISS_SUCCESS)
    {
	    Vps_rprintf("FD:FD ISR unregister failed\n");
	}
#endif	

    /* free the FD handle */
    free(Fdhndl);

    return CSL_SOK;
}

/* FD address init */
/* ===================================================================
 *  @func     app_fd_Addr_init                                               
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
CSL_Status app_fd_Addr_init(MSP_BUFHEADER_TYPE *ptBufaddr)
{
    CSL_FdHwSetupCtrl handle;
	CSL_Status        status = CSL_SOK;

	handle.PictureAddress  = (Uint32)ptBufaddr->pBuf[0];
	handle.WorkAreaAddress = (Uint32)ptBufaddr->pBuf[1];

    if((handle.PictureAddress % 32 != 0) || (handle.WorkAreaAddress % 32 != 0))
    {
        Vps_rprintf("FD:unalligned picture/workarea address\n");
	}

    status = _CSL_fdSetPictureAddress(Fdhndl,handle.PictureAddress);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetPictureAddress failed - %d\n",status);
	    goto EXIT;
	}

    status = _CSL_fdSetWorkAreaAddress(Fdhndl,handle.WorkAreaAddress);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetWorkAreaAddress failed - %d\n",status);
	    goto EXIT;
	}

EXIT:
    return status;
}

/* FD direction update */
/* ===================================================================
 *  @func     fd_config_direction_update                                               
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
CSL_Status fd_config_direction_update(MSP_FD_DIR_PARAM  fdDirection)
{
    CSL_FdHwSetupCtrl handle;
	CSL_Status        status = CSL_SOK;

	handle.Face_Direction = fdDirection;

	status = _CSL_fdSetFace_Direction(Fdhndl,handle.Face_Direction);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetFace_Direction failed - %d\n",status);
	}

    return status;
}


/* FD threshold update */
/* ===================================================================
 *  @func     fd_config_threshold_update                                               
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
CSL_Status fd_config_threshold_update(MSP_U32 threshold)
{
	CSL_FdHwSetupCtrl handle;
	CSL_Status        status = CSL_SOK;

	handle.ThresholdValue = threshold;

	status = _CSL_fdSetThresholdValue(Fdhndl,handle.ThresholdValue);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetThresholdValue failed - %d\n",status);
	}

    return status;
}

/* FD minface size update */
/* ===================================================================
 *  @func     fd_config_minfacesize_update                                               
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
CSL_Status fd_config_minfacesize_update(MSP_FD_MIN_SIZE_PARAM  minFaceSize)
{
	CSL_FdHwSetupCtrl handle;
	CSL_Status        status = CSL_SOK;

	handle.MinfaceSize = minFaceSize;

    status = _CSL_fdSetMinfaceSize(Fdhndl,handle.MinfaceSize);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetMinfaceSize failed - %d\n",status);
	}

    return status;
}

/* FD ROI update */
/* ===================================================================
 *  @func     fd_config_ROI_update                                               
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
CSL_Status fd_config_ROI_update(MSP_FD_ROI_PARAMS *ptFdRoi)
{
	CSL_FdHwSetupCtrl handle;
	CSL_Status        status = CSL_SOK;

	handle.ImageStartX = ptFdRoi->ulInputImageStartX;
	handle.ImageStartY = ptFdRoi->ulInputImageStartY;
	handle.SizeX       = ptFdRoi->ulSizeX;
	handle.SizeY       = ptFdRoi->ulSizeY;

	status = _CSL_fdSetImageStartX(Fdhndl,handle.ImageStartX);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetImageStartX failed - %d\n",status);
	    goto EXIT;
	}

    status = _CSL_fdSetImageStartY(Fdhndl,handle.ImageStartY);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetImageStartY failed - %d\n",status);
	    goto EXIT;
	}

    status = _CSL_fdSetSizeX(Fdhndl,handle.SizeX);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetSizeX failed - %d\n",status);
	    goto EXIT;
	}

    status = _CSL_fdSetSizeY(Fdhndl,handle.SizeY);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdSetSizeY failed - %d\n",status);
	    goto EXIT;
	}

EXIT :
    return status;
}

/* FD execute*/
/* ===================================================================
 *  @func     fd_execute                                               
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
CSL_Status fd_execute()
{
    CSL_FdHwSetupCtrl handle;
	CSL_Status        status = CSL_SOK;
	
#ifdef POLL
    UInt32 finishStatus;
#endif	

#ifndef POLL	
	handle.InterruptEnable = CSL_FD_HW_INT_ENABLE;
	
    status = _CSL_fdInt0Enable(Fdhndl,handle.InterruptEnable);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdInt0Enable failed - %d\n",status);
	    goto EXIT;
	}
#endif	

    handle.Fd_Run = CSL_FD_HW_FD_RUN_ENABLE;
    status = _CSL_fdRun(Fdhndl,handle.Fd_Run);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdRun failed - %d\n",status);
	    goto EXIT;
	}

#ifndef POLL	
    /* wait for semaphore */
    Semaphore_pend(gFdSem,BIOS_WAIT_FOREVER);

    handle.InterruptEnable = CSL_FD_HW_INT_ENABLE;

    status = _CSL_fdInt0EnableClear(Fdhndl,handle.InterruptEnable);
    if(status != CSL_SOK)
    {
	    Vps_rprintf("FD:_CSL_fdInt0EnableClear failed - %d\n",status);
	    goto EXIT;
	}
#else
    /* Poll for FD process completion */
    do
	{
	    Task_sleep(1); 
	    status = _CSL_fdGetFinishStatus(Fdhndl,&finishStatus);
		if(status != CSL_SOK)
		{
			Vps_rprintf("FD:_CSL_fdGetFinishStatus failed - %d\n",status);
			goto EXIT;
		}		
	} while(finishStatus == 0);
#endif	

EXIT:
    return status;
}

/* Get the no of faces detected */
/* ===================================================================
 *  @func     FD_get_num_faces                                               
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
void FD_get_num_faces(MSP_FD_RESULT_PARAMS *ptresultParam)
{
    Uint32 data;

	_CSL_fdGetNumofFacesDetected(Fdhndl,&data);
	ptresultParam->ulFaceCount = data;
}

/* Get face info */
/* ===================================================================
 *  @func     FD_get_all_face_detect_positions                                               
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
CSL_Status FD_get_all_face_detect_positions(MSP_FD_RESULT_PARAMS *ptresultParam)
{
    Uint16 loop = 0;
	Uint32 data;
	Uint32 facecount  = 0;
    CSL_Status status = CSL_SOK;

    if(ptresultParam->ulFaceCount > 35)
    {
        facecount = 35;
    }
    else
    {
        facecount = ptresultParam->ulFaceCount;
    }

	for (loop = 0; loop< facecount;loop++)
	{
        status=  _CSL_fdGetResultXpostion(Fdhndl,&data,loop);
        ptresultParam->tFacePosition[loop].ulResultX = data;

        status=  _CSL_fdGetResultYpostion(Fdhndl,&data,loop);
        ptresultParam->tFacePosition[loop].ulResultY = data;


        status=  _CSL_fdGetResultConfidence(Fdhndl,&data,loop);
        ptresultParam->tFacePosition[loop].ulResultConfidenceLevel = data;

        status=  _CSL_fdGetResultSize(Fdhndl,&data,loop);
        ptresultParam->tFacePosition[loop].ulResultSize = data;

        status=  _CSL_fdGetResultAngle(Fdhndl,&data,loop);
        ptresultParam->tFacePosition[loop].ulResultAngle = data;
    }

    return status;
}
