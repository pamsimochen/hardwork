/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file TriDisplay.h
 *
 *  \brief Triple display example header file containing test parameters.
 *
 */

#ifndef _TRIDISPLAY_H
#define _TRIDISPLAY_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps_displayCtrl.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Number of loop count to run the application. */
#define TOTAL_LOOP_COUNT                (1000u)

/* Parameters for BP0 and BP1 paths. */
#define HD_FRAME_WIDTH                  (1920u)
#define HD_FRAME_HEIGHT                 (1080u)
#define HD_WINDOW_WIDTH                 (720u)
#define HD_WINDOW_HEIGHT                (480u)
#define HD_SCAN_FORMAT                  (FVID2_SF_INTERLACED)
#define HD_NUM_MOSAIC_COL               (2u)
#define HD_NUM_MOSAIC_ROW               (2u)

/* Parameters for SEC1 path. */
#define SD_FRAME_WIDTH                  (720u)
#define SD_FRAME_HEIGHT                 (480u)
#define SD_WINDOW_WIDTH                 (720u)
#define SD_WINDOW_HEIGHT                (480u)
#define SD_SCAN_FORMAT                  (FVID2_SF_INTERLACED)
#define SD_NUM_MOSAIC_COL               (1u)
#define SD_NUM_MOSAIC_ROW               (1u)

/* Input buffer parameters. Make sure that pitch is a multiple of 16 bytes.
 * and HD/SD window width and height is <= the buffer dimensions. */
#define BUFFER_WIDTH                    (720u)
#define BUFFER_HEIGHT                   (480u)
#define BUFFER_PITCH                    (BUFFER_WIDTH * 2u)
#define BUFFER_SIZE                     (BUFFER_PITCH * BUFFER_HEIGHT)

/* Parameters for GRPX 0, 1 and 2 paths. */
#define GRPX_WIDTH                      (640u)
#define GRPX_HEIGHT                     (80u)
#define GRPX_BUFFER_PITCH               (GRPX_WIDTH * 3u)
#define GRPX_BUFFER_SIZE                (GRPX_BUFFER_PITCH * GRPX_HEIGHT)

/* Maximum number of mosaic windows in a frame. */
#define MAX_MOSAIC_FRAMES               (HD_NUM_MOSAIC_ROW * HD_NUM_MOSAIC_COL)

/* Total number of buffers to allocate.
 * This should be the number of frames that should be loaded in to the
 * input buffer. */
#define TOTAL_NUM_BUFFERS               (10u)
#define TOTAL_NUM_GRPX_BUFFERS          (1u)

/* Number of framelist used for priming. */
#define NUM_PRIMING                     (3u)

/* Number of framelist used for priming. */
#define NUM_FRAMELIST                   (NUM_PRIMING)

/* Number of displays. */
#define NUM_VIDEO_DISPLAYS              (3u)
#define NUM_GRPX_DISPLAYS               (3u)

/* Application name string used in print statements. */
#define APP_NAME                        "TriDisplay"


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Display application mosaic information object.
 */
typedef struct
{
    Vps_LayoutId            layoutId;
    /**< Layout ID as returned by create layout IOCTL used for selecting a
         layout at runtime. */
    UInt32                  numCol;
    /**< Number of columns for this layout. */
    UInt32                  numRow;
    /**< Number of rows for this layout. */
    UInt32                  numWindows;
    /**< Total number of windows in this layout. */
} App_MosaicObj;

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

    App_MosaicObj           mosaicInfo;
    /**< List of mosaic object containing various layout settings. */

    UInt32                  bufferIndex;
    /**< Current buffer index. Used to wrap around after TOTAL_NUM_BUFFERS
         iteration. */
    UInt32                  submittedFrames;
    /**< Total number of frames submitted to driver. */
    UInt32                  completedFrames;
    /**< Total number of completed frames. */
    Semaphore_Handle        readtyToDqSem;
    /**< Frame complete semaphore used to sync between callback and task. */

    FVID2_FrameList         frameList[NUM_FRAMELIST];
    /**< Frame list used for queue/dequeue operations. */
    FVID2_Frame             frames[NUM_FRAMELIST][MAX_MOSAIC_FRAMES];
    /**< Frames used for queue/dequeue operations. */
    FVID2_FrameList         errFrameList;
    /**< Frame list used for error operations. */
    UInt32                  isActive;
} App_DisplayObj;


typedef struct
{
    char         comboName[100];
    Vps_DcConfig dcConfig;
    UInt32       isDispActive[NUM_VIDEO_DISPLAYS];
    UInt32       isGrpxActive[NUM_GRPX_DISPLAYS];
} App_DispComboObj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 App_dispInitGrpx(App_DispComboObj *combObj);
Int32 App_dispDeInitGrpx(App_DispComboObj *combObj);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _TRIDISPLAY_H */
