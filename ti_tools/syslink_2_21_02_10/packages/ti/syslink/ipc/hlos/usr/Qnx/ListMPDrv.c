/*
 *  @file   ListMPDrv.c
 *
 *  @brief      OS-specific implementation of ListMP driver
 *              for Linux
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
#include <ti/syslink/inc/_ListMP.h>
#include <ti/ipc/ListMP.h>
#include <ti/syslink/inc/ListMPDrvDefs.h>
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
 *  @brief  Driver name for ListMP.
 */
#define LISTMP_DRIVER_NAME     "/dev/syslinkipc/ListMP"


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
 *  @brief  Function to open the ListMP driver.
 *
 *  @sa     ListMPDrv_close
 */
Int
ListMPDrv_open (Void)
{
    Int status      = ListMP_S_SUCCESS;

    return status;
}


/*!
 *  @brief  Function to close the ListMP driver.
 *
 *  @sa     ListMPDrv_open
 */
Int
ListMPDrv_close (Void)
{
    Int status      = ListMP_S_SUCCESS;

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
ListMPDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = ListMP_S_SUCCESS;
    Int osStatus    = -1;

    GT_2trace (curTrace, GT_ENTER, "ListMPDrv_ioctl", cmd, args);

    switch (cmd) {
        case CMD_LISTMP_CREATE: {
            ListMPDrv_CmdArgs * cargs;
            iov_t               iov[3];

            cargs = (ListMPDrv_CmdArgs *)args;
            SETIOV(&iov[0], cargs, sizeof(ListMPDrv_CmdArgs));
            SETIOV(&iov[1], cargs->args.create.params, sizeof(ListMP_Params));
            SETIOV(&iov[2], cargs->args.create.params->name,
                cargs->args.create.nameLen * sizeof(Char));

            osStatus = devctlv(IpcDrv_handle, DCMD_LISTMP_CREATE, 3, 1,
                iov, iov, NULL);
        }
        break;

        case CMD_LISTMP_DELETE: {
            osStatus = devctl(IpcDrv_handle, DCMD_LISTMP_DELETE, args,
                sizeof(ListMPDrv_CmdArgs), NULL);
        }
        break;

			case CMD_LISTMP_OPEN:
			{
				ListMPDrv_CmdArgs *cargs = (ListMPDrv_CmdArgs *)args;
				iov_t open_iov[2];

				SETIOV( &open_iov[0], cargs, sizeof(ListMPDrv_CmdArgs) );
				SETIOV( &open_iov[1], cargs->args.open.name, sizeof(char) * cargs->args.open.nameLen );
				osStatus = devctlv( IpcDrv_handle, DCMD_LISTMP_OPEN, 2, 1, open_iov, open_iov, NULL);
			}
			break;

			case CMD_LISTMP_OPENBYADDR:
			{

				osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_OPENBYADDR, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_CLOSE:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_CLOSE, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_SHAREDMEMREQ:
			{
				ListMPDrv_CmdArgs *cargs = (ListMPDrv_CmdArgs *)args;
				iov_t shm_iov[2];

				SETIOV( &shm_iov[0], cargs, sizeof(ListMPDrv_CmdArgs) );
				SETIOV( &shm_iov[1], cargs->args.sharedMemReq.params, sizeof(ListMP_Params) );

				osStatus = devctlv( IpcDrv_handle, DCMD_LISTMP_SHAREDMEMREQ, 2, 1, shm_iov, shm_iov, NULL);
			}
			break;

			case CMD_LISTMP_ISEMPTY:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_ISEMPTY, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_PUTTAIL:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_PUTTAIL, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_GETHEAD:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_GETHEAD, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_NEXT:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_NEXT, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_PREV:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_PREV, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_INSERT:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_INSERT, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_REMOVE:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_REMOVE, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_GETTAIL:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_GETTAIL, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_PUTHEAD:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_PUTHEAD, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_GETCONFIG:
			{
				ListMPDrv_CmdArgs *cargs = (ListMPDrv_CmdArgs *)args;
				iov_t config_iov[2];

				SETIOV( &config_iov[0], cargs, sizeof(ListMPDrv_CmdArgs) );
				SETIOV( &config_iov[1], cargs->args.getConfig.config, sizeof(ListMP_Config) );

				/* the osal_drv handle is used instead of ipcdrv_handle as the ipcdrc_handle is not yet initialized */
				osStatus = devctlv( OsalDrv_handle, DCMD_LISTMP_GETCONFIG, 2, 2, config_iov, config_iov, NULL);
			}
			break;

			case CMD_LISTMP_SETUP:
			{
				ListMPDrv_CmdArgs *cargs = (ListMPDrv_CmdArgs *)args;
				iov_t setup_iov[2];

				SETIOV( &setup_iov[0], cargs, sizeof(ListMPDrv_CmdArgs) );
				SETIOV( &setup_iov[1], cargs->args.setup.config, sizeof(ListMP_Config) );

				osStatus = devctlv( IpcDrv_handle, DCMD_LISTMP_SETUP, 2, 1, setup_iov, setup_iov, NULL);
			}
			break;

			case CMD_LISTMP_DESTROY:
			{
					osStatus = devctl( IpcDrv_handle, DCMD_LISTMP_DESTROY, args, sizeof(ListMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_LISTMP_PARAMS_INIT:
			{
				ListMPDrv_CmdArgs *cargs = (ListMPDrv_CmdArgs *)args;
				iov_t params_iov[2];

				SETIOV( &params_iov[0], cargs, sizeof(ListMPDrv_CmdArgs) );
				SETIOV( &params_iov[1], cargs->args.ParamsInit.params, sizeof(ListMP_Params) );

				osStatus = devctlv( IpcDrv_handle, DCMD_LISTMP_PARAMS_INIT, 1, 2, params_iov, params_iov, NULL);
			}
			break;

			default:
			{
				/* This does not impact return status of this function, so retVal
				 * comment is not used.
				 */
				status = ListMP_E_INVALIDARG;
				GT_setFailureReason (curTrace,
									 GT_4CLASS,
									 "ListMPDrv_ioctl",
									 status,
									 "Unsupported ioctl command specified");
			}
			break;
		}


    if (osStatus != 0) {
        /*! @retval ListMP_E_OSFAILURE Driver ioctl failed */
        status = ListMP_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMPDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((ListMPDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "ListMPDrv_ioctl", status);

    return status;
}
