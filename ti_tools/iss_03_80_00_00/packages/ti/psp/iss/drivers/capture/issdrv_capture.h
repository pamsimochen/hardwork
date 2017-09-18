/** ==================================================================
 *  @file   issdrv_capture.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/drivers/capture/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/**
  \file iss_capture_driver.h

  \brief ISS Driver layer - VIP Capture API
*/

/**
  \ingroup ISS_DRV_DRV_API
  \defgroup ISS_DRV_DRV_CAPTURE ISS Driver layer - VIP Capture API

  The API from this modules are used by FVID2 layer to make call to
  specific drivers.
  User's do not need to know all details about this API.

  This is documented for reference purposes and advanced level users.

  Refer to \ref ISS_DRV_FVID2_VIP_CAPTURE for details of user level VIP
  capture API

  @{
*/

#ifndef _ISS_CAPTURE_DRIVER_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_CAPTURE_DRIVER_H_

#include <ti/psp/iss/iss_capture.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

/* functions's */

/**
 * \brief One-time capture sub-system init
 *
 * Gets called as part of FVID2_init()
 *
 * Initializes internal data structures and common system level resources
 * related to capture
 *
 * \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captInit                                               
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
Int32 Iss_captInit();

/**
 * \brief One-time capture sub-system de-init
 *
 * Free's all resources allocated during Iss_captInit
 *
 * \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captDeInit                                               
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
Int32 Iss_captDeInit();

/**
  \brief Create a capture instance

  This API gets called when FVID2_create() is called

  \param drvId        [I ] Must be FVID2_ISS_CAPT_VIP_DRV
  \param instanceId   [I ] ISS_CAPT_INST_VIP0_PORTA .. ISS_CAPT_INST_VIP1_PORTB
  \param createArgs   [I ] Instance creation arguments
  \param createStatus [O ] Instance creation status
  \param cbPrm        [I ] Capture callback related parameters
  \param fdmData      [I ] Application data that is returned
                           when cbFxn is called

  \return Capture handle on success else NULL
*/
/* ===================================================================
 *  @func     Iss_captCreate                                               
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
Fdrv_Handle Iss_captCreate(UInt32 drvId,
                           UInt32 instanceId,
                           Iss_CaptCreateParams * createArgs,
                           Iss_CaptCreateStatus * createStatus,
                           const FVID2_CbParams * cbPrm);

/**
  \brief Delete a capture instance

  This API gets called when FVID2_delete() is called

  \param handle     [I ] Capture handle
  \param reserved   [I ] NOT USED, set as NULL

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captDelete                                               
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
Int32 Iss_captDelete(Fdrv_Handle handle, Ptr reserved);

/**
  \brief Start capture

  This API gets called when FVID2_start() is called

  Make sure capture handle is created before starting it

  \param handle     [I ] Capture handle

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captStart                                               
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
Int32 Iss_captStart(Fdrv_Handle handle);

/**
  \brief Start capture

  This API gets called when FVID2_stop() is called

  \param handle     [I ] Capture handle

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captStop                                               
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
Int32 Iss_captStop(Fdrv_Handle handle);

/**
  \brief Queue frames in capture handle

  This API gets called when FVID2_queue() is called

  \param handle     [I ] Capture handle
  \param frameList  [I ] Frames to queue
  \param streamId   [I ] Stream to which the frames belong

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captQueue                                               
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
Int32 Iss_captQueue(Fdrv_Handle handle,
                    FVID2_FrameList * frameList, UInt32 streamId);

/**
  \brief De-Queue frames from capture handle

  This API gets called when FVID2_dequeue() is called

  \param handle     [I ] Capture handle
  \param frameList  [ O] De-queued frames
  \param streamId   [I ] Stream from which to dequeue the frames
  \param timeout    [I ] BIOS_WAIT_FOREVER to block the call
                         until atleast one frame is available,
                         BIOS_NO_WAIT for non-blocking call

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captDequeue                                               
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
Int32 Iss_captDequeue(Fdrv_Handle handle,
                      FVID2_FrameList * frameList, UInt32 streamId,
                      UInt32 timeout);

/**
  \brief Capture control

  This API gets called when FVID2_control() is called

  \param handle        [I ] Capture handle
  \param cmd           [I ] command ID
  \param cmdArgs       [I ] command arguments
  \param cmdStatusArgs [ O] command return status

  \return 0 on success, else failure
*/
/* ===================================================================
 *  @func     Iss_captControl                                               
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
Int32 Iss_captControl(Fdrv_Handle handle, UInt32 cmd,
                      Ptr cmdArgs, Ptr cmdStatusArgs);

/* ===================================================================
 *  @func     Iss_captPrintAdvancedStatistics                                               
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
Int32 Iss_captPrintAdvancedStatistics(UInt32 totalTimeInMsecs);

Int32 Iss_ispInit();
Int32 Iss_ispDeInit();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
