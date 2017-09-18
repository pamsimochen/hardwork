/*
 *  @file   SharedRegionDrv.c
 *
 *  @brief      OS-specific implementation of SharedRegion driver for Qnx
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
#include <ti/ipc/MultiProc.h>

/* Module specific header files */
#include <ti/syslink/utils/Gate.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/SharedRegionDrvDefs.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>

/* C/posix header files */
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
 *  @brief  Driver name for SharedRegion.
 */
#define SHAREDREGION_DRIVER_NAME     "/dev/syslinkipc/SharedRegion"


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
 *  @brief  Function to open the SharedRegion driver.
 *
 *  @sa     SharedRegionDrv_close
 */
Int
SharedRegionDrv_open (Void)
{
    Int status      = SharedRegion_S_SUCCESS;

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
    int                       osStatus = -1;
    SharedRegionDrv_CmdArgs * cargs    = (SharedRegionDrv_CmdArgs *) args;
    SharedRegion_Region    *  regions  = NULL;
    SharedRegion_Config *     config;
    Memory_MapInfo            mapInfo;
    UInt16                    i;

    GT_2trace (curTrace, GT_ENTER, "SharedRegionDrv_ioctl", cmd, args);

/*  TODO: SharedRegionDrv_refCount not defined in QNX build
 *  GT_assert (curTrace, (SharedRegionDrv_refCount > 0));
 */

    switch (cmd) {

        case CMD_SHAREDREGION_GETCONFIG:
        {
			SharedRegionDrv_CmdArgs *cargs = (SharedRegionDrv_CmdArgs *)args;
			iov_t sharedregion_getconfig_iov[2];
			SharedRegion_Config * shreg_config = cargs->args.getConfig.config;

			SETIOV( &sharedregion_getconfig_iov[0], cargs, sizeof(SharedRegionDrv_CmdArgs) );
			SETIOV( &sharedregion_getconfig_iov[1], cargs->args.getConfig.config, sizeof(SharedRegion_Config) );

			/* the osal_drv handle is used instead of ipcdrv_handle as the ipcdrc_handle is not yet initialized */
			osStatus = devctlv( OsalDrv_handle, DCMD_SHAREDREGION_GETCONFIG, 1, 2, sharedregion_getconfig_iov, sharedregion_getconfig_iov, NULL);

			if ( osStatus == 0 ){
				cargs->args.getConfig.config = shreg_config;
			}
        }
        break;

        case CMD_SHAREDREGION_SETUP:
        {
			SharedRegionDrv_CmdArgs *cargs = (SharedRegionDrv_CmdArgs *)args;
			iov_t sharedregion_setup_iov[3];

			SETIOV( &sharedregion_setup_iov[0], cargs, sizeof(SharedRegionDrv_CmdArgs) );
			SETIOV( &sharedregion_setup_iov[1], cargs->args.setup.config, sizeof(SharedRegion_Config) );
			SETIOV( &sharedregion_setup_iov[2], cargs->args.setup.regions, cargs->args.setup.config->numEntries * sizeof(SharedRegion_Region) );

			osStatus = devctlv( IpcDrv_handle, DCMD_SHAREDREGION_SETUP, 2, 3, sharedregion_setup_iov, sharedregion_setup_iov, NULL);
        }
        break;


        case CMD_SHAREDREGION_GETREGIONINFO:
        {
			SharedRegionDrv_CmdArgs *cargs = (SharedRegionDrv_CmdArgs *)args;
			iov_t sharedregion_getregconfig_iov[2];	/* no of max shared region entries + 1 */
            //UInt16              i=0;
            SharedRegion_Config   config;

            SharedRegion_getConfig (&config);
			SETIOV( &sharedregion_getregconfig_iov[0], cargs, sizeof(SharedRegionDrv_CmdArgs) );
			SETIOV( &sharedregion_getregconfig_iov[1], cargs->args.getRegionInfo.regions, (sizeof(SharedRegion_Region) * config.numEntries));

			//for (i = 1; i< cargs->args.getConfig.config->numEntries+1;i++) {
			//	SETIOV( &sharedregion_getregconfig_iov[i], &cargs->args.setup.regions[i-1], sizeof(SharedRegion_Region) );
			//}

//			SETIOV( &sharedregion_getregconfig_iov[1], cargs->args.setup.regions, (sizeof(SharedRegion_Region) * cargs->args.getConfig.config->numEntries));

			//osStatus = devctlv( IpcDrv_handle, DCMD_SHAREDREGION_GETREGIONINFO, 1, cargs->args.getConfig.config->numEntries + 1, sharedregion_getregconfig_iov, sharedregion_getregconfig_iov, NULL);
			osStatus = devctlv( IpcDrv_handle, DCMD_SHAREDREGION_GETREGIONINFO, 2, 2, sharedregion_getregconfig_iov, sharedregion_getregconfig_iov, NULL);
        }
        break;

        case CMD_SHAREDREGION_DESTROY:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_DESTROY, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_START:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_START, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_STOP:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_STOP, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_ATTACH:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_ATTACH, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_DETACH:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_DETACH, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_SETENTRY:
        {

			SharedRegionDrv_CmdArgs *cargs = (SharedRegionDrv_CmdArgs *)args;

			iov_t sharedregion_getregsetentry_iov[2];

			SETIOV( &sharedregion_getregsetentry_iov[0], cargs, sizeof(SharedRegionDrv_CmdArgs) );
			SETIOV( &sharedregion_getregsetentry_iov[1], &cargs->args.setEntry.entry, sizeof(SharedRegion_Entry) );

			osStatus = devctlv( IpcDrv_handle, DCMD_SHAREDREGION_SETENTRY, 2, 1, sharedregion_getregsetentry_iov, sharedregion_getregsetentry_iov, NULL);
		}
        break;

        case CMD_SHAREDREGION_CLEARENTRY:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_CLEARENTRY, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_GETHEAP:
        {

			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_GETHEAP, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_RESERVEMEMORY:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_RESERVEMEMORY, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        case CMD_SHAREDREGION_CLEARRESERVEDMEMORY:
        {
			osStatus = devctl( IpcDrv_handle, DCMD_SHAREDREGION_CLEARRESERVEDMEMORY, cargs, sizeof(SharedRegionDrv_CmdArgs), NULL);
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = SharedRegion_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SharedRegionDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    if (osStatus != 0) {
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
        status = ((SharedRegionDrv_CmdArgs *) cargs)->apiStatus;

        /* Convert the base address to user virtual address */
        if (cmd == CMD_SHAREDREGION_SETUP) {
            config = cargs->args.setup.config;
            for (i = 0u; (   (i < config->numEntries) && (status >= 0)); i++) {
                regions = &(cargs->args.setup.regions [i]);
                if (regions->entry.isValid == TRUE) {
                    mapInfo.src  = (UInt32) regions->entry.base;
                    mapInfo.size = regions->entry.len;
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
        else {
            /* TBD: Need to do Memory_unmap in destroy. */
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "SharedRegionDrv_ioctl", status);

/*! @retval SharedRegion_S_SUCCESS Operation successfully completed. */
    return status;
}
