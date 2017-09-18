/** 
 *  @file   FrameQBufMgrDrvDefs.h
 *
 *  @brief      Definitions of FrameQBufMgrDrv types and structures.
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



#ifndef FRAMEQBUFMGRDRVDEFS_H_0x684e
#define FRAMEQBUFMGRDRVDEFS_H_0x684e


/* Utilities headers */
#include <ti/syslink/FrameQBufMgr.h>
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
 *  IOCTL command IDs for FrameQBufMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for FrameQBufMgr
 */
#define FRAMEQBUFMGR_BASE_CMD                  (0x150)

/*!
 *  @brief  Command for FrameQBufMgr_getConfig
 */
#define CMD_FRAMEQBUFMGR_GETCONFIG         _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 1u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_setup
 */
#define CMD_FRAMEQBUFMGR_SETUP             _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 2u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_setup
 */
#define CMD_FRAMEQBUFMGR_DESTROY           _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 3u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_Params_init
 */
#define CMD_FRAMEQBUFMGR_PARAMS_INIT       _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 4u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_create
 */
#define CMD_FRAMEQBUFMGR_CREATE            _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 5u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_delete
 */
#define CMD_FRAMEQBUFMGR_DELETE            _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 6u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_open
 */
#define CMD_FRAMEQBUFMGR_OPEN              _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 7u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_close
 */
#define CMD_FRAMEQBUFMGR_CLOSE             _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 8u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_sharedMemModReq
 */
#define CMD_FRAMEQBUFMGR_SHAREDMEMMODREQ   _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 9u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_sharedMemReq
 */
#define CMD_FRAMEQBUFMGR_SHAREDMEMREQ      _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 10u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_getHandle
 */
#define CMD_FRAMEQBUFMGR_GETHANDLE         _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 11u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_getId
 */
#define CMD_FRAMEQBUFMGR_GETID             _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 12u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_alloc
 */
#define CMD_FRAMEQBUFMGR_ALLOC             _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 13u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_allocv
 */
#define CMD_FRAMEQBUFMGR_ALLOCV            _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 14u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_free
 */
#define CMD_FRAMEQBUFMGR_FREE              _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 15u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_free
 */
#define CMD_FRAMEQBUFMGR_FREEV             _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 16u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_add
 */
#define CMD_FRAMEQBUFMGR_ADD               _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 17u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_remove
 */
#define CMD_FRAMEQBUFMGR_REMOVE            _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 18u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_registerNotifier
 */
#define CMD_FRAMEQBUFMGR_REG_NOTIFIER         _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 19u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_unregisterNotifier
 */
#define CMD_FRAMEQBUFMGR_UNREG_NOTIFIER         _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 20u,\
                                           FrameQBufMgrDrv_CmdArgs)

/*!
 *  @brief  Command for FrameQBufMgr_dup
 */
#define CMD_FRAMEQBUFMGR_DUP               _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 21u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQBufMgr_dupv
 */
#define CMD_FRAMEQBUFMGR_DUPV              _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 22u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command to set the notify id in the instance's kernel object.
 *
 */
#define CMD_FRAMEQBUFMGR_SET_NOTIFYID         _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 23u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command to reset the notify id in the instance's kernel object
 */
#define CMD_FRAMEQBUFMGR_RESET_NOTIFYID    _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 24u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command to transalte the given address to destination address type.
 */
#define CMD_FRAMEQBUFMGR_TRANSLATE         _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 25u,\
                                           FrameQBufMgrDrv_CmdArgs)

/*!
 *  @brief  Command to perform custom operation on the instance.
 */
#define CMD_FRAMEQBUFMGR_CONTROL           _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 26u,\
                                           FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command to get the default params for ShMem .
 */
#define CMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT _IOWR(IPCCMDBASE,\
                                           FRAMEQBUFMGR_BASE_CMD + 27u,\
                                           FrameQBufMgrDrv_CmdArgs)

/*!
 *  @brief  Command for FrameQ_getNumFreeFrames
 */
#define CMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES            _IOWR(IPCCMDBASE,\
                                                 FRAMEQBUFMGR_BASE_CMD + 28u,\
                                                 FrameQBufMgrDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_getVNumFreeFrames
 */
#define CMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES           _IOWR(IPCCMDBASE,\
                                                 FRAMEQBUFMGR_BASE_CMD + 29u,\
                                                 FrameQBufMgrDrv_CmdArgs)
/*  ----------------------------------------------------------------------------
 *  Command arguments for FrameQBufMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for FrameQBufMgr
 */
typedef struct FrameQBufMgrDrv_CmdArgs {
    union {
        struct {
            FrameQBufMgr_Config   * config;
        } getConfig;

        struct {
            FrameQBufMgr_Config   * config;
        } setup;

        struct {
            Ptr                     params;
            UInt32                  bytes;
            UInt32                  bufSize;
        } sharedMemReq;

        struct {
            Ptr                     handle;
            Ptr                     params;
        } ParamsInit;

        struct {
            FrameQBufMgr_Handle     handle;
            UInt32                  interfaceType;
            Ptr                     params;
            String                  name;
            UInt32                  nameLen;
            SharedRegion_SRPtr      cliNotifyMgrSharedMem;
            SharedRegion_SRPtr      cliGateSharedMem;
            UInt32                  instId;
        } create;

        struct {
            FrameQBufMgr_Handle     handle;
        } deleteInstance;

        struct {
            FrameQBufMgr_Handle     handle;
            UInt32                  interfaceType;
            Ptr                     openParams;
            UInt32                  nameLen;
            SharedRegion_SRPtr      cliNotifyMgrSharedMem;
            SharedRegion_SRPtr      cliGateSharedMem;
            UInt32                  instId;
        } open;

        struct {
            FrameQBufMgr_Handle     handle;
        } close;

        struct {
            FrameQBufMgr_Handle     handle;
            UInt32                  instId;
        } getHandle;

        struct {
            FrameQBufMgr_Handle     handle;
            UInt32                  instId;
        } getId;

        struct {
            FrameQBufMgr_Handle     handle;
            Ptr                     frame;
        } alloc;

        struct {
            FrameQBufMgr_Handle handle;
            UInt32              *aframePtr;
            UInt32               numFrames;
            UInt32               *afreeQId;
        } allocv;

        struct {
            FrameQBufMgr_Handle handle;
            FrameQBufMgr_Frame  frame;
        } free;
        struct {
            FrameQBufMgr_Handle handle;
            UInt32              *aframePtr;
            UInt32              numFrames;
        } freev;

        struct {
            FrameQBufMgr_Handle     handle;
            Ptr                     buf;
            UInt32                  size;
        } add;

        struct {
            FrameQBufMgr_Handle     handle;
            FrameQBufMgr_NotifyParams notifyParams;
        } regNotifier;

        struct {
            FrameQBufMgr_Handle     handle;
        } unregNotifier;

        struct {
            Ptr                   handle;
            UInt16                notifyId;
        } setNotifyId;

        struct {
            Ptr                   handle;
            UInt16                notifyId;
        } resetNotifyId;

        struct {
            FrameQBufMgr_Handle     handle;
            Ptr                     frame;
            UInt32                  numDupedFrames;
            FrameQBufMgr_Frame      *adupedFramePtr;
        } dup;

        struct {
            FrameQBufMgr_Handle     handle;
            FrameQBufMgr_Frame      **dupedFramePtr;
            UInt32                  numDupedFrames;
            UInt32                  numFrames;
            FrameQBufMgr_Frame      *framePtr;
        } dupv;

        struct {
            FrameQBufMgr_Handle     handle;
            Ptr                     dstAddr;
            FrameQBufMgr_AddrType   dstAddrType;
            Ptr                     srcAddr;
            FrameQBufMgr_AddrType   srcAddrType;
            FrameQBufMgr_BufType    bufType;
        } translate;

        struct {
            Ptr                   handle;
            UInt32                cmd;
            Ptr                   arg;
            UInt32                size;
        } control;
        struct {
            Ptr                   handle;
            UInt32                numFreeFrames;
        } getNumFreeFrames;

        struct {
            Ptr                   handle;
            UInt32              * numFreeFrames;
            UInt32              * freeQId;
            UInt32                numFreeQids;
        } getvNumFreeFrames;

    } args;

    Int32 apiStatus;
} FrameQBufMgrDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* FRAMEQBUFMGRDRVDEFS_H_0x684e */
