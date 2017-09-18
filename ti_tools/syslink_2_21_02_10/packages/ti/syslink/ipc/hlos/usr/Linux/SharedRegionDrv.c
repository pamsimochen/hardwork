/*
 *  @file   SharedRegionDrv.c
 *
 *  @brief      OS-specific implementation of SharedRegion driver for Linux
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
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Dev.h>
#include <ti/ipc/MultiProc.h>

/* Module specific header files */
#include <ti/syslink/utils/Gate.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/SharedRegionDrvDefs.h>

/* C/posix header files */
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
 *  @brief  Driver name for SharedRegion.
 */
#define SHAREDREGION_DRIVER_NAME     "/dev/syslinkipc_SharedRegion"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for SharedRegion in this process.
 */
static Int32 SharedRegionDrv_handle = 0;

/*!
 *  @brief  Reference count for the driver handle.
 */
static UInt32 SharedRegionDrv_refCount = 0;


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the SharedRegion driver.
 *
 *  @sa     SharedRegionDrv_close
 */
Int
SharedRegionDrv_open (Void)
{
    Int status      = SharedRegion_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "SharedRegionDrv_open");

    if (SharedRegionDrv_refCount == 0) {
        /* TBD: Protection for refCount. */
        SharedRegionDrv_refCount++;

        SharedRegionDrv_handle = Dev_pollOpen (SHAREDREGION_DRIVER_NAME,
                                       O_SYNC | O_RDWR);
        if (SharedRegionDrv_handle < 0) {
            perror ("SharedRegion driver open: ");
            /*! @retval SharedRegion_E_OSFAILURE Failed to open SharedRegion
                                 driver with OS */
            status = SharedRegion_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SharedRegionDrv_open",
                                 status,
                                 "Failed to open SharedRegion driver with OS!");
        }
        else {
            osStatus = fcntl (SharedRegionDrv_handle, F_SETFD, FD_CLOEXEC);
            if (osStatus != 0) {
                /*! @retval SharedRegion_E_OSFAILURE Failed to set file
                                    descriptor flags */
                status = SharedRegion_E_OSFAILURE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "SharedRegionDrv_open",
                                     status,
                                     "Failed to set file descriptor flags!");
            }
        }
    }
    else {
        SharedRegionDrv_refCount++;
    }

    GT_1trace (curTrace, GT_LEAVE, "SharedRegionDrv_open", status);

    /*! @retval SharedRegion_S_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief  Function to close the SharedRegion driver.
 *
 *  @sa     SharedRegionDrv_open
 */
Int
SharedRegionDrv_close (Void)
{
    Int status      = SharedRegion_S_SUCCESS;
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "SharedRegionDrv_close");

    /* TBD: Protection for refCount. */
    SharedRegionDrv_refCount--;
    if (SharedRegionDrv_refCount == 0) {
        osStatus = close (SharedRegionDrv_handle);
        if (osStatus != 0) {
            perror ("SharedRegion driver close: ");
            /*! @retval SharedRegion_E_OSFAILURE Failed to open SharedRegion
                        driver with OS */
            status = SharedRegion_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SharedRegionDrv_close",
                                 status,
                                 "Failed to close SharedRegion driver with OS!");
        }
        else {
            SharedRegionDrv_handle = 0;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "SharedRegionDrv_close", status);

    /*! @retval SharedRegion_S_SUCCESS Operation successfully completed. */
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
SharedRegionDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int                       status   = SharedRegion_S_SUCCESS;
    int                       osStatus = 0;
    SharedRegionDrv_CmdArgs * cargs    = (SharedRegionDrv_CmdArgs *) args;
    SharedRegion_Region    *  regions  = NULL;
    SharedRegion_Config *     config;
    Memory_MapInfo            mapInfo;
    Memory_UnmapInfo          unmapInfo;
    UInt16                    i;

    GT_2trace (curTrace, GT_ENTER, "SharedRegionDrv_ioctl", cmd, args);

    GT_assert (curTrace, (SharedRegionDrv_refCount > 0));

    osStatus = ioctl (SharedRegionDrv_handle, cmd, args);
    if (osStatus < 0) {
        /*! @retval SharedRegion_E_OSFAILURE Driver ioctl failed */
        status = SharedRegion_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SharedRegionDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((SharedRegionDrv_CmdArgs *) args)->apiStatus;

        /* Convert the base address to user virtual address */
        if (cmd == CMD_SHAREDREGION_SETUP) {
            config = cargs->args.setup.config;
            for (i = 0u; (   (i < config->numEntries) && (status >= 0)); i++) {
                regions = &(cargs->args.setup.regions [i]);
                if (regions->entry.isValid == TRUE) {
                    mapInfo.src  = (UInt32) regions->entry.base;
                    mapInfo.size = regions->entry.len;
                    mapInfo.isCached = regions->entry.cacheEnable;
                    status = Memory_map (&mapInfo);
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "SharedRegionDrv_ioctl",
                                             status,
                                             "Memory_map failed!");
                    }
                    else {
                        regions->entry.base = (Ptr) mapInfo.dst;
                    }
                }
            }
        }
        if (cmd == CMD_SHAREDREGION_DESTROY) {
            config = cargs->args.setup.config;
            for (i = 0u; (   (i < config->numEntries) && (status >= 0)); i++) {
                regions = &(cargs->args.setup.regions [i]);
                if (regions->entry.isValid == TRUE) {
                    unmapInfo.addr  = (UInt32) regions->entry.base;
                    unmapInfo.size = regions->entry.len;
                    unmapInfo.isCached = regions->entry.cacheEnable;
                    status = Memory_unmap (&unmapInfo);
               }
            }
        }
        else {
            /* All other control calls do nothing */
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "SharedRegionDrv_ioctl", status);

/*! @retval SharedRegion_S_SUCCESS Operation successfully completed. */
    return status;
}
