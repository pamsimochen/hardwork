/** 
 *  @file   _RingIO.h
 *
 *  @brief      Defines for RingIO module.
 *
 *              Internal data structures
 *
 *
 */
/* 
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */



#ifndef _RINGIO_H_12
#define _RINGIO_H_12


/* Standard headers */
#include <ti/syslink/RingIO.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Macros and types
 * =============================================================================
 */

/*!
 *  @brief  Name of memory that contains the RingIO global structure
 */
#define RingIO_GLOBAL "RingIO.global"

/*!
 *  @brief  Name of memory that contains the RingIO configuration structure
 */
#define RingIO_CONFIG "RingIO.cfg"

/*!
 *  @brief  This constant denotes an invalid attribute type.
 *          This should not be used for any valid attribute.
 */
#define RingIO_INVALID_ATTR           (UInt16) 0xFFFF

/*!
 *  @brief  This constant an invalid offset in the attribute buffer.
 */
#define RingIO_INVALID_OFFSET                 -1l

/*!
 *  @brief   This type is used for the Notification Parameter.
 */
typedef Ptr RingIO_NotifyParam;

/*!
 *  @brief   Size of RingIO attributes
 */
#define   RingIO_NOTIFYENTRIES   2
/*!
 *  @brief  For reader and writer
 */
#define   RingIO_SUBNOTIFYENTRIES   1


/* =============================================================================
 * Configuration
 * =============================================================================
 */
/*! @brief Type for function pointer to initialize the parameters for the RingIO
 *         instance.
 */
// typedef Void (*RingIO_paramsInitFxn) (Void * params);

/*! @brief Type for function pointer to get shared memory requirements */
typedef UInt32 (*RingIO_sharedMemReqFxn) (
                            const Void *     params,
                            RingIO_sharedMemReqDetails  * sharedMemReqDetails);

/*! @brief Type for function pointer to create a RingIO object for a specific
 *         slave processor.
 */
typedef Int (*RingIO_createFxn) (const Void * params, RingIO_Handle * handlePtr);
/*! @brief Type for function pointer to create a RingIO object for a specific
 *         slave processor.
 */
typedef Int (*RingIO_openByAddrFxn) (Ptr                 ctrlSharedAddr,
                               const RingIO_openParams * params,
                                     RingIO_Handle     * handlePtr);

/*!
 *  @brief  Structure defining config parameters for the RingIO module.
 */
typedef struct RingIO_Config_Tag {
    UInt32       maxNameLen;
    /*!<  Maximum length of the name of RingIO instance */
    UInt32       maxInstances;
    /*!<  Maximum number of RingIO's that can be made */
} RingIO_Config;

/*!
 *  @brief  Structure for the Handle for the RingIO.
 */
typedef struct RingIO_CfgFxns_tag {
    RingIO_createFxn              create;
    /*!<  Create function */
    RingIO_sharedMemReqFxn        sharedMemReq;
    /*!<  SharedMemReq function */
    RingIO_openByAddrFxn          openByAddr;
    /*!<  SharedMemReq function */
} RingIO_CfgFxns;



/*!
 *  @brief  RingIO information data structure to be added to Name Server
 */
typedef struct RingIO_NameServerEntry_tag {
    UInt16                 procId;
    /*!< Proc Id on which RingIO was created */
    UInt32                 ctrlSharedSrPtr;
    /*!< Ctrl Shared address of the RingIO instance */
} RingIO_NameServerEntry ;


/*!
 *  @brief  Structure defining interface type for RingIO.
 */
typedef struct RingIO_InterfaceAttrs_tag {
    RingIO_Type            type;
    /*!< Interface type */
} RingIO_InterfaceAttrs;


/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      RingIO instance Handle.
 */

Ptr RingIO_getCliNotifyMgrGateShAddr (RingIO_Handle handle);
/*!
 * @brief Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 * @param handle      RingIO instance Handle.
 */

Ptr RingIO_getCliNotifyMgrShAddr (RingIO_Handle handle);

/*!
 *  @brief     Function to reset the notifyid received in userspace during call to
 *             ClientNotifyMgr_register client.
 *  @param     handle  Instance handle.
 *  @param     reset value to set in the object.
 *
 */
Int RingIO_resetNotifyId (RingIO_Handle handle, UInt32 notifyId);
/* Sets the notification type */
Int RingIO_setNotifyId (RingIO_Handle        handle,
                         UInt32               notifyId,
                         RingIO_NotifyType    notifyType,
                         UInt32               watermark);

#ifdef SYSLINK_BUILDOS_QNX
/*!
 *  @brief      Function to setup the RingIO module.
 *
 *              This function sets up the RingIO module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then RingIO_getConfig() can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call RingIO with NULL parameters.
 *              The default parameters would get automatically used.
 *
 *  @param      cfgParams   Optional RingIO module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_S_ALREADYSETUP @copydoc RingIO_S_ALREADYSETUP
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *  @retval     RingIO_E_FAIL         @copydoc RingIO_E_FAIL
 *
 *  @sa         RingIO_destroy()
 */
Int RingIO_setup(Void);

/*!
 *  @brief      Function to destroy the RingIO module.
 *
 *              Once this function is called, other RingIO module APIs, except
 *              for the RingIO_getConfig() API cannot be called anymore.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE @copydoc RingIO_E_INVALIDSTATE
 *
 *  @sa         RingIO_setup()
 */
Int RingIO_destroy(Void);
#else /* for linux and bios */
/*!
 *  @brief      Function to get the default configuration for the RingIO
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to RingIO_setup filled in by the
 *              RingIO module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      cfgParams Pointer to the RingIO module configuration structure
 *                        in which the default config is to be returned.
 *
 *  @sa         RingIO_setup()
 */
Void RingIO_getConfig(RingIO_Config *cfgParams);

/*!
 *  @brief      Function to setup the RingIO module.
 *
 *              This function sets up the RingIO module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then RingIO_getConfig() can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call RingIO with NULL parameters.
 *              The default parameters would get automatically used.
 *
 *  @param      cfgParams   Optional RingIO module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_S_ALREADYSETUP @copydoc RingIO_S_ALREADYSETUP
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *  @retval     RingIO_E_FAIL         @copydoc RingIO_E_FAIL
 *
 *  @sa         RingIO_destroy()
 */
Int RingIO_setup(const RingIO_Config *cfgParams);

/*!
 *  @brief      Function to destroy the RingIO module.
 *
 *              Once this function is called, other RingIO module APIs, except
 *              for the RingIO_getConfig() API cannot be called anymore.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE @copydoc RingIO_E_INVALIDSTATE
 *
 *  @sa         RingIO_setup()
 */
Int RingIO_destroy(Void);

#endif

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* _RINGIO_H_ */
