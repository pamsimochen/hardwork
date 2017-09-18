/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_DEVICE_API
 * \defgroup VPS_DRV_FVID2_DEVICE_VIDEO_FILTERS_API External Video Filters API
 *
 *  This module defines API interface for external THS video
 *  buffer/amplifier devices
 *
 * @{
*/

/**
 *  \file vps_thsfilters.h
 *
 *  \brief External Video Filters API
*/
#ifndef _VPS_VIDEO_DEVICE_THS_FILTERS_H_
#define _VPS_VIDEO_DEVICE_THS_FILTERS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Enum to enable/bypass/disable THS360 SD and THS7535 modules */
typedef enum
{
    VPS_THSFILTER_ENABLE_MODULE,
    /**< Enable the Module */
    VPS_THSFILTER_BYPASS_MODULE,
    /**< Bypass the Module */
    VPS_THSFILTER_DISABLE_MODULE
    /**< Disable the module */
} Vps_ThsFilterCtrl;

/** \brief Enum for selecting filter for component input/output in THS7360 */
typedef enum
{
    VPS_THS7360_DISABLE_SF,
    /**< Disable THS7360 SF */
    VPS_THS7360_BYPASS_SF,
    /**< Bypass THS7360 SF */
    VPS_THS7360_SF_SD_MODE,
    /**< Select 9.5 MHs Filter to SD mode in THS7360 */
    VPS_THS7360_SF_ED_MODE,
    /**< Select 18 MHs Filter to ED mode in THS7360 */
    VPS_THS7360_SF_HD_MODE,
    /**< Select 36 MHs Filter to HD mode in THS7360 */
    VPS_THS7360_SF_TRUE_HD_MODE
    /**< Select 72 MHs Filter to True HD mode in THS7360 */
} Vps_Ths7360SfCtrl;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                         Functions                                          */
/* ========================================================================== */

/**
  \brief Function to set the THS7353 parameters

  \param ths7375Ctrl  [IN] THS7353 params

  \return FVID2_SOK on success else failure
*/
Int32 Vps_ths7375SetParams(Vps_ThsFilterCtrl ths7375Ctrl);

/**
  \brief Function to set the THS7360 SD parameters

  \param ths7360SdCtrl  [IN] THS7360 Sd params. Used only for Composite
                             and SVIdeo Inputs

  \return FVID2_SOK on success else failure
*/
Int32 Vps_ths7360SetSdParams(Vps_ThsFilterCtrl ths7360SdCtrl);

/**
  \brief Function to set the THS7360 SF parameters

  \param ths7360SfCtrl  [IN] THS7360 Sf params. Used only for Componebt Inputs

  \return FVID2_SOK on success else failure
*/
Int32 Vps_ths7360SetSfParams(Vps_Ths7360SfCtrl ths7360SfCtrl);


#endif /*  _VPS_VIDEO_DEVICE_THS_FILTERS_H_  */

/*@}*/
