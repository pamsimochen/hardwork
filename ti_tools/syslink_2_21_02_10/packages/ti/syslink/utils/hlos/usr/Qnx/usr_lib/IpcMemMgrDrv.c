/*
 *  @file   IpcMemMgrDrv.c
 *
 *  @brief      Driver for IpcMemMgr on HLOS side
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
#include "ti/syslink/Std.h"

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/IpcMemMgr.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/inc/IpcMemMgrDrvDefs.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>

/* QNX specific header files */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver name for IpcMemMgr.
 */
#define IPCMEMMGR_DRIVER_NAME   "/dev/syslinkipc/IpcMemMgr"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

extern Int32 IpcDrv_handle;
extern Int32 OsalDrv_handle;


/** ============================================================================
 *  Functions
 *  ============================================================================
 */

/*!
 *  @brief  Function to open the IpcMemMgr driver.
 *
 *  @sa     IpcMemMgrDrv_close
 */
Int IpcMemMgrDrv_open(Void)
{
    Int status = IpcMemMgr_S_SUCCESS;

    /*! @retval IpcMemMgr_S_SUCCESS Operation successfully completed. */
    return(status);
}


/*!
 *  @brief  Function to close the IpcMemMgr driver.
 *
 *  @sa     IpcMemMgrDrv_open
 */
Int IpcMemMgrDrv_close(Void)
{
    Int status = IpcMemMgr_S_SUCCESS;

    /*! @retval IpcMemMgr_S_SUCCESS Operation successfully completed. */
    return(status);
}


/*!
 *  @brief  Function to invoke the APIs through devctl.
 *
 *  @param  cmd     Command for driver ioctl
 *  @param  args    Arguments for the ioctl command
 *
 *  @sa
 */
Int IpcMemMgrDrv_ioctl(UInt32 cmd, Ptr args)
{
    Int status;
    Int osStatus;
    Int apiStatus;

    GT_2trace(curTrace, GT_ENTER, "IpcMemMgrDrv_ioctl:", cmd, args);

    status = IpcMemMgr_S_SUCCESS;
    osStatus = 0;

    switch (cmd) {
         case CMD_IPCMEMMGR_GETCONFIG: {
            IpcMemMgrDrv_CmdArgs *cargs;
            IpcMemMgr_Config *cfg;

            cargs = (IpcMemMgrDrv_CmdArgs *)args;
            cfg = cargs->args.getConfig.config;

            osStatus = devctl(OsalDrv_handle, DCMD_IPCMEMMGR_GETCONFIG,
                cfg, sizeof(IpcMemMgr_Config), &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        } break;

       case CMD_IPCMEMMGR_SETUP: {
            IpcMemMgrDrv_CmdArgs *cargs;
            IpcMemMgr_Config *cfg;

            cargs = (IpcMemMgrDrv_CmdArgs *)args;
            cfg = cargs->args.setup.config;

            osStatus = devctl(IpcDrv_handle, DCMD_IPCMEMMGR_SETUP,
                cfg, sizeof(IpcMemMgr_Config), &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        } break;

        case CMD_IPCMEMMGR_DESTROY: {
            osStatus = devctl(IpcDrv_handle, DCMD_IPCMEMMGR_DESTROY, NULL,
                0, &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        }
        break;

        case CMD_IPCMEMMGR_ALLOC: {
            IpcMemMgrDrv_CmdArgs *cargs = (IpcMemMgrDrv_CmdArgs *)args;

            osStatus = devctl(IpcDrv_handle, DCMD_IPCMEMMGR_ALLOC,
                cargs, sizeof(IpcMemMgrDrv_CmdArgs), &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        }
        break;

        case CMD_IPCMEMMGR_FREE: {
            IpcMemMgrDrv_CmdArgs *cargs = (IpcMemMgrDrv_CmdArgs *)args;

            osStatus = devctl(IpcDrv_handle, DCMD_IPCMEMMGR_FREE,
                    cargs, sizeof(IpcMemMgrDrv_CmdArgs), &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        }
        break;

        case CMD_IPCMEMMGR_ACQUIRE: {
            IpcMemMgrDrv_CmdArgs *cargs;
            iov_t iov[2];

            cargs = (IpcMemMgrDrv_CmdArgs *)args;
            SETIOV(&iov[0], cargs, sizeof(IpcMemMgrDrv_CmdArgs));
            SETIOV(&iov[1], cargs->args.acquire.name,
                cargs->args.acquire.nameSize);

            osStatus = devctlv(IpcDrv_handle, DCMD_IPCMEMMGR_ACQUIRE,
                    2, 1, iov, iov, &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        }
        break;

        case CMD_IPCMEMMGR_RELEASE: {
            IpcMemMgrDrv_CmdArgs *cargs = (IpcMemMgrDrv_CmdArgs *)args;

            osStatus = devctl(IpcDrv_handle, DCMD_IPCMEMMGR_RELEASE,
                cargs->args.release.name, cargs->args.release.nameSize,
                &apiStatus);

            if (osStatus != EOK) {
                status = IpcMemMgr_E_OSFAILURE;
            }
            else {
                status = apiStatus;
            }
        }
        break;

        default: {
            status = IpcMemMgr_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcMemMgrDrv_ioctl",
                status, "Unsupported ioctl command specified");
        }
        break;

    } /* switch */

    GT_1trace(curTrace, GT_LEAVE, "IpcMemMgrDrv_ioctl", status);

    return (status);
}
