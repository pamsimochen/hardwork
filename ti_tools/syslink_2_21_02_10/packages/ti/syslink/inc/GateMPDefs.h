/** 
 *  @file   GateMPDefs.h
 *
 *  @brief      Definitions of GateMPDrv types and structures.
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



#ifndef GATEMP_DRVDEFS_H_0xF416
#define GATEMP_DRVDEFS_H_0xF416


/* Utilities headers */
#include "GatePeterson.h"
#include "_GateMPSupportNull.h"
#include "GateMPSupportNull.h"
#include "GateHWSpinlock.h"
#include "GateHWSem.h"
#include "GateAAMonitor.h"


#if defined (__cplusplus)
extern "C" {
#endif

#if defined(SYSLINK_PLATFORM_TI81XX)
#define GateMP_RemoteSystemProxy_Params_init   GateHWSpinlock_Params_init
#define GateMP_RemoteCustom1Proxy_Params_init  GatePeterson_Params_init
#define GateMP_RemoteCustom2Proxy_Params_init  GateMPSupportNull_Params_init
#define GateMP_RemoteSystemProxy_create        GateHWSpinlock_create
#define GateMP_RemoteCustom1Proxy_create       GatePeterson_create
#define GateMP_RemoteCustom2Proxy_create       GateMPSupportNull_create
#define GateMP_RemoteSystemProxy_delete        GateHWSpinlock_delete
#define GateMP_RemoteCustom1Proxy_delete       GatePeterson_delete
#define GateMP_RemoteCustom2Proxy_delete       GateMPSupportNull_delete
#define GateMP_RemoteSystemProxy_Params        GateHWSpinlock_Params
#define GateMP_RemoteCustom1Proxy_Params       GatePeterson_Params
#define GateMP_RemoteCustom2Proxy_Params       GateMPSupportNull_Params
#define GateMP_RemoteSystemProxy_sharedMemReq  GateHWSpinlock_sharedMemReq
#define GateMP_RemoteCustom1Proxy_sharedMemReq GatePeterson_sharedMemReq
#define GateMP_RemoteCustom2Proxy_sharedMemReq GateMPSupportNull_sharedMemReq

#define GateMP_RemoteSystemProxy_getNumInstances   GateHWSpinlock_getNumInstances
#define GateMP_RemoteCustom1Proxy_getNumInstances  GatePeterson_getNumInstances
#define GateMP_RemoteCustom2Proxy_getNumInstances  GateMPSupportNull_getNumInstances

#define GateMP_RemoteSystemProxy_locksinit         GateHWSpinlock_locksinit
#define GateMP_RemoteCustom1Proxy_locksinit        GatePeterson_locksinit
#define GateMP_RemoteCustom2Proxy_locksinit        GateMPSupportNull_locksinit

#define GateMP_RemoteSystemProxy_Handle        GateHWSpinlock_Handle
#define GateMP_RemoteCustom1Proxy_Handle       GatePeterson_Handle
#define GateMP_RemoteCustom2Proxy_Handle       GateMPSupportNull_Handle
#define GateMP_RemoteSystemProxy_enter         GateHWSpinlock_enter
#define GateMP_RemoteSystemProxy_leave         GateHWSpinlock_leave
#define GateMP_RemoteSystemProxy_getKnlHandle  GateHWSpinlock_getKnlHandle
#define GateMP_RemoteCustom1Proxy_enter        GatePeterson_enter
#define GateMP_RemoteCustom1Proxy_leave        GatePeterson_leave
#define GateMP_RemoteCustom1Proxy_getKnlHandle GatePeterson_getKnlHandle
#define GateMP_RemoteCustom2Proxy_enter        GateMPSupportNull_enter
#define GateMP_RemoteCustom2Proxy_leave        GateMPSupportNull_leave
#define GateMP_RemoteCustom2Proxy_getKnlHandle GateMPSupportNull_getKnlHandle

#define GateMP_RemoteSystemProxy_getReservedMask   GateHWSpinlock_getReservedMask
#define GateMP_RemoteCustom1Proxy_getReservedMask  GatePeterson_getReservedMask
#define GateMP_RemoteCustom2Proxy_getReservedMask  GateMPSupportNull_getReservedMask

#define GateMP_RemoteSystemProxy_setReserved       GateHWSpinlock_setReserved
#define GateMP_RemoteCustom1Proxy_setReserved      GatePeterson_setReserved
#define GateMP_RemoteCustom2Proxy_setReserved      GateMPSupportNull_setReserved

#elif defined(SYSLINK_PLATFORM_OMAP3530)

#define GateMP_RemoteSystemProxy_Params_init   GatePeterson_Params_init
#define GateMP_RemoteCustom1Proxy_Params_init  GatePeterson_Params_init
#define GateMP_RemoteCustom2Proxy_Params_init  GateMPSupportNull_Params_init
#define GateMP_RemoteSystemProxy_create        GatePeterson_create
#define GateMP_RemoteCustom1Proxy_create       GatePeterson_create
#define GateMP_RemoteCustom2Proxy_create       GateMPSupportNull_create
#define GateMP_RemoteSystemProxy_delete        GatePeterson_delete
#define GateMP_RemoteCustom1Proxy_delete       GatePeterson_delete
#define GateMP_RemoteCustom2Proxy_delete       GateMPSupportNull_delete
#define GateMP_RemoteSystemProxy_Params        GatePeterson_Params
#define GateMP_RemoteCustom1Proxy_Params       GatePeterson_Params
#define GateMP_RemoteCustom2Proxy_Params       GateMPSupportNull_Params
#define GateMP_RemoteSystemProxy_sharedMemReq  GatePeterson_sharedMemReq
#define GateMP_RemoteCustom1Proxy_sharedMemReq GatePeterson_sharedMemReq
#define GateMP_RemoteCustom2Proxy_sharedMemReq GateMPSupportNull_sharedMemReq

#define GateMP_RemoteSystemProxy_getNumInstances   GatePeterson_getNumInstances
#define GateMP_RemoteCustom1Proxy_getNumInstances  GatePeterson_getNumInstances
#define GateMP_RemoteCustom2Proxy_getNumInstances  GateMPSupportNull_getNumInstances

#define GateMP_RemoteSystemProxy_locksinit         GatePeterson_locksinit
#define GateMP_RemoteCustom1Proxy_locksinit        GatePeterson_locksinit
#define GateMP_RemoteCustom2Proxy_locksinit        GateMPSupportNull_locksinit


#define GateMP_RemoteSystemProxy_Handle        GatePeterson_Handle
#define GateMP_RemoteCustom1Proxy_Handle       GatePeterson_Handle
#define GateMP_RemoteCustom2Proxy_Handle       GateMPSupportNull_Handle
#define GateMP_RemoteSystemProxy_enter         GatePeterson_enter
#define GateMP_RemoteSystemProxy_leave         GatePeterson_leave
#define GateMP_RemoteSystemProxy_getKnlHandle  GatePeterson_getKnlHandle
#define GateMP_RemoteCustom1Proxy_enter        GatePeterson_enter
#define GateMP_RemoteCustom1Proxy_leave        GatePeterson_leave
#define GateMP_RemoteCustom1Proxy_getKnlHandle GatePeterson_getKnlHandle
#define GateMP_RemoteCustom2Proxy_enter        GateMPSupportNull_enter
#define GateMP_RemoteCustom2Proxy_leave        GateMPSupportNull_leave
#define GateMP_RemoteCustom2Proxy_getKnlHandle GateMPSupportNull_getKnlHandle
#define GateMP_RemoteSystemProxy_getReservedMask   GatePeterson_getReservedMask
#define GateMP_RemoteCustom1Proxy_getReservedMask  GatePeterson_getReservedMask
#define GateMP_RemoteCustom2Proxy_getReservedMask  GateMPSupportNull_getReservedMask

#define GateMP_RemoteSystemProxy_setReserved       GatePeterson_setReserved
#define GateMP_RemoteCustom1Proxy_setReserved      GatePeterson_setReserved
#define GateMP_RemoteCustom2Proxy_setReserved      GateMPSupportNull_setReserved

#elif defined(SYSLINK_PLATFORM_OMAPL1XX)

#define GateMP_RemoteSystemProxy_Params_init   GatePeterson_Params_init
#define GateMP_RemoteCustom1Proxy_Params_init  GatePeterson_Params_init
#define GateMP_RemoteCustom2Proxy_Params_init  GateMPSupportNull_Params_init
#define GateMP_RemoteSystemProxy_create        GatePeterson_create
#define GateMP_RemoteCustom1Proxy_create       GatePeterson_create
#define GateMP_RemoteCustom2Proxy_create       GateMPSupportNull_create
#define GateMP_RemoteSystemProxy_delete        GatePeterson_delete
#define GateMP_RemoteCustom1Proxy_delete       GatePeterson_delete
#define GateMP_RemoteCustom2Proxy_delete       GateMPSupportNull_delete
#define GateMP_RemoteSystemProxy_Params        GatePeterson_Params
#define GateMP_RemoteCustom1Proxy_Params       GatePeterson_Params
#define GateMP_RemoteCustom2Proxy_Params       GateMPSupportNull_Params
#define GateMP_RemoteSystemProxy_sharedMemReq  GatePeterson_sharedMemReq
#define GateMP_RemoteCustom1Proxy_sharedMemReq GatePeterson_sharedMemReq
#define GateMP_RemoteCustom2Proxy_sharedMemReq GateMPSupportNull_sharedMemReq

#define GateMP_RemoteSystemProxy_getNumInstances   GatePeterson_getNumInstances
#define GateMP_RemoteCustom1Proxy_getNumInstances  GatePeterson_getNumInstances
#define GateMP_RemoteCustom2Proxy_getNumInstances  GateMPSupportNull_getNumInstances

#define GateMP_RemoteSystemProxy_locksinit         GatePeterson_locksinit
#define GateMP_RemoteCustom1Proxy_locksinit        GatePeterson_locksinit
#define GateMP_RemoteCustom2Proxy_locksinit        GateMPSupportNull_locksinit


#define GateMP_RemoteSystemProxy_Handle        GatePeterson_Handle
#define GateMP_RemoteCustom1Proxy_Handle       GatePeterson_Handle
#define GateMP_RemoteCustom2Proxy_Handle       GateMPSupportNull_Handle
#define GateMP_RemoteSystemProxy_enter         GatePeterson_enter
#define GateMP_RemoteSystemProxy_leave         GatePeterson_leave
#define GateMP_RemoteSystemProxy_getKnlHandle  GatePeterson_getKnlHandle
#define GateMP_RemoteCustom1Proxy_enter        GatePeterson_enter
#define GateMP_RemoteCustom1Proxy_leave        GatePeterson_leave
#define GateMP_RemoteCustom1Proxy_getKnlHandle GatePeterson_getKnlHandle
#define GateMP_RemoteCustom2Proxy_enter        GateMPSupportNull_enter
#define GateMP_RemoteCustom2Proxy_leave        GateMPSupportNull_leave
#define GateMP_RemoteCustom2Proxy_getKnlHandle GateMPSupportNull_getKnlHandle
#define GateMP_RemoteSystemProxy_getReservedMask   GatePeterson_getReservedMask
#define GateMP_RemoteCustom1Proxy_getReservedMask  GatePeterson_getReservedMask
#define GateMP_RemoteCustom2Proxy_getReservedMask  GateMPSupportNull_getReservedMask

#define GateMP_RemoteSystemProxy_setReserved       GatePeterson_setReserved
#define GateMP_RemoteCustom1Proxy_setReserved      GatePeterson_setReserved
#define GateMP_RemoteCustom2Proxy_setReserved      GateMPSupportNull_setReserved
#else
#error Unsupported platform
#endif

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEMP_DRVDEFS_H_0xF416 */
