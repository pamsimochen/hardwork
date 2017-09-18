/*
 *  @file   CacheDrv.c
 *
 *  @brief      User-side OS-specific implementation of CacheDrv driver for Linux
 *
 *
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



/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Trace.h>

/* Module specific header files */
#include <ti/syslink/inc/CacheDrv.h>
#include <ti/syslink/inc/CacheDrvDefs.h>
#include <ti/syslink/inc/usr/Linux/OsalDrv.h>


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to invoke the APIs through ioctl.
 *
 *  @param  cmd     Command for driver ioctl
 *  @param  args    Arguments for the ioctl command
 */
Int CacheDrv_ioctl(UInt32 cmd, Ptr args)
{
    Int status      = 0;
    Int osStatus    = Cache_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "CacheDrv_ioctl", cmd, args);

    osStatus = OsalDrv_ioctl(cmd, args);
    if (osStatus < Cache_S_SUCCESS) {
        GT_setFailureReason(curTrace, GT_4CLASS, "CacheDrv_ioctl",
                OSALDRV_E_OSFAILURE, "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((CacheDrv_CmdArgs *)args)->apiStatus;
    }

    GT_1trace(curTrace, GT_LEAVE, "CacheDrv_ioctl", status);

    return (status);
}
