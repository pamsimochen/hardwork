/*
 *  @file   FrameQDrv.c
 *
 *  @brief      OS-specific implementation of FrameQ driver for Qnx
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
#include <ti/syslink/utils/Gate.h>

/* Module specific header files */
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/inc/FrameQDrvDefs.h>
#include <ti/syslink/FrameQ_ShMem.h>

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
 *  @brief  Driver name for FrameQ.
 */
#define FRAMEQ_DRIVER_NAME     "/dev/syslinkipc/FrameQ"


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
 *  @brief  Function to open the FrameQ driver.
 *
 *  @sa     GatePetersonDrv_close
 */
Int
FrameQDrv_open (Void)
{
    Int status      = FrameQ_S_SUCCESS;

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

    return (status);
}


/*!
 *  @brief  Function to invoke the APIs through ioctl.
 *
 *  @param  cmd     Command for driver ioctl
 *  @param  args     Arguments for the ioctl command
 *
 *  @sa
 */
Int
FrameQDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = FrameQ_S_SUCCESS;
    int osStatus    = -1;


    GT_2trace (curTrace, GT_ENTER, "FrameQDrv_ioctl", cmd, args);

	switch(cmd) {

		case CMD_FRAMEQ_GETCONFIG:
		{
			  FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			  iov_t	frameq_getconfig_iov[2];

			  SETIOV( &frameq_getconfig_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			  SETIOV( &frameq_getconfig_iov[1], cargs->args.getConfig.config, sizeof(FrameQ_Config) );

			  /* the osal_drv handle is used instead of ipcdrv_handle as the ipcdrv_handle is not yet initialized */
			  osStatus = devctlv( OsalDrv_handle, DCMD_FRAMEQ_GETCONFIG, 1, 2, frameq_getconfig_iov, frameq_getconfig_iov,NULL);
			  break;
		}

		case CMD_FRAMEQ_SETUP:
		{
			  FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			  iov_t	frameq_setup_iov[2];

			  SETIOV( &frameq_setup_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			  SETIOV( &frameq_setup_iov[1], cargs->args.getConfig.config, sizeof(FrameQ_Config) );

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_SETUP, 2, 1, frameq_setup_iov, frameq_setup_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_DESTROY:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_DESTROY, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_PARAMS_INIT:
		{
			//Linux side implementation commented out
			break;
		}

		case CMD_FRAMEQ_CREATE:
		{

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			iov_t	frameq_create_iov[4];
			int i = 0;

			SETIOV( &frameq_create_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			i++;
			SETIOV( &frameq_create_iov[1], cargs->args.create.params, sizeof(FrameQ_ShMem_Params));
			i++;
			if (cargs->args.create.nameLen > 0) {
				SETIOV( &frameq_create_iov[i], cargs->args.create.name, sizeof(char) * cargs->args.create.nameLen);
				i++;
			}
			if (cargs->args.create.fQBMnameLen > 0) {
				SETIOV( &frameq_create_iov[i], cargs->args.create.fQBMName, sizeof(char) * cargs->args.create.fQBMnameLen);
				i++;
			}

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_CREATE, i, 1, frameq_create_iov, frameq_create_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_DELETE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_DELETE, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_OPEN:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			iov_t	frameq_open_iov[3];
			int i = 0;

			SETIOV( &frameq_open_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			i++;
			SETIOV( &frameq_open_iov[1], cargs->args.open.openParams, sizeof(FrameQ_ShMem_OpenParams));
			i++;
			if (cargs->args.open.nameLen > 0) {
				SETIOV( &frameq_open_iov[i], ((FrameQ_OpenParams *)cargs->args.open.openParams)->commonOpenParams.name, sizeof(char) * cargs->args.open.nameLen);
				i++;
			}
			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_OPEN, i, 1, frameq_open_iov, frameq_open_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_CLOSE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_CLOSE, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}


		case CMD_FRAMEQ_PUT:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_PUT, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_PUTV:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			iov_t frameq_putv_iov[3];

			SETIOV( &frameq_putv_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			SETIOV( &frameq_putv_iov[1], cargs->args.putv.aframePtr,cargs->args.putv.numFrames * sizeof(UInt32));
			SETIOV( &frameq_putv_iov[2], cargs->args.putv.filledQueueId, cargs->args.putv.numFrames * sizeof(UInt32));

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_PUTV, 3, 1, frameq_putv_iov, frameq_putv_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_GET:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_GET, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_GETV:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			iov_t frameq_getv_iov[3];

			SETIOV( &frameq_getv_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			SETIOV( &frameq_getv_iov[1], cargs->args.getv.aframePtr,cargs->args.getv.numFrames * sizeof(UInt32));
			SETIOV( &frameq_getv_iov[2], cargs->args.getv.filledQueueId, cargs->args.getv.numFrames * sizeof(UInt32));



			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_GETV, 2, 2, frameq_getv_iov, frameq_getv_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_GET_NUMFRAMES:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_GET_NUMFRAMES, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_GET_VNUMFRAMES:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			iov_t frameq_get_vnumframes_iov[3];

			SETIOV( &frameq_get_vnumframes_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			SETIOV( &frameq_get_vnumframes_iov[1], cargs->args.getvNumFrames.numFrames, sizeof(UInt32) * cargs->args.getvNumFrames.numFilledQids);
			SETIOV( &frameq_get_vnumframes_iov[2], cargs->args.getvNumFrames.filledQId, cargs->args.getvNumFrames.numFilledQids * sizeof (UInt32));



			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_GET_VNUMFRAMES, 2, 2, frameq_get_vnumframes_iov, frameq_get_vnumframes_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_CONTROL:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			  iov_t	frameq_control_iov[2];

			  SETIOV( &frameq_control_iov[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			  SETIOV( &frameq_control_iov[1], cargs->args.control.arg, cargs->args.control.size);

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_CONTROL, 2, 1, frameq_control_iov, frameq_control_iov, NULL);
			break;
		}

		case CMD_FRAMEQ_SET_NOTIFYID:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_SET_NOTIFYID, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_RESET_NOTIFYID:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_RESET_NOTIFYID, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_SHMEM_PARAMS_INIT:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			  iov_t	frameq_shmem_params_init[2];

			  SETIOV( &frameq_shmem_params_init[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			  SETIOV( &frameq_shmem_params_init[1], cargs->args.ParamsInit.params, sizeof (FrameQ_ShMem_Params));

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_SHMEM_PARAMS_INIT, 1, 2, frameq_shmem_params_init, frameq_shmem_params_init, NULL);
			break;
		}

		case CMD_FRAMEQ_SHMEM_MEMREQ:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			  iov_t	frameq_shmem_memreq[3];

			  SETIOV( &frameq_shmem_memreq[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			  SETIOV( &frameq_shmem_memreq[1], &cargs->args.sharedMemReq.bytes, sizeof (UInt32));
			  SETIOV( &frameq_shmem_memreq[2], cargs->args.sharedMemReq.params, sizeof(FrameQ_ShMem_Params));


			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_SHMEM_MEMREQ, 2, 2, frameq_shmem_memreq, frameq_shmem_memreq, NULL);
			break;
		}

		case CMD_FRAMEQ_GET_NUMFREEFRAMES:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_GET_NUMFREEFRAMES, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQ_GET_VNUMFREEFRAMES:
		{
			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *)args;
			iov_t frameq_get_vnumfreeframes[3];

			SETIOV( &frameq_get_vnumfreeframes[0], cargs, sizeof(FrameQDrv_CmdArgs) );
			SETIOV( &frameq_get_vnumfreeframes[1], cargs->args.getvNumFreeFrames.numFreeFrames, sizeof(UInt32) * cargs->args.getvNumFreeFrames.numFreeQids);
			SETIOV( &frameq_get_vnumfreeframes[2], cargs->args.getvNumFreeFrames.freeQId, cargs->args.getvNumFreeFrames.numFreeQids * sizeof (UInt32));


			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQ_GET_VNUMFREEFRAMES, 2, 2, frameq_get_vnumfreeframes, frameq_get_vnumfreeframes, NULL);
			break;
		}


		case CMD_FRAMEQ_REG_NOTIFIER:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_REG_NOTIFIER, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}


		case CMD_FRAMEQ_UNREG_NOTIFIER:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQ_UNREG_NOTIFIER, args, sizeof(FrameQDrv_CmdArgs), NULL);
			break;
		}

		default:
		{
			  /* This does not impact return status of this function, so retVal
			   * comment is not used.
			   */
			  status = FrameQ_E_INVALIDARG;
			  GT_setFailureReason (curTrace,
								   GT_4CLASS,
								   "FrameQDrv_ioctl",
								   status,
								   "Unsupported ioctl command specified");
			  break;
		}


	}



	if (osStatus != 0) {
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

    return (status);
}
