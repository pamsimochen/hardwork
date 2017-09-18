/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SW_MS_LINK_PRIV_H_
#define _SW_MS_LINK_PRIV_H_

#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>
#include <ti/psp/examples/common/vps/chains/links/swMsLink.h>
#include <ti/sysbios/knl/Clock.h>


#define SW_MS_LINK_OBJ_MAX              (3)

#define SW_MS_LINK_CMD_DO_SCALING       (0x0500)

#define SW_MS_LINK_MAX_OUT_FRAMES       (SYSTEM_LINK_FRAMES_PER_CH)

#define SW_MS_LINK_TIMER_DEFAULT_PERIOD (16)

#define SW_MS_MAX_DEI_CH                (8)

#define SW_MS_SKIP_PROCESSING           (2)

typedef struct
{
    Bool                applyRtPrm;
    UInt32              repeatFrameCount;
    UInt32              expectedFid;
    FVID2_ScanFormat    scanFormat;
    FVID2_Frame         blankFrame;
    FVID2_Frame         curOutFrame;
    FVID2_Frame        *pCurInFrame;
    VpsUtils_BufHndl    bufInQue;
    Vps_FrameParams     scRtInFrmPrm;
    Vps_FrameParams     scRtOutFrmPrm;
    Vps_M2mDeiRtParams  deiRtPrm;
    Vps_DeiRtConfig     deiRtCfg;
    Vps_M2mScRtParams   scRtPrm;
    Vps_ScRtConfig      scRtCfg;

} SwMsLink_OutWinObj;



typedef struct
{
    Bool                    bypassDei;
    UInt32                  drvInstId;
    Vps_M2mDeiCreateParams  deiCreateParams;
    Vps_M2mDeiCreateStatus  deiCreateStatus;
    Vps_M2mDeiChParams      deiChParams[SYSTEM_SW_MS_MAX_WIN];
    FVID2_Handle            fvidHandle;
    FVID2_ProcessList       errCbProcessList;
    FVID2_FrameList         inFrameList;
    FVID2_FrameList         outFrameList;
    FVID2_ProcessList       processList;
    Semaphore_Handle        complete;
    FVID2_Format            drvOutFormat[SYSTEM_SW_MS_MAX_WIN];
    Vps_DeiHqConfig         deiHqCfg;
    Vps_DeiConfig           deiCfg;
    Vps_ScConfig            scCfg;
    Vps_CropConfig          scCropCfg[SYSTEM_SW_MS_MAX_WIN];

} SwMsLink_DeiDrvObj;



typedef struct
{
    UInt32                 drvInstId;
    Vps_M2mScCreateParams  scCreateParams;
    Vps_M2mScCreateStatus  scCreateStatus;
    Vps_M2mScChParams      scChParams[SYSTEM_SW_MS_MAX_WIN];
    FVID2_Handle           fvidHandle;
    FVID2_ProcessList      errCbProcessList;
    FVID2_FrameList        inFrameList;
    FVID2_FrameList        outFrameList;
    FVID2_ProcessList      processList;
    Semaphore_Handle       complete;
    Vps_ScConfig           scCfg;
    Vps_CropConfig         scCropCfg[SYSTEM_SW_MS_MAX_WIN];
    
} SwMsLink_ScDrvObj;



typedef struct
{
    UInt32                 linkId;
    
    Bool                   isDei;

    VpsUtils_TskHndl       tsk;

    SwMsLink_CreateParams  createArgs;
    System_LinkInfo        info;
    System_LinkInfo       *pInTskInfo;
    System_LinkQueInfo     inQueInfo;

    SwMsLink_LayoutParams  layoutPrm;
    SwMsLink_LayoutInfo    layoutInfo;

    Bool                   switchLayout;

    SwMsLink_OutWinObj     winObj[SYSTEM_SW_MS_MAX_WIN];

    Semaphore_Handle       lock;

    Clock_Handle           timer;
    UInt32                 timerPeriod;

    VpsUtils_BufHndl       bufOutQue;
    FVID2_Frame            outFrames[SW_MS_LINK_MAX_OUT_FRAMES];
    System_FrameInfo       outFrameInfo[SW_MS_LINK_MAX_OUT_FRAMES];
    FVID2_Format           outFrameFormat;
    FVID2_Format           bufferFrameFormat;

    FVID2_FrameList        freeFrameList;

    SwMsLink_DeiDrvObj     deiDrvObj;
    SwMsLink_DeiDrvObj     deiBypassDrvObj;
    SwMsLink_ScDrvObj      scDrvObj;

    UInt32                 frameCount;
    UInt32                 totalTime;

    UInt32                 skipProcessing;
} SwMsLink_Obj;



System_LinkInfo *SwMsLink_getInfo(VpsUtils_TskHndl *pTsk);
Int32 SwMsLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList);
Int32 SwMsLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList);

Int32 SwMsLink_drvCreate(SwMsLink_Obj *pObj, SwMsLink_CreateParams *pPrm);
Int32 SwMsLink_drvStart(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvProcessData(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvDoScaling(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvMakeFrameLists(SwMsLink_Obj *pObj, FVID2_Frame *pOutFrame);
Int32 SwMsLink_drvStop(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvDelete(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvLock(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvUnlock(SwMsLink_Obj *pObj);

Int32 SwMsLink_drvSwitchLayout(SwMsLink_Obj *pObj, SwMsLink_LayoutParams *pLayoutPrm);
Int32 SwMsLink_drvCreateWinObj(SwMsLink_Obj *pObj, UInt32 winId);
Int32 SwMsLink_drvCreateDeiDrv(SwMsLink_Obj *pObj, Bool bypassDrv);
Int32 SwMsLink_drvCreateScDrv(SwMsLink_Obj *pObj);
Int32 SwMsLink_drvDeleteDrv(SwMsLink_Obj *pObj, Bool bypassDrv);

#endif
