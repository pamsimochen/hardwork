/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file Vps_dctrlInt.h
 *
 * \brief VPS Display Controller internal header file
 * This file exposes the APIs of the VPS Display Controller to the other
 * drivers.
 *
 */

#ifndef _VPS_DCTRLINT_H
#define _VPS_DCTRLINT_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlDefaultsPriv.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Macro to define size of character array for name */
#define MAX_MODE_NAME                   (20u)

/**
 *  \brief Number of descriptor sets that will be allocated per client.
 *  Currently 2 descriptor sets are used similar to ping pong buffers
 */
#define DC_NUM_DESC_SET                 (2u)

/*
 *  Index to the client descriptor type array for various types in a client.
 *  Caution: Don't change this order/value.
 */
/** \brief Shadow descriptor type index. */
#define DC_SHADOW_IDX                   (0u)
/** \brief Inbound data descriptor type index. */
#define DC_IN_DATA_IDX                  (1u)
/** \brief Outbound data descriptor type index. */
#define DC_OUT_DATA_IDX                 (2u)
/** \brief Non-shadow descriptor type index. */
#define DC_NSHADOW_IDX                  (3u)
/** \brief Mosaic data descriptor type index. */
#define DC_MOSAIC_DATA_IDX              (4u)

/**
 * \brief Number of descriptor types per client per set.
 * 1 Shadow descriptor type.
 * 1 Inbound data descriptor type.
 * 1 Outbound data descriptor type.
 * 1 Non-shadow descriptor type.
 * 1 Mosaic data descriptor type for non-first row mosaic descriptors.
 */
#define DC_DESC_PER_CLIENT_PER_SET      (5u)

/* Typedef for display controller client handle */
typedef Void *DcClientHandle;

/** \brief Aux CSC HAL index */
#define VDC_AUX_CSC_IDX                 (0u)
/** \brief SD CSC HAL index */
#define VDC_SD_CSC_IDX                  (1u)
/** \brief VCOMP HAL index */
#define VDC_VCOMP_IDX                   (2u)
/** \brief EDE HAL index */
#define VDC_EDE_IDX                     (3u)
/** \brief CIG HAL index */
#define VDC_CIG_IDX                     (4u)
/** \brief CPROC HAL index */
#define VDC_CPROC_IDX                   (5u)
/** \brief COMP HAL index */
#define VDC_COMP_IDX                    (6u)
/** \brief SDVENC HAL index */
#define VDC_SD_VENC_IDX                 (7u)
/** \brief HDVENCD0 HAL index */
#define VDC_HDMI_VENC_IDX               (8u)
/** \brief HDCOMP HAL index */
#define VDC_HDCOMP_VENC_IDX             (9u)
/** \brief HDVENCD1 HAL index */
#define VDC_DVO2_VENC_IDX               (10u)
/** \brief SDVENC HAL index */
#define VDC_SD_VENCA_IDX                (11u)
/** \brief VCOMP CSC HAL index */
#define VDC_VCOMP_CSC_IDX               (12u)
/** \brief Maximum Hal Indices */
#define VDC_MAX_HAL                     (13u)


/**
 * enum Dc_NodeNum
 * \brief Enum defining Node Number. Each processing module in the VPS is
 * represented with a node and identified with a number. This enum provides
 * number to each module.
 */
typedef enum Dc_NodeNum_t
{
    DC_NODE_PRI = 30,
    DC_NODE_PRI_MUX = 0,
    DC_NODE_VCOMP_MUX = 1,
    DC_NODE_HDCOMP_MUX = 2,
    DC_NODE_SD_MUX = 3,
    DC_NODE_AUX = 7,
    DC_NODE_BP0 = 8,
    DC_NODE_BP1 = 9,
    DC_NODE_SEC1 = 10,
    DC_NODE_G0 = 11,
    DC_NODE_G1 = 12,
    DC_NODE_G2 = 13,
    DC_NODE_VCOMP = 14,
    DC_NODE_CIG_0 = 15,
    DC_NODE_CIG_1 = 16,
    DC_NODE_HDMI_BLEND = 22,
    DC_NODE_HDCOMP_BLEND = 23,
    DC_NODE_DVO2_BLEND = 24,
    DC_NODE_SD_BLEND = 25,
    DC_NODE_HDMI_VENC = 26,
    DC_NODE_HDCOMP_VENC = 27,
    DC_NODE_DVO2_VENC = 28,
    DC_NODE_SD_VENC = 29
} Dc_NodeNum;

/**
 * Int32 (*Dc_ClientCbFxn) (UInt32 curSet, UInt32 timeStamp, Ptr arg)
 * \brief Client call back function. This function will be called by the
 * DLM when it is required to update buffer pointers in the descriptor.
 */
typedef Int32 (*Dc_ClientCbFxn) (UInt32 curSet, UInt32 timeStamp, Ptr arg);

/**
 * enum Dc_RtConfigModule_t
 * \brief This enum defines module for which runtime configuration is to
 *  be applied. If the input path is connected to both VCOMP and CIG, it
 *  is possible to have runtime configuration in both the module. This enum
 *  tells where to apply runtime configuration when calling Dc_setRtConfig.
 */
typedef enum Dc_RtConfigModule_t
{
    DC_RTCONFIGMODULE_VCOMP = 0,
    /**< Runtime Configuration will be applied to VCOMP overlay */
    DC_RTCONFIGMODULE_CIG,
    /**< Runtime Configuration will be applied to CIG PIP overlay */
    DC_RTCONFIGMODULE_MAX
    /**< This should be the last enum */
} Dc_RtConfigModule;

typedef enum Dc_CscHandleIdx_t
{
    DC_SD_CSC_HANDLE_IDX = 0,
    /**< Index of CSC available on the SDVENC path */
    DC_HD1_CSC_HANDLE_IDX,
    /**< Index of CSC available on the HDCOMP path */
    DC_HD0_CSC_HANDLE_IDX,
    /**< Index of CSC available after Vcomp */
    DC_NUM_CSC_HANDLES
    /**< This should be last enum */
} Dc_CscHandleIdx;

typedef enum Dc_HdVencHandlIdx_t
{
    DC_HDVENC_HDMI_HANDLE_IDX = 0,
    /**< Index of the HDMI Venc Handle */
    DC_HDVENC_HDCOMP_HANDLE_IDX,
    /**< Index of the HDCOMP Venc Handle */
    DC_HDVENC_DVO2_HANDLE_IDX,
    /**< Index of the DVO2 Venc Handle */
    DC_NUM_HDVENC_HANDLES
    /**< This should be last enum */
} Dc_HdVencHandlIdx;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct Dc_InitOutputNode_t
{
    UInt32                     nodeNum;
    UInt32                     vencId;
    VpsHal_Handle             vencHalHandle;
    VpsHal_VpsClkcModule     clkcModule;
    VpsHal_VpsVencOutPixClk    vencOutPixClk;
} Dc_InitOutputNode;

/**
 * struct Dc_InitParams
 * \brief Structure containing init params for the display controller.
 *  This structure contains a set of the handles for the modules handled by the
 *  display controller.
 */
typedef struct Dc_InitParams_t
{
    VpsHal_Handle       halHandle[VDC_MAX_HAL];
    Dc_InitOutputNode   outNode[DC_MAX_OUTPUT];
    Vps_DcVencClkSrc    clkSrc[DC_MAX_OUTPUT];
    UInt32                numClkSrc;
} Dc_InitParams;

/**
 * struct Dc_ClientInfo_t
 * \brief Structure containing client information. All input drivers are
 * considered as clients to the display controller driver. Each client
 * registers with the display controller driver by passing pointer to this
 * structure.
 */
typedef struct Dc_ClientInfo_t
{
    Dc_ClientCbFxn      cbFxn;
    /**< Client call back function, which will be called by DLM to update
         buffer pointers */
    Ptr                 arg;
    /**< Private data of the client */
    UInt32              isFbMode;
    /**< Flag to indicate whether the client is working in frame buffer
         mode or in streaming mode.
         TRUE - Frame buffer mode, FALSE - Streaming mode. */
    Int32 (*started)    (Ptr arg);
    /**< Function pointer pointing to the function, which will be called
         when client can start streaming */
    Int32 (*stopped)    (Ptr arg);
    /**< Function pointer pointing to the function, which will be called
         when client can stop streaming */
    Int32 (*starting)   (Ptr arg, VpsHal_VpdmaFSEvent event);
} Dc_ClientInfo;

/**
 *  struct Dc_DescInfo
 *  \brief Contains parameters required to set the various descriptor memories.
 */
typedef struct
{
    Ptr                     descAddr[DC_NUM_DESC_SET]
                                    [DC_DESC_PER_CLIENT_PER_SET];
    /**< Descriptor start address. */
    UInt32                  descSize[DC_NUM_DESC_SET]
                                    [DC_DESC_PER_CLIENT_PER_SET];
    /**< Size of the descriptors in bytes. This should include the reload
         descriptor memory as well. */
    Ptr                     rldDescAddr[DC_NUM_DESC_SET]
                                       [DC_DESC_PER_CLIENT_PER_SET];
    /**< Pointer to the reload descriptor in the descriptor. Used to link to
         the next descriptor memory. The memory for this descriptor should be
         allocated by the client. */
} Dc_DescInfo;

/**
 *  struct Dc_UpdateDescInfo
 *  \brief Contains parameters required to update the various descriptor
 *  memories for a given set.
 */
typedef struct
{
    Ptr                     descAddr[DC_DESC_PER_CLIENT_PER_SET];
    /**< Descriptor start address. */
    UInt32                  descSize[DC_DESC_PER_CLIENT_PER_SET];
    /**< Size of the descriptors in bytes. This should include the reload
         descriptor memory as well. */
    Ptr                     rldDescAddr[DC_DESC_PER_CLIENT_PER_SET];
    /**< Pointer to the reload descriptor in the descriptor. Used to link to
         the next descriptor memory. The memory for this descriptor should be
         allocated by the client. */
} Dc_UpdateDescInfo;

/**
 * struct Dc_RtConfig_t
 * \brief Structure containing runtime parameters for modifying input
 * video size parameters.
 */
typedef struct Dc_RtConfig_t
{
    UInt32 scanFormat;
    /**< Input Video ScanFormat */
    UInt16 inputWidth;
    /**< Width of the input Video. This is the size of the video given to
         the input node by the upper driver. */
    UInt16 inputHeight;
    /**< Height of the input Video. This is the size of the video given to
         the input node by the upper driver. */
    UInt16 cropStartX;
    /**< Horizontal offset in the input video, from where video is to
         be cropped. If the cropping is not supported, this field will
         be ignored. */
    UInt16 cropStartY;
    /**< Vertical offset in the input video, from where video is to
         be cropped. If the cropping is not supported, this field will
         be ignored. */
    UInt16 cropWidth;
    /**< Horizontal size of the crop window. If the cropping is not
         supported, this field will be ignored. */
    UInt16 cropHeight;
    /**< Vertical size of the crop window. If the cropping is not
         supported, this field will be ignored. */
    UInt16 outCropStartX;
    /**< Horizontal offset in the output window, where input window will be
         put */
    UInt16 outCropStartY;
    /**< Vertical offset in the output window, where input window will be
         put */
} Dc_RtConfig;

/**
 * struct Dc_PathInfo
 * \brief Structure for getting path information for the given client.
 *
 */
typedef struct
{
    Vps_DcModeInfo modeInfo;
    /**< Mode Information */
    UInt32         isMainVcompAvail;
    UInt32         isAuxVcompAvail;
    UInt32         isPipCigAvail;
} Dc_PathInfo;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Dc_init
 *  \brief Function to initialize Display Controller. It initializes all global
 *  variables and keeps it ready. It takes handles to HALs (VCOMP, CIG,
 *  BLEND, VENCs as an arguments) and uses them whenever required.
 *
 *  VCOMP, CIG, COMP, CSC HALs should be initialized and opened before
 *  initializing Display Controller.
 *
 *  \param instParams    Instance specific parameters containing handles of
 *                       modules handled by Display controller
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Dc_init(Dc_InitParams *instParams, Ptr arg);

/**
 *  Dc_deInit
 *  \brief Function to de-initialize Display Controller. It currently does not
 *  do anything.
 *
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Dc_deInit(Ptr arg);

/**
 *  Dc_registerClient
 *  \brief Function to register a client to display controller
 *
 *  This function registers the client callbacks with the display controller.
 *
 *  The client can be either present in the same core where master DC resides
 *  or it can reside in some other core. This is differentiated by the
 *  callback parameter value.
 *  For local client, callback parameter should be non-NULL. For remote client
 *  callback parameter should be NULL.
 *  VpsDlm_init should be called prior to this.
 *  The state for the client changes to registered and appropriate
 *  client objects are allocated.
 *
 *  \param nodeNum      Input Node Number for which this client is to be
 *                      registered
 *  \param clientInfo   Pointer to client info structure containing client
 *                      information
 *  \param arg          Not used currently. For the future use
 *
 *  \return             Handle to the client if registered successfully
 *                      NULL if not able to register
 */
DcClientHandle Dc_registerClient(Dc_NodeNum nodeNum,
                                 Dc_ClientInfo *clientInfo,
                                 Ptr arg);

/**
 *  Dc_unRegisterClient
 *  \brief Unregisters the clients from display controller.
 *
 *  This function inregisters the display controller client and
 *  de-allocates client objects.
 *  If client is started, it cannot be unregistered. It returns error in
 *  this case
 *  Dc_registerClient should be called prior to this call.
 *  Client is freed.
 *
 *  \param handle     Valid client handle returned by Dc_registerClient
 *                    function
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Dc_unRegisterClient(DcClientHandle handle);

/**
 *  Dc_getPathInfo
 *  \brief Function to get the path information.
 *
 *  Input client driver needs to know the size of the frame and frame mode
 *  by which it should pump input frame data. This function returns the
 *  size of the frame and frame mode to the client at the input node.
 *
 *  \param handle     Valid client handle returned by Dc_registerClient
 *                    function
 *  \param pathInfo   Pointer to path information structure returned by
 *                    display controller
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Dc_getPathInfo(DcClientHandle handle, Dc_PathInfo *pathInfo);

/**
 *  Dc_startClient
 *  \brief Adds the client descriptors to the corresponding list
 *  descriptor circular link.
 *
 *  It traverses entire tree starting from this input client's node to VENC's
 *  node and enables input and output paths in appropriate modules. If
 *  possible, it enables paths directly in registers otherwise it enables path
 *  in the display controller's overlay and adds this overlay along with this
 *  client's descriptors to the DLM using VpsDlm_startStopClients function. DLM
 *  adds all of these clients in the list in one go. After this it waits to get
 *  the callback. Once callback is called, it is sure that display controller's
 *  overlay is consumed by VPDMA and registers are set appropriately, so
 *  display controller removes overlay client from the DLM.
 *
 *  If one client has called this function to start, other client, which calls
 *  this function will be blocked untill first one completes.
 *
 *  Client should have be registered and allocated descriptors prior to
 *  this call. It is assumed that the client drivers have actually filled the
 *  data descriptors and configuration descriptors with valid data prior to
 *  calling this function.
 *
 *  The state for the client changes to started.
 *
 *  \param handle     Valid client handle returned by Dc_registerClient
 *                    function
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Dc_startClient(DcClientHandle handle);

/**
 *  Dc_stopClient
 *  \brief Removes the client descriptors from the corresponding list
 *  descriptor circular link and de-allocates list objects.
 *
 *  It traverses entire tree starting from this input client's node to VENC's
 *  node and disables input and output paths in appropriate modules. If
 *  possible, it disables paths directly in registers otherwise it disables path
 *  in the display controller's overlay and adds this overlay and removes this
 *  client's descriptors to the DLM using VpsDlm_startStopClients function. DLM
 *  adds display controller's client and removes descriptors of this client
 *  from the list in one go. After this it waits to get
 *  the callback. Once callback is called, it is sure that display controller's
 *  overlay is consumed by VPDMA and registers are set appropriately, so
 *  display controller removes overlay client from the DLM.
 *
 *  Client should have be started prior to this call.
 *  The state for the client changes to stopped and list object
 *  (if it is last client in the list) is freed.
 *
 *  \param handle      Valid client handle returned by Dc_registerClient
 *                     function
 *
 *  \return            Returns 0 on success else returns error value
 */
Int32 Dc_stopClient(DcClientHandle handle);

/**
 *  Dc_setDescMem
 *  \brief Refer DLM for details.
 */
Int32 Dc_setDescMem(DcClientHandle handle, const Dc_DescInfo *descInfo);

/**
 *  Dc_clrDescMem
 *  \brief Refer DLM for details.
 */
Int32 Dc_clrDescMem(DcClientHandle handle);

/**
 *  Dc_updateDescMem
 *  \brief Refer DLM for details.
 */
Int32 Dc_updateDescMem(DcClientHandle handle,
                       UInt32 descSet,
                       const Dc_UpdateDescInfo *descInfo);

/**
 *  Dc_setInputFrameSize
 *  \brief Function to set the input frame size in the overlay.
 *
 *  This function sets the input video window size in the overlay memory.
 *  If this input path is connected to the VCOMP or CIG, which supports PIP
 *  window, this function must be called before calling startClient function
 *  so that video window size is configured along with enabling input path.
 *  It traverses entire tree starting from the input node from this client and
 *  checks to see if VCOMP or CIG is available in the path. If any of them are
 *  available, it copies dlm handle and overlay pointers from the display
 *  controllers global structure to this clients structure indicating that
 *  this client can have run time configuration.
 *
 *  \param handle     Valid client handle returned by Dc_registerClient
 *                    function
 *  \param rtParams   Pointer to the rtConfig structure containing parameters
 *                    to be configured in VCOMP or in CIG.
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Dc_setInputFrameSize(DcClientHandle handle, Dc_RtConfig *rtConfig);

/**
 *  Dc_setRtConfig
 *  \brief Function to set the Runtime configuration in the overlay memory.
 *
 *  If input node for this client supports runtime configuration for the
 *  given module, it configures overlay memory from the given given pointer
 *  of rtconfig structure
 *
 *  \param handle     Valid client handle returned by Dc_registerClient
 *                    function
 *  \param rtConfigModule Module in which this configuration should be applied.
 *                        This is required to be passed to the display
 *                        controller because it is possible to connect same
 *                        input in VCOMP as well in CIG, so where to apply this
 *                        runtime configuration should be known to the display
 *                        controller.
 *  \param rtParams   Pointer to the rtConfig structure containing parameters
 *                    to be configured in VCOMP or in CIG.
 *
 *  \return           Returns 0 on success else returns error value
 */
Int32 Dc_setRtConfig(DcClientHandle handle,
                     UInt32 descSet,
                     Dc_RtConfigModule rtConfigModule,
                     Dc_RtConfig *rtConfig);

/**
 *  Dc_dlmListCompleteCb
 *  \brief Callback function called by DLM to intimate the DCTRL that a
 *  particular list is complete.
 *
 *  Note: This should be used exclusively by DLM only.
 */
Int32 Dc_dlmListCompleteCb(UInt32 listNum);


Int32 Dc_setDispMode(DcClientHandle handle, UInt32 isFbMode);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPS_DCTRLINT_H */

