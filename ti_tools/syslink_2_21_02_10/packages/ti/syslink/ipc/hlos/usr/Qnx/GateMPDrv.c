/*
 *  @file   GateMPDrv.c
 *
 *  @brief      OS-specific implementation of GateMP driver for Qnx
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

/* Module specific header files */
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/GateMPDrvDefs.h>
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
 *  @brief  Driver name for GateMP.
 */
#define GATEMP_DRIVER_NAME     "/dev/syslinkipc/GateMP"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for GateMP in this process.
 */

extern Int32 IpcDrv_handle;

extern Int32 OsalDrv_handle;

/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the GateMP driver.
 *
 *  @sa     GateMPDrv_close
 */
Int
GateMPDrv_open (Void)
{
    Int status      = GateMP_S_SUCCESS;

    return status;
}


/*!
 *  @brief  Function to close the GateMP driver.
 *
 *  @sa     GateMPDrv_open
 */
Int
GateMPDrv_close (Void)
{
    Int status      = GateMP_S_SUCCESS;

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
GateMPDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = GateMP_S_SUCCESS;
    int osStatus    = -1;

    GT_2trace (curTrace, GT_ENTER, "GateMPDrv_ioctl", cmd, args);

		switch (cmd) {

			case CMD_GATEMP_ENTER:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_ENTER, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_LEAVE:
			{
			osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_LEAVE, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_GETCONFIG:
			{
				  GateMPDrv_CmdArgs *cargs = (GateMPDrv_CmdArgs *)args;
				  iov_t	gatempgetconfig_iov[2];

				  SETIOV( &gatempgetconfig_iov[0], cargs, sizeof(GateMPDrv_CmdArgs) );
				  SETIOV( &gatempgetconfig_iov[1], cargs->args.getConfig.config, sizeof(GateMP_Config) );

				  /* the osal_drv handle is used instead of ipcdrv_handle as the ipcdrc_handle is not yet initialized */
				  osStatus = devctlv( OsalDrv_handle, DCMD_GATEMP_GETCONFIG, 1, 2, gatempgetconfig_iov, gatempgetconfig_iov, NULL);
			}
			break;

			case CMD_GATEMP_SETUP:
			{
				GateMPDrv_CmdArgs *cargs = (GateMPDrv_CmdArgs *)args;
				iov_t gatempsetup_iov[2];

				SETIOV( &gatempsetup_iov[0], cargs, sizeof(GateMPDrv_CmdArgs) );
				SETIOV( &gatempsetup_iov[1], cargs->args.setup.config, sizeof(GateMP_Config) );
				osStatus = devctlv( IpcDrv_handle, DCMD_GATEMP_SETUP, 2, 1, gatempsetup_iov, gatempsetup_iov, NULL);
			}
			break;

			case CMD_GATEMP_DESTROY:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_DESTROY, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_PARAMS_INIT:
			{
				GateMPDrv_CmdArgs *cargs = (GateMPDrv_CmdArgs *)args;
				iov_t gatempparamsinit_iov[2];

				SETIOV( &gatempparamsinit_iov[0], cargs, sizeof(GateMPDrv_CmdArgs) );
				SETIOV( &gatempparamsinit_iov[1], cargs->args.ParamsInit.params, sizeof(GateMP_Params) );
				osStatus = devctlv( IpcDrv_handle, DCMD_GATEMP_PARAMS_INIT, 2, 2, gatempparamsinit_iov, gatempparamsinit_iov, NULL);
			}
			break;

			case CMD_GATEMP_CREATE:
			{
				GateMPDrv_CmdArgs *cargs = (GateMPDrv_CmdArgs *)args;
				iov_t gatempcreate_iov[3];
				GateMP_Params * gatemp_params = cargs->args.create.params;

				SETIOV( &gatempcreate_iov[0], cargs, sizeof(GateMPDrv_CmdArgs) );
				SETIOV( &gatempcreate_iov[1], cargs->args.create.params, sizeof(GateMP_Params) );
				SETIOV( &gatempcreate_iov[2], gatemp_params->name, sizeof(char) * cargs->args.create.nameLen );
				osStatus = devctlv( IpcDrv_handle, DCMD_GATEMP_CREATE, 3, 1, gatempcreate_iov, gatempcreate_iov, NULL);
			}
			break;

			case CMD_GATEMP_DELETE:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_DELETE, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_OPEN:
			{
				GateMPDrv_CmdArgs *cargs = (GateMPDrv_CmdArgs *)args;
				iov_t gatempopen_iov[2];

				SETIOV( &gatempopen_iov[0], cargs, sizeof(GateMPDrv_CmdArgs) );
				SETIOV( &gatempopen_iov[1], cargs->args.open.name, sizeof(char) * cargs->args.open.nameLen );
				osStatus = devctlv( IpcDrv_handle, DCMD_GATEMP_OPEN, 2, 1, gatempopen_iov, gatempopen_iov, NULL);
			}
			break;

			case CMD_GATEMP_OPENBYADDR:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_OPENBYADDR, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_CLOSE:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_CLOSE, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_SHAREDMEMREQ:
			{
				GateMPDrv_CmdArgs *cargs = (GateMPDrv_CmdArgs *)args;
				iov_t gatempshm_iov[2];

				SETIOV( &gatempshm_iov[0], cargs, sizeof(GateMPDrv_CmdArgs) );
				SETIOV( &gatempshm_iov[1], cargs->args.sharedMemReq.params, sizeof(GateMP_Params) );
				osStatus = devctlv( IpcDrv_handle, DCMD_GATEMP_SHAREDMEMREQ, 2, 1, gatempshm_iov, gatempshm_iov, NULL);
			}
			break;

			case CMD_GATEMP_GETDEFAULTREMOTE:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_GETDEFAULTREMOTE, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			case CMD_GATEMP_GETSHAREDADDR:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_GATEMP_GETSHAREDADDR, args, sizeof(GateMPDrv_CmdArgs), NULL);
			}
			break;

			default:
			{
				/* This does not impact return status of this function, so retVal
				 * comment is not used.
				 */
				status = GateMP_E_INVALIDARG;
				GT_setFailureReason (curTrace,
									 GT_4CLASS,
									 "GateMPDrv_drvioctl",
									 status,
									 "Unsupported ioctl command specified");
			}
			break;
		}


    if (osStatus != 0) {
    /*! @retval GateMP_E_OSFAILURE Driver ioctl failed */
        status = GateMP_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMPDrv_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((GateMPDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMPDrv_ioctl", status);

    return status;
}
