/** 
 *  @file   GateMPDrvDefs.h
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



#ifndef GATEMP_DRVDEFS_H_0xF415
#define GATEMP_DRVDEFS_H_0xF415


/* Utilities headers */
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/_GateMP.h>
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
 *  IOCTL command IDs for GateMP
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for GateMP
 */
#define GATEMP_BASE_CMD                 (0x150)

/*!
 *  @brief  Command for GateMP_getConfig
 */
#define CMD_GATEMP_GETCONFIG           _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 1u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_setup
 */
#define CMD_GATEMP_SETUP               _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 2u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_setup
 */
#define CMD_GATEMP_DESTROY            _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 3u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_destroy
 */
#define CMD_GATEMP_PARAMS_INIT           _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 4u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_create
 */
#define CMD_GATEMP_CREATE              _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 5u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_delete
 */
#define CMD_GATEMP_DELETE              _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 6u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_open
 */
#define CMD_GATEMP_OPEN                  _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 7u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_close
 */
#define CMD_GATEMP_CLOSE               _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 8u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_enter
 */
#define CMD_GATEMP_ENTER                 _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 9u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_leave
 */
#define CMD_GATEMP_LEAVE               _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 10u,\
                                             GateMPDrv_CmdArgs)
/*!
 *  @brief  Command for GateMP_sharedMemReq
 */
#define CMD_GATEMP_SHAREDMEMREQ        _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 11u,\
                                             GateMPDrv_CmdArgs)

/*!
 *  @brief  Command for GateMP_openByAddr
 */
#define CMD_GATEMP_OPENBYADDR          _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 12u,\
                                             GateMPDrv_CmdArgs)

/*!
 *  @brief  Command for GateMP_getDefaultRemote
 */
#define CMD_GATEMP_GETDEFAULTREMOTE    _IOWR(IPCCMDBASE,\
                                             GATEMP_BASE_CMD + 13u,\
                                             GateMPDrv_CmdArgs)

/*!
 *  @brief  Command for setting hwspinlocks as reserved
 */
#define CMD_GATEMP_SETRESERVED          _IOWR(IPCCMDBASE,\
                                              GATEMP_BASE_CMD + 14u,\
                                              GateMPDrv_CmdArgs)

#define CMD_GATEMP_GETSHAREDADDR        _IOWR(IPCCMDBASE,\
                                              GATEMP_BASE_CMD + 15u,\
                                              GateMPDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  Command arguments for GateMP
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for GateMP
 */
typedef struct GateMPDrv_CmdArgs {
    union {
        struct {
            GateMP_Params * params;
        } ParamsInit;

        struct {
            GateMP_Config * config;
        } getConfig;

        struct {
            GateMP_Config * config;
        } setup;

        struct {
            GateMP_Handle         handle;
            GateMP_Params * params;
            UInt32                nameLen;
            SharedRegion_SRPtr    sharedAddrSrPtr;
        } create;

        struct {
            GateMP_Handle         handle;
        } deleteInstance;

        struct {
            GateMP_Handle         handle;
            String                name;
            UInt32                nameLen;
            SharedRegion_SRPtr    sharedAddrSrPtr;
        } open;

        struct {
            GateMP_Handle         handle;
            SharedRegion_SRPtr    sharedAddrSrPtr;
        } openByAddr;

        struct {
            GateMP_Handle         handle;
        } close;

        struct {
            GateMP_Handle         handle;
            IArg                  flags;
        } enter;

        struct {
            GateMP_Handle         handle;
            IArg                  flags;
        } leave;

        struct {
            GateMP_Params       * params;
            UInt32                retVal;
        } sharedMemReq;

        struct {
            Ptr                  handle;
        } getDefaultRemote;
        struct {
            UInt32               remoteProtectType;
            UInt32               lockNum;
        } setReserved;
        struct {
            GateMP_Handle        handle;
            UInt32               retVal;
        } getSharedAddr;
    } args;

    Int32 apiStatus;
} GateMPDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEMP_DRVDEFS_H_0xF415 */
