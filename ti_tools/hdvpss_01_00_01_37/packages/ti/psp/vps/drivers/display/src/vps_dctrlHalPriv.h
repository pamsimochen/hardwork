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
 * This file exposes the HAL APIs of the VPS Display Controller to the main
 * display Controller driver.
 *
 */

#ifndef _VPS_DCTRLHAL_H
#define _VPS_DCTRLHAL_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* This defines maximum number of nodes in a resource */
#define DC_MAX_RESOURCE_NODES               (5u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * struct Dc_ResMngr_t
 * \brief This structure keeps track of resources, nodes included
 *  in a resources and state of the resource.
 */
typedef struct Dc_ResMngr_t
{
    Vrm_Resource resource;
    /**< Indicates the Resource */
    UInt32       isAllocated;
    /**< Flag to keep track whether resource is allocated or not */
    UInt32       useCnt;
    /**< Counter to keep track of how many nodes are using this resource.
         The resource will be free in dmDeAlllocResource only
         when this counter is zero. */
    UInt32       nodes[DC_MAX_RESOURCE_NODES];
    /**< Array of nodes, which are part of this resource */
    UInt32       numNodes;
    /**< Number of valid nodes in nodeNum array */
} Dc_ResMngr;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief Function to set the mode information in Blender
 *
 * \param nodeNum     [IN] Node Number to identify blender
 * \param modeInfo    [IN] mode information structure containing mode to be set
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCompSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo);

/**
 * \brief Function to set the mode information in VCOMP. It configures EDE and
 *        CPROC also depending on the mode information.
 *
 * \param nodeNum     [IN] VCOMP Node Number
 * \param modeInfo    [IN] mode information structure containing mode to be set
 *
 * \return 0 on success -1 on error.
 */
Int32 dcVcompSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo);

/**
 * \brief Function to set the mode information in Mux. It configures
 *        CSC also depending on the mode.
 *
 * \param nodeNum     [IN] Node Number to identify the multiplexer
 * \param modeInfo    [IN] mode information structure containing mode to be set
 *
 * \return 0 on success -1 on error.
 */
Int32 dcMuxSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo);

/**
 * \brief Function to set the mode information in CIG.
 *
 * \param nodeNum     [IN] CIG Node Number
 * \param modeInfo    [IN] mode information structure containing mode to be set
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCigSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo);

/**
 * \brief Function to enable/disable given input in multiplexer.
 *
 * \param nodeNum     [IN] Node Number to identify the multiplexer
 * \param inputNum    [IN] input number to identify which input to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable input
 *
 * \return 0 on success -1 on error.
 */
Int32 dcMuxSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given output in multiplexer. Currently
 *        it does not do anything.
 *
 * \param nodeNum     [IN] Node Number to identify the multiplexer
 * \param inputNum    [IN] output number to identify which output to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable output
 *
 * \return 0 on success -1 on error.
 */
Int32 dcMuxSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given input in Vcomp.
 *
 * \param nodeNum     [IN] Vcomp Node Number
 * \param inputNum    [IN] input number to identify which input to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable input
 *
 * \return 0 on success -1 on error.
 */
Int32 dcVcompSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given output in VComp. Currently
 *        it does not do anything.
 *
 * \param nodeNum     [IN] Vcomp Node Number
 * \param inputNum    [IN] output number to identify which output to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable output
 *
 * \return 0 on success -1 on error.
 */
Int32 dcVcompSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given input in Comp/Blender.
 *
 * \param nodeNum     [IN] Node Number to identify Blender
 * \param inputNum    [IN] input number to identify which input to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable input
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCompSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given output in Comp/Blender. Currently
 *        it does not do anything. Comp output is enabled as part of
 *        setting mode in the Venc.
 *
 * \param nodeNum     [IN] Node Number to identify Blender
 * \param inputNum    [IN] output number to identify which output to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable output
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCompSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given input in CIG. Currently it just used
 *        for enabling pip input.
 *
 * \param nodeNum     [IN] CIG Node Number
 * \param inputNum    [IN] input number to identify which input to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable input
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCigSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Function to enable/disable given output in CIG. Currently it doesnot
 *        do anything.
 *
 * \param nodeNum     [IN] CIG Node Number
 * \param inputNum    [IN] output number to identify which output to be enabled
 * \param enable      [IN] flag to indicate whether to enable/disable output
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCigSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);

/**
 * \brief Configures the opentime configuration in HAL controlled by
 *        Display Controller.
 *
 * \param openConfig  [IN] Pointer to structure containing open time params
 * \param initParams  [IN] Pointer to init params containing Hal Handles
 *
 * \return 0 on success -1 on error.
 */
Int32 dcConfigHal(Vps_DcCreateConfig *createConfig, Dc_InitParams *initParams);

/**
 * \brief Function to set Runtime configuration in Vcomp. This
 *        includes Cropping and positioning parameters.
 *
 * \param initParams    [IN] Pointer to init params containing Hal Handles
 * \param vcompRtConfig [IN] Pointer to the structure Containing
 *                           Runtime configuration for Vcomp
 *
 * \return 0 on success -1 on error.
 */
Int32 dcVcompSetRtConfig(Dc_InitParams *initParams,
                         Vps_DcVcompRtConfig *vcompRtConfig);

/**
 * \brief Function to get Runtime configuration in Vcomp.
 *
 * \param initParams    [IN] Pointer to init params containing Hal Handles
 * \param vcompRtConfig [OUT] Pointer to the structure Containing
 *                           Runtime configuration for Vcomp
 *
 * \return 0 on success -1 on error.
 */
Int32 dcVcompGetRtConfig(Dc_InitParams *initParams,
                         Vps_DcVcompRtConfig *vcompRtConfig);

/**
 * \brief Function to set Runtime configuration in CIG. This includes
 *  setting transparency and alpha values for the output video
 *
 * \param initParams    [IN] Pointer to init params containing Hal Handles
 * \param cigRtConfig   [IN] Pointer to the structure in which
 *                           Runtime configuration for CIG will be returned
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCigSetRtConfig(Dc_InitParams *initParams,
                       Vps_DcCigRtConfig *cigRtConfig);

/**
 * \brief Function to get Runtime configuration in CIG.
 *
 * \param initParams    [IN] Pointer to init params containing Hal Handles
 * \param cigRtConfig   [OUT] Pointer to the structure in which runtime
 *                            config will be returned
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCigGetRtConfig(Dc_InitParams *initParams,
                       Vps_DcCigRtConfig *cigRtConfig);

/**
 * \brief Function to set Runtime configuration in COMP. This
 *        includes Priority of the input windows.
 *
 * \param initParams    [IN] Pointer to init params containing Hal Handles
 * \param compRtConfig  [IN] Pointer to the structure Containing
 *                           Runtime configuration for COMP
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCompSetRtConfig(Dc_InitParams *initParams,
                        Vps_DcCompRtConfig *compRtConfig);

/**
 * \brief Function to get Runtime configuration in COMP. This
 *        includes Priority of the input windows.
 *
 * \param initParams    [IN] Pointer to init params containing Hal Handles
 * \param compRtConfig  [OUT] Pointer to the structure in which
 *                           Runtime configuration for COMP will be returned
 *
 * \return 0 on success -1 on error.
 */
Int32 dcCompGetRtConfig(Dc_InitParams *initParams,
                        Vps_DcCompRtConfig *compRtConfig);


/**
 * \brief Function to allcoate resource for the given node number.
 *  It checks whether resource required is allocated or not. If it is
 *  not, it will ask resource manager to allocate the resource.
 *
 * \param initParams    [IN] Node Number for which resource is to be allocated
 *
 * \return 0 on success -1 on error.
 */
Int32 dcAllocResource(UInt32 nodeNum);

/**
 * \brief Function to de-allcoate resource for the given node number.
 *  It checks whether resource required is allocated or not. If it is
 *  , it will ask resource manager to de-allocate the resource.
 *
 * \param initParams    [IN] Node Number for which resource is to be allocated
 *
 * \return 0 on success -1 on error.
 */
Int32 dcDeAllocResource(UInt32 nodeNum);

/**
 * \brief Function to allcoate resource for the given node number.
 *  It checks whether resource required is allocated or not. If it is
 *  not, it will ask resource manager to allocate the resource.
 *
 * \param initParams    [IN] Node Number for which resource is to be allocated
 *
 * \return 0 on success -1 on error.
 */
Int32 dcAllocEdgeResource(const Vps_DcEdgeInfo *edgeInfo);

/**
 * \brief Function to de-allcoate resource for the given node number.
 *  It checks whether resource required is allocated or not. If it is
 *  , it will ask resource manager to de-allocate the resource.
 *
 * \param initParams    [IN] Node Number for which resource is to be allocated
 *
 * \return 0 on success -1 on error.
 */
Int32 dcDeAllocEdgeResource(const Vps_DcEdgeInfo *edgeInfo);

Int32 dcVcompEnableInput(Vps_DcNodeInput *nodeInfo, Ptr ovlyPtr);

#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPS_DCTRLHAL_H */

