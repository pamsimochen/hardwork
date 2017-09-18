/** 
 *  @file   ListMPDrvDefs.h
 *
 *  @brief      Definitions of ListMPDrv types and structures.
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



#ifndef LISTMPSHAREDMEMORY_DRVDEFS_H_0x42d8
#define LISTMPSHAREDMEMORY_DRVDEFS_H_0x42d8


/* Utilities headers */
#include <ti/ipc/SharedRegion.h>
#include <ti/ipc/ListMP.h>
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
 *  IOCTL command IDs for ListMP
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for ListMP
 */
#define LISTMPSHAREDMEMORY_BASE_CMD                 0x150

/*!
 *  @brief  Command for ListMP_getConfig
 */
#define CMD_LISTMP_GETCONFIG    _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 1u,\
                                            ListMPDrv_CmdArgs)

/*!
 *  @brief  Command for ListMP_setup
 */
#define CMD_LISTMP_SETUP        _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 2u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_setup
 */
#define CMD_LISTMP_DESTROY      _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 3u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_destroy
 */
#define CMD_LISTMP_PARAMS_INIT  _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 4u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_create
 */
#define CMD_LISTMP_CREATE       _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 5u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_delete
 */
#define CMD_LISTMP_DELETE       _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 6u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_open
 */
#define CMD_LISTMP_OPEN    _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 7u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_close
 */
#define CMD_LISTMP_CLOSE    _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 8u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_isEmpty
 */
#define CMD_LISTMP_ISEMPTY    _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 9u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_getHead
 */
#define CMD_LISTMP_GETHEAD      _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 10u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_getTail
 */
#define CMD_LISTMP_GETTAIL        _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 11u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_putHead
 */
#define CMD_LISTMP_PUTHEAD       _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 12u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_putTail
 */
#define CMD_LISTMP_PUTTAIL      _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 13u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_insert
 */
#define CMD_LISTMP_INSERT       _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 14u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_remove
 */
#define CMD_LISTMP_REMOVE       _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 15u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_next
 */
#define CMD_LISTMP_NEXT         _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 16u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_prev
 */
#define CMD_LISTMP_PREV         _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 17u,\
                                            ListMPDrv_CmdArgs)
/*!
 *  @brief  Command for ListMP_sharedMemReq
 */
#define CMD_LISTMP_SHAREDMEMREQ _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 18u,\
                                            ListMPDrv_CmdArgs)

/*!
 *  @brief  Command for ListMP_openByAddr
 */
#define CMD_LISTMP_OPENBYADDR  _IOWR(IPCCMDBASE,\
                                            LISTMPSHAREDMEMORY_BASE_CMD + 19u,\
                                            ListMPDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  Command arguments for ListMP
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for ListMP
 */
typedef struct ListMPDrv_CmdArgs {
    union {
        struct {
            ListMP_Params *             params;
        } ParamsInit;

        struct {
            ListMP_Config * config;
        } getConfig;

        struct {
            ListMP_Config * config;
        } setup;

        struct {
            Ptr                 handle;
            ListMP_Params *     params;
            UInt32              nameLen;
            SharedRegion_SRPtr  sharedAddrSrPtr;
#ifdef SYSLINK_BUILDOS_QNX
            SharedRegion_SRPtr  gateAttrs;
#else
            Ptr                 knlGate;
#endif
        } create;

        struct {
            Ptr                   handle;
        } deleteInstance;

        struct {
            Ptr                         handle;
            UInt32                      nameLen;
            String                      name;
        } open;

        struct {
            Ptr                         handle;
            SharedRegion_SRPtr          sharedAddrSrPtr;
        } openByAddr;

        struct {
            Ptr    handle;
        } close;

        struct {
            Ptr    handle;
            Bool   isEmpty;
        } isEmpty;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr;
        } getHead;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr;
        } getTail;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr ;
        } putHead;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr ;
        } putTail;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    newElemSrPtr;
            SharedRegion_SRPtr    curElemSrPtr;
        } insert;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr ;
        } remove;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr ;
            SharedRegion_SRPtr    nextElemSrPtr ;
        } next;

        struct {
            Ptr                   handle;
            SharedRegion_SRPtr    elemSrPtr ;
            SharedRegion_SRPtr    prevElemSrPtr ;
        } prev;

        struct {
            ListMP_Params *     params;
            UInt32              bytes;
#ifndef SYSLINK_BUILDOS_QNX
            Ptr                 knlGate;
#endif
            SharedRegion_SRPtr  sharedAddrSrPtr;
            UInt32              nameLen;
            Ptr                 handle;
        } sharedMemReq;

    } args;

    Int32 apiStatus;
} ListMPDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* LISTMPSHAREDMEMORY_DRVDEFS_H_0x42d8 */
