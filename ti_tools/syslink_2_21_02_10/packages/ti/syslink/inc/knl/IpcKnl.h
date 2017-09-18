/** 
 *  @file   IpcKnl.h
 *
 *  @brief   Kernel-side specific header for Ipc module.
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



#ifndef _IPCKNL_H_
#define _IPCKNL_H_


#include <ti/ipc/Ipc.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Enums & Structures
 * =============================================================================
 */
/*!
 *  The different options for processor synchronization
 */
typedef enum Ipc_ProcSync {
    Ipc_ProcSync_NONE,          /* don't do any processor sync            */
    Ipc_ProcSync_PAIR,          /* sync pair of processors in Ipc_attach  */
    Ipc_ProcSync_ALL            /* sync all processors, done in Ipc_start */
} Ipc_ProcSync;

/*!
 *   Ipc configuration structure.
 */
typedef struct Ipc_Config {
    Ipc_ProcSync procSync;
    /*!< The different options for processor synchronization */
} Ipc_Config;

/*!
 *   Ipc configuration structure.
 */
typedef struct Ipc_Params {
    Bool  setupMessageQ;
    Bool  setupNotify;
    Bool  slave;
    Ipc_ProcSync  procSync;
} Ipc_Params;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/*!
 *  @brief Returns default configuration values for Ipc.
 *
 *  @param cfgParams Pointer to configuration structure
 */
Void Ipc_getConfig (Ipc_Config * cfgParams);

/*!
 *  @brief Sets up Ipc for this processor.
 *
 *  @param cfgParams Pointer to configuration structure
 */
Int Ipc_setup (const Ipc_Config * cfgParams);

/*!
 *  @brief Destroys Ipc for this processor.
 */
Int Ipc_destroy (Void);

/*!
 *  @brief Creates a IPC.
 */
Int Ipc_create (UInt16 procId, Ipc_Params * params);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* _IPCKNL_H_ */
