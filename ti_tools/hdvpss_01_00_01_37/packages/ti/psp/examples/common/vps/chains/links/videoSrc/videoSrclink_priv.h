/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VIDEO_SRC_LINK_PRIV_H_
#define _VIDEO_SRC_LINK_PRIV_H_


#include <ti/psp/examples/common/vps/chains/links/videoSrclink.h>
#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>


#ifdef __cplusplus
extern "C" {
#endif

#define VIDEO_SRC_LINK_OBJ_MAX                     (1)

#define VIDEO_SRC_LINK_MAX_CH                      (8)
#define VIDEO_SRC_LINK_MAX_OUT_FRAMES_PER_CH       (SYSTEM_LINK_FRAMES_PER_CH)

#define VIDEO_SRC_LINK_MAX_OUT_FRAMES              (VIDEO_SRC_LINK_MAX_CH \
                                          *VIDEO_SRC_LINK_MAX_OUT_FRAMES_PER_CH)

typedef struct
{

    FVID2_Frame         outFrames[VIDEO_SRC_LINK_MAX_OUT_FRAMES];
    FVID2_Format        outFormat;
    UInt32              outNumFrames;

} VideoSrcLink_OutObj;

typedef struct
{
    UInt32                    tskId;

    VpsUtils_TskHndl          tsk;

    VpsUtils_TskHndl          tsksend;

    VideoSrcLink_CreateParams createArgs;

    UInt32                    receviedFrameCount;

    VideoSrcLink_OutObj       outObj[VIDEO_SRC_LINK_MAX_OUT_QUE];

    VpsUtils_BufHndl          bufOutQue[VIDEO_SRC_LINK_MAX_OUT_QUE];

    /* Video source link info that is returned when queried by next link */
    System_LinkInfo info;

    volatile Bool senddata;

    /* Semaphore to signal end of senddata */
    Semaphore_Handle senddatacomplete;

} VideoSrcLink_Obj;

/* extract que CH ID from Video src driver channelNum */
static inline UInt32 VideoSrcLink_getQueChId(UInt32 channelNum ,
                UInt32 numofoutchanl)
{
    return channelNum%numofoutchanl;
}

#ifdef __cplusplus
}
#endif

#endif

