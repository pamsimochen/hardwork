/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPS_CAPT_LM_PRIV_H_
#define _VPS_CAPT_LM_PRIV_H_


#include <string.h>

#include <xdc/runtime/System.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/timers/dmtimer/Timer.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/psp/vps/drivers/capture/vpsdrv_captureLm.h>
#include <ti/psp/vps/drivers/capture/vpsdrv_capture.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_vip.h>
#include <ti/psp/vps/hal/vpshal_sc.h>

/* constants  */

#ifdef VPS_CAPT_LIST_ENABLE_ASSERT_ON_ERROR
#define VPS_CAPT_LM_MAX_LIST                        (4)
#else
#define VPS_CAPT_LM_MAX_LIST                        (1)
#endif


#define VPS_CAPT_LM_VCH_STATE_NORMAL                (0)
#define VPS_CAPT_LM_VCH_STATE_REPEAT_ADDR           (1)

/* Size of list maximum
  Atmost four descriptor can be programmed worst case per channel
*/
#define VPS_CAPT_LM_LIST_SIZE_MAX                   (512*VPSHAL_VPDMA_DATA_DESC_SIZE)

/* Information data structure array log size  */
#define VPS_CAPT_LOG_INFO_MAX                       (500)

/* list post to list free timeout in msecs */
#define VPS_CAPT_LM_LIST_POST_TIMEOUT               (3 * 1000)

/* Wait time before printing the advanced statistics once the error
 * desc is received
 */
#define VPS_CAPT_WAIT_BEFORE_PRINT_ADV_STATS        (200u)


#define VPS_CAPT_LOOP_CNT_NUM_LOG                   (1024)



/* Enable this to use GP Timer instead of BIOS clock tick */
//#define VPS_CFG_CAPT_USE_GP_TIMER

/* GP Timer ID to use (used when VPS_CFG_CAPT_USE_GP_TIMER is enabled).
 * Currently using GP Timer 5. Below is the mapping as per BIOS user guide
 * 0 -> GPTimer 4, 1 -> GPTimer 5, 2 -> GPTimer 6, 3 -> GPTimer 7
 * And GP Timers 0 to 3 are reserved and/or not supported by BIOS.
 * Note: Ensure that the selected GP Timer clock is ON using PRCM module. */
#define VPS_CFG_CAPT_GP_TIMER_ID                    (1u)

/* data structure's */

/*
  List Object
*/
typedef struct
{

    UInt32 listSize;            /* list size in bytes */
    UInt8 *listAddr;            /* list addr  */

} Vps_CaptLmListObj;

/*
  HW Que Object
*/
typedef struct
{
    UInt8 *hwQueStartAddr;
    UInt8 *hwQueEndAddr;

    UInt8 *hwQueCurAddr;
    UInt8 *hwQuePrevAddr;

} Vps_CaptLmHwQueObj;


typedef struct
{
    /*
     * VPDMA channel to logical driver channel map
     */
    UInt16 vChannel2lChannelMap;

    /*
     * VPDMA channel to VPDMA channel index map  (Y/C plane channel)
     */
    UInt16 vChannel2vChannelIdxMap;

    /* number of desc's recevied in current sampling */
    UInt16 curDescRecv;

    UInt8  *lastBufAddr;
    UInt16 state;

    UInt32 prevDescId;
    UInt32 curDescId;
    UInt32 programExtraDesc;
    UInt32 numProgramExtraDesc;

} Vps_CaptLmVpdmaChannelInfo;

typedef struct
{
    UInt32 timestamp;
    UInt32 numRecvDescs;
    UInt32 numValidDescs;
    UInt8 *prevAddr;
    UInt8 *curAddr;

} Vps_CaptLmHwQueInfoLog;

/*
  Capture list manager object
*/
typedef struct
{
    Int32 state;                /* state - VPS_CAPT_LM_STATE_xxx  */
    UInt8  hwListNum[VPS_CAPT_LM_MAX_LIST];           /* HW list ID */
    UInt16 hwListId;

    Semaphore_Handle lock;      /* CLM lock */

    Vps_CaptLmHwQueObj hwQueObj; /* HW Que info */


    Vps_CaptLmVpdmaChannelInfo vChannelInfo[VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS];

    /*
     * CLM completion driver callback
     */
    Vps_CaptLmCompleteCb completeCb;

    /*
     * Timer handle - used to periodically wake up driver task
     */
#ifdef VPS_CFG_CAPT_USE_GP_TIMER
    Timer_Handle timerHndl;
#else
    Clock_Handle clockHndl;
#endif

    UInt32 maxListPostTime  ;
    UInt32 minListPostTime  ;
    UInt32 avgListPostTime  ;
    UInt32 totalListPostTime;
    UInt32 listPostCount;
    UInt32 listStallCount;
    UInt32 listStallTimestamp[VPS_CAPT_LOG_INFO_MAX];

    Vps_CaptLmDescInfo errorDescInfo[VPS_CAPT_LOG_INFO_MAX];
    Vps_CaptLmHwQueInfoLog  hwQueInfo[VPS_CAPT_LOG_INFO_MAX];
    UInt32 hwQueInfoCount;

    Vps_CaptLmDescInfo recvDescInfo[VPS_CAPT_LOG_INFO_MAX];
    UInt32 recvDescCount;
    UInt32 errorDescCount;
    UInt32 dropFrameCount;
    UInt32 extraDescProgrammedCount;

    UInt32 errorDescWrCount;
    UInt32 intcTimeoutCount;
    UInt32 intcTimeoutCount2;
    UInt32 pidSpecialConditionCount;
    UInt32 zeroPidCnt;
    UInt32 loopCntLog[VPS_CAPT_LOOP_CNT_NUM_LOG];
    UInt32 minLoopCnt;
    UInt32 loopNum;
    UInt32 loop2CntLog[VPS_CAPT_LOOP_CNT_NUM_LOG];
    UInt32 minLoop2Cnt;
    UInt32 loop2Num;
    UInt32 descPidMask;
} Vps_CaptLmObj;

Int32 Vps_captLmHwQueReset();

Int32 Vps_captLmLogErrorDesc(volatile VpsHal_VpdmaInDataDesc *pDataDesc);

Int32 Vps_captLmLogRecvDesc(volatile VpsHal_VpdmaInDataDesc *pDataDesc);

Int32 Vps_captLmLogHwQueInfo(Vps_CaptLmHwQueObj *hwQue, UInt16 numRecvDescs, UInt16 numValidDescs);

Int32 Vps_captLmWriteHwQueToFile();

Int32 Vps_captLmPostAllAbortList(VpsHal_VipInst vipInstId);

Void Vps_captLmCheckDescId(volatile VpsHal_VpdmaInDataDesc *pSrcDesc);

Int32 Vps_captPostAllAbortList(VpsHal_VipInst vipInstId);

Int32 Vps_captLmAddDummyDisSyncDesc(Ptr memPtr, VpsHal_VpdmaChannel vChannel);
#endif
