/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_dispGrpxPriv.h
 *
 *  \brief VPS GRPX Display driver internal header file.
 */

#ifndef _VPSDRV_DISPGRPXPRIV_H
#define _VPSDRV_DISPGRPXPRIV_H

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
 *  \brief Maximum Regions supported in one frame
 */
#define VPSDDRV_GRPX_MAX_REGIONS          (VPS_GRPX_MAX_REGIONS_PER_FRAME)


/** \breif Maximum Number of pending requrest per driver instance. */
#define VPSDDRV_GRPX_MAX_QOBJS_PER_INST   (10u)


/**
 *  \brief Miminum number of buffers to start the graphics display
 */
#define VPSDDRV_GRPX_MIN_BUFFERS           (1u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsDdrv_GrpxDescDirty
 *  \brief  flag to indicate the data and config descriptor is dirty or not
 */
typedef struct
{
    UInt32         dataDescDirty[VPSDDRV_GRPX_MAX_REGIONS];
    /**< flag whether the descriptor set is needed update*/
    UInt32         configDescSet[VPSDDRV_GRPX_MAX_REGIONS];
    /**< flag whether the config descriptor is set or not associate
         with each regions this is reserved for the future usage*/
}VpsDdrv_GrpxDescDirty;

/**
 *  struct VpsDdrv_GrpxStates
 *  \brief  state of driver.
 */
typedef struct
{
    UInt32                   isInit;
    /**< Flag to indicate whether intialization is done or not for an instance. */
    UInt32                   isFbMode;
    /**< Flag to indicate whether the drive is openned as Frame buffer mode */
    UInt32                   isOpened;
    /**< Flag to indicate whether a instance is opened or not. */
    UInt32                   isStarted;
    /**< Flag to indicate whether the display has started or not. */
    UInt32                   isMultiRegion;
    /**< Flag to indicate whether it is multiple regions or not. */
} VpsDdrv_GrpxStates;


/**
 *  struct VpsDdrv_GrpxDescParams
 *  \brief descriptor and virReg information
 */
typedef struct
{
    UInt32                   numInDataDesc;
    /**< number of inbound data descriptor */
    UInt32                   numOutDataDesc;
    /**< number of outbound data descriptor */
    UInt32                   numMultiDataDesc;
    /**< number of multiple region data descriptors */
    UInt32                   configMemSize;
    /**< configure overlay memory:frame attribute + coeff size */
    VpsHal_VpdmaConfigDest   configDest;
    /**< config descriptor destination, this filed is used to tell VPDMA
        that which GRPX port this configuration descrpitor should go to*/
    UInt32                   confgiMemOffset;
    /**< config descriptor offset in the GRPX hardware, this will tell the VPDMA
      where this config descriptor should go to.*/
    Ptr                      configDesc[DC_NUM_DESC_SET];
    /**< GRPX config descriptor address used to store both frame configuration
          and scaling coefficients*/
    Ptr                      descMem;
    /**< This is pointer to the descriptor memory allocated by the driver*/
    UInt32                   descMemSize;
    /**< Size of descriptor memory allocated in bytes. */
    Vcore_DescMem            coreDescMem[DC_NUM_DESC_SET];
    /**< Desc informaion need by the core created based on the descMem*/
    Void                     *configMem;
    /**< Points to the config overlay memory*/
    Void                     *scMem[VPSDDRV_GRPX_MAX_REGIONS];
    /**< Points to the SC overlay memory, this is reserved for future usage.*/
    Dc_DescInfo               dcDescInfo;
    /**< Display Controller descriptor memory information*/
    Void                   *abortDesc[DC_NUM_DESC_SET];
    /**< Pointer to the abort descriptor used to abort all the channels
         of the client before starting the display. */
    UInt32                  numAbortCh;
    /**< Number of channels to be aborted before start of display. */
    VpsHal_VpdmaChannel     abortCh[VCORE_MAX_VPDMA_CH];
    /**< Channels to abort as returned by core. */

}VpsDdrv_GrpxDescParams;
/**
 *  struct VpsDdrv_GrpxQueueObj
 *  \brief Structure defining the queue object used in queue/dequeue operation.
 *  Instead of creating framelist objects, this is used so that any other
 *  information could be queued/dequeued along with the framelist.
 *  Already qElem is added to avoid dynamic allocation of Queue nodes.
 */
typedef struct
{
    VpsUtils_QElem           qElem;
    /**< VPS utils queue element used in node addition. */
    FVID2_FrameList          frameList;
    /**< FVID2 frame list to store the incoming/outgoing IO packets. */
} VpsDdrv_GrpxQueueObj;

/**
 *  struct VpsDdrv_GrpxBufManObj
 *  \brief the buffer management functionality information
 */
typedef struct
{
    UInt32                   isProgressive;
    /**< Flag to indicate whether the display is progressive or interlaced. */
    UInt32                   expectedSet;
    /**< Indicates the next set interrupt expected from the DLM. This is
         used to check if DLM interrupts are occurring at proper sequence
         without any interrupt misses. */
    UInt32                   fid;
    /**< Indicates current field ID. Used in interlaced display. */
    VpsDdrv_GrpxQueueObj     *prgmedBuf[DC_NUM_DESC_SET];
    /**< Current programmed buffers. For progressive input this will be equal to
         the number of sets. For interlaced input this will be half of the same as
         fields are being programmed in each of the set. */
    VpsDdrv_GrpxQueueObj     *sPrgmedBuf;
    /**< Used in interlaced graphics input since frames (both feilds) are queued to
         the driver for interlaced display instead of one field at a time. */
    UInt32                   numPrgmedReg;
    /**< Number of frames programmed in the descriptors. For progressive mode
         this will be equal to DC_NUM_DESC_SET. For interlaced mode this
         will be equal to DC_NUM_DESC_SET/2. The other pointers in prgmedBuf
         are not used. */
    VpsUtils_QHandle         inQHandle;
    /**< Queue object to put the input requests. */
    VpsUtils_QHandle         outQHandle;
    /**< Queue object to put the processed output requests. */
    VpsDdrv_GrpxQueueObj     *fbQObj;
    /**< this is only valid when operated under frame buffer mode*/
} VpsDdrv_GrpxBufManObj;

/**
 *  struct VpsDdrv_GrpxInstObj
 *  \brief GRPX instance Object
 */
typedef struct
{
    UInt32                   drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource             resourceId;
    /**< Path resource ID. */
    Dc_NodeNum               dcNodeNum;
    /**< Node number for registering to display controller. */
    UInt32                   coreInstId;
    /**< Core instance number/ID. */
    const Vcore_Ops          *coreOps;
    /**< Core function pointers. */
    FVID2_DrvCbParams        fdmCbParams;
    /**< FVID2 driver manager callback function parameters. */
    VpsDdrv_GrpxStates       state;
    /**< Current state. */
    VpsDdrv_GrpxBufManObj    bmObj;
    /**< Buffer management object. */
    VpsUtils_QHandle         freeGrpxQ;
    /**< Queue for all free queue objects */
    UInt32                   memType;
    /**< Buffer memory type - tiled or non-tiled.
         For valid values see #Vps_VpdmaMemoryType. */
    Dc_PathInfo              dcPathInfo;
    /**< Display controller mode information */
    Dc_RtConfig              dcRtConfig;
    /**< Display controller runtime configuration. Dummy for GRPX*/
    Dc_ClientInfo            dcClientInfo;
    /**< Display controller client info. */
    Vcore_Handle             coreHandle;
    /**< Core handle. */
    DcClientHandle           dcHandle;
    /**< Display controller handle. */
    VpsDdrv_GrpxDescParams   descParams;
    /**< descriptor and virtual register params */
    UInt32                   configSet[DC_NUM_DESC_SET];
    /**< is Configuration descriptor set */
    UInt32                   clutSet[DC_NUM_DESC_SET];
    /**< is CLUT data descriptor set */
    VpsDdrv_GrpxDescDirty    descDirty[DC_NUM_DESC_SET];
    /**< flag whether descriptor is dirty or not */
    UInt32                   numRegions;
    /**< max region number per frame to support */
    Semaphore_Handle        instSem;
    /**< Semaphore to protect the open/close calls and other memory
         allocation per instance. */
    VpsDdrv_GrpxQueueObj    qObjPool[VPSDDRV_GRPX_MAX_QOBJS_PER_INST];
    /**< GRPX Display driver queue object pool */
    UInt32                  displayCount;
    /**< Counter to track how many frame been displayed since last start*/
    UInt32                  periodicCallbackEnable;
    /**< Periodic callback enable or not. */

} VpsDdrv_GrpxInstObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  vpsDdrvGrpxPrivInit
 *  \brief Display GRPX Driver Private Init function
 *  Initialize driver objects, allocats memory etc. This function is call
 *  by the VpsDdrv_GrpxInit.
 *
 *  \param numInst          [IN] Number of instance objects to be initialized.
 *  \param initPrms         [IN] Pointer to the instance parameter containing
 *                          instance specific information. If the number of
 *                          instance is greater than 1, then this pointer
 *                          should point to an array of init parameter
 *                          structure of size numInst.
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
Int32 vpsDdrvGrpxPrivInit(UInt32 numInst,
                          const VpsDdrv_GrpxInitParams *initPrms);
/**
 *  vpsDdrvGrpxPrivDeInit
 *  \brief Graphis driver private deinit function.
 *  Deallocates memory, objects allocated by init function.
 *
 *  \param arg              [IN] Not used currently. Meant for future purpose.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 vpsDdrvGrpxPrivDeInit(void);
/**
 *  vpsDdrvGrpxGetInstObj
 *  \brief  Get the GRPX instance Object based on the instance ID
 *
 *  \param instId          [IN] Instance to open - VPS_DISP_INST_GRPX0,
 *                          VPS_DISP_INST_GRPX1 or VPS_DISP_INST_GRPX2.
 *
 *  Returns the instance object pointer for the instance id.
 */
VpsDdrv_GrpxInstObj *vpsDdrvGrpxGetInstObj(UInt32 instId);
/**
 *  vpsDdrvGrpxSetCore
 *  \brief Configure the core based on both VENC settings and app input. This
 *  functin will recapture the current VENC mode information
 *
 *  \param   instObj         [IN] instance Objects
 *  \param   fmt             [IN]  FVID2_Format to set the core
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.

 */
UInt32 vpsDdrvGrpxSetCore(VpsDdrv_GrpxInstObj *instObj,
                          const FVID2_Format        *fmt);
/**
 *  vpsDdrvGrpxStop
 *  \brief Stop the GRPX streaming
 *
 *  \param   instObj         [IN] instance Objects
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.

 */
UInt32 vpsDdrvGrpxStop(VpsDdrv_GrpxInstObj *instObj);

/**
 *  vpsDrvCanCurBufBeDequeued
 *  \brief  current buffer can dequeue
 *
 *  \param  bmObj            [IN] buffer manager object assocated with the buffer
 *  \param  curSet           [IN] descriptor set to to be checked
 *
 *  Returns TRUE if the current set frame could be put in the done queue
 *  by checking if the frame is present in any of the buffer management states
 *  like other sets, semi programmed buffer etc... Else this returns FALSE.
 */

UInt32 vpsDdrvGrpxCanCurBufBeDequeued(const VpsDdrv_GrpxBufManObj *bmObj,
                                    UInt32 curSet);

/**
 *  vpsDdrvGrpxGetNextBufState
 *  \brief Depending on the availability of buffer in the input request queue
 *  and depending on progressive or interlaced mode of operation, this function
 *  determines what buffer should be programmed to the next descriptor set.
 *  If no reprogramming is required, then this returns NULL and the caller
 *  should not update the descriptor again.
 *
 *  \param  bmObj            [IN] buffer manager object assocated with the buffer
 *  \param  curSet           [IN] descriptor set currently been processed
 *  \param  nextSet          [IN] Next descriptor set tp be checked
 *  \param  curFid           [IN] Current Field ID
 *
 *  \return                  Return the buffer of NULL if no reprogramming is
 *                            required.
 */

VpsDdrv_GrpxQueueObj *vpsDdrvGrpxGetNextBufState(
                                         VpsDdrv_GrpxBufManObj *bmObj,
                                         UInt32 curSet,
                                         UInt32 nextSet,
                                         UInt32 curFid);
/**
 *  vpsDdrvRemoveBufFromState
 *  \Brief This removes frames from the buffer management state and accordingly
 *  clear the states of its occurrence. reverseOrder will determine in which
 *  order the buffers should be removed from the state. When stopping
 *  display, frames should be removed in reverse order so that the next start
 *  IOCTL will start displaying frames in the same sequence as the user
 *  has queued it initially. For all other operation (Dequeue) the order
 *  is normal.
 *
 *  \param  bmObj            [IN] buffer manager object assocated with the buffer
 *  \param  reverseOrder     [IN] the order to remove.
 *
 *  \return                  Retunrn the buffer object from Queue and return NULL if
 *                             buffer is available.
 */

VpsDdrv_GrpxQueueObj *vpsDdrvGrpxRemoveBufFromState(
                                         VpsDdrv_GrpxBufManObj *bmObj,
                                         UInt32 reverseOrder);
/**
 *  vpsDdrvGrpxAllocDescMem
 *  \brief This function allocates the descriptor memory based on the core's
 *  requirement and assign core descriptor pointers and finally program
 *  the descriptor by calling core function.
 *
 *  \param instObj       [IN] instance Object to allocate memroy
 *  \param  descParams   [OUT] structure to store the memroy
 *
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int vpsDdrvGrpxAllocDescMem(VpsDdrv_GrpxInstObj *instObj,
                            VpsDdrv_GrpxDescParams   *descParams);


/**
 *  vpsDdrvGrpxFreeDescMem
 *  \brief Frees the allocated core memory and resets the core descriptor
 *    pointers.
 *
 *  \param  instObj   [IN]  GRPX instance object to free memory
 *  \param  descParams [IN] points to memory to free
 *
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvGrpxFreeDescMem(VpsDdrv_GrpxInstObj *instObj,
                             VpsDdrv_GrpxDescParams  *descParams);
/**
 *  vpsDdrvGrpxConfigDctrl
 *  \brief Configures display controller and programs the register overlay.
 *
 *  \param  instObj      [IN]  GRPX instance Object
 *
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */

Int32 vpsDdrvGrpxConfigDctrl(VpsDdrv_GrpxInstObj *instObj);
/**
 *  vpsDdrvGrpxDeConfigDctrl
 *  \brief Clear the descriptor memory from display controller.
 *
 * \param   instObj     [IN]  GRPX instance Object
 *
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */

Int32 vpsDdrvGrpxDeConfigDctrl(VpsDdrv_GrpxInstObj *instObj);

/**
 *  vpsDdrvGrpxCanReCreateConfigDesc
 *  \brief whether it is able to update the coefficient and create the
 *  configuration descriptor
 *
 *  \param  instObj    [IN]  GRPX instance Object
 *  \param  frameList  [IN]  framelist to check
 *
 *  Returns TRUE on recreate else returns FALSE.
 */
UInt32  vpsDdrvGrpxCanReCreateConfigDesc(
                                 const VpsDdrv_GrpxInstObj *instObj,
                                 FVID2_FrameList *frameList);
/**
 *  vpsDdrvGrpxCheckFormat
 *  \brief Checks the format provided by the application.
 *
 *  \param  instObj   [IN]  GRPX instance Object
 *  \param  fmt       [IN]  point to FVID2_Format to check
 *
 *  Returns FVID2_SOK on success else returns error value.
 */
Int32 vpsDdrvGrpxCheckFormat(const VpsDdrv_GrpxInstObj *instObj,
                             const FVID2_Format *fmt);
/**
 *  vpsDdrvGrpxIsDuplicateQ
 *  \brief check whether the Q is duplicate or not
 *
 *  \param   instObj  [IN]  instance object to check
 *
 *   return TRUE if it is duplicate Q
 */

UInt32 vpsDdrvGrpxIsDuplicateQ(const VpsDdrv_GrpxInstObj*instObj);
/**
 *  vpsDdrvGrpxGetBppFromFmt
 *  \brief This function is to get the real bits per pixel from the format
 *
 *  \param dataFormat    [IN] data format of the buffer
 *  \param *bpp          [OUT] bits per pixel
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */

Int32  vpsDdrvGrpxGetBppFromFmt(UInt32 dataFormat, UInt32 *bpp);


#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_DISPGRPXPRIV_H */
