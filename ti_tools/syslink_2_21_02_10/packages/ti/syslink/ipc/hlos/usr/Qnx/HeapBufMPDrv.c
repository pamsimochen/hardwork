/*
 *  @file   HeapBufMPDrv.c
 *
 *  @brief      OS-specific implementation of HeapBufMP driver for Qnx
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
#include <ti/syslink/utils/IHeap.h>

/* Module specific header files */
#include <ti/syslink/inc/_HeapBufMP.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/syslink/inc/HeapBufMPDrvDefs.h>
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
 *  @brief  Driver name for HeapBufMP.
 */
#define HEAPBUF_DRIVER_NAME     "/dev/syslinkipc/HeapBufMP"


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
 *  @brief  Function to open the HeapBufMP driver.
 *
 *  @sa     HeapBufMPDrv_close
 */
Int
HeapBufMPDrv_open (Void)
{
    Int status      = HeapBufMP_S_SUCCESS;

    return status;
}


/*!
 *  @brief  Function to close the HeapBufMP driver.
 *
 *  @sa     HeapBufMPDrv_open
 */
Int
HeapBufMPDrv_close (Void)
{
    Int status      = HeapBufMP_S_SUCCESS;

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
HeapBufMPDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = HeapBufMP_S_SUCCESS;
    int osStatus    = -1;

    GT_2trace (curTrace, GT_ENTER, "HeapBufMPDrv_ioctl", cmd, args);

    switch (cmd) {
        case CMD_HEAPBUFMP_ALLOC:
            osStatus = devctl(IpcDrv_handle, DCMD_HEAPBUFMP_ALLOC, args,
                    sizeof(HeapBufMPDrv_CmdArgs), NULL);
            break;

        case CMD_HEAPBUFMP_FREE:
            osStatus = devctl( IpcDrv_handle, DCMD_HEAPBUFMP_FREE, args,
                    sizeof(HeapBufMPDrv_CmdArgs), NULL);
            break;

        case CMD_HEAPBUFMP_PARAMS_INIT:
            {
                HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
                iov_t hbmpparamsinit_iov[2];

                SETIOV( &hbmpparamsinit_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
                SETIOV( &hbmpparamsinit_iov[1], cargs->args.ParamsInit.params, sizeof(HeapBufMP_Params) );
                osStatus = devctlv( IpcDrv_handle, DCMD_HEAPBUFMP_PARAMS_INIT, 1, 2, hbmpparamsinit_iov, hbmpparamsinit_iov, NULL);
            }
            break;

        case CMD_HEAPBUFMP_CREATE: {
            HeapBufMPDrv_CmdArgs *cargs;
            iov_t iov[3];

            cargs = (HeapBufMPDrv_CmdArgs *)args;
            SETIOV(&iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs));
            SETIOV(&iov[1], cargs->args.create.params,
                sizeof(HeapBufMP_Params));
            SETIOV(&iov[2], cargs->args.create.params->name,
                cargs->args.create.nameLen * sizeof(Char));

            osStatus = devctlv(IpcDrv_handle, DCMD_HEAPBUFMP_CREATE, 3, 1,
                iov, iov, NULL);
        }
        break;

        case CMD_HEAPBUFMP_DELETE: {
            osStatus = devctl(IpcDrv_handle, DCMD_HEAPBUFMP_DELETE, args,
                sizeof(HeapBufMPDrv_CmdArgs), NULL);
        }
        break;

        case CMD_HEAPBUFMP_OPEN:
            {
			HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
			iov_t hbmpopen_iov[2];

			SETIOV( &hbmpopen_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
			SETIOV( &hbmpopen_iov[1], cargs->args.open.name, sizeof(char) * cargs->args.open.nameLen );
			osStatus = devctlv( IpcDrv_handle, DCMD_HEAPBUFMP_OPEN, 2, 1, hbmpopen_iov, hbmpopen_iov, NULL);
            }
            break;

        case CMD_HEAPBUFMP_OPENBYADDR:
        {
           osStatus = devctl( IpcDrv_handle, DCMD_HEAPBUFMP_OPENBYADDR, args, sizeof(HeapBufMPDrv_CmdArgs), NULL);
        }
        break;

        case CMD_HEAPBUFMP_CLOSE:
        {
           osStatus = devctl( IpcDrv_handle, DCMD_HEAPBUFMP_CLOSE, args, sizeof(HeapBufMPDrv_CmdArgs), NULL);
        }
        break;

        case CMD_HEAPBUFMP_SHAREDMEMREQ:
        {
			HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
			iov_t hbmpshm_iov[2];

			SETIOV( &hbmpshm_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
			SETIOV( &hbmpshm_iov[1], cargs->args.sharedMemReq.params, sizeof(HeapBufMP_Params) );
			osStatus = devctlv( IpcDrv_handle, DCMD_HEAPBUFMP_SHAREDMEMREQ, 2, 1, hbmpshm_iov, hbmpshm_iov, NULL);
        }
        break;

        case CMD_HEAPBUFMP_GETCONFIG:
        {
			HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
			iov_t hbmpgconfig_iov[2];

			SETIOV( &hbmpgconfig_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
			SETIOV( &hbmpgconfig_iov[1], cargs->args.getConfig.config, sizeof(HeapBufMP_Config) );
			osStatus = devctlv( OsalDrv_handle, DCMD_HEAPBUFMP_GETCONFIG, 1, 2, hbmpgconfig_iov, hbmpgconfig_iov, NULL);
        }
        break;

        case CMD_HEAPBUFMP_SETUP:
        {
			HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
			iov_t hbmpsetup_iov[2];

			SETIOV( &hbmpsetup_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
			SETIOV( &hbmpsetup_iov[1], cargs->args.setup.config, sizeof(HeapBufMP_Config) );
            osStatus = devctlv( IpcDrv_handle, DCMD_HEAPBUFMP_SETUP, 2, 1, hbmpsetup_iov, hbmpsetup_iov, NULL);
        }
        break;

        case CMD_HEAPBUFMP_DESTROY:
        {
          osStatus = devctl( IpcDrv_handle, DCMD_HEAPBUFMP_DESTROY, args, sizeof(HeapBufMPDrv_CmdArgs), NULL);
        }
        break;

        case CMD_HEAPBUFMP_GETSTATS:
        {
			HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
			iov_t hbmpstats_iov[2];

			SETIOV( &hbmpstats_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
			SETIOV( &hbmpstats_iov[1], cargs->args.getStats.stats, sizeof(Memory_Stats) );
			osStatus = devctlv( IpcDrv_handle, DCMD_HEAPBUFMP_GETSTATS, 1, 2, hbmpstats_iov, hbmpstats_iov, NULL);
        }
        break;

        case CMD_HEAPBUFMP_GETEXTENDEDSTATS:
        {
			HeapBufMPDrv_CmdArgs *cargs = (HeapBufMPDrv_CmdArgs *)args;
			iov_t hbmpstats_iov[2];

			SETIOV( &hbmpstats_iov[0], cargs, sizeof(HeapBufMPDrv_CmdArgs) );
			SETIOV( &hbmpstats_iov[1], cargs->args.getExtendedStats.stats, sizeof(HeapBufMP_ExtendedStats) );

			osStatus = devctlv( IpcDrv_handle, DCMD_HEAPBUFMP_GETEXTENDEDSTATS, 1, 2, hbmpstats_iov, hbmpstats_iov, NULL);
        }
        break;

        default :
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = HeapBufMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapBufMPDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
  }

    if (osStatus != 0) {
        /*! @retval HeapBufMP_E_OSFAILURE Driver ioctl failed */
        status = HeapBufMP_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapBufMPDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((HeapBufMPDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "HeapBufMPDrv_ioctl", status);

    return status;
}
