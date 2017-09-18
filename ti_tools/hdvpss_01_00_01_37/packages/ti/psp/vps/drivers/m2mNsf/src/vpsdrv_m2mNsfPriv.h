/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPS_NSF_DRVIVER_PRIV_H_
#define _VPS_NSF_DRVIVER_PRIV_H_


#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_nf.h>
#include <ti/psp/vps/drivers/m2mNsf/vpsdrv_m2mNsf.h>
#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>
#include <ti/psp/platforms/vps_platform.h>

/* constant's */

//#define VPS_NSF_ENABLE_WR_DESC

/* Frame noise resetting state - frame noise reset applied */
#define VPS_NSF_RESET_FRAME_NOISE_IDLE          (0)

/* Frame noise resetting state - do frame noise reset*/
#define VPS_NSF_RESET_FRAME_NOISE_RESET         (1)

/* Frame noise resetting state - frame noise reset done, set HW
to original CH values */
#define VPS_NSF_RESET_FRAME_NOISE_RESET_DONE    (2)

/* Width and height alignement needed for NSF from VPDMA point
of view  */
#define VPS_NSF_FRAME_SIZE_ALIGN                (32)

/* Max frame noise index  */
#define VPS_NSF_MAX_FRAME_NOISE_IDX   (VPSHAL_NF_MAX_VIDEO_SRC_IDX)

/* Max channelNum */
#define VPS_NSF_MAX_CHANNEL_NUM      (0xFF)

/* Max descriptor set per channel */
#define VPS_NSF_MAX_DESC_SET_PER_CH  ( 4)

/* Max queued request list's  */
#define VPS_NSF_MAX_REQ_LIST         ( 4)

/* Max requests in a request list */
#define VPS_NSF_MAX_REQ_PER_LIST     (FVID2_MAX_FVID_FRAME_PTR)

#ifdef VPS_CFG_USE_SYNC_ON_CH

/* Number of SOCH descriptor taken in descriptor set */
#define VPS_NSF_NUM_SOCH_DESC         (4)
#define VPS_NSF_NUM_SOC_DESC          (0)

#else

#define VPS_NSF_NUM_SOCH_DESC         (0)
#define VPS_NSF_NUM_SOC_DESC          (1)

#endif

/* Config descriptor data offset  */
#define VPS_NSF_CONFIG_DESC_OFFSET   (VPSHAL_VPDMA_CONFIG_DESC_SIZE*1 \
                + VPSHAL_VPDMA_DATA_DESC_SIZE*5 \
                + VPSHAL_VPDMA_CTRL_DESC_SIZE \
                  *(1+VPS_NSF_NUM_SOCH_DESC+VPS_NSF_NUM_SOC_DESC) \
                  )

#define VPS_NSF_WR_DESC_OFFSET      (VPS_NSF_CONFIG_DESC_OFFSET+VPS_NSF_CONFIG_PAYLOAD_SIZE)

#ifdef VPS_NSF_ENABLE_WR_DESC
#define VPS_NSF_WR_DESC_SIZE        (VPSHAL_VPDMA_DATA_DESC_SIZE*2)
#else
#define VPS_NSF_WR_DESC_SIZE        (0)
#endif

/* NSF object state - unused  */
#define VPS_NSF_OBJ_STATE_UNUSED     (0)

/* NSF object state - used and idle  */
#define VPS_NSF_OBJ_STATE_IDLE       (1)

/* Config data payload size */
#define VPS_NSF_CONFIG_PAYLOAD_SIZE  \
          VpsUtils_align((sizeof(VpsHal_NfConfigOverlay)),16)

/* Descriptor set size  */
#define VPS_NSF_MAX_DESC_SET_SIZE    (VPS_NSF_WR_DESC_OFFSET \
                                    + VPS_NSF_WR_DESC_SIZE   \
                                    )

/* In frame list Idx  */
#define VPS_NSF_IN_FRAME_LIST_IDX         (0)

/* Previous out frame list idx  */
#define VPS_NSF_PREV_OUT_FRAME_LIST_IDX   (1)

/* Output frame list idx  */
#define VPS_NSF_OUT_FRAME_LIST_IDX        (0)

/* Number of input frame list's */
#define VPS_NSF_IN_FRAME_LIST_NUM         (2)

/* Number of output frame list's */
#define VPS_NSF_OUT_FRAME_LIST_NUM        (1)

//#define VPS_NSF_DEBUG

/* forward definition of NSF object */
typedef struct Vps_NsfObj_t Vps_NsfObj;


typedef struct
{

    /** subframe configuration : subframe mdoe enable flag and subframe size in No of Lines*/
    UInt32          subFrameMdEnable;
    UInt16          slcSz;

    /** subframe related information  */
    UInt16          subFrameNum;
    UInt32          numOfSlcs;

}VpsMdrv_NsfSubFrameChInst;

/* Channel information - specific to each channel in a handle */
typedef struct
{

    /*
     * Address of descriptor set's
     */
    Ptr descSetMem[VPS_NSF_MAX_DESC_SET_PER_CH];

    /*
     * Free descriptor set queue
     */
    VpsUtils_QueHandle descSetFreeQue;

    /*
     * Free descriptor set queue - memory
     */
    Ptr descSetFreeQueMem[VPS_NSF_MAX_DESC_SET_PER_CH];

    /*
     * Number of times to update descriptor set
     */
    UInt32 updateDescSet;

    /*
     * NSF HW config
     */
    VpsHal_NfConfig nfHwConfig;

    /*
     * TRUE: NSF channel is created, else not created
     */
    UInt32 isCreate;

    /*
     * NSF HW frame noise Idx
     */
    Int16 hwFrameNoiseIdx;

    /*
     * VPS_NSF_RESET_FRAME_NOISE_XXXX
     */
    UInt16 frameNoiseCalcReset;

    /*
     * Original 'initFrameNoise' value
     */
    UInt32 initFrameNoiseOrg;

    /*
     * Original 'refFrameConfig' value
     */
    Vps_NsfRefFrameConfig refFrameConfigOrg;

    /* received desc info */
    UInt32 recvDescWidthY;
    UInt32 recvDescWidthC;

    VpsMdrv_NsfSubFrameChInst subFrameChPrms;

} Vps_NsfChObj;

/* Request object */
typedef struct
{

    /*
     * channel ID
     */
    UInt32 chId;

    /*
     * Descruiptor set for this request
     */
    Ptr descSetMem;

} Vps_NsfReq;

/* Request list */
typedef struct
{

    /*
     * Array of request's that make up this request list
     */
    Vps_NsfReq reqObj[VPS_NSF_MAX_REQ_PER_LIST];

    /*
     * Number of request's
     */
    UInt32 numReq;

    /*
     * Pointer to parent NSF Object
     */
    Vps_NsfObj *pNsfObj;

    /*
     * MLM request info
     */
    Mlm_SubmitReqInfo mlmReq;

    /*
     * Process List associated with this request list
     */
    FVID2_ProcessList processList;

} Vps_NsfReqList;

/* NSF Object - specific to each driver handle
*/
struct Vps_NsfObj_t
{

    /*
     * Object state
     */
    UInt32 state;

    /*
     * handle ID
     */
    UInt32 handleId;

    /*
     * Mutex semaphore, used in some IOCTLs to get exculsive
     * access to data structures
     */
    Semaphore_Handle lock;

    /*
     * Create arguments
     */
    Vps_NsfCreateParams createArgs;

    /*
     * data format per CH
     */
    Vps_NsfDataFormat dataFormat[VPS_NSF_MAX_CH_PER_HANDLE];

    /*
     * NF processing params per CH
     */
    Vps_NsfProcessingCfg processingCfg[VPS_NSF_MAX_CH_PER_HANDLE];

    /*
     * Create status
     */
    Vps_NsfCreateStatus createStatus;

    /*
     * Callback params
     */
    FVID2_DrvCbParams cbParams;

    /*
     * Descriptor Set size for a channel
     */
    UInt32 descSetSize;

    /*
     * Channel Objects
     */
    Vps_NsfChObj chObj[VPS_NSF_MAX_CH_PER_HANDLE];

    /*
     * Time out for reqList Free blocking
     */
    UInt32 reqListFreeTimeout;

    /*
     * Time out for channel descriptor Free blocking
     */
    UInt32 chDescSetFreeTimeout;

    /*
     * Request list's
     */
    Vps_NsfReqList reqList[VPS_NSF_MAX_REQ_LIST];

    /*
     * Free request list queue
     */
    VpsUtils_QueHandle reqListFreeQue;

    /*
     * Free request list queue - memory
     */
    Vps_NsfReqList *reqListFreeQueMem[VPS_NSF_MAX_REQ_LIST];

    /*
     * Ready/completed request list queue
     */
    VpsUtils_QueHandle reqListReadyQue;

    /*
     * Ready/completed request list queue - memory
     */
    Vps_NsfReqList *reqListReadyQueMem[VPS_NSF_MAX_REQ_LIST];

    /*
     * TRUE: reqList queue's are created, else FALSE
     */
    UInt32 reqIsCreate;

    UInt32 blockOnReqQueFull;
    /*
     * TRUE: Block FVID2_processFrames() until current overall request
     * can be submitted, FALSE: return error if current request
     * cannot be submitted
     */

    UInt32 blockOnChQueFull;
    /*
     * TRUE: Block FVID2_processFrames() until current request
     * for a channel can be submitted, FALSE: return error
     * if current request for a channel cannot be submitted
     */

    /* number of pending submissions */
    UInt32 numPendSubmission;
};

/*
  NSF information common across all handles
*/
typedef struct
{

    /*
     * NSF lock across all handles
     */
    Semaphore_Handle lock;

    /*
     * MLM handle
     */
    Mlm_Handle mlmHandle;

    /*
     * FVID2 driver function operations
     */
    FVID2_DrvOps fvidDrvOps;

    /*
     * NSF HW HAL handle
     */
    VpsHal_Handle nsfHalHandle;

    /*
     * NSF Objects
     */
    Vps_NsfObj nsfObj[VPS_NSF_MAX_HANDLES];

    /*
     * HW frame noise Idx alloc status
     */
    UInt16 hwFrameNoiseIdxAllocStatus[VPS_NSF_MAX_FRAME_NOISE_IDX];

    /*
     * Open count of the driver
     */
     UInt32 openCnt;

} Vps_NsfCommonObj;

/* gloabl's */

extern UInt8 gVps_nsfDescSetMem[];
extern Vps_NsfCommonObj gVps_nsfCommonObj;

/* function's */

Vps_NsfObj *Vps_nsfAllocObj (  );


Int32 Vps_nsfFreeObj ( Vps_NsfObj * pObj );
Int32 Vps_nsfLock (  );
Int32 Vps_nsfUnlock (  );

Int16 Vps_nsfHwFrameIdxAlloc ( UInt32 autoMode );
Void Vps_nsfHwFrameIdxFree ( Int16 hwFrameIdx );

Int32 Vps_nsfChannelCreate ( Vps_NsfObj * pObj, UInt32 curCh );
Int32 Vps_nsfChannelDelete ( Vps_NsfObj * pObj, UInt32 curCh );
Int32 Vps_nsfChannelSetFormat ( Vps_NsfObj * pObj, Vps_NsfDataFormat * format );

Int32 Vps_nsfChannelSetProcessingCfg ( Vps_NsfObj * pObj,
                                       Vps_NsfProcessingCfg * params );

Int32 Vps_nsfChannelGetAdvanceCfg( Vps_NsfObj * pObj,
                Vps_NsfAdvConfig * params);

void vpsMdrvNsfSubFrameUpdtBufAddr ( VpsMdrv_NsfSubFrameChInst *slcNsfdata,
                                     FVID2_Frame *inFrame,
                                     FVID2_Frame *prevOutFrame,
                                     FVID2_Frame *outFrame,
                                     Vps_NsfDataFormat    *dataformat);

Int32 Vps_nsfChannelMakeDescSet ( Vps_NsfObj * pObj,
                                  Vps_NsfReq * reqObj,
                                  FVID2_ProcessList * processList,
                                  UInt32 reqNum );

Int32 Vps_nsfReqCreate ( Vps_NsfObj * pObj );
Int32 Vps_nsfReqDelete ( Vps_NsfObj * pObj );

Ptr Vps_nsfChDescSetAlloc (void);
Void Vps_nsfChDescSetFree ( Ptr memPtr );

Vps_NsfReqList *Vps_nsfReqGetFreeList ( Vps_NsfObj * pObj,
                                        FVID2_ProcessList * processList );

Int32 Vps_nsfReqSubmit ( Vps_NsfObj * pObj, Vps_NsfReqList * reqList );
Int32 Vps_nsfReqComplete ( Mlm_SubmitReqInfo * mlmReq );

Int32 Vps_nsfReqGetProcessedFrames ( Vps_NsfObj * pObj,
                                     FVID2_ProcessList * processList,
                                     UInt32 timeout );

Int32 Vps_nsfReqAbort ( Vps_NsfObj * pObj,
                        Vps_NsfReqList * reqList, UInt16 validReqNum );

/*
  Lock NSF object
*/
static inline Int32 Vps_nsfObjLock ( Vps_NsfObj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Unlock NSF object
*/
static inline Int32 Vps_nsfObjUnlock ( Vps_NsfObj * pObj )
{
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/*
  Given descriptor set base address return config overlay address
*/
static inline Ptr Vps_nsfChannelGetNfConfigOverlayAddr ( UInt8 * descSetMem )
{
    return descSetMem + VPS_NSF_CONFIG_DESC_OFFSET;
}

/*
  Give a request object retrun Reload descriptor address for that request
*/
static inline Ptr Vps_nsfChannelGetDescSetReloadAddr ( Vps_NsfReq * reqObj )
{
    return ( Ptr ) ( ( UInt32 )
                     Vps_nsfChannelGetNfConfigOverlayAddr ( reqObj->
                                                            descSetMem ) -
                     VPSHAL_VPDMA_CTRL_DESC_SIZE );
}

/*
  Retrun descripter set to free queue
*/
static inline Int32 Vps_nsfChannelFreeDescSet ( Vps_NsfObj * pObj,
                                                Vps_NsfReq * reqObj )
{
    Vps_NsfChObj *chObj;
    Int32 status;

    GT_assert( GT_DEFAULT_MASK,  reqObj->chId < pObj->createArgs.numCh );

    chObj = &pObj->chObj[reqObj->chId];

    status =
        VpsUtils_quePut ( &chObj->descSetFreeQue, reqObj->descSetMem,
                          BIOS_NO_WAIT );

    #ifdef VPS_NSF_ENABLE_WR_DESC
    {
        VpsHal_VpdmaInDataDescOverlay *pWrDesc;

        pWrDesc = (VpsHal_VpdmaInDataDescOverlay*)((UInt8*)reqObj->descSetMem + VPS_NSF_WR_DESC_OFFSET);

        chObj->recvDescWidthY  = (pWrDesc->frameWidthHeight & 0xFFFF0000)>>16;

        pWrDesc++;

        chObj->recvDescWidthC  = (pWrDesc->frameWidthHeight & 0xFFFF0000)>>16;

        if(chObj->recvDescWidthY==64||chObj->recvDescWidthC==32)
        {
            Vps_printf(" %d: NSF 64x32 condition occured !!!\r\n", Clock_getTicks());
            GT_assert( GT_DEFAULT_MASK, 0);
        }

    }
    #endif

    GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

    return status;
}

/*
  Link current request with previous request
*/
static inline Int32 Vps_nsfChannelLinkDescSet ( Vps_NsfReq * prevReq,
                                                Vps_NsfReq * curReq )
{
    VpsHal_VpdmaReloadDesc *prevRld;

    prevRld = ( VpsHal_VpdmaReloadDesc * )
        Vps_nsfChannelGetDescSetReloadAddr ( prevReq );

    if ( NULL != curReq )
    {
        prevRld->reloadAddr = VpsHal_vpdmaVirtToPhy( ( Ptr ) curReq->descSetMem);
        prevRld->listSize =
            ( VPS_NSF_CONFIG_DESC_OFFSET ) >> VPSHAL_VPDMA_LIST_SIZE_SHIFT;
    }
    else
    {
        prevRld->reloadAddr = 0;
        prevRld->listSize = 0;
    }

    return 0;
}

#endif /*  _VPS_NSF_DRVIVER_PRIV_H_  */
