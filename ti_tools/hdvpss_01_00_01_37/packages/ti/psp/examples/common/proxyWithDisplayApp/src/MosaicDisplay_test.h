/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file MosaicDisplay_test.h
 *
 *  \brief Defines the mosaic layout parameter table and other test related
 *  parameters.
 */

#ifndef _MOSAIC_DISPLAY_TEST_H
#define _MOSAIC_DISPLAY_TEST_H

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

/* Driver instance to test. */
#define DRIVER_INSTANCE                 (VPS_DISP_INST_BP0)

/* Input buffer parameters. Make sure that pitch is a multiple of 16 bytes. */
#define BUFFER_WIDTH                    (1920u)
#define BUFFER_HEIGHT                   (1080u)
#define DATA_FORMAT                     (FVID2_DF_YUV422I_YUYV)
/* Total number of buffers to allocate.
 * This should be the number of frames that should be loaded in to the
 * input buffer. */
#define TOTAL_NUM_BUFFERS               (5u)

/* Maximum number of mosaic windows in a frame. */
#define MAX_MOSAIC_FRAMES               (16u)
/* Maximum number of mosaic layout object to allocate. This should be greater
 * than the number of layouts specified in mosaic layout table. */
#define MAX_MOSAIC_LAYOUT               (16u)

/* Number of loop count to run the application. */
#define TOTAL_LOOP_COUNT                (2000u)

/* Used in dynamic layout change test - Layout will be changed every
 * switch rate frames. To disable dynamic layout change, set this to more
 * than the application loop count. */
#define LAYOUT_SWITCH_RATE              (180u)  /* Every 3 secs with 60 FPS */
/* Input frames will be increment only every this many frames. */
#define FRAME_SWITCH_RATE               (60u)   /* Every sec with 60 FPS */

/* Number of buffers to prime at the start. */
#define NUM_PRIMING                     (3u)

/* Number of FVID2 frame objects to allocate. */
#define NUM_FRAMES                      (NUM_PRIMING * MAX_MOSAIC_FRAMES)

/* Application name string used in print statements. */
#define APP_NAME                        "MosaicDisplay"

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)

/* Set this macro to FALSE to disable all mosaic operations */
#define MOSAIC_ENABLE                   (TRUE)


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
    UInt32                  numWindows;
    /**< Total number of windows in this layout. */
    Vps_WinFormat           winFmt[MAX_MOSAIC_FRAMES];
    /**< Window parameters for each of the windows. */
} App_MosaicObj;

/**
 *  \brief Display application object.
 */
typedef struct
{
    UInt32                  driverInst;
    /**< Display driver instance. */
    FVID2_ModeInfo          modeInfo;
    /**< Information about the standard. */
    UInt32                  vencId;
    /**< VENC ID to display. */

    UInt32                  width;
    /*< Buffer width. */
    UInt32                  height;
    /*< Buffer height. */
    UInt32                  pitch;
    /*< Buffer pitch. */
    UInt32                  dataFormat;
    /**< Data format - YUV420SP or YUV422SP. */

    FVID2_Handle            fvidHandle;
    /**< Display driver handle. */
    FVID2_Handle            dcHandle;
    /**< Display controller handle. */
    FVID2_Handle            hdmiHandle;
    /**< HDMI transmitter handle. */

    FVID2_FrameList         frameList;
    /**< Frame list used for queue/dequeue operations. */
    FVID2_FrameList         errFrameList;
    /**< Frame list used for error operations. */

    UInt8                  *inBuf;
    /**< Input buffer used to load the input video frames. */
    UInt32                  inFrmSize;
    /**< Input frame size. */

    UInt32                  bufferIndex;
    /**< Current buffer index. Used to wrap around after TOTAL_NUM_BUFFERS
         iteration. */
    UInt32                  submittedFrames;
    /**< Total number of frames submitted to driver. */
    UInt32                  completedFrames;
    /**< Total number of completed frames. */
    Semaphore_Handle        readtyToDqSem;
    /**< Frame complete semaphore used to sync between callback and task. */

    App_MosaicObj          *curLayout;
    /**< Pointer to the currently used layout object. */
    UInt32                  curLayoutIdx;
    /**< Index to the currently used layout object. */
    App_MosaicObj           mosaicInfo[MAX_MOSAIC_LAYOUT];
    /**< List of mosaic object containing various layout settings. */
    UInt32                  drvMaxLayouts;
    /**< Maximum number of mosaic layouts supported by driver. */
    UInt32                  totalLayouts;
    /**< Total number of layout created depending on application request and
         driver support. */
    Bool                    mosaicEnable;
    /**< Enable/disable all mosaic operations. */

    HeapBuf_Handle          frameHandle;
    /**< Frame memory pool handle. */
    UInt64                  framePool[
                                ((sizeof(FVID2_Frame) + sizeof(UInt64) - 1u)
                                    / sizeof(UInt64)) * NUM_FRAMES];
    /**< Frame memory pool used for queue/dequeue operations. Make it aligned
         to 8 byte boundary. */

    Vps_PlatformId          platformId;
    /**< Platform identifier. */
    Vps_PlatformCpuRev      cpuRev;
    /**< CPU version identifier. */
    Vps_PlatformBoardId     boardId;
    /**< Board identifier. */
} App_DisplayObj;

/* Mosaic layout table for 1920x1080 resolution. */
#define APP_DISP_LAYOUT_TABLE_DEF_1080                                         \
/* 1x1 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    1u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u,                  /* winWidth */                             \
            1080u,                  /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 1x1 (Non-Full screen) */                                                    \
{                                                                              \
    0u,                             /* layoutId */                             \
    1u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            600u,                   /* winStartX */                            \
            300u,                   /* winStartY */                            \
            720u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 2x2 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    4u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/2u,               /* winWidth */                             \
            1080u/2u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/2u,               /* winWidth */                             \
            1080u/2u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            0u,                     /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/2u,               /* winWidth */                             \
            1080u/2u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/2u,               /* winWidth */                             \
            1080u/2u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 4x4 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    16u,                            /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/4u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            1080u/4u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/4u,               /* winStartX */                            \
            1080u/4u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            1080u/4u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            1080u/4u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/4u,               /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 3 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/4u,               /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 3x3 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    9u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/3u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*2u)/3u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            1080u/3u,               /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/3u,               /* winStartX */                            \
            1080u/3u,               /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*2u)/3u,          /* winStartX */                            \
            1080u/3u,               /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (1080u*2u)/3u,          /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/3u,               /* winStartX */                            \
            (1080u*2u)/3u,          /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*2u)/3u,          /* winStartX */                            \
            (1080u*2u)/3u,          /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
    }                                                                          \
},                                                                             \
/* 8 CH (Full screen) */                                                       \
{                                                                              \
    0u,                             /* layoutId */                             \
    8u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            (1920u/4u)*3u,          /* winWidth */                             \
            (1080u/4u)*3u,          /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            1080u/4u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            1080u/2u,               /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 3 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/4u,               /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/2u,               /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*3u)/4u,          /* winStartX */                            \
            (1080u*3)/4u,           /* winStartY */                            \
            1920u/4u,               /* winWidth */                             \
            1080u/4u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 6 CH (Full screen) */                                                       \
{                                                                              \
    0u,                             /* layoutId */                             \
    6u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            (1920u/3u)*2u,          /* winWidth */                             \
            (1080u/3u)*2u,          /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*2u)/3u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            (1920u*2u)/3u,          /* winStartX */                            \
            1080u/3u,               /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (1080u*2u)/3u,          /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1920u/3u,               /* winStartX */                            \
            (1080u*2u)/3u,          /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1920u*2u)/3u,          /* winStartX */                            \
            (1080u*2u)/3u,          /* winStartY */                            \
            1920u/3u,               /* winWidth */                             \
            1080u/3u,               /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
    }                                                                          \
},                                                                             \
/* 1x2 (Non-Full screen) */                                                    \
{                                                                              \
    0u,                             /* layoutId */                             \
    2u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            240u,                   /* winStartX */                            \
            0u,                     /* winStartY */                            \
            720u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            960u,                   /* winStartX */                            \
            0u,                     /* winStartY */                            \
            720u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
    }                                                                          \
}

/* Mosaic layout table for 1280x720 resolution. */
#define APP_DISP_LAYOUT_TABLE_DEF_720                                          \
/* 1x1 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    1u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u,                  /* winWidth */                             \
            720u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 1x1 (Non-Full screen) */                                                    \
{                                                                              \
    0u,                             /* layoutId */                             \
    1u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            280u,                   /* winStartX */                            \
            120u,                   /* winStartY */                            \
            720u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 2x2 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    4u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/2u,               /* winWidth */                             \
            720u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/2u,               /* winWidth */                             \
            720u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            0u,                     /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/2u,               /* winWidth */                             \
            720u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/2u,               /* winWidth */                             \
            720u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 4x4 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    16u,                            /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/4u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            720u/4u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/4u,               /* winStartX */                            \
            720u/4u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            720u/4u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            720u/4u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/4u,               /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 3 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/4u,               /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 3x3 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    9u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/3u,               /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*2u)/3u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            720u/3u,                /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/3u,               /* winStartX */                            \
            720u/3u,                /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*2u)/3u,          /* winStartX */                            \
            720u/3u,                /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (720u*2u)/3u,           /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/3u,               /* winStartX */                            \
            (720u*2u)/3u,           /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*2u)/3u,          /* winStartX */                            \
            (720u*2u)/3u,           /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
    }                                                                          \
},                                                                             \
/* 8 CH (Full screen) */                                                       \
{                                                                              \
    0u,                             /* layoutId */                             \
    8u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            (1280u/4u)*3u,          /* winWidth */                             \
            (720u/4u)*3u,           /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            720u/4u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            720u/2u,                /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 3 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/4u,               /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/2u,               /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*3u)/4u,          /* winStartX */                            \
            (720u*3)/4u,            /* winStartY */                            \
            1280u/4u,               /* winWidth */                             \
            720u/4u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 6 CH (Full screen) */                                                       \
{                                                                              \
    0u,                             /* layoutId */                             \
    6u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            (1280u/3u)*2u,          /* winWidth */                             \
            (720u/3u)*2u,           /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*2u)/3u,          /* winStartX */                            \
            0u,                     /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 1 */                                                            \
        {                                                                      \
            (1280u*2u)/3u,          /* winStartX */                            \
            720u/3u,                /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        /* Row 2 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            (720u*2u)/3u,           /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            1280u/3u,               /* winStartX */                            \
            (720u*2u)/3u,           /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            (1280u*2u)/3u,          /* winStartX */                            \
            (720u*2u)/3u,           /* winStartY */                            \
            1280u/3u,               /* winWidth */                             \
            720u/3u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
    }                                                                          \
},                                                                             \
/* 1x2 (Non-Full screen) */                                                    \
{                                                                              \
    0u,                             /* layoutId */                             \
    2u,                             /* numWindows */                           \
    {                                                                          \
        /* Row 0 */                                                            \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            640u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            640u,                   /* winStartX */                            \
            0u,                     /* winStartY */                            \
            640u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
    }                                                                          \
}


/* Mosaic layout table for D1 resolution. */
#define APP_DISP_LAYOUT_TABLE_DEF_D1                                           \
/* 1x1 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    1u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            720u,                   /* winWidth */                             \
            480u,                   /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
},                                                                             \
/* 2x2 (Full screen) */                                                        \
{                                                                              \
    0u,                             /* layoutId */                             \
    4u,                             /* numWindows */                           \
    {                                                                          \
        {                                                                      \
            0u,                     /* winStartX */                            \
            0u,                     /* winStartY */                            \
            720u/2u,                /* winWidth */                             \
            480u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            720u/2u,                /* winStartX */                            \
            0u,                     /* winStartY */                            \
            720u/2u,                /* winWidth */                             \
            480u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            0u,                     /* winStartX */                            \
            480u/2u,                /* winStartY */                            \
            720u/2u,                /* winWidth */                             \
            480u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        },                                                                     \
        {                                                                      \
            720u/2u,                /* winStartX */                            \
            480u/2u,                /* winStartY */                            \
            720u/2u,                /* winWidth */                             \
            480u/2u,                /* winHeight */                            \
            {0u, 0u, 0u},           /* pitch[] */                              \
            FVID2_DF_YUV422I_YUYV,  /* dataFormat */                           \
            FVID2_BPP_BITS16,       /* bpp */                                  \
            0u                      /* priority */                             \
        }                                                                      \
    }                                                                          \
}

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _MOSAIC_DISPLAY_TEST_H */
