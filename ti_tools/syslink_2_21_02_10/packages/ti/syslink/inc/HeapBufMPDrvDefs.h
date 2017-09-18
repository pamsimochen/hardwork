/** 
 *  @file   HeapBufMPDrvDefs.h
 *
 *  @brief      Definitions of HeapBufMPDrv types and structures.
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



#ifndef HEAPBUFMP_DRVDEFS_H_0xb9a7
#define HEAPBUFMP_DRVDEFS_H_0xb9a7


/* Utilities headers */
#include <ti/ipc/HeapBufMP.h>
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
 *  IOCTL command IDs for HeapBufMP
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for HeapBufMP
 */
#define HEAPBUFMP_BASE_CMD                 (0x150)

/*!
 *  @brief  Command for HeapBufMP_getConfig
 */
#define CMD_HEAPBUFMP_GETCONFIG         _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 1u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_setup
 */
#define CMD_HEAPBUFMP_SETUP             _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 2u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_setup
 */
#define CMD_HEAPBUFMP_DESTROY           _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 3u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_destroy
 */
#define CMD_HEAPBUFMP_PARAMS_INIT         _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 4u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_create
 */
#define CMD_HEAPBUFMP_CREATE            _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 5u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_delete
 */
#define CMD_HEAPBUFMP_DELETE            _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 6u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_open
 */
#define CMD_HEAPBUFMP_OPEN              _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 7u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_close
 */
#define CMD_HEAPBUFMP_CLOSE             _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 8u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_alloc
 */
#define CMD_HEAPBUFMP_ALLOC             _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 9u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_free
 */
#define CMD_HEAPBUFMP_FREE              _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 10u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_sharedMemReq
 */
#define CMD_HEAPBUFMP_SHAREDMEMREQ      _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 11u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_getStats
 */
#define CMD_HEAPBUFMP_GETSTATS          _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 12u,\
                                      HeapBufMPDrv_CmdArgs)
/*!
 *  @brief  Command for HeapBufMP_getExtendedStats
 */
#define CMD_HEAPBUFMP_GETEXTENDEDSTATS  _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 13u,\
                                      HeapBufMPDrv_CmdArgs)

/*!
 *  @brief  Command for HeapBufMP_open
 */
#define CMD_HEAPBUFMP_OPENBYADDR        _IOWR(IPCCMDBASE,\
                                      HEAPBUFMP_BASE_CMD + 14u,\
                                      HeapBufMPDrv_CmdArgs)
/*  ----------------------------------------------------------------------------
 *  Command arguments for HeapBufMP
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for HeapBufMP
 */
typedef struct HeapBufMPDrv_CmdArgs {
    union {
        struct {
            HeapBufMP_Params     * params;
        } ParamsInit;

        struct {
            HeapBufMP_Config     * config;
        } getConfig;

        struct {
            HeapBufMP_Config     * config;
        } setup;

        struct {
            Ptr                   handle;
            HeapBufMP_Params      * params;
            UInt32                nameLen;
            SharedRegion_SRPtr    sharedAddrSrPtr;
#ifdef SYSLINK_BUILDOS_QNX
            SharedRegion_SRPtr  gateAttrs;
#else
            Ptr                   knlGate;
#endif
        } create;

        struct {
            Ptr                   handle;
        } deleteInstance;

        struct {
            Ptr                   handle;
            String                name;
            UInt32                nameLen;
        } open;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    sharedAddrSrPtr;
        } openByAddr;

        struct {
            Ptr                   handle;
        } close;

        struct {
            Ptr                   handle;
            UInt32                size;
            UInt32                align;
            SharedRegion_SRPtr    blockSrPtr;
        } alloc;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    blockSrPtr;
            UInt32                size;
        } free;

        struct {
            Ptr                   handle;
            Memory_Stats       *  stats;
        } getStats;

        struct {
            Ptr                   handle;
            HeapBufMP_ExtendedStats *  stats;
        } getExtendedStats;

        struct {
            Ptr                     handle;
            HeapBufMP_Params      * params;
            SharedRegion_SRPtr      sharedAddrSrPtr;
            UInt32                  bytes;
        } sharedMemReq;

    } args;

    Int32 apiStatus;
} HeapBufMPDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* HEAPBUFMP_DRVDEFS_H_0xb9a7 */
