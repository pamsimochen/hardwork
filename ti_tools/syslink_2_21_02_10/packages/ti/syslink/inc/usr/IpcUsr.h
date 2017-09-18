/** 
 *  @file   IpcUsr.h
 *
 *  @brief   This module is primarily used to configure IPC-wide settings and
 *           initialize IPC at runtime.
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



#ifndef _IPCUSR_H__
#define _IPCUSR_H__


#include <ti/ipc/Ipc.h>

/* Module headers */
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/syslink/inc/_MessageQ.h>
#include <ti/syslink/inc/_Notify.h>
#include <ti/syslink/inc/_NameServer.h>
#include <ti/syslink/inc/_ProcMgr.h>
#include <ti/syslink/inc/_HeapBufMP.h>
#include <ti/syslink/inc/_HeapMemMP.h>
#include <ti/syslink/inc/_GateMP.h>

#include <ti/syslink/SysLink.h>
#ifndef SYSLINK_BUILDOS_QNX
#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/_RingIOShm.h>
#endif
#include <ti/syslink/inc/ClientNotifyMgr.h>

#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/inc/_FrameQ.h>

#ifdef SYSLINK_BUILDOS_QNX
#include <ti/syslink/utils/IpcMemMgr.h>
#endif

#if 0 /* TBD: Temporarily commented. */
#include <ti/syslink/inc/_SysMemMgr.h>
#endif


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Enums & Structures
 * =============================================================================
 */
/*!
 *  @brief  Structure defining config parameters for overall System.
 */
typedef struct Ipc_Config {
    SysLink_MemoryMap           *   sysMemMap;
    /*!< System memory map */

    String                          params;
    /*!< instance params override */

#ifdef SYSLINK_BUILDOS_QNX
    IpcMemMgr_Config                ipcMemMgrConfig;
    /*!< IpcMemMgr config parameter */
#endif

    MultiProc_Config                multiProcConfig;
    /*!< Multiproc config parameter */

    SharedRegion_Config             sharedRegionConfig;
    /*!< SharedRegion config parameter */

    GateMP_Config                   gateMPConfig;
    /*!< GateMP config parameter */

    MessageQ_Config                 messageQConfig;
    /*!< MessageQ config parameter */

    Notify_Config                   notifyConfig;
    /*!< Notify config parameter */

    ProcMgr_Config                  procMgrConfig;
    /*!< Processor manager config parameter */

    HeapBufMP_Config                heapBufMPConfig;
    /*!< HeapBufMP config parameter */

    HeapMemMP_Config                heapMemMPConfig;
    /*!< HeapMemMP config parameter */

    ListMP_Config                   listMPConfig;
    /*!< ListMP config parameter */
    ClientNotifyMgr_Config          cliNotifyMgrCfgParams;
    /*!< ClientNotifyMgr config parameter */

#ifndef SYSLINK_BUILDOS_QNX
    RingIO_Config                   ringIOConfig;
    /*!< RingIO config parameter */

    RingIOShm_Config                ringIOShmConfig;
    /*!< RingIO config parameter */
#endif

    FrameQBufMgr_Config             frameQBufMgrCfgParams;
    /*!< FrameQBufMgr config parameter */

    FrameQ_Config                   frameQCfgParams;
    /*!< FrameQ config parameter */

#if 0 /* TBD: Temporarily commented. */
    SysMemMgr_Config                sysMemMgrConfig;
    /*!< System memory manager config parameter */
#endif
} Ipc_Config;


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
Int Ipc_destroy (void);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* _IPCUSR_H__ */
