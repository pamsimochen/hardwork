/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
 *  \file vpsdrv_m2mScPriv.h
 *
 *  \brief VPS SC M2M driver internal header file.
 *
 */

/* Non Shadow and shadow config descriptors are updated by driver.
 * Non Shadow and Shadow overlay memories are updated by respective cores.
 * Data descriptors are updated by respective cores.
 * Sync on channel and Send interrupt is added by the MLM to wait for the
 * request to get complete and trigger interrupt once the request gets
 * complete. Below layout is shown for per channel configuration
 *
 *    |------------|      |------------|
 *    |Non Shadow  |----->|Non Shadow  |
 *    |Config Desc |      |Ovly memory |
 *    |------------|      |------------|
 *    |Reload desc |
 *    |------------|
 *          |
 *          |
 *          |------>|------------|    |-----------------------------|
 *                  |ch 1        |--->|Overlay memory for           |
 *                  |coeff desc  |    |core 1 and core 2: This is in|
 *                  |horizontal  |    |default coeffs memory in HAL |
 *                  |------------|    |-----------------------------|
 *                  |ch 1        |--->|Overlay memory for           |
 *                  |coeff desc  |    |core 1 and core 2: This is in|
 *                  |vertical    |    |default coeffs memory in HAL |
 *                  |------------|    |-----------------------------|
 *                  |            |    |------------------|
 *                  |ch 1 shadow |--->|Shadow overlay    |
 *                  |config desc |    |memory for core 1 |
 *                  |------------|    |and core 2        |
 *                  |ch1 out desc|    |------------------|
 *                  |------------|
 *                  |ch1 in0 desc|
 *                  |------------|
 *                  |ch1 in1 desc|
 *                  |------------|
 *                  |ch1 SOCH    |
 *                  |desc        |
 *                  |------------|
 *                  |ch1 rld desc|
 *                  --------------
 *                        |
 *                        |
 *                        |---->|------------|   |-----------------------------|
 *                              |ch 2        |-->|Overlay memory for           |
 *                              |coeff desc  |   |core 1 and core 2: This is in|
 *                              |horizontal  |   |default coeffs memory in HAL |
 *                              |------------|   |-----------------------------|
 *                              |ch 2        |-->|Overlay memory for           |
 *                              |coeff desc  |   |core 1 and core 2: This is in|
 *                              |vertical    |   |default coeffs memory in HAL |
 *                              |------------|   |-----------------------------|
 *                              |            |   |------------------|
 *                              |ch 2 shadow |-->|Shadow overlay    |
 *                              |config desc |   |memory for core 1 |
 *                              |------------|   |and core 2        |
 *                              |ch2 out desc|   |------------------|
 *                              |------------|
 *                              |ch2 in0 desc|
 *                              |------------|
 *                              |ch2 in1 desc|
 *                              |------------|
 *                              |ch2 SOCH    |
 *                              |desc        |
 *                              |------------|
 *                              |ch2 rld desc|
 *                              --------------
 *                                    |
 *                                    |
 *                                    |--------|-----------------|
 *                                             |Send Interrupt   |
 *                                             |-----------------|
 */
/* Following is the descriptor layout for the per handle configuration. Here
 * the config descriptor is applied only once for the entire request.
 *    |------------|       |------------|
 *    |Non Shadow  |-----> |Non Shadow  |
 *    |Config Desc |       |Ovly memory |
 *    |            |       |------------|
 *    |            |
 *    |------------|       |-----------------------------|
 *    |            |-----> |Overlay memory for           |
 *    |coeff desc  |       |core 1 and core 2: This is in|
 *    |horizontal  |       |default coeffs memory in HAL |
 *    |------------|       |-----------------------------|
 *    |            |-----> |Overlay memory for           |
 *    |coeff desc  |       |core 1 and core 2: This is in|
 *    |vertical    |       |default coeffs memory in HAL |
 *    |------------|       |-----------------------------|
 *    |            |-----> |------------------|
 *    |Shadow      |       |Shadow overlay    |
 *    |Config Desc |       |memory for core 1 |
 *    |----------- |       |and core 2        |
 *    |Reload desc |       |------------------|
 *    |------------|
 *          |
 *          |------>|------------|
 *                  |ch1 out desc|
 *                  |------------|
 *                  |ch1 in0 desc|
 *                  |------------|
 *                  |ch1 in1 desc|
 *                  |------------|
 *                  |ch1 SOCH    |
 *                  |desc        |
 *                  |------------|
 *                  |ch1 rld desc|
 *                  --------------
 *                        |
 *                        |
 *                        |---->|------------|
 *                              |ch2 out desc|
 *                              |------------|
 *                              |ch2 in0 desc|
 *                              |------------|
 *                              |ch2 in1 desc|
 *                              |------------|
 *                              |ch2 SOCH    |
 *                              |desc        |
 *                              |------------|
 *                              |ch2 rld desc|
 *                              --------------
 *                                    |
 *                                    |
 *                                    |--------|-----------------|
 *                                             |Send Interrupt   |
 *                                             |-----------------|
 */

#ifndef _VPSDRV_M2MSCPRIV_H
#define _VPSDRV_M2MSCPRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/hal/vpshal_sc.h>

/* None */
#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of effective driver instance - one BP0-SC5/BP1-SC5,
 *  one SEC0-VIP0 (SC3) and one SEC1-VIP1 (SC4). When SEC0-SC5 instance is
 *  opened, only SEC1-VIP1 (SC4) could be opened.
 *  paths. All driver instance can't be opened at a time.
 *  Hence for memory allocation like descriptors, channels etc this macro
 *  is used instead of VPS_M2M_SC_INST_MAX which could be as many as different
 *  possible drivers for the same paths.
 */
#define VPSMDRV_SC_MAX_EFF_INST         (3u)

/**
 *  \brief Maximum number of queue objects required to store the incoming
 *  requests per instance. After the queue objects are full. Driver will
 *  not queue further requests and will return error to application.
 *  Queue objects pool is shared between all the handles of instance.
 */
#define VPSMDRV_SC_MAX_Q_OBJS           (VPS_M2M_SC_MAX_CH)

/**
 *  \brief Maximum number of descriptor sets supported by driver
 *  per channel currently ping and pong. If all the sets are consumed subsequent
 *  requests will be placed in driver queue.
 */
#define VPSMDRV_SC_MAX_DESC_SET         (2u)

/** \brief Maximum number of inputs for this driver.  */
#define VPSMDRV_SC_MAX_INPUTS           (1u)

/** \brief Maximum number of outputs for this driver. */
#define VPSMDRV_SC_MAX_OUTPUTS          (1u)

/**
 *  \brief Maximum number of inbound data descriptor required by driver
 *  per channel.
 *  1 for Chroma client and 1 for Luma client.
 */
#define VPSMDRV_SC_MAX_IN_DESC          (2u)

/**
 *  \brief Maximum number of outbound data descriptor required by driver
 *  per channel.
 *  1 for Chroma client and 1 for Luma client.
 */
#define VPSMDRV_SC_MAX_OUT_DESC         (2u)

/** \brief Total number of data descriptors required per channel. */
#define VPSMDRV_SC_TOTAL_DATA_DESC      (VPSMDRV_SC_MAX_IN_DESC +              \
                                                    VPSMDRV_SC_MAX_OUT_DESC)

/**
 *  \brief Maximum number of shadow config descriptor required by driver
 *  to configure shadow memory per channel or per handle.
 */
#define VPSMDRV_SC_MAX_SHW_CFG_DESC     (1u)

/**
 *  \brief Maximum number of non-shadow config descriptor required by driver
 *  to configure non-shadow memory per channel or per handle.
 */
#define VPSMDRV_SC_MAX_NSHW_CFG_DESC    (1u)

/**
 *  \brief Maximum number of Reload descriptors required by driver
 *  to link multiple channels for each request.
 */
#define VPSMDRV_SC_MAX_RLD_DESC         (1u)

/** \brief Maximum number of SOCH descriptors needed. */
#define VPSMDRV_SC_MAX_SOCH_DESC        (VPSMDRV_SC_TOTAL_DATA_DESC)

/**
 *  \brief Maximum number of coeff descriptors required by driver
 *  to configure the coefficient memory per channel. The coefficient
 *  configuration shall be done per channel in case config per channel is
 *  selected. One descriptor is used for horizontal, one for vertical, and one
 *  for bilinear scaling coefficients (if applicable).
 */
#define VPSMDRV_SC_MAX_COEFF_CFG_DESC   (3u)

/**
 *  \brief Maximum number of coeff descriptors required by driver
 *  at runtime to configure the coefficient memory per channel. Since vertical
 *  bilinear coefficients are set (if applicable) only once in the beginning, at
 *  runtime only horizontal and vertical coefficient descriptors are required.
 */
#define VPSMDRV_SC_MAX_RT_COEFF_CFG_DESC    (2u)

#define VPSMDRV_SC_TOTAL_DESC_MEM       ((VPSMDRV_SC_MAX_COEFF_CFG_DESC *      \
                                            VPSHAL_VPDMA_CONFIG_DESC_SIZE) +   \
                                        (VPSMDRV_SC_MAX_NSHW_CFG_DESC *        \
                                            VPSHAL_VPDMA_CONFIG_DESC_SIZE) +   \
                                        (VPSMDRV_SC_MAX_SHW_CFG_DESC *         \
                                            VPSHAL_VPDMA_CONFIG_DESC_SIZE) +   \
                                        (VPSMDRV_SC_TOTAL_DATA_DESC *          \
                                            VPSHAL_VPDMA_DATA_DESC_SIZE) +     \
                                        (VPSMDRV_SC_MAX_SOCH_DESC *            \
                                            VPSHAL_VPDMA_CTRL_DESC_SIZE) +     \
                                        (VPSMDRV_SC_MAX_RLD_DESC *             \
                                            VPSHAL_VPDMA_CTRL_DESC_SIZE))

/**
 *  \brief Total descriptor memory required per channel considering ping and
 *  pong desc set. Multiplication by two is for the scratch pad memory.
 */
#define VPSMDRV_SC_MAX_DESC_MEM_PER_CH  (VPSMDRV_SC_TOTAL_DESC_MEM *           \
                                                VPSMDRV_SC_MAX_DESC_SET * 2u)

/**
 *  \brief Shadow config overlay memory (in bytes) needed by driver for
 *  programming shadow registers for each channel * number of sets
 *  per channel (2 for ping/pong, no scratch memory as same overlay memory
 *  is used for both channel and scratch pad channel).
 *  Caution: Make this a multiple of VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN so
 *  that each memory pool element will also be aligned to this alignment
 *  when defining the array for this pool.
 *  48  - for programming CHR_US
 *  208 - for programming SC
 */
#define VPSMDRV_SC_MAX_SHW_OVLY_MEM     ((48u + 208u) * VPSMDRV_SC_MAX_DESC_SET)

/**
 *  \brief Non shadow config overlay memory (in bytes) needed by driver for
 *  programming non shadow registers for each channel * number of sets
 *  per channel (2 for ping/pong, no scratch memory as same overlay memory
 *  is used for both channel and scratch pad channel).
 *  Caution: Make this a multiple of VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN so
 *  that each memory pool element will also be aligned to this alignment
 *  when defining the array for this pool.
 *  64  - for programming frame start event of input core
 *  320 - for programming VIP muxes in case of VIP core
 */
#define VPSMDRV_SC_MAX_NSHW_OVLY_MEM    ((64u + 320u) * VPSMDRV_SC_MAX_DESC_SET)

/**
 *  \brief Total coefficient descriptor memory required for each instance
 *  to configure the coefficient memory each time ioctl is fired to update
 *  the coefficients.
 */
#define VPSMDRV_SC_MAX_COEFF_DESC_MEM   ((VPSMDRV_SC_MAX_COEFF_CFG_DESC *      \
                                            VPSHAL_VPDMA_CONFIG_DESC_SIZE)     \
                                        + VPSHAL_VPDMA_CTRL_DESC_SIZE)

/** \brief Non shadow descriptor memory. */
#define VPSMDRV_SC_NSHW_DESC_MEM        ((((VPSMDRV_SC_MAX_COEFF_CFG_DESC      \
                                              + VPSMDRV_SC_MAX_NSHW_CFG_DESC   \
                                              + VPSMDRV_SC_MAX_SHW_CFG_DESC)   \
                                            * VPSHAL_VPDMA_CONFIG_DESC_SIZE)   \
                                           + VPSHAL_VPDMA_CTRL_DESC_SIZE)      \
                                          * VPSMDRV_SC_MAX_DESC_SET)

/**
 *  \brief Maximum number of channel memory pool to allocate.
 *  Maximum number of instance * maximum number of channels per instance.
 */
#define VPSMDRV_SC_MAX_CH_MEM_POOL      (VPSMDRV_SC_MAX_EFF_INST               \
                                       * VPS_M2M_SC_MAX_CH)

/**
 *  \brief Descriptor set to use. Normal is used for the first frame
 *  of channel in a request.
 */
#define VPSMDRV_SC_NORMAL_DESC_SET      (0u)

/**
 *  \brief Scratch pad descriptor set is used for the subsequent frames
 *  from the same channel in same request.
 */
#define VPSMDRV_SC_SCRATCH_PAD_DESC_SET (1u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Forward declaration for SC instance object. */
typedef struct VpsMdrv_ScInstObj_t VpsMdrv_ScInstObj;

/** \brief Forward declaration for SC handle object. */
typedef struct VpsMdrv_ScHandleObj_t VpsMdrv_ScHandleObj;

/**
 *  struct VpsMdrv_ScQueueObj
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
    VpsMdrv_ScHandleObj            *hObj;
    /**< Reference to the handle object for this queue object. */
    FVID2_ProcessList               procList;
    /**< FVID2 process frame list to store the incoming/outgoing IO packets. */
    VpsMdrv_QueObjType              qObjType;
    /**< Request type whether it is a frame request or coefficient request. */
    Int32                           descSetInUse;
    /**< Descriptor set used by this queue object. */
    Mlm_SubmitReqInfo               mlmReqInfo;
    /**< MLM submit request info. */
} VpsMdrv_ScQueueObj;

/**
 *  struct VpsMdrv_ScChParams
 *  \brief Structure containing the channel params. This is similar to the
 *  chParams structure of the applications. Only difference being here the
 *  instances (memory) are taken.
 */
typedef struct
{
    Vps_M2mScChParams               chParams;
    /**< Strucuture containing the channel params. */
    Vps_ScConfig                    scCfg;
    /**< Instance of scalar config. Pointer to scalar config from channel params
         will point to this scalar config. */
    Vps_CropConfig                  srcCropCfg;
    /**< Instance of crop config. Pointer to crop confing from channel params
         will point to this crop config. */
    Vps_SubFrameParams              subFrameParams;
    /**< SubFrame mode configuration paramters set by application. */
} VpsMdrv_ScChParams;


/**
 *  struct VpsMdrv_ScChObj
 *  \brief Structure for storing the per channel information like configuration,
 *  descriptors pointers, number of descriptors etc.
 */
typedef struct
{
    UInt32                          isAllocated;
    /**< Flag to mark whether the channel object is allocated or not. */

    Vcore_DescMem                   coreDescMem[VPSMDRV_SC_MAX_DESC_SET]
                                               [VPSMDRV_SC_MAX_CORE];
    /**< Structure holding the data descriptor, overlay memory references
         for all the cores and for all the sets. */

    VpsHal_VpdmaConfigDesc         *coeffDesc
                                        [VPSMDRV_SC_MAX_DESC_SET]
                                        [VPSMDRV_SC_MAX_RT_COEFF_CFG_DESC];
    /**< Points to the coefficient descriptor pointer. */
    VpsHal_VpdmaConfigDesc         *nshwCfgDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the non shadow config descriptor of the handle. */
    VpsHal_VpdmaConfigDesc         *shwCfgDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the shadow config descriptor of the channel. */
    VpsHal_VpdmaSyncOnChannelDesc  *sochDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the SOCH control descriptor of the channel. */
    VpsHal_VpdmaReloadDesc         *rldDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the reload descriptor of the channel. */

    Void                           *descMem;
    /**< Points to the descriptor memory containing the data, config,
         coeff and reload descriptors for all the sets.
         This is used for alloc/free call. */
    Void                           *nshwOvlyMem;
    /**< Points to the non-shadow overlay memory of the channel for all the
         sets. This is used for alloc/free call. */
    Void                           *shwOvlyMem;
    /**< Points to the shadow overlay memory of the channel for all the sets.
         This is used for alloc/free call. */

    UInt32                          numInDataDesc;
    /**< Number of inbound data descriptors required for the channel. If
         configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */
    UInt32                          numOutDataDesc;
    /**< Number of outbound data descriptors required for the channel. */
    UInt32                          numCoeffDesc;
    /**< Number of coefficient descriptors required for the channel. If
         configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */
    UInt32                          numSochDesc;
    /**< Number of Sync on client control descrsecor requried. */
    UInt32                          sochChNum[VPSMDRV_SC_MAX_SOCH_DESC];
    /**< Channel number for each of the sync on channel descriptors. */

    UInt32                          totalCoeffDescMem;
    /**< Total coefficient descriptor memory in bytes. */
    UInt32                          totalNshwOvlyMem[VPSMDRV_SC_MAX_CORE];
    /**< Non-shadow overlay memory size required for both cores. If
         configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */
    UInt32                          totalShwOvlyMem[VPSMDRV_SC_MAX_CORE];
    /**< Shadow overlay size required for cores. If
         configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field. */

    UInt32                          horzCoeffOvlySize;
    /**< Size (in bytes) of the coefficient overlay memory required to program
         horizontal scaling coefficients. This is required by cores which have
         scalar in their paths - Assuming only one scalar supported. */
    UInt32                          vertCoeffOvlySize;
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical scaling coefficients. This is required by cores which have
         scalar in their paths - Assuming only one scalar supported. */
    UInt32                          vertBilinearCoeffOvlySize;
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical bilinear scaling coefficients. This is required by cores which
         have HQ scalar in their paths - Assuming only one scalar supported. */
    Ptr                             horzCoeffMemPtr;
    /**< Pointer to the coefficient overlay memory required to program
         horizontal scaling coefficients. This is required by cores which have
         scalar in their paths - Assuming only one scalar supported. */
    Ptr                             vertCoeffMemPtr;
    /**< Pointer to the coefficient overlay memory required to program
         vertical scaling coefficients. This is required by cores which have
         scalar in their paths - Assuming only one scalar supported. */
    VpsHal_VpdmaConfigDest          coeffConfigDest;
    /**< Config destination for the both cores. If
         configuration is selected per handle Only the first channel of the
         handle object will contain valid information for this field.
         Assuming only one scalar supported. */

    Void                           *firstDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Pointer to the first descriptor in the descriptor layout for the
         channel. */
    UInt32                          descListSize;
    /**< Desciptor list size to be posted to VPDMA LM which includes the
         data, config, sync on channel and reload descritptor. */

    UInt32                          isDescDirty[VPSMDRV_SC_MAX_DESC_SET];
    /**< Flag to indicate whether a descriptor is dirty or not. */

    VpsMdrv_SubFrameChInst          slcChObj;
    /**< Instance memory for subframe based porcessing. */
    VpsHal_ScFactorConfig           scFactor;
    /**< Scaling factor for this channel based on required scaling ratio for
         this channel. */
    Vps_SubFrameHorzSubFrmInfo      hSlcInfo;
} VpsMdrv_ScChObj;

/**
 *  struct VpsMdrv_ScHandleObj_t
 *  \brief Structure for storing the per handle information.
 *  This structure represents the each handle of the driver instance.
 *  It will encapsulate the channel object instances equal to the maximum number
 *  of channels supported by handle and the handle specific elements like
 *  non shadow config descriptor and memory and coeff descriptor and memory.
 */
struct VpsMdrv_ScHandleObj_t
{
    VpsMdrv_ScInstObj              *instObj;
    /**< Instance object to which this handle belongs to. */
    VpsMdrv_ScChObj                *chObjs[VPS_M2M_SC_MAX_CH];
    /**< Pointer to channel objects for this handle. */
    VpsMdrv_ScChObj                *sPadChObjs[VPS_M2M_SC_MAX_CH];
    /**< Pointer to hold the information for scratch pad descriptors. These
     * descriptors are not tied to any channels. */
    Vps_M2mScChParams              *chParams[VPS_M2M_SC_MAX_CH];
    /**< Pointer to configuration parameters for each channel.  If the
         configuration is per handle only first object makes sense. */

    UInt32                          mode;
    /**< Mode to indicate whether configuration is required per handle or
         per channel. For valid values see #Vps_M2mMode. */
    UInt32                          numCh;
    /**< Number of channels to be supported by handle. */
    FVID2_DrvCbParams               fdmCbPrms;
    /**< FVID2 driver manager callback function parameters. */

    VpsUtils_QHandle                reqQ;
    /**< Queue object to put the input requests. */
    VpsUtils_Handle                 doneQ;
    /**< Queue object to put the processed output requests. */
    Semaphore_Handle                ctrlSem;
    /**< Semaphore to block the control command until they are finished.
         This will typically used for programming coefficients where the
         coefficient update request is submitted to MLM and control ioctl is
         waiting for the coefficient request to be completed. */
    UInt32                          numPendReq;
    /**< Number of pending requests to be dequeued by application once
         submitted to driver. */

    Vcore_Handle                    inCoreHandle;
    /**< Handle to the input core. */
    Vcore_Handle                    outCoreHandle;
    /**< Handle to output core. */

    UInt32                          isDescSetFree[VPSMDRV_SC_MAX_DESC_SET];
    /**< Flag to indicate whether the descriptor set is free or not. */

    UInt32                          firstDescListSize;
    /**< Size of the first descriptor list. */

    VpsHal_VpdmaConfigDesc         *ioctlCoeffDesc;
    /**< To store the coefficient descriptor pointer during the coefficient
         request submission ioctl and free it once the call back arrives. */

    Void                           *descMem;
    /**< Points to the descriptor memory containing the data, config,
         coeff and reload descriptors for all the sets.
         This is used for alloc/free call. */
    Void                           *nshwOvlyMem;
    /**< Points to the non-shadow overlay memory of the channel for all the
         sets. This is used for alloc/free call. */
    Void                           *shwOvlyMem;
    /**< Points to the shadow overlay memory of the channel for all the sets.
         This is used for alloc/free call. */

    Void                           *firstDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Pointer to the first descriptor in the descriptor layout for the
         handle. */
    VpsHal_VpdmaConfigDesc         *coeffDesc
                                        [VPSMDRV_SC_MAX_DESC_SET]
                                        [VPSMDRV_SC_MAX_RT_COEFF_CFG_DESC];
    /**< Points to the coefficient descriptor pointer of the handle. */
    VpsHal_VpdmaConfigDesc         *nshwCfgDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the non shadow config descriptor of the handle. */
    VpsHal_VpdmaConfigDesc         *shwCfgDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the shadow config descriptor of the handle. */
    VpsHal_VpdmaReloadDesc         *rldDesc[VPSMDRV_SC_MAX_DESC_SET];
    /**< Points to the reload descriptor of the handle. */

    UInt32                          chCnt
                                        [VPSMDRV_SC_MAX_DESC_SET]
                                        [VPS_M2M_SC_MAX_CH];
    /**< Counter to indicate how many frames for each channel is there in the
         frame list. */
    UInt32                          sPadChIsAllocated
                                        [VPSMDRV_SC_MAX_DESC_SET]
                                        [VPS_M2M_SC_MAX_CH];
    /**< Flag to indicate that the descriptor is already allocatd for the
         frames in the framelist. */
    UInt32                          isAllocated;
    /**< Flag to indicate whether the handle object is free or in use.*/
};

/** \brief Driver structure per instance of driver */
struct VpsMdrv_ScInstObj_t
{
    UInt32                          openCnt;
    /**< Open count. */
    UInt32                          initDone;
    /**< Flag to indicate intialization is done or not for an instance. */

    UInt32                          drvInstId;
    /**< Driver instance ID. */
    Vrm_Resource                    resrcId[VPSMDRV_SC_MAX_RESR];
    /**< ID of the resources required for this instance of the driver. */
    UInt32                          inCoreInstId;
    /**< Input core instance number/ID. */
    UInt32                          outCoreInstId;
    /**< Output core instance number/ID. */
    const Vcore_Ops                *inCoreOps;
    /**< Input core function pointers. */
    const Vcore_Ops                *outCoreOps;
    /**< Output core function pointers. */

    VpsHal_VpsWb2MuxSrc             wb2MuxSrc;
    /**< WB2 mux configuration for this instance. */
    VpsHal_VpsSecMuxSrc             secMuxSrc;
    /**< Secondary mux configuration for this instance. */

    Semaphore_Handle                instSem;
    /**< Semaphore to protect the open/close calls. */

    Mlm_Handle                      mlmHandle;
    /**< MLM handle. */
    Mlm_ClientInfo                  mlmInfo;
    /**< MLM client information for registering to MLM. */

    VpsUtils_QHandle                freeQ;
    /**< Queue for queueing all the free queue objects of this instance. */
    VpsMdrv_ScQueueObj             *qObjPool;
    /**< Pointer to the Queue object pool for this instance. */

    UInt32                          enableLazyLoading;
    /**< Indicates whether lazy loading is enabled. */
    UInt32                          enableFilterSelect;
    /**< Indicates whether the filter to be used shall be automatically selected
         based on scaling ratios. Recommended to be set to TRUE for best
         vertical scaling output if lazy loading is enabled. */
    VpsHal_ScFactorConfig           curScFactor;
    /**< Currently configured scaling factor for this instance. */
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                         Application interface functions                    */
/* ========================================================================== */
Fdrv_Handle VpsMdrv_scCreate(UInt32 drvId,
                             UInt32 instId,
                             Ptr createArgs,
                             Ptr createStatusArgs,
                             const FVID2_DrvCbParams *fdmCbPrms);
Int32 VpsMdrv_scDelete(FVID2_Handle handle, Ptr deleteArgs);
Int32 VpsMdrv_scProcessFrms(FVID2_Handle handle,
                            FVID2_ProcessList *procList);
Int32 VpsMdrv_scGetProcessedFrms(FVID2_Handle handle,
                                 FVID2_ProcessList *procList,
                                 UInt32 timeout);
Int32 VpsMdrv_scControl(FVID2_Handle handle,
                        UInt32 cmd,
                        Ptr cmdArgs,
                        Ptr cmdStatusArgs);

/* ========================================================================== */
/*     Functions reserving and un-reserving the hardware resources            */
/* ========================================================================== */
Int32 vpsMdrvScAllocResrc(VpsMdrv_ScInstObj *instObj);
Void vpsMdrvScFreeResrc(VpsMdrv_ScInstObj *instObj);

/* ========================================================================== */
/*     Functions getting and releasing the channel objects and associated     */
/*     elements like descriptors, memories                                    */
/* ========================================================================== */
Int32 vpsMdrvScAllocChObjs(VpsMdrv_ScHandleObj *hObj, UInt32 numCh);
Void vpsMdrvScFreeChObjs(VpsMdrv_ScHandleObj *hObj);
Void vpsMdrvScFreeChPrms(VpsMdrv_ScHandleObj *hObj);
Int32 vpsMdrvScUpdatePrms(VpsMdrv_ScHandleObj *hObj,
                          Vps_M2mScCreateParams *params);

/* ========================================================================== */
/*     Allocates and frees the allocated descs and mems                       */
/* ========================================================================== */
VpsMdrv_ScHandleObj *vpsMdrvScAllocHandleObj(void);
Void vpsMdrvScFreeHandleObj(VpsMdrv_ScHandleObj *hObj);
VpsMdrv_ScQueueObj *vpsMdrvScAllocQueObjs(void);
Void vpsMdrvScFreeQueObjs(VpsMdrv_ScQueueObj *qObj);

/* ========================================================================== */
/*                         Misc functions                                     */
/* ========================================================================== */
Int32 vpsMdrvScAllocDescMem(VpsMdrv_ScHandleObj *hObj);
Int32 vpsMdrvScFreeDescMem(VpsMdrv_ScHandleObj *hObj);
VpsMdrv_ScInstObj* vpsMdrvScGetInstObj(UInt32 instId);
Vps_M2mScRtParams *vpsMdrvScGetRtPrms(VpsMdrv_ScHandleObj *hObj,
                                      FVID2_ProcessList *procList,
                                      UInt32 frmCnt);
VpsMdrv_ScChObj *vpsMdrvScGetChObj(VpsMdrv_ScHandleObj *hObj,
                                   UInt32 descSetType,
                                   UInt32 index);

Int32 vpsMdrvScUpdateChPrms(
        VpsMdrv_ScHandleObj *hObj,
        VpsMdrv_ScChObj *chObj,
        Vps_M2mScChParams *chPrms,
        const Vps_M2mScChParams *inChPrms);

#ifdef __cplusplus
}
#endif

#endif  /* _VPSDRV_M2MSCPRIV_H */
