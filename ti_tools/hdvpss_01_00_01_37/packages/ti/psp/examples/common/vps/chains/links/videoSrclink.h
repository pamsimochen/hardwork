/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**

    Video Src Link can be used to provide input to the next frames
    This is used to integrate other links when capture link is not available.

    This is useful when capture link is not used but some input is needed for other links


    @{
*/

/**
    \file VideoSrclink.h
    \brief Video Source Link API
*/

#ifndef _VIDEO_SRC_LINK_H_
#define _VIDEO_SRC_LINK_H_

#include <ti/psp/examples/common/vps/chains/links/system.h>

#ifdef __cplusplus
extern "C" {
#endif

/**\brief Max output queues to which a given Video sourcel link can connect to */
#define VIDEO_SRC_LINK_MAX_OUT_QUE          (4)

/** \brief Max Channels per output queue */
#define VIDEO_SRC_LINK_MAX_CH_PER_OUT_QUE   (16)

typedef struct
{
    FVID2_DataFormat    dataFormat;
    Vps_VpdmaMemoryType memType;
    /** Tiled / non-tiled */
    UInt16              width;
    UInt16              height;
    UInt32              pitch[FVID2_MAX_PLANES];
    FVID2_ScanFormat    scanFormat;

} VideoSrc_Input;

/**
*    brief Video source link create parameters
*/
typedef struct
{
    UInt32                    numOutQue;
    /**< Number of out queues */

    System_LinkOutQueParams   OutQueParams[VIDEO_SRC_LINK_MAX_OUT_QUE];
    /**< output queue information */

    VideoSrc_Input            InputInfo[VIDEO_SRC_LINK_MAX_CH_PER_OUT_QUE];
    /**< Input information for all channels */

    /** Number of channels per out que **/
    UInt16                    numchannels;

} VideoSrcLink_CreateParams;

/**
    \brief Video source link register and init

    - Creates link task
    - Registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 VideoSrcLink_init();

/**
    \brief Video source link de-register and de-init

    - Deletes link task
    - De-registers as a link with the system API

    \return FVID2_SOK on success
*/
Int32 VideoSrcLink_deInit();

#ifdef __cplusplus
}
#endif

#endif

/*@}*/

