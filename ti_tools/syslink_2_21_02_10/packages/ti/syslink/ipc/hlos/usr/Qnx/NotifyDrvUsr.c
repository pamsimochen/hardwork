/*
 *  @file   NotifyDrvUsr.c
 *
 *  @brief      User-side OS-specific implementation of Notify driver for Qnx
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
#include <ti/syslink/inc/usr/NotifyDrvUsr.h>
#include <ti/syslink/inc/NotifyDrvDefs.h>
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
#include <pthread.h>


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver name for Notify.
 */
#define NOTIFY_DRIVER_NAME         "/dev/syslinkipc/Notify"


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Reference count for the driver handle.
 */
static UInt32 NotifyDrvUsr_refCount = 0;

/*!
 *  @brief  Thread handler for event receiver thread.
 */
static pthread_t  NotifyDrv_workerThread;

extern Int32 IpcDrv_handle;

extern Int32 OsalDrv_handle;

/** ============================================================================
 *  Forward declaration of internal functions
 *  ============================================================================
 */
/*!
 *  @brief      This is the worker thread for polling on events.
 *
 *  @param      arg module attributes
 */
Void _NotifyDrvUsr_eventWorker(Void *arg);


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the Notify driver.
 *
 *  @param  createThread  Flag to indicate whether to create thread or not.
 *
 *  @sa     NotifyDrvUsr_close
 */
Int
NotifyDrvUsr_open (Bool createThread)
{
    Int    status   = Notify_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "NotifyDrvUsr_open", createThread);

    if (NotifyDrvUsr_refCount == 0) {
        /* TBD: Protection for refCount. */
        NotifyDrvUsr_refCount++;

        if (createThread == TRUE) {
            Notify_CmdArgsThreadAttach attachParams;
            attachParams.pid = getpid ();
            status = NotifyDrvUsr_ioctl (CMD_NOTIFY_THREADATTACH, &attachParams);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDrvUsr_close",
                                     status,
                                     "Notify attach failed on kernel "
                                     "side!");
            }
            else {
                /* Create the pthread */
                pthread_create (&NotifyDrv_workerThread,
                                NULL,
                                (Ptr) _NotifyDrvUsr_eventWorker,
                                NULL);
                if (NotifyDrv_workerThread == (UInt32) NULL) {
                    /*! @retval Notify_E_OSFAILURE Failed to create
                                                   Notify thread */
                    status = Notify_E_OSFAILURE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "NotifyDrvUsr_open",
                                         status,
                                         "Failed to create Notify "
                                         "thread!");
                }
            }
        }
    }
    else {
        /* TBD: Protection for refCount. */
        NotifyDrvUsr_refCount++;
    }

    GT_1trace (curTrace, GT_LEAVE, "NotifyDrvUsr_open", status);

    /*! @retval Notify_S_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief  Function to close the Notify driver.
 *
 *  @param  deleteThread  Flag to indicate whether to delete thread or not.
 *
 *  @sa     NotifyDrvUsr_open
 */
Int
NotifyDrvUsr_close (Bool deleteThread)
{
    Int    status      = Notify_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "NotifyDrvUsr_close", deleteThread);

    /* TBD: Protection for refCount. */
    if (NotifyDrvUsr_refCount == 1) {
        if (deleteThread == TRUE) {
        	Notify_CmdArgsThreadDetach detachParams;
            detachParams.pid = getpid ();
            status = NotifyDrvUsr_ioctl (CMD_NOTIFY_THREADDETACH, &detachParams);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "NotifyDrvUsr_close",
                                     status,
                                     "Notify detach failed on kernel side!");
            }

            pthread_join (NotifyDrv_workerThread, NULL);
        }
        NotifyDrvUsr_refCount--;
    }
    else {
        NotifyDrvUsr_refCount--;
    }

    GT_1trace (curTrace, GT_LEAVE, "NotifyDrvUsr_close", status);

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
NotifyDrvUsr_ioctl (UInt32 cmd, Ptr args)
{
    Int status      = Notify_S_SUCCESS;
    int osStatus    = -1;

    GT_2trace (curTrace, GT_ENTER, "NotifyDrvUsr_ioctl", cmd, args);

    GT_assert (curTrace, (NotifyDrvUsr_refCount > 0));

	 switch (cmd) {

			case CMD_NOTIFY_SENDEVENT:
			{
				 osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_SENDEVENT, args, sizeof(Notify_CmdArgsSendEvent), NULL);
			}
			break;

			case CMD_NOTIFY_DISABLE:
			{
				 osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_DISABLE, args, sizeof(Notify_CmdArgsDisable), NULL);
			}
			break;

			case CMD_NOTIFY_RESTORE:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_RESTORE, args, sizeof(Notify_CmdArgsRestore), NULL);
			}
			break;

			case CMD_NOTIFY_DISABLEEVENT:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_DISABLEEVENT, args, sizeof(Notify_CmdArgsDisableEvent), NULL);
			}
			break;

			case CMD_NOTIFY_ENABLEEVENT:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_ENABLEEVENT, args, sizeof(Notify_CmdArgsEnableEvent), NULL);
			}
			break;

			case CMD_NOTIFY_REGISTEREVENTSINGLE:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_REGISTEREVENTSINGLE, args, sizeof(Notify_CmdArgsRegisterEvent), NULL);
			}
			break;

			case CMD_NOTIFY_REGISTEREVENT:
			{

				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_REGISTEREVENT, args, sizeof(Notify_CmdArgsRegisterEvent), NULL);
			}
			break;

			case CMD_NOTIFY_UNREGISTEREVENTSINGLE:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_UNREGISTEREVENTSINGLE, args, sizeof(Notify_CmdArgsUnregisterEvent), NULL);
			}
			break;

			case CMD_NOTIFY_UNREGISTEREVENT:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_UNREGISTEREVENT, args, sizeof(Notify_CmdArgsUnregisterEvent), NULL);
			}
			break;

			case CMD_NOTIFY_GETCONFIG:
			{
				Notify_CmdArgsGetConfig *cargs = (Notify_CmdArgsGetConfig *)args;

				iov_t notify_getconfig_iov[2];

				SETIOV( &notify_getconfig_iov[0], cargs, sizeof(Notify_CmdArgsGetConfig) );
				SETIOV( &notify_getconfig_iov[1], cargs->cfg, sizeof(Notify_Config) );

				/* the osal_drv handle is used instead of ipcdrv_handle as the ipcdrc_handle is not yet initialized */
				osStatus = devctlv( OsalDrv_handle, DCMD_NOTIFY_GETCONFIG, 2, 2, notify_getconfig_iov, notify_getconfig_iov, NULL);
			}
			break;

			case CMD_NOTIFY_SETUP:
			{

				Notify_CmdArgsSetup *cargs = (Notify_CmdArgsSetup *)args;

				iov_t notify_setup_iov[2];

				SETIOV( &notify_setup_iov[0], cargs, sizeof(Notify_CmdArgsSetup) );
				SETIOV( &notify_setup_iov[1], cargs->cfg, sizeof(Notify_Config) );

				osStatus = devctlv( IpcDrv_handle, DCMD_NOTIFY_SETUP, 2, 2, notify_setup_iov, notify_setup_iov, NULL);
			}
			break;

			case CMD_NOTIFY_DESTROY:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_DESTROY, args, sizeof(Notify_CmdArgsDestroy), NULL);
			}
			break;
			case CMD_NOTIFY_ATTACH:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_ATTACH, args, sizeof(Notify_CmdArgsAttach), NULL);
			}
			break;
			case CMD_NOTIFY_DETACH:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_DETACH, args, sizeof(Notify_CmdArgsDetach), NULL);
			}
			break;
			case CMD_NOTIFY_SHAREDMEMREQ:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_SHAREDMEMREQ, args, sizeof(Notify_CmdArgsSharedMemReq), NULL);
			}
			break;

			case CMD_NOTIFY_INTLINEREGISTERED:
			{

				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_INTLINEREGISTERED, args, sizeof(Notify_CmdArgsIntLineRegistered), NULL);
			}
			break;

			case CMD_NOTIFY_EVENTAVAILABLE:
			{

				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_EVENTAVAILABLE, args, sizeof(Notify_CmdArgsEventAvailable), NULL);
			}
			break;

			case CMD_NOTIFY_THREADATTACH:
			{
				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_THREADATTACH, args, sizeof(Notify_CmdArgsThreadAttach), NULL);
			}
			break;

			case CMD_NOTIFY_THREADDETACH:
			{

				osStatus = devctl( IpcDrv_handle, DCMD_NOTIFY_THREADDETACH, args, sizeof(Notify_CmdArgsThreadDetach), NULL);
			}
			break;

			default:
			{
				/* This does not impact return status of this function, so retVal
				 * comment is not used.
				 */
				status = Notify_E_INVALIDARG;
				GT_setFailureReason (curTrace,
									 GT_4CLASS,
									 "NotifyDrv_ioctl",
									 status,
									 "Unsupported ioctl command specified");
			}
			break;
		}


    if (osStatus != 0) {
        /*! @retval Notify_E_OSFAILURE Driver ioctl failed */
        status = Notify_E_OSFAILURE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NotifyDrvUsr_ioctl",
                             status,
                             "Driver ioctl failed!");
    }
    else {
        /* First field in the structure is the API status. */
        status = ((Notify_CmdArgs *) args)->apiStatus; // TODO: THIS WILL NEED TO BE MODIFIED FOR IOCTL THAT DO NOT USE COMMOM CMD ARGS
    }

    GT_1trace(curTrace, GT_LEAVE, "NotifyDrvUsr_ioctl", status);

    return status;
}


/** ============================================================================
 *  Internal functions
 *  ============================================================================
 */
/*!
 *  @brief      This is the worker thread which polls for events.
 *
 *  @param      attrs module attributes
 *
 *  @sa
 */
Void
_NotifyDrvUsr_eventWorker (Void * arg)
{
    Int32                 status = Notify_S_SUCCESS;
    UInt32                nRead  = 0;
    NotifyDrv_EventPacket packet;
    sigset_t              blockSet;

    GT_1trace (curTrace, GT_ENTER, "_NotifyDrvUsr_eventWorker", arg);

    if (sigfillset (&blockSet) != 0) {
        perror ("Event worker thread error in sigfillset");
        pthread_exit (NULL);
    }

    if (pthread_sigmask (SIG_BLOCK, &blockSet, NULL) != 0) {
        perror ("Event worker thread error in setting sigmask");
        pthread_exit (NULL);
    }

    while (status >= 0) {
        memset (&packet, 0, sizeof (NotifyDrv_EventPacket));
        packet.pid = getpid ();
        nRead = read (IpcDrv_handle,
                      &packet,
                      sizeof (NotifyDrv_EventPacket));
        if (nRead == sizeof (NotifyDrv_EventPacket)) {
            /* check for termination packet */
            if (packet.isExit  == TRUE) {
                pthread_exit (NULL);
            }

            if (packet.func != NULL) {
                packet.func (packet.procId,
                             packet.lineId,
                             packet.eventId,
                             packet.param,
                             packet.data);
            }
        }
        else if (nRead == -1) {
            Osal_printf("_NotifyDrvUsr_eventWorker: nRead returned -1\n");
            break;
        }
    }

    GT_0trace (curTrace, GT_LEAVE, "_NotifyDrvUsr_eventWorker");
}
