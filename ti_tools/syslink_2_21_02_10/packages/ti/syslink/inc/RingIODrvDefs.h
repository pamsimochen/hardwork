/** 
 *  @file   RingIODrvDefs.h
 *
 *  @brief      Definitions of RingIO types and structures.
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



#ifndef RINGIO_DRVDEFS_H_0xf653
#define RINGIO_DRVDEFS_H_0xf653


/* Osal and utils headers */

/* Module headers */
#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/_RingIOShm.h>
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
 *  IOCTL command IDs for RingIO
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for RingIO
 */
#define RINGIO_BASE_CMD                 0x150

/*!
 *  @brief  Command for RingIO_getConfig
 */
#define CMD_RINGIO_GETCONFIG                     _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 1u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_setup
 */
#define CMD_RINGIO_SETUP                         _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 2u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_setup
 */
#define CMD_RINGIO_DESTROY                       _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 3u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_destroy
 */
#define CMD_RINGIO_PARAMS_INIT                   _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 4u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_create
 */
#define CMD_RINGIO_CREATE                        _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 5u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_delete
 */
#define CMD_RINGIO_DELETE                         _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 6u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_open
 */
#define CMD_RINGIO_OPEN                          _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 7u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_close
 */
#define CMD_RINGIO_CLOSE                         _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 8u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_registerNotifier
 */
#define CMD_RINGIO_REGISTERNOTIFIER              _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 11u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_unregisterNotifier
 */
#define CMD_RINGIO_UNREGISTERNOTIFIER            _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 12u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getValidSize
 */
#define CMD_RINGIO_GETVALIDSIZE                  _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 13u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getEmptySize
 */
#define CMD_RINGIO_GETEMPTYSIZE                  _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 14u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getValidAttrSize
 */
#define CMD_RINGIO_GETVALIDATTRSIZE              _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 15u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getEmptyAttrSize
 */
#define CMD_RINGIO_GETEMPTYATTRSIZE              _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 16u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getAcquiredOffset
 */
#define CMD_RINGIO_GETACQUIREDOFFSET             _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 17u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getAcquiredSize
 */
#define CMD_RINGIO_GETACQUIREDSIZE               _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 18u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getWaterMark
 */
#define CMD_RINGIO_GETWATERMARK                  _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 19u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_acquire
 */
#define CMD_RINGIO_ACQUIRE                       _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 20u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_release
 */
#define CMD_RINGIO_RELEASE                       _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 21u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_cancel
 */
#define CMD_RINGIO_CANCEL                        _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 22u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_getvAttribute
 */
#define CMD_RINGIO_GETVATTRIBUTE                 _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 23u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_setvAttribute
 */
#define CMD_RINGIO_SETVATTRIBUTE                 _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 24u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_sendNotify
 */
#define CMD_RINGIO_SYNC                          _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 25u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_flush
 */
#define CMD_RINGIO_FLUSH                         _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 26u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_sharedMemReq
 */
#define CMD_RINGIO_SHAREDMEMREQ                  _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 27u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command to set the notify id in the instance's kernel object
 */

#define CMD_RINGIO_SET_NOTIFYID                  _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 28u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command to reset the notify id in the instance's kernel object
 */
#define CMD_RINGIO_RESET_NOTIFYID                _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 29u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_setNotifyType
 */
#define CMD_RINGIO_SETNOTIFYTYPE                 _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 30u,\
                                                 RingIODrv_CmdArgs)
/*!
 *  @brief  Command for RingIO_setWaterMark
 */
#define CMD_RINGIO_SETWATERMARK                  _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 31u,\
                                                 RingIODrv_CmdArgs)

/*!
 *  @brief  Command for RingIO_open
 */
#define CMD_RINGIO_OPENBYADDR                          _IOWR(IPCCMDBASE,\
                                                 RINGIO_BASE_CMD + 32u,\
                                                 RingIODrv_CmdArgs)
/*  ----------------------------------------------------------------------------
 *  Command arguments for RingIO
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for RingIO
 */
typedef struct RingIODrv_CmdArgs {
    union {
        struct {
            RingIO_Params     * params;
        } ParamsInit;

        struct {
            RingIO_Config     * config;
        } getConfig;

        struct {
            RingIO_Config     * config;
        } setup;

        struct {
            Ptr                 params;
            UInt32              nameLen;
            Ptr                 handle;
            Ptr                 knlLockHandle;
            SharedRegion_SRPtr  cliNotifyMgrSharedMem;
            SharedRegion_SRPtr  cliGateSharedMem;
            SharedRegion_SRPtr  ctrlSharedAddrSrPtr;
            SharedRegion_SRPtr  dataSharedAddrSrPtr;
            SharedRegion_SRPtr  attrSharedAddrSrPtr;
        } create;

        struct {
            Ptr                 handle;
        } deleteRingIO;

        struct {
            Ptr                 handle;
            RingIO_openParams * params;
            UInt32              nameLen;
            String              name;
            UInt16            * procIds;
            UInt16              remoteProcId;
            SharedRegion_SRPtr  cliNotifyMgrSharedMem;
            SharedRegion_SRPtr  cliGateSharedMem;
        } open;

        struct {
            Ptr                 handle;
            RingIO_openParams * params;
            SharedRegion_SRPtr  cliNotifyMgrSharedMem;
            SharedRegion_SRPtr  cliGateSharedMem;
            SharedRegion_SRPtr  ctrlSharedAddrSrPtr;
        } openByAddr;


        struct {
            Ptr                   handle;
            UInt16                notifyId;
            UInt32                notifyType;
            UInt32                watermark;
        } setNotifyId;

        struct {
            Ptr                   handle;
            UInt32                notifyType;
        } setNotifyType;

        struct {
            Ptr                   handle;
            UInt16                notifyId;
        } resetNotifyId;

        struct {
            Ptr                 handle;
        } close;


        struct {
            Ptr               handle;
            RingIO_NotifyType notifyType;
            UInt32            watermark;
            RingIO_NotifyFxn  notifyFunc;
            Ptr               cbContext;
        } registerNotifier;

        struct {
            Ptr               handle;
        } unregisterNotifier;

        struct {
            Ptr               handle;
            UInt32            size;
        } getValidSize;

        struct {
            Ptr               handle;
            UInt32            size;
        } getEmptySize;

        struct {
            Ptr               handle;
            UInt32            attrSize;
        } getValidAttrSize;

        struct {
            Ptr               handle;
            UInt32            attrSize;
        } getEmptyAttrSize;

        struct {
            Ptr               handle;
            UInt32            offset;
        } getAcquiredOffset;

        struct {
            Ptr               handle;
            UInt32            size;
        } getAcquiredSize;

        struct {
            Ptr               handle;
            UInt32            waterMark;
        } getWaterMark;

        struct {
            Ptr               handle;
            UInt32            waterMark;
        } setWaterMark;

        struct {
            Ptr               handle;
            Ptr             * pData;
            UInt32          * pSize;
        } acquire;

        struct {
            Ptr               handle;
            UInt32            size;
        } release;

        struct {
            Ptr               handle;
        } cancel;

        struct {
            Ptr               handle;
            UInt16 *            type;
            UInt32 *            param;
            RingIO_BufPtr       vptr;
            UInt32 *            pSize;
        } getvAttribute;

        struct {
            Ptr                 handle;
            UInt16              type;
            UInt32              param;
            RingIO_BufPtr       pData;
            UInt32              size;
            Bool                sendNotification;
        } setvAttribute;

        struct {
            Ptr                 handle;
            Bool                hardFlush;
            UInt16 *            type;
            UInt32 *            param;
            UInt32 *            bytesFlushed;
        } flush;

        struct {
            Ptr                 handle;
            RingIO_NotifyMsg    msg;
        } notify;

        struct {
            Ptr             params;
            UInt32          bytes;
            UInt32          ctrlSharedMemReq;
            UInt32          dataSharedMemReq;
            UInt32          attrSharedMemReq;
        } sharedMemReq;

    } args;

    Int32 apiStatus;
} RingIODrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* RINGIO_DRVDEFS_H_0xf653 */
