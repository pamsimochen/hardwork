/** 
 *  @file   OsalDrvDefs.h
 *
 *  @brief      Definitions of CacheDrv types and structures.
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



#ifndef __OSAL_DRV_DEFS_H__
#define __OSAL_DRV_DEFS_H__


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
 *  IOCTL command IDs for Osal
 *  ----------------------------------------------------------------------------
 */
/*!
 *    @brief    Base command ID for Osal
 */
#define OSAL_BASE_CMD                 0x150

/*!
 *    @brief    Command for OsalDrv_map()
 */
#define CMD_OSALDRV_CACHEMMAP         _IOWR(UTILSCMDBASE,\
                                            OSAL_BASE_CMD + 1u,\
                                            OsalDrv_CmdArgs)

/*  ----------------------------------------------------------------------------
 *  Command arguments for Osal
 *  ----------------------------------------------------------------------------
 */
/*!
 *  @brief  Command arguments for Osal
 */
/*
 * This should really be a union, but in order to match up with other
 * *Drv_CmdArgs for common handling by OsalDrv_ioctl(), it's a struct with
 * the first field being the apiStatus for the modules that call
 * OsalDrvDrv_ioctl (by way of OsalDrv_ioctl()).
 */
typedef struct OsalDrv_CmdArgs {
    Int32 apiStatus;

    ULong physAddr;
    /*!< Physical Address. */
} OsalDrv_CmdArgs;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* __OSAL_DRV_DEFS_H__ */
