/*
 *  @file   HeapMemMPDrv.c
 *
 *  @brief      OS-specific implementation of HeapMemMP driver for Qnx
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
#include <ti/ipc/HeapMemMP.h>
#include <ti/syslink/inc/HeapMemMPDrvDefs.h>
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
 *  @brief  Driver name for HeapMemMP.
 */
#define HEAPMEM_DRIVER_NAME     "/dev/syslinkipc/HeapMemMP"


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
 *  @brief  Function to open the HeapMemMP driver.
 *
 *  @sa     HeapMemMPDrv_close
 */
Int
HeapMemMPDrv_open (Void)
{
    Int status      = HeapMemMP_S_SUCCESS;

    return status;
}


/*!
 *  @brief  Function to close the HeapMemMP driver.
 *
 *  @sa     HeapMemMPDrv_open
 */
Int
HeapMemMPDrv_close (Void)
{
    Int status      = HeapMemMP_S_SUCCESS;

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
HeapMemMPDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = HeapMemMP_S_SUCCESS;
    int osStatus    = -1;

    GT_2trace (curTrace, GT_ENTER, "HeapMemMPDrv_ioctl", cmd, args);

    switch (cmd) {

        case CMD_HEAPMEMMP_ALLOC:
            osStatus = devctl(IpcDrv_handle, DCMD_HEAPMEMMP_ALLOC, args,
                    sizeof(HeapMemMPDrv_CmdArgs), NULL);
            break;

        case CMD_HEAPMEMMP_FREE:
            osStatus = devctl( IpcDrv_handle, DCMD_HEAPMEMMP_FREE, args,
                    sizeof(HeapMemMPDrv_CmdArgs), NULL);
            break;

        case CMD_HEAPMEMMP_PARAMS_INIT:
            {
                HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
                iov_t paramsinit_iov[2];

                SETIOV( &paramsinit_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
                SETIOV( &paramsinit_iov[1], cargs->args.ParamsInit.params, sizeof(HeapMemMP_Params) );
                osStatus = devctlv( IpcDrv_handle, DCMD_HEAPMEMMP_PARAMS_INIT, 1, 2, paramsinit_iov, paramsinit_iov, NULL);
            }
            break;

        case CMD_HEAPMEMMP_CREATE: {
            HeapMemMPDrv_CmdArgs *      cargs;
            iov_t                       iov[3];

            cargs = (HeapMemMPDrv_CmdArgs *)args;
            SETIOV(&iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs));
            SETIOV(&iov[1], cargs->args.create.params,
                sizeof(HeapMemMP_Params));
            SETIOV(&iov[2], cargs->args.create.params->name,
                cargs->args.create.nameLen * sizeof(Char));

            osStatus = devctlv(IpcDrv_handle, DCMD_HEAPMEMMP_CREATE, 3, 1,
                iov, iov, NULL);
        }
        break;

        case CMD_HEAPMEMMP_DELETE: {
            osStatus = devctl(IpcDrv_handle, DCMD_HEAPMEMMP_DELETE, args,
                sizeof(HeapMemMPDrv_CmdArgs), NULL);
        }
        break;

        case CMD_HEAPMEMMP_OPEN:
            {
				HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
				iov_t open_iov[2];

				SETIOV( &open_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
				SETIOV( &open_iov[1], cargs->args.open.name, sizeof(char) * cargs->args.open.nameLen );
				osStatus = devctlv( IpcDrv_handle, DCMD_HEAPMEMMP_OPEN, 2, 1, open_iov, open_iov, NULL);
            }
            break;

			case CMD_HEAPMEMMP_OPENBYADDR:
			{
				 osStatus = devctl( IpcDrv_handle, DCMD_HEAPMEMMP_OPENBYADDR, args, sizeof(HeapMemMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_HEAPMEMMP_CLOSE:
			{
				 osStatus = devctl( IpcDrv_handle, DCMD_HEAPMEMMP_CLOSE, args, sizeof(HeapMemMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_HEAPMEMMP_SHAREDMEMREQ:
			{
				HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
				iov_t shm_iov[2];

				SETIOV( &shm_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
				SETIOV( &shm_iov[1], cargs->args.sharedMemReq.params, sizeof(HeapMemMP_Params) );
				osStatus = devctlv( IpcDrv_handle, DCMD_HEAPMEMMP_SHAREDMEMREQ, 2, 1, shm_iov, shm_iov, NULL);
			}
			break;

			case CMD_HEAPMEMMP_GETCONFIG:
			{
				HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
				iov_t config_iov[2];

				SETIOV( &config_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
				SETIOV( &config_iov[1], cargs->args.getConfig.config, sizeof(HeapMemMP_Config) );
				/* the osal_drv handle is used instead of ipcdrv_handle as the ipcdrc_handle is not yet initialized */
				osStatus = devctlv( OsalDrv_handle, DCMD_HEAPMEMMP_GETCONFIG, 1, 2, config_iov, config_iov, NULL);
			}
			break;

			case CMD_HEAPMEMMP_SETUP:
			{
				HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
				iov_t setup_iov[2];

				SETIOV( &setup_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
				SETIOV( &setup_iov[1], cargs->args.setup.config, sizeof(HeapMemMP_Config) );
	            osStatus = devctlv( IpcDrv_handle, DCMD_HEAPMEMMP_SETUP, 2, 1, setup_iov, setup_iov, NULL);
			}
			break;

			case CMD_HEAPMEMMP_DESTROY:
			{
				 osStatus = devctl( IpcDrv_handle, DCMD_HEAPMEMMP_DESTROY, args, sizeof(HeapMemMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_HEAPMEMMP_GETSTATS:
			{
				HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
				iov_t stats_iov[2];

				SETIOV( &stats_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
				SETIOV( &stats_iov[1], cargs->args.getStats.stats, sizeof(Memory_Stats) );
				osStatus = devctlv( IpcDrv_handle, DCMD_HEAPMEMMP_GETSTATS, 1, 2, stats_iov, stats_iov, NULL);
			}
			break;

			case CMD_HEAPMEMMP_GETEXTENDEDSTATS:
			{
				HeapMemMPDrv_CmdArgs *cargs = (HeapMemMPDrv_CmdArgs *)args;
				iov_t stats_iov[2];

				SETIOV( &stats_iov[0], cargs, sizeof(HeapMemMPDrv_CmdArgs) );
				SETIOV( &stats_iov[1], cargs->args.getExtendedStats.stats, sizeof(HeapMemMP_ExtendedStats) );

				osStatus = devctlv( IpcDrv_handle, DCMD_HEAPMEMMP_GETEXTENDEDSTATS, 1, 2, stats_iov, stats_iov, NULL);
			}
			break;

			case CMD_HEAPMEMMP_RESTORE:
			{
				 osStatus = devctl( IpcDrv_handle, DCMD_HEAPMEMMP_RESTORE, args, sizeof(HeapMemMPDrv_CmdArgs), NULL);
			}
			break;

			default :
			{
				/* This does not impact return status of this function, so retVal
				 * comment is not used.
				 */
				status = HeapMemMP_E_INVALIDARG;
				GT_setFailureReason (curTrace,
									 GT_4CLASS,
									 "HeapMemMPDrv_ioctl",
									 status,
									 "Unsupported ioctl command specified");
			}
			break;

		}

    if (osStatus != 0) {
        /*! @retval HeapMemMP_E_OSFAILURE Driver ioctl failed */
        status = HeapMemMP_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMPDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((HeapMemMPDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMPDrv_ioctl", status);

    return status;
}
