/*
 *  @file   ClientNotifyMgrDrv.c
 *
 *  @brief      OS-specific implementation of FraemQ driver for Linux
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

/* Osal And Utils  headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/Dev.h>

/* Module specific header files */
#include <ti/syslink/inc/ClientNotifyMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgrDrvDefs.h>

/* Linux specific header files */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver name for ClientNotifyMgr.
 */
#define CLIENTNOTIFYMGR_DRVIER_NAME     "/dev/syslinkipc_ClientNotifyMgr"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for ClientNotifyMgr in this process.
 */
static Int32 ClientNotifyMgrDrv_handle = 0;

/*!
 *  @brief  Reference count for the driver handle.
 */
static UInt32 ClientNotifyMgrDrv_refCount = 0;


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the ClientNotifyMgr driver.
 *
 *  @sa     GatePetersonDrv_close
 */
Int
ClientNotifyMgrDrv_open (Void)
{
    Int status      = ClientNotifyMgr_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_open");

    if (ClientNotifyMgrDrv_refCount == 0) {
        /* TBD: Protection for refCount. */
        ClientNotifyMgrDrv_refCount++;

        ClientNotifyMgrDrv_handle = Dev_pollOpen (CLIENTNOTIFYMGR_DRVIER_NAME,
                              O_SYNC | O_RDWR);
        if (ClientNotifyMgrDrv_handle < 0) {
            perror (CLIENTNOTIFYMGR_DRVIER_NAME);
            /*! @retval ClientNotifyMgr_E_OSFAILURE Failed to open ClientNotifyMgr driver with OS
             */
            status = ClientNotifyMgr_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgrDrv_open",
                                 status,
                                 "Failed to open ClientNotifyMgr driver with OS!");
        }
        else {
            osStatus = fcntl (ClientNotifyMgrDrv_handle, F_SETFD, FD_CLOEXEC);
            if (osStatus != 0) {
            /*! @retval ClientNotifyMgr_E_OSFAILURE Failed to set file
             * descriptor flags
             */
                status = ClientNotifyMgr_E_OSFAILURE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ClientNotifyMgrDrv_open",
                                     status,
                                     "Failed to set file descriptor flags!");
            }
        }
    }
    else {
        ClientNotifyMgrDrv_refCount++;
    }



    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_open", status);

/*! @retval ClientNotifyMgr_SUCCESS Operation successfully completed. */
    return (status);
}


/*!
 *  @brief  Function to close the ClientNotifyMgr driver.
 *
 *  @sa     ClientNotifyMgrDrv_open
 */
Int
ClientNotifyMgrDrv_close (Void)
{
    Int status      = ClientNotifyMgr_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_close");

    /* TBD: Protection for refCount. */
    ClientNotifyMgrDrv_refCount--;
    if (ClientNotifyMgrDrv_refCount == 0) {
        osStatus = close (ClientNotifyMgrDrv_handle);
        if (osStatus != 0) {
            perror ("ClientNotifyMgr driver close: ");
            /*! @retval ClientNotifyMgr_E_OSFAILURE Failed to open ClientNotifyMgr
             * driver with OS
             */
            status = ClientNotifyMgr_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ClientNotifyMgrDrv_close",
                                 status,
                                 "Failed to close ClientNotifyMgr driver with OS!");
        }
        else {
            ClientNotifyMgrDrv_handle = 0;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_close", status);

/*! @retval ClientNotifyMgr_S_SUCCESS Operation successfully completed. */
    return (status);
}


/*!
 *  @brief  Function to invoke the APIs through ioctl.
 *
 *  @param  cmd     Command for driver ioctl
 *  @param  args    Arguments for the ioctl command
 *
 *  @sa
 */
Int
ClientNotifyMgrDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = ClientNotifyMgr_S_SUCCESS;
    int osStatus    = 0;

    GT_2trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_ioctl", cmd, args);

    GT_assert (curTrace, (ClientNotifyMgrDrv_refCount > 0));

    osStatus = ioctl (ClientNotifyMgrDrv_handle, cmd, args);
    if (osStatus < 0) {
    /*! @retval ClientNotifyMgr_E_OSFAILURE Driver ioctl failed */
        status = ClientNotifyMgr_E_OSFAILURE;
        GT_setFailureReason (curTrace, GT_4CLASS, "ClientNotifyMgrDrv_ioctl",
                status, "Driver ioctl failed");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((ClientNotifyMgrDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_ioctl", status);

    return (status);
}
