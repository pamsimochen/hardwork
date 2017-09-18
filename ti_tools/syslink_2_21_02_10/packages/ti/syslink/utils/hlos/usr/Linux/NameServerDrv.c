/*
 *  @file   NameServerDrv.c
 *
 *  @brief      OS-specific implementation of NameServer driver for Linux
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
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/Dev.h>

/* Module specific header files */
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/_NameServer.h>
#include <ti/syslink/inc/NameServerDrvDefs.h>

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
 *  @brief  Driver name for NameServer.
 */
#define NAMESERVER_DRVIER_NAME     "/dev/syslinkipc_NameServer"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for NameServer in this process.
 */
static Int32 NameServerDrv_handle = 0;

/*!
 *  @brief  Reference count for the driver handle.
 */
static UInt32 NameServerDrv_refCount = 0;


/** ============================================================================
 *  Functions
 *  ============================================================================
 */

/*!
 *  @brief  Function to open the NameServer driver.
 *
 *  @sa     NameServerDrv_close
 */
Int NameServerDrv_open(Void)
{
    Int status = NameServer_S_SUCCESS;

    GT_0trace(curTrace, GT_ENTER, "NameServerDrv_open");

    /* TBD: Protection for refCount. */
    if (NameServerDrv_refCount == 0) {

        /* open the drive */
        NameServerDrv_handle = Dev_pollOpen(NAMESERVER_DRVIER_NAME,
                O_SYNC | O_RDWR);

        if (NameServerDrv_handle < 0) {
            status = NameServer_E_OSFAILURE;
            GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_open",
                    status, "Failed to open NameServer driver with OS");
        }

        /* set flag to close file descriptor on exec */
        if (status == NameServer_S_SUCCESS) {
            status = fcntl(NameServerDrv_handle, F_SETFD, FD_CLOEXEC);

            if (status != 0) {
                status = NameServer_E_OSFAILURE;
                GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_open",
                        status, "Failed to set file descriptor flags");
            }
        }

        /* set pid on file descriptor for resource tracking */
        if (status == NameServer_S_SUCCESS) {
            status = fcntl(NameServerDrv_handle, F_SETOWN, getpid());

            if (status != 0) {
                status = NameServer_E_OSFAILURE;
                GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_open",
                    status, "Failed to set process id");
            }
        }
    }

    if (status == NameServer_S_SUCCESS) {
        NameServerDrv_refCount++;
    }

    /* failure case */
    if (status < 0) {
        if (NameServerDrv_handle > 0) {
            close(NameServerDrv_handle);
            NameServerDrv_handle = 0;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "NameServerDrv_open", status);

    return(status);
}

/*!
 *  @brief  Function to close the NameServer driver.
 *
 *  @sa     NameServerDrv_open
 */
Int
NameServerDrv_close (Void)
{
    Int status      = NameServer_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "NameServerDrv_close");

    /* TBD: Protection for refCount. */
    NameServerDrv_refCount--;
    if (NameServerDrv_refCount == 0) {
        osStatus = close (NameServerDrv_handle);
        if (osStatus != 0) {
            perror ("NameServer driver close: ");
/*! @retval NameServer_E_OSFAILURE Failed to open NameServer driver with OS */
            status = NameServer_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerDrv_close",
                                 status,
                                 "Failed to close NameServer driver with OS!");
        }
        else {
            NameServerDrv_handle = 0;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "NameServerDrv_close", status);

/*! @retval NameServer_S_SUCCESS Operation successfully completed. */
    return status;
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
NameServerDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = NameServer_S_SUCCESS;
    int osStatus    = 0;

    GT_2trace (curTrace, GT_ENTER, "NameServerDrv_ioctl", cmd, args);

    GT_assert (curTrace, (NameServerDrv_refCount > 0));

    osStatus = ioctl (NameServerDrv_handle, cmd, args);
    if (osStatus < 0) {
    /*! @retval NameServer_E_OSFAILURE Driver ioctl failed */
        status = NameServer_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((NameServerDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "NameServerDrv_ioctl", status);

/*! @retval NameServer_S_SUCCESS Operation successfully completed. */
    return status;
}
