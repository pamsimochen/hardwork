/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _NSF_LINK_PRIV_H_
#define _NSF_LINK_PRIV_H_

#include <ti/psp/examples/common/vps/chains/links/nsfLink.h>
#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>

/* Maximum NSF link objects*/
#define NSF_LINK_OBJ_MAX                    (2)

/* Maximum supported output queues */
#define NSF_LINK_MAX_OUTPUTS_PER_INST       (2)

/* Maximum frames allocated per channel */
#define NSF_LINK_FRAMES_PER_CH              (SYSTEM_LINK_FRAMES_PER_CH)

/* Maximum NF channels per output queue */
#define NSF_LINK_MAX_CH_PER_QUE             (16)

/*
  Per Channel Information
*/
typedef struct
{
    /* Channel number */
    UInt32 channelId;

    /*
     * Input/output frame queue - per channel
     * VpsUtils_BufHndl.fullQue will be used to fill frames coming from
     * capture link.
     *
     * VpsUtils_BufHndl.emptyQue will be used to fill the noise-filtered frames.
     * These frames will be allocated during initialization and used in the
     * FVID2_processFrames() API.
     */
    VpsUtils_BufHndl bufInQue;

    /*
     * Maximum FVID2 Frames that may be used for noise filtering
     */
    FVID2_Frame frames[NSF_LINK_FRAMES_PER_CH];

    /*
     * Run-time parameter structure for every possible frame
     */
    System_FrameInfo frameInfo[NSF_LINK_FRAMES_PER_CH];

    /* previous output frame */
    FVID2_Frame *pPrevOutFrame;

} NsfLink_ChObj;


typedef struct
{
    /* NSF task structure */
    VpsUtils_TskHndl tsk;

    /* NSF Link create params - to be passed from the application */
    NsfLink_CreateParams createArgs;

    /* NSF link info */
    System_LinkInfo info;

    /* Previous link (Capture) info */
    System_LinkInfo *pInTskInfo;

    /* Previous link's (Capture) queue info - only 1 input queue is allowed */
    System_LinkQueInfo inQueInfo;

    /* Maximum 2 output queues */
    VpsUtils_BufHndl bufOutQue[NSF_LINK_MAX_OUT_QUE];

    VpsUtils_BufHndl bufEvenFieldOutQue;

    FVID2_Frame evenFieldFrames[NSF_LINK_FRAMES_PER_CH*NSF_LINK_MAX_CH_PER_QUE];

    UInt32 curEvenFieldFrame;

    /* NSF channels related info */
    NsfLink_ChObj linkChInfo[NSF_LINK_MAX_CH_PER_QUE];

    /* Semaphore to signal completion of noise filtering */
    Semaphore_Handle complete;

    /* NSF FVID2 handle */
    FVID2_Handle fvidHandleNsf;

    /* Error process list - not used currently */
    FVID2_ProcessList errCbProcessList;

    /* Channel specific configuration information */
    Vps_NsfDataFormat nsfDataFormat[NSF_LINK_MAX_CH_PER_QUE];
    Vps_NsfProcessingCfg nsfProcCfg[NSF_LINK_MAX_CH_PER_QUE];

    /* Create params and returned status stored here */
    Vps_NsfCreateParams nsfCreateParams;
    Vps_NsfCreateStatus nsfCreateStatus;        //TBD: used nowhere ???

    UInt32 inFrameReceiveCount;
    UInt32 inFrameGivenCount;
    UInt32 outFrameReceiveCount;
    UInt32 outFrameGivenCount;
    UInt32 processFrameReqCount;
    UInt32 getProcessFrameReqCount;
    UInt32 getFrames;

    UInt32 curTime;
    UInt32 totalTime;

} NsfLink_Obj;

System_LinkInfo *NsfLink_getInfo(VpsUtils_TskHndl *pTsk);
Int32 NsfLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId,
    FVID2_FrameList *pFrameList);
Int32 NsfLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId,
    FVID2_FrameList *pFrameList);
Int32 NsfLink_drvCreate(NsfLink_Obj *pObj, NsfLink_CreateParams *pPrm);
Int32 NsfLink_drvProcessData(NsfLink_Obj *pObj);
Int32 NsfLink_drvDoNsfFilter(NsfLink_Obj *pObj);
Int32 NsfLink_drvPutEmptyFrames(NsfLink_Obj *pObj, FVID2_FrameList *pFrameList);
Int32 NsfLink_drvDelete(NsfLink_Obj *pObj);
Int32 NsfLink_drvInitCh(NsfLink_Obj *pObj);
Int32 NsfLink_drvFreeFrames(NsfLink_Obj *pObj);

#endif

