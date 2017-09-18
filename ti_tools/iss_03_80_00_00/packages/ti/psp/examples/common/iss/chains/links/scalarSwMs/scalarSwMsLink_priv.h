/** ==================================================================
 *  @file   scalarSwMsLink_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/scalarSwMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _SCALAR_SW_MS_LINK_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SCALAR_SW_MS_LINK_PRIV_H_

#include <ti/psp/examples/common/iss/chains/links/scalarSwMsLink.h>
#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/psp/examples/utility/vpsutils_tsk.h>
#include <ti/psp/examples/utility/vpsutils_buf.h>

#define SCALAR_SW_MS_LINK_OBJ_MAX              (4)

#define SCALAR_SW_MS_LINK_CMD_DO_SCALING       (0x0400)

#define SCALAR_SW_MS_LINK_MAX_WIN              (16)

#define SCALAR_SW_MS_LINK_MAX_OUT_FRAMES       (SYSTEM_LINK_FRAMES_PER_CH)

#define SCALAR_SW_MS_LINK_TIMER_DEFAULT_PERIOD (33)

#define SCALAR_SW_MS_LINK_BLANK_THRESHOLD      (60*30000)

/* ===================================================================
 *  @func     ScalarSwMsLink_makeChannelNum                                               
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
static inline UInt32 ScalarSwMsLink_makeChannelNum(UInt32 winId, UInt32 fid)
{
    return winId * FVID2_MAX_FIELDS + fid;
}

typedef struct {
    UInt32 channelNum;

    UInt32 missFrameCount[FVID2_MAX_FIELDS];
    Bool isInterlaced;

    FVID2_Frame *pCurInFrame[FVID2_MAX_FIELDS];

    VpsUtils_BufHndl bufInQue[FVID2_MAX_FIELDS];

    FVID2_Frame curOutFrame[FVID2_MAX_FIELDS];

    UInt32 outBufAddrOffset[FVID2_MAX_FIELDS];

    UInt32 outStartX;
    UInt32 outStartY;
    UInt32 outWidth;
    UInt32 outHeight;

    FVID2_Frame blankFrame[FVID2_MAX_FIELDS];

} ScalarSwMsLink_OutWinObj;

typedef struct {
    UInt32 linkId;

    VpsUtils_TskHndl tsk;

    ScalarSwMsLink_CreateParams createArgs;
    System_LinkInfo info;

    System_LinkInfo *pInTskInfo;

    System_LinkQueInfo inQueInfo;

    UInt32 layoutId;
    UInt32 startWinChannelNum;
    UInt32 numWin;

    ScalarSwMsLink_OutWinObj winObj[SCALAR_SW_MS_LINK_MAX_WIN];

    Vps_M2mScChParams winScParams[SCALAR_SW_MS_LINK_MAX_WIN * FVID2_MAX_FIELDS];

    Semaphore_Handle lock;
    Semaphore_Handle complete;

    Clock_Handle timer;
    UInt32 timerPeriod;

    VpsUtils_BufHndl bufOutQue;
    FVID2_Frame outFrames[SCALAR_SW_MS_LINK_MAX_OUT_FRAMES];
    System_FrameInfo frameInfo[SCALAR_SW_MS_LINK_MAX_OUT_FRAMES];
    FVID2_Format outFrameFormat;

    UInt32 blankThrehold;

    UInt32 drvInstId;
    UInt32 vipInstId;

    Vps_M2mScCreateParams scCreateParams;
    Vps_M2mScCreateStatus scCreateStatus;

    FVID2_Handle fvidHandle;

    Vps_ScCoeffParams scCoeff;

    FVID2_ProcessList errCbProcessList;

    UInt32 processFrameCount;
    UInt32 getProcessFrameCount;
    UInt32 frameCount;

    UInt32 inFrameGetCount;
    UInt32 inFramePutCount;

    UInt32 curTime;
    UInt32 totalTime;

    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList;
    FVID2_FrameList freeFrameList;
    FVID2_ProcessList processList;

} ScalarSwMsLink_Obj;

/* ===================================================================
 *  @func     ScalarSwMsLink_getInfo                                               
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
System_LinkInfo *ScalarSwMsLink_getInfo(VpsUtils_TskHndl * pTsk);

/* ===================================================================
 *  @func     ScalarSwMsLink_getFullFrames                                               
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
Int32 ScalarSwMsLink_getFullFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                                   FVID2_FrameList * pFrameList);
/* ===================================================================
 *  @func     ScalarSwMsLink_putEmptyFrames                                               
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
Int32 ScalarSwMsLink_putEmptyFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                                    FVID2_FrameList * pFrameList);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvCreate                                               
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
Int32 ScalarSwMsLink_drvCreate(ScalarSwMsLink_Obj * pObj,
                               ScalarSwMsLink_CreateParams * pPrm);
/* ===================================================================
 *  @func     ScalarSwMsLink_drvStart                                               
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
Int32 ScalarSwMsLink_drvStart(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvProcessData                                               
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
Int32 ScalarSwMsLink_drvProcessData(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvDoScaling                                               
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
Int32 ScalarSwMsLink_drvDoScaling(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvStop                                               
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
Int32 ScalarSwMsLink_drvStop(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvDelete                                               
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
Int32 ScalarSwMsLink_drvDelete(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvLock                                               
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
Int32 ScalarSwMsLink_drvLock(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvUnlock                                               
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
Int32 ScalarSwMsLink_drvUnlock(ScalarSwMsLink_Obj * pObj);

/* ===================================================================
 *  @func     ScalarSwMsLink_drvSwitchLayout                                               
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
Int32 ScalarSwMsLink_drvSwitchLayout(ScalarSwMsLink_Obj * pObj, UInt32 layoutId,
                                     UInt32 startChannelNum);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
