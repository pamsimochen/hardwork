/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
  \file vps_capture_lm.h

  \brief List layer - VIP Capture List Manager (CLM)
*/

#ifndef _VPS_CAPT_LM_H_
#define _VPS_CAPT_LM_H_

#include <ti/psp/vps/drivers/capture/vpsdrv_capture.h>

/**
  \ingroup VPS_DRV_UTIL_API
  \defgroup VPS_DRV_UTIL_CAPTURE_LM_API List layer - VIP Capture List Manager (CLM)

  <b>IMPORTANT: </b> CLM is used internally by VIP capture driver
  and users should not use these APIs directly.
  The APIs are documented mainly for reference and for capture driver
  internal developer

  \code
  vChannel means VPDMA Channel over which VPDMA is transferring video data

  lChannel means logical software Channel over which capture in
    happening from user point of view
  \endcode

  @{
*/

/* constants  */

#define VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
//#define VPS_CAPT_LIST_ENABLE_ASSERT_ON_ERROR

/* Time interval is OS ticks to trigger wakeup of list processing task  */
#ifdef PLATFORM_SIM
#define VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS   (4)
#else
#define VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS   (8)
#endif

/* uncomment this to enable debug prints in capture list processing */
//#define VPS_CAPT_DEBUG_LIST


/** CLM State - Idle  */
#define VPS_CAPT_LM_STATE_IDLE     (0)

/** CLM State - Running/Active */
#define VPS_CAPT_LM_STATE_RUNNING  (1)


/* data structure's */

/**
  \brief List Manager - "Do list processing" callback prototype
*/
typedef Void (*Vps_CaptLmCompleteCb)();

/**
  \brief Capture descriptor parsing info for recevied descriptor
*/
typedef struct
{
  Ptr                 descAddr;
  /**< Address from where descriptor needs to be parsed */

  UInt16              fid;
  /**< received field ID, 0 for even field, 1 for odd field  */

  UInt16              frameWidth;
  /**< received field/frame width, in pixels  */

  UInt16              frameHeight;
  /**< received field/frame height, in lines  */

  VpsHal_VpdmaChannel vChannel;
  /**< VPDMA channel to which this descriptor belongs */

  UInt16              lChannel;
  /**< Logical channel to which this descriptor belongs */

  UInt16              vChannelIdx;
  /**< VPDMA channel idx to which this descriptor belongs */

} Vps_CaptLmDataDescParseInfo;

/**
  \brief Capture descriptor update info when updating a
          existing descriptor in CLM
*/
typedef struct
{
  Ptr descAddr;
  /**< Address where descriptor needs to be updated */

  Ptr bufAddr;
  /**< data write address, if NULL, data is not written to memory */

  UInt32 enableDescWb;
  /**< TRUE: Write back descriptor when field/frame is received, \n
       FALSE: Do NOT write back descriptor  */
  Ptr descWrAddr;
  /**< Descriptor write address. This is where descriptor will be
       written by VPDMA */

  UInt32 enableDescSkip;
  /**< TRUE: Enables Skip Descriptor feature \n
       FALSE: Disables Skip Descriptor feature */

  UInt16 memType;
  /**< Memory type, 1D or 2D  */

  UInt16 maxOutWidth;
  /**< Max output height limit */

  UInt16 maxOutHeight;
  /**< Max output height limit */

  VpsHal_VpdmaChannel vChannel;
  /**< VPDMA channel to which this descriptor belongs */

  UInt32 dropData;
  /**< Program Drop Data Descriptor */

} Vps_CaptLmDataDescUpdateInfo;

/**
  \brief Capture descriptor info when adding a new descriptor to CLM
*/
typedef struct
{
  VpsHal_VpdmaChanDT        dataType;
  /**< data type, YUV422 etc  */

  VpsHal_VpdmaDataDescPrio  priority;
  /**< descriptor priority  */

  VpsHal_VpdmaMemoryType    memType;
  /**< Memory type, 1D or 2D  */

  VpsHal_VpdmaLineSkip      lineSkip;
  /**< Line Skip  */

  UInt16                    lineStride;
  /**< line stride in bytes */

  Ptr                       *addr;
  /**< data write address, if NULL, data is not written to memory */

  UInt16                    maxOutWidth;
  /**< Max output height limit */

  UInt16                    maxOutHeight;
  /**< Max output height limit */

  VpsHal_VpdmaChannel       vChannel;
  /**< VPDMA channel to which this descriptor belongs */

  UInt8                    *descAddr;
  /**< Address where descriptor is to be created */
  UInt32                    enableDescSkip;
} Vps_CaptLmDataDescAddInfo;

typedef struct
{
    VpsHal_VpdmaChannel vChannel;
    UInt32 width;
    UInt32 height;
    UInt32 addr;
    UInt32 descAddr;
    UInt32 timestamp;
    UInt32 fid;

} Vps_CaptLmDescInfo;

/* functions  */

/**
  \brief CLM init - allocates resources like semaphores, interrupts etc

  \param completeCb [I ] Completion callback thats called when
                         list processing needs to happen

  \return 0 on success, else failure
*/
Int32 Vps_captLmInit(Vps_CaptLmCompleteCb completeCb);

/**
  \brief CLM de-init - free's resources allocated during Vps_captLmInit()

  \param completeCb [I ] Completion callback thats called when
                         list processing needs to happen

  \return 0 on success, else failure
*/
Int32 Vps_captLmDeInit();

/**
  \brief Set CLM state as idle or running

  \param state  [I ] CLM state

  \return 0 on success, else failure
*/
Int32 Vps_captLmSetState(UInt16 state);

/**
  \brief Get CLM state

  \return current CLM state
*/
Int32 Vps_captLmGetState();


/**
  \brief CLM Mutual exculsion lock - used to get exclusive
         access to list processing

  \return 0 on success, else failure
*/
Int32 Vps_captLmLock();

/**
  \brief CLM Mutual exculsion unlock

  \return 0 on success, else failure
*/
Int32 Vps_captLmUnlock();

/**
  \brief Map logical channel to VPDMA HW channel

  \param vChannel [I ] VPDMA channel
  \param lChannel [I ] Capture logical channel
  \param vChannelIdx   [I ] VPDMA channel index

  \return 0 on success, else failure
*/
Int32  Vps_captLmMapChannel(VpsHal_VpdmaChannel vChannel,
        UInt16 lChannel, UInt16 vChannelIdx);

/**
  \brief Get logical channel for a VPDMA HW channel

  \param vChannel [I ] VPDMA channel

  \return Capture logical channel
*/
UInt16 Vps_captLmGetLogicalChannelId(VpsHal_VpdmaChannel vChannel);

/**
  \brief Get VPDMA channel idx for a VPDMA HW channel

  \param vChannel [I ] VPDMA channel

  \return Capture logical channel
*/
UInt16 Vps_captLmGetVpdmaChannelIdx(VpsHal_VpdmaChannel vChannel);

/**
  \brief Add a new descriptor to current list that is being processed

  \param info [I ] descriptor info

  \return 0 on success, else failure
*/
Int32 Vps_captLmAddDataDesc(Vps_CaptLmDataDescAddInfo *info);

/**
  \brief Add a abort descriptor to current list that is being processed

  \param vChannel [I ] VPDMA channel which needs to be aborted

  \return 0 on success, else failure
*/
Int32 Vps_captLmAddAbortDesc(UInt8 *memPtr, VpsHal_VpdmaChannel vChannel);

/**
  \brief Parse received data descriptor from current list
          that is being processed

  \param info [ O] Parsed descriptor info

  \return 0 on success, else failure
*/
Int32 Vps_captLmParseDataDesc (Vps_CaptLmDataDescParseInfo *info);

/**
  \brief Update received data descriptor with new information for
          current list that is being processed

  \param info [I ] info that needs to be updated

  \return 0 on success, else failure
*/
Int32 Vps_captLmUpdateDataDesc(Vps_CaptLmDataDescUpdateInfo *info);


/**
  \brief Goto next data descriptor

  This is used you want to goto next data descriptor without updating current
  data desc.

  This will insert dummy descriptors in place of current desc
*/
Int32 Vps_captLmNextDataDesc();

/**
  \brief Submit current list that is being processed

  \param checkIntervalInTicks [I ] time intervals in units of
                                    OS ticks after which the callback
                                    will called to do next list processing

  \return 0 on success, else failure
*/
Int32 Vps_captLmSubmit(Int16 checkIntervalInTicks);

/**
  \brief Copy complete descriptors from HW que to list post area

  \return 0 on success, else failure
*/
Int32 Vps_captLmSwitch();

/**
  \brief Add descriptor to program scaler coeffs

  Make scaler coeffs are filled before calling this API

  \param vipInstId  [IN] VIP inst 0 or 1
  \param listAddr   [IN] Pointer to descriptor start Address
  \param coeffMem   [IN] Pointer to Coefficient memory

  \return 0 on success, else failure
*/
Int32 Vps_captLmAddScCoeffConfigDesc(UInt32 vipInstId, Ptr listAddr, Ptr coeffMem);


/**
  \brief Print advanced capture list manager related statistics

  \return 0 on success, else failure
*/
Int32 Vps_captLmPrintAdvancedStatistics();

Int32 Vps_captLmAddDummyMultiChDesc (VpsHal_VpdmaChannel vChannel);

Int32 Vps_captLmDoDisSyncStartSequence();

UInt32 Vps_captLmPostDescList(UInt8 *listAddr, UInt16 listSize);

UInt32 Vps_captLmPostList(UInt8 *listAddr, UInt16 listSize);

Int32 Vps_captLmParseDescWrInfo(UInt8 *pDescPtr,
                                UInt8 *pDataAddr,
                                UInt32 channelNum,
                                UInt32 *fid,
                                UInt32 *frameWidth,
                                UInt32 *frameHeight);

#endif

/* @} */
