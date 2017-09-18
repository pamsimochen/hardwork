/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_vpdma.c
 *
 *  \brief VPS VPDMA HAL Source file.
 *  This file implements the HAL APIs of the VPS VPDMA.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

#include <stdio.h>
#include <string.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vpdmaMosaic.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/cslr/cslr_hd_vps_vpdma.h>
#include <ti/psp/vps/hal/src/vpshalVpdma.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/platforms/vps_platform.h>

#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti814x_v286.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti814x_v288.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti816x_v195.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti816x_v1A3.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti81xx_v1AD.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti816x_v1AE.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti816x_v1B2.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti816x_v1B0.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti81xx_v1B5.h>
#include <ti/psp/vps/hal/src/vpshalVpdmaFirmware_ti81xx_v1B7.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VPSHAL_VPDMA_BUSY_TIMEOUT                      (500000u)

/**
 *  Macro to check whether VPDMA is busy reading the list and whether new list
 *  can be submitted to the VPDMA
 */
#define VPSHAL_VPDMA_ISBUSY              ((VpdmaBaseAddress->LIST_ATTR &       \
                               CSL_HD_VPS_VPDMA_LIST_ATTR_RDY_MASK) >>   \
                               CSL_HD_VPS_VPDMA_LIST_ATTR_RDY_SHIFT)


/* Macro to check if VPDMA firmware is loaded or not  */
#define VPSHAL_VPDMA_FW_IS_LOADED   (VpdmaBaseAddress->PID & 0x80)

/**
 * Macro to check whether given list is busy or not
 */
#define VPSHAL_VPDMA_ISLISTBUSY(listNum) ((VpdmaBaseAddress->LIST_STAT_SYNC >> \
                                     (VPSHAL_VPDMA_LIST_ATTR_LISTBUSYOFFSET +  \
                                     (listNum))) &  0x1)

#define VPSHAL_VPDMA_PAYLOAD_SIZE_ALIGN                  (16u)
#define VPSHAL_CONFIG_PAYLOAD_ADDR_ALIGN                 (16u)

#define VPSHAL_VPDMA_CONFIG_DATASIZE_MASK                (0x0000FFFFu)
#define VPSHAL_VPDMA_OUTBOUND_DESC_WRITE_ADDR_SHIFT      (0x5u)

#define VPSHAL_VPDMA_VPI_SIZE_ALIGN                      (0x4u)

#define VPSHAL_VPDMA_ADDR_SET_ALIGN                      (4u)
#define VPSHAL_VPDMA_REG_OFFSET_DIFF                     (4u)
#define VPSHAL_VPDMA_ADDR_SET_SIZE                       (4u)
#define VPSHAL_VPDMA_WORD_SIZE                           (4u)
#define VPSHAL_VPDMA_MIN_REG_SET_SIZE                    (4u)

#define VPSHAL_VPDMA_MAX_DATA_TYPE                       (4u)
#define VPSHAL_VPDMA_CONFIG_PAYLOADSIZE_SHIFT            (4u)

#define VPSHAL_VPDMA_GRPX_REGION_ATTR_MASK               (0x11Fu)

#define VPSHAL_VPDMA_MAX_TRACE_COUNT                     (100u)

/* Maximum number of register in a block in MMR configured */
#define VPSHAL_VPDMA_CONFIG_MMR_MAX_BLOCK_REG            (28u)

/* VPDMA firmware size in bytes. */
#define VPDMA_FIRWARE_SIZE          (8u * 1024u)

/* VPDMA number Control of control descriptor to be used for
   cleaning out channels */
#define VPSHAL_VPDMA_MAX_CTRL_DESC   (VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS)

#define VPSHAL_VPDMA_BUSY_WAIT       (5000u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vpdma_ChannelInfo
 *  \brief This structure keeps track of the channel information like channel
 *  number, direction of the channel, data type supported on the channel etc.
 *  When client of the VPDMA asks to create data descriptor in the memory, VPDMA
 *  HAL will fill up the memory with these default information
 *
 */
typedef struct
{
    VpsHal_VpdmaChannel         channelNum;
    /**< VPDMA Channel Number */
    UInt32                      clientCtrlReg;
    /**< Register into which frame start event for channel can be set */
    VpsHal_VpdmaMemoryType      memType;
    /**< Type of the memory from which data can be taken for this channel
         ---- Can we assume all 422p paths will uPossible value for this fields
         are 0 for 1D and 1 for 2D memory se non-tiled memory ---- */
    UInt8                       assigned;
    /**< Flag to indicate whether channel is free or not. This flag will
         be used for free channel only */
} Vpdma_ChannelInfo;

/**
 *  struct Vpdma_PathInfo
 *  \brief This structure keeps track of the channel information like channel
 *  number, direction of the channel, data type supported on the channel etc.
 *  When client of the VPDMA asks to create data descriptor in the memory, VPDMA
 *  HAL will fill up the memory with these default information
 *
 */
typedef struct
{
    VpsHal_VpdmaPath            vpdmaPath;
    /**< VPDMA Path */
    UInt32                      isTiledDataSupported;
    /**< Flag to indicate whether tiled data can be supported on this channel */
    UInt32                      tiledLineSize;
    /**< Line size in terms of bytes for tiled data */
    UInt32                      lineSize;
    /**< Line size in terms of bytes for non-tiled data or pixels for grpx data*/
    UInt32                      enhancedLineSize;
    /**< Line size in terms of bytes for non-tiled data or pixels for grpx data
         Valid only for PG's version greater than 2.0 (Both Ce and Ne)*/
} Vpdma_PathInfo;

/**
 *  Object to store the trace information about each list post for a particular
 *  list number.
 */
typedef struct
{
    UInt32                  totalPosts;
    /**< Total number of list post. */
    UInt32                  curIdx;
    /**< Current index - Points to the next element to the last update in the
         below arrays. This will be used like a circular array index. */
    Ptr                     listAddr[VPSHAL_VPDMA_MAX_TRACE_COUNT];
    /**< List address. */
    UInt32                  listSize[VPSHAL_VPDMA_MAX_TRACE_COUNT];
    /**< List size. */
    Int32                   retVal[VPSHAL_VPDMA_MAX_TRACE_COUNT];
    /**< Return value of the post function. */
    VpsHal_VpdmaListType    listType[VPSHAL_VPDMA_MAX_TRACE_COUNT];
    /**< Type of list submitted. */
    UInt32                  timeStamp[VPSHAL_VPDMA_MAX_TRACE_COUNT];
    /**< Timestamp when the list post happens. */
    UInt32                  listStatus[VPSHAL_VPDMA_MAX_TRACE_COUNT];
    /**< List Status */
    UInt32                  listWaitCnt;
    /**< Total number of times list has to wait before posting */
    UInt32                  maxListWait;
} Vpdma_ListPostTrace;

/**
 *  Object to store the trace information of VPDMA HAL.
 */
typedef struct
{
    Vpdma_ListPostTrace     listPost[VPSHAL_VPDMA_MAX_LIST];
} Vpdma_TraceObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vpdmaCalcBlockInfo(UInt32 * const *regOffset,
                               UInt32 numReg,
                               UInt32 *blockSize,
                               UInt32 *numBlockReg);
static UInt16 *VpsHal_vpdmaGetFirmwareAddr(VpsHal_VpdmaVersion ver);
char *VpsHal_vpdmaGetChStr(UInt32 channel);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static Vpdma_ChannelInfo Channels[VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS] =
                                        {VPSHAL_VPDMA_DEFAULT_CHANNEL_INFO};

static Vpdma_PathInfo VpdmaPathInfo[VPSHAL_VPDMA_MAX_PATH] =
                                        VPSHAL_VPDMA_DEFAULT_PATH_INFO;

CSL_VpsVpdmaRegsOvly VpdmaBaseAddress;

/**
 *  \brief Storing VPS Base address locally within VPDMA. This address will be
 *  subtracted from all the registers programmed in the overlay memory. This is
 *  done because overlay requires memory to starting from the offset/base
 *  address zero.
 */
UInt32 gVpsHal_vpsBaseAddress;

/* Variable to store the VPDMA trace information. */
Vpdma_TraceObj gVpsHalVpdmaTraceObj;

#ifdef VPS_CFG_ENABLE_CUSTOM_FW_LOAD
/* Buffer to store the user loadable firmware */
#pragma DATA_ALIGN(gVpsVpdmaHalCustomFw, 32)
static UInt16 gVpsVpdmaHalCustomFw[(VPDMA_FIRWARE_SIZE / sizeof(UInt16))];
#endif


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_vpdmaInit
 *  \brief Function for initializing VPDMA HAL
 *  \param arg           For the Future use, not used currently.
 *
 *  \param initParams    Instance Specific parameters. Contains register base
 *                       address for VPDMA
 *  \param ver           Version of VPDMA that should be used.
 *  \return              Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaInit(VpsHal_VpdmaInstParams *initParams,
                     VpsHal_VpdmaVersion ver,
                     Ptr arg)
{
    Int32                   retVal;
    UInt32                  listCnt;
#ifndef PLATFORM_ZEBU
    UInt32                  chCnt;
#endif
    UInt32                  regValue;
    UInt16                 *vpdmaFirmware;
    CSL_VpsVpdmaRegsOvly    regOvly;
    void                   *descMem = NULL;
    UInt8                  *descPtr;

    GT_assert(VpsHalTrace, (NULL != initParams));

    VpdmaBaseAddress = (CSL_VpsVpdmaRegsOvly) initParams->vpdmaBaseAddress;
    gVpsHal_vpsBaseAddress = initParams->vpsBaseAddress;

    VpsUtils_memset(&gVpsHalVpdmaTraceObj, 0u, sizeof(gVpsHalVpdmaTraceObj));

    /* load VPDMA firmware */
    vpdmaFirmware = VpsHal_vpdmaGetFirmwareAddr(ver);
    retVal = VpsHal_vpdmaLoadStateMachine(vpdmaFirmware, 0);

    /* Allocate descriptor memory */
    if (FVID2_SOK == retVal)
    {
        descMem = VpsUtils_allocDescMem(
                      VPSHAL_VPDMA_CTRL_DESC_SIZE * VPSHAL_VPDMA_MAX_CTRL_DESC,
                      VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == descMem)
        {
            GT_0trace(VpsHalTrace, GT_ERR, "Desc memory alloc failed!!\n");
            retVal = VPS_EALLOC;
        }
    }

    /* Initialize utility to compute mosaic configuration */
    if (FVID2_SOK == retVal)
    {
        retVal = VpsHal_vpdmaInitMosaic();

        VpsHal_vpdmaSetBgColorRGB(
            (VPS_CFG_VPDMA_ARGB_BKCOLOR >> 16u) & 0xFFu,
            (VPS_CFG_VPDMA_ARGB_BKCOLOR >> 8u) & 0xFFu,
            (VPS_CFG_VPDMA_ARGB_BKCOLOR >> 0u) & 0xFFu,
            (VPS_CFG_VPDMA_ARGB_BKCOLOR >> 24u) & 0xFFu);

        VpsHal_vpdmaSetBgColorYUV(
            (VPS_CFG_VPDMA_YUV_BKCOLOR >> 16u) & 0xFFu,
            (VPS_CFG_VPDMA_YUV_BKCOLOR >> 8u) & 0xFFu,
            (VPS_CFG_VPDMA_YUV_BKCOLOR >> 0u) & 0xFFu);
    }

    if (FVID2_SOK == retVal)
    {
        /* Stop the running list */
        for (listCnt = 0u; listCnt < VPSHAL_VPDMA_MAX_LIST; listCnt ++)
        {
            if(0u != VPSHAL_VPDMA_ISLISTBUSY(listCnt))
            {
                /* Clear out any pending sync on register */
                /* VpsHal_vpdmaSetSyncList(listCnt); */

                /* Assuming here lists are normal always */
                VpsHal_vpdmaStopList(listCnt, VPSHAL_VPDMA_LT_NORMAL);

                retVal = VpsHal_vpdmaListWaitComplete(listCnt, 5u);

                if (VPSHAL_VPDMA_ISLISTBUSY(listCnt))
                {
                    GT_1trace(
                        VpsHalTrace,
                        GT_INFO,
                        "List %d is busy, starting debug list to stop it\n",
                        listCnt);

                    regOvly = VpdmaBaseAddress;

                    VpsHal_vpdmaCreateAbortCtrlDesc(
                        descMem,
                        (VpsHal_VpdmaChannel)0);

                    /* Set the List Address */
                    regOvly->LIST_ADDR = (UInt32) descMem;

                    /* Set the List Attributes */
                    regValue = (((UInt32)listCnt <<
                         CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_NUM_SHIFT) &
                         CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_NUM_MASK) |
                                        ((VPSHAL_VPDMA_LT_NORMAL <<
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_TYPE_SHIFT) &
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_TYPE_MASK) |
                                        ((0 <<
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_SIZE_SHIFT) &
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_SIZE_MASK) |
                          0x80000;
                    regOvly->LIST_ATTR = regValue;

                    /* Wait for some time */
                    Task_sleep(10);

                    VpsHal_vpdmaStopList(listCnt, VPSHAL_VPDMA_LT_NORMAL);

                    /* Wait for some time */
                    Task_sleep(10);

                    if (VPSHAL_VPDMA_ISLISTBUSY(listCnt))
                    {
                        GT_1trace(VpsHalTrace,
                                  GT_ERR,
                                  "List %d is busy, Could not stop it\n",
                                  listCnt);
                    }
                    else
                    {
                        GT_1trace(VpsHalTrace,
                                  GT_INFO,
                                  "List %d is free\n",
                                  listCnt);

                        VpsHal_vpdmaCreateAbortCtrlDesc(
                            descMem,
                            (VpsHal_VpdmaChannel)0);

                        /* Assuming List-0 is free */
                        retVal = VpsHal_vpdmaPostList(
                                     0,
                                     VPSHAL_VPDMA_LT_NORMAL,
                                     descMem,
                                     VPSHAL_VPDMA_CTRL_DESC_SIZE,
                                     TRUE);

                        VpsHal_vpdmaStopList(0, VPSHAL_VPDMA_LT_NORMAL);
                    }
                }
            }
        }
    }

#ifndef PLATFORM_ZEBU
    if (FVID2_SOK == retVal)
    {
        descPtr = (UInt8* )descMem;
        /* Program Abort Descriptor for all the channels */
        for (chCnt = 0; chCnt < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS; chCnt++)
        {
            VpsHal_vpdmaCreateAbortCtrlDesc(
                (Void *)descPtr,
                (VpsHal_VpdmaChannel)chCnt);
            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
        /* Assuming List-0 is free */
        retVal = VpsHal_vpdmaPostList(
                     0,
                     VPSHAL_VPDMA_LT_NORMAL,
                     descMem,
                     (VPSHAL_VPDMA_MAX_CTRL_DESC *
                     VPSHAL_VPDMA_CTRL_DESC_SIZE),
                     TRUE);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsHalTrace,
                      GT_ERR,
                      "Could not post list to clean channel %d\n",
                      listCnt);
        }

        retVal = VpsHal_vpdmaListWaitComplete(0u, 100u);

        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsHalTrace,
                      GT_ERR,
                      "While clearing channel %d list is busy\n",
                      chCnt);
        }
    }
#endif

    if (NULL != descMem)
    {
        VpsUtils_freeDescMem(
            descMem,
            VPSHAL_VPDMA_CTRL_DESC_SIZE * VPSHAL_VPDMA_MAX_CTRL_DESC);
    }

    return (retVal);
}


/**
 *  VpsHal_vpdmaDeInit
 *  \brief Function De Initialize VPDMA and associated HALs.
 *
 *  \param arg           For the Future use, not used currently.
 *
 *  \return              Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaDeInit(Ptr arg)
{
    Int32   retVal;

    /* De Initialize mosaic utility function */
    retVal = VpsHal_vpdmaDeInitMosaic();

    return(retVal);
}



UInt32 VpsHal_vpdmaInitDebug(UInt32 baseAddr)
{
    /* Set VPDMA base address since FVID2_init is not called from application */
    VpdmaBaseAddress = (CSL_VpsVpdmaRegsOvly) baseAddr;
    gVpsHal_vpsBaseAddress = baseAddr - 0x0000D000u;

    VpsUtils_memset(&gVpsHalVpdmaTraceObj, 0u, sizeof(gVpsHalVpdmaTraceObj));

    return (VPS_SOK);
}



Int32 VpsHal_vpdmaListWaitComplete(UInt8 listNum, UInt32 timeout)
{
    Int32 ret = 0;
    UInt32 time;
    volatile CSL_VpsVpdmaRegsOvly    regOvly;
    Bool getListStatus;
    UInt32 listStatus;
    UInt32 cookie;

    getListStatus = TRUE;

    time = 0u;
    while ((0u != (VPSHAL_VPDMA_ISLISTBUSY(listNum))) &&
           (time < timeout))
    {
        /* ----- Delay for some time ----- */
        time += 1u;
        Task_sleep(1);

        if(time>8 && getListStatus)
        {
            Vpdma_ListPostTrace    *listPost;

            getListStatus = FALSE;

            regOvly = VpdmaBaseAddress;

            cookie = Hwi_disable();

            regOvly->PID =  0xDEAD0000 | (listNum & 0xF);

            listStatus = regOvly->PID & 0xFFF;

            Hwi_restore(cookie);

            /* Update trace */
            listPost = &gVpsHalVpdmaTraceObj.listPost[listNum];
            if (listPost->curIdx)
            {
                listPost->listStatus[listPost->curIdx-1] = listStatus;
            }
            else
            {
                listPost->listStatus[VPSHAL_VPDMA_MAX_TRACE_COUNT-1] = listStatus;
            }

        }
    }

    if(0u != (VPSHAL_VPDMA_ISLISTBUSY(listNum)))
    {
        ret = -1;
    }

    return ret;
}

/**
 *  VpsHal_vpdmaPostList
 *  \brief Function for posting the list to the VPDMA. Once posted, VPDMA will
 *  start reading and processing the list. It is interrupt protected so
 *  can be called from the interrupt context also.
 *
 *  \param listNum          List Number
 *  \param listType         List Type i.e. Normal or Self Modifying
 *  \param listAddr         Physical address of the contiguous memory
 *                          containing list
 *  \param listSize         List size in bytes
 *  \param enableCheck      Flag to indicate whether parameter check needs to
 *                          be done or not.
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaPostList(UInt8 listNum,
                         VpsHal_VpdmaListType listType,
                         Ptr listAddr,
                         UInt32 listSize,
                         UInt32 enableCheck)
{
    UInt32                  time = 0u, cnt;
    UInt32                  listAttr = 0u;
    UInt32                  lt = (UInt32) listType;
    Int                     ret = 0;
    CSL_VpsVpdmaRegsOvly    regOvly;
    UInt32                  cookie;
    Vpdma_ListPostTrace    *listPost;

    if (TRUE == enableCheck)
    {
        GT_assert(VpsHalTrace, (NULL != listAddr));
        GT_assert(VpsHalTrace, (0u == ((UInt32)listAddr &
                      (VPSHAL_VPDMA_LIST_ADDR_ALIGN - 1u))));
        GT_assert(VpsHalTrace, (0u == ((UInt32)listSize &
                      (VPSHAL_VPDMA_LIST_SIZE_ALIGN - 1u))));
        GT_assert(VpsHalTrace, (listNum < VPSHAL_VPDMA_MAX_LIST));
    }

    listAddr = (Ptr)VpsHal_vpdmaVirtToPhy(listAddr);
    regOvly = VpdmaBaseAddress;

    /* Number of 16 byte word in the new list of descriptors */
    listSize = (listSize & (~(VPSHAL_VPDMA_LIST_SIZE_ALIGN - 1u))) >>
                    VPSHAL_VPDMA_LIST_SIZE_SHIFT;

    while ((0u == (VPSHAL_VPDMA_ISBUSY)) && (time < VPSHAL_VPDMA_BUSY_TIMEOUT))
    {
        /* ----- Delay for some time ----- */
        time += 1u;
    }

    if(0u == (VPSHAL_VPDMA_ISBUSY))
    {
        ret = -1;
    }

    if(0 == ret)
    {
        time = 0u;
        while ((0u != (VPSHAL_VPDMA_ISLISTBUSY(listNum))) &&
               (time < VPSHAL_VPDMA_BUSY_TIMEOUT))
        {
            /* ----- Delay for some time ----- */
            time += 1u;
        }

        /* Disable global interrupts
           Note: Since List is always going to be
           Submitted from the single processor, hardware interrupts are
           disabled. If multiple processor will submit the list, it is
           required to take the hardware spinlock here. */
        cookie = Hwi_disable();

        if(0u != (VPSHAL_VPDMA_ISLISTBUSY(listNum)))
        {
            ret = -1;
        }

        if(0 == ret)
        {
            listAttr = (((UInt32)listNum <<
                         CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_NUM_SHIFT) &
                         CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_NUM_MASK);
            listAttr |= ((lt <<
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_TYPE_SHIFT) &
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_TYPE_MASK);
            listAttr |= ((listSize <<
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_SIZE_SHIFT) &
                          CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_SIZE_MASK);


            /* Set the List Address */
            regOvly->LIST_ADDR = (UInt32)listAddr;

#ifdef VPSHAL_VPDMA_DO_PID_REG_CHECK /* defined in vpshalVpdmaFirmware_vNNN.h */

            /* Post the list only when other list is not getting cleared */
            time = 0u;
            do
            {
                /* Wait for some time if other list is getting
                   cleared by checking bit 15th of the PID register */
                if ((regOvly->PID & (1 << 15)) == 0)
                {
                    break;
                }

                for (cnt = 0u; cnt < VPSHAL_VPDMA_BUSY_WAIT; cnt ++);

                time ++;
            } while (time < VPSHAL_VPDMA_BUSY_TIMEOUT);

            if (time != 0u)
            {
                gVpsHalVpdmaTraceObj.listPost[listNum].listWaitCnt ++;

                if (gVpsHalVpdmaTraceObj.listPost[listNum].maxListWait < time)
                {
                    gVpsHalVpdmaTraceObj.listPost[listNum].maxListWait = time;
                }
            }
#endif

            /* Set the List Attributes */
            regOvly->LIST_ATTR = listAttr;
        }
        /* ----- Release the Hardware spinlock ----- */
        /* Enable global interrupts */
        Hwi_restore(cookie);
    }


    /* Update trace */
    listPost = &gVpsHalVpdmaTraceObj.listPost[listNum];

    if (listPost->curIdx >= VPSHAL_VPDMA_MAX_TRACE_COUNT)
    {
        listPost->curIdx = 0u;
    }

    listPost->totalPosts++;
    listPost->listAddr[listPost->curIdx] = listAddr;
    listPost->listSize[listPost->curIdx] = listSize;
    listPost->retVal[listPost->curIdx] = ret;
    listPost->listType[listPost->curIdx] = listType;
    listPost->timeStamp[listPost->curIdx] = Clock_getTicks();
    listPost->curIdx++;
    if (listPost->curIdx >= VPSHAL_VPDMA_MAX_TRACE_COUNT)
    {
        listPost->curIdx = 0u;
    }

    return (ret);
}



/**
 *  VpsHal_vpdmaSetBgColorRGB
 *  \brief Function to set the background color in RGB.
 *
 *  \param red              Value of Red component in the color
 *  \param green            Value of Blue component in the color
 *  \param blue             Value of Green component in the color
 *  \param alpha            Value of Alpha component in the color
 *  \returns                None
 */
Void VpsHal_vpdmaSetBgColorRGB(UInt8 red, UInt8 green, UInt8 blue, UInt8 alpha)
{
    UInt32 value;

    value = (((UInt32)red << CSL_HD_VPS_VPDMA_BG_RGB_RED_SHIFT) &
              CSL_HD_VPS_VPDMA_BG_RGB_RED_MASK);
    value |= (((UInt32)green << CSL_HD_VPS_VPDMA_BG_RGB_GREEN_SHIFT) &
               CSL_HD_VPS_VPDMA_BG_RGB_GREEN_MASK);
    value |= (((UInt32)blue << CSL_HD_VPS_VPDMA_BG_RGB_BLUE_SHIFT) &
               CSL_HD_VPS_VPDMA_BG_RGB_BLUE_MASK);
    value |= (((UInt32)alpha << CSL_HD_VPS_VPDMA_BG_RGB_BLEND_SHIFT) &
               CSL_HD_VPS_VPDMA_BG_RGB_BLEND_MASK);

    (VpdmaBaseAddress)->BG_RGB = value;
}



/**
 *  VpsHal_vpdmaSetBgColorYUV
 *  \brief Function to set the background color in RGB.
 *
 *  \param y                Value of Luma component in the color
 *  \param cb               Value of Cb component in the color
 *  \param cr               Value of Cr component in the color
 *  \return                 None
 */
Void VpsHal_vpdmaSetBgColorYUV(UInt8 y, UInt8 cb, UInt8 cr)
{
    UInt32 value;

    value = (((UInt32)y << CSL_HD_VPS_VPDMA_BG_YUV_Y_SHIFT) &
                CSL_HD_VPS_VPDMA_BG_YUV_Y_MASK);
    value |= (((UInt32)cb << CSL_HD_VPS_VPDMA_BG_YUV_CB_SHIFT) &
                CSL_HD_VPS_VPDMA_BG_YUV_CB_MASK);
    value |= (((UInt32)cr << CSL_HD_VPS_VPDMA_BG_YUV_CR_SHIFT) &
                CSL_HD_VPS_VPDMA_BG_YUV_CR_MASK);

    (VpdmaBaseAddress)->BG_YUV = value;
}



/**
 *  VpsHal_vpdmaSetFrameStartEvent
 *  \brief Function is used to set the frame start event for the channel/client.
 *  This function tells VPDMA to use a given source of NF to transfer data
 *  to the down stream module. VPDMA starts transfer of data at the NF
 *  signal only. If there is no NF signal, it will transfer data to the
 *  internal FIFO. When FIFO becomes full, that channel will be blocked.
 *  Upper layer should pass the channel number to set the frame
 *  start event and VPDMA HAL sets it in the client register. This VPDMA
 *  client is the one associated with the channel number. If event is to
 *  be set for multiplexed client, upper layer can pass any channel
 *  associated with that client to set frame start event
 *
 *  \param chanNum          Channel Number for which frame start event is
 *                          to be set
 *  \param fsEvent          Source of NF signal
 *  \param lineMode         NONE
 *  \param reqDelay         NONE
 *
 *  \return                 None
 */
Void VpsHal_vpdmaSetFrameStartEvent(VpsHal_VpdmaChannel chanNum,
                                    VpsHal_VpdmaFSEvent fsEvent,
                                    VpsHal_VpdmaLineMode lineMode,
                                    UInt32 reqDelay,
                                    Ptr ovlyPtr,
                                    UInt32 index)
{
    UInt32 value;
    UInt32 fs = (UInt32)fsEvent;
    volatile UInt32 *ptr = NULL;

    ptr = (volatile UInt32 *)(((UInt32)VpdmaBaseAddress) +
                            (UInt32)(Channels[chanNum].clientCtrlReg));

    value = 0x0;
    value |= ((fs << CSL_HD_VPS_VPDMA_PRI_CHROMA_CSTAT_FRAME_START_SHIFT)
                & CSL_HD_VPS_VPDMA_PRI_CHROMA_CSTAT_FRAME_START_MASK);

    value |= ((UInt32)lineMode <<
                CSL_HD_VPS_VPDMA_SEC1_CHROMA_CSTAT_LINE_MODE_SHIFT) &
                CSL_HD_VPS_VPDMA_SEC1_CHROMA_CSTAT_LINE_MODE_MASK;

    value |= ((UInt32)reqDelay <<
                CSL_HD_VPS_VPDMA_SEC1_CHROMA_CSTAT_REQ_DELAY_SHIFT) &
                CSL_HD_VPS_VPDMA_SEC1_CHROMA_CSTAT_REQ_DELAY_MASK;

    if (NULL == ovlyPtr)
    {
        (*ptr) = value;
    }
    else
    {
        *(((UInt32 *)ovlyPtr) + index) = value;
    }
    /* TODO: LineMode should be set only to the client where chrus
     * is available */

}
Ptr VpsHal_vpdmaGetClientRegAdd(VpsHal_VpdmaChannel chanNum)
{
    return ((Ptr)(((UInt32)VpdmaBaseAddress) +
                            (UInt32)(Channels[chanNum].clientCtrlReg)));
}


/**
 *  VpsHal_vpdmaStopList
 *  \brief Function to stop the self modiyfing list. Self modifying list is a
 *  free running list. It is like a circular list which runs on its own.
 *  This function is used to stop self modifying list. When stop bit is set,
 *  it completes the current transfer and stops the list.
 *
 *  \param listNum          List to be stopped
 *  \param listType         NONE
 *  \return                 None
 */
Void VpsHal_vpdmaStopList(UInt8 listNum, VpsHal_VpdmaListType listType)
{
    UInt32 value;

    value = (((UInt32)listNum <<
             CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_NUM_SHIFT) &
             CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_NUM_MASK) |
             (((UInt32)listType <<
             CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_TYPE_SHIFT) &
             CSL_HD_VPS_VPDMA_LIST_ATTR_LIST_TYPE_MASK) |
             ((1u << CSL_HD_VPS_VPDMA_LIST_ATTR_STOP_SHIFT) &
             CSL_HD_VPS_VPDMA_LIST_ATTR_STOP_MASK);

    (VpdmaBaseAddress)->LIST_ATTR = value;
}



Int VpsHal_vpdmaLoadStateMachine(Ptr firmwareAddr, UInt32 timeout)
{
    Int                     ret = 0;
    UInt32                  time = 0u;
    CSL_VpsVpdmaRegsOvly    regOvly;

    GT_assert(VpsHalTrace, (NULL != firmwareAddr));
    GT_assert(VpsHalTrace,
        (0u == ((UInt32) firmwareAddr & (VPSHAL_VPDMA_LIST_ADDR_ALIGN - 1u))));

    firmwareAddr = (Ptr)VpsHal_vpdmaVirtToPhy(firmwareAddr);
    regOvly = VpdmaBaseAddress;
    if (0u == timeout)
    {
        timeout = 0xFFFFFFFF;
    }

    GT_0trace(VpsHalTrace, GT_INFO, "*** VPDMA Firmware Loading... ***\n");

    if (VPSHAL_VPDMA_FW_IS_LOADED)
    {
        /* firmware is already loaded, no need to load again */
        GT_0trace(VpsHalTrace, GT_INFO, "VPDMA Firmware already loaded !!!\n");
    }
    else
    {
        GT_1trace(VpsHalTrace, GT_INFO,
            "VPDMA Firmware Address = 0x%08x\n", firmwareAddr);
        GT_1trace(VpsHalTrace, GT_INFO,
            "VPDMA Load Address     = 0x%08x\n", (UInt32) &regOvly->LIST_ADDR);

        /* ----- Reset VPDMA ----- */

        /* toggle VPDMA reset to load the firmware */
        VpsHal_vpsClkcModuleReset(VPSHAL_VPS_CLKC_VPDMA, TRUE);
        Task_sleep(1);
        VpsHal_vpsClkcModuleReset(VPSHAL_VPS_CLKC_VPDMA, FALSE);

        /* Load firmware in the VPDMA */
        regOvly->LIST_ADDR = (UInt32) firmwareAddr;

        /* Wait till firmware gets loaded */
        while ((0u == (VPSHAL_VPDMA_ISBUSY)) && (time < timeout))
        /* ----- wait for some time ----- */
        {
            /* ----- Delay for some time ----- */
            time += 1u;
        }
        if (0u == (VPSHAL_VPDMA_ISBUSY))
        {
            ret = -1;
        }
        else
        {
            ret = 0;
        }
    }

    GT_1trace(VpsHalTrace, GT_INFO,
        "VPDMA Firmware Version = 0x%08x\n", regOvly->PID);
    GT_1trace(VpsHalTrace, GT_INFO,
        "VPDMA List Busy Status = 0x%08x\n", regOvly->LIST_STAT_SYNC);
    if (0u == ret)
    {
        GT_0trace(VpsHalTrace, GT_INFO,
            "*** VPDMA Firmware Load Success ***\n\n");
    }
    else
    {
        GT_0trace(VpsHalTrace, GT_INFO,
            "*** VPDMA Firmware Load ERROR!!! ***\n\n");
    }

    return (ret);
}



/**
 *  VpsHal_vpdmaCreateInBoundDataDesc
 *  \brief Function to create In Bound Data descriptor from the given parameters
 *  and in the given contiguous memory. Memory pointer given must contain
 *  physically contiguous memory because VPDMA works with that only.
 *
 *  \param memPtr           Pointer to physically contiguous memory into
 *                          which descriptor will be created
 *  \param descInfo         Pointer to structure containing in bound data
 *                          descriptor parameters
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateInBoundDataDesc(Ptr memPtr,
                                      VpsHal_VpdmaInDescParams *descInfo)
{
    volatile VpsHal_VpdmaInDataDesc *dataDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != descInfo));
    GT_assert(VpsHalTrace, (0u == ((UInt32)(descInfo->lineStride) &
                  (VPSHAL_VPDMA_LINE_STRIDE_ALIGN - 1u))));

    dataDesc = (volatile VpsHal_VpdmaInDataDesc *)memPtr;

    dataDesc->descType = VPSHAL_VPDMA_PT_DATA;
    dataDesc->channel = descInfo->channel;
    dataDesc->dataType = descInfo->dataType;
    dataDesc->transferHeight = descInfo->transHeight;
    dataDesc->transferWidth = descInfo->transWidth;
    dataDesc->frameHeight = descInfo->frameHeight;
    dataDesc->frameWidth = descInfo->frameWidth;
    dataDesc->horizontalStart = descInfo->startX;
    dataDesc->verticalStart = descInfo->startY;
    dataDesc->lineStride = descInfo->lineStride;
    dataDesc->evenSkip = dataDesc->oddSkip = descInfo->lineSkip;
    dataDesc->nextChannel = descInfo->nextChannel;
    dataDesc->memType = descInfo->memType;
    dataDesc->direction = VPSHAL_VPDMA_INBOUND_DATA_DESC;
    dataDesc->priority = descInfo->priority;
    dataDesc->notify = descInfo->notify;
    dataDesc->priority = descInfo->priority;
    if (TRUE == descInfo->is1DMem)
    {
        dataDesc->oneD = 1;
    }
    else
    {
        dataDesc->oneD = 0;
    }

    /* Reset unused fields to 0 */
    dataDesc->mosaicMode = 0u;
    dataDesc->clientSpecific1 = 0u;
    dataDesc->clientSpecific2 = 0u;
    dataDesc->reserved1 = dataDesc->reserved2 = dataDesc->reserved3 = 0;
    return (0);
}



/**
 *  VpsHal_vpdmaCreateOutBoundDataDesc
 *  \brief Function to create Out Bound Data descriptor from the given parameters
 *  and in the given contiguous memory. Memory pointer given must contain
 *  physically contiguous memory because VPDMA works with that only.
 *
 *  \param memPtr           Pointer to physically contiguous memory into
 *                          which descriptor will be created
 *  \param descInfo         Pointer to structure containing out bound data
 *                          descriptor parameters
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateOutBoundDataDesc(Ptr memPtr,
                                       VpsHal_VpdmaOutDescParams *descInfo)
{
    volatile VpsHal_VpdmaOutDataDesc *dataDesc;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != descInfo));
    GT_assert(VpsHalTrace, (0u == ((UInt32)(descInfo->lineStride) &
                  (VPSHAL_VPDMA_LINE_STRIDE_ALIGN - 1u))));

    dataDesc = (volatile VpsHal_VpdmaOutDataDesc *)memPtr;

    dataDesc->descType = VPSHAL_VPDMA_PT_DATA;
    dataDesc->dataType = (UInt32)descInfo->dataType;
    dataDesc->memType = descInfo->memType;
    dataDesc->direction = VPSHAL_VPDMA_OUTBOUND_DATA_DESC;
    dataDesc->channel = (UInt32)descInfo->channel;
    dataDesc->nextChannel = (UInt32)descInfo->nextChannel;
    dataDesc->lineStride = descInfo->lineStride;
    dataDesc->evenSkip = dataDesc->oddSkip = (UInt32)descInfo->lineSkip;
    dataDesc->notify = descInfo->notify;
    dataDesc->priority = descInfo->priority;
    dataDesc->maxWidth = descInfo->maxWidth;
    dataDesc->maxHeight = descInfo->maxHeight;
    if (TRUE == descInfo->is1DMem)
    {
        dataDesc->oneD = 1;
    }
    else
    {
        dataDesc->oneD = 0;
    }

    /* Reset unused fields to 0 */
    dataDesc->useDescReg = 0u;
    dataDesc->dropData = 0u;
    dataDesc->writeDesc = 0u;
    dataDesc->outDescAddress = 0u;
    dataDesc->clientSpecific1 = 0u;
    dataDesc->clientSpecific2 = 0u;
    dataDesc->descSkip = 0u;
    dataDesc->reserved1 = dataDesc->reserved2 = 0u;
    dataDesc->reserved3 = dataDesc->reserved4 = 0u;
    dataDesc->reserved5 = dataDesc->reserved6 = 0u;
    dataDesc->reserved7 = 0u;
    return (0);
}



/**
 *  VpsHal_vpdmaCreateRegionDataDesc
 *  \brief This function is used to create the Region data descriptor in
 *  the given memory. Upper layer passes the parameters need to be set
 *  in second argument and this function makes data descriptor in
 *  the memory given in the first argument. Memory pointer given must
 *  contain physically contiguous memory because VPDMA works with that only.
 *
 *  \param memPtr           Pointer to physically contiguous memory into
 *                          which descriptor will be created
 *  \param descInfo         Pointer to structure containing region data
 *                          descriptor parameters
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateRegionDataDesc(Ptr memPtr,
                                     VpsHal_VpdmaRegionDataDescParams *descInfo)
{
    volatile VpsHal_VpdmaRegionDataDesc *dataDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != descInfo));
    GT_assert(VpsHalTrace, (0u == ((UInt32)(descInfo->lineStride) &
                  (VPSHAL_VPDMA_LINE_STRIDE_ALIGN - 1u))));

    dataDesc = (volatile VpsHal_VpdmaRegionDataDesc *)memPtr;
    VpsUtils_memset((UInt8*)dataDesc, 0u,
                    sizeof(VpsHal_VpdmaRegionDataDesc));
    dataDesc->descType = VPSHAL_VPDMA_PT_DATA;
    dataDesc->channel = descInfo->channel;
    dataDesc->dataType = descInfo->dataType;
    dataDesc->transferHeight = dataDesc->regionHeight = descInfo->regionHeight;
    dataDesc->transferWidth = dataDesc->regionWidth = descInfo->regionWidth;
    dataDesc->horizontalStart = descInfo->startX;
    dataDesc->verticalStart = descInfo->startY;
    dataDesc->lineStride = descInfo->lineStride;
    dataDesc->evenSkip = dataDesc->oddSkip = descInfo->lineSkip;
    dataDesc->nextChannel = descInfo->nextChannel;
    dataDesc->memType = descInfo->memType;
    dataDesc->direction = VPSHAL_VPDMA_INBOUND_DATA_DESC;
    dataDesc->regionPriority = descInfo->regionPriority;
    dataDesc->regionAttr = descInfo->regionAttr &
                                        VPSHAL_VPDMA_GRPX_REGION_ATTR_MASK;
    dataDesc->bbAlpha = descInfo->bbAlpha;
    dataDesc->blendAlpha = descInfo->blendAlpha;
    dataDesc->blendType = descInfo->blendType;
    if(0 != (descInfo->regionAttr & VPSHAL_VPDMA_GRPX_TRANSPARENCY))
    {
        dataDesc->enableTransparency = 1u;
    }
    dataDesc->transMask = descInfo->transMask;
    dataDesc->transColor = descInfo->transColor;
    dataDesc->notify = descInfo->notify;
    dataDesc->priority = descInfo->priority;
    return (0);
}



/**
 *  VpsHal_vpdmaCalcRegOvlyMemSize
 *  \brief Function to calculate the size of memory required for creating
 *  VPDMA configuration address set for the given register offsets. It
 *  also returns the offsets of the virtual register in the register
 *  overlay from the starting location of the overlay in terms of
 *  words. This function will be used for the other HALs to calculate
 *  the size required to store configured through VPDMA and to get
 *  the virtual register offset so that they can modify virtual
 *  registers.
 *
 *  \param regOffset        List of Register Offsets
 *  \param numReg           Number of register offsets in first parameter
 *  \param virtRegOffset    Pointer to array in which virtual register
 *                          offsets will be returned. This offsets are
 *                          in terms of words.
 *  \return                 Size of memory in terms of bytes
 */
UInt32 VpsHal_vpdmaCalcRegOvlyMemSize(UInt32 * const *regOffset,
                                      UInt32 numReg,
                                      UInt32 *virtRegOffset)
{
    UInt32 memSize = 0u, cnt, offsetCnt;
    UInt32 blockSize, numBlockReg;

    GT_assert(VpsHalTrace, (NULL != regOffset));
    GT_assert(VpsHalTrace, (NULL != virtRegOffset));

    for (cnt = 0u; cnt < numReg;)
    {
        /* Get the number of register in the block and block size */
        vpdmaCalcBlockInfo(
                &regOffset[cnt],
                (numReg - cnt),
                &blockSize,
                &numBlockReg);
       /* Store the virtual register offset in the block */
       for(offsetCnt = 0; offsetCnt < numBlockReg; offsetCnt++)
       {
           virtRegOffset[cnt + offsetCnt] = memSize +
                                                VPSHAL_VPDMA_ADDR_SET_SIZE +
                                                offsetCnt;
       }
       /* Update the number of register counter and total memory size */
       cnt += numBlockReg;
       memSize += blockSize;
    }

    return (memSize * VPSHAL_VPDMA_WORD_SIZE);
}



/**
 *  VpsHal_vpdmaCreateRegOverlay
 *  Function is used to create complete configuration descriptor
 *  with the address set for the given registers offsets. It takes
 *  list of register offsets and pointer to memory, creates
 *  configuration descriptor and address set for the register set and
 *  returns the virtual addresses of the registers.
 *
 *  \param configRegList    List of Register Offsets
 *  \param numConfigReg     List of Register Offsets
 *  \param regOverlay       Pointer to array in which virtual register
 *                          offsets will be returned. This offsets are
 *                          in terms of words.
 *  \return                 Size of memory in terms of bytes
 */
Int VpsHal_vpdmaCreateRegOverlay(UInt32 * const *configRegList,
                                 UInt32 numConfigReg,
                                 Void *regOverlay)
{
    UInt32 blockSize, numBlockReg, cnt;
    UInt32 *regOvlyPtr = (UInt32 *)regOverlay;

    GT_assert(VpsHalTrace, (NULL != configRegList));
    GT_assert(VpsHalTrace, (NULL != regOverlay));

   for (cnt = 0u; cnt < numConfigReg;)
    {
        /* Get the number of register in the block and block size */
        vpdmaCalcBlockInfo(
                &configRegList[cnt],
                (numConfigReg - cnt),
                &blockSize,
                &numBlockReg);
       /* Set the start address and number of registers in the overlay
          header */
       ((VpsHal_VpdmaSubBlockHeader *)regOvlyPtr)->nextClientAddr =
            ((UInt32)configRegList[cnt] - gVpsHal_vpsBaseAddress);
       ((VpsHal_VpdmaSubBlockHeader *)regOvlyPtr)->subBlockLength = numBlockReg;
       ((VpsHal_VpdmaSubBlockHeader *)regOvlyPtr)->reserved1 = 0u;
       ((VpsHal_VpdmaSubBlockHeader *)regOvlyPtr)->reserved2 = 0u;
       ((VpsHal_VpdmaSubBlockHeader *)regOvlyPtr)->reserved3 = 0u;
       /* Move to the next register block */
       regOvlyPtr = regOvlyPtr + blockSize;
       /* Update the register counter */
       cnt += numBlockReg;
    }
    return (0);
}



/**
 *  VpsHal_vpdmaCreateCoeffOverlay
 *  \brief Function is used to create complete configuration descriptor for
 *  the scalar coefficient. It takes memory containing coefficients
 *  and type of scalar and returns configuration descriptor.
 *
 *  \param coeffMemory      Physical Address of the memory containing
                            coefficients
 *  \param coeffSize        Coefficient Memory Size In terms of bytes
 *  \param configDest       Scalar for which coefficients is to be
 *                          configured
 *  \param descPtr           Address of the Configuration Descriptor
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateCoeffOverlay(UInt32 *coeffMemory,
                                   UInt32 coeffSize,
                                   VpsHal_VpdmaConfigDest configDest,
                                   Void *descPtr)
{
    volatile VpsHal_VpdmaConfigDesc *configDesc =
        (volatile VpsHal_VpdmaConfigDesc *)descPtr;
    GT_assert(VpsHalTrace, (0 == ((UInt32)coeffMemory &
                        (VPSHAL_CONFIG_PAYLOAD_ADDR_ALIGN - 1u))));
    coeffMemory = (Ptr)VpsHal_vpdmaVirtToPhy(coeffMemory);
    /* Create the configuration Descriptor with the indirect method and block
       data set for the scalar coefficients */

    configDesc->descType = VPSHAL_VPDMA_PT_CONFIG;
    configDesc->class = VPSHAL_VPDMA_CPT_BLOCK_SET;
    configDesc->direct = VPSHAL_VPDMA_CCT_INDIRECT;
    configDesc->destination = (UInt32)configDest;
    configDesc->payloadAddress = (UInt32)coeffMemory;
    /* Payload size is in terms of 128 bit word. memSize is in terms of
     * words so divide it by word size to get 128 bit aligned size. */
    configDesc->payloadLength = (coeffSize >>
                                    VPSHAL_VPDMA_CONFIG_PAYLOADSIZE_SHIFT);
    /* DataLength need not be 128 bit aligned. */
    configDesc->dataLength = coeffSize;

    configDesc->reserved1 = 0u;

    return (0);
}

UInt8 *VpsHal_vpdmaGetCurrentDescWriteAddr(void)
{
    volatile CSL_VpsVpdmaRegsOvly    regOvly;

    regOvly = VpdmaBaseAddress;

    return (UInt8*)VpsHal_vpdmaPhyToVirt((Ptr)regOvly->CURRENT_DESCRIPTOR);
}

/**
 *  VpsHal_vpdmaSetDescWriteAddrReg
 *  \brief Function to write descriptor write address in the register. This
 *  is the address at which VPDMA will write back descriptor for the
 *  already captured channels.
 *
 *  \param addr             Physical address where descriptor is to
 *                          be written
 *  \param numDesc          Maximum number of descriptors which could be
 *                          written to this memory.
 *  \return                 None
 */
Int32 VpsHal_vpdmaSetDescWriteAddrReg(Ptr addr, UInt16 size)
{
    volatile UInt32 addrValue = (UInt32)addr;
    volatile UInt32 descWritten;
    volatile CSL_VpsVpdmaRegsOvly    regOvly;
    volatile UInt32 cookie;

    /* disable interrupts */
    cookie = Hwi_disable();

    addrValue = VpsHal_vpdmaVirtToPhy((Ptr)addrValue);
    regOvly = VpdmaBaseAddress;

    regOvly->DESCRIPTOR_TOP = (addrValue &
                               CSL_HD_VPS_VPDMA_DESCRIPTOR_TOP_ADDR_MASK);

    addrValue += size;

    regOvly->DESCRIPTOR_BOTTOM = ((UInt32)addrValue &
                            CSL_HD_VPS_VPDMA_DESCRIPTOR_BOTTOM_ADDR_MASK);

    /* Clear the Descriptor Count */
    regOvly->DESCRIPTOR_STATUS_CONTROL = CSL_HD_VPS_VPDMA_DESCRIPTOR_STATUS_CONTROL_CLR_DESCR_COUNT_MASK;

    /* restore interrupts */
    Hwi_restore(cookie);

    Task_sleep(1);

    descWritten = ( regOvly->DESCRIPTOR_STATUS_CONTROL >> 16 ) & 0xFF;


    return descWritten;
}



/**
 *  VpsHal_vpdmaSetSyncList
 *  \brief Function to register bit for the Sync on Register event.
 *
 *  \param listNum          List Number
 *  \return                 None
 */
Void VpsHal_vpdmaSetSyncList(UInt8 listNum)
{
    /* This is W1TS register, write 1 to set and writing 0 will
       have no effect. */
    (VpdmaBaseAddress)->LIST_STAT_SYNC = (0x1 << listNum);
}



/**
 *  VpsHal_vpdmaCreateConfigDesc
 *  \brief Function to create the configuration descriptor in the given memory.
 *
 *  \param memPtr           Pointer memory in which config descriptor is
 *                          to be created
 *  \param dest             Config descriptor destination
 *  \param clss             This indicates whether payload is blocks of
 *                          data or single contiguous block.
 *  \param direct           This indicates whether payload is stored in
 *                          contiguous with the descriptor or stored in some
 *                          other memory location
 *  \param payloadSize      Payload size in bytes
 *  \param payloadAddr      Pointer to payload memory for indirect
 *                          configuration
 *  \param destAddr         Destination address offset used for block type
 *                          configuration class. Not used for address set.
 *  \param dataSize         NONE
 *
 *  \return                 None
 */
Void VpsHal_vpdmaCreateConfigDesc(Ptr memPtr,
                                  VpsHal_VpdmaConfigDest dest,
                                  VpsHal_VpdmaConfigPayloadType clss,
                                  VpsHal_VpdmaConfigCmdType direct,
                                  UInt32 payloadSize,
                                  Ptr payloadAddr,
                                  Ptr destAddr,
                                  UInt16 dataSize)
{
    volatile VpsHal_VpdmaConfigDesc *configDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (VPSHAL_VPDMA_CCT_INDIRECT == direct) ||
                  (VPSHAL_VPDMA_CCT_DIRECT == direct));
    GT_assert(VpsHalTrace, (VPSHAL_VPDMA_CPT_ADDR_DATA_SET == clss) ||
                  (VPSHAL_VPDMA_CPT_BLOCK_SET == clss));
    payloadAddr = (Ptr)VpsHal_vpdmaVirtToPhy(payloadAddr);
    configDesc = (volatile VpsHal_VpdmaConfigDesc*)memPtr;
    configDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONFIG;
    configDesc->destination = (UInt32)dest;
    configDesc->class = clss;
    configDesc->direct = direct;
    configDesc->reserved1 = 0u;
    /* Payload size is in terms of 128 bit word. memSize is in terms of
     * words so divide it by word size to get 128 bit aligned size. */
    configDesc->payloadLength = (payloadSize >>
                                    VPSHAL_VPDMA_CONFIG_PAYLOADSIZE_SHIFT);

    /* Data Size need not be 128 bit aligned */
    configDesc->dataLength = (dataSize & VPSHAL_VPDMA_CONFIG_DATASIZE_MASK);

    if (VPSHAL_VPDMA_CCT_INDIRECT == direct)
    {
        GT_assert(VpsHalTrace, (0 == ((UInt32)payloadAddr &
            (VPSHAL_CONFIG_PAYLOAD_ADDR_ALIGN - 1u))));

        configDesc->payloadAddress = (UInt32)payloadAddr;
    }

    if (VPSHAL_VPDMA_CPT_BLOCK_SET == clss)
    {
        /* When creating dummy config descriptor, destAddr will be null,
           so there is no assert for this error */
        if ((VPSHAL_VPDMA_CONFIG_DEST_MMR == dest) &&
            (NULL != destAddr))
        {
            configDesc->destAddr = (UInt32) destAddr;
            GT_assert(VpsHalTrace,
                     (configDesc->destAddr >= gVpsHal_vpsBaseAddress));
            /* Subtract VPS Base address if config destination is
             * mmr register */
            configDesc->destAddr -= gVpsHal_vpsBaseAddress;
        }
        /*configuration descriptor is different for GRPX*/
        else if (((VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX0 == dest) ||
            (VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX1 == dest) ||
            (VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX2 == dest)) &&
            (NULL != destAddr))
        {
                configDesc->destAddr = (UInt32) destAddr;
        }
        else
        {
            /* The address offset in destination needs to be in units of 128-bit
             * words. The provided destAddr offset is in bytes. Hence it needs
             * to be divided by 16 to get the correct value.
             */
            configDesc->destAddr = (((UInt32) destAddr) >>
                                    VPSHAL_VPDMA_CONFIG_PAYLOADSIZE_SHIFT);
        }
    }
}



/**
 *  VpsHal_vpdmaCreateSOLCtrlDesc
 *  \brief Function to create Sync on List Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to memory for control descriptor
 *  \param lists            Array of list numbers
 *  \param numList          Number of lists in second argument
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOLCtrlDesc(Ptr memPtr, UInt8 *lists, UInt8 numList)
{
    UInt32 count, listMask = 0u;
    volatile VpsHal_VpdmaSyncOnListDesc *solDesc = NULL;

    GT_assert(VpsHalTrace, (NULL != memPtr));
    solDesc = (volatile VpsHal_VpdmaSyncOnListDesc *)memPtr;
    for (count = 0u; count < numList; count++)
    {
        listMask |= (0x1 << lists[count]);
    }
    solDesc->descType = VPSHAL_VPDMA_PT_CONTROL;
    solDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_SOL;
    solDesc->lists = listMask;
    solDesc->reserved1 = solDesc->reserved2 = 0u;
    solDesc->reserved3 = solDesc->reserved4 = 0u;
    solDesc->reserved5 = 0u;
}



/**
 *  VpsHal_vpdmaCreateSICtrlDesc
 *  \brief Function to create a Send Interrupt Control Descriptor
 *  in the given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param source           Send Interrupt source possible values are 0-15
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSICtrlDesc(Ptr memPtr, UInt16 source)
{
    volatile VpsHal_VpdmaSendIntrDesc *siDesc = NULL;

    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (source < VPSHAL_VPDMA_MAX_SI_SOURCE));

    siDesc = (volatile VpsHal_VpdmaSendIntrDesc *)memPtr;
    siDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    siDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_SI;
    siDesc->source = source;
    siDesc->reserved1 = siDesc->reserved2 = 0u;
    siDesc->reserved3 = siDesc->reserved4 = 0u;
    siDesc->reserved5 = 0u;
}



/**
 *  VpsHal_vpdmaCreateChangeClientIntrCtrlDesc
 *  \brief Function to create a control descriptor to change
 *  client interrupt
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose interrupt is to be changed
 *  \param event            event to which interrupt is to be changed
 *  \param lineCount        Line Count
 *  \param pixCount         Pixel Count
 *  \return                 None
 */
Void VpsHal_vpdmaCreateChangeClientIntrCtrlDesc(Ptr memPtr,
                                                VpsHal_VpdmaChannel channelNum,
                                                VpsHal_VpdmaSocEvent event,
                                                UInt16 lineCount,
                                                UInt16 pixCount)
{
    volatile VpsHal_VpdmaSyncOnClientDesc *socDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    socDesc = (volatile VpsHal_VpdmaSyncOnClientDesc *)memPtr;
    socDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    socDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_INTR_CHANGE;
    socDesc->event = (UInt32)event;
    socDesc->channel = (UInt32)channelNum;
    socDesc->lineCount = lineCount;
    socDesc->pixelCount = pixCount;
    socDesc->reserved1 = socDesc->reserved2 = 0u;
    socDesc->reserved3 = socDesc->reserved4 = 0u;
}



/**
 *  VpsHal_vpdmaCreateSOCCtrlDesc
 *  Function to create a Sync on Client control Descriptor
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose interrupt is to be changed
 *  \param event            event to which interrupt is to be changed
 *  \param lineCount        Line Count
 *  \param pixCount         Pixel Count
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOCCtrlDesc(Ptr memPtr,
                                   VpsHal_VpdmaChannel channelNum,
                                   VpsHal_VpdmaSocEvent event,
                                   UInt16 lineCount,
                                   UInt16 pixCount)
{
    volatile VpsHal_VpdmaSyncOnClientDesc *socDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    socDesc = (volatile VpsHal_VpdmaSyncOnClientDesc *)memPtr;
    socDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    socDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_SOC;
    socDesc->event = (UInt32)event;
    socDesc->channel = (UInt32)channelNum;
    socDesc->lineCount = lineCount;
    socDesc->pixelCount = pixCount;
    socDesc->reserved1 = socDesc->reserved2 = 0u;
    socDesc->reserved3 = socDesc->reserved4 = 0u;
}


/**
 *  VpsHal_vpdmaCreateCCISCtrlDesc
 *  Function to create a Change client interrupt source control descriptor
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose interrupt is to be changed
 *  \param event            event to which interrupt is to be changed
 *  \param lineCount        Line Count
 *  \param pixCount         Pixel Count
 *  \return                 None
 */
Void VpsHal_vpdmaCreateCCISCtrlDesc(Ptr memPtr,
                                    VpsHal_VpdmaChannel channelNum,
                                    VpsHal_VpdmaSocEvent event,
                                    UInt16 lineCount,
                                    UInt16 pixCount)
{
    volatile VpsHal_VpdmaChangeClientIntSrc *ccisDesc = NULL;

    GT_assert(VpsHalTrace, (NULL != memPtr));
    ccisDesc = (volatile VpsHal_VpdmaChangeClientIntSrc *)memPtr;

    ccisDesc->lineCount     = lineCount;
    ccisDesc->pixelCount    = pixCount;
    ccisDesc->event         = (UInt32)event;
    ccisDesc->ctrl          = (UInt32)VPSHAL_VPDMA_CDT_INTR_CHANGE;
    ccisDesc->source        = (UInt32)channelNum;
    ccisDesc->descType      = (UInt32)VPSHAL_VPDMA_PT_CONTROL;

    ccisDesc->reserved1 = ccisDesc->reserved2 = 0u;
    ccisDesc->reserved3 = ccisDesc->reserved4 = 0u;
}


/**
 *  VpsHal_vpdmaCreateInvertLmFidCtrlDesc
 *  \brief Function to create a control descriptor to Invert List Manager
 *  FID
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param fidNum           Indicates which LM Fid to be changed
 *  \return                 None
 */
Void VpsHal_vpdmaCreateLmFidCtrlDesc(Ptr memPtr,
                                     UInt32 fidNum,
                                     VpsHal_VpdmaLmFidCtrl lmFidCtrl)
{
    volatile VpsHal_VpdmaToggleLmFidDesc *lmFid = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    lmFid = (volatile VpsHal_VpdmaToggleLmFidDesc *)memPtr;
    lmFid->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    lmFid->ctrl = (UInt32)VPSHAL_VPDMA_CDT_TOGGLE_FID;
    if(0u == fidNum)
    {
        lmFid->lmFidCtrl0 = (UInt32)lmFidCtrl;
        lmFid->lmFidCtrl1 = 0u;
        lmFid->lmFidCtrl2 = 0u;
    }
    else if(1 == fidNum)
    {
        lmFid->lmFidCtrl0 = 0u;
        lmFid->lmFidCtrl1 = (UInt32)lmFidCtrl;
        lmFid->lmFidCtrl2 = 0u;
    }
    else
    {
        lmFid->lmFidCtrl0 = 0u;
        lmFid->lmFidCtrl1 = 0u;
        lmFid->lmFidCtrl2 = (UInt32)lmFidCtrl;
    }
    lmFid->reserved1 = lmFid->reserved2 = 0u;
    lmFid->reserved3 = lmFid->reserved4 = 0u;
}



/**
 *  VpsHal_vpdmaCreateAbortCtrlDesc
 *  \brief Function to create a control descriptor to Invert List Manager
 *  FID
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose data transfer is to be aborted
 *  \return                 None
 */
Void VpsHal_vpdmaCreateAbortCtrlDesc(Ptr memPtr,
                                     VpsHal_VpdmaChannel channelNum)
{
    volatile VpsHal_VpdmaAbortDesc *aDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    aDesc = (volatile VpsHal_VpdmaAbortDesc *)memPtr;
    aDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    aDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_ABT_CHANNEL;
    aDesc->channel = (UInt32)channelNum;
    aDesc->reserved1 = aDesc->reserved2 = 0u;
    aDesc->reserved3 = aDesc->reserved4 = 0u;
    aDesc->reserved5 = 0u;
}



/**
 *  VpsHal_vpdmaCreateRLCtrlDesc
 *  \brief Function to create Reload Control Descriptor in the given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param listNum          NONE
 *  \param rlListAddr       Address of the memory from where list is to be
 *                          reloaded
 *  \param size             Size of the reloaded list in bytes
 *  \return                 None
 */
Void VpsHal_vpdmaCreateRLCtrlDesc(Ptr memPtr,
                                  Ptr rlListAddr,
                                  UInt32 size)
{
    volatile VpsHal_VpdmaReloadDesc *rlDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    rlListAddr = (Ptr)VpsHal_vpdmaVirtToPhy(rlListAddr);
    rlDesc = (volatile VpsHal_VpdmaReloadDesc *)memPtr;
    rlDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    rlDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_RL;
    rlDesc->listSize = (size & (~(VPSHAL_VPDMA_LIST_SIZE_ALIGN - 1u))) >>
                            VPSHAL_VPDMA_LIST_SIZE_SHIFT;
    rlDesc->reloadAddr = (UInt32)rlListAddr;
    rlDesc->reserved1 = rlDesc->reserved2 = rlDesc->reserved3 = 0u;
}



/**
 *  VpsHal_vpdmaCreateSORCtrlDesc
 *  \brief Function to create Sync On Register Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param listNum          List Number for which SOR is to be created
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSORCtrlDesc(Ptr memPtr, UInt8 listNum)
{
    volatile VpsHal_VpdmaSyncOnRegDesc *sorDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    sorDesc = (volatile VpsHal_VpdmaSyncOnRegDesc *)memPtr;
    sorDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    sorDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_SOR;
    sorDesc->listNum = listNum;
    sorDesc->reserved1 = sorDesc->reserved2 = 0u;
    sorDesc->reserved3 = sorDesc->reserved4 = 0u;
    sorDesc->reserved5 = 0u;
}



/**
 *  VpsHal_vpdmaCreateSOTCtrlDesc
 *  \brief Function to create Sync On Timer Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param numCycles        Number of cycles to wait for.
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOTCtrlDesc(Ptr memPtr, UInt16 numCycles)
{
    volatile VpsHal_VpdmaSyncOnLmTimerDesc *sotDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    sotDesc = (volatile VpsHal_VpdmaSyncOnLmTimerDesc *)memPtr;
    sotDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    sotDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_SOT;
    sotDesc->numCycles = numCycles;
    sotDesc->reserved1 = sotDesc->reserved2 = 0u;
    sotDesc->reserved3 = sotDesc->reserved4 = 0u;
}



/**
 *  VpsHal_vpdmaCreateSOCHCtrlDesc
 *  \brief Function to create Sync On Channel Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel Number to sync on
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOCHCtrlDesc(Ptr memPtr,
                                    VpsHal_VpdmaChannel channelNum)
{
    volatile VpsHal_VpdmaSyncOnChannelDesc *socDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    socDesc = (volatile VpsHal_VpdmaSyncOnChannelDesc *)memPtr;
    socDesc->descType = (UInt32)VPSHAL_VPDMA_PT_CONTROL;
    socDesc->ctrl = (UInt32)VPSHAL_VPDMA_CDT_SOCH;
    socDesc->channel = (UInt32)channelNum;
    socDesc->reserved1 = socDesc->reserved2 = 0u;
    socDesc->reserved3 = socDesc->reserved4 = 0u;
    socDesc->reserved5 = 0u;
}



/**
 *  VpsHal_vpdmaCreateDummyDesc
 *  \brief Function to create Dummy descriptor of 4 words in the given memory
 *
 *  \param memPtr           Pointer to the memory in which dummy
 *                          descriptor is to be created
 *  \return                 None
 */
Void VpsHal_vpdmaCreateDummyDesc(Ptr memPtr)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    VpsHal_vpdmaCreateConfigDesc(memPtr,
                                 VPSHAL_VPDMA_CONFIG_DEST_MMR,
                                 VPSHAL_VPDMA_CPT_BLOCK_SET,
                                 VPSHAL_VPDMA_CCT_DIRECT,
                                 0u,
                                 NULL,
                                 NULL,
                                 0u);
}



/**
 *  VpsHal_vpdmaIsValidSize
 *  \brief Function to check whether the size is valid or not for
 *         the given input path and for the given format
 *
 *  \param vpdmaPath        VPDMA Path
 *  \param dataFormat       VPDMA Path
 *                          descriptor is to be created
 *  \return                 None
 */
UInt32 VpsHal_vpdmaIsValidSize(VpsHal_VpdmaPath vpdmaPath,
                               UInt32 dataFormat,
                               VpsHal_VpdmaMemoryType memType,
                               UInt32 width,
                               UInt32 height)
{
    Int32 retVal = FVID2_SOK;
    UInt32 lineSize;
	UInt32 cpuRev, platformId;

    GT_assert(VpsHalTrace, (vpdmaPath == VpdmaPathInfo[vpdmaPath].vpdmaPath));
    if ((VPSHAL_VPDMA_MT_TILEDMEM == memType) &&
        (TRUE != VpdmaPathInfo[vpdmaPath].isTiledDataSupported))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Width and Height should not be zero */
        if ((0u == width) || (0u == height))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check against the minimum and maximum width and height
         * supported by VPDMA path */
        if ((width > VPSHAL_VPDMA_MAX_FRAME_WIDTH) ||
            (height > VPSHAL_VPDMA_MAX_FRAME_HEIGHT)||
            (width < VPSHAL_VPDMA_MIN_FRAME_WIDTH) ||
            (height < VPSHAL_VPDMA_MIN_FRAME_HEIGHT))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
	cpuRev = Vps_platformGetCpuRev();
    platformId = Vps_platformGetId();

    if (FVID2_SOK == retVal)
    {
        if (VPSHAL_VPDMA_MT_TILEDMEM == memType)
        {
            lineSize = VpdmaPathInfo[vpdmaPath].tiledLineSize;
        }
        else
        {
            /* For Ne/Ce cpu >= 2.0 only and remaining platforms always*/
            if (((VPS_PLATFORM_ID_EVM_TI816x == platformId) &&
               (cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
               ((VPS_PLATFORM_ID_EVM_TI814x == platformId) &&
               (cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
               (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
            {
                lineSize = VpdmaPathInfo[vpdmaPath].enhancedLineSize;
            }
            else
            {
                lineSize = VpdmaPathInfo[vpdmaPath].lineSize;
            }
        }
        if (FVID2_DF_YUV422I_YUYV == dataFormat)
        {
            lineSize = lineSize / 2u;
        }
        if (width > lineSize)
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    return (retVal);
}



/**
 *  VpsHal_vpdmaReadConfig
 *  \brief Function to read configuration and coefficients through the VPI
 *  control bus. VPDMA provides two registers VPI_CTL_ADDRESS and
 *  VPI_CTL_DATA to to write/read the modules that are only on
 *  VPI CTL bus and not on the normal memory mapped bus. These
 *  include the GRPX modules and the coefficients for the SCs. This
 *  function reads the length number of words from the given
 *  client address for the given client. and stores them into the data
 *  pointer.
 *
 *  \param clientNum    Client Number. It must be between 0 to 127
 *  \param clientAddr   Address offset of the client. This address is
 *                      the local client address.
 *  \param length       Number of 32 bit words to be written from
 *                      the client. VPDMA writes data after collecting
 *                      128 bits of data so length must be 16 bytes/4 word
 *                      aligned.
 *  \param data         Pointer where read data will be stored. Memory
 *                      pointed by this pointer must be larde enough to
 *                      store length number of words.
 *  \return             Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaReadConfig(UInt8 clientNum,
                           UInt32 clientAddr,
                           UInt32 length,
                           UInt32 *data)
{
    UInt32 cnt;
    GT_assert(VpsHalTrace, (NULL != data));
    GT_assert(VpsHalTrace, (clientNum <= VPSHAL_VPDMA_MAX_CLIENT_NUM));

    /* --------- Get the Lock -------------- */
    /* Write the client number and client address in vpi_ctl_address
     * register */
    VpdmaBaseAddress->VPI_CTL_ADDRESS = ((clientNum <<
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_SHIFT) &
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_MASK) |
                ((clientAddr <<
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_ADDR_SHIFT) &
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_ADDR_MASK);
    for (cnt = 0; cnt < length; cnt ++)
    {
        data[cnt] = (VpdmaBaseAddress->VPI_CTL_DATA &
                        CSL_HD_VPS_VPDMA_VPI_CTL_DATA_DATA_MASK) >>
                        CSL_HD_VPS_VPDMA_VPI_CTL_DATA_DATA_SHIFT;
    }
    /* --------- Release the Lock -------------- */
    return (0);
}



Void VpsHal_vpdmaPrintCStat(void)
{
    UInt32                  chNum, regVal, flag = FALSE;
    volatile UInt32        *tempPtr;
    Vpdma_ChannelInfo      *chInfo;

    GT_0trace(VpsHalVpdmaDebugTrace, GT_INFO, " \n");
    GT_0trace(VpsHalVpdmaDebugTrace, GT_INFO, "CStat Register Dump:\n");
    GT_0trace(VpsHalVpdmaDebugTrace, GT_INFO, "--------------------\n");

    for (chNum = 0u; chNum < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS; chNum++)
    {
        chInfo = &Channels[chNum];

        if ((VPSHAL_VPDMA_CHANNEL_INVALID == chInfo->channelNum) ||
            (0u == chInfo->clientCtrlReg))
        {
            continue;
        }

        tempPtr = (volatile UInt32 *)
            ((UInt32)VpdmaBaseAddress + (UInt32) chInfo->clientCtrlReg);
        regVal = *tempPtr;
        if ((regVal & 0x0000C000u) != 0u)
        {
            GT_3trace(VpsHalVpdmaDebugTrace, GT_INFO,
                "CSTAT (0x%0.8X): 0x%0.8X (%s)\n",
                tempPtr, regVal, VpsHal_vpdmaGetChStr(chInfo->channelNum));
            flag = TRUE;
        }
    }

    if (!flag)
    {
        GT_0trace(VpsHalVpdmaDebugTrace, GT_INFO,
            "None of the clients are busy!!!\n");
    }

    return;
}

UInt32 VpsHal_vpdmaGetDeadListStatus(UInt32 listNum, UInt32 deadValue)
{
    volatile CSL_VpsVpdmaRegsOvly    regOvly;

    regOvly = VpdmaBaseAddress;

    regOvly->PID = (deadValue | (listNum & 0xF));

    return (regOvly->PID);
}



UInt32 VpsHal_vpdmaGetListCompleteStatus(UInt32 listNum)
{
    volatile CSL_VpsVpdmaRegsOvly    regOvly;

    regOvly = VpdmaBaseAddress;

    return (regOvly->INT0_LIST0_INT_STAT & (1u << (listNum * 2u)) >> (listNum * 2u));
}

Void VpsHal_vpdmaClearListCompleteStatus(UInt32 listNum)
{
    volatile CSL_VpsVpdmaRegsOvly    regOvly;

    regOvly = VpdmaBaseAddress;

    regOvly->INT0_LIST0_INT_STAT = 1u << (listNum * 2u);
}

/**
 *  VpsHal_vpdmaWriteConfig
 *  \brief Function to write configuration and coefficients through the VPI
 *  control bus. VPDMA provides two registers VPI_CTL_ADDRESS and
 *  VPI_CTL_DATA to to write/read the modules that are only on
 *  VPI CTL bus and not on the normal memory mapped bus. These
 *  include the GRPX modules and the coefficients for the SCs. This
 *  function writes the length number of words from the given
 *  client address for the given client. and stores them into the data
 *  pointer
 *
 *  \param clientNum    Client Number. It must be between 0 to 127
 *  \param clientAddr   Address offset of the client. This address is
 *                      the local client address.
 *  \param length       Number of 32 bit words to be written from
 *                      the client. VPDMA writes data after collecting
 *                      128 bits of data so length must be 16 bytes/4 word
 *                      aligned.
 *  \param data         Pointer to the memory containing data to be
 *                      written.
 *  \return             Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaWriteConfig(UInt8 clientNum,
                            UInt32 clientAddr,
                            UInt32 length,
                            UInt32 *data)
{
    UInt32 cnt;
    GT_assert(VpsHalTrace, (NULL != data));
    GT_assert(VpsHalTrace, (clientNum <= VPSHAL_VPDMA_MAX_CLIENT_NUM));
    GT_assert(VpsHalTrace,
        (0 == (length & (VPSHAL_VPDMA_VPI_SIZE_ALIGN - 1u))));

    /* --------- Get the Lock -------------- */
    /* Write the client number and client address in vpi_ctl_address
     * register */
    VpdmaBaseAddress->VPI_CTL_ADDRESS = ((clientNum <<
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_SHIFT) &
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_MASK) |
                ((clientAddr <<
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_ADDR_SHIFT) &
                CSL_HD_VPS_VPDMA_VPI_CTL_ADDRESS_CLIENT_ADDR_MASK);
    for (cnt = 0; cnt < length; cnt ++)
    {
        /* Write data in the VPI_CTL_DATA register */
        VpdmaBaseAddress->VPI_CTL_DATA = (data[cnt] <<
                    CSL_HD_VPS_VPDMA_VPI_CTL_DATA_DATA_SHIFT) &
                    CSL_HD_VPS_VPDMA_VPI_CTL_DATA_DATA_MASK;
    }
    /* --------- Release the Lock -------------- */
    return (0);
}



/**
 *  \brief Function to calculate block information for the VPDMA Register
 *  Overlay
 */
Void vpdmaCalcBlockInfo(UInt32 * const *regOffset,
                        UInt32 numReg,
                        UInt32 *blockSize,
                        UInt32 *numBlockReg)
{
    UInt32 prevOffset = 0u, count;
    Int diff;

    GT_assert(VpsHalTrace, (NULL != regOffset));
    GT_assert(VpsHalTrace, (NULL != blockSize));
    GT_assert(VpsHalTrace, (NULL != numBlockReg));

    if (numReg > 0)
    {
        /* At least one address set is needed for the first register */
        *blockSize = VPSHAL_VPDMA_ADDR_SET_SIZE + VPSHAL_VPDMA_MIN_REG_SET_SIZE;
        *numBlockReg = 1;
        prevOffset = (UInt32)regOffset[0];
    }
    for (count = 1u; count < numReg; count++)
    {
        /*
         * Workaround: Data payload cannot be more than 128 bytes (including
         * header of 4 words). Hence creating new block when number of
         * registers in a block cross VPSHAL_VPDMA_CONFIG_MMR_MAX_BLOCK_REG.
         */
        /* If the offset difference between current and previous register is
         * greater than 4, create a new address set */
        diff = (Int)regOffset[count] - (Int)prevOffset;
        if (((diff > (Int)VPSHAL_VPDMA_REG_OFFSET_DIFF) || (diff < 0)) ||
            ((*numBlockReg) >= VPSHAL_VPDMA_CONFIG_MMR_MAX_BLOCK_REG))
        {
            break;
        }
        /* Address set is always 4 word aligned so for the 5th register,
         * new memory is required */
        if (0u == ((*numBlockReg) & (VPSHAL_VPDMA_REG_OFFSET_DIFF - 1u)))
        {
            (*blockSize) += VPSHAL_VPDMA_ADDR_SET_SIZE;
        }
        (*numBlockReg) += 1u;
        prevOffset = (UInt32)regOffset[count];
    }
}

static UInt16 *VpsHal_vpdmaGetFirmwareAddr(VpsHal_VpdmaVersion ver)
{
    UInt16         *vpdmaFirmware = NULL;
    switch (ver)
    {
        case VPSHAL_VPDMA_VER_195:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_195;
        break;
        case VPSHAL_VPDMA_VER_1A3:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1A3;
        break;
        case VPSHAL_VPDMA_VER_1AD:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1AD;
        break;
        case VPSHAL_VPDMA_VER_1AE:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1AE;
        break;
        case VPSHAL_VPDMA_VER_1B0:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1B0;
        break;
        case VPSHAL_VPDMA_VER_1B2:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1B2;
        break;
        case VPSHAL_VPDMA_VER_1B5:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1B5;
        break;
        case VPSHAL_VPDMA_VER_1B7:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1B7;
        break;
        case VPSHAL_VPDMA_VER_286:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_286;
        break;
        case VPSHAL_VPDMA_VER_288:
            vpdmaFirmware = gVpsHal_vpdmaFirmware_288;
        break;
        default :
            vpdmaFirmware = gVpsHal_vpdmaFirmware_1AD;
        break;
    }
#ifdef VPS_CFG_ENABLE_CUSTOM_FW_LOAD
    UInt16         *tempPtr;
    FILE           *hFile;
    static char     filePath[256u];
    UInt32          bytesRead, totalRead;

    while (1)
    {
        Vps_printf("Enter VPDMA firmware hex file path: ");
        scanf("%s", filePath);
        Vps_printf("\n");

        /* Open file */
        hFile = fopen(filePath, "rt");
        if (NULL == hFile)
        {
            Vps_printf("Can't open file %s. Try again!!\n", filePath);
            continue;
        }
        else
        {
            totalRead = 0u;
            tempPtr = &gVpsVpdmaHalCustomFw[0u];
            while (1)
            {
                bytesRead = fscanf(hFile, "%x", tempPtr);
                if (bytesRead <= 0)
                {
                    break;
                }
                totalRead += 2u;
                tempPtr++;
            }
            GT_assert(VpsHalTrace, (totalRead <= sizeof(gVpsVpdmaHalCustomFw)));
            fclose(hFile);

            vpdmaFirmware = gVpsVpdmaHalCustomFw;
            break;
        }
    }
#endif

    return (vpdmaFirmware);
}

/**
 *  \brief Function returns channels associate client number. Supports only for
 *         VIP channels as of now. Un supported channel would result in assert.
 */
VpsHal_VpdmaClient VpsHal_vpdmaGetClientNo(VpsHal_VpdmaChannel chan)
{
    VpsHal_VpdmaClient clientNo = VPSHAL_VPDMA_CLIENT_INVALID;

    if ((chan > VPSHAL_VPDMA_CHANNEL_INVALID) &&
        (chan < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS))
    {
        /* Following mappings is based on VPDMA spec versioned 9.1.1 */
        if ((chan >= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC0) &&
              (chan <= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC15))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP0_LO_Y;
        }
        else if ((chan == VPSHAL_VPDMA_CHANNEL_VIP0_PORTB_LUMA) ||
                 (chan == VPSHAL_VPDMA_CHANNEL_VIP0_PORTB_RGB))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP0_LO_Y;
        }
        else if (((chan >= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC0) &&
                  (chan <= VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC15))
                 ||
                 (chan == VPSHAL_VPDMA_CHANNEL_VIP0_PORTB_CHROMA))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP0_LO_UV;
        }
        else if ((chan == VPSHAL_VPDMA_CHANNEL_VIP0_PORTA_LUMA) ||
                 (chan == VPSHAL_VPDMA_CHANNEL_VIP0_PORTA_RGB))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP0_HI_Y;
        }
        else if (chan == VPSHAL_VPDMA_CHANNEL_VIP0_PORTA_CHROMA)
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP0_HI_UV;
        }
        else if ((chan >= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC0) &&
                 (chan <= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC15))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP1_LO_Y;
        }
        else if ((chan == VPSHAL_VPDMA_CHANNEL_VIP1_PORTB_LUMA) ||
                 (chan == VPSHAL_VPDMA_CHANNEL_VIP1_PORTB_RGB))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP1_LO_Y;
        }
        else if (((chan >= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC0) &&
                  (chan <= VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC15))
                 ||
                 (chan == VPSHAL_VPDMA_CHANNEL_VIP1_PORTB_CHROMA))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP1_LO_UV;
        }
        else if ((chan == VPSHAL_VPDMA_CHANNEL_VIP1_PORTA_LUMA) ||
                 (chan == VPSHAL_VPDMA_CHANNEL_VIP1_PORTA_RGB))
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP1_HI_Y;
        }
        else if (chan == VPSHAL_VPDMA_CHANNEL_VIP1_PORTA_CHROMA)
        {
            clientNo = VPSHAL_VPDMA_CLIENT_VIP1_HI_UV;
        }
        else
        {
            GT_assert(VpsHalTrace, FALSE);
        }
    }

    return (clientNo);
}
