/** ==================================================================
 *  @file   alg_vstab.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/drivers/alg/vstab/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ALG_VSTAB_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ALG_VSTAB_H_

// #include "vstabLink_priv.h"
#include <ti/psp/iss/alg/vstab/ti/xdais/dm/ivs1.h>
#include <ti/psp/iss/hal/iss/isp/ipipe/inc/ipipe.h>

#define DRV_IPIPE_BSC_BUFFER_MAX		(16384)
#define ALG_BSC_DATA_SIZE_MAX   		(DRV_IPIPE_BSC_BUFFER_MAX)
#define VIDEO_VS_NUM_STATUS        		(100)
#define ALG_VSTAB_LINK_TSK_STACK_SIZE	(16 * KB)
#define ALG_VSTAB_LINK_TSK_PRI 			(9)

typedef struct {

    Uint16 totalFrameWidth;
    Uint16 totalFrameHeight;

    Uint32 bsc_row_vct;
    Uint32 bsc_row_vpos;
    Uint32 bsc_row_vnum;
    Uint32 bsc_row_vskip;
    Uint32 bsc_row_hpos;
    Uint32 bsc_row_hnum;
    Uint32 bsc_row_hskip;
    Uint32 bsc_row_shf;

    Uint32 bsc_col_vct;
    Uint32 bsc_col_vpos;
    Uint32 bsc_col_vnum;
    Uint32 bsc_col_vskip;
    Uint32 bsc_col_hpos;
    Uint32 bsc_col_hnum;
    Uint32 bsc_col_hskip;
    Uint32 bsc_col_shf;

} ALG_VstabCreate;

typedef struct {

    Uint32 frameNum;
    Uint16 startX;
    Uint16 startY;

} VIDEO_VsStatus;

typedef struct {

    Uint8 *curBscDataVirtAddr;
    Uint8 *curBscDataPhysAddr;
    Uint8 *prevBscDataVirtAddr;
    Uint8 *prevBscDataPhysAddr;

} ALG_VstabRunPrm;

typedef struct {

    Uint16 startX;
    Uint16 startY;

} ALG_VstabStatus;

typedef struct {

    void *algVsHndl;
    ALG_VstabCreate vsCreatePrm;
    VIDEO_VsStatus vsStatus[VIDEO_VS_NUM_STATUS];
    // OSA_MutexHndl vsLock;
    // OSA_FlgHndl vsFlag;
    Uint32 curFrame;

} VIDEO_VsStream;

typedef struct {
    ALG_VstabCreate createPrm;
    IALG_Handle vsHandle;
    Uint32 bscBufAddr[2];
    Uint32 curBscBufAddr;
    Uint32 prevBscBufAddr;
    Uint16 curBscBufIdx;

} ALG_VsObj;

/* ===================================================================
 *  @func     VS_ALG_create                                               
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
IALG_Handle VS_ALG_create(IALG_Fxns * fxns, IALG_Handle p,
                          IALG_Params * params);
/* ===================================================================
 *  @func     VS_ALG_delete                                               
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
Void VS_ALG_delete(IALG_Handle alg);

/* ===================================================================
 *  @func     ALG_vstabCreate                                               
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
void *ALG_vstabCreate(ALG_VstabCreate * create);

/* ===================================================================
 *  @func     ALG_vstabRun                                               
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
int ALG_vstabRun(void *hndl, ALG_VstabRunPrm * prm, ALG_VstabStatus * status);

/* ===================================================================
 *  @func     ALG_vstabDelete                                               
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
int ALG_vstabDelete(void *hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
