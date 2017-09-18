/** ==================================================================
 *  @file   issdrv_ispPriv.h
 *
 *  @path   /ti/psp/iss/drivers/capture/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_ISP_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_ISP_PRIV_H_

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <stdlib.h>
#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/iss/iss.h>
#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/drivers/capture/issdrv_capture.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/core/inc/iss_comp.h>
#include <ti/psp/iss/hal/iss/bte/bte.h>

/* constant's */

#define ISP_floor(value,align)   (((value)/(align)) * (align))

#define ISS_ISP_INST_HANDLES	(4)

/* ISP driver states */

/*
 * State transitions are explained below
 *
 * IDLE - initial or not open state | CREATED - state after calling
 * FVID2_create | DO_START - temporary state when FVID2_start() is called |
 * this means driver has requested CLM to start the instance | RUNNING - this
 * is the state after FVID2_start() completes, this means | CLM started
 * capture and now it is running, it remains in this state | until
 * FVID2_stop() is called | DO_STOP - temporary state when FVID2_stop is
 * called() | this means driver has request CLM to stop the instance |
 * STOPPED - this is the state after FVID2_stop() completes, this means | CLM
 * has stopped this instance | IDLE - this is the state after FVID2_delete()
 * completes */
#define ISS_ISP_STATE_IDLE          (0)                        /* Driver is not
                                                            * open and is
                                                            * idle */
#define ISS_ISP_STATE_CREATED       (1)                        /* Driver is
                                                            * created */
#define ISS_ISP_STATE_DO_START      (2)                        /* Driver is
                                                            * requesting a
                                                            * start */
#define ISS_ISP_STATE_RUNNING       (3)                        /* Driver is
                                                            * running */
#define ISS_ISP_STATE_DO_STOP       (4)                        /* Driver is
                                                            * requesting a
                                                            * stop */
#define ISS_ISP_STATE_STOPPED       (5)                        /* Driver is
                                                            * stopped */

#define ISS_ISP_MAX_ELEMENTS_QUEUE  (8)

#define ISS_ISP_PROCESS_TASK_PRI    (12)

#define ISS_ISP_PROCESS_TASK_STACK  (10 * 1024)

typedef struct
{
    /**
     * sem for yuv420 to yuv420 resizing
     */
    Semaphore_Handle sem420;

    /**
     * Number of streams in this instance
     */
    UInt16 numStream;

    /**
     * Instance ID
     */
    UInt16 instanceId;

    /**
     * instance state
     */
    UInt16 state;

    /**
     * create arguments
     */
    Iss_IspCreateParams createArgs;

    /**
     * user callback parameters
     */
    FVID2_DrvCbParams cbPrm;

    /**
     * user callback flag
     */
    UInt32 usrCbFlag;

    FVID2_DataFormat inDataFmt;
    FVID2_DataFormat outDataFmt0;
    FVID2_DataFormat outDataFmt1;
    UInt32 inWidth;
    UInt32 inHeight;
    UInt32 inPitch;
    UInt32 outWidth0;
    UInt32 outHeight0;
    UInt32 outWidth1;
    UInt32 outHeight1;
    UInt32 pitch[2][3];

    VpsUtils_QueHandle processQueue;
    FVID2_Frame        processQueueMem[ISS_ISP_MAX_ELEMENTS_QUEUE];

    VpsUtils_QueHandle outQueue;
    FVID2_Frame        outQueueMem[ISS_ISP_MAX_ELEMENTS_QUEUE];

    FVID2_Frame        *pOutFrame[2];
    FVID2_Frame        *inFrame;

    UInt32 prevBscBufAddr;
    UInt32 curBscBufAddr;
    Bool vsEnable;
    Bool runVs;
    UInt32 bscInWidth;
    UInt32 bscInHeight;
    UInt32 mirrorMode;
    UInt32 outStartX;
    UInt32 outStartY;

    UInt32 flipOffset[2][2];
    UInt32 isRszOvfl;

    FVID2_ProcessList procList;
}Iss_IspObj;

typedef struct
{
    /**
     * lock handle
     */
    Semaphore_Handle lock;
    Semaphore_Handle bscSync;
    Semaphore_Handle process;

    /**
     * FVID2 driver ops
     */
    FVID2_DrvOps fvidDrvOps;

    /**
     * ISP instance objects
     */
    Iss_IspObj ispObj[ISS_ISP_INST_HANDLES];
    UInt32     isAllocated[ISS_ISP_INST_HANDLES];

    UInt32 numObjUsed;

    Iss_IspObj *curActIspObj;

    Task_Handle processTask;
    volatile UInt32      exitTask;

    UInt32 numRszOvfl;

    bteHandle handleBTE;

    UInt32 isBteInitDone;
}Iss_IspCommonObj;

Int32 Iss_ispDrvCallBack();

Void Issdrv_ispDrvRszOvfl();

Void IssDrv_ispH3aCallback();

Int32 Iss_ispDelete(Fdrv_Handle handle, Ptr reserved);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_ISP_PRIV_H_
                                                            */
