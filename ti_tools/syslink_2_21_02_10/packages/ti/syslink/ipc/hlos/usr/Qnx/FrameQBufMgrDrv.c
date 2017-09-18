/*
 *  @file   FrameQBufMgrDrv.c
 *
 *  @brief      OS-specific implementation of FraemQBufMgr driver for Qnx
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
#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/inc/FrameQBufMgrDrvDefs.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>

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
 *  @brief  Driver name for FrameQBufMgr.
 */
#define FRAMEQBUFMGR_DRIVER_NAME     "/dev/syslinkipc/FrameQBufMgr"


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
 *  @brief  Function to open the FrameQBufMgr driver.
 *
 *  @sa     FrameQBufMgrDrv_close
 */
Int
FrameQBufMgrDrv_open (Void)
{
    Int status      = FrameQBufMgr_S_SUCCESS;

    return (status);
}


/*!
 *  @brief  Function to close the FrameQBufMgr driver.
 *
 *  @sa     FrameQBufMgrDrv_open
 */
Int
FrameQBufMgrDrv_close (Void)
{
    Int status      = FrameQBufMgr_S_SUCCESS;

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
FrameQBufMgrDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = FrameQBufMgr_S_SUCCESS;
    int osStatus    = -1;

    GT_2trace (curTrace, GT_ENTER, "FrameQBufMgrDrv_ioctl", cmd, args);

	switch(cmd) {

		case CMD_FRAMEQBUFMGR_GETCONFIG:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_getconfig_iov[2];

			SETIOV( &frameqbufmgr_getconfig_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_getconfig_iov[1], cargs->args.getConfig.config, sizeof (FrameQBufMgr_Config));

			osStatus = devctlv( OsalDrv_handle, DCMD_FRAMEQBUFMGR_GETCONFIG, 1, 2, frameqbufmgr_getconfig_iov, frameqbufmgr_getconfig_iov, NULL);

			break;
		}

		case CMD_FRAMEQBUFMGR_SETUP:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_setup_iov[2];

			SETIOV( &frameqbufmgr_setup_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_setup_iov[1], cargs->args.setup.config, sizeof (FrameQBufMgr_Config));

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_SETUP, 2, 1, frameqbufmgr_setup_iov, frameqbufmgr_setup_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_DESTROY:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_DESTROY, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_PARAMS_INIT:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_PARAMS_INIT, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_CREATE:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			FrameQBufMgr_ShMem_Params *params;
			int i = 0, k = 0;
			UInt32 j;

			iov_t frameqbufmgr_create_iov[FrameQBufMgr_MAX_POOLS + 3];

			SETIOV( &frameqbufmgr_create_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			i++;
			SETIOV( &frameqbufmgr_create_iov[1], cargs->args.create.params, sizeof(FrameQBufMgr_ShMem_Params));
			i++;


			if (cargs->args.create.interfaceType ==  FrameQBufMgr_INTERFACE_SHAREDMEM) {
				params = (FrameQBufMgr_ShMem_Params*)(cargs->args.create.params);
				j = params->numFreeFramePools;
				for (k = 0; k < j;k++,i++) {
						SETIOV( &frameqbufmgr_create_iov[i], ((FrameQBufMgr_ShMem_Params*)cargs->args.create.params)->frameBufParams[k], sizeof(FrameQBufMgr_ShMem_FrameBufParams) * params->numFrameBufsInFrame[k]);
				}
			}

			if (cargs->args.open.nameLen > 0) {
				SETIOV( &frameqbufmgr_create_iov[i], cargs->args.create.name, sizeof(char) * cargs->args.create.nameLen);
				i++;
			}

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_CREATE, i, 1, frameqbufmgr_create_iov, frameqbufmgr_create_iov, NULL);

			break;
		}

		case CMD_FRAMEQBUFMGR_OPEN:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_open_iov[3];
			int i = 0;

			SETIOV( &frameqbufmgr_open_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			i++;
			SETIOV( &frameqbufmgr_open_iov[1], cargs->args.open.openParams, sizeof(FrameQBufMgr_ShMem_OpenParams));
			i++;
			if (cargs->args.open.nameLen > 0) {
				SETIOV( &frameqbufmgr_open_iov[i], ((FrameQBufMgr_OpenParams *)cargs->args.open.openParams)->commonOpenParams.name, sizeof(char) * cargs->args.open.nameLen);
				i++;
			}
			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_OPEN, i, 1, frameqbufmgr_open_iov, frameqbufmgr_open_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_CLOSE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_CLOSE, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_GETHANDLE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_GETHANDLE, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_ALLOC:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_ALLOC, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_ALLOCV:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_allocv_iov[3];

			SETIOV( &frameqbufmgr_allocv_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_allocv_iov[1], cargs->args.allocv.aframePtr, sizeof (UInt32) * cargs->args.allocv.numFrames);
			SETIOV( &frameqbufmgr_allocv_iov[2], cargs->args.allocv.afreeQId, sizeof (UInt32) * cargs->args.allocv.numFrames);


			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_ALLOCV, 2, 2, frameqbufmgr_allocv_iov, frameqbufmgr_allocv_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_FREE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_FREE, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_DUP:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_dup_iov[2];

			SETIOV( &frameqbufmgr_dup_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_dup_iov[1], cargs->args.dup.adupedFramePtr, sizeof (UInt32) * cargs->args.dup.numDupedFrames);

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_DUP, 1, 2, frameqbufmgr_dup_iov, frameqbufmgr_dup_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_DUPV:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t frameqbufmgr_dupv_iov[3]; //TO DO!
			int j=0;

			SETIOV( &frameqbufmgr_dupv_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_dupv_iov[1], cargs->args.dupv.framePtr, cargs->args.dupv.numFrames * sizeof(UInt32));

			for (j = 0; j < cargs->args.dupv.numFrames; j++)
					SETIOV( &frameqbufmgr_dupv_iov[j + 2], cargs->args.dupv.dupedFramePtr[j], cargs->args.dupv.numDupedFrames * sizeof(UInt32));

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_DUPV, 2, j+1, frameqbufmgr_dupv_iov, frameqbufmgr_dupv_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_SET_NOTIFYID:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_SET_NOTIFYID, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_RESET_NOTIFYID:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_RESET_NOTIFYID, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_TRANSLATE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_TRANSLATE, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_shmem_params_init_iov[2];

			SETIOV( &frameqbufmgr_shmem_params_init_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_shmem_params_init_iov[1], cargs->args.ParamsInit.params, sizeof (FrameQBufMgr_ShMem_Params));

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT, 1, 2, frameqbufmgr_shmem_params_init_iov, frameqbufmgr_shmem_params_init_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_get_vnumfreeframes_iov[3];

			SETIOV( &frameqbufmgr_get_vnumfreeframes_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_get_vnumfreeframes_iov[1], cargs->args.getvNumFreeFrames.numFreeFrames, sizeof (UInt32) * cargs->args.getvNumFreeFrames.numFreeQids);
			SETIOV( &frameqbufmgr_get_vnumfreeframes_iov[2], cargs->args.getvNumFreeFrames.freeQId, cargs->args.getvNumFreeFrames.numFreeQids * sizeof (UInt32));


			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES, 2, 2, frameqbufmgr_get_vnumfreeframes_iov, frameqbufmgr_get_vnumfreeframes_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_DELETE:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_DELETE, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_GETID:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_GETID, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_FREEV:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_freev_iov[2];

			SETIOV( &frameqbufmgr_freev_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_freev_iov[1], cargs->args.freev.aframePtr, cargs->args.freev.numFrames * sizeof(UInt32));

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_FREEV, 2, 1, frameqbufmgr_freev_iov, frameqbufmgr_freev_iov, NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_REG_NOTIFIER:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_REG_NOTIFIER, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_UNREG_NOTIFIER:
		{
			osStatus = devctl( IpcDrv_handle, DCMD_FRAMEQBUFMGR_UNREG_NOTIFIER, args, sizeof(FrameQBufMgrDrv_CmdArgs), NULL);
			break;
		}

		case CMD_FRAMEQBUFMGR_CONTROL:
		{
			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *)args;
			iov_t	frameqbufmgr_control_iov[2];

			SETIOV( &frameqbufmgr_control_iov[0], cargs, sizeof(FrameQBufMgrDrv_CmdArgs) );
			SETIOV( &frameqbufmgr_control_iov[1], cargs->args.control.arg, cargs->args.control.size);

			osStatus = devctlv( IpcDrv_handle, DCMD_FRAMEQBUFMGR_CONTROL, 2, 1, frameqbufmgr_control_iov, frameqbufmgr_control_iov, NULL);
			break;
		}

	 	default:
		{
			  /* This does not impact return status of this function, so retVal
			   * comment is not used.
			   */
			  status = FrameQBufMgr_E_INVALIDARG;
			  GT_setFailureReason (curTrace,
								   GT_4CLASS,
								   "FrameQBufMgrDrv_ioctl",
								   status,
								   "Unsupported ioctl command specified");
			  break;
		}

	}

	if (osStatus != 0) {
        /*! @retval FrameQBufMgr_E_OSFAILURE Driver ioctl failed */
        status = FrameQBufMgr_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgrDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((FrameQBufMgrDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgrDrv_ioctl", status);

    return (status);
}
