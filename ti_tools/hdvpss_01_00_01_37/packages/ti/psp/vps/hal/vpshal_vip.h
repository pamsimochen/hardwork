/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
 *  \file vpshal_vip.h
 *
 *  \brief VPS Vip Parser HAL header file
 *  This file exposes the HAL APIs of the VPS Vip Parser.
 *
 */

#ifndef _VPSHAL_VIP_H
#define _VPSHAL_VIP_H

/* ========================================================================= */
/*                             Include Files                                 */
/* ========================================================================= */

#include <ti/psp/vps/vps.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================= */
/*                           Macros & Typedefs                               */
/* ========================================================================= */

/**
 *  Typedef for Vip Parser Hal Handle
 */
typedef void *VpsHal_VipHandle;

/**
 *  enum VpsHal_VipSyncType.
 *  \brief Enum for Vip Sync Type.
 */
typedef enum
{
     VPSHAL_VIP_SYNC_TYPE_EMB_SINGLE_422_YUV=0,
     /**< Embedded sync single 4:2:2 YUV stream */

     VPSHAL_VIP_SYNC_TYPE_EMB_2X_422_YUV = 1,
     /**< Embedded sync 2x multiplexed 4:2:2 YUV stream */

     VPSHAL_VIP_SYNC_TYPE_EMB_4X_422_YUV = 2,
     /**< Embedded sync 4x multiplexed 4:2:2 YUV stream */

     VPSHAL_VIP_SYNC_TYPE_EMB_LINE_YUV = 3,
     /**< Embedded sync line multiplexed 4:2:2 YUV stream */

     VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_YUV = 4,
     /**< Discrete sync single 4:2:2 YUV stream */

     VPSHAL_VIP_SYNC_TYPE_EMB_SINGLE_RGB_OR_444_YUV = 5,
     /**< Embedded sync single RGB stream or single 444 YUV stream */

#if 0
     VPSHAL_VIP_SYNC_TYPE_EMB_2X_RGB,
     /**< Embedded sync 2x multiplexed RGB stream */

     VPSHAL_VIP_SYNC_TYPE_EMB_4X_RGB,
     /**< Embedded sync 4x multiplexed RGB stream */

     VPSHAL_VIP_SYNC_TYPE_EMB_LINE_RGB,
     /**< Embedded sync line multiplexed RGB stream */

     VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_8B_RGB,
     /**< Discrete sync single 8b RGB stream */
#endif

     VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_24B_RGB = 10
     /**< Discrete sync single 24b RGB stream */
} VpsHal_VipSyncType;

/**
 *  enum VpsHal_VipIntfMode
 *  \brief Enum for Input Interface Mode
 */
typedef enum
{
     VPSHAL_VIP_INTF_MODE_24B=0,
     /**< 24b data interface. Uses Port A settings */

     VPSHAL_VIP_INTF_MODE_16B,
     /**< 16b data interface. Uses Port A settings */

     VPSHAL_VIP_INTF_MODE_8B
     /**< Dual independent 8b data interface.Uses independent Port A and Port B
      * settings
      */
} VpsHal_VipIntfMode;

/**
 *  enum VpsHal_VipInst
 */
typedef enum
{
    VPSHAL_VIP_INST_0 = 0,
    /**< Vip Instance 0 */

    VPSHAL_VIP_INST_1,
    /**< Vip Instance 1 */

    VPSHAL_VIP_INST_MAX
    /**< Vip Instance MAX */
} VpsHal_VipInst;

/**
 *  enum VpsHal_VipPort
 *  \brief Vip Port gives information about which port is getting used
 */
typedef enum
{
    VPSHAL_VIP_PORT_A = 0,
    /**< Vip PortA */

    VPSHAL_VIP_PORT_B,
    /**< Vip PortB */

    VPSHAL_VIP_PORT_MAX
    /**< Vip Port Max */

} VpsHal_VipPort;

/* ========================================================================= */
/*                         Structure Declarations                            */
/* ========================================================================= */

/**
 *  struct VpsHal_vipInstParams
 *  \brief Structure containg parameter to initialize vip instances
 */
typedef struct
{
    UInt32         baseAddress;
    /**< base address of the register */
} VpsHal_VipInstParams;

/**
 *  struct VpsHal_VipConfig
 *  \brief Structure containing configuration parameter for VIP
 */
typedef struct
{
     Vps_VipConfig  vipConfig;
     /**< VIP config  */

     UInt32 intfMode;
     /**< Interface mode. See #VpsHal_VipIntfMode */
} VpsHal_VipConfig;

/**
 *  struct VpsHal_VipPortConfig
 *  \brief Structure containing configuration parameter for Port
 */
typedef struct
{
      UInt32   syncType;
     /**< Sync Type. See #VpsHal_VipSyncType */

      UInt32   ctrlChanSel;
     /**< channel selection to extract control codes and Vertical Ancillary Data
           See #Vps_VipCtrlChanSel
      */

      UInt32   ancChSel8b;
     /**< '0' = Extract 8b mode vert ancillary data from Luma side,
      * '1' = from Chroma Side
      * See #Vps_VipAncChSel8b
      */

      UInt32   pixClkEdgePol;
     /**< '0' = Rising edge, '1' = Falling edge
            See #Vps_VipPixClkEdgePol
        */

     Vps_VipDiscreteConfig  disConfig;
     /**< configuration parameter specific to Discrete Sync only */

     Vps_VipEmbConfig       embConfig;
     /**< configuration parameter specific to Embedded Sync only */

     UInt32                    invertFidPol;
     /**< '0' Keep FID as found, '1' Invert Determined Value of FID */

     UInt32                    enablePort;
     /**< '0' = Disable Port, '1' = Enable Port */

     UInt32                    clrAsyncFifoRd;
     /**< '0' = Normal, '1' = Clear Async FIFO Read Logic */

     UInt32                    clrAsyncFifoWr;
     /**< '0' = Normal,  '1' = Clear Async FIFO Write Logic */

     UInt32                    discreteBasicMode;
     /**< FALSE = Normal Discerete Mode, TRUE = Basic Discerete Mode.
          TRUE = DE signal need not be active during the VBLANK period. Most of
            the encoder provide the discerete signals in this way.
          FALSE = DE signal needs to be active during the VBLANK period */
} VpsHal_VipPortConfig;

/**
 *  struct VpsHal_VipStatus
 *  \brief Structure containing configuration status parameter.
 *  All parameter are read only
 */
typedef struct
{
    UInt32      curFid;
    /**< Current FID of the source frame. See #Vps_VipFid  */

    UInt32      prevFid;
    /**< Previous FID of the source frame. See #Vps_VipFid  */

    UInt32      curEncFid;
    /**< Current Encoder FID of the source frame. See #Vps_VipFid  */

    UInt32      prevEncFid;
    /**< Previous Encoder FID of the Source frame. See #Vps_VipFid  */

    UInt16      height;
    /**< Height of source frame */

    UInt16      width;
    /**< Width of source frame */

    UInt32      isVideoDetect;
    /**< '1' = proper video sync is found, '0' = Proper video sync is not found
     * for Embedded Sync only
     */
} VpsHal_VipStatus;

/**
 *  \brief Structure for VIP crop configuration.
 */
typedef struct
{
    UInt32                  channelNum;
    /**< Port channel number for which this configuration is be applied.  */
    UInt32                  vipCropEnable;
    /**< Enable or disable VIP crop feature. */
    Vps_CropConfig          vipCropCfg;
    /**< VIP crop configuration - crop width, crop height, startX and startY. */
} VpsHal_VipCropConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_vipInit
 *  \brief VIP HAL init function.
 *  This function should be called before calling any of VIP HAL API's.
 *
 *  \param numInstances       Number of instances to initialize.
 *  \param initParams         Pointer to structure VpsHal_VipInstParam.
 *  \param arg                Not used currently. Meant for future purpose.
 *
 *  \return                   success    0
 *                            failure    error value.
 */
Int VpsHal_vipInit(UInt32 numInstances,
                   VpsHal_VipInstParams *initParams,
                   Ptr arg);

/**
 *  VpsHal_vipDeInit
 *  \brief VIP HAL exit function.
 *  Currently this function does not do anything.
 *
 *  \param arg       Not used currently. Meant for future purpose
 *
 *  \return         success    0
 *                  failure    error value
 */
Int VpsHal_vipDeInit(Ptr arg);

/**
 *  VpsHal_vipOpen
 *  \brief Returns the handle to the requested Vip instance.
 *  It is allowed to open only one handle for each vip Instance.
 *
 *  This function should be called prior to calling any of the VIP HAL
 *  configuration APIs.
 *
 *  VpsHal_vipInit should be called prior to this
 *
 *  \param vipInst       Vip Instance
 *  \param vipPort       Vip Port

 *  \return              success     VIP handle
 *                       failure     NULL
 */
VpsHal_VipHandle VpsHal_vipOpen(VpsHal_VipInst vipInst,
                                VpsHal_VipPort vipPort);

/**
 *  VpsHal_vipClose
 *  \brief Closes the Vip HAL
 *  VpsHal_vipInit and Vip_open should be called prior to this.
 *
 *  \param handle       Valid handle returned by VpsHal_vipOpen function.
 *
 *  \return             success    0
 *                      failure    error value
 */
Int VpsHal_vipClose(VpsHal_VipHandle handle);

/**
 *  VpsHal_vipSetVipConfig
 *  \brief Sets the entire Vip configuration to the actual Vip Registers.
 *
 *  VpsHal_vipInit and VpsHal_vipOpen should be called prior to this.
 *
 *  \param vipInst             Vip Instance
 *  \param vipConfig           Pointer to VpsHal_VipConfig structure.
 *                             This parameter should be non-NULL.

 *  \return                    success    0
 *                             failure    error value
 */
Int VpsHal_vipSetVipConfig(VpsHal_VipInst vipInst,
                           const VpsHal_VipConfig *vipConfig);

/**
 *  VpsHal_vipGetVipConfig
 *  \brief Gets the entire Vip configuration from the actual Vip registers.
 *
 *  \param  vipInst          Valid handle returned by VpsHal_vipOpen function.
 *  \param  vipConfig        Pointer to VpsHal_VipConfig structure to be filled
 *                           with register configurations. This parameter
 *                           should be non-NULL.
 *
 *  \return                  success     0
 *                           failure     error value
 */
Int VpsHal_vipGetVipConfig(VpsHal_VipInst vipInst,
                           VpsHal_VipConfig *vipConfig);

/**
 *  VpsHal_vipSetPorConfig
 *  \brief Sets the entire port configuration
 *
 *  \param  handle            Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param  portConfig        Pointer to VpsHal_VipConfig structure.
 *                            This parameter should be non-NULL.
 *
 *  \return                   success    0
 *                            failure    error value
 */
Int VpsHal_vipSetPortConfig(VpsHal_VipHandle handle,
                            const VpsHal_VipPortConfig *portConfig);

/**
 *  VpsHal_vipGetPortConfig
 *  \brief Gets the entire Port configuration from the actual Vip registers.
 *
 *  \param handle            Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param portConfig        Pointer to VpsHal_VipPortConfig structure.
 *                           This parameter should be non-NULL.
 *
 *  \return                  success    0
 *                           failure    error value
 */
Int VpsHal_vipGetPortConfig(VpsHal_VipHandle handle,
                            VpsHal_VipPortConfig *portConfig);

/**
 *  \brief Get the status of Video Source.
 *
 *  \param handle            Valid handle returned by VpsHal_vipOpen function.
 *  \param sid               Source Id Number.
 *  \param vipStatus         Pointer to VpsHal_VipStatus structure.
 *                           This parameter should be non-NULL
 *
 *  \return                  success     0
 *                           failure     error value.
 */
Int VpsHal_vipGetSrcStatus(UInt32 vpdmaCh,
                        VpsHal_VipStatus *vipStatus);

/**
 *  VpsHal_vipIsVideoDetect
 *  Check if proper video sync is found.
 *  This function return whether video sync is found properly or not.
 *
 *  \param  handle           Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param  sid              Source Id Number.
 *
 *  \return                  success    1   proper video sync is found
 *                           failure    0   proper video sync is not found
 */
UInt32 VpsHal_vipIsVideoDetect(VpsHal_VipHandle handle,
                             UInt16 sid);


/**
  \brief Enable/Disable VIP Port

  \param enable [I ] TRUE: enable, FALSE: disable

  \return 0 on success, else failure
*/
Int32 VpsHal_vipPortEnable(VpsHal_VipHandle handle, UInt32 enable);


/**
 *  VpsHal_vipPortReset
 *  Can be used to put VIP port into software reset or remove out of it.
 *
 *  \param  handle           Valid handle returned by VpsHal_vipOpen function.
 *
 *  \param  enable           TRUE to reset / FALSE to remove out of reset.
 *
 *  \return                  success    0   always
 */
Int32 VpsHal_vipPortReset(VpsHal_VipHandle handle, UInt32 enable);

Int32 VpsHal_vipPrintDebugInfo();

Int32 VpsHal_vipInstDisable(VpsHal_VipInst vipInstId);
Int32 VpsHal_vipInstResetFIFO(VpsHal_VipInst vipInstId, Bool assertReset);
Int32 VpsHal_vipInstPortEnable(VpsHal_VipInst vipInstId, VpsHal_VipPort vipPortId, Bool enable, UInt32 *cfgOvlyPtr);
Void VpsHal_vipInstPortEnableCreateOvly(VpsHal_VipInst vipInstId,
                                        VpsHal_VipPort vipPortId,
                                        UInt32 *cfgOvlyPtr);
UInt32 VpsHal_vipInstPortEnableGetCfgOvlySize(VpsHal_VipInst vipInstId, VpsHal_VipPort vipPortId);
Int32 VpsHal_vipInstPortClearFIQ(VpsHal_VipInst vipInstId);
Int32 VpsHal_vipInstPortIsOverflow(VpsHal_VipInst vipInstId, volatile UInt32 *isOverflowPortA, volatile UInt32 *isOverflowPortB);
UInt32 VpsHal_vipPortGetCompleteStatus(VpsHal_VipHandle handle);
Void VpsHal_vipPortClearCompleteStatus(VpsHal_VipHandle handle);

Int32 VpsHal_vipSetVipActCropCfg(VpsHal_VipHandle handle,
                                 const VpsHal_VipCropConfig *vipHalCropCfg);
Int32 VpsHal_vipGetVipActCropCfg(VpsHal_VipHandle handle,
                                 VpsHal_VipCropConfig *vipHalCropCfg);
void VpsHal_vipInstChkOverflowAndDisable(VpsHal_VipInst vipInstId,
                        Bool isBlockRst);
/**
 *  VpsHal_vipGetFidRegAddr
 *  \brief Function used to return the physical address of FID register. 
 *
 *  \param  handle          Valid handle returned by VpsHal_vipOpen function.
 *  \param  reg             Pointer to VIP register
 *  \param  mask            Mask that should be applied to the read value
 *  \param  shift           No of bits to rigth rotate to get the value
 *
 *  \return                 Return 0, on success, negative number otherwise
 */

Int32 VpsHal_vipGetFidRegAddr(VpsHal_VipHandle handle, 
                              volatile UInt32 **reg,
                              UInt32 *mask,
                              UInt32 *shift);

/**
 *  VpsHal_vipGetSrcHeightRegAddr
 *  \brief Function used to return physical address of source height register. 
 *
 *  \param  handle          Valid handle returned by VpsHal_vipOpen function.
 *  \param  reg             Pointer to VIP register
 *  \param  mask            Mask that should be applied to the read value
 *  \param  shift           No of bits to rigth rotate to get the value
 *
 *  \return                 Return 0, on success, negative number otherwise
 */
Int32 VpsHal_vipGetSrcHeightRegAddr(VpsHal_VipHandle handle, 
                                  volatile UInt32 **reg,
                                  UInt32 *mask,
                                  UInt32 *shift);

/**
 *  VpsHal_vipGetSrcWidthRegAddr
 *  \brief Function used to return the physical address of source width register. 
 *
 *  \param  handle          Valid handle returned by VpsHal_vipOpen function.
 *  \param  reg             Pointer to VIP register
 *  \param  mask            Mask that should be applied to the read value
 *  \param  shift           No of bits to rigth rotate to get the value
 *
 *  \return                 Return 0, on success, negative number otherwise
 */
Int32 VpsHal_vipGetSrcWidthRegAddr(VpsHal_VipHandle handle, 
                                  volatile UInt32 **reg,
                                  UInt32 *mask,
                                  UInt32 *shift);
#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_VIPHAL_H */
