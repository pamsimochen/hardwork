/*
 *  @file   ClientNotifyMgrDrv.c
 *
 *  @brief      OS-specific implementation of FraemQ driver for Qnx
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
#include <ti/syslink/inc/ClientNotifyMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgrDrvDefs.h>

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


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver name for ClientNotifyMgr.
 */
#define CLIENTNOTIFYMGR_DRVIER_NAME     "/dev/syslinkipc/ClientNotifyMgr"


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
 *  @brief  Function to open the ClientNotifyMgr driver.
 *
 *  @sa     GatePetersonDrv_close
 */
Int
ClientNotifyMgrDrv_open (Void)
{
    Int status      = ClientNotifyMgr_S_SUCCESS;

    return status;
}


/*!
 *  @brief  Function to close the ClientNotifyMgr driver.
 *
 *  @sa     ClientNotifyMgrDrv_open
 */
Int
ClientNotifyMgrDrv_close (Void)
{
    Int status      = ClientNotifyMgr_S_SUCCESS;

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
ClientNotifyMgrDrv_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = ClientNotifyMgr_S_SUCCESS;
    int osStatus    = -1;
    ClientNotifyMgrDrv_CmdArgs *cargs = (ClientNotifyMgrDrv_CmdArgs *)args;

    GT_2trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_ioctl", cmd, args);

    switch (cmd) {

        case CMD_CLIENTNOTIFYMGR_CREATE:
        {
            iov_t       clientnotifymgr_create_iov[3];
            Int numIovItems = 2;

            SETIOV( &clientnotifymgr_create_iov[0], cargs, sizeof(ClientNotifyMgrDrv_CmdArgs) );
            SETIOV( &clientnotifymgr_create_iov[1], cargs->args.create.params, sizeof(ClientNotifyMgr_Params) );
            if (cargs->args.create.nameLen > 0) {
                SETIOV( &clientnotifymgr_create_iov[2], cargs->args.create.params->name, cargs->args.create.nameLen );
                numIovItems++;
            }
            osStatus = devctlv( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_CREATE, numIovItems, 1, clientnotifymgr_create_iov, clientnotifymgr_create_iov, NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_GETCONFIG:
        {
            iov_t   clientnotifymgr_getconfig_iov[2];

            SETIOV( &clientnotifymgr_getconfig_iov[0], cargs, sizeof(ClientNotifyMgrDrv_CmdArgs) );
            SETIOV( &clientnotifymgr_getconfig_iov[1], cargs->args.getConfig.config, sizeof(ClientNotifyMgr_Config) );

            osStatus = devctlv( OsalDrv_handle, DCMD_CLIENTNOTIFYMGR_GETCONFIG, 1, 2, clientnotifymgr_getconfig_iov, clientnotifymgr_getconfig_iov, NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_SETUP:
        {
            iov_t   clientnotifymgr_setup_iov[2];

            SETIOV( &clientnotifymgr_setup_iov[0], cargs, sizeof(ClientNotifyMgrDrv_CmdArgs) );
            SETIOV( &clientnotifymgr_setup_iov[1], cargs->args.setup.config, sizeof(ClientNotifyMgr_Config) );

            osStatus = devctlv( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_SETUP, 2, 1, clientnotifymgr_setup_iov, clientnotifymgr_setup_iov, NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_DESTROY:
        {
            osStatus = devctl( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_DESTROY, cargs, sizeof(ClientNotifyMgrDrv_CmdArgs), NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_PARAMS_INIT:
        {
            iov_t   clientnotifymgr_params_init_iov[2];

            SETIOV( &clientnotifymgr_params_init_iov[0], cargs, sizeof(ClientNotifyMgrDrv_CmdArgs) );
            SETIOV( &clientnotifymgr_params_init_iov[1], cargs->args.ParamsInit.params, sizeof(ClientNotifyMgr_Params) );

            osStatus = devctlv( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_PARAMS_INIT, 1, 2, clientnotifymgr_params_init_iov, clientnotifymgr_params_init_iov, NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_DELETE:
        {
            osStatus = devctl( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_DELETE, cargs, sizeof(ClientNotifyMgrDrv_CmdArgs), NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_CLOSE:
        {
            osStatus = devctl( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_CLOSE, cargs, sizeof(ClientNotifyMgrDrv_CmdArgs), NULL);
        }
        break;
        case CMD_CLIENTNOTIFYMGR_SHAREDMEMREQ:
        {
            iov_t   clientnotifymgr_sjaredmemreq_iov[2];

            SETIOV( &clientnotifymgr_sjaredmemreq_iov[0], cargs, sizeof(ClientNotifyMgrDrv_CmdArgs) );
            SETIOV( &clientnotifymgr_sjaredmemreq_iov[1], cargs->args.sharedMemReq.params, sizeof(ClientNotifyMgr_Params) );

            osStatus = devctlv( IpcDrv_handle, DCMD_CLIENTNOTIFYMGR_SHAREDMEMREQ, 2, 1, clientnotifymgr_sjaredmemreq_iov, clientnotifymgr_sjaredmemreq_iov, NULL);
        }
        break;

        default:
        {
          /* This does not impact return status of this function, so retVal
           * comment is not used.
           */
          status = ClientNotifyMgr_E_INVALIDARG;
          GT_setFailureReason (curTrace,
                               GT_4CLASS,
                               "ClientNotifyMgrDrv_ioctl",
                               status,
                               "Unsupported ioctl command specified");
        }
        break;
    }


    if (osStatus != 0) {
        /*! @retval CLIENTNOTIFYMGR_E_OSFAILURE Driver ioctl failed */
        status = ClientNotifyMgr_E_OSFAILURE;
        GT_setFailureReason (curTrace, GT_4CLASS, "ClientNotifyMgrDrv_ioctl",
                status, "Driver ioctl failed");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((ClientNotifyMgrDrv_CmdArgs *) args)->apiStatus;
    }

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_ioctl", status);

    return (status);
}
