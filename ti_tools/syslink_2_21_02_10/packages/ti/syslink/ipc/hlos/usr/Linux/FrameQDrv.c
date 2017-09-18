/*
 *  @file   FrameQDrv.c
 *
 *  @brief      OS-specific implementation of FrameQ driver for Linux
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
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/inc/FrameQDrvDefs.h>

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
 *  @brief  Driver name for FrameQ.
 */
#define FRAMEQ_DRIVER_NAME     "/dev/syslinkipc_FrameQ"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for FrameQ in this process.
 */
static Int32 FrameQDrv_handle = 0;

/*!
 *  @brief  Reference count for the driver handle.
 */
static UInt32 FrameQDrv_refCount = 0;


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the FrameQ driver.
 *
 *  @sa     GatePetersonDrv_close
 */
Int
FrameQDrv_open (Void)
{
    Int status      = FrameQ_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "FrameQDrv_open");

    if (FrameQDrv_refCount == 0) {
        /* TBD: Protection for refCount. */
        FrameQDrv_refCount++;

        FrameQDrv_handle = Dev_pollOpen (FRAMEQ_DRIVER_NAME,
                              O_SYNC | O_RDWR);
        if (FrameQDrv_handle < 0) {
            perror (FRAMEQ_DRIVER_NAME);
            /*! @retval FrameQ_E_OSFAILURE Failed to open FrameQ driver with OS
             */
            status = FrameQ_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQDrv_open",
                                 status,
                                 "Failed to open FrameQ driver with OS!");
        }
        else {
            osStatus = fcntl (FrameQDrv_handle, F_SETFD, FD_CLOEXEC);
            if (osStatus != 0) {
            /*! @retval GATEPETERSON_E_OSFAILURE Failed to set file
             * descriptor flags
             */
                status = FrameQ_E_OSFAILURE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQDrv_open",
                                     status,
                                     "Failed to set file descriptor flags!");
            }
        }
    }
    else {
        FrameQDrv_refCount++;
    }



    GT_1trace (curTrace, GT_LEAVE, "FrameQDrv_open", status);

/*! @retval FrameQ_S_SUCCESS Operation successfully completed. */
    return (status);
}


/*!
 *  @brief  Function to close the FrameQ driver.
 *
 *  @sa     FrameQDrv_open
 */
Int
FrameQDrv_close (Void)
{
    Int status      = FrameQ_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "FrameQDrv_close");

    /* TBD: Protection for refCount. */
    FrameQDrv_refCount--;
    if (FrameQDrv_refCount == 0) {
        osStatus = close (FrameQDrv_handle);
        if (osStatus != 0) {
            perror ("FrameQ driver close: ");
            /*! @retval FrameQ_E_OSFAILURE Failed to open FrameQ
             * driver with OS
             */
            status = FrameQ_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQDrv_close",
                                 status,
                                 "Failed to close FrameQ driver with OS!");
        }
        else {
            FrameQDrv_handle = 0;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQDrv_close", status);

/*! @retval FrameQ_S_SUCCESS Operation successfully completed. */
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
FrameQDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = FrameQ_S_SUCCESS;
    int osStatus    = 0;

    GT_2trace (curTrace, GT_ENTER, "FrameQDrv_ioctl", cmd, args);

    GT_assert (curTrace, (FrameQDrv_refCount > 0));

    osStatus = ioctl (FrameQDrv_handle, cmd, args);
    if (osStatus < 0) {
    /*! @retval FrameQ_E_OSFAILURE Driver ioctl failed */
        status = FrameQ_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((FrameQDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQDrv_ioctl", status);

/*! @retval FrameQ_S_SUCCESS Operation successfully completed. */
    return (status);
}
