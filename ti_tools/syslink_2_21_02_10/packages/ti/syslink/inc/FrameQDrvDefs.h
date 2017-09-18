/** 
 *  @file   FrameQDrvDefs.h
 *
 *  @brief      Definitions of FrameQDrv types and structures.
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



#ifndef FRAMEQDRVDEFS_H_0x6e6f
#define FRAMEQDRVDEFS_H_0x6e6f


/* Utilities headers */
#include <ti/syslink/FrameQ.h>
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
 *  IOCTL command IDs for FrameQ
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for FrameQ
 */
#define FRAMEQ_BASE_CMD                        (0x150)

/*!
 *  @brief  Command for FrameQ_getConfig
 */
#define CMD_FRAMEQ_GETCONFIG                     _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 1u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_setup
 */
#define CMD_FRAMEQ_SETUP                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 2u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_setup
 */
#define CMD_FRAMEQ_DESTROY                        _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 3u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_destroy
 */
#define CMD_FRAMEQ_PARAMS_INIT                   _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 4u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_create
 */
#define CMD_FRAMEQ_CREATE                        _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 5u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_delete
 */
#define CMD_FRAMEQ_DELETE                        _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 6u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_open
 */
#define CMD_FRAMEQ_OPEN                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 7u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_close
 */
#define CMD_FRAMEQ_CLOSE                         _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 8u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_alloc
 */
#define CMD_FRAMEQ_ALLOC                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 9u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_allocv
 */
#define CMD_FRAMEQ_ALLOCV                        _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 10u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_free
 */
#define CMD_FRAMEQ_FREE                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 11u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_freev
 */
#define CMD_FRAMEQ_FREEV                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 12u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_dup
 */
#define CMD_FRAMEQ_DUP                           _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 13u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_put
 */
#define CMD_FRAMEQ_PUT                           _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 14u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_putv
 */
#define CMD_FRAMEQ_PUTV                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 15u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_get
 */
#define CMD_FRAMEQ_GET                           _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 16u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_getv
 */
#define CMD_FRAMEQ_GETV                          _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 17u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_registerNotifier
 */
#define CMD_FRAMEQ_REG_NOTIFIER                  _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 18u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_unregisterNotifier
 */
#define CMD_FRAMEQ_UNREG_NOTIFIER                 _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 19u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_getNumFrames
 */
#define CMD_FRAMEQ_GET_NUMFRAMES                 _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 20u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_getNumFrames
 */
#define CMD_FRAMEQ_GET_VNUMFRAMES                 _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 21u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_getNumFrames
 */
#define CMD_FRAMEQ_CONTROL                       _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 22u,\
                                                 FrameQDrv_CmdArgs)

/*!
 *  @brief  Command to set the notify id in the instance's kernel object
 */

#define CMD_FRAMEQ_SET_NOTIFYID                  _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 23u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command to reset the notify id in the instance's kernel object
 */
#define CMD_FRAMEQ_RESET_NOTIFYID                 _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 24u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command to get the default instance params for ShMem instance.
 */
#define CMD_FRAMEQ_SHMEM_PARAMS_INIT             _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 25u,\
                                                 FrameQDrv_CmdArgs)

/*!
 *  @brief  Command to get the shared memory required for ShMem instance.
 */
#define    CMD_FRAMEQ_SHMEM_MEMREQ              _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 26u,\
                                                 FrameQDrv_CmdArgs)

/*!
 *  @brief  Command for FrameQ_getNumFrames
 */
#define CMD_FRAMEQ_GET_NUMFREEFRAMES            _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 27u,\
                                                 FrameQDrv_CmdArgs)
/*!
 *  @brief  Command for FrameQ_getNumFrames
 */
#define CMD_FRAMEQ_GET_VNUMFREEFRAMES           _IOWR(IPCCMDBASE,\
                                                 FRAMEQ_BASE_CMD + 28u,\
                                                 FrameQDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  Command arguments for FrameQ
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for FrameQ
 */
typedef struct GatePetersonDrv_CmdArgs {
    union {

        struct {
            FrameQ_Config       * config;
        } getConfig;

        struct {
            FrameQ_Config       * config;
        } setup;

        struct {
            Ptr                   handle;
            Ptr                   params;
        } ParamsInit;

        struct {
            Ptr                     handle;
            UInt32                  interfaceType;
            Ptr                     params;
            String                  name;
            UInt32                  nameLen;
            SharedRegion_SRPtr      cliNotifyMgrSharedMem;
            SharedRegion_SRPtr      cliGateSharedMem;
            UInt32                  fQBMnameLen;
            String                  fQBMName;
        } create;

        struct {
            Ptr                   handle;
        } deleteInstance;

        struct {
            Ptr                   handle;
            UInt32                interfaceType;
            Ptr                   openParams;
            UInt32                nameLen;
            SharedRegion_SRPtr    cliNotifyMgrSharedMem;
            SharedRegion_SRPtr    cliGateSharedMem;
            SharedRegion_SRPtr    fQBufMgrSharedAddr;
            String                fQBufMgrName;
        } open;

        struct {
            Ptr                   handle;
        } close;

        struct {
            Ptr                   handle;
            FrameQ_Frame          frame;
        } alloc;

        struct {
            Ptr                   handle;
            FrameQ_Frame          frame;
        } put;

        struct {
            Ptr                 handle;
            FrameQ_Frame        *aframePtr;
            UInt32              *filledQueueId;
            UInt32              numFrames;
        } putv;

        struct {
            Ptr                 handle;
            FrameQ_Frame        frame;
        } get;

        struct {
            Ptr                 handle;
            FrameQ_Frame        *aframePtr;
            UInt32              *filledQueueId;
            UInt8               numFrames;
        } getv;

        struct {
            Ptr                   handle;
            FrameQ_NotifyParams   notifyParams;
        } regNotifier;

        struct {
            Ptr                   handle;
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
            Ptr                   handle;
            UInt32                numFrames;
        } getNumFrames;

        struct {
            Ptr                   handle;
            UInt32              * numFrames;
            UInt32              * filledQId;
            UInt32                numFilledQids;
        } getvNumFrames;

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

        struct {
            Ptr                   handle;
            Ptr                   params;
            UInt32                bytes;
        } sharedMemReq;

        struct {
            Ptr                   handle;
            UInt32                cmd;
            Ptr                   arg;
            UInt32                size;
        } control;

    } args;

    Int32 apiStatus;
} FrameQDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* FRAMEQDRVDEFS_H_0x6e6f */
