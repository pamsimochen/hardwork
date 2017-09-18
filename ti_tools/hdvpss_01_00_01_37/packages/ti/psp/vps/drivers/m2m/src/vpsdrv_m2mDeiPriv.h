/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDeiPriv.h
 *
 *  \brief VPS DEI M2M driver internal header file.
 */

/* Non Shadow and shadow config descriptors are updated by driver.
 * Non Shadow and Shadow overlay memories are updated by respective cores.
 * Data descriptors are updated by respective cores.
 * Sync on channels are updated by driver.
 * Send interrupt is added by the MLM to wait for the
 * request to get complete and trigger interrupt once the request gets
 * complete.
 * There are VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC coeff descriptors
 * needed for each core in the path that has a scalar.
 * See vps_m2mDei.h: Core 0 (VPSMDRV_DEI_DEI_CORE_IDX) may have scalar enabled.
 *                   Core 2 (VPSMDRV_DEI_VIP_CORE_IDX) may have scalar enabled.
 * If scalar is enabled for the core, the horizontal and vertical scalar
 * coefficient descriptors are present. Otherwise they are not present in the
 * descriptor list.
 * Below layout is shown for per channel configuration
 *
 *    |------------|   |-----------------------------|
 *    |(optional)  |-->|Overlay memory: This is in   |
 *    |Ch 1 Core 0 |   |default coeffs memory in HAL |
 *    |coeff desc  |   |-----------------------------|
 *    |horizontal  |
 *    |------------|   |-----------------------------|
 *    |(optional)  |-->|Overlay memory: This is in   |
 *    |Ch 1 Core 0 |   |default coeffs memory in HAL |
 *    |coeff desc  |   |-----------------------------|
 *    |vertical    |
 *    |------------|   |-----------------------------|
 *    |(optional)  |-->|Overlay memory: This is in   |
 *    |Ch 1 Core 2 |   |default coeffs memory in HAL |
 *    |coeff desc  |   |-----------------------------|
 *    |horizontal  |
 *    |------------|   |-----------------------------|
 *    |(optional)  |-->|Overlay memory: This is in   |
 *    |Ch 1 Core 2 |   |default coeffs memory in HAL |
 *    |coeff desc  |   |-----------------------------|
 *    |vertical    |
 *    |------------|      |------------|
 *    |Ch 1        |----->|Non Shadow  |
 *    |Non-Shadow  |      |Ovly memory |
 *    |Config Desc |      |------------|
 *    |------------|   |------------------|
 *    |Ch 1 shadow |-->|Shadow overlay    |
 *    |config desc |   |memory for Core 1 |
 *    |            |   |and Core 2        |
 *    |------------|   |------------------|
 *    |-------------------|
 *    |Core 0             |
 *    |Ch1 out descriptors|
 *    |-------------------|
 *    |Core 1 (optional)  |
 *    |Ch1 out descriptors|
 *    |-------------------|
 *    |Core 2 (optional)  |
 *    |Ch1 out descriptors|
 *    |-------------------|
 *    |Core 0             |
 *    |Ch1 in  descriptors|
 *    |-------------------|
 *    |Core 1 (optional)  |
 *    |Ch1 in  descriptors|
 *    |-------------------|
 *    |Core 2 (optional)  |
 *    |Ch1 in  descriptors|
 *    |-------------------|
 *    |Core 0             |
 *    |Ch1 multi-window   |
 *    |descriptors        |
 *    |-------------------|
 *    |Core 1 (optional)  |
 *    |Ch1 multi-window   |
 *    |descriptors        |
 *    |-------------------|
 *    |Core 2 (optional)  |
 *    |Ch1 multi-window   |
 *    |descriptors        |
 *    |-------------------|
 *    |------------|
 *    |Ch1 SOCH    |
 *    |descriptors |
 *    |------------|
 *    |Ch1 rld desc|
 *    --------------
 *          |
 *          |
 *          |---->|------------|   |-----------------------------|
 *                |(optional)  |-->|Overlay memory: This is in   |
 *                |Ch 1 Core 0 |   |default coeffs memory in HAL |
 *                |coeff desc  |   |-----------------------------|
 *                |horizontal  |
 *                |------------|   |-----------------------------|
 *                |(optional)  |-->|Overlay memory: This is in   |
 *                |Ch 1 Core 0 |   |default coeffs memory in HAL |
 *                |coeff desc  |   |-----------------------------|
 *                |vertical    |
 *                |------------|   |-----------------------------|
 *                |(optional)  |-->|Overlay memory: This is in   |
 *                |Ch 1 Core 2 |   |default coeffs memory in HAL |
 *                |coeff desc  |   |-----------------------------|
 *                |horizontal  |
 *                |------------|   |-----------------------------|
 *                |(optional)  |-->|Overlay memory: This is in   |
 *                |Ch 1 Core 2 |   |default coeffs memory in HAL |
 *                |coeff desc  |   |-----------------------------|
 *                |vertical    |
 *                |------------|      |------------|
 *                |Ch 2        |----->|Non Shadow  |
 *                |Non-Shadow  |      |Ovly memory |
 *                |Config Desc |      |------------|
 *                |------------|   |------------------|
 *                |Ch 1 shadow |-->|Shadow overlay    |
 *                |config desc |   |memory for Core 1 |
 *                |            |   |and Core 2        |
 *                |------------|   |------------------|
 *                |-------------------|
 *                |Core 0             |
 *                |Ch2 out descriptors|
 *                |-------------------|
 *                |Core 1 (optional)  |
 *                |Ch2 out descriptors|
 *                |-------------------|
 *                |Core 2 (optional)  |
 *                |Ch2 out descriptors|
 *                |-------------------|
 *                |Core 0             |
 *                |Ch2 in  descriptors|
 *                |-------------------|
 *                |Core 1 (optional)  |
 *                |Ch2 in  descriptors|
 *                |-------------------|
 *                |Core 2 (optional)  |
 *                |Ch2 in  descriptors|
 *                |-------------------|
 *                |Core 0             |
 *                |Ch2 multi-window   |
 *                |descriptors        |
 *                |-------------------|
 *                |Core 1 (optional)  |
 *                |Ch2 multi-window   |
 *                |descriptors        |
 *                |-------------------|
 *                |Core 2 (optional)  |
 *                |Ch2 multi-window   |
 *                |descriptors        |
 *                |-------------------|
 *                |------------|
 *                |Ch2 SOCH    |
 *                |descriptors |
 *                |------------|
 *                |Ch2 rld desc|
 *                --------------
 *                      |
 *                      |
 *                      |--------|-----------------|
 *                               |Send Interrupt   |
 *                               |-----------------|
 */
/* Following is the descriptor layout for the per handle configuration. Here
 * the config descriptor is applied only once for the entire request.
 * Similarly, the coeff descriptors are applied only once for the entire
 * request, and not per channel.
 *
 *    |------------|       |-----------------------------|
 *    |(optional)  |-----> |Overlay memory: This is in   |
 *    |Core 0      |       |default coeffs memory in HAL |
 *    |coeff desc  |       |-----------------------------|
 *    |horizontal  |
 *    |------------|       |-----------------------------|
 *    |(optional)  |-----> |Overlay memory: This is in   |
 *    |Core 0      |       |default coeffs memory in HAL |
 *    |coeff desc  |       |-----------------------------|
 *    |vertical    |
 *    |------------|       |-----------------------------|
 *    |(optional)  |-----> |Overlay memory: This is in   |
 *    |Core 2      |       |default coeffs memory in HAL |
 *    |coeff desc  |       |-----------------------------|
 *    |horizontal  |
 *    |------------|       |-----------------------------|
 *    |(optional)  |-----> |Overlay memory: This is in   |
 *    |Core 2      |       |default coeffs memory in HAL |
 *    |coeff desc  |       |-----------------------------|
 *    |vertical    |
 *    |------------|       |------------|
 *    |Non Shadow  |-----> |Non Shadow  |
 *    |Config Desc |       |Ovly memory |
 *    |            |       |------------|
 *    |------------|
 *    |Shadow      |-----> |------------------|
 *    |Config Desc |       |Shadow overlay    |
 *    |            |       |memory for all    |
 *    |----------- |       |Cores             |
 *    |Reload desc |       |------------------|
 *    |------------|
 *          |
 *          |------>|-------------------|
 *                  |Core 0             |
 *                  |Ch1 out descriptors|
 *                  |-------------------|
 *                  |Core 1 (optional)  |
 *                  |Ch1 out descriptors|
 *                  |-------------------|
 *                  |Core 2 (optional)  |
 *                  |Ch1 out descriptors|
 *                  |-------------------|
 *                  |Core 0             |
 *                  |Ch1 in  descriptors|
 *                  |-------------------|
 *                  |Core 1 (optional)  |
 *                  |Ch1 in  descriptors|
 *                  |-------------------|
 *                  |Core 2 (optional)  |
 *                  |Ch1 in  descriptors|
 *                  |-------------------|
 *                  |Core 0             |
 *                  |Ch1 multi-window   |
 *                  |descriptors        |
 *                  |-------------------|
 *                  |Core 1 (optional)  |
 *                  |Ch1 multi-window   |
 *                  |descriptors        |
 *                  |-------------------|
 *                  |Core 2 (optional)  |
 *                  |Ch1 multi-window   |
 *                  |descriptors        |
 *                  |-------------------|
 *                  |------------|
 *                  |Ch1 SOCH    |
 *                  |descriptors |
 *                  |------------|
 *                  |Ch1 rld desc|
 *                  --------------
 *                        |
 *                        |
 *                        |---->|-------------------|
 *                              |Core 0             |
 *                              |Ch2 out descriptors|
 *                              |-------------------|
 *                              |Core 1 (optional)  |
 *                              |Ch2 out descriptors|
 *                              |-------------------|
 *                              |Core 2 (optional)  |
 *                              |Ch2 out descriptors|
 *                              |-------------------|
 *                              |Core 0             |
 *                              |Ch2 in  descriptors|
 *                              |-------------------|
 *                              |Core 1 (optional)  |
 *                              |Ch2 in  descriptors|
 *                              |-------------------|
 *                              |Core 2 (optional)  |
 *                              |Ch2 in  descriptors|
 *                              |-------------------|
 *                              |Core 0             |
 *                              |Ch2 multi-window   |
 *                              |descriptors        |
 *                              |-------------------|
 *                              |Core 1 (optional)  |
 *                              |Ch2 multi-window   |
 *                              |descriptors        |
 *                              |-------------------|
 *                              |Core 2 (optional)  |
 *                              |Ch2 multi-window   |
 *                              |descriptors        |
 *                              |-------------------|
 *                              |------------|
 *                              |Ch2 SOCH    |
 *                              |descriptors |
 *                              |------------|
 *                              |Ch2 rld desc|
 *                              --------------
 *                                    |
 *                                    |
 *                                    |--------|-----------------|
 *                                             |Send Interrupt   |
 *                                             |-----------------|
 */

#ifndef _VPSDRV_M2MDEIPRIV_H
#define _VPSDRV_M2MDEIPRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_deiPath.h>
#include <ti/psp/vps/core/vpscore_deiWrbkPath.h>
#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/vps/core/vpscore_vip.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>
#include <ti/psp/vps/drivers/m2m/vps_m2mInt.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSlice.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mDei.h>
#include <ti/psp/platforms/vps_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of effective driver instance - one HQ and one DEI
 *  paths. All driver instance can't be opened at a time. Out of DEI_HQ-SC,
 *  DEI_HQ-VIP_SC and DEI_HQ-SC,VIP_SC dual drivers, only one could be opened.
 *  Similarly for DEI driver set.
 *  Hence for memory allocation like descriptors, channels etc this macro
 *  is used instead of VPS_M2M_DEI_INST_MAX which could be as many as different
 *  possible drivers for the same paths.
 */
#define VPSMDRV_DEI_MAX_EFF_INST        (2u)

/**
 *  \brief Maximum number of inputs for this driver.
 *  Note: Even though DEI HQ mode 1 driver takes two inputs from the user,
 *  still this is considered as a single input as only one core is used for
 *  both the inputs. Second input is just a side info for the first core.
 */
#define VPSMDRV_DEI_MAX_INPUTS          (1u)

/** \brief Maximum number of outputs for this driver. */
#define VPSMDRV_DEI_MAX_OUTPUTS         (2u)

/**
 *  \brief Maximum number of descriptor set supported by driver per channel.
 *  If all the sets are consumed subsequent requests will be placed in driver
 *  queue.
 *
 *  Currently only one set is supported. This is because when in
 *  deinterlacing mode, a request can't have channel which is already submitted
 *  in previous requests as the compressed previous field size is unknown at
 *  the time of programming the second set. Since this is not taken care
 *  in the driver, restricting the set to 1 will make sure requests are
 *  automatically serialized.
 */
#define VPSMDRV_DEI_MAX_DESC_SET        (1u)

/** \brief Maximum number of pending request per driver handle. */
#define VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE    (10u)

/** \brief Time out to be used in sem pend. */
#define VPSMDRV_DEI_SEM_TIMEOUT         (BIOS_WAIT_FOREVER)

/** \brief Macro representing invalid descriptor set. */
#define VPSMDRV_DEI_INVALID_DESC_SET    (0xFFFFFFFFu)

/** \brief Maximum number of context fields required for de-interlacing. */
#define VPSMDRV_DEI_MAX_FIELD_CONTEXT   (2u)

/**
 *  \brief Flag to indicate duplicated context fields. In cases where we do not
 *  enough context fields but would require to de-interlace, we used the
 *  current field as context.
 */
#define VPSMDRV_DEI_DUPE_FIELD_CONTEXT  (0x0FFFFFFFu)

/**
 *  \brief Configures the instance of inFrameList to be used to return,
 *  context fields.
 */
#define VPSMDRV_DEI_INST_IN_FRAME_LIST_USED (0u)

/**
 *  \brief Maximum number of shadow config descriptor required by driver
 *  to configure shadow memory per channel or per handle.
 */
#define VPSMDRV_DEI_MAX_SHW_CFG_DESC    (1u)

/**
 *  \brief Maximum number of non-shadow config descriptor required by driver
 *  to configure non-shadow memory per channel or per handle.
 */
#define VPSMDRV_DEI_MAX_NSHW_CFG_DESC   (1u)

/**
 *  \brief Maximum number of Reload descriptors required by driver
 *  to link multiple channels for each request.
 */
#define VPSMDRV_DEI_MAX_RLD_DESC        (1u)

/**
 *  \brief Shadow config overlay memory (in bytes) needed by driver for
 *  programming shadow registers for each channel * number of sets
 *  per channel (2 for ping/pong, no scratch memory as same overlay memory
 *  is used for both channel and scratch pad channel).
 *  Caution: Make this a multiple of VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN so
 *  that each memory pool element will also be aligned to this alignment
 *  when defining the array for this pool.
 */
#define VPSMDRV_DEI_MAX_SHW_OVLY_MEM    (864u * VPSMDRV_DEI_MAX_DESC_SET)

/**
 *  \brief Non shadow config overlay memory (in bytes) needed by driver for
 *  programming non shadow registers for each channel * number of sets
 *  per channel (2 for ping/pong, no scratch memory as same overlay memory
 *  is used for both channel and scratch pad channel).
 *  Caution: Make this a multiple of VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN so
 *  that each memory pool element will also be aligned to this alignment
 *  when defining the array for this pool.
 *  96  - for programming frame start event of input core
 *  320 - for programming VIP muxes in case of VIP core
 */
#define VPSMDRV_DEI_MAX_NSHW_OVLY_MEM  ((96u + 320u) * VPSMDRV_DEI_MAX_DESC_SET)

/**
 *  \brief Maximum number of coeff descriptors required by driver
 *  to configure the coefficient memory per channel. The coefficient
 *  configuration shall be done per channel in case config per channel is
 *  selected. One descriptor is used for horizontal, one for vertical, and one
 *  for bilinear scaling coefficients (if applicable).
 */
#define VPSMDRV_DEI_MAX_COEFF_CFG_DESC  (3u)

/**
 *  \brief Number of coeff descriptors required by driver at runtime per scalar
 *  to configure the coefficient memory per channel. Since vertical
 *  bilinear coefficients are set (if applicable) only once in the beginning, at
 *  runtime only horizontal and vertical coefficient descriptors are required.
 */
#define VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC (2u)

/**
 *  \brief Maximum number of coeff descriptors required by driver
 *  at runtime to configure the coefficient memory per channel.
 */
#define VPSMDRV_DEI_MAX_RT_COEFF_CFG_DESC                                      \
                        (VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC *                \
                         VPS_M2M_DEI_SCALAR_ID_MAX)

/** \brief Maximum number of handle to allocate. */
#define VPSMDRV_DEI_MAX_HANDLE_MEM_POOL (VPS_M2M_DEI_MAX_HANDLE_PER_INST       \
                                       * VPSMDRV_DEI_MAX_EFF_INST)

/**
 *  \brief Maximum number of queue objects required to store the incoming
 *  requests per instance. After the queue objects are full, driver will
 *  not queue further requests and will return error to application.
 *
 *  When stopped, an qObj would be used to store all the fields for all
 *  channels to be de-queued by application. Hence one extra qObj needed for
 *  TI814x
 *
 *  Note: Queue objects pool is shared between all the handles of instance.
 */
#define VPSMDRV_DEI_MAX_QOBJS_POOL      (VPSMDRV_DEI_MAX_EFF_INST              \
                                       * VPS_M2M_DEI_MAX_HANDLE_PER_INST       \
                                       * (VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE + 1))

/**
 *  \brief Maximum number of channel memory pool to allocate.
 *  Maximum number of instance * maximum number of channels per instance.
 */
#define VPSMDRV_DEI_MAX_CH_MEM_POOL     (VPSMDRV_DEI_MAX_EFF_INST              \
                                       * VPS_M2M_DEI_MAX_CH_PER_INST)

/**
 *  \brief Total coefficient descriptor memory required for each instance
 *  to configure the coefficient memory each time ioctl is fired to update
 *  the coefficients.
 */
#define VPSMDRV_DEI_MAX_COEFF_DESC_MEM  ((VPSMDRV_DEI_MAX_COEFF_CFG_DESC *     \
                                            VPSHAL_VPDMA_CONFIG_DESC_SIZE)     \
                                        + VPSHAL_VPDMA_CTRL_DESC_SIZE)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Forward declaration for DEI instance object. */
typedef struct VpsMdrv_DeiInstObj_t VpsMdrv_DeiInstObj;

/** \brief Forward declaration for DEI handle object. */
typedef struct VpsMdrv_DeiHandleObj_t VpsMdrv_DeiHandleObj;

typedef struct
{
    FVID2_Frame                    *fieldCtxBuf;
    /**< Application supplied frame FVID2_Frame pointer, used as context
         field for next fields. */
    UInt32                         tth;
    /**< Time To Hold - The current field would be used as context field for
         next VPSMDRV_DEI_MAX_FIELD_CONTEXT input fields.
         Will be initialized with VPSMDRV_DEI_MAX_FIELD_CONTEXT when
         de-interlacing operation is completed on the current field and moved
         to context buffer place holder.
         With evey new input field the tth would be decreased by one for all
         the context buffer objects in context FIFO. */
} VpsMdrv_DeiCtxBufObj;

/**
 *  struct VpsMdrv_DeiQueueObj
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
    VpsMdrv_DeiHandleObj           *hObj;
    /**< Reference to the handle object for this queue object. */
    FVID2_ProcessList               procList;
    /**< FVID2 process frame list to store the incoming/outgoing IO packets. */
    VpsMdrv_QueObjType              qObjType;
    /**< Request type whether it is a frame request or coefficient request. */
    UInt32                          descSetInUse;
    /**< Descriptor set used by this queue object. */
    Mlm_SubmitReqInfo               mlmReqInfo;
    /**< MLM submit request info. */
} VpsMdrv_DeiQueueObj;

/**
 *  struct VpsMdrv_DeiChObj
 *  \brief Structure for storing the per channel information like configuration,
 *  descriptors pointers, number of descriptors etc.
 */
typedef struct
{
    UInt32                          chNum;
    /**< Channel Number. */
    Vps_M2mDeiChParams              chPrms;
    /**< Configuration parameters for the channel. */
    FVID2_Format                    outFmtDei;
    /**< Frame format for the output frame from DEI-Scalar output
         used to init the pointer in channel params. */
    FVID2_Format                    outFmtVip;
    /**< Frame format for the output frame from DEI-VIP output
         used to init the pointer in channel params. */
    FVID2_Format                    inFmtFldN_1;
    /**< Frame format for the N-1 input field DEI input
         used to init the pointer in channel params. */
    Vps_DeiHqConfig                 deiHqCfg;
    /**< DEI HQ configuration used to init the pointer in channel params. */
    Vps_DeiConfig                   deiCfg;
    /**< DEI configuration used to init the pointer in channel params. */
    Vps_ScConfig                    scCfg;
    /**< Scalar parameters like crop and scaler type for the scalar in
         DEI path used to init the pointer in channel params. */
    Vps_ScConfig                    vipScCfg;
    /**< Scalar parameters like crop and scaler type for the VIP scalar
         used to init the pointer in channel params. */
    Vps_CropConfig                  deiCropCfg;
    /**< Cropping configuration for the DEI scalar used to init the pointer
         in channel params. */
    Vps_CropConfig                  vipCropCfg;
    /**< Cropping configuration for the VIP scalar used to init the pointer
         in channel params. */
    Vps_SubFrameParams              subFrameInitPrms;
    /**< SubFrame processing init parameters. */

    Void                           *descMem;
    /**< Points to the descriptor memory containing the data, config,
         coeff and reload descriptors for all the sets.
         This is used for alloc/free call. */
    Void                           *wrDescMem;
    /**< Points to the write descriptor memory used when DEI is
         compressing previous fields.
         This is used for alloc/free call. */
    Void                           *nshwOvlyMem;
    /**< Points to the non-shadow overlay memory of the channel for all the
         sets. This is used for alloc/free call. */
    Void                           *shwOvlyMem;
    /**< Points to the shadow overlay memory of the channel for all the sets.
         This is used for alloc/free call. */
    UInt32                          totalDescMem;
    /**< Total descriptor memory in bytes used for all the cores. */
    UInt32                          totalCoeffDescMem;
    /**< Total coefficient descriptor memory in bytes. */
    UInt32                          totalWrDescMem;
    /**< Total write descriptor memory in bytes. */
    UInt32                          totalNshwOvlyMem;
    /**< Total non-shadow overlay memory in bytes used for all the cores. */
    UInt32                          totalShwOvlyMem;
    /**< Total shadow overlay memory in bytes used for all the cores. */
    UInt32                          numCoeffDesc;
    /**< Number of coefficient descriptors required for the channel. If
         configuration is selected per handle only the first channel of the
         handle object will contain valid information for this field. */
    UInt32                          horzCoeffOvlySize[VPSMDRV_DEI_MAX_CORE];
    /**< Size (in bytes) of the coefficient overlay memory required to program
         horizontal scaling coefficients. This is required by cores which have
         scalar in their paths. */
    UInt32                          vertCoeffOvlySize[VPSMDRV_DEI_MAX_CORE];
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical scaling coefficients. This is required by cores which have
         scalar in their paths. */
    UInt32                          vertBilinearCoeffOvlySize
                                            [VPSMDRV_DEI_MAX_CORE];
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical bilinear scaling coefficients. This is required by cores which
         have HQ scalar in their paths. */
    Ptr                             horzCoeffMemPtr[VPSMDRV_DEI_MAX_CORE];
    /**< Pointer to the coefficient overlay memory required to program
         horizontal scaling coefficients. This is required by cores which have
         scalar in their paths. */
    Ptr                             vertCoeffMemPtr[VPSMDRV_DEI_MAX_CORE];
    /**< Pointer to the coefficient overlay memory required to program
         vertical scaling coefficients. This is required by cores which have
         scalar in their paths. */
    VpsHal_VpdmaConfigDest          coeffConfigDest[VPSMDRV_DEI_MAX_CORE];
    /**< Config destination ID used to program the scalar coefficients. */

    Vcore_DeiCtxInfo                deiCoreCtxInfo;
    /**< DEI context information containing the number of field/MV/MVSTM
         buffers needed for deinterlacing operation. This is derived from
         cores during opening of cores. */
    Vcore_DeiUpdateContext          deiCoreCtxBuf;
    /**< Place holder for DEI context buffer pointers,
         used to program the core descriptors. */
    Void                           *curOutWriteDesc
                                        [VCORE_DEI_MAX_OUT_FLD]
                                        [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< Current out write descriptor memory pointers. */
    UInt32                          curOutFrmSize
                                        [VCORE_DEI_MAX_OUT_FLD]
                                        [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< Last compressed out frame size as written out by VPDMA. */
    UInt32                          curOutTxSize
                                        [VCORE_DEI_MAX_OUT_FLD]
                                        [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< Last compressed out transfer size as written out by VPDMA. */

    VpsMdrv_DeiCtxBufObj            ctxFieldFifo
                                        [VPSMDRV_DEI_MAX_FIELD_CONTEXT + 1u];
    /**< Place holder for context fields
         ctxFieldFifo[0] - holds N field - i.e. current field - would become
            N - 1 field for the next field
         ctxFieldFifo[1] - holds N - 1 context field
         ctxFieldFifo[2] - holds N - 2 context field and so on... */
    UInt32                          isCtxFieldInit;
    /**< Flag to indicate if context fields are initialized, Would be set to
         TRUE, when de-interlacing for the very first field. Would be
         initialized to FALSE at intilization. */
    VpsUtils_QueHandle              agedCtxFieldQ;
    /**< Q that holds the context buffer that are no longer needed by the driver
         for de-interlacing operation. */
    UInt32                          sapce4AgedCtxFieldQ
                                        [VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE + 1u];
    /**< Place holder to implement the Q.
         Size should be max q depth + 1 as in case of stopping, there could
         be conditions where in user has queued max and wants to stop. */

    Vps_DeiCtxInfo                  deiCtxInfo;
    /**< DEI context buffer information to be given to application. */
    Vps_DeiCtxBuf                   deiCtxBuf;
    /**< DEI context buffer provided by application. */
    UInt32                          isDeiCtxBufSet;
    /**< Flag to indicate whether the DEI context buffers are set by application
         or allocated by driver. Used as de-allocating flag in tear down. */
    UInt32                          fldIdx;
    /**< Field buffer index pointing to the N-1 field input. This is used for
         rotation of DEI context buffers. */
    UInt32                          mvIdx;
    /**< MV buffer index pointing to the N-1 MV input. This is used for
         rotation of DEI context buffers. */
    UInt32                          mvstmIdx;
    /**< MVSTM buffer index pointing to the N-1 MVSTM input. This is used for
         rotation of DEI context buffers. */
    UInt32                          prevFldState;
    /**< At the start up, DEI should operate in line-average mode for
         the few fields. This variable keeps track of number of frame
         submitted to the DEI. It will be initialized to zero at the
         start up or at the time of resetting DEI context. */
    UInt32                          dontRotateCtxBuf;
    /**< Flag to indicate whether the context buffers needs to be rotated.
         This flag will be set when the compressed buffer size reported by
         VPDMA is invalid and in the next frame submission for the channel,
         the invalid compressed frame size is not used by not rotating
         the context buffers. */

    Void                           *firstDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Pointer to the first descriptor in the descriptor layout for the
         channel. */
    VpsHal_VpdmaConfigDesc         *coeffDesc
                                        [VPSMDRV_DEI_MAX_DESC_SET]
                                        [VPSMDRV_DEI_MAX_RT_COEFF_CFG_DESC];
    /**< Points to the coefficient descriptor pointer. */
    VpsHal_VpdmaConfigDesc         *nshwCfgDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the non shadow config descriptor of the channel. */
    VpsHal_VpdmaConfigDesc         *shwCfgDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the shadow config descriptor of the channel. */
    VpsHal_VpdmaSyncOnChannelDesc  *sochDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the SOCH control descriptor of the channel. */
    VpsHal_VpdmaReloadDesc         *rldDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the reload descriptor of the channel. */
    Vcore_DescMem                   coreDescMem[VPSMDRV_DEI_MAX_DESC_SET]
                                               [VPSMDRV_DEI_MAX_CORE];
    /**< Structure holding the data descriptor, overlay memory references
         for all the cores and for all the sets. */

    UInt32                          isDescDirty[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Flag to indicate whether a descriptor is dirty or not. */

    VpsMdrv_SubFrameChInst          slcChObj;
    /**< Instance memory for subframe based porcessing. */
    VpsHal_ScFactorConfig           scFactor[VPS_M2M_DEI_SCALAR_ID_MAX];
    /**< Scaling factor for this channel based on required scaling ratio for
         each scalar for this channel. */
} VpsMdrv_DeiChObj;

/**
 *  struct VpsMdrv_DeiHandleObj_t
 *  \brief Structure for storing the per handle information.
 *  This structure represents the each handle of the driver instance.
 *  It will encapsulate the channel object instances equal to the maximum number
 *  of channels supported by handle and the handle specific elements like
 *  non shadow config descriptor and memory and coeff descriptor and memory.
 */
struct VpsMdrv_DeiHandleObj_t
{
    VpsMdrv_DeiInstObj             *instObj;
    /**< Instance object to which this handle belongs to. */
    VpsMdrv_DeiChObj               *chObjs[VPS_M2M_DEI_MAX_CH_PER_HANDLE];
    /**< Pointer to channel objects for this handle. */

    UInt32                          mode;
    /**< Mode to indicate whether configuration is required per handle or
         per channel. For valid values see #Vps_M2mMode. */
    UInt32                          numCh;
    /**< Number of channels to be supported by handle. */
    UInt32                          deiHqCtxMode;
    /**< Determines the mode in which the DEI HQ context buffers are handled
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
         reload descriptors for the handle. This is used for alloc/free call. */
    Void                           *nshwOvlyMem;
    /**< Points to the non-shadow overlay memory of the handle used when
         per handle config is TRUE. This is used for alloc/free call. */
    Void                           *shwOvlyMem;
    /**< Points to the shadow overlay memory of the handle used when
         per handle config is TRUE. This is used for alloc/free call. */
    UInt32                          numCoeffDesc;
    /**< Number of coefficient descriptors required for handle.
         If configuration is selected per channel this will be set to zero and
         memory for the coeff descriptors will be allocated as the part of
         the channel object.
         If configuration is selected per handle this will be set to the number
         of coeff descriptors and memory for the coeff descriptors will be
         allocated as the part of the handle object. */
    UInt32                          totalDescMem;
    /**< Total descriptor memory in bytes used for per handle descriptors. */
    UInt32                          totalCoeffDescMem;
    /**< Total coefficient descriptor memory in bytes for per handle
         descriptors. */
    UInt32                          totalNshwOvlyMem;
    /**< Total non-shadow overlay memory in bytes used for all the cores. */
    UInt32                          totalShwOvlyMem;
    /**< Total shadow overlay memory in bytes used for all the cores. */

    Void                           *firstDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Pointer to the first descriptor in the descriptor layout for the
         handle. */
    VpsHal_VpdmaConfigDesc         *coeffDesc
                                        [VPSMDRV_DEI_MAX_DESC_SET]
                                        [VPSMDRV_DEI_MAX_RT_COEFF_CFG_DESC];
    /**< Points to the coefficient descriptor pointer of the handle. */
    VpsHal_VpdmaConfigDesc         *nshwCfgDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the non shadow config descriptor of the handle. */
    VpsHal_VpdmaConfigDesc         *shwCfgDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the shadow config descriptor of the handle. */
    VpsHal_VpdmaReloadDesc         *rldDesc[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Points to the reload descriptor of the handle. */

    UInt32                          isDescSetFree[VPSMDRV_DEI_MAX_DESC_SET];
    /**< Flag to indicate whether the descriptor set is free or not. */

    Vcore_Handle                    coreHandle[VPSMDRV_DEI_MAX_CORE];
    /**< Core handles for DEI / DEI HQ, DWP, VIP cores. */
    Bool                            isVipScReq;
    /**< Flag to indicate whether VIP scalar is required for this
         handle or not. */

    VpsHal_VpdmaChannel             socChNum[VCORE_MAX_VPDMA_CH];
    /**< Client on which MLM should wait for the end of frame signal. */
    UInt32                          numVpdmaChannels;
    /**< Size of socChNum array. */
    UInt32                          sochIdx;
    /**< Index into sochChNum array to get the VPDMA channel
         number for programming Sync on Client control descriptor. */
    UInt32                          isStopped;
    /**< Flag to indicate if driver is being stopped, FALSE is normal operation
         TRUE to indicate mlmCb not to Q up new requests */
    FVID2_FrameList                 tempFrameList;
    /**< When driver is stopped, all the fields held as context fields would
         required to be released back to apps. On stop all the fields are copied
         into this frameList and put back as completed qObj. On de-q, the normal
         de-q operation is performed and qObj would be released.
         frameList required here, as qObj, has processList but not frameList.
         Currently assuming that we can have maximum of
         VPS_M2M_DEI_MAX_CH_PER_HANDLE channel at any time, so a maximum of
         VPS_M2M_DEI_MAX_CH_PER_HANDLE * 3 context fields requires to be released
         to apps. The below compilation gaurd is to ensure that. */

	VpsMdrv_DeiQueueObj	*qObj[VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE];
    /**< KC: Array of queue obj's that were allocated during create
         This que obj's in this arrary will be used to free when doing delete
         instead of relying on objects found in freeQ
    */
	
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#if ((VPS_M2M_DEI_MAX_CH_PER_HANDLE * 0x03) >= FVID2_MAX_FVID_FRAME_PTR)
#error "DEI M2M IOCTL STOP will not work - Require to increase space to hold " \
        "context fields"
#endif
#endif
};

/** \brief Driver structure per instance of driver */
struct VpsMdrv_DeiInstObj_t
{
    UInt32                          openCnt;
    /**< Open count. */
    UInt32                          initDone;
    /**< Flag to indicate intialization is done or not for an instance. */
    UInt32                          numChAllocated;
    /**< Total number of channels allocated per instance. Used for error
         checking. */

    UInt32                          drvInstId;
    /**< Driver instance ID. */
    VpsHal_VpsSecInst               secMuxInstId;
    /**< Secondary path mux instance ID. */
    Vrm_Resource                    resrcId[VPSMDRV_DEI_MAX_RESR];
    /**< DEI / DEI HQ, DWP, VIP core resource ID. */
    UInt32                          coreInstId[VPSMDRV_DEI_MAX_CORE];
    /**< DEI / DEI HQ, DWP, VIP core instance number/ID. */
    const Vcore_Ops                *coreOps[VPSMDRV_DEI_MAX_CORE];
    /**< DEI / DEI HQ, DWP, VIP core function pointers. */

    UInt32                          isCoreReq[VPSMDRV_DEI_MAX_CORE];
    /**< Flag to indicate whether a core has to be included for a particular
         instance or not. */
    UInt32                          numInListReq;
    /**< Number of input frame list required for an instance.
         Currently this will be 1 for DEI input core. */
    UInt32                          numOutListReq;
    /**< Number of output frame list required for an instance.
         Currently this will be either 1 or 2 for DEI WRBK and/or VIP cores. */

    Semaphore_Handle                instSem;
    /**< Semaphore to protect the open/close calls. */

    Mlm_Handle                      mlmHandle;
    /**< MLM handle. */
    Mlm_ClientInfo                  mlmInfo;
    /**< MLM client information for registering to MLM. */

    UInt32                          scalarCoreId
                                        [VPS_M2M_DEI_SCALAR_ID_MAX];
    /**< Indicates the core ID corresponding to the scalar ID passed for scalar
         functionality like setting coefficients or enabling/disabling Lazy
         Loading.
         If this instance of the driver does not have a core supporting either
         or both scalars, the value of scalarCoreId would be set to
         VPSMDRV_DEI_MAX_CORE, which indicates invalid core ID. */
    UInt32                          enableLazyLoading
                                        [VPS_M2M_DEI_SCALAR_ID_MAX];
    /**< Indicates whether lazy loading is enabled for each scalar in paths
         supported by DEI M2M driver. */
    UInt32                          enableFilterSelect
                                        [VPS_M2M_DEI_SCALAR_ID_MAX];
    /**< Indicates whether the filter to be used shall be automatically selected
         based on scaling ratios for each scalar in paths supported by DEI M2M
         driver. Recommended to be set to TRUE for best vertical scaling output
         if lazy loading is enabled. */
    VpsHal_ScFactorConfig           curScFactor[VPS_M2M_DEI_SCALAR_ID_MAX];
    /**< Currently configured scaling factor for each scalar in this instance */
};

/**
 *  struct VpsMdrv_DeiPoolObj
 *  DEI M2M driver memory pool object containing all the statically allocated
 *  objects - used structure to avoid multiple global variables.
 */
typedef struct
{
    VpsMdrv_DeiHandleObj    handleMemPool[VPSMDRV_DEI_MAX_HANDLE_MEM_POOL];
    /**< Memory pool for the handle objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any handle object. */
    UInt32                  handleMemFlag[VPSMDRV_DEI_MAX_HANDLE_MEM_POOL];
    /**< The flag variable represents whether a handle object is allocated
         or not. */
    VpsUtils_PoolParams     handlePoolPrm;
    /**< Pool params for handle memory. */

    VpsMdrv_DeiChObj        chMemPool[VPSMDRV_DEI_MAX_CH_MEM_POOL];
    /**< Memory pool for the channel objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any channel object. */
    UInt32                  chMemFlag[VPSMDRV_DEI_MAX_CH_MEM_POOL];
    /**< The flag variable represents whether a channel object is allocated
         or not. */
    VpsUtils_PoolParams     chPoolPrm;
    /**< Pool params for channel memory. */

    VpsMdrv_DeiQueueObj     qObjMemPool[VPSMDRV_DEI_MAX_QOBJS_POOL];
    /**< Memory pool for the queue objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any queue object. */
    UInt32                  qObjMemFlag[VPSMDRV_DEI_MAX_QOBJS_POOL];
    /**< The flag variable represents whether a queue object is allocated
         or not. */
    VpsUtils_PoolParams     qObjPoolPrm;
    /**< Pool params for queue object memory. */
} VpsMdrv_DeiPoolObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/*
 * M2M DEI process request APIs
 */
Int32 VpsMdrv_deiMlmCbFxn(Mlm_SubmitReqInfo *reqInfo);
Int32 vpsMdrvDeiProcessReq(VpsMdrv_DeiHandleObj *hObj,
                           VpsMdrv_DeiQueueObj *qObj);
UInt32 vpsMdrvDeiAllocDescSet(VpsMdrv_DeiHandleObj *hObj);
Int32 vpsMdrvDeiFreeDescSet(VpsMdrv_DeiHandleObj *hObj, UInt32 descSet);
Int32 VpsMdrvDeiGetScFactorConfig(VpsMdrv_DeiHandleObj *hObj,
                                  UInt32 scalarId,
                                  UInt32 chNum,
                                  UInt32 descSet);

/*
 * M2M DEI core APIs
 */
Int32 vpsMdrvDeiOpenCores(VpsMdrv_DeiHandleObj *hObj);
Int32 vpsMdrvDeiCloseCores(VpsMdrv_DeiHandleObj *hObj);
Int32 vpsMdrvDeiHqRdAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvDeiHqWrAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvDeiRdAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvDeiWrAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvDeiScRdAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
Int32 vpsMdrvDeiScWrAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);

/*
 * M2M DEI platform specific APIs
 */
/* TI814x specific functions */
Int32 vpsMdrvDeiAgeCtxBufs(VpsMdrv_DeiChObj *chObj, FVID2_Frame *curFrame);
Int32 vpsMdrvDeiReleaseAgedCtxBufs(VpsMdrv_DeiHandleObj *hObj,
                                   VpsMdrv_DeiQueueObj *qObj);
Int32 vpsMdrvDeiStop(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
UInt32 vpsMdrvDeiIsCtxFieldsSet(VpsMdrv_DeiHandleObj *hObj);
Int32 vpsMdrvDeiOverridePrevFldBufIoctl(VpsMdrv_DeiHandleObj *hObj,
                                        Ptr cmdArgs);

/*
 * M2M DEI private/common APIs
 */
Int32 vpsMdrvDeiInit(UInt32 numInst,
                     const VpsMdrv_DeiInitParams *initPrms);
Int32 vpsMdrvDeiDeInit(void);

VpsMdrv_DeiInstObj *vpsMdrvDeiGetInstObj(UInt32 instId);
Int32 vpsMdrvDeiAllocResrc(VpsMdrv_DeiInstObj *instObj);
Int32 vpsMdrvDeiFreeResrc(VpsMdrv_DeiInstObj *instObj);

VpsMdrv_DeiHandleObj *vpsMdrvDeiAllocHandleObj(
                          Vps_M2mDeiCreateParams *createPrms,
                          VpsMdrv_DeiInstObj *instObj,
                          const FVID2_DrvCbParams *fdmCbPrms,
                          Int32 *retValPtr);
Int32 vpsMdrvDeiFreeHandleObj(VpsMdrv_DeiHandleObj *hObj);

Int32 vpsMdrvDeiAllocCoreMem(VpsMdrv_DeiHandleObj *hObj);
Int32 vpsMdrvDeiFreeCoreMem(VpsMdrv_DeiHandleObj *hObj);

Int32 vpsMdrvDeiProgramDesc(VpsMdrv_DeiHandleObj *hObj);

Void *vpsMdrvDeiAllocCoeffDesc(void);
Int32 vpsMdrvDeiFreeCoeffDesc(Void *coeffDesc);


/**
 *  vpsMdrvDeiIsDeiPresent
 *  \brief Function returns weather DEI is present or not.
 *
 *  \param arg          handle Object.
 *
 *  \return             Returns TRUE if DEI is present in the path else FALSE.
 */
static inline Bool vpsMdrvDeiIsDeiPresent(VpsMdrv_DeiHandleObj *hObj)
{
    Bool        isDeiPresent = TRUE;

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    UInt32  drvInstId;

    drvInstId = hObj->instObj->drvInstId;
    if ((drvInstId == VPS_M2M_INST_AUX_SC2_WB1) ||
        (drvInstId == VPS_M2M_INST_AUX_SC4_VIP1) ||
        (drvInstId == VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1))
    {
        isDeiPresent = FALSE;
    }
#endif  /* TI_814X_BUILD || TI_8107_BUILD */

    return (isDeiPresent);
}

/**
 *  \brief Checks if the M2M DEI driver instance is a DEI_HQ instance.
 *
 *  \param drvInst      [IN] Valid M2M DEI driver instance ID.
 *
 *  \return TRUE if the instance is DEI_HQ path else returns FALSE.
 */
static Int32 vpsMdrvDeiIsHqInst(UInt32 drvInst)
{
    Int32       result = FALSE;

#ifdef TI_816X_BUILD
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == drvInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == drvInst))
    {
        result = TRUE;
    }
#endif

    return (result);
}

#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_M2MDEIPRIV_H */
