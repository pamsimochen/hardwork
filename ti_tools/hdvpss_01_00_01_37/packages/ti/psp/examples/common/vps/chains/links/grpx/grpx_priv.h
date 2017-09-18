/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file grpxLink_priv.h
 *
 *  \brief
 *
 */

#ifndef _GRPX_LINK_PRIV_H
#define _GRPX_LINK_PRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Parameters for BP0 and BP1 paths. */
#define HD_FRAME_WIDTH                  (1920u)
#define HD_FRAME_HEIGHT                 (1080u)

/* Parameters for SEC1 path. */
#define SD_FRAME_WIDTH                  (720u)
#define SD_FRAME_HEIGHT                 (480u)

/* Parameters for GRPX 0, 1 and 2 paths. */
#define GRPX_BPP                        (3u)
#define GRPX_WIDTH                      (1920u)
#define GRPX_HEIGHT                     (1080u)
#define GRPX_BUFFER_PITCH               (GRPX_WIDTH * GRPX_BPP)
#define GRPX_BUFFER_SIZE                (GRPX_BUFFER_PITCH * GRPX_HEIGHT)

#define GRPX_ICON_BPP                   (3u)
#define GRPX_ICON_WIDTH                 (640)
#define GRPX_ICON_HEIGHT                (80)

/* From right top corner */
#define GRPX_STARTX                     (10u)
/* From top line */
#define GRPX_STARTY                     (20u)

/* Number of framelist used for priming. */
#define NUM_PRIMING                     (3u)

/* Maximum number of GRPX displays. */
#define NUM_GRPX_DISPLAYS               (3u)

/* Application name string used in print statements. */
#define APP_NAME                        "GRPX"

#if 0
#define NUM_REGIONS                     (VPS_GRPX_MAX_REGIONS_PER_FRAME)
#else
#define NUM_REGIONS                     (1)
#endif
#define REGION_HEIGHT                   (GRPX_HEIGHT/NUM_REGIONS)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Display application object.
 */
typedef struct
{
    FVID2_Handle            fvidHandle;
    /**< Display driver handle. */
    UInt32                  driverId;
    /**< Display driver ID - Video or GRPX driver. */
    UInt32                  driverInst;
    /**< Display driver instance. */

    UInt8                  *inBuf;
    /**< Input buffer used to load the input video frames. */
    UInt32                  inFrmSize;
    /**< Input frame size. */
    UInt32                  numInBuf;
    /**< Number of input buffers. */

    UInt32                  frameWidth;
    /**< VENC display frame width. */
    UInt32                  frameHeight;
    /**< VENC display frame height. */
    UInt32                  windowWidth;
    /**< Each window/region frame width. */
    UInt32                  windowHeight;
    /**< Each window/region frame height. */
    FVID2_ScanFormat        scanFormat;
    /**< Display scan format. */

    UInt32                  bufferIndex;
    /**< Current buffer index. Used to wrap around after TOTAL_NUM_BUFFERS
         iteration. */
    UInt32                  submittedFrames;
    /**< Total number of frames submitted to driver. */
    UInt32                  completedFrames;
    /**< Total number of completed frames. */

    FVID2_FrameList         frameList;
    /**< Frame list used for queue/dequeue operations. */
    FVID2_Frame             frames[NUM_PRIMING*NUM_REGIONS];
    /**< Frames used for queue/dequeue operations. */
    FVID2_FrameList         errFrameList;
    /**< Frame list used for error operations. */

    Vps_GrpxRtParams        grpxRtPrms[NUM_REGIONS];
    Vps_WinFormat           winFormat[NUM_REGIONS];

} App_DisplayObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _GRPX_LINK_PRIV_H */
