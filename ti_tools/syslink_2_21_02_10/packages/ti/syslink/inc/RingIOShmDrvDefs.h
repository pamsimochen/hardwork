/** 
 *  @file   RingIOShmDrvDefs.h
 *
 *  @brief      Definitions of RingIOShm types and structures.
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



#ifndef RINGIOSHM_DRVDEFS_H_0xf653
#define RINGIOSHM_DRVDEFS_H_0xf653


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
 *  IOCTL command IDs for RingIOShm
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Base command ID for RingIOShm
 */
#define RINGIOSHM_BASE_CMD                 0x150

/*!
 *  @brief  Command for RingIO_getConfig
 */
#define CMD_RINGIOSHM_GETCONFIG                     _IOWR(IPCCMDBASE,\
                                                 RINGIOSHM_BASE_CMD + 1u,\
                                                 RingIOShmDrv_CmdArgs)
/*!
 *  @brief  Command for RingIOShm_setup
 */
#define CMD_RINGIOSHM_SETUP                         _IOWR(IPCCMDBASE,\
                                                 RINGIOSHM_BASE_CMD + 2u,\
                                                 RingIOShmDrv_CmdArgs)
/*!
 *  @brief  Command for RingIOShm_setup
 */
#define CMD_RINGIOSHM_DESTROY                       _IOWR(IPCCMDBASE,\
                                                 RINGIOSHM_BASE_CMD + 3u,\
                                                 RingIOShmDrv_CmdArgs)
/*!
 *  @brief  Command for RingIOShm_getConfig
 */
#define CMD_RINGIOSHM_PARAMS_INIT                _IOWR(IPCCMDBASE,\
                                                 RINGIOSHM_BASE_CMD + 4u,\
                                                 RingIOShmDrv_CmdArgs)


/*  ----------------------------------------------------------------------------
 *  Command arguments for RingIOShm
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for RingIOShm
 */
typedef struct RingIOShmDrv_CmdArgs {
    union {
        struct {
            Ptr     * params;
        } ParamsInit;

        struct {
            RingIOShm_Config     * config;
        } getConfig;

        struct {
            RingIOShm_Config     * config;
        } setup;

    } args;

    Int32 apiStatus;
} RingIOShmDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* RINGIOSHM_DRVDEFS_H_0xf653 */
