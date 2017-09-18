/** 
 *  @file   ClientNotifyMgr_config.h
 *
 *  @brief      Defines for configurable macros for ClientNotifyMgr module.
 *
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



#ifndef CLIENTNOTIFYMGR_CONFIG_H
#define CLIENTNOTIFYMGR_CONFIG_H


#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * macros & defines
 * =============================================================================
 */

/*!
 *  @brief  Maximum length of the name string in bytes.
 */
#define ClientNotifyMgr_maxNameLen                 (32u)

/*!
 *  @brief  Cache line size.
 */
#define ClientNotifyMgr_CACHE_LINESIZE             (128u)

/*!
 *  @brief  Maximum number of instances managed by ClientNotifyMgr module.
 */
#define ClientNotifyMgr_maxInstances               (256u)

/*!
 *  @brief  Maximum number of clients supported for each instance.
 */
#define ClientNotifyMgr_MAX_CLIENTS                (32u)

/*!
 *  @brief  Maximum number of sub notification entries for this instance.
 *          Each notfication entry of type NotifyParamInfo will contain this
 *          number of sub notification entries for the same call back function.
 *          This enables an instance of this module to use same call back fxn
 *          and watermark conditions for event notifications to notify the
 *          other clients of the instance.
 */
#define CLIENTNOTIFYMGR_MAX_SUB_NOTIFYENTRIES       (64u)

#if defined (__cplusplus)
}
#endif

#endif
