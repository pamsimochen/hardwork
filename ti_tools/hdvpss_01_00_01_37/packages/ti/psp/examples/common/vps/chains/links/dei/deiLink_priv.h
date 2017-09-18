/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEI_LINK_PRIV_H_
#define _DEI_LINK_PRIV_H_

#include <ti/psp/examples/common/vps/chains/links/deiLink.h>
#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>


#ifdef TI_816X_BUILD
#define DEI_LINK_OBJ_MAX              (4)
#endif /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#define DEI_LINK_OBJ_MAX              (2)
#endif /* TI_814X_BUILD || TI_8107_BUILD */


#define DEI_LINK_MAX_CH                      (8)
#define DEI_LINK_MAX_OUT_FRAMES_PER_CH       (SYSTEM_LINK_FRAMES_PER_CH+2)

#define DEI_LINK_MAX_OUT_FRAMES              (DEI_LINK_MAX_CH*DEI_LINK_MAX_OUT_FRAMES_PER_CH)

#define DEI_LINK_CMD_GET_PROCESSED_DATA      (0x6000)

#define DEI_LINK_MAX_REQ                     (4)

typedef struct
{
    VpsUtils_BufHndl    bufOutQue;
    FVID2_Frame         outFrames[DEI_LINK_MAX_OUT_FRAMES];
    System_FrameInfo    frameInfo[DEI_LINK_MAX_OUT_FRAMES];
    FVID2_Format        outFormat;
    UInt32              outNumFrames;

} DeiLink_OutObj;

typedef struct
{
    VpsUtils_BufHndl inQue;
    UInt32  nextFid;

    FVID2_Format    outFormat[DEI_LINK_MAX_OUT_QUE];
    Vps_DeiHqConfig deiHqCfg;
    Vps_DeiConfig   deiCfg;
    Vps_ScConfig    scCfg[DEI_LINK_MAX_OUT_QUE];
    Vps_CropConfig  scCropConfig[DEI_LINK_MAX_OUT_QUE];

} DeiLink_ChObj;

typedef struct
{
    FVID2_ProcessList processList;

    FVID2_FrameList inFrameList;
    FVID2_FrameList outFrameList[DEI_LINK_MAX_OUT_QUE];

} DeiLink_ReqObj;

typedef struct
{
    UInt32 linkId;

    VpsUtils_TskHndl tsk;

    DeiLink_CreateParams createArgs;

    System_LinkInfo     *pInTskInfo;
    System_LinkQueInfo   inQueInfo;

    DeiLink_ReqObj       reqObj[DEI_LINK_MAX_REQ];
    VpsUtils_QueHandle   reqQue;
    DeiLink_ReqObj      *reqQueMem[DEI_LINK_MAX_REQ];
    UInt32               reqQueCount;
    Bool                 isReqPend;
    UInt32               reqNumOutLists;

    DeiLink_OutObj       outObj[DEI_LINK_MAX_OUT_QUE];
    System_LinkInfo      info;

    Vps_M2mDeiChParams      drvChArgs[DEI_LINK_MAX_CH];
    DeiLink_ChObj           chObj[DEI_LINK_MAX_CH];

    Vps_M2mDeiCreateParams  drvCreateArgs;
    Vps_M2mDeiCreateStatus  drvCreateStatus;
    FVID2_Handle            fvidHandle;
    FVID2_ProcessList       errProcessList;
    UInt32                  drvInstId;
    UInt32                  vipInstId; // used when reloading SC co-effs after reset

    FVID2_Frame             outFrameDrop;

    /* Semaphore to signal completion of DEI */
    Semaphore_Handle        complete;

    UInt32                  inFrameGetCount;
    UInt32                  inFrameSkipCount;
    UInt32                  inFramePutCount;
    UInt32                  outFrameGetCount[DEI_LINK_MAX_OUT_QUE];
    UInt32                  outFramePutCount[DEI_LINK_MAX_OUT_QUE];
    UInt32                  processFrameReqPendCount;
    UInt32                  processFrameReqPendSubmitCount;
    UInt32                  processFrameCount;
    UInt32                  getProcessFrameCount;
    UInt32                  processFrameReqCount;
    UInt32                  getProcessFrameReqCount;

    UInt32                  totalTime;
    UInt32                  curTime;

    UInt32                  givenInFrames;
    UInt32                  returnedInFrames;

    Bool                    loadUpsampleCoeffs;

} DeiLink_Obj;



System_LinkInfo *DeiLink_getInfo(VpsUtils_TskHndl *pTsk);
Int32 DeiLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList);
Int32 DeiLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList);

Int32 DeiLink_drvCreate(DeiLink_Obj *pObj, DeiLink_CreateParams *pPrm);
Int32 DeiLink_drvProcessData(DeiLink_Obj *pObj);
Int32 DeiLink_drvGetProcessedData(DeiLink_Obj *pObj);
Int32 DeiLink_drvStop(DeiLink_Obj *pObj);
Int32 DeiLink_drvDelete(DeiLink_Obj *pObj);

#endif
