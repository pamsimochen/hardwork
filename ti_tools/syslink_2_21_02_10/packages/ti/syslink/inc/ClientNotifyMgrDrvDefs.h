/** 
 *  @file   ClientNotifyMgrDrvDefs.h
 *
 *  @brief      Definitions of ClientNotifyMgrDrv types and structures.
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



#ifndef CLIENTNOTIFYMGRDRVDEFS_H_0x684e
#define CLIENTNOTIFYMGRDRVDEFS_H_0x684e


/* Utilities headers */
#include "_GateMP.h"
#include <ti/ipc/GateMP.h>
#include "ClientNotifyMgr.h"
#include <ti/ipc/SharedRegion.h>
#include "IpcCmdBase.h"
#include "IoctlDefs.h"

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for ClientNotifyMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for ClientNotifyMgr
 */
#define CLIENTNOTIFYMGR_BASE_CMD                  (0x150)

/*!
 *  @brief  Command for ClientNotifyMgr_getConfig
 */
#define CMD_CLIENTNOTIFYMGR_GETCONFIG            _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 1u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_setup
 */
#define CMD_CLIENTNOTIFYMGR_SETUP            _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 2u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_setup
 */
#define CMD_CLIENTNOTIFYMGR_DESTROY            _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 3u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_Params_init
 */
#define CMD_CLIENTNOTIFYMGR_PARAMS_INIT            _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 4u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_create
 */
#define CMD_CLIENTNOTIFYMGR_CREATE               _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 5u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_delete
 */
#define CMD_CLIENTNOTIFYMGR_DELETE            _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 6u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_open
 */
#define CMD_CLIENTNOTIFYMGR_OPEN                  _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 7u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_close
 */
#define CMD_CLIENTNOTIFYMGR_CLOSE                 _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 8u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_sharedMemModReq
 */
#define CMD_CLIENTNOTIFYMGR_SHAREDMEMMODREQ      _IOWR(IPCCMDBASE,\
                                                 CLIENTNOTIFYMGR_BASE_CMD + 9u,\
                                                 ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_sharedMemReq
 */
#define CMD_CLIENTNOTIFYMGR_SHAREDMEMREQ        _IOWR(IPCCMDBASE,\
                                                CLIENTNOTIFYMGR_BASE_CMD + 10u,\
                                                ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_register
 */
#define CMD_CLIENTNOTIFYMGR_NOTIFYDRV_REGISTER  _IOWR(IPCCMDBASE,\
                                                CLIENTNOTIFYMGR_BASE_CMD + 11u,\
                                                ClientNotifyMgrDrv_CmdArgs)
/*!
 *  @brief  Command for ClientNotifyMgr_unregister
 */
#define CMD_CLIENTNOTIFYMGR_NOTIFYDRV_UNREGISTER _IOWR(IPCCMDBASE,\
                                                CLIENTNOTIFYMGR_BASE_CMD + 12u,\
                                                ClientNotifyMgrDrv_CmdArgs)
/*  ----------------------------------------------------------------------------
 *  Command arguments for ClientNotifyMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for ClientNotifyMgr
 */
typedef struct ClientNotifyMgrDrv_CmdArgs {
    union {
        struct {
            ClientNotifyMgr_Config   * config;
        } getConfig;

        struct {
            ClientNotifyMgr_Config   * config;
        } setup;

        struct {
            UInt16                  procId;
            Ptr                     notifyDrvHandle;
        } registerNotifyDriver;

        struct {
            UInt16                  procId;
        } unregisterNotifyDriver;

        struct {
            ClientNotifyMgr_Params   * params;
            UInt32                  bytes;
        } sharedMemReq;

        struct {
            ClientNotifyMgr_Handle     handle;
            ClientNotifyMgr_Params   * params;
        } ParamsInit;

        struct {
            ClientNotifyMgr_Handle     handle;
            ClientNotifyMgr_Params   * params;
            UInt32                     nameLen;
            UInt32                     Id;
#if defined(__QNX__)
            SharedRegion_SRPtr         gateMPSrPtr;
#else
            Ptr                        knlLockHandle;
#endif
            SharedRegion_SRPtr         sharedAddrSrPtr;
        } create;

        struct {
            ClientNotifyMgr_Handle     handle;
        } deleteInstance;

        struct {
            ClientNotifyMgr_Handle     handle;
            String                  name;
            UInt32                  nameLen;
            SharedRegion_SRPtr      sharedAddrSrPtr;
            GateMP_Handle           gate;
            UInt32                  cacheFlags;
            UInt32                  Id;
        } open;

        struct {
            ClientNotifyMgr_Handle     handle;
        } close;


        struct {
            ClientNotifyMgr_Handle     handle;
            UInt32                  notifyType;
            UInt32                  watermarkNumBuffers;
            UInt32                  watermarkSize;
            Ptr                     fxnPtr;
            Ptr                     cbContext;
            Ptr                     ClientHandle;
        } registerNotifier;

        struct {
            ClientNotifyMgr_Handle     handle;
        } unregisterNotifier;


    } args;

    Int32 apiStatus;
} ClientNotifyMgrDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* ClientNotifyMgrDRVDEFS_H_0x684e */
