/** ==================================================================
 *  @file   system.h                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _SYSTEM_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SYSTEM_H_

/**
    \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
    \defgroup VPSEXAMPLE_SYSTEM_LINK_API System API

    The API defined in this module are used to create links and connect
    then to each other to form a chain

    @{
*/

#include <string.h>
#include <stdio.h>
#include <ti/psp/vps/vps.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/common/iss/chains/links/system_debug.h>
#include <ti/psp/examples/common/iss/chains/links/system_config.h>

/**
    \file system.h
    \brief Links and chain - System API
*/

// #define SYSTEM_PRINT_VIP_RES_LOG
// #define SYSTEM_PRINT_TILER_LOG

// #define SYSTEM_USE_VIDEO_DETECT_INFO

/** \brief Indicates whether tiler is to be used */
#define SYSTEM_USE_TILER

#ifdef PLATFORM_EVM_SI

#ifndef PLATFORM_ZEBU
/** \brief Indicates whether off-chip HDMI is to be used */
#define SYSTEM_USE_OFF_CHIP_HDMI
/** \brief Indicates whether on-chip HDMI is to be used */
#define SYSTEM_USE_ON_CHIP_HDMI
/** \brief Indicates whether Video decoder is to be used */
#define SYSTEM_USE_VIDEO_DECODER
/** \brief Indicates whether Video detect is to be done */
// #define SYSTEM_DO_VIDEO_DETECT
#endif

#endif

/** \brief Link ID for capture */
#define SYSTEM_LINK_ID_CAMERA              ( 0)

/** \brief Link ID for NSF 0 */
#define SYSTEM_LINK_ID_NSF_0                ( 1)
/** \brief Link ID for NSF 1 */
#define SYSTEM_LINK_ID_NSF_1                ( 2)

#ifdef TI_816X_BUILD
/** \brief Link ID for DEI_HQ 0 */
#define SYSTEM_LINK_ID_DEI_HQ_0             ( 3)
/** \brief Link ID for DEI_HQ 1 */
#define SYSTEM_LINK_ID_DEI_HQ_1             ( 4)
#endif                                                     /* TI_816X_BUILD */

/** \brief Link ID for DEI 0 */
#define SYSTEM_LINK_ID_DEI_0                ( 5)
/** \brief Link ID for DEI 1 */
#define SYSTEM_LINK_ID_DEI_1                ( 6)

/** \brief Link ID for DEI_MD1 0 */
#define SYSTEM_LINK_ID_DEI_MD1_0            ( 7)
/** \brief Link ID for DEI_MD1 1 */
#define SYSTEM_LINK_ID_DEI_MD1_1            ( 8)

/** \brief Link ID for SCALAR_SW_MS 0 */
#define SYSTEM_LINK_ID_SCALAR_SW_MS_0       ( 9)
/** \brief Link ID for SCALAR_SW_MS 1 */
#define SYSTEM_LINK_ID_SCALAR_SW_MS_1       (10)
/** \brief Link ID for SCALAR_SW_MS 2 */
#define SYSTEM_LINK_ID_SCALAR_SW_MS_2       (11)
/** \brief Link ID for SCALAR_SW_MS 3 */
#define SYSTEM_LINK_ID_SCALAR_SW_MS_3       (12)

/** \brief Link ID for SCALAR 0 */
#define SYSTEM_LINK_ID_SCALAR_0             (13)
/** \brief Link ID for SCALAR 1 */
#define SYSTEM_LINK_ID_SCALAR_1             (14)

/** \brief Link ID for DISPLAY 0 */
#define SYSTEM_LINK_ID_DISPLAY_0            (15)
/** \brief Link ID for DISPLAY 1 */
#define SYSTEM_LINK_ID_DISPLAY_1            (16)
/** \brief Link ID for DISPLAY 2 */
#define SYSTEM_LINK_ID_DISPLAY_2            (17)

/** \brief Link ID for DISPLAY_HW_MS 0 */
#define SYSTEM_LINK_ID_DISPLAY_HW_MS_0      (18)
/** \brief Link ID for DISPLAY_HW_MS 1 */
#define SYSTEM_LINK_ID_DISPLAY_HW_MS_1      (19)

/** \brief Link ID for GRPX 0 */
#define SYSTEM_LINK_ID_GRPX_0               (20)
/** \brief Link ID for GRPX 1 */
#define SYSTEM_LINK_ID_GRPX_1               (21)
/** \brief Link ID for GRPX 2 */
#define SYSTEM_LINK_ID_GRPX_2               (22)

/** \brief Link ID for NULL 0 */
#define SYSTEM_LINK_ID_NULL_0               (23)
/** \brief Link ID for NULL 1 */
#define SYSTEM_LINK_ID_NULL_1               (24)
/** \brief Link ID for NULL 2 */
#define SYSTEM_LINK_ID_NULL_2               (25)

/** \brief Link ID for VIDEO_SRC 0 */
#define SYSTEM_LINK_ID_VIDEO_SRC_0          (26)
/** \brief Link ID for VIDEO_SRC 1 */
#define SYSTEM_LINK_ID_VIDEO_SRC_1          (27)

/** \brief Link ID for CAMURE_UT */
#define SYSTEM_LINK_ID_CAMERA_UT           (28)

/** \brief Link ID for DUP 0 */
#define SYSTEM_LINK_ID_DUP_0                (29)
/** \brief Link ID for DUP 1 */
#define SYSTEM_LINK_ID_DUP_1                (30)

/** \brief Link ID for SW_MS_DEI 0 */
#define SYSTEM_LINK_ID_SW_MS_DEI_0          (31)
/** \brief Link ID for SW_MS_DEI_HQ 0 */
#define SYSTEM_LINK_ID_SW_MS_DEI_HQ_0       (32)

/** \brief Link ID for SW_MS_SC 0 */
#define SYSTEM_LINK_ID_SW_MS_SC_0           (33)
/** \brief Link ID for SW_MS_SC 1 */
#define SYSTEM_LINK_ID_SW_MS_SC_1           (34)
/** \brief Link ID for SW_MS_SC 2 */
#define SYSTEM_LINK_ID_SW_MS_SC_2           (35)
/** \brief Link ID for SW_MS_SC 3 */
#define SYSTEM_LINK_ID_SW_MS_SC_3           (36)

/** \brief Maximum number of link IDs */
#define SYSTEM_LINK_ID_MAX                  (37)

/* Pre-defined window layout types */
/** \brief Pre-defined window layout mode for 1 channel */
#define SYSTEM_LAYOUT_MODE_1CH              (0)
/** \brief Pre-defined window layout mode for 4 channels */
#define SYSTEM_LAYOUT_MODE_4CH              (1)
/** \brief Pre-defined window layout mode for 8 channels */
#define SYSTEM_LAYOUT_MODE_8CH              (2)
/** \brief Pre-defined window layout mode for 16 channels */
#define SYSTEM_LAYOUT_MODE_16CH             (3)
/** \brief Pre-defined window layout mode for 5 channels + 1 channel */
#define SYSTEM_LAYOUT_MODE_5CH_PLUS_1CH     (4)
/** \brief Pre-defined window layout mode for 7 channels + 1 channel */
#define SYSTEM_LAYOUT_MODE_7CH_PLUS_1CH     (5)
/** \brief Pre-defined window layout mode for 1 channel + 2 channels PIP */
#define SYSTEM_LAYOUT_MODE_1CH_PLUS_2CH_PIP (6)
/** \brief Maximum number of pre-defined window layout modes */
#define SYSTEM_LAYOUT_MODE_MAX              (7)

/* Pre-defined resolution types */
/** \brief Pre-defined resolution type for 1080p30 */
#define SYSTEM_DISPLAY_RES_1080P30  (0)
/** \brief Pre-defined resolution type for 1080i60 */
#define SYSTEM_DISPLAY_RES_1080I60  (1)
/** \brief Pre-defined resolution type for 1080p60 */
#define SYSTEM_DISPLAY_RES_1080P60  (2)
/** \brief Pre-defined resolution type for 720p60 */
#define SYSTEM_DISPLAY_RES_720P60   (3)
/** \brief Pre-defined resolution type for NTSC */
#define SYSTEM_DISPLAY_RES_NTSC     (4)
/** \brief Pre-defined resolution type for PAL */
#define SYSTEM_DISPLAY_RES_PAL      (5)
/** \brief Maximum number of pre-defined resolution types */
#define SYSTEM_DISPLAY_RES_MAX      (6)

/**
    \brief In queue params
*/
typedef struct {
    UInt32 prevLinkId;
    /**< Previous link ID to which current link will be connected */

    UInt32 prevLinkQueId;
    /**< Previous link Que ID, with which current link
        will exchange frames
    */

} System_LinkInQueParams;

/**
    \brief Out queue params
*/
typedef struct {
    UInt32 nextLink;
    /**< Next link ID to which current link will be connected */

} System_LinkOutQueParams;

/**
    \brief Create a link

    Note, links of a chain should be create in
    start (source) to end (sink) order.

    Example, in a capture + display chain.

    The capture link is the source and should be created
    before the display link which is the sink for the source data.

    Create only create the link, driver, buffer memory
    and other related resources.

    Actual data transfer is not started when create is done.

    \param linkId       [IN] link ID
    \param createArgs   [IN] Create time link specific arguments

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_linkCreate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_linkCreate(UInt32 linkId, Ptr createArgs);

/**
    \brief Start the link

    The makes the link start generating or consuming data.

    Note, the order of starting links of a chain depend on
    specific link implementaion.

    \param linkId       [IN] link ID

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_linkStart                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_linkStart(UInt32 linkId);

/**
    \brief Stop the link

    The makes the link stop generating or consuming data.

    Note, the order of starting links of a chain depend on
    specific link implementaion.

    \param linkId       [IN] link ID

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_linkStop                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_linkStop(UInt32 linkId);

/**
    \brief Delete the link

    A link must be in stop state before it is deleted.

    The links of a chain can be deleted in any order.

    \param linkId       [IN] link ID

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_linkDelete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_linkDelete(UInt32 linkId);

/**
    \brief Send a control command to a link

    The link must be created before a control command could be sent.
    It need not be in start state for it to be able to received
    a control command

    \param linkId       [IN] link ID
    \param cmd          [IN] Link specific command ID
    \param pPrm         [IN] Link specific command parameters
    \param waitAck      [IN] TRUE: wait until link ACKs the sent command,
                             FALSE: return after sending command

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_linkControl                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_linkControl(UInt32 linkId, UInt32 cmd, Void * pPrm, Bool waitAck);

/**
    \brief Start the links and chain system

    This API is typically called in main() just before BIOS_start().

    This API creates a system task that starts executing immediately when BIOS_start()
    gets called.

    This API does the below system init
    - FVID2 init
    - Buffer memory allocator init
    - Optional tiler memory allocator init
    - EVM specific init
    - System task specific init
    - Mailbox, other utility functionality init

    After completing this init it calls the user supplied
    'chainsMainFunc'

    Inside the 'chainsMainFunc' user should init the required links.
    Use System_linkXxxx APIs to connect, create, start and control a chains of links

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_start                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_start(Task_FuncPtr chainsMainFunc);

/**
    \brief Init display controller related resources

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_displayCtrlInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_displayCtrlInit(UInt32 displayRes, UInt32 sdDisplayRes);

/**
    \brief De-init display controller related resources

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_displayCtrlDeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 System_displayCtrlDeInit();

/**
    \brief Print the memory heap status
*/
/* ===================================================================
 *  @func     System_memPrintHeapStatus                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void System_memPrintHeapStatus();

/**
    \brief Resume execution

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_resumeExecution                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int System_resumeExecution();

/**
    \brief Halt execution

    \return FVID2_SOK on success
*/
/* ===================================================================
 *  @func     System_haltExecution                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int System_haltExecution();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */

/**
    \ingroup VPSEXAMPLE_API
    \defgroup VPSEXAMPLE_LINKS_AND_CHAIN_API Sample Example - Links and Chain API

    Links and chains APIs allow user to connect different drivers in a
    logical consistant way inorder to make a chain of data flow.

    Example,
    Camure + NSF + DEI + SC + Display
    OR
    Camure + Display

    A link is basically a task which exchange frames with other links and
    makes FVID2 driver  calls to process the frames.

    A chain is a connection of links.

    Links exchange frames with each other via buffer queue's.

    Links exchange information with each other and the top level system task
    via mail box.

    When a link is connected to another link, it basically means output queue of
    one link is connected to input que of another link.

    All links have a common minimum interface which makes it possible for a link
    to exchange frames with another link without knowing the other links specific
    details. This allow the same link to connect to different other links in
    different data flow scenario's

    Example,
    Camure can be connected to either display in the Camure + Display chain
    OR
    Camure can be connected to NSF in the Camure + NSF + DEI + SC + Display
    chain
*/
