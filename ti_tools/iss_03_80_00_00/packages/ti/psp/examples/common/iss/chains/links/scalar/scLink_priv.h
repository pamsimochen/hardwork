/** ==================================================================
 *  @file   scLink_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/scalar/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _SCHWMSLINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SCHWMSLINK_PRIV_H_

#include <ti/psp/examples/common/iss/chains/links/scLink.h>
#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>

#define SC_LINK_OBJ_MAX                     (2)

#define SC_LINK_MAX_CH                      (8)
#define SC_LINK_MAX_OUT_FRAMES_PER_CH       (SYSTEM_LINK_FRAMES_PER_CH)

#define SC_LINK_MAX_OUT_FRAMES              (SC_LINK_MAX_CH*SC_LINK_MAX_OUT_FRAMES_PER_CH)

#define SC_LINK_CMD_GET_PROCESSED_DATA      (0x6000)

#define SC_LINK_MAX_REQ                     (4)

typedef struct {
    VpsUtils_BufHndl bufOutQue;
    FVID2_Frame outFrames[SC_LINK_MAX_OUT_FRAMES];
    FVID2_Format outFormat;
    UInt32 outNumFrames;

} ScLink_OutObj;

typedef struct {
    VpsUtils_BufHndl inQue;

    FVID2_Format outFormat[SC_LINK_MAX_OUT_QUE];
    Vps_ScConfig scCfg[SC_LINK_MAX_OUT_QUE];
    Vps_CropConfig scCropConfig[SC_LINK_MAX_OUT_QUE];

} ScLink_ChObj;

typedef struct {
    FVID2_ProcessList processList;

    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList[SC_LINK_MAX_OUT_QUE];

} ScLink_ReqObj;

typedef struct {
    UInt32 linkId;

    VpsUtils_TskHndl tsk;

    ScLink_CreateParams createArgs;

    System_LinkInfo *pInTskInfo;
    System_LinkQueInfo inQueInfo;

    ScLink_ReqObj reqObj[SC_LINK_MAX_REQ];
    VpsUtils_QueHandle reqQue;
    ScLink_ReqObj *reqQueMem[SC_LINK_MAX_REQ];
    UInt32 reqQueCount;
    Bool isReqPend;
    UInt32 reqNumOutLists;

    ScLink_OutObj outObj[SC_LINK_MAX_OUT_QUE];
    System_LinkInfo info;

    Vps_M2mScChParams drvChArgs[SC_LINK_MAX_CH];
    ScLink_ChObj chObj[SC_LINK_MAX_CH];

    Vps_M2mScCreateParams drvCreateArgs;
    Vps_M2mScCreateStatus drvCreateStatus;
    FVID2_Handle fvidHandle;
    FVID2_ProcessList errProcessList;
    UInt32 drvInstId;

    UInt32 inFrameGetCount;
    UInt32 inFrameSkipCount;
    UInt32 inFramePutCount;
    UInt32 outFrameGetCount[SC_LINK_MAX_OUT_QUE];
    UInt32 outFramePutCount[SC_LINK_MAX_OUT_QUE];
    UInt32 processFrameReqPendCount;
    UInt32 processFrameReqPendSubmitCount;
    UInt32 processFrameCount;
    UInt32 getProcessFrameCount;
    UInt32 processFrameReqCount;
    UInt32 getProcessFrameReqCount;
} ScLink_Obj;

/* ===================================================================
 *  @func     ScLink_getInfo                                               
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
System_LinkInfo *ScLink_getInfo(VpsUtils_TskHndl * pTsk);

/* ===================================================================
 *  @func     ScLink_getFullFrames                                               
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
Int32 ScLink_getFullFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                           FVID2_FrameList * pFrameList);
/* ===================================================================
 *  @func     ScLink_putEmptyFrames                                               
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
Int32 ScLink_putEmptyFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     ScLink_drvCreate                                               
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
Int32 ScLink_drvCreate(ScLink_Obj * pObj, ScLink_CreateParams * pPrm);

/* ===================================================================
 *  @func     ScLink_drvProcessData                                               
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
Int32 ScLink_drvProcessData(ScLink_Obj * pObj);

/* ===================================================================
 *  @func     ScLink_drvGetProcessedData                                               
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
Int32 ScLink_drvGetProcessedData(ScLink_Obj * pObj);

/* ===================================================================
 *  @func     ScLink_drvStop                                               
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
Int32 ScLink_drvStop(ScLink_Obj * pObj);

/* ===================================================================
 *  @func     ScLink_drvDelete                                               
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
Int32 ScLink_drvDelete(ScLink_Obj * pObj);

/* ===================================================================
 *  @func     ScLink_drvSubmitData                                               
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
Int32 ScLink_drvSubmitData(ScLink_Obj * pObj);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
