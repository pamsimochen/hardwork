/** 
 *  @file   IpcDrvDefs.h
 *
 *  @brief      Definitions of IpcDrv types and structures.
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



#ifndef IPC_DRVDEFS_H_0xF414
#define IPC_DRVDEFS_H_0xF414


/* Utilities headers */
#include <ti/ipc/Ipc.h>

#include <ti/syslink/inc/IoctlDefs.h>
#include <ti/syslink/osal/OsalTypes.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for Ipc
 *  ----------------------------------------------------------------------------
 */

/*!
 *  @brief  Base command ID for Ipc
 */
#define IPCCMDBASE                   0xF4
/*!
 *  @brief  Base command ID for Ipc
 */
#define IPC_BASE_CMD                 150

/*!
 *  @brief  Command for Ipc_control
 */
#define CMD_IPC_CONTROL                 _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 1u,\
                                        IpcDrv_CmdArgs)
/*!
 *  @brief  Command for Ipc_readConfig
 */
#define CMD_IPC_READCONFIG              _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 2u,\
                                        IpcDrv_CmdArgs)
/*!
 *  @brief  Command for Ipc_writeConfig
 */
#define CMD_IPC_WRITECONFIG             _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 3u,\
                                        IpcDrv_CmdArgs)

/*!
 *  @brief  Command for Ipc_addTerminateEvent
 */
#define CMD_IPC_ADDTERMINATEEVENT       _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 4u,\
                                        IpcDrv_CmdArgs)

/*!
 *  @brief  Command for Ipc_removeTerminateEvent
 */
#define CMD_IPC_REMOVETERMINATEEVENT    _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 5u,\
                                        IpcDrv_CmdArgs)

/*!
 *  @brief  Command for Ipc_runtimeId
 */
#define CMD_IPC_RUNTIMEID               _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 6u,\
                                        IpcDrv_CmdArgs)

/*!
 *  @brief  Command for Ipc_isAttached
 */
#define CMD_IPC_ISATTACHED              _IOWR(IPCCMDBASE,\
                                        IPC_BASE_CMD + 7u,\
                                        IpcDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  Command arguments for Ipc
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for Ipc
 */
typedef struct IpcDrv_CmdArgs {
    union {
        struct {
            UInt16                procId;
            Int32                 cmdId;
            Ptr                   arg;
        } control;

        struct {
            UInt16                remoteProcId;
            UInt32                tag;
            Ptr                   cfg;
            SizeT                 size;
        } readConfig;

        struct {
            UInt16                remoteProcId;
            UInt32                tag;
            Ptr                   cfg;
            SizeT                 size;
        } writeConfig;

        struct {
            Osal_Pid            pid;
            UInt32              payload;
            UInt16              procId;
            Int                 policy;
        } addTermEvent;

        struct {
            Osal_Pid            pid;
            UInt16              procId;
        } removeTermEvent;

        struct {
            Osal_Pid            pid;
            UInt8               rtid;
        } runtimeId;

        struct {
            UInt16              remoteProcId;
            Bool                attached;
        } isAttached;
    } args;

    Int32 apiStatus;
} IpcDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* IPC_DRVDEFS_H_0xF414 */
