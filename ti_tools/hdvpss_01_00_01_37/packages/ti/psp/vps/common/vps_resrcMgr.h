/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 * \file vps_resrcMgr.h
 *
 * \brief VPS Resource Manager header file
 * This file exposes the APIs of the VPS Resource Manager.
 *
 */

#ifndef _VPS_RESRCMGR_H
#define _VPS_RESRCMGR_H

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

/* \brief Enum defining the VPS resources. Display/Capture
 * drivers asks for the specific resource by using these enums. Memory to
 * Memory drivers should ask for two resource i.e. one input and other output
 * resource. In order to use resource, driver should ask for the resource to
 * the resource Manager at the time of opening and then, driver can use the
 * resource exclusively if driver gets that resource.
 * Warning: This enum is used as an index into the array so there
 * should not be any enum with the negative number. */
typedef enum
{
    VRM_RESOURCE_INVALID = 0,
    /**< Invalid Resource ID */
    VRM_RESOURCE_PRI_VIDEO_PATH,
    /**< Primary Input Path (or Primary Display Path) */
    VRM_RESOURCE_AUX_VIDEO_PATH,
    /**< Auxiliary Input Path (or Auxiliary Display Path) */
    VRM_RESOURCE_BP0_PATH,
    /**< Primary Memory Input path from VPDMA */
    VRM_RESOURCE_BP1_PATH,
    /**< Auxiliary Memory Input path from VPDMA */
    VRM_RESOURCE_GRPX0_PATH,
    /**< Graphics-1 Input path */
    VRM_RESOURCE_GRPX1_PATH,
    /**< Graphics-2 Input path */
    VRM_RESOURCE_GRPX2_PATH,
    /**< Graphics-3 Input path */
    VRM_RESOURCE_WB0_PATH,
    /**< Primary Writeback path */
    VRM_RESOURCE_WB1_PATH,
    /**< Auxiliary Writeback path */
    VRM_RESOURCE_SC5_WB2_PATH,
    /**< Scalar Write back path from VCOMP */
    VRM_RESOURCE_SEC0_PATH,
    /**< Secondary-0 input path */
    VRM_RESOURCE_SEC1_PATH,
    /**< Secondary-1 input path */
    VRM_RESOURCE_NF0,
    /**< Noise Filter */
    VRM_RESOURCE_VBI_HD_PATH,
    /**< VBI Path to HD Venc */
    VRM_RESOURCE_VBI_SD_PATH,
    /**< VBI Path to SD Venc */
    VRM_RESOURCE_HD0_PATH,
    /**< HD 0path in Display Controller. This includes VCOMP, CIG_1,
         CPROC, VCOMP_MUX and PRI_MUX */
    VRM_RESOURCE_HD1_PATH,
    /**< HD1 path in Display Controller. This includes
         HDCOMP_MUX, CIG_2 and CSC */
    VRM_RESOURCE_SD_PATH,
    /**< SD path in Display Controller. This includes
         SDVENC_MUX and CSC */
    VRM_RESOURCE_HDCOMP_MUX,
    /**< HDCOMP/DVO2 input multiplexer */
    VRM_RESOURCE_SD_MUX,
    /**< SDVENC input multiplexer */
    VRM_RESOURCE_CIG,
    /**< Constrained Image Generator */
    VRM_RESOURCE_HDMI_BLEND,
    /**< HDMI Blender */
    VRM_RESOURCE_HDCOMP_BLEND,
    /**< HDCOMP Blender */
    VRM_RESOURCE_DVO2_BLEND,
    /**< DVO2 Blender */
    VRM_RESOURCE_SD_BLEND,
    /**< SD Venc Blender */
    VRM_RESOURCE_VIP0_PARSER_PORT_A,
    /**< VIP: Port A input */
    VRM_RESOURCE_VIP0_PARSER_PORT_A_ANC,
    /**< VIP:  Port A ancillary output */
    VRM_RESOURCE_VIP0_PARSER_PORT_B,
    /**< VIP:  Port B input */
    VRM_RESOURCE_VIP0_PARSER_PORT_B_ANC,
    /**< VIP:  Port B ancillary output */
    VRM_RESOURCE_VIP0_SEC_PATH,
    /**< VIP:  Secondary input */
    VRM_RESOURCE_VIP0_COMP_RGB,
    /**< VIP:  COMP input */
    VRM_RESOURCE_VIP0_CSC,
    /**< VIP:  CSC block  */
    VRM_RESOURCE_VIP0_SC,
    /**< VIP:  SC block */
    VRM_RESOURCE_VIP0_CHR_DS_0,
    /**< VIP:  Chroma DS 0 block */
    VRM_RESOURCE_VIP0_CHR_DS_1,
    /**< VIP:  Chroma DS 1 block */
    VRM_RESOURCE_VIP0_Y_UP,
    /**< VIP:  Y up output  */
    VRM_RESOURCE_VIP0_UV_UP,
    /**< VIP:  UV up output  */
    VRM_RESOURCE_VIP0_Y_LOW,
    /**< VIP:  Y low output */
    VRM_RESOURCE_VIP0_UV_LOW,
    /**< VIP:  UV low output  */
    VRM_RESOURCE_VIP1_PARSER_PORT_A,
    /**< VIP: Port A input */
    VRM_RESOURCE_VIP1_PARSER_PORT_A_ANC,
    /**< VIP:  Port A ancillary output */
    VRM_RESOURCE_VIP1_PARSER_PORT_B,
    /**< VIP:  Port B input */
    VRM_RESOURCE_VIP1_PARSER_PORT_B_ANC,
    /**< VIP:  Port B ancillary output */
    VRM_RESOURCE_VIP1_SEC_PATH,
    /**< VIP:  Secondary input */
    VRM_RESOURCE_VIP1_COMP_RGB,
    /**< VIP:  COMP input */
    VRM_RESOURCE_VIP1_CSC,
    /**< VIP:  CSC block  */
    VRM_RESOURCE_VIP1_SC,
    /**< VIP:  SC block */
    VRM_RESOURCE_VIP1_CHR_DS_0,
    /**< VIP:  Chroma DS 0 block */
    VRM_RESOURCE_VIP1_CHR_DS_1,
    /**< VIP:  Chroma DS 1 block */
    VRM_RESOURCE_VIP1_Y_UP,
    /**< VIP:  Y up output  */
    VRM_RESOURCE_VIP1_UV_UP,
    /**< VIP:  UV up output  */
    VRM_RESOURCE_VIP1_Y_LOW,
    /**< VIP:  Y low output */
    VRM_RESOURCE_VIP1_UV_LOW,
    /**< VIP:  UV low output  */
    VRM_RESOURCE_SEC0_MUX,
    /**< VIP:  UV low output  */
    VRM_RESOURCE_SEC1_MUX,
    /**< VIP:  UV low output  */
    VRM_RESOURCE_MAX
    /**< This must be the last enum */
} Vrm_Resource;


/**
    \brief Type of driver requesting the list
*/
typedef enum
{
    VRM_LIST_TYPE_CAPTURE,
    /**< Capture driver requesting the list */

    VRM_LIST_TYPE_DISPLAY,
    /**< Display driver requesting the list */

    VRM_LIST_TYPE_M2M
    /**< M2M driver requesting the list */

} Vrm_ListType;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vrm_init
 *  \brief Function to initialize VPS resource manager. It initializes
 *  all global variables and keeps it ready.
 *
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_init(Ptr arg);

/**
 *  Vrm_deInit
 *  \brief Function to de-initialize VPS resource manager.
 *
 *  \param arg           Currently not used. For the future reference
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_deInit(Ptr arg);

/**
 *  Vrm_allocResource
 *  \brief Function to allocate given resource and mark it as allocated. The
 *  first caller, who asks for a perticular resource, will get the resource.
 *  all subsequent callers will not be able to get the same resource. It
 *  returns pointer to the allocated resource.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param resource      Resource to be allocated
 *
 *  \return              Returns 0 if resource is allocated to the caller or -1.
 */
Int32 Vrm_allocResource(Vrm_Resource resource);

/**
 *  Vrm_releaseResource
 *  \brief Function to free the allocated resource. It marks the resource,
 *  whose handle is provided, as free resource and another caller can ask
 *  for the same resource.
 *
 *  Vrm_init and allocResource API should be called before calling this API
 *
 *  \param resource      Resource to be freed
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseResource(Vrm_Resource resource);

/**
 *  Vrm_allocFreeChannel
 *  \brief Function to get the free VPDMA channel. It returns success if it
 *  is able to find free VPDMA channel and marks it as allocated.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param channel       Array of allocated VPDMA channels returned from
 *                       this call
 *  \param numChannels   number of channels to be allocated
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_allocFreeChannel(UInt32 *channels, UInt32 numChannels);

/**
 *  Vrm_releaseFreeChannel
 *  \brief Function to release free VPDMA channel and mark it as free.
 *
 *  Vrm_init and Vrm_allocFreeChannel function should be called before
 *  calling this function.
 *
 *  \param channel       Array of VPDMA channels to be freed
 *  \param numChannels   number of channels to be freed
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseFreeChannel(UInt32 *channels, UInt32 numChannels);

/**
 *  Vrm_allocList
 *  \brief Function to get the free VPDMA list. It returns success if it
 *  is able to find free VPDMA list and marks it as allocated.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param listNum       VPDMA free list number
 *  \param listType      Depending on type of driver this API coudl allocate
 *                       different list
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_allocList(UInt8 *listNum, Vrm_ListType listType);

/**
 *  Vrm_releaseList
 *  \brief Function to release VPDMA list number and mark it as free.
 *
 *  Vrm_init and Vrm_allocList function should be called before
 *  calling this function.
 *
 *  \param listNum       VPDMA list number
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseList(UInt8 listNum);

/**
 *  Vrm_allocLmFid
 *  \brief Function to get the free LM FID. It returns success if it
 *  is able to find free LM FID and marks it as allocated.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param lmFidNum      Free LM FID
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_allocLmFid(UInt32 *lmFidNum);

/**
 *  Vrm_releaseLmFid
 *  \brief Function to release VPDMA LM FID and mark it as free.
 *
 *  Vrm_init and Vrm_allocList function should be called before
 *  calling this function.
 *
 *  \param lmFidNum      LM FID Number
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseLmFid(UInt32 lmFidNum);

/**
 *  Vrm_allocSendIntr
 *  \brief Function to get the free Send Insterrupt. It returns success if it
 *  is able to find free LM FID and marks it as allocated.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param si           Array of allocated send interrupt
 *  \param numSendIntr  number of send interrupt requested.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_allocSendIntr(UInt32 *si, UInt32 numSendIntr);

/**
 *  Vrm_releaseSendIntr
 *  \brief Function to release VPDMA LM FID and mark it as free.
 *
 *  Vrm_init and Vrm_allocList function should be called before
 *  calling this function.
 *
 *  \param lmFidNum      LM FID Number
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseSendIntr(UInt32 *si, UInt32 numSendIntr);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPS_RESRCMGR_H */
