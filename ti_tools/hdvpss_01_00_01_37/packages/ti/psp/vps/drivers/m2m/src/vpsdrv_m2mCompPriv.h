/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mCompPriv.h
 *
 *  \brief VPS COMP M2M driver internal header file.
 */

#ifndef _VPSDRV_M2MCOMPPRIV_H
#define _VPSDRV_M2MCOMPPRIV_H

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
 *  \brief Maximum number of inputs for this driver.
 *  Note: Even though COMP HQ mode 1 driver takes two inputs from the user,
 *  still this is considered as a single input as only one core is used for
 *  both the inputs. Second input is just a side info for the first core.
 */
#define VPSMDRV_COMP_MAX_INPUTS          (1u)

/** \brief Maximum number of outputs for this driver. */
#define VPSMDRV_COMP_MAX_OUTPUTS         (2u)

/**
 *  \brief Maximum number of descriptor set supported by driver per channel.
 *  If all the sets are consumed subsequent requests will be placed in driver
 *  queue.
 *
 */
#define VPSMDRV_COMP_MAX_LAYOUTS         (4u) //Enough?
#define VPSMDRV_COMP_MAX_DESC_SET        (VPSMDRV_COMP_MAX_LAYOUTS)

/** \brief Maximum number of pending request per driver instance. */
#define VPSMDRV_COMP_MAX_QOBJS_PER_HANDLE    (10u)

/** \brief Default Sync Mode to be used. */
#ifdef VPS_CFG_USE_SYNC_ON_CH
#define VPSMDRV_COMP_DEFAULT_SYNC_MODE   (VPSMDRV_SYNC_MODE_SOCH)
#else
#define VPSMDRV_COMP_DEFAULT_SYNC_MODE   (VPSMDRV_SYNC_MODE_SOC)
#endif

/** \brief Time out to be used in sem pend. */
#define VPSMDRV_COMP_SEM_TIMEOUT         (BIOS_WAIT_FOREVER)

/** \brief Macro representing invalid descriptor set. */
#define VPSMDRV_COMP_INVALID_DESC_SET    (0xFFFFFFFF)

/** \brief Maximum number of object to collect statistics. */
#define VPSMDRV_COMP_MAX_TRACE_COUNT     (50u)

/** \brief Maximum number of coeff descriptors required by driver
 *  to configure the coefficient memory per channel. The coefficient
 *  configuration shall be done per channel in case config per channel is
 *  selected. One descriptor is used for horizontal, one for vertical, and one
 *  for bilinear scaling coefficients (if applicable).
 */
#define VPSMDRV_COMP_MAX_COEFF_CFG_DESC         (3u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Typedef for Comp M2M driver instance object. */
typedef struct VpsMdrv_CompInstObj_t VpsMdrv_CompInstObj;

/** \brief Typedef for COMP M2M driver handle object. */
typedef struct VpsMdrv_CompHandleObj_t VpsMdrv_CompHandleObj;

/**
 *  struct VpsMdrv_CompQueueObj
 *  \brief Structure defining the queue object used in queue/dequeue operation.
 *  Instead of creating process frame list objects, this is used so that
 *  any other information could be queued/dequeued along with the framelist.
 *  Already qElem is added to avoid dynamic allocation of Queue nodes.
 *  It will also hold the necessary MLM params structure instance to submit
 *  the request to the MLM.
 */
typedef struct
{
    VpsUtils_QElem                  qElem;
    /**< VPS utils queue element used in node addition. */
    VpsMdrv_CompHandleObj           *hObj;
    /**< Reference to the handle object for this queue object. */
    FVID2_ProcessList               procList;
    /**< FVID2 process frame list to store the incoming/outgoing IO packets. */
    VpsMdrv_QueObjType              qObjType;
    /**< Request type whether it is a frame request or coefficient request. */
    UInt32                          descSetInUse;
    /**< Descriptor set used by this queue object. */
    Mlm_SubmitReqInfo               mlmReqInfo;
    /**< MLM submit request info. */
} VpsMdrv_CompQueueObj;

/**
 *  struct VpsMdrv_CompLogObj
 *  \brief Log object to keep track of various error counts and important
 *  parameters.
 */
typedef struct
{
    UInt32                          totalLog;
    /**< Total number of log. */
    UInt32                          onexOneErrCnt;
    /**< Count to keep track of 1x1 frame size descriptors. */
    UInt32                          invFrmSzErrCnt;
    /**< Count to keep track of invalid frame size reported by VPDMA write
         descriptor. */
    UInt32                          skipCtxRotationErrCnt;
    /**< Count to keep trach of number of times invalid frame size reported
         by VPDMA was irrecoverable. When this happens for the next frame
         the previous fields won't be rotated. */
    UInt32                          curIdx;
    /**< Current index - Points to the next element to the last update in the
         below arrays. This will be used like a circular array index. */
    UInt32                          frmSizeTrace[VPSMDRV_COMP_MAX_TRACE_COUNT];
    /**< Trace of the last N frame size for Y and C as written out by VPDMA
         write descriptor. */
} VpsMdrv_CompLogObj;

/**
 *  struct VpsMdrv_CompChObj
 *  \brief Structure for storing the per channel information like pointers,
 *  number of descriptors etc.
 */
typedef struct
{
    UInt32                          chNum;
    /**< Channel Number. */
    Vps_M2mCompChParams              chPrms;
    /**< Configuration parameters for the channel. */

    Vps_GrpxCreateParams    grpxPrms;
    /**< Graphics parameters . */

    FVID2_Format            outFmtSc5;
    /**< Frame format for the output frame from COMP-Scalar5 output. */
    Vps_ScConfig            sc5Cfg;
    /**< scalar parameters like crop and scaler type for the
         write-back scalar: SC5 */
    Vps_CropConfig         sc5CropCfg;
    /**< Cropping configuration for the write-back scalar: SC5 */

    Void                           *descMem;
    /**< Points to the descriptor memory containing the data, config,
         coeff and reload descriptors for all the sets. */
    Void                           *shwOvlyMem;
    /**< Points to the shadow overlay memory of the channel for all the sets. */
    UInt32                          totalDescMem;
    /**< Total descriptor memory in bytes used for all the cores. */
    UInt32                          totalShwOvlyMem;
    /**< Total shadow overlay memory in bytes used for all the cores. */
    UInt32                          numCfgDesc;
    /**< Number of config descriptors required for channel.
         If configuration is selected per channel this will be set to one and
         memory for the config descriptor will be allocated as the part of
         the channel object.
         If configuration is selected per handle this will be set to zero and
         memory for the config descriptor will be allocated as the part of
         the handle object. */

    UInt32                           horzCoeffOvlySize[VPSMDRV_COMP_MAX_CORE];
    /**< Size (in bytes) of the coefficient overlay memory required to program
         horizontal scaling coefficients. This is required by cores which have
         scalar in their paths.
         If configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */
    UInt32                           vertCoeffOvlySize[VPSMDRV_COMP_MAX_CORE];
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical scaling coefficients. This is required by cores which have
         scalar in their paths.
         If configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */
    UInt32                     vertBilinearCoeffOvlySize[VPSMDRV_COMP_MAX_CORE];
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical bilinear scaling coefficients. This is required by cores which
         have HQ scalar in their paths.
         If configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */
    VpsHal_VpdmaConfigDest          coeffConfigDest[VPSMDRV_COMP_MAX_CORE];
    /**< Config destination ID used to program the scalar coefficients. */
    UInt32                          scPresent[VPSMDRV_COMP_MAX_CORE];
    /**< Flag to indicate whether scalar is present in a core or not.
         This is used to identify the scalar to be programmed in set coeff
         IOCTL. */

    UInt32                          cbcrOffset;
    /**< C buffer offset in the field buffer in YUV422 semi planar format. */
    UInt32                          isDeiCtxBufSet;
    /**< Flag to indicate whether the COMP buffers are set by application or
         allocated by driver. */
    UInt32                          fldIdx;
    /**< Field buffer index pointing to the N-1 field input. This is used for
         rotation of COMP context buffers. */
    UInt32                          mvIdx;
    /**< MV buffer index pointing to the N-1 MV input. This is used for
         rotation of COMP context buffers. */
    UInt32                          mvstmIdx;
    /**< MVSTM buffer index pointing to the N-1 MVSTM input. This is used for
         rotation of COMP context buffers. */
    UInt32                          prevFldState;
    /**< At the start up, COMP should operate in line-average mode for
         the few fields. This variable keeps track of number of frame
         submitted to the COMP. It will be initialized to zero at the
         start up or at the time of resetting COMP context. */
    UInt32                          dontRotateCtxBuf;
    /**< Flag to indicate whether the context buffers needs to be rotated.
         This flag will be set when the compressed buffer size reported by
         VPDMA is invalid and in the next frame submission for the channel,
         the invalid compressed frame size is not used by not rotating
         the context buffers. */

    Void                           *firstDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Pointer to the first descriptor in the descriptor layout for the
         channel. */
    VpsHal_VpdmaConfigDesc         *shwCfgDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Points to the shadow config descriptor of the channel. */
    VpsHal_VpdmaSyncOnClientDesc   *socDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Points to the SOC control descriptor of the channel. */
    VpsHal_VpdmaReloadDesc         *rldDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Points to the reload descriptor of the channel. */
    Vcore_DescMem                   coreDescMem[VPSMDRV_COMP_MAX_DESC_SET]
                                               [VPSMDRV_COMP_MAX_CORE];
    /**< Structure holding the data descriptor, overlay memory references
         for all the cores and for all the sets. */
    UInt32                          isDescDirty[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Flag to indicate whether a descriptor is dirty or not. */
    VpsMdrv_CompLogObj               logObj;
    /** Log object per channel. */
} VpsMdrv_CompChObj;

/**
 *  struct VpsMdrv_CompHandleObj_t
 *  \brief Structure for storing the per handle information.
 *  This structure represents the each handle of the driver instance.
 *  It will encapsulate the channel object instances equal to the maximum number
 *  of channels supported by handle and the handle specific elements like
 *  non shadow config descriptor and memory and coeff descriptor and memory.*/
struct VpsMdrv_CompHandleObj_t
{
    VpsMdrv_CompInstObj             *parent;
    /**< Instance object to which this handle belongs to. */
    VpsMdrv_CompChObj               *chObjs[VPS_M2M_COMP_MAX_CH_PER_INST];
    /**< Pointer to channel objects for this handle */

    UInt32                          mode;
    /**< Mode to indicate whether configuration is required per handle or
         per channel. For valid values see #Vps_M2mMode. */
    UInt32                          numCh;
    /**< Number of channels to be supported by handle. */
    UInt32                          deiHqCtxMode;
    /**< Determines the mode in which the COMP HQ context buffers are handled
         by driver and application - all context buffers maintaned by driver,
         or N-1 context buffer provided by application etc...
         For valid values see #Vps_DeiHqCtxMode. */
    FVID2_DrvCbParams               fdmCbPrms;
    /**< FVID2 driver manager callback function parameters. */

    VpsUtils_QHandle                reqQ;
    /**< Queue object to put the input requests. */
    VpsUtils_Handle                 doneQ;
    /**< Queue object to put the processed output requests. */
    VpsUtils_QHandle                freeQ;
    /**< Queue for queueing all the free queue objects for this handle. */
    Semaphore_Handle                ctrlSem;
    /**< Semaphore to block the control command until they are finished.
         This will typically used for programming coefficients where the
         coefficient update request is submitted to MLM and control ioctl is
         waiting for the coefficient request to be completed. */
    UInt32                          numPendReq;
    /**< Number of pending requests to be dequeued by application once
         submitted to driver. */

    UInt32                          numUnProcessedReq;
    /**< Number of requests yet to be processed by driver. */

    Void                           *descMem;
    /**< Points to the descriptor memory containing the config, coeff and
         reload descriptors for the handle. */
    Void                           *shwOvlyMem;
    /**< Points to the shadow overlay memory of the handle used when
         per handle config is TRUE. */
    Void                           *nshwOvlyMem;
    /**< Points to the non-shadow overlay memory of the handle. */
    UInt32                          numCfgDesc;
    /**< Number of config descriptors required for handle.
         If configuration is selected per channel this will be set to zero and
         memory for the config descriptor will be allocated as the part of
         the channel object.
         If configuration is selected per handle this will be set to one and
         memory for the config descriptor will be allocated as the part of
         the handle object. */
    UInt32                          totalDescMem;
    /**< Total descriptor memory in bytes used for per handle descriptors. */
    UInt32                          totalShwOvlyMem;
    /**< Total shadow overlay memory in bytes used for all the cores. */
    UInt32                          totalNshwOvlyMem;
    /**< Total non-shadow overlay memory in bytes used for all the cores. */

    UInt32                          isDescSetFree[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Flag to indicate whether the descriptor set is free or not. */

    Void                           *firstDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Pointer to the first descriptor in the descriptor layout for the
         handle. */
    VpsHal_VpdmaConfigDesc         *shwCfgDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Points to the shadow config descriptor of the handle. */
    VpsHal_VpdmaConfigDesc         *nshwCfgDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Points to the non shadow config descriptor of the handle. */
    VpsHal_VpdmaReloadDesc         *rldDesc[VPSMDRV_COMP_MAX_DESC_SET];
    /**< Points to the reload descriptor of the handle. */

    Vcore_Handle                    coreHandle[VPSMDRV_COMP_MAX_CORE];
    /**< Core handles for Input, output and process cores. */
    Bool                            isVipScReq;
    /**< Flag to indicate whether VIP scalar is required for this
         handle or not */

    VpsHal_VpdmaChannel             socChNum[VCORE_MAX_VPDMA_CH];
    /**< Client on which MLM should wait for the end of frame signal. */
    UInt32                          numVpdmaChannels;
    /**< Size of socChNum array. */
    UInt32                          socIdx;
    /**< Index into socChNum array to get the VPDMA channel
         number for programming Sync on Client control descriptor. */
    UInt32                          syncMode;
    /**< Flag to indicate whether sync on client or sync on channel to be
         used between each channel descriptors while chaining.
         For valid values see #VpsMdrv_SyncMode. */
};

/** \brief Driver strcuture per instance of driver */
struct VpsMdrv_CompInstObj_t
{
    UInt32                          openCnt;
    /**< Open count. */
    UInt32                          initDone;
    /**< Flag to indicate intialization is done or not for an instance. */
    UInt32                          drvInstId;
    /**< Driver instance ID. */

    Vrm_Resource                    resrcId[VPSMDRV_COMP_MAX_CORE];
    /**< BP0/1, DEI/ DEI HQ, SWP, VIP, COMP core resource ID. */
    UInt32                          coreInstId[VPSMDRV_COMP_MAX_CORE];
    /**< BP0/1, DEI/ DEI HQ, SWP, VIP, COMP core instance number/ID. */
    const Vcore_Ops                *coreOps[VPSMDRV_COMP_MAX_CORE];
    /**< BP0/1, DEI/ DEI HQ, SWP, VIP, COMP core function pointers. */

    UInt32                          isCoreReq[VPSMDRV_COMP_MAX_CORE];
    /**< Flag to indicate whether a core has to be included for a particular
         instance or not. */
    UInt32                          numInListReq;
    /**< Number of input frame list required for an instance.
         Currently this will be 1 for COMP input core. */
    UInt32                          numOutListReq;
    /**< Number of output frame list required for an instance.
         Currently this will be either 1 or 2 for SC5 WRBK and/or VIP cores. */

    Semaphore_Handle                instSem;
    /**< Semaphore to protect the open/close calls. */

    Mlm_Handle                      mlmHandle;
    /**< MLM handle. */
    Mlm_ClientInfo                  mlmInfo;
    /**< MLM client information for registering to MLM. */
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 vpsMdrvCompInit(UInt32 numInst,
                     const VpsMdrv_CompInitParams *initPrms);
Int32 vpsMdrvCompDeInit(void);

VpsMdrv_CompInstObj *vpsMdrvCompGetInstObj(UInt32 instId);
Int32 vpsMdrvCompAllocResrc(VpsMdrv_CompInstObj *instObj);
Int32 vpsMdrvCompFreeResrc(VpsMdrv_CompInstObj *instObj);

VpsMdrv_CompHandleObj *vpsMdrvCompAllocInitHandleObj(
                          Vps_M2mCompCreateParams *createPrms,
                          VpsMdrv_CompInstObj *instObj,
                          const FVID2_DrvCbParams *fdmCbPrms,
                          Int32 *retValPtr);
Int32 vpsMdrvCompFreeHandleObj(VpsMdrv_CompHandleObj *hObj);

Int32 vpsMdrvCompOpenCores(VpsMdrv_CompHandleObj *hObj);
Int32 vpsMdrvCompCloseCores(VpsMdrv_CompHandleObj *hObj);

Int32 vpsMdrvCompAllocCoreMem(VpsMdrv_CompHandleObj *hObj);
Int32 vpsMdrvCompFreeCoreMem(VpsMdrv_CompHandleObj *hObj);

Int32 vpsMdrvCompProgramDesc(VpsMdrv_CompHandleObj *hObj);
Int32 vpsMdrvCompGetSocCh(VpsMdrv_CompHandleObj *hObj);

Void *vpsMdrvCompAllocCoeffDesc(void);
Int32 vpsMdrvCompFreeCoeffDesc(Void *coeffDesc);

Int32 vpsMdrvCompHqRdAdvCfgIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvCompHqWrAdvCfgIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvCompRdAdvCfgIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvCompWrAdvCfgIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvCompScRdAdvCfgIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvCompScWrAdvCfgIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);

#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_M2MCOMPPRIV_H */
