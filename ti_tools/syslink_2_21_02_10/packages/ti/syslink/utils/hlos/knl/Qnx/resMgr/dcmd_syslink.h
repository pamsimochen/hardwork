/** 
 *  @file   dcmd_syslink.h
 *
 *  @brief      DCMD definitions for all ipc, procmgr & utils
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


#include <devctl.h>

#define _DCMD_SYSLINK   _DCMD_MISC

typedef enum {
    _DCMD_SYSLINK_MULTIPROC         =0x1A,
    _DCMD_SYSLINK_NAMESERVER,
    _DCMD_SYSLINK_SHAREDREGION,
    _DCMD_SYSLINK_NOTIFY,
    _DCMD_SYSLINK_HEAPBUFMP,
    _DCMD_SYSLINK_LISTMP,
    _DCMD_SYSLINK_MESSAGEQ,
    _DCMD_SYSLINK_IPC,
    _DCMD_SYSLINK_HEAPMEMMP,
    _DCMD_SYSLINK_GATEMP,
    _DCMD_SYSLINK_OSALMEM,
    _DCMD_SYSLINK_PROCMGR,
    _DCMD_SYSLINK_RINGIO,
    _DCMD_SYSLINK_CLIENTNOTIFYMGR,
    _DCMD_SYSLINK_RINGIOSHM,
    _DCMD_SYSLINK_FRAMEQ,
    _DCMD_SYSLINK_FRAMEQBUFMGR,
    _DCMD_SYSLINK_TRACE,
    _DCMD_SYSLINK_IPCMEMMGR
} dcmd_class_t_val;

/*  ----------------------------------------------------------------------------
 *  DEVCTL command IDs for MultiProc
 *  ----------------------------------------------------------------------------
 */

typedef enum {
    DCMD_OSALMEM_MMAP_VAL = 0x1,
    DCMD_OSALMEM_MUNMAP_VAL,
} dcmd_osal_class_val_t;

/*!
 *  @brief  Command for MultiProc_setup
 */
#define DCMD_MULTIPROC_SETUP \
        __DIOTF(_DCMD_SYSLINK_MULTIPROC, 1, MultiProcDrv_CmdArgs)

/*!
 *  @brief  Command for MultiProc_destroy
 */
#define DCMD_MULTIPROC_DESTROY \
        __DIOTF(_DCMD_SYSLINK_MULTIPROC, 2, MultiProcDrv_CmdArgs)

/*!
 *  @brief  Command for MultiProc_getConfig
 */
#define DCMD_MULTIPROC_GETCONFIG \
        __DIOTF(_DCMD_SYSLINK_MULTIPROC, 3, MultiProcDrv_CmdArgs)

/*!
 *  @brief  Command for MultiProc_setLocalId
 */
#define DCMD_MULTIPROC_SETLOCALID \
        __DIOTF(_DCMD_SYSLINK_MULTIPROC, 4, MultiProcDrv_CmdArgs)


/*  ----------------------------------------------------------------------------
 *  DEVCTL command IDs for Nameserver
 *  ----------------------------------------------------------------------------
 */

/*!
 *  @brief  Command for NameServer_setup
 */
#define DCMD_NAMESERVER_SETUP                __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            2,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_setup
 */
#define DCMD_NAMESERVER_DESTROY                __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            3,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_destroy
 */
#define DCMD_NAMESERVER_PARAMS_INIT          __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            4,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_create
 */
#define DCMD_NAMESERVER_CREATE                __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            5,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_delete
 */
#define DCMD_NAMESERVER_DELETE                __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            6,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_add
 */
#define DCMD_NAMESERVER_ADD                   __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            9,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_addUInt32
 */
#define DCMD_NAMESERVER_ADDUINT32            __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            10,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_get
 */
#define DCMD_NAMESERVER_GET                  __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            11,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_getLocal
 */
#define DCMD_NAMESERVER_GETLOCAL             __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            12,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_match
 */
#define DCMD_NAMESERVER_MATCH                __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            13,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_remove
 */
#define DCMD_NAMESERVER_REMOVE                __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            14,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_removeEntry
 */
#define DCMD_NAMESERVER_REMOVEENTRY          __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            15,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_getHandle
 */
#define DCMD_NAMESERVER_GETHANDLE            __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            16,\
                                            NameServerDrv_CmdArgs)
/*!
 *  @brief  Command for NameServer_isRegistered
 */
#define DCMD_NAMESERVER_ISREGISTERED         __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            17,\
                                            NameServerDrv_CmdArgs)

/*!
 *  @brief  Command for NameServer_getConfig
 */
#define DCMD_NAMESERVER_GETCONFIG            __DIOTF(_DCMD_SYSLINK_NAMESERVER,\
                                            18,\
                                            NameServerDrv_CmdArgs)

/*!
 *  @brief  Command for NameServer_open
 */
#define DCMD_NAMESERVER_OPEN \
    __DIOTF(_DCMD_SYSLINK_NAMESERVER, 19, NameServerDrv_CmdArgs)

/*!
 *  @brief  Command for NameServer_close
 */
#define DCMD_NAMESERVER_CLOSE \
    __DIOT(_DCMD_SYSLINK_NAMESERVER, 20, NameServerDrv_CmdArgs)


/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for SharedRegion
*   ----------------------------------------------------------------------------
*/


/*!
*   @brief  Command for SharedRegion_getConfig
*/
#define DCMD_SHAREDREGION_GETCONFIG                 __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    1,\
                                                    SharedRegionDrv_CmdArgs)
/*!
 *  @brief  Command for SharedRegion_setup
 */
#define DCMD_SHAREDREGION_SETUP                    __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    2,\
                                                    SharedRegionDrv_CmdArgs)
/*!
 *  @brief  Command for SharedRegion_destroy
 */
#define DCMD_SHAREDREGION_DESTROY                  __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    3,\
                                                    SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_start
 */
#define DCMD_SHAREDREGION_START                    __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    4,\
                                                    SharedRegionDrv_CmdArgs)

/*!
*   @brief  Command for SharedRegion_stop
*/
#define DCMD_SHAREDREGION_STOP                     __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    5,\
                                                    SharedRegionDrv_CmdArgs)

/*!
*   @brief  Command for SharedRegion_attach
*/
#define DCMD_SHAREDREGION_ATTACH                   __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    6,\
                                                    SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_detach
                                             */
#define DCMD_SHAREDREGION_DETACH                   __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    7,\
                                                    SharedRegionDrv_CmdArgs)

/*!
*   @brief  Command for SharedRegion_getHeap
*/
#define DCMD_SHAREDREGION_GETHEAP                  __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    8,\
                                                    SharedRegionDrv_CmdArgs)
/*!
 *  @brief  Command for SharedRegion_clearEntry
*/
#define DCMD_SHAREDREGION_CLEARENTRY          __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                9,\
                                                SharedRegionDrv_CmdArgs)
/*!
*
*   @brief  Command for SharedRegion_setEntry
*/
#define DCMD_SHAREDREGION_SETENTRY             __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                10,\
                                                SharedRegionDrv_CmdArgs)
/*!
 *
*   @brief  Command for SharedRegion_reserveMemory
*/
#define DCMD_SHAREDREGION_RESERVEMEMORY            __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    11,\
                                                    SharedRegionDrv_CmdArgs)
/*!
*
*   @brief  Command for SharedRegion_clearReservedMemory
*/
#define DCMD_SHAREDREGION_CLEARRESERVEDMEMORY      __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                    12,\
                                                    SharedRegionDrv_CmdArgs)
/*!
 *
 *  @brief  Command for To get the shared region info which is set in kernel
*           Space.
*/
#define DCMD_SHAREDREGION_GETREGIONINFO         __DIOTF(_DCMD_SYSLINK_SHAREDREGION,\
                                                13,\
                                                SharedRegionDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for Ipc
*   ----------------------------------------------------------------------------
*/

/*!
*   @brief  Command for Ipc_control
 */
#define DCMD_IPC_CONTROL                 __DIOTF(_DCMD_SYSLINK_IPC,\
                                            1,\
                                            IpcDrv_CmdArgs)
/*!
*   @brief  Command for Ipc_readConfig
*/
#define DCMD_IPC_READCONFIG              __DIOTF(_DCMD_SYSLINK_IPC,\
                                            2,\
                                            IpcDrv_CmdArgs)
/*!
*   @brief  Command for Ipc_writeConfig
 */
#define DCMD_IPC_WRITECONFIG             __DIOTF(_DCMD_SYSLINK_IPC,\
                                            3,\
                                            IpcDrv_CmdArgs)

/*!
 *  @brief  Command for Ipc_isAttached
 */
#define DCMD_IPC_ISATTACHED              __DIOTF(_DCMD_SYSLINK_IPC,\
                                            4,\
                                            IpcDrv_CmdArgs)

/*!  ----------------------------------------------------------------------------
 *  IOCTL command IDs for HeapMemMP
 *  ----------------------------------------------------------------------------
 */
/*!
*   @brief  Command for HeapMemMP_getConfig
*/
#define DCMD_HEAPMEMMP_GETCONFIG         __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            1,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_setup
*/
#define DCMD_HEAPMEMMP_SETUP             __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            2,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_setup
*/
#define DCMD_HEAPMEMMP_DESTROY           __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                             3,\
                                            HeapMemMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapMemMP_destroy
 */
#define DCMD_HEAPMEMMP_PARAMS_INIT         __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            4,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_create
*/
#define DCMD_HEAPMEMMP_CREATE            __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                             5,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_delete
*/
#define DCMD_HEAPMEMMP_DELETE            __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            6,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_open
*/
#define DCMD_HEAPMEMMP_OPEN              __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            7,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_close
*/
#define DCMD_HEAPMEMMP_CLOSE             __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            8,\
                                            HeapMemMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapMemMP_alloc
*/
#define DCMD_HEAPMEMMP_ALLOC             __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            9,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_free
*/
#define DCMD_HEAPMEMMP_FREE              __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            10,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_sharedMemReq
*/
#define DCMD_HEAPMEMMP_SHAREDMEMREQ      __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            11,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_getStats
*/
#define DCMD_HEAPMEMMP_GETSTATS          __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            12,\
                                            HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_getExtendedStats
*/
#define DCMD_HEAPMEMMP_GETEXTENDEDSTATS  __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                                13,\
                                                HeapMemMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapMemMP_restore
*/
#define DCMD_HEAPMEMMP_RESTORE           __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            14,\
                                            HeapMemMPDrv_CmdArgs)

/*!
*   @brief  Command for HeapMemMP_openByAddr
*/
#define DCMD_HEAPMEMMP_OPENBYADDR       __DIOTF(_DCMD_SYSLINK_HEAPMEMMP,\
                                            15,\
                                            HeapMemMPDrv_CmdArgs)

typedef struct osalmemmap_cmdargs {
    size_t len;  // len of mapping
    off_t paddr; // paddr to be mapped
    void *vaddr; // vaddr to be unmapped or returned vaddr
    bool isCached; // is cached?
    int status;  // reply status
} OsalMemMap_CmdArgs;

#define DCMD_OSALMEM_MMAP      __DIOTF(_DCMD_SYSLINK_OSALMEM,\
                               DCMD_OSALMEM_MMAP_VAL,\
                               OsalMemMap_CmdArgs)

#define DCMD_OSALMEM_MUNMAP    __DIOTF(_DCMD_SYSLINK_OSALMEM,\
                               DCMD_OSALMEM_MUNMAP_VAL,\
                               OsalMemMap_CmdArgs)
/*!
*   @brief  Command for FrameQ_getVNumFreeFrames
*/
#define DCMD_TRACEDRV_SETTRACE          __DIOTF(_DCMD_SYSLINK_OSALMEM,\
                                          3, \
                                        TraceDrv_CmdArgs)
/*  ----------------------------------------------------------------------------
 *  DEVCTL command IDs for ProcMgr
 *  ----------------------------------------------------------------------------
*/

/*!
 *  @brief  Command for ProcMgr_attach
*/



#define DCMD_PROCMGR_ATTACH            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x1,\
                                            ProcMgr_CmdArgsAttach)

#define DCMD_PROCMGR_DETACH            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x2,\
                                            ProcMgr_CmdArgsDetach)

#define DCMD_PROCMGR_LOAD            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x3,\
                                            ProcMgr_CmdArgsLoad)
#define DCMD_PROCMGR_UNLOAD            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x4,\
                                            ProcMgr_CmdArgsUnload)

#define DCMD_PROCMGR_OPEN            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x5,\
                                            ProcMgr_CmdArgsOpen)

#define DCMD_PROCMGR_CLOSE            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x6,\
                                            ProcMgr_CmdArgsClose)

#define DCMD_PROCMGR_CONTROL            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x7,\
                                            ProcMgr_CmdArgsControl)

#define DCMD_PROCMGR_CREATE             __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x8,\
                                            ProcMgr_CmdArgsCreate)

#define DCMD_PROCMGR_DELETE            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x9,\
                                            ProcMgr_CmdArgsDelete)

#define DCMD_PROCMGR_SETUP            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0xa,\
                                            ProcMgr_CmdArgsSetup)

#define DCMD_PROCMGR_DESTROY            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0xb,\
                                            ProcMgr_CmdArgsDestroy)

#define DCMD_PROCMGR_START            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0xc,\
                                            ProcMgr_CmdArgsStart)

#define DCMD_PROCMGR_STOP            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0xd,\
                                            ProcMgr_CmdArgsStop)


#define DCMD_PROCMGR_GETATTACHPARAMS            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0xe,\
                                            ProcMgr_CmdArgsGetAttachParams)

#define DCMD_PROCMGR_GETSTARTPARAMS            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0xf,\
                                            ProcMgr_CmdArgsGetStartParams)

#define DCMD_PROCMGR_GETCONFIG            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x10,\
                                            ProcMgr_CmdArgsGetConfig)

#define DCMD_PROCMGR_GETSECTIONDATA            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x11,\
                                            ProcMgr_CmdArgsGetSectionData)

#define DCMD_PROCMGR_GETSTATE            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x12,\
                                            ProcMgr_CmdArgsGetState)

#define DCMD_PROCMGR_GETSYMBOLADDR            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x13,\
                                            ProcMgr_CmdArgsGetSymbolAddress)

#define DCMD_PROCMGR_MAP            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x14,\
                                            ProcMgr_CmdArgsMap)

#define DCMD_PROCMGR_UNMAP            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x15,\
                                            ProcMgr_CmdArgsUnmap)

#define DCMD_PROCMGR_GETPROCINFO            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x16,\
                                            ProcMgr_CmdArgsGetProcInfo)

#define DCMD_PROCMGR_GETSECTIONINFO            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x17,\
                                            ProcMgr_CmdArgsGetSectionInfo)

#define DCMD_PROCMGR_READ            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x18,\
                                            ProcMgr_CmdArgsRead)

#define DCMD_PROCMGR_WRITE            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x19,\
                                            ProcMgr_CmdArgsWrite)

#define DCMD_PROCMGR_TRANSLATEADDR            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x1a,\
                                            ProcMgr_CmdArgsTranslateAddr)

#define DCMD_PROCMGR_PARAMS_INIT            __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x1b,\
                                            ProcMgr_CmdArgsTranslateAddr)

#define DCMD_PROCMGR_GETLOADEDFILEID        __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x1c,\
                                            ProcMgr_CmdArgsTranslateAddr)

#define DCMD_PROCMGR_CONFIGSYSMEMMAP        __DIOTF(_DCMD_SYSLINK_PROCMGR,\
                                            0x1d,\
                                            ProcMgr_CmdArgsTranslateAddr)

/*  ----------------------------------------------------------------------------
 * IOCTL command IDs for GateMP
*   ----------------------------------------------------------------------------
*/

/*!
*   @brief  Command for GateMP_getConfig
*/
#define DCMD_GATEMP_GETCONFIG           __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            1,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_setup
*/
#define DCMD_GATEMP_SETUP               __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            2,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_setup
*/
#define DCMD_GATEMP_DESTROY            __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            3,\
                                            GateMPDrv_CmdArgs)
        /*!
        *   @brief  Command for GateMP_destroy
                                             */
#define DCMD_GATEMP_PARAMS_INIT           __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            4,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_create
*/
#define DCMD_GATEMP_CREATE              __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            5,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_delete
*/
#define DCMD_GATEMP_DELETE              __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            6,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_open
*/
#define DCMD_GATEMP_OPEN                  __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                                7,\
                                                GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_close
*/
#define DCMD_GATEMP_CLOSE               __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            8,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_enter
*/
#define DCMD_GATEMP_ENTER                 __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            9,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_leave
*/
#define DCMD_GATEMP_LEAVE               __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            10,\
                                            GateMPDrv_CmdArgs)
/*!
*   @brief  Command for GateMP_sharedMemReq
*/
#define DCMD_GATEMP_SHAREDMEMREQ        __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            11,\
                                            GateMPDrv_CmdArgs)

/*!
 *  @brief  Command for GateMP_openByAddr
*/
#define DCMD_GATEMP_OPENBYADDR          __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            12,\
                                            GateMPDrv_CmdArgs)

/*!
 *  @brief  Command for GateMP_getDefaultRemote
*/
#define DCMD_GATEMP_GETDEFAULTREMOTE    __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            13,\
                                            GateMPDrv_CmdArgs)

#define DCMD_GATEMP_GETSHAREDADDR       __DIOTF(_DCMD_SYSLINK_GATEMP,\
                                            15,\
                                            GateMPDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for ListMP
 *  ----------------------------------------------------------------------------
*/
/*!
*   @brief  Command for ListMP_getConfig
*/
#define DCMD_LISTMP_GETCONFIG    __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    1,\
                                    ListMPDrv_CmdArgs)

/*!
*   @brief  Command for ListMP_setup
*/
#define DCMD_LISTMP_SETUP        __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    2,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_setup
*/
#define DCMD_LISTMP_DESTROY      __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    3,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_destroy
*/
#define DCMD_LISTMP_PARAMS_INIT  __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    4,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_create
*/
#define DCMD_LISTMP_CREATE       __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    5,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_delete
*/
#define DCMD_LISTMP_DELETE       __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    6,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_open
*/
#define DCMD_LISTMP_OPEN    __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                7,\
                                ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_close
*/
#define DCMD_LISTMP_CLOSE    __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                8,\
                                ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_isEmpty
*/
#define DCMD_LISTMP_ISEMPTY    __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                9,\
                                ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_getHead
*/
#define DCMD_LISTMP_GETHEAD      __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                10,\
                                ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_getTail
 */
#define DCMD_LISTMP_GETTAIL        __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                11,\
                                ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_putHead
*/
#define DCMD_LISTMP_PUTHEAD       __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    12,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_putTail
*/
#define DCMD_LISTMP_PUTTAIL      __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    13,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_insert
*/
#define DCMD_LISTMP_INSERT       __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    14,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_remove
*/
#define DCMD_LISTMP_REMOVE       __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    15,\
                                    ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_next
 */
#define DCMD_LISTMP_NEXT         __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    16,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_prev
*/
#define DCMD_LISTMP_PREV         __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    17,\
                                    ListMPDrv_CmdArgs)
/*!
*   @brief  Command for ListMP_sharedMemReq
*/
#define DCMD_LISTMP_SHAREDMEMREQ __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    18,\
                                    ListMPDrv_CmdArgs)

/*!
*   @brief  Command for ListMP_openByAddr
*/
#define DCMD_LISTMP_OPENBYADDR  __DIOTF(_DCMD_SYSLINK_LISTMP,\
                                    19,\
                                    ListMPDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for HeapBufMP
*   ----------------------------------------------------------------------------
*/

/*!
*   @brief  Command for HeapBufMP_getConfig
*/
#define DCMD_HEAPBUFMP_GETCONFIG         __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        1,\
                                        HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_setup
*/
#define DCMD_HEAPBUFMP_SETUP             __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        2,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_setup
*/
#define DCMD_HEAPBUFMP_DESTROY           __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        3,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_destroy
*/
#define DCMD_HEAPBUFMP_PARAMS_INIT         __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        4,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_create
*/
#define DCMD_HEAPBUFMP_CREATE            __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        5,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_delete
*/
#define DCMD_HEAPBUFMP_DELETE            __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        6,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_open
*/
#define DCMD_HEAPBUFMP_OPEN              __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        7,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_close
*/
#define DCMD_HEAPBUFMP_CLOSE             __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        8,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_alloc
*/
#define DCMD_HEAPBUFMP_ALLOC             __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        9,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_free
 */
#define DCMD_HEAPBUFMP_FREE              __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        10,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_sharedMemReq
 */
#define DCMD_HEAPBUFMP_SHAREDMEMREQ      __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        11,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_getStats
*/
#define DCMD_HEAPBUFMP_GETSTATS          __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        12,\
                                        HeapBufMPDrv_CmdArgs)
/*!
*   @brief  Command for HeapBufMP_getExtendedStats
*/
#define DCMD_HEAPBUFMP_GETEXTENDEDSTATS  __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        13,\
                                        HeapBufMPDrv_CmdArgs)

/*!
*   @brief  Command for HeapBufMP_open
*/
#define DCMD_HEAPBUFMP_OPENBYADDR        __DIOTF(_DCMD_SYSLINK_HEAPBUFMP,\
                                        14,\
                                        HeapBufMPDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for Notify
*   ----------------------------------------------------------------------------
*/

/*!
 *  @brief  Command for Notify_getConfig
*/
#define DCMD_NOTIFY_GETCONFIG                     __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                1,\
                                                Notify_CmdArgsGetConfig)

/*!
 *  @brief  Command for Notify_setup
*/
#define DCMD_NOTIFY_SETUP                         __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                2,\
                                                Notify_CmdArgsSetup)

/*!
 *  @brief  Command for Notify_destroy
*/
#define DCMD_NOTIFY_DESTROY                       __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                3,\
                                                Notify_CmdArgsDestroy)

/*!
 *  @brief  Command for Notify_registerEvent
*/
#define DCMD_NOTIFY_REGISTEREVENT                 __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                4,\
                                                Notify_CmdArgsRegisterEvent)

/*!
 *  @brief  Command for Notify_unregisterEvent
*/
#define DCMD_NOTIFY_UNREGISTEREVENT               __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                5,\
                                                Notify_CmdArgsUnregisterEvent)

/*!
 *  @brief  Command for Notify_sendEvent
*/
#define DCMD_NOTIFY_SENDEVENT                     __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                6,\
                                                Notify_CmdArgsSendEvent)

/*!
 *  @brief  Command for Notify_disable
*/
#define DCMD_NOTIFY_DISABLE                       __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                7,\
                                                Notify_CmdArgsDisable)

/*!
 *  @brief  Command for Notify_restore
*/
#define DCMD_NOTIFY_RESTORE                       __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                8,\
                                                Notify_CmdArgsRestore)

/*!
 *  @brief  Command for Notify_disableEvent
*/
#define DCMD_NOTIFY_DISABLEEVENT                  __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                    9,\
                                                    Notify_CmdArgsDisableEvent)

/*!
 *  @brief  Command for Notify_enableEvent
*/
#define DCMD_NOTIFY_ENABLEEVENT                   __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                10,\
                                                Notify_CmdArgsEnableEvent)

/*!
 *  @brief  Command for Notify_attach
*/
#define DCMD_NOTIFY_ATTACH                        __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                11,\
                                                Notify_CmdArgsAttach)

/*!
 *  @brief  Command for Notify_detach
*/
#define DCMD_NOTIFY_DETACH                        __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                12,\
                                                Notify_CmdArgsDetach)

/*!
 *  @brief  Command for Notify_attach
*/
#define DCMD_NOTIFY_THREADATTACH                  __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                13,\
                                                Notify_CmdArgs)

/*!
 *  @brief  Command for Notify_detach
*/
#define DCMD_NOTIFY_THREADDETACH                  __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                14,\
                                                Notify_CmdArgs)

/*!
 *  @brief  Command for Notify_intLineRegistered
*/
#define DCMD_NOTIFY_INTLINEREGISTERED             __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                15,\
                                                Notify_CmdArgsIntLineRegistered)

/*!
 *  @brief  Command for Notify_sharedMemReq
*/
#define DCMD_NOTIFY_SHAREDMEMREQ                  __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                16,\
                                                Notify_CmdArgsSharedMemReq)
/*!
 *  @brief  Command for Notify_registerEventSingle
*/
#define DCMD_NOTIFY_REGISTEREVENTSINGLE           __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                17,\
                                                Notify_CmdArgsRegisterEvent)

/*!
 *  @brief  Command for Notify_unregisterEventSingle
*/
#define DCMD_NOTIFY_UNREGISTEREVENTSINGLE         __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                18,\
                                                Notify_CmdArgsUnregisterEvent)

/*!
 *  @brief  Command for Notify_eventAvailable
*/
#define DCMD_NOTIFY_EVENTAVAILABLE                __DIOTF(_DCMD_SYSLINK_NOTIFY,\
                                                19,\
                                                Notify_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for MessageQ
*   ----------------------------------------------------------------------------
*/

/*!
 *  @brief  Command for MessageQ_getConfig
*/
#define DCMD_MESSAGEQ_GETCONFIG              __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            1,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_setup
*/
#define DCMD_MESSAGEQ_SETUP                  __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            2,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_setup
*/
#define DCMD_MESSAGEQ_DESTROY                __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            3,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_destroy
*/
#define DCMD_MESSAGEQ_PARAMS_INIT            __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            4,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_create
*/
#define DCMD_MESSAGEQ_CREATE                 __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            5,\
                                            MessageQDrv_CmdArgs)

/*!
 *  @brief  Command for MessageQ_delete
*/
#define DCMD_MESSAGEQ_DELETE                 __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            6,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_open
*/
#define DCMD_MESSAGEQ_OPEN                   __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            7,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_close
*/
#define DCMD_MESSAGEQ_CLOSE                  __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            8,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_count
*/
#define DCMD_MESSAGEQ_COUNT                  __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            9,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_alloc
*/
#define DCMD_MESSAGEQ_ALLOC                  __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            10,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_free
 */
#define DCMD_MESSAGEQ_FREE                   __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            11,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_put
*/
#define DCMD_MESSAGEQ_PUT                    __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            12,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_registerHeap
*/
#define DCMD_MESSAGEQ_REGISTERHEAP           __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            13,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_unregisterHeap
*/
#define DCMD_MESSAGEQ_UNREGISTERHEAP         __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            14,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_attach
*/
#define DCMD_MESSAGEQ_ATTACH                 __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            15,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_detach
*/
#define DCMD_MESSAGEQ_DETACH                 __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            16,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_get
*/
#define DCMD_MESSAGEQ_GET                    __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            17,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_sharedMemReq
*/
#define DCMD_MESSAGEQ_SHAREDMEMREQ           __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            18,\
                                            MessageQDrv_CmdArgs)
/*!
 *  @brief  Command for MessageQ_unblock
*/
#define DCMD_MESSAGEQ_UNBLOCK                __DIOTF(_DCMD_SYSLINK_MESSAGEQ,\
                                            19,\
                                            MessageQDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for RingIO
 *  ----------------------------------------------------------------------------
*/

/*!
*   @brief  Command for RingIO_getConfig
*/
#define DCMD_RINGIO_GETCONFIG                     __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      1, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_setup
*/
#define DCMD_RINGIO_SETUP                         __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      2, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_setup
*/
#define DCMD_RINGIO_DESTROY                       __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      3, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_destroy
*/
#define DCMD_RINGIO_PARAMS_INIT                   __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      4, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_create
*/
#define DCMD_RINGIO_CREATE                        __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      5, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_delete
*/
#define DCMD_RINGIO_DELETE                         __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      6, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_open
                                             */
#define DCMD_RINGIO_OPEN                          __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      7, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_close
*/
#define DCMD_RINGIO_CLOSE                         __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      8, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_registerNotifier
*/
#define DCMD_RINGIO_REGISTERNOTIFIER              __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      11, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_unregisterNotifier
*/
#define DCMD_RINGIO_UNREGISTERNOTIFIER            __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      12, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_getValidSize
*/
#define DCMD_RINGIO_GETVALIDSIZE                  __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      13, \
                                                    RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getEmptySize
*/
#define DCMD_RINGIO_GETEMPTYSIZE                  __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      14, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_getValidAttrSize
*/
#define DCMD_RINGIO_GETVALIDATTRSIZE              __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      15, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_getEmptyAttrSize
*/
#define DCMD_RINGIO_GETEMPTYATTRSIZE              __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      16, \
                                                    RingIODrv_CmdArgs)
/*!
 * @brief   Command for RingIO_getAcquiredOffset
 */
#define DCMD_RINGIO_GETACQUIREDOFFSET             __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      17, \
                                                    RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getAcquiredSize
 */
#define DCMD_RINGIO_GETACQUIREDSIZE               __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      18, \
                                                    RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getWaterMark
 */
#define DCMD_RINGIO_GETWATERMARK                  __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      19, \
                                                    RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_acquire
 */
#define DCMD_RINGIO_ACQUIRE                       __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      20, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_release
*/
#define DCMD_RINGIO_RELEASE                       __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      21, \
                                                    RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_cancel
*/
#define DCMD_RINGIO_CANCEL                        __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      22, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_getvAttribute
*/
#define DCMD_RINGIO_GETVATTRIBUTE                 __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      23, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_setvAttribute
*/
#define DCMD_RINGIO_SETVATTRIBUTE                 __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      24, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_notify
*/
#define DCMD_RINGIO_SYNC                          __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      25, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_flush
*/
#define DCMD_RINGIO_FLUSH                         __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      26, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_sharedMemReq
*/
#define DCMD_RINGIO_SHAREDMEMREQ                  __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      27, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command to set the notify id in the instance's kernel object
*/

#define DCMD_RINGIO_SET_NOTIFYID                  __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      28, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command to reset the notify id in the instance's kernel object
*/
#define DCMD_RINGIO_RESET_NOTIFYID                __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      29, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_setNotifyType
*/
#define DCMD_RINGIO_SETNOTIFYTYPE                 __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      30, \
                                                    RingIODrv_CmdArgs)
/*!
*   @brief  Command for RingIO_setWaterMark
*/
#define DCMD_RINGIO_SETWATERMARK                  __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                      31, \
                                                    RingIODrv_CmdArgs)

/*!
*   @brief  Command for RingIO_open
*/
#define DCMD_RINGIO_OPENBYADDR                    __DIOTF(_DCMD_SYSLINK_RINGIO,\
                                                          32, \
                                                        RingIODrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for RingIOShm
*   ----------------------------------------------------------------------------
*/
/*!
*   @brief  Command for ringioshm_getconfig
*/
#define DCMD_RINGIOSHM_GETCONFIG                        __DIOTF(_DCMD_SYSLINK_RINGIOSHM,\
                                                          1, \
                                                        RingIOShmDrv_CmdArgs)
/*!
*   @brief  Command for ringioshm_setup
*/
#define DCMD_RINGIOSHM_SETUP                            __DIOTF(_DCMD_SYSLINK_RINGIOSHM,\
                                                          2, \
                                                        RingIOShmDrv_CmdArgs)
/*!
*   @brief  Command for ringioshm_destroy
*/
#define DCMD_RINGIOSHM_DESTROY                          __DIOTF(_DCMD_SYSLINK_RINGIOSHM,\
                                                          3, \
                                                        RingIOShmDrv_CmdArgs)
/*!
*   @brief  Command for ringioshm_params_init
*/
#define DCMD_RINGIOSHM_PARAMS_INIT                      __DIOTF(_DCMD_SYSLINK_RINGIOSHM,\
                                                          4, \
                                                        RingIOShmDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for ClientMgrNotify
*   ----------------------------------------------------------------------------
*/
/*!
*   @brief  Command for clientnotifymgr_getconfig
*/
#define DCMD_CLIENTNOTIFYMGR_GETCONFIG                      __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              1, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_setup
*/
#define DCMD_CLIENTNOTIFYMGR_SETUP                          __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              2, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_destroy
*/
#define DCMD_CLIENTNOTIFYMGR_DESTROY                        __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              3, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_params_init
*/
#define DCMD_CLIENTNOTIFYMGR_PARAMS_INIT                        __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              4, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_create
*/
#define DCMD_CLIENTNOTIFYMGR_CREATE                         __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              5, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_delete
*/
#define DCMD_CLIENTNOTIFYMGR_DELETE                         __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              6, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_open
*/
#define DCMD_CLIENTNOTIFYMGR_OPEN                           __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              7, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_close
*/
#define DCMD_CLIENTNOTIFYMGR_CLOSE                          __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              8, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_sharedmemmodreq
*/
#define DCMD_CLIENTNOTIFYMGR_SHAREDMEMMODREQ                __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              9, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_sharedmemreq
*/
#define DCMD_CLIENTNOTIFYMGR_SHAREDMEMREQ                   __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              10, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_notifydr _register
*/
#define DCMD_CLIENTNOTIFYMGR_NOTIFYDRV_REGISTER             __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              11, \
                                                            ClientNotifyMgrDrv_CmdArgs)
/*!
*   @brief  Command for clientnotifymgr_notifydrv_unregister
*/
#define DCMD_CLIENTNOTIFYMGR_NOTIFYDRV_UNREGISTER           __DIOTF(_DCMD_SYSLINK_CLIENTNOTIFYMGR,\
                                                              12, \
                                                            ClientNotifyMgrDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for FrameQ
*   ----------------------------------------------------------------------------
*/
/*!
*   @brief  Command for FrameQ_getConfig
*/
#define DCMD_FRAMEQ_GETCONFIG                               __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              1, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_setup
*/
#define DCMD_FRAMEQ_SETUP                                   __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              2, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_destroy
*/
#define DCMD_FRAMEQ_DESTROY                                 __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              3, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_parms_init
*/
#define DCMD_FRAMEQ_PARAMS_INIT                             __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              4, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_create
*/
#define DCMD_FRAMEQ_CREATE                                  __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              5, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_delete
*/
#define DCMD_FRAMEQ_DELETE                                  __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              6, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_open
*/
#define DCMD_FRAMEQ_OPEN                                    __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              7, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_close
*/
#define DCMD_FRAMEQ_CLOSE                                   __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              8, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_alloc
*/
#define DCMD_FRAMEQ_ALLOC                                   __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              9, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_allocv
*/
#define DCMD_FRAMEQ_ALLOCV                                  __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              10, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_free
*/
#define DCMD_FRAMEQ_FREE                                    __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              11, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_freev
*/
#define DCMD_FRAMEQ_FREEV                                   __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              12, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_dup
*/
#define DCMD_FRAMEQ_DUP                                     __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              13, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_put
*/
#define DCMD_FRAMEQ_PUT                                     __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              14, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_putv
*/
#define DCMD_FRAMEQ_PUTV                                    __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              15, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_get
*/
#define DCMD_FRAMEQ_GET                                     __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              16, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getv
*/
#define DCMD_FRAMEQ_GETV                                    __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              17, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_registerNotifier
*/
#define DCMD_FRAMEQ_REG_NOTIFIER                            __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              18, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_unregisterNotifier
*/
#define DCMD_FRAMEQ_UNREG_NOTIFIER                          __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              19, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getNumFrames
*/
#define DCMD_FRAMEQ_GET_NUMFRAMES                           __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              20, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getNumFrames
*/
#define DCMD_FRAMEQ_GET_VNUMFRAMES                          __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              21, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getNumFrames
*/
#define DCMD_FRAMEQ_CONTROL                                 __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              22, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command to set the notify id in the instance's kernel object
*/
#define DCMD_FRAMEQ_SET_NOTIFYID                            __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              23, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command to reset the notify id in the instance's kernel object
*/
#define DCMD_FRAMEQ_RESET_NOTIFYID                          __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              24, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command to get the default instance params for ShMem instance.
*/
#define DCMD_FRAMEQ_SHMEM_PARAMS_INIT                       __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              25, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command to get the shared memory required for ShMem instance.
*/
#define DCMD_FRAMEQ_SHMEM_MEMREQ                            __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              26, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getNumFrames
*/
#define DCMD_FRAMEQ_GET_NUMFREEFRAMES                       __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              27, \
                                                            FrameQDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_vNumFreeFrames
*/
#define DCMD_FRAMEQ_GET_VNUMFREEFRAMES                      __DIOTF(_DCMD_SYSLINK_FRAMEQ,\
                                                              28, \
                                                            FrameQDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
*   IOCTL command IDs for FrameQBufferMgr
*   ----------------------------------------------------------------------------
*/
/*!
*   @brief  Command for FrameQBufMgr_getConfig
*/
#define DCMD_FRAMEQBUFMGR_GETCONFIG                         __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              1, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_setup
*/
#define DCMD_FRAMEQBUFMGR_SETUP                             __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              2, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_destroy
*/
#define DCMD_FRAMEQBUFMGR_DESTROY                           __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              3, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_Params_init
*/
#define DCMD_FRAMEQBUFMGR_PARAMS_INIT                       __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              4, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_create
*/
#define DCMD_FRAMEQBUFMGR_CREATE                            __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              5, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_delete
*/
#define DCMD_FRAMEQBUFMGR_DELETE                            __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              6, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_open
*/
#define DCMD_FRAMEQBUFMGR_OPEN                              __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              7, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_close
*/
#define DCMD_FRAMEQBUFMGR_CLOSE                             __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              8, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_sharedMemModReq
*/
#define DCMD_FRAMEQBUFMGR_SHAREDMEMMODREQ                   __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              9, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_sharedMemReq
*/
#define DCMD_FRAMEQBUFMGR_SHAREDMEMREQ                      __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              10, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_getHandle
*/
#define DCMD_FRAMEQBUFMGR_GETHANDLE                         __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              11, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_getId
*/
#define DCMD_FRAMEQBUFMGR_GETID                             __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              12, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_alloc
*/
#define DCMD_FRAMEQBUFMGR_ALLOC                             __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              13, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_allocv
*/
#define DCMD_FRAMEQBUFMGR_ALLOCV                            __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              14, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_free
*/
#define DCMD_FRAMEQBUFMGR_FREE                          __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              15, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_free
*/
#define DCMD_FRAMEQBUFMGR_FREEV                             __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              16, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_add
*/
#define DCMD_FRAMEQBUFMGR_ADD                               __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              17, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_remove
*/
#define DCMD_FRAMEQBUFMGR_REMOVE                            __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              18, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_registerNotifier
*/
#define DCMD_FRAMEQBUFMGR_REG_NOTIFIER                      __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              19, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_unregisterNotifier
*/
#define DCMD_FRAMEQBUFMGR_UNREG_NOTIFIER                    __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              20, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_dup
*/
#define DCMD_FRAMEQBUFMGR_DUP                               __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              21, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQBufMgr_dupv
*/
#define DCMD_FRAMEQBUFMGR_DUPV                              __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              22, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command to set the notify id in the instance's kernel object.
*/
#define DCMD_FRAMEQBUFMGR_SET_NOTIFYID                      __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              23, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command to reset the notify id in the instance's kernel object
*/
#define DCMD_FRAMEQBUFMGR_RESET_NOTIFYID                    __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              24, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command to transalte the given address to destination address type.
*/
#define DCMD_FRAMEQBUFMGR_TRANSLATE                         __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              25, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command to perform custom operation on the instance.
*/
#define DCMD_FRAMEQBUFMGR_CONTROL                           __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              26, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command to get the default params for ShMem .
*/
#define DCMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT                 __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              27, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getNumFreeFrames
*/
#define DCMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES                 __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              28, \
                                                            FrameQBufMgrDrv_CmdArgs)
/*!
*   @brief  Command for FrameQ_getVNumFreeFrames
*/
#define DCMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES                __DIOTF(_DCMD_SYSLINK_FRAMEQBUFMGR,\
                                                              29, \
                                                            FrameQBufMgrDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for Trace
 *  ----------------------------------------------------------------------------
 */


/*  ----------------------------------------------------------------------------
 *  DEVCTL command IDs for IpcMemMgr
 *  ----------------------------------------------------------------------------
 */

/*!
 *  @brief  Command for IpcMemMgr_getConfig
 */
#define DCMD_IPCMEMMGR_GETCONFIG \
        __DIOF(_DCMD_SYSLINK_IPCMEMMGR, 1, IpcMemMgr_Config)

/*!
 *  @brief  Command for IpcMemMgr_setup
 */
#define DCMD_IPCMEMMGR_SETUP \
        __DIOF(_DCMD_SYSLINK_IPCMEMMGR, 2, IpcMemMgr_Config)

/*!
 *  @brief  Command for IpcMemMgr_destroy
 */
#define DCMD_IPCMEMMGR_DESTROY \
        __DIOF(_DCMD_SYSLINK_IPCMEMMGR, 3, Int)

/*!
 *  @brief  Command for IpcMemMgr_alloc
 */
#define DCMD_IPCMEMMGR_ALLOC \
        __DIOTF(_DCMD_SYSLINK_IPCMEMMGR, 4, IpcMemMgrDrv_CmdArgs)

/*!
 *  @brief  Command for IpcMemMgr_free
 */
#define DCMD_IPCMEMMGR_FREE \
        __DIOT(_DCMD_SYSLINK_IPCMEMMGR, 5, IpcMemMgrDrv_CmdArgs)

/*!
 *  @brief  Command for IpcMemMgr_acquire
 */
#define DCMD_IPCMEMMGR_ACQUIRE \
        __DIOTF(_DCMD_SYSLINK_IPCMEMMGR, 6, IpcMemMgrDrv_CmdArgs)

/*!
 *  @brief  Command for IpcMemMgr_release
 */
#define DCMD_IPCMEMMGR_RELEASE \
        __DIOT(_DCMD_SYSLINK_IPCMEMMGR, 7, String)
