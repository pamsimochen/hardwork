/** 
 *  @file   SysMemMgrDrvDefs.h
 *
 *  @brief      Definitions of SysMemMgrDrv types and structures.
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



#ifndef SYSMEMMGR_DRVDEFS_H_0xF414
#define SYSMEMMGR_DRVDEFS_H_0xF414


/* Utilities headers */
#include <SysMemMgr.h>
#ifdef SYSLINK_BUILDOS_LINUX
#include <linux/ioctl.h>
#elif SYSLINK_BUILDOS_QNX
#include <sys/ioctl.h>
#endif

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*  ----------------------------------------------------------------------------
 *  IOCTL command IDs for SysMemMgr
 *  ----------------------------------------------------------------------------
 */

/*!
 *  @brief  Base command ID for SysMemMgr
 */
#define SYSMEMMGRCMDBASE                   0xF6
/*!
 *  @brief  Base command ID for SysMemMgr
 */
#define SYSMEMMGR_BASE_CMD                 150

/*!
 *  @brief  Command for SysMemMgr_getConfig
 */
#define CMD_SYSMEMMGR_GETCONFIG            _IOWR(SYSMEMMGRCMDBASE,\
                                            SYSMEMMGR_BASE_CMD + 1u,\
                                            SysMemMgrDrv_CmdArgs)

/*!
 *  @brief  Command for SysMemMgr_setup
 */
#define CMD_SYSMEMMGR_SETUP                 _IOWR(SYSMEMMGRCMDBASE,\
                                            SYSMEMMGR_BASE_CMD + 2u,\
                                            SysMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for SysMemMgr_destroy
 */
#define CMD_SYSMEMMGR_DESTROY               _IOWR(SYSMEMMGRCMDBASE,\
                                            SYSMEMMGR_BASE_CMD + 3u,\
                                            SysMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for SysMemMgr_alloc
 */
#define CMD_SYSMEMMGR_ALLOC                 _IOWR(SYSMEMMGRCMDBASE,\
                                            SYSMEMMGR_BASE_CMD + 4u,\
                                            SysMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for SysMemMgr_free
 */
#define CMD_SYSMEMMGR_FREE                   _IOWR(SYSMEMMGRCMDBASE,\
                                            SYSMEMMGR_BASE_CMD + 5u,\
                                            SysMemMgrDrv_CmdArgs)
/*!
 *  @brief  Command for SysMemMgr_translate
 */
#define CMD_SYSMEMMGR_TRANSLATE             _IOWR(SYSMEMMGRCMDBASE,\
                                            SYSMEMMGR_BASE_CMD + 6u,\
                                            SysMemMgrDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  Command arguments for SysMemMgr
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for SysMemMgr
 */
typedef struct SysMemMgrDrv_CmdArgs {
    union {
        struct {
            SysMemMgr_Config * config;
        } getConfig;

        struct {
            SysMemMgr_Config * config;
        } setup;

        struct {
            UInt32              size;
            Ptr                 buf;
            Ptr                 phys;
            Ptr                 kbuf;
            SysMemMgr_AllocFlag flags;
        } alloc;

        struct {
            UInt32              size;
            Ptr                 buf;
            Ptr                 phys;
            Ptr                 kbuf;
            SysMemMgr_AllocFlag flags;
        } free;

        struct {
            Ptr                 buf;
            Ptr                 retPtr;
            SysMemMgr_XltFlag flags;
        } translate;

    } args;

    Int32 apiStatus;
} SysMemMgrDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* SYSMEMMGR_DRVDEFS_H_0xF414 */
