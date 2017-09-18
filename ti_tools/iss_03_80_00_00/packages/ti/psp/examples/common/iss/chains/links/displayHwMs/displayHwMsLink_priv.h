/** ==================================================================
 *  @file   displayHwMsLink_priv.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/displayHwMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
 *  \defgroup VPSEXAMPLE_DISP_HWMS_LINK_API Display Mosaic Link API
 *
 *  Display HW Mosaic Link can be used to instantiate -mosiac display
 *  over HDMI, HDDAC or SDTV.
 *
 *  For each of the display a different display link instance needs to be
 *  created using the system API.
 *
 *  The system config ID will determine the position/size of each channel and
 *  the chain application should judicially select the config ID depending
 *  on the number of channels and number of input queues.
 *
 *  The display link can take input for multiple input queues. Each input
 *  queue could give n number of channels needed for the mosaic display.
 *  This is used in case channels from two different links needs to be
 *  displayed on the same output.
 *
 *  When a channel is not available at a VSYNC interval, the link will
 *  repeat the previous frame up to some time and after that displays the
 *  blank frame for that channel.
 *
 *  @{
 */

/**
 *  \file displayHwMsLink_priv.h
 *
 *  \brief Display Mosaic Link private header file.
 */

#ifndef _DISP_HWMS_LINK_PRIV_H_
#define _DISP_HWMS_LINK_PRIV_H_

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/examples/common/iss/chains/links/displayHwMsLink.h>
#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>
#include <ti/psp/examples/utility/vpsutils_tsk.h>

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

/** \brief Blank channel indicator. */
#define DISP_HWMS_LINK_BLK_FRM_CH_NUM   (0xFFFFu)

/** \brief Number of blank frames to be queued at start. */
#define DISP_HWMS_LINK_NUM_PRIMING      (4u)

/** \brief Maximum number of instances - BP0 and BP1. */
#define DISP_HWMS_LINK_OBJ_MAX          (2u)

/** \brief Local command for doing dequeue operation. */
#define DISP_HWMS_LINK_CMD_DO_DEQUE     (0x0500)

/** \brief Maximum number of sub-windows that can be queued for display. */
#define DISP_HWMS_LINK_MAX_QOBJ_PER_WIN (10u)

/** \brief Maximum number of sub-windows supported per layout. */
#define DISP_HWMS_LINK_MAX_SUB_WIN      (16u)

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

/**
 *  \brief Display Mosaic object per link instance.
 */
    typedef struct {
        UInt32 taskId;
    /**< Task identifier that handles an instance of the display. */
        VpsUtils_TskHndl taskHndl;
    /**< Task handle that handles an instance of the display. */

        DispHwMsLink_CreateParams createPrms;
    /**< Create time arguments associated with an instance. */
        System_LinkQueInfo inQueInfo[DISP_HWMS_LINK_MAX_IN_QUE];
    /**< Input Queue information. */

        UInt32 dispInstId;
    /**< Display driver instance ID. */
        FVID2_Handle dispHandle;
    /**< Display driver handle. */
        Semaphore_Handle lock;
    /**< Instance lock. */

        FVID2_Format dispFmt;
    /**< Display driver format. */
        Vps_LayoutId layoutId;
    /**< Place holder to store the create layout ID. */
        UInt32 totalSubWin;
    /**< Total number of sub-windows for all of the input queues. */
        Vps_WinFormat winFmt[DISP_HWMS_LINK_MAX_SUB_WIN];
    /**< Window format (position and size) for each window. */

        FVID2_Frame blankFrm;
    /**< Frame that would be displayed when we have no input frames */
        FVID2_Frame *pLastQueuedFrm[DISP_HWMS_LINK_MAX_SUB_WIN];
    /**< In case where we do not have frame for all windows but have frame for
         one or more windows, we would require to display the available frames.
         However the driver would require data for all sub-windows. In this
         case we repeat the last queued frame of the sub-window. */
        System_FrameInfo frameInfo;
    /**< Run-time parameter structure for blank frame.
         One run-time parameter is associated to one FVID2_Frame during
         link create. */

        FVID2_Frame *subWinQuePool
            [DISP_HWMS_LINK_MAX_SUB_WIN][DISP_HWMS_LINK_MAX_QOBJ_PER_WIN];
    /**< A mosiac framelist is made up of finite set of smaller frames. We have
         seperate queue for each of these sub-frames. This variable is the
         memory required to implement the queue. */
        VpsUtils_QueHandle qHandles[DISP_HWMS_LINK_MAX_SUB_WIN];
    /**< Queue handles for each of the sub-window queue. */

        /* 
         *  Counters/Flags for statistics/debugging.
         */
        UInt32 queueCount;
    /**< Tracks the number of requests queued with display driver. */
        UInt32 dequeueCount;
    /**< Tracks the number of requests dequeued from display driver. */
        UInt32 repeatCount[DISP_HWMS_LINK_MAX_SUB_WIN];
    /**< Tracks the number of frame repeat per window. */

        UInt32 cbCount;

        UInt32 startTime;
        UInt32 prevTime;
        UInt32 minCbTime;
        UInt32 maxCbTime;
        UInt32 lastCbTime;

    } DispHwMsLink_Obj;

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

    Int32 DispHwMsLink_drvCreate(DispHwMsLink_Obj * pObj,
                                 DispHwMsLink_CreateParams * pPrm);
    Int32 DispHwMsLink_drvStart(DispHwMsLink_Obj * pObj);

    Int32 DispHwMsLink_drvProcessData(DispHwMsLink_Obj * pObj);
    Int32 DispHwMsLink_drvRcvFrames(DispHwMsLink_Obj * pObj);

    Int32 DispHwMsLink_drvStop(DispHwMsLink_Obj * pObj);
    Int32 DispHwMsLink_drvDelete(DispHwMsLink_Obj * pObj);

    Int32 DispHwMsLink_drvLock(DispHwMsLink_Obj * pObj);
    Int32 DispHwMsLink_drvUnlock(DispHwMsLink_Obj * pObj);

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _DISP_HWMS_LINK_PRIV_H_ 
   *//* @} */
