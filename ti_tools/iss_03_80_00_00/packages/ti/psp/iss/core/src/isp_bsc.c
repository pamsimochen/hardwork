/* =============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under 
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file bsc_sdma.c
*
* This file contains routines for BSC SDMA driver
*
* @path iss/drivers/bsc_sdma/src
*
* @rev 01.01
*/
/* -------------------------------------------------------------------------- 
 */
/* ----------------------------------------------------------------------------
 * *! *! Revision History *! =================================== *! 22-Jul-2009 
 * Petar Sivenov - Created *! *
 * =========================================================================== */

/* User code goes here */
/* ------compilation control switches --------------------------------------- 
 */
/****************************************************************
 * INCLUDE FILES
 ***************************************************************/
/* ----- system and platform files ---------------------------- */
// #include <msp/msp_utils.h>
#include <ti/psp/iss/core/msp_types.h>
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe.h>
#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe_reg.h>
#include "ti/psp/iss/core/inc/isp_bsc.h"

/****************************************************************
*  EXTERNAL REFERENCES NOTE: only use if not found in header file
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------macros ----------------------------------------------*/
/*--------data declarations -----------------------------------*/

#define MAX_PIX_VALUE           (255)
#define MAX_BSC_SUM_VALUE       (1l<<16)

MSP_HANDLE pSdmaHandle;

// static MSP_Sdma_ChannelInfo ChanInfo;
// static MSP_Sdma_ConfigParam ConfigParam;
// static MSP_SDMA_DESC_3A_NODE BscSecTransfNode;

/*--------function prototypes ---------------------------------*/

/* ========================================================================== 
 */
/**
* bscCfgPrepare()
*
*  Calculates Boundary signal calculator parameters and fills the IPIPE BSC
*  configuration structure accordingly.
*
* @param - ptInputCfg - bscInputCfgT* - Pointer to input data structure
*
* @param - ptBscReadyCfg - ipipe_bsc_cfg_t* - Pointer to IPIPE BSC configuration
*
* @return = void - none
*
* @pre structure vs_ctx_sizes should contain valid data
*
* @post Updates vs_ctx_sizes.
*
* @see
*
*/
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     bscCfgPrepare                                               
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
MSP_ERROR_TYPE bscCfgPrepare(bscInputCfgT * ptInputCfg,
                             ipipe_bsc_cfg_t * ptBscReadyCfg)
{
    MSP_ERROR_TYPE result = MSP_ERROR_NONE;

    MSP_U32 decimate;

    MSP_U32 bsc_size;

    MSP_U32 numX;

    MSP_U32 numY;

    MSP_U32 maxSum;

    MSP_U16 nDownShift;

    if ((NULL != ptInputCfg) && (NULL != ptBscReadyCfg))
    {

        // Enables Boundary Signal Calculator
        ptBscReadyCfg->enable = DRV_BSC_ENABLE_ON;
        // Configures BSC for single shot mode
        ptBscReadyCfg->mode = DRV_BSC_MODE_SINGLE;
        // Enable row and column sampling
        ptBscReadyCfg->row_sample = DRV_BSC_SAMPLING_ENABLED;
        ptBscReadyCfg->col_sample = DRV_BSC_SAMPLING_ENABLED;
        // Choose Y sampling
        ptBscReadyCfg->element = DRV_BSC_ELEMENT_Y;

        // Calculate decimation coefficient
        decimate = ptInputCfg->nInImgWidth;
        if (ptInputCfg->nInImgHeight > ptInputCfg->nInImgWidth)
        {
            decimate = ptInputCfg->nInImgHeight;
        }

        bsc_size = (BSC_BUFF_SIZE / ptInputCfg->nNumVectors);
        decimate = (decimate + bsc_size - 1) / bsc_size;

        // Calculate number of signals
        numX = ptInputCfg->nInImgWidth;
        numY = ptInputCfg->nInImgHeight;

        if (decimate > 1)
        {
            numX /= decimate;
            numY /= decimate;
        }

        // number of signals must be even after division to
        // ptInputCfg->nNumVectors
        numX =
            (numX / (ptInputCfg->nNumVectors << 1)) *
            (ptInputCfg->nNumVectors << 1);
        numY =
            (numY / (ptInputCfg->nNumVectors << 1)) *
            (ptInputCfg->nNumVectors << 1);

        // Set column sampling parameters
        ptBscReadyCfg->col_pos.vectors = ptInputCfg->nNumVectors;
        ptBscReadyCfg->col_pos.v_pos = ptInputCfg->nInImgStartY;
        ptBscReadyCfg->col_pos.h_pos = ptInputCfg->nInImgStartX;
        ptBscReadyCfg->col_pos.v_num = (numY / ptInputCfg->nNumVectors);
        ptBscReadyCfg->col_pos.h_num = (numX / ptInputCfg->nNumVectors);
        ptBscReadyCfg->col_pos.v_skip = decimate;
        ptBscReadyCfg->col_pos.h_skip = decimate;

        // Set row sampling parameters
        ptBscReadyCfg->row_pos.vectors = ptInputCfg->nNumVectors;
        ptBscReadyCfg->row_pos.v_pos = ptInputCfg->nInImgStartY;
        ptBscReadyCfg->row_pos.h_pos = ptInputCfg->nInImgStartX;
        ptBscReadyCfg->row_pos.v_num = (numY / ptInputCfg->nNumVectors);
        ptBscReadyCfg->row_pos.h_num = (numX / ptInputCfg->nNumVectors);
        ptBscReadyCfg->row_pos.v_skip = decimate;
        ptBscReadyCfg->row_pos.h_skip = decimate;

        // Find maximum summation value
        maxSum = ptBscReadyCfg->row_pos.h_num + 1;
        if (ptBscReadyCfg->col_pos.v_num > ptBscReadyCfg->row_pos.h_num)
        {
            maxSum = ptBscReadyCfg->col_pos.v_num + 1;
        }
        maxSum *= MAX_PIX_VALUE;

        // Find needed Down Shift
        nDownShift = 0;
        while ((MAX_BSC_SUM_VALUE) < (maxSum >> nDownShift))
        {
            nDownShift++;
        }

        ptBscReadyCfg->row_pos.shift = nDownShift;
        ptBscReadyCfg->col_pos.shift = nDownShift;

    }
    else
    {
        result = MSP_ERROR_NULLPTR;
    }

    return (result);

}

/* ===================================================================
 *  @func     bscDataGet                                               
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
MSP_ERROR_TYPE bscDataGet(MSP_PTR pRowSums, MSP_PTR pColSums, MSP_U32 row_size,
                          MSP_U32 col_size)
{
    register MSP_ERROR_TYPE result = MSP_ERROR_NULLPTR;

    MSP_U32 pTempPtr;

    if ((NULL != pRowSums) && (NULL != pColSums))
    {
        ipipe_get_bsc_address(&pTempPtr, IPIPE_BSC_MEMORY_0);
        memcpy(pRowSums, (MSP_PTR) pTempPtr, row_size);
        ipipe_get_bsc_address(&pTempPtr, IPIPE_BSC_MEMORY_1);
        memcpy(pColSums, (MSP_PTR) pTempPtr, col_size);
        result = MSP_ERROR_NONE;
    }

    return (result);
}

/* 
 * MSP_ERROR_TYPE bscSdmaInit(MSP_APPCBPARAM_TYPE *ptAppCBParam,
 * MSP_Sdma_ChannelEventCallback sdmaCallback) { MSP_ERROR_TYPE result =
 * MSP_ERROR_NONE;
 * 
 * result = MSP_init( &pSdmaHandle, "MSP.SDMA", MSP_PROFILE_DEFAULT,
 * ptAppCBParam);
 * 
 * return(result); }
 * 
 * bscSdmaDeinit() { MSP_ERROR_TYPE result = MSP_ERROR_NONE;
 * 
 * MSP_deInit(pSdmaHandle);
 * 
 * return(result); }
 * 
 * bscSdmaConfig(MSP_U32 nBscSdmaChanId, MSP_Sdma_ChannelEventCallback
 * sdmaCallback) { MSP_SDMA_INIT_CHANNEL_CONF_STRUCT_TO_DEFAULT((&ChanInfo));
 * 
 * ChanInfo.dataType = MSP_SDMA_DATA_TYPE_S16; ChanInfo.elementsPerFrame =
 * BSC_BUFF_SIZE; ChanInfo.srcElemIndex = 1; ChanInfo.dstElemIndex = 1;
 * ChanInfo.channelEventCallback = sdmaCallback;
 * 
 * ConfigParam.chanId = nBscSdmaChanId; ConfigParam.channelInfo = &ChanInfo;
 * }
 * 
 * bscSdmaStart() {
 * 
 * } */
