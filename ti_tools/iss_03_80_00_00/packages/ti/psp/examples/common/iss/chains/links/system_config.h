/** ==================================================================
 *  @file   system_config.h                                                  
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

/**
 *  \ingroup VPSEXAMPLE_LINKS_AND_CHAIN_API
 *  \defgroup VPSEXAMPLE_SYSTEM_LINK_API System API
 *
 *  The API defined in this module is used to configure each link depending
 *  on the layout and use case selected by the chain.
 *
 *  @{
 */

/**
 *  \file system_config.h
 *
 *  \brief System level configuration API
 */

#ifndef _SYSTEM_CONFIG_H
#define _SYSTEM_CONFIG_H

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

    /* ========================================================================== 
     */
    /* Macros & Typedefs */
    /* ========================================================================== 
     */

/** \brief Maximum number of output queues */
#define SYSTEM_MAX_OUT_QUE              (4u)
/** \brief Maximum number of channels per output queue */
#define SYSTEM_MAX_CH_PER_OUT_QUE       (16u)

/**
 *  enum Sys_DeiCfgId
 *  \brief Enumerations for system config ID used to select a particular use
 *  case or layout for DEI link.
 */
    typedef enum {
        SYS_DEI_CFG_DEFAULT = 0,
        SYS_DEI_CFG_1CH,
        SYS_DEI_CFG_2CH,
        SYS_DEI_CFG_4CH,
        SYS_DEI_CFG_4CH_DUALCHAIN,
        SYS_DEI_CFG_6CH,
        SYS_DEI_CFG_6CH_DUALCHAIN,
        SYS_DEI_CFG_8CH,
        SYS_DEI_CFG_8CH_DUALCHAIN,
        SYS_DEI_CFG_8CH_12CHMODE
    } Sys_DeiCfgId;

/**
 *  enum Sys_ScCfgId
 *  \brief Enumerations for system config ID used to select a particular use
 *  case or layout for SC link.
 */
    typedef enum {
        SYS_SC_CFG_DEFAULT = 0,
        SYS_SC_CFG_8CH_12CHMODE
    } Sys_ScCfgId;

/**
 *  enum Sys_DispCfgId
 *  \brief Enumerations for system config ID used to select a particular use
 *  case or layout for display mosaic link.
 */
    typedef enum {
        SYS_DISP_CFG_DEFAULT = 0,
        SYS_DISP_CFG_1CH,
        SYS_DISP_CFG_2CH,
        SYS_DISP_CFG_4CH,
        SYS_DISP_CFG_6CH,
        SYS_DISP_CFG_8CH,
        SYS_DISP_CFG_12CH,
        SYS_DISP_CFG_16CH
    } Sys_DispCfgId;

    /* ========================================================================== 
     */
    /* Structure Declarations */
    /* ========================================================================== 
     */

/**
    \brief DEI link configuration
*/
    typedef struct {
        Sys_DeiCfgId deiCfgId;
    /**< DEI configuration ID */
        UInt32 numCh;
    /**< Number of channels */
        UInt32 deiOutWidth[2u][SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< DEI output width for all channels */
        UInt32 deiOutHeight[2u][SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< DEI output height for all channels */
    } Sys_DeiLinkCfg;

/**
    \brief Scalar link configuration
*/
    typedef struct {
        Sys_ScCfgId scCfgId;
    /**< Scalar configuration ID */
        UInt32 numCh;
    /**< Number of channels */
        UInt32 scOutWidth[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Scalar output width for all channels */
        UInt32 scOutHeight[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Scalar output height for all channels */
    } Sys_ScLinkCfg;

/**
    \brief Display link configuration
*/
    typedef struct {
        Sys_DispCfgId dispCfgId;
    /**< Display configuration ID */
        UInt32 numCh;
    /**< Number of channels */
        UInt32 dispWidth;
    /**< Display width */
        UInt32 dispHeight;
    /**< Display height */
        UInt32 winStartX[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Start X coordinate of the display window for all channels */
        UInt32 winStartY[SYSTEM_MAX_CH_PER_OUT_QUE];
    /**< Start Y coordinate of the display window for all channels */
    } Sys_DispLinkCfg;

    /* ========================================================================== 
     */
    /* Function Declarations */
    /* ========================================================================== 
     */

/**
    \brief Get the DEI link configuration

    This function returns the DEI link configuration for the specified ID

    \param deiCfgId       [IN] DEI configuration ID

    \return Pointer to DEI link configuration for valid ID else NULL
*/
    const Sys_DeiLinkCfg *Sys_getDeiLinkCfg(Sys_DeiCfgId deiCfgId);

/**
    \brief Get the Scalar link configuration

    This function returns the Scalar link configuration for the specified ID

    \param scCfgId       [IN] Scalar configuration ID

    \return Pointer to Scalar link configuration for valid ID else NULL
*/
    const Sys_ScLinkCfg *Sys_getScLinkCfg(Sys_ScCfgId scCfgId);

/**
    \brief Get the Display link configuration

    This function returns the Display link configuration for the specified ID

    \param dispCfgId       [IN] Display configuration ID

    \return Pointer to Display link configuration for valid ID else NULL
*/
    const Sys_DispLinkCfg *Sys_getDispLinkCfg(Sys_DispCfgId dispCfgId);

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * _SYSTEM_CONFIG_H 
   *//* @} */
