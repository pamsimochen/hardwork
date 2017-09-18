/** 
 *  @file   _GateMP.h
 *
 *  @brief      GateMP wrapper defines.
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



#ifndef _GATEMP_H_0xAF70
#define _GATEMP_H_0xAF70


#include <ti/syslink/utils/IGateProvider.h>
#include <ti/ipc/GateMP.h>
#include <ti/ipc/SharedRegion.h>


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    GateMP_MODULEID
 *  @brief  Unique module ID.
 */
#define GateMP_MODULEID      (0xAF70)

/*!
 *  @def    GateMP_E_LOCALGATE
 *  @brief  Gate is local gate not remote  */
#define GateMP_E_LOCALGATE             -11


/*!
 *  @brief  Structure defining config parameters for the GateMP module.
 */
typedef struct GateMP_Config_tag {
    UInt32                  numResources;
    /*!< Maximum number of resources */
    GateMP_LocalProtect     defaultProtection;
    UInt32                  maxNameLen;
    UInt32                  maxRunTimeEntries;
} GateMP_Config;


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  Get the default configuration for the GateMP module.
 */
Void
GateMP_getConfig (GateMP_Config * cfgParams);

/*!
 * Setup the GateMP module.
 */
Int32
GateMP_setup (const GateMP_Config * cfg);

/*!
 * Function to destroy the GateMP module.
 */
Int32
GateMP_destroy (Void);

/*! Function to attach GateMP to a remote processor */
Int
GateMP_attach (UInt16 remoteProcId, Ptr sharedAddr);

/*! Function to detach GateMP from a remote processor */
Int
GateMP_detach (UInt16 remoteProcId, Ptr sharedAddr);

/*! Function to start gate mp */
Int
GateMP_start (Ptr sharedAddr);

/*! Function to start gate mp */
Int
GateMP_stop (Void);

/*!
 *  Function to create local GateMP
 */
IGateProvider_Handle GateMP_createLocal (GateMP_LocalProtect localProtect);

/*!
 *  Function to return size required in shared region 0
 */
SizeT GateMP_getRegion0ReservedSize (Void);

/*! Function to shared address of a GateMP object */
SharedRegion_SRPtr GateMP_getSharedAddr (GateMP_Handle obj);

/*
 *  Returns the gatepeterson kernel object pointer.
 */
Void *
GateMP_getKnlHandle (Void * handle) ;

/*!
 *  @brief      Function to set reserved locks that can be  set aside of syslink use.
 *              This API is currently useful on netra to reserve some hardware
 *              spinlocks for linux driver use.
 *
 *  @param[in]  remoteSystemType   remote protection type.
 *                      Pass GateMP_remoteProtect_SYSTEM for GateHWSpinlocks
 *  @param[in ]  lockNum   lock number.
 *
 *
 *  @return    None
 */
Void GateMP_setReserved(UInt32 remoteSystemType, UInt32 lockNum);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* _GATEMP_H_0xAF70 */
