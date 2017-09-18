/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_comp.h
 *
 *  \brief VPS Compositor HAL header file
 *  This file exposes the HAL APIs of the VPS Compositor.
 *
 */

#ifndef VPSHAL_COMP_H
#define VPSHAL_COMP_H

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

#define VPSHAL_COMP_STATUS_HDMI_MISS_DATA_MASK (0x00000080u)
/**< Not all layers have data available when VENC request data */
#define VPSHAL_COMP_STATUS_HDMI_MISS_EOL_MASK (0x00000040u)
/**< EOL signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_HDMI_MISS_EOF_MASK (0x00000020u)
/**< EOF signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_HDMI_FAULTY_LAYER_MASK (0x0000001Cu)
/**< The faulty layer */


#define VPSHAL_COMP_STATUS_DVO2_MISS_DATA_MASK (0x00800000u)
/**< Not all layers have data available when VENC request data */
#define VPSHAL_COMP_STATUS_DVO2_MISS_EOL_MASK (0x00400000u)
/**< EOL signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_DVO2_MISS_EOF_MASK (0x00200000u)
/**< EOF signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_DVO2_FAULTY_LAYER_MASK (0x001C0000u)
/**< The faulty layer */


#define VPSHAL_COMP_STATUS_HDCOMP_MISS_DATA_MASK (0x00008000u)
/**< Not all layers have data available when VENC request data */
#define VPSHAL_COMP_STATUS_HDCOMP_MISS_EOL_MASK (0x00004000u)
/**< EOL signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_HDCOMP_MISS_EOF_MASK (0x00002000u)
/**< EOF signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_HDCOMP_FAULTY_LAYER_MASK (0x00001C00u)
/**< The faulty layer */


#define VPSHAL_COMP_STATUS_SD_MISS_DATA_MASK (0x80000000u)
/**< Not all layers have data available when VENC request data */
#define VPSHAL_COMP_STATUS_SD_MISS_EOL_MASK (0x40000000u)
/**< EOL signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_SD_MISS_EOF_MASK (0x20000000u)
/**< EOF signal is missing or does not happen at same pixel */
#define VPSHAL_COMP_STATUS_SD_FAULTY_LAYER_MASK (0x0C000000u)
/**< The faulty layer */

#define VPSHAL_COMP_ERROR_HDMI_MASK (VPSHAL_COMP_STATUS_HDMI_MISS_DATA_MASK | \
                             VPSHAL_COMP_STATUS_HDMI_MISS_EOL_MASK | \
                             VPSHAL_COMP_STATUS_HDMI_MISS_EOF_MASK | \
                             VPSHAL_COMP_STATUS_HDMI_FAULTY_LAYER_MASK)
/**< All possible error for HDMI */


#define VPSHAL_COMP_ERROR_DVO2_MASK (VPSHAL_COMP_STATUS_DVO2_MISS_DATA_MASK | \
                             VPSHAL_COMP_STATUS_DVO2_MISS_EOL_MASK | \
                             VPSHAL_COMP_STATUS_DVO2_MISS_EOF_MASK | \
                             VPSHAL_COMP_STATUS_DVO2_FAULTY_LAYER_MASK)
/**< All possible error for HD_2 */


#define VPSHAL_COMP_ERROR_HDCOMP_MASK (VPSHAL_COMP_STATUS_HDCOMP_MISS_DATA_MASK | \
                                VPSHAL_COMP_STATUS_HDCOMP_MISS_EOL_MASK | \
                                VPSHAL_COMP_STATUS_HDCOMP_MISS_EOF_MASK | \
                                VPSHAL_COMP_STATUS_HDCOMP_FAULTY_LAYER_MASK)
/**< All possible error for HDCOMP */

#define VPSHAL_COMP_ERROR_SD_MASK (VPSHAL_COMP_STATUS_SD_MISS_DATA_MASK | \
                           VPSHAL_COMP_STATUS_SD_MISS_EOL_MASK | \
                           VPSHAL_COMP_STATUS_SD_MISS_EOF_MASK | \
                           VPSHAL_COMP_STATUS_SD_FAULTY_LAYER_MASK)
/**< All possible error for SD */


/** \brief COMP instance 0 */
#define VPSHAL_COMP_INST_0              (0u)
/** \brief COMP maximum number of instances */
#define VPSHAL_COMP_MAX_INST            (1u)

/**
 *  enum VpsHal_CompId
 *  \brief Enum for selction one of Compositor out of four.
 */
typedef enum
{
    VPSHAL_COMP_SELECT_HDMI = 0,
    /**< Select HDMI compositor */
    VPSHAL_COMP_SELECT_HDCOMP,
    /**< Select HD compositor */
    VPSHAL_COMP_SELECT_DVO2,
    /**< Select DVO2 compositor */
    VPSHAL_COMP_SELECT_SD,
    /**< Select SD compositor */
    VPSHAL_COMP_NUM_MAX
} VpsHal_CompId;


/**
 *  enum VpsHal_CompMode
 *  \brief Enum for Comp mode (Interlaced or Progressive video).
 */
typedef enum
{
    VPSHAL_COMP_MODE_PROGRESSIVE = 0,
    /**< Progressive video */
    VPSHAL_COMP_MODE_INTERLACED
    /**< Interlaced video */
} VpsHal_CompMode;

/**
 *  enum VpsHal_CompGlobalReorderMode
 *  \brief Enum for selection of Global reorder.
 */
typedef enum
{
    VPSHAL_COMP_GLOBAL_REORDER_OFF = 0,
    /**< no global reorder.  The graphic layer priority is based priority
    bits [35:32] in the data bus */
    VPSHAL_COMP_GLOBAL_REORDER_ON
    /**< The graphic layer priority is based on g1_reorder, g2_reorder, g3_order
    settings */
} VpsHal_CompGlobalReorderMode;


/**
 *  enum VpsHal_CompDisplayOrder
 *  \brief Enum for display order selection. PLease note alpha blending is done from
 *  bottom to top layer
 */
typedef enum
{
    VPSHAL_COMP_DISPLAY_VID_ORDER = 0,
    /**< Video layer display order.  From low to high: 00, 01, 10, and 11 */
    VPSHAL_COMP_DISPLAY_G0_ORDER,
    /**< Graphic0 layer display order when g_reorder = 1.  From low to high: 00,
     01, 10, and 11 */
    VPSHAL_COMP_DISPLAY_G1_ORDER,
    /**< Graphic1 layer display order when g_reorder = 1.  From low to high: 00,
     01, 10, and 11 */
    VPSHAL_COMP_DISPLAY_G2_ORDER,
    /**< Graphic2 layer display order when g_reorder = 1.  From low to high: 00,
     01, 10, and 11 */
    VPSHAL_COMP_DISPLAY_ORDER_MAX
    /**< Defined to find out maximum*/
} VpsHal_CompDisplayOrder;


/**
 *  enum VpsHal_CompFeedbkPathSelect
 *  \brief Enum for selection of Feedback data
 */
typedef enum
{
    VPSHAL_COMP_OUT_VIDEO_ALPHA_BLENDING = 0,
    /**< select data from video alpha blending */
    VPSHAL_COMP_OUT_FINAL_ALPHA_BLENDING
    /**< select data from final alpha blending */
} VpsHal_CompFeedbkPathSelect;


/**
 * enum VpsHal_CompHdmiBlendBase
 * \brief Enum for selection of blending order of hdmi input video.
 */
typedef enum
{
    VPSHAL_COMP_HDMI_BLEND_BASE_HD_VID =0,
    /**< Use HD_vid as base (lower) layer for video stream  */
    VPSHAL_COMP_HDMI_BLEND_BASE_HD_PIP
    /**< Use HD_PIP as base (lower) layer for video stream */
} VpsHal_CompHdmiBlendBase;


/**
 *  enum VpsHal_CompHdCompBlendBase
 *  \brief Enum for selection of blending order of HdComp input video.
 */
typedef enum
{
    VPSHAL_COMP_HDCOMP_BLEND_BASE_HD_CIT =0,
    /**<Use HD_cit(constraint video) as base (lower) layer for video stream */
    VPSHAL_COMP_HDCOMP_BLEND_BASE_HD_PIP
    /**< Use HD_PIP as base (lower) layer for video stream */
} VpsHal_CompHdCompBlendBase;

/**
 *  enum VpsHal_CompDvo2BlendBase
 *  \brief Enum for selection of blending order of Hd_2 input video.
 */
typedef enum
{
    VPSHAL_COMP_DVO2_BLEND_BASE_HD_VID =0,
    /**< Use HD_vid as base (lower) layer for video stream */
    VPSHAL_COMP_DVO2_BLEND_BASE_HD_PIP
    /**< Use HD_PIP as base (lower) layer for video stream */
} VpsHal_CompDvo2BlendBase;



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/**
 *  struct VpsHal_CompInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 *
 */
typedef struct
{
    UInt32             instId;
    /**< Comp Instance (Instance Identifier)*/
    UInt32             baseAddress;
    /**< Base address of the instance. (Physical register start address for this
         instance) */
} VpsHal_CompInstParams;

/**
 *  struct VpsHal_CompConfigHdmi
 *  \brief Structure containing configuration parameters specific to  HDMI comp
 *  configuration.
 *
 */
typedef struct
{
    VpsHal_CompHdmiBlendBase    hdmiBase;
    /**< Selects lower base for blending */
    UInt32                      enableHdVid;
    /**< enables HD_VID for HDMI compositor */
    UInt32                      enableHdPip;
    /**< enables HD_PIP for HDMI compositor */
} VpsHal_CompConfigHdmi;


/**
 *  struct VpsHal_CompConfigHdComp
 *  \brief Structure containing configuration parameters specific to  HDComp
 *  configuration.
 *
 */
typedef struct
{
    VpsHal_CompHdCompBlendBase      hdCompBase;
    /**< Selects lower base for blending */
    UInt32                          enableHdCit;
    /**< enables HD_CIT for HDComp compositor */
    UInt32                          enableHdPip;
    /**< enables HD_PIP for HDComp compositor */
} VpsHal_CompConfigHdComp;

/**
 *  struct VpsHal_CompConfigDvo2
 *  \brief Structure containing configuration parameters specific to  DVO2
 *  configuration.
 *
 */
typedef struct
{
    VpsHal_CompDvo2BlendBase        dvo2Base;
    /**< Selects lower base for blending */
    UInt32                          enableHdVid;
    /**< enables HD_VID for HDComp compositor */
    UInt32                          enableHdPip;
    /**< enables HD_PIP for HDComp compositor */
} VpsHal_CompConfigDvo2;



/**
 *  struct VpsHal_CompConfigVideoInput
 *  \brief Structure containing selection of input to different compositors
 *
 */
typedef union
{
    VpsHal_CompConfigHdmi       hdmiConfig;
    /**< configuration parameters specific to  HDMI */
    VpsHal_CompConfigHdComp     hdCompConfig;
    /**< configuration parameters specific to  HDComp */
    VpsHal_CompConfigDvo2       dvo2Config;
    /**< configuration parameters specific to  DVO2 */
    UInt32                      enableSd;
    /**< Set to TRUE for enbling SD IN */
} VpsHal_CompConfigVideoInput ;



/**
 *  struct VpsHal_CompConfig
 *  \brief Structure containing configuration parameters for compositor.
 *
 */
typedef struct
{
    VpsHal_CompFeedbkPathSelect   fbPath;
    /**< Selects Feedback data selection */
    VpsHal_CompGlobalReorderMode  gReorderMode;
    /**< Selection between reorder priority between which is based
         on either g1_reorder, g2_reorder, g3_order settings or bits [35:32] in
         the data bus.*/
    VpsHal_CompDisplayOrder       displayOrder[VPSHAL_COMP_DISPLAY_ORDER_MAX];
    /**< decides order of priority between three graphic and video if
         gReorderMode is set 1. It is array of size 4 and user has to configure
         priority.  In the case,if gReorderMode is set 0, user has to provide
         priority for vidoe layer only and for graphic layer, it will be taken
         from settings or bits [35:32] in the data bus.*/
    VpsHal_CompConfigVideoInput   videoInConfig;
    /**< Configuration specific to different compositors */
    UInt32                        grpx0InEnable;
    /**< Used for enabling Graphic 0 */
    UInt32                        grpx1InEnable;
    /**< Used for enabling Graphic 1 */
    UInt32                        grpx2InEnable;
    /**< Used for enabling Graphic 2 */
} VpsHal_CompConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 *  Comp_ErrorCallback
 *  \brief Signature error call back function which could be
 *  regitered while opening different compositors. Compositor does not handle
 *  ISR It has been provided API like VpsHal_CompGetErrorSatus for query the
 *  error for any of compositors out of HDMI, HDCOMP, SD and DVO2. Once this API
 *  is called for any of compsoitors, compositor will call this callbacks for
 *  other comp provided error bit is set. For example, HDMI calls
 *  VpsHal_CompGetErrorSatus and so error value will be returned for HDMI and
 *  call back will be called for other compositors provided error has occured.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  Call back to respective compositors has been called.
 *
 *  \param appHandle  Returns handle regitered during comp_open by
 *                    application.
 *  \param errorCode  Returns error code for repective compositor. This
 *                    errorcode code could have several errors and one bit is
 *                    set for each error. Mask for these bits are difined and
 *                    it is same as STATU regiter of compositor.
 *
 *  \return           void
 */
typedef void (*VpsHal_CompErrorCallback)(Ptr appHandle,UInt32 errorCode);



/**
 *  VpsHal_compInit
 *  \brief COMP HAL init function.
 *
 *  Initializes COMP objects, gets the register overlay offsets for COMP
 *  registers.
 *  This function should be called before calling any of COMP HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of COMP HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param numInstances NONE
 *  \param initParams   NONE
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_compInit(UInt32 numInstances,
                    const VpsHal_CompInstParams *initParams,
                    Ptr arg);



/**
 *  VpsHal_compDeInit
 *  \brief COMP HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_compDeInit(Ptr arg);



/**
 *  VpsHal_compOpen
 *  \brief Returns the handle to the requested COMP instance and
 *  compsoitor/blender(like HDMI, HDCOMP,DVO2,SD). It is allowed to open only
 *  one handle for each of compositors.
 *
 *  This function should be called prior to calling any of the COMP HAL
 *  configuration APIs.
 *
 *  VpsHal_init should be called prior to this.
 *
 *  \param compInst  application handle which will be returned along
 *                   with callback.
 *  \param arg       NONE
 *
 *  \return          Returns COMP handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_compOpen(UInt32 compInst, Ptr arg);



/**
 *  VpsHal_compClose
 *  \brief Closes the COMP HAL and compositor(like HDMI,SD etc) instance
 *
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle  Valid handle returned by VpsHal_compOpen function.
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_compClose(VpsHal_Handle handle);



/**
 *  VpsHal_compGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA
 *  register configuration overlay required to program the COMP registers using
 *  VPDMA config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register overlay
 *  used to program the COMP registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving valuable
 *  CPU in programming the VPS registers.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle Valid handle returned by VpsHal_compOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
UInt32 VpsHal_compGetConfigOvlySize(VpsHal_Handle handle, VpsHal_CompId compId);




 /**
 *  VpsHal_compCreateConfigOvly
 *  \brief Creates the COMP register configuration
 *  overlay using VPDMA helper function.
 *
 *  This function does not initialize the overlay with COMP configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  Comp_setConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  \param handle         Valid handle returned by VpsHal_compOpen function.
 *
 *  \param configOvlyPtr  Pointer to the overlay memory where the overlay is
 *                        formed. The memory for the overlay should be allocated
 *                        by client driver. This parameter should be non-NULL.
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_compCreateConfigOvly(VpsHal_Handle handle,
                                VpsHal_CompId compId,
                                Ptr configOvlyPtr);



/**
 *  Comp_setConfig
 *  \brief Sets the entire COMP configuration to either the
 *  actual COMP registers or to the configuration overlay memory.
 *
 *  This function configures the entire COMP registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual COMP MMR or
 *  to configuration overlay memory.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  COMP registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle        Valid handle returned by VpsHal_compOpen function.
 *
 *  \param config        Pointer to VpsHal_CompConfig structure containing the
 *                       register configurations. This parameter should be
 *                       non-NULL.
 *
 *  \param compId        NONE
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual COMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_compSetConfig(VpsHal_Handle handle,
                         const VpsHal_CompConfig *config,
                         VpsHal_CompId compId,
                         Ptr configOvlyPtr);



/**
 *  Comp_getConfig
 *  \brief Gets the entire COMP configuration from the actual
 *  COMP registers.
 *
 *  This function gets the entire COMP configuration.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle Valid handle returned by VpsHal_compOpen function.
 *
 *  \param config Pointer to VpsHal_CompConfig structure to be filled with
 *                register configurations. This parameter should be non-NULL.
 *  \param compId NONE
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_compGetConfig(VpsHal_Handle handle,
                         VpsHal_CompConfig *config,
                         VpsHal_CompId compId);



/**
 *  VpsHal_compSetBackGrColor
 *  \brief Sets background color.
 *
 *  This backround color is common for all the compositors/blenders(like HDMI,
 *  HDCOMP,DVO2, SD). It means it will affect all compositors/blenders if it is
 *  changed for any of compositors/blenders.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  backround color is set.
 *
 *  \param handle       Valid handle returned by VpsHal_compOpen function.
 *  \param backgrdColor 30 bit backround color in RGB format.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_compSetBackGrColor(VpsHal_Handle handle,
                              UInt32 backgrdColor);



/**
 *  VpsHal_compGetBackGrColor
 *  \brief Gets background color.
 *
 *  This backround color is common for all the compositors/blenders(like HDMI,
 *  HDCOMP,DVO2, SD).
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *
 *  \param handle       Valid handle returned by VpsHal_compOpen function.
 *  \param backgrdColor 30 bit backround color in RGB format.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_compGetBackGrColor(VpsHal_Handle handle,
                              UInt32* backgrdColor);


/**
 *  VpsHal_CompGetErrorSatus
 *  \brief Gets error bits for respective
 *  compositors/blenders. Please note that Compositor does not handle ISR.
 *  This API has been provided API like VpsHal_CompGetErrorSatus for query the error
 *  for any of compositors out of HDMI, HDCOMP, SD and DVO2. Once this API is
 *  called for any of compsoitors/blenders, compositor/blenders will call this
 *  callbacks for other comp provided error bit is set.
 *  For example, HDMI calls VpsHal_CompGetErrorSatus and so error value will be
 *  returned for HDMI and call back will be called for other compositors
 *  provided error has occured.
 *
 *  VpsHal_init and VpsHal_compOpen should be called prior to this.
 *
 *  gets error condition.
 *
 *  \param handle    Valid handle returned by VpsHal_compOpen function.
 *  \param errStatus Error bits will returned. This errStatus code could
 *                   have several errors and one bit is set for each error.
 *                   Mask for these bits are difined and it is same as STATU
 *                   regiter of compositor.
 *
 *  \return          Returns 0 on success else returns error value.
 */
Int VpsHal_CompGetErrorSatus(VpsHal_Handle handle,UInt32 *errStatus);



/**
 *  VpsHal_compEnableOutput
 *  \brief Enable respective compositor/blender.
 *
 *  VpsHal_init, VpsHal_compOpen and Comp_setConfig should be called prior
 *  to this.
 *
 *
 *  \param handle   Valid handle returned by VpsHal_compOpen function.
 *  \param compId        NONE
 *  \param mode     Interlace or progressive
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_compEnableOutput(VpsHal_Handle handle,
                            VpsHal_CompId compId,
                            VpsHal_CompMode mode);



/**
 *  VpsHal_compDisableOutput
 *  \brief Enable respective compositor/blender.
 *
 *
 *  VpsHal_init, VpsHal_compOpen, Comp_setConfig and Comp_disableOuput
 *  should be called prior to this.
 *
 *
 *  \param handle  Valid handle returned by VpsHal_compOpen function.
 *  \param compId  None
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_compDisableOutput(VpsHal_Handle handle, VpsHal_CompId compId);

Int32 VpsHal_compEnableInput(VpsHal_Handle handle,
                             VpsHal_CompId compId,
                             UInt32 inputNum,
                             Ptr ovlyPtr,
                             UInt32 isEnable);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef VPSHAL_COMP_H */
