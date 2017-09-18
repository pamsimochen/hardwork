/** 
 *  @file   _RingIODefs.h
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



#ifndef _RINGIO_H_0x8fc0
#define _RINGIO_H_0x8fc0


/* Standard headers */

/* Utilities headers */
#include <ti/syslink/utils/List.h>


#if defined (__cplusplus)
extern "C" {
#endif



/* =============================================================================
 * Setup related API's
 * =============================================================================
 */
/* =============================================================================
 *  Structure for setup related configuration
 * =============================================================================
 */
/*!
 *  @brief  Structure defining config parameters for the RingIO module.
 */
typedef struct RingIO_Config_Tag {
    UInt32       notifyEventNo;
    /*!<  Notify event number to be used by the Client Notify Manager */
    UInt32       maxNameLen;
    /*!<  Maximum length of the name of RingIO instance */
    UInt32       maxRuntimeEntries;
    /*!<  Maximum number of RingIO's that can be made */
} RingIO_Config;

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
 *  @sa         RingIO_setup
 */
Void  RingIO_getConfig (RingIO_Config * cfgParams);

/*!
 *  @brief      Function to setup the RingIO module.
 *
 *              This function sets up the RingIO module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then RingIO_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call RingIO with NULL parameters.
 *              The default parameters would get automatically used.
 *
 *  @param      cfgParams   Optional RingIO module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @retval
 *
 *  @sa         RingIO_destroy
 *              NameServer_create
 *              GateMutex_create
 *              Memory_alloc
 */
Int   RingIO_setup (const RingIO_Config * cfgParams);

/*!
 *  @brief      Function to destroy the RingIO module.
 *
 *              Once this function is called, other RingIO module APIs, except
 *              for the RingIO_getConfig API cannot be called anymore.
 *
 *  @retval
 *  @sa         RingIO_setup, NameServer_delete, GateMutex_delete
 */
Int   RingIO_destroy (Void);



#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* _RINGIO_H_0x8fc0 */
