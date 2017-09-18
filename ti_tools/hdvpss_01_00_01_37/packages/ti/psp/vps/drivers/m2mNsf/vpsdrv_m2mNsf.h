/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
  \file vps_nsf_driver.h

  \brief VPS Driver layer - Nsf API
*/

/**
  \ingroup VPS_DRV_DRV_API
  \defgroup VPS_DRV_DRV_NSF VPS Driver layer - Noise Filter API

  The API from this modules are used by FVID2 layer to make
  call to specific drivers.
  User's do not need to know all details about this API.

  This is documented for reference purposes and advanced level users.

  Refer to \ref VPS_DRV_FVID2_NSF_API for details of user level nsf API

  @{
*/

#ifndef _VPS_NSF_DRIVER_H_
#define _VPS_NSF_DRIVER_H_

#include <ti/psp/vps/vps_m2mNsf.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

/* constant's */

/* functions's  */

/**
 * \brief One-time NSF sub-system init
 *
 * Gets called as part of FVID2_init()
 *
 * Initializes internal data structures and common system level resources
 * related to nsf
 *
 * \return 0 on success, else failure
*/
Int32 Vps_nsfInit (  );

/**
 * \brief One-time NSF sub-system de-init
 *
 * Free's all resources allocated during Vps_Nsfinit
 *
 * \return 0 on success, else failure
*/
Int32 Vps_nsfDeInit (  );

/**
  \brief Create a nsf instance

  This API gets called when FVID2_create() is called

  \param drvId        [I ] Must be FVID2_VPS_M2M_NSF_DRV
  \param instanceId   [I ] NSF instance ID
  \param createArgs   [I ] Instance creation arguments
  \param createStatus [ O] Instance creation status
  \param cbParams     [I ] Nsf callback related parameters
  \param fdmData      [I ] Application data that is returned
                           when cbFxn is called

  \return Nsf handle on success else NULL
*/
Fdrv_Handle Vps_nsfCreate ( UInt32 drvId,
                            UInt32 instanceId,
                            Vps_NsfCreateParams * createArgs,
                            Vps_NsfCreateStatus * createStatus,
                            const FVID2_CbParams * cbParams );

/**
  \brief Delete a nsf instance

  This API gets called when FVID2_delete() is called

  \param handle     [I ] Nsf handle
  \param reserved   [I ] NOT USED, set as NULL

  \return 0 on success, else failure
*/
Int32 Vps_nsfDelete ( Fdrv_Handle handle, Ptr reserved );

/**
  \brief Submit NSF frame's for processing

  \param handle       [I ] Nsf handle
  \param processList  [I ] NSF input frame list,
                           previous output frame list
                           and current output frame list

  \return 0 on success, else failure
*/
Int32 Vps_nsfProcessFrames ( Fdrv_Handle handle,
                             FVID2_ProcessList * processList );

/**
  \brief Delete a nsf instance

  This API gets called when FVID2_delete() is called

  \param handle       [I ] Nsf handle
  \param processList  [ O] frame's that were processed via Vps_Nsfprocess
  \param timeout      [I ] BIOS_NO_WAIT: non-blocking wait,

  \return 0 on success, else failure
*/
Int32 Vps_nsfGetProcessedFrames ( Fdrv_Handle handle,
                                  FVID2_ProcessList * processList,
                                  UInt32 timeout );

/**
  \brief Nsf control

  This API gets called when FVID2_control() is called

  \param handle        [I ] Nsf handle
  \param cmd           [I ] command ID
  \param cmdArgs       [I ] command arguments
  \param cmdStatusArgs [ O] command return status

  \return 0 on success, else failure
*/
Int32 Vps_nsfControl ( Fdrv_Handle handle, UInt32 cmd,
                       Ptr cmdArgs, Ptr cmdStatusArgs );

/**
  \brief Set information like width, height for a particular channel

  \param handle        [I ] Nsf handle
  \param format        [I ] Nsf channel format

  \return 0 on success, else failure
*/
Int32 Vps_nsfSetDataFormat ( Fdrv_Handle handle, Vps_NsfDataFormat * format );

/**
  \brief Set NSF processing parameters like bypass mode,
         subframe mode for a particular channel

  \param handle        [I ] Nsf handle
  \param format        [I ] Nsf channel format

  \return 0 on success, else failure
*/
Int32 Vps_nsfSetProcessingCfg ( Fdrv_Handle handle,
                                Vps_NsfProcessingCfg * params );

/**
  \brief Get Advanced configuration parameters for a specific channel

  \param handle        [I ] Nsf handle
  \param params        [O ] Nsf channel ADV CFG Params

  \return 0 on success, else failure
*/
Int32 Vps_nsfGetAdvCfg ( Fdrv_Handle handle,
                                Vps_NsfAdvConfig * params );

#endif

/* @}*/
