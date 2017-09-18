/** 
 *  @file   IpcMemMgrDrvDefs.h
 *
 *  @brief      Definitions of IpcMemMgr driver types and structures.
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



#ifndef ti_syslink_inc_IpcMemMgrDrvDefs__include
#define ti_syslink_inc_IpcMemMgrDrvDefs__include


#include "UtilsCmdBase.h"
#include <ti/syslink/inc/IoctlDefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for IpcMemMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for IpcMemMgr
 */
#define IPCMEMMGR_BASE_CMD                 0x130        /* TODO */

/*!
 *  @brief  Command for IpcMemMgr_setup
 */
#define CMD_IPCMEMMGR_SETUP                 _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 1u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_setup
 */
#define CMD_IPCMEMMGR_DESTROY                 _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 2u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_destroy
 */
#define CMD_IPCMEMMGR_GETCONFIG             _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 3u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_delete
 */
#define CMD_IPCMEMMGR_SETLOCALID            _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 4u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_alloc
 */
#define CMD_IPCMEMMGR_ALLOC                 _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 5u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_free
 */
#define CMD_IPCMEMMGR_FREE                  _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 6u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_acquire
 */
#define CMD_IPCMEMMGR_ACQUIRE               _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 7u,\
                                            IpcMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for IpcMemMgr_release
 */
#define CMD_IPCMEMMGR_RELEASE               _IOWR(UTILSCMDBASE,\
                                            IPCMEMMGR_BASE_CMD + 8u,\
                                            IpcMemMgrDrv_CmdArgs)



/*  ----------------------------------------------------------------------------
 *  Command arguments for IpcMemMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for IpcMemMgr
 */
typedef struct IpcMemMgrDrv_CmdArgs {
    union {
        struct {
            IpcMemMgr_Config *config;
        } getConfig;

        struct {
            IpcMemMgr_Config *config;
        } setup;

        struct {
            SizeT   size;
            UInt32  offset;
        } alloc;

        struct {
            UInt32  offset;
            SizeT   size;
        } free;

        struct {
            String  name;
            SizeT   nameSize;
            SizeT   size;
            UInt32  offset;
            Bool    newAlloc;
        } acquire;

        struct {
            String name;
            SizeT  nameSize;
        } release;
    } args;

    Int32 apiStatus;
} IpcMemMgrDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* ti_syslink_inc_IpcMemMgrDrvDefs__include */
