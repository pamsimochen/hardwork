/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_displayPriv.h
 *
 *  \brief VPS display driver internal header file containing instance and
 *  other object declaration and function declarations.
 */

#ifndef _VPSDRV_DISPLAYPRIV_H
#define _VPSDRV_DISPLAYPRIV_H

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

/**
 *  \brief Number of descriptor set used for display operation.
 *  Currently same as supported by DLM.
 */
#define VPSDDRV_NUM_DESC_SET            (DC_NUM_DESC_SET)

/**
 *  \brief Minimum number of buffers needed for starting the display operation.
 */
#define VPSDDRV_MIN_PRIME_BUFFERS       (1u)

/**
 *  \brief Maximum number of memory objects to create - 1 extra for non-mosaic
 *  configuration.
 */
#define VPSDDRV_MAX_MEMORY_OBJECTS      (VPSDDRV_MAX_MULTIWIN_SETTINGS + 1u)

/** \brief Invalid Multi window layout ID. */
#define VPSDDRV_INVALID_LAYOUT_ID       (0xFFFFFFFFu)

/** \brief Index to memory object to store non-mosaic memory information. */
#define VPSDDRV_NON_MOSAIC_MEM_IDX      (0u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsDdrv_States
 *  \brief Different states of the driver.
 */
typedef struct
{
    UInt32                  isInit;
    /**< Flag to indicate intialization is done or not for an instance. */
    UInt32                  isOpened;
    /**< Flag to indicate whether a instance is opened or not. */
    UInt32                  isFbMode;
    /**< Flag to indicate whether the drive is openned as Frame buffer mode */
    UInt32                  isStarted;
    /**< Flag to indicate whether the display has started or not. */
    UInt32                  isMultiWinMode;
    /**< Flag to indicate whether the driver is configured for multiple window
         mode. */
} VpsDdrv_States;

/**
 *  struct VpsDdrv_QueObj
 *  \brief Structure defining the queue object used in queue/dequeue operation.
 *  Instead of creating framelist objects, this is used so that any other
 *  information could be queued/dequeued along with the framelist.
 *  Already qElem is added to avoid dynamic allocation of Queue nodes.
 */
typedef struct
{
    VpsUtils_QElem          qElem;
    /**< VPS utils queue element used in node addition. */
    FVID2_FrameList         frameList;
    /**< FVID2 frame list to store the incoming/outgoing IO packets. */
} VpsDdrv_QueObj;

/**
 *  struct VpsDdrv_BufManObj
 *  \brief Structure to store the buffer management functionality variables.
 */
typedef struct
{
    UInt32                  isProgressive;
    /**< Flag to indicate whether the display is progressive or interlaced. */
    UInt32                  expectedSet;
    /**< Indicates the next set interrupt expected from the DLM. This is
         used to check if DLM interrupts are occurring at proper sequence
         without any interrupt misses. */
    UInt32                  fid;
    /**< Indicates current field ID. Used in interlaced display. */
    VpsDdrv_QueObj         *semiPrgmedBuf;
    /**< Used in interlaced display since frames (both feilds) are queued to
         the driver for interlaced display instead of one field at a time. */
    VpsDdrv_QueObj         *prgmedBuf[VPSDDRV_NUM_DESC_SET];
    /**< Current programmed buffers. For progressive this will be equal to
         the number of sets. For interlaced this will be half of the same as
         fields are being programmed in each of the set. */
    UInt32                  numPrgmedFrm;
    /**< Number of frames programmed in the descriptors. For progressive mode
         this will be equal to VPSDDRV_NUM_DESC_SET. For interlaced mode this
         will be equal to VPSDDRV_NUM_DESC_SET/2. The other pointers in
         prgmedBuf are not used. */
    VpsUtils_QHandle        reqQ;
    /**< Queue object to put the input requests. */
    VpsUtils_QHandle        doneQ;
    /**< Queue object to put the processed output requests. */
    VpsDdrv_QueObj         *fbQObj;
    /**< queue object for frame buffer mode of display */
} VpsDdrv_BufManObj;

/**
 *  struct VpsDdrv_MemObj
 *  \brief Structure to store the descriptor memory information variables.
 */
typedef struct
{
    UInt32                  layoutId;
    /**< Layout ID to which this object belongs to. */
    UInt32                  coreLayoutId;
    /**< Core layout ID to which this object belongs to. This could be
         different from the layout ID from user space. */
    Void                   *descMem;
    /**< Points to the descriptor memory containing the data, config,
         coeff and reload descriptors for all the sets. */
    UInt32                  descMemSize;
    /**< Size of descriptor memory allocated in bytes. */
    Void                   *abortDesc[VPSDDRV_NUM_DESC_SET];
    /**< Pointer to the abort descriptor used to abort all the channels
         of the client before starting the display. */
    UInt32                  numAbortCh;
    /**< Number of channels to be aborted before start of display. */
    VpsHal_VpdmaChannel     abortCh[VCORE_MAX_VPDMA_CH];
    /**< Channels to abort as returned by core. */
    Vcore_DescMem           coreDescMem[VPSDDRV_NUM_DESC_SET];
    /**< Structure holding the data descriptor, overlay memory references
         for the core for all the set. */
    Dc_DescInfo             dcDescInfo;
    /**< Display controller descriptor memory information. */
} VpsDdrv_MemObj;

/**
 *  struct VpsDdrv_InstObj
 *  \brief Per instance information.
 */
typedef struct
{
    UInt32                  drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource            resrcId;
    /**< Path resource ID. */
    Dc_NodeNum              dcNodeNum;
    /**< Node number for registering to display controller. */
    UInt32                  coreInstId;
    /**< Core instance number/ID. */
    const Vcore_Ops        *coreOps;
    /**< Core function pointers. */

    FVID2_DrvCbParams       fdmCbPrms;
    /**< FVID2 driver manager callback function parameters. */

    VpsDdrv_States          state;
    /**< Current state. */
    VpsUtils_QHandle        freeQ;
    /**< Queue for queueing all the free queue objects. */
    VpsDdrv_BufManObj       bmObj;
    /**< Buffer management object. */

    VpsDdrv_MemObj          memObj[VPSDDRV_MAX_MEMORY_OBJECTS];
    /**< Memory management object.
         Note: Index 0 is used for storing the memory information for
         non-mosaic display configuration. The remaining objects are used for
         mosaic configuration. */
    UInt32                  curMemIdx;
    /**< Index to current memory object used. */
    UInt32                  isDescDirty[VPSDDRV_NUM_DESC_SET];
    /**< Flag to indicate whether a descriptor is dirty or not. */

    UInt32                  queueCount;
    /**< Counter to keep track of how many requests are queued to the driver. */
    UInt32                  dequeueCount;
    /**< Counter to keep track of how many requests are dequeued from the
        driver. */
    UInt32                  displayedFrameCount;
    /**< Counter to keep track of how many frames are displayed. For interlaced
         display, this is half of the actual field display. */
    UInt32                  repeatFrameCount;
    /**< Counter to keep track of how many frames are repeated when the
         application fails to queue buffer at the display rate. */

    UInt32                  memType;
    /**< Buffer memory type - tiled or non-tiled.
         For valid values see #Vps_VpdmaMemoryType. */
    UInt32                  periodicCallbackEnable;
    /**< Periodic callback enable or not. */

    Dc_PathInfo             dcPathInfo;
    /**< Display controller path information. */
    Dc_RtConfig             vcompRtCfg;
    /**< VCOMP runtime configuration - used when VCOMP is in the path. */
    Dc_ClientInfo           dcClientInfo;
    /**< Display controller client info. */
    Vps_DeiDispParams       deiDispParams;
    /**< DEI core related parameters only valid for
     * VPS_DISP_INST_MAIN_DEIH_SC1 & VPS_DISP_INST_AUX_DEI_SC2 driver instances.
     */
    Vcore_Handle            coreHandle;
    /**< Core handle. */
    DcClientHandle          dcHandle;
    /**< Display controller handle. */

    VpsDdrv_QueObj          qObjPool[VPSDDRV_MAX_QOBJS_PER_INST];
    /**< Display driver queue object pool. */

    Semaphore_Handle        instSem;
    /**< Semaphore to protect the open/close calls and other memory
         allocation per instance. */

    Vcore_ScCoeffParams     coeffPrms;
    /**< Scalar coefficients params */

    UInt32                  isFrmUpdated;
    /**< Flag to check if frame is updated within one frame time period,
         otherwise there will be frame repeat */
} VpsDdrv_InstObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 vpsDdrvPrivInit(UInt32 numInst, const VpsDdrv_InitPrms *initPrms);
Int32 vpsDdrvPrivDeInit(void);

VpsDdrv_InstObj *vpsDdrvGetInstObj(UInt32 instId);
void vpsDdrvFillCreateStatus(const VpsDdrv_InstObj *instObj,
                             Vps_DispCreateStatus *createStatus);

Int32 vpsDdrvConfigCore(VpsDdrv_InstObj *instObj, const FVID2_Format *fmt);
Int32 vpsDrvAllocSliceCoreMem(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj);
Int32 vpsDrvFreeCoreMem(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj);

Int32 vpsDdrvConfigDctrl(VpsDdrv_InstObj *instObj);
Int32 vpsDdrvDeConfigDctrl(VpsDdrv_InstObj *instObj);

Int32 vpsDdrvDeleteLayout(VpsDdrv_InstObj *instObj, UInt32 layoutId);
Int32 vpsDdrvDeleteAllLayout(VpsDdrv_InstObj *instObj);

Int32 vpsDdrvAssignLayoutId(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj);
VpsDdrv_MemObj *vpsDdrvGetMemObjForLayoutId(VpsDdrv_InstObj *instObj,
                                            UInt32 layoutId);
UInt32 vpsDdrvGetIdxForMemObj(const VpsDdrv_InstObj *instObj,
                              const VpsDdrv_MemObj *memObj);

VpsDdrv_MemObj *vpdDdrvAllocMemObj(VpsDdrv_InstObj *instObj);
Int32 vpdDdrvFreeMemObj(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj);

Int32 vpsDdrvCheckFormat(const VpsDdrv_InstObj *instObj,
                         const FVID2_Format *fmt);
Int32 vpsDdrvCheckDeiDispParams(const VpsDdrv_InstObj *instObj,
                                const Vps_DeiDispParams *deiDispPrms);


#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_DISPLAYPRIV_H */
