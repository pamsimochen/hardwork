/** 
 *  @file   SharedRegionDrvDefs.h
 *
 *  @brief      Definitions of SharedRegionDrv types and structures.
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



#ifndef SHAREDREGION_DRVDEFS_H_0xf2ba
#define SHAREDREGION_DRVDEFS_H_0xf2ba


/* Utilities headers */
#include <ti/ipc/SharedRegion.h>
#include "_SharedRegion.h"
#include <ti/syslink/utils/IHeap.h>
#include "IpcCmdBase.h"
#include <ti/syslink/inc/IoctlDefs.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for SharedRegion
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for SharedRegion
 */
#define SHAREDREGION_BASE_CMD                 150

/*!
 *  @brief  Command for SharedRegion_getConfig
 */
#define CMD_SHAREDREGION_GETCONFIG                 _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 1u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *  @brief  Command for SharedRegion_setup
 */
#define CMD_SHAREDREGION_SETUP                    _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 2u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *  @brief  Command for SharedRegion_destroy
 */
#define CMD_SHAREDREGION_DESTROY                  _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 3u,\
                                                  SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_start
 */
#define CMD_SHAREDREGION_START                    _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 4u,\
                                                  SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_stop
 */
#define CMD_SHAREDREGION_STOP                     _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 5u,\
                                                  SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_attach
 */
#define CMD_SHAREDREGION_ATTACH                   _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 6u,\
                                                  SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_detach
 */
#define CMD_SHAREDREGION_DETACH                   _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 7u,\
                                                  SharedRegionDrv_CmdArgs)

/*!
 *  @brief  Command for SharedRegion_getHeap
 */
#define CMD_SHAREDREGION_GETHEAP                  _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 8u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *  @brief  Command for SharedRegion_clearEntry
 */
#define CMD_SHAREDREGION_CLEARENTRY          _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 9u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *
 *  @brief  Command for SharedRegion_setEntry
 */
#define CMD_SHAREDREGION_SETENTRY             _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 10u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *
 *  @brief  Command for SharedRegion_reserveMemory
 */
#define CMD_SHAREDREGION_RESERVEMEMORY            _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 11u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *
 *  @brief  Command for SharedRegion_clearReservedMemory
 */
#define CMD_SHAREDREGION_CLEARRESERVEDMEMORY      _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 12u,\
                                                  SharedRegionDrv_CmdArgs)
/*!
 *
 *  @brief  Command for To get the shared region info which is set in kernel
 *          Space.
 */
#define CMD_SHAREDREGION_GETREGIONINFO         _IOWR((IPCCMDBASE),\
                                                  SHAREDREGION_BASE_CMD + 13u,\
                                                  SharedRegionDrv_CmdArgs)
/*  ----------------------------------------------------------------------------
 *  Command arguments for SharedRegion
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for SharedRegion
 */
typedef struct SharedRegionDrv_CmdArgs_tag {
    union {
        struct {
            SharedRegion_Config * config;
        } getConfig;

        struct {
            SharedRegion_Region  * regions;
            SharedRegion_Config * config;
        } setup;

        struct {
            SharedRegion_Region  * regions;
        } getRegionInfo;

        struct {
            UInt16                remoteProcId;
        } attach;

        struct {
            UInt16                remoteProcId;
        } detach;

        struct {
            UInt16                id;
            IHeap_Handle          heapHandle;
        } getHeap;

        struct {
           UInt16                id;
           SharedRegion_Entry    entry;
        } setEntry;

        struct {
           UInt16                id;
        } clearEntry;

        struct {
           UInt16                id;
           UInt32                size;
        } reserveMemory;
    } args;

    Int32 apiStatus;

}SharedRegionDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* SharedRegion_DrvDefs_H_0xf2ba */
