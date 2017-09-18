/*
 *  @file   ProcMgrDrvUsr.c
 *
 *  @brief      User-side OS-specific implementation of ProcMgr driver for Linux
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



/* Linux specific header files */
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/inc/usr/ProcMgrDrvUsr.h>

/* Module headers */
#include <ti/syslink/inc/ProcMgrDrvDefs.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver name for ProcMgr.
 */
#define ProcMgr_DRIVER_NAME         "/dev/syslinkipc"
#define ALIGN(x,b) (((x) + (b) - 1) & (~(b)))


/** ============================================================================
 *  Globals
 *  ============================================================================
 */
/*!
 *  @brief  Driver handle for ProcMgr in this process.
 */

extern Int32 IpcDrv_handle;

extern Int32 OsalDrv_handle;

/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Function to open the ProcMgr driver.
 *
 *  @sa     ProcMgrDrvUsr_close
 */
Int
ProcMgrDrvUsr_open (Void)
{
    Int status      = ProcMgr_S_SUCCESS;

#if 0
    GT_0trace (curTrace, GT_ENTER, "ProcMgrDrvUsr_open");
    if (ProcMgrDrvUsr_refCount == 0) {
        /* TBD: Protection for refCount. */
        ProcMgrDrvUsr_refCount++;

        ProcMgrDrvUsr_handle = open (ProcMgr_DRIVER_NAME, O_SYNC | O_RDWR);
        if (ProcMgrDrvUsr_handle < 0) {
            perror ("ProcMgr driver open: " ProcMgr_DRIVER_NAME);
            /*! @retval ProcMgr_E_OSFAILURE Failed to open ProcMgr driver with
                        OS */
            status = ProcMgr_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrvUsr_open",
                                 status,
                                 "Failed to open ProcMgr driver with OS!");
        }
        else {
            osStatus = fcntl (ProcMgrDrvUsr_handle, F_SETFD, FD_CLOEXEC);
            if (osStatus != 0) {
                /*! @retval ProcMgr_E_OSFAILURE Failed to set file descriptor
                                                flags */
                status = ProcMgr_E_OSFAILURE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ProcMgrDrvUsr_open",
                                     status,
                                     "Failed to set file descriptor flags!");
            }
        }
    }
    else {
        ProcMgrDrvUsr_refCount++;
    }

    GT_1trace (curTrace, GT_LEAVE, "ProcMgrDrvUsr_open", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successfully completed. */
#endif
    return status;
}


/*!
 *  @brief  Function to close the ProcMgr driver.
 *
 *  @sa     ProcMgrDrvUsr_open
 */
Int
ProcMgrDrvUsr_close (Void)
{
    Int status      = ProcMgr_S_SUCCESS;
#if 0
    int osStatus    = 0;

    GT_0trace (curTrace, GT_ENTER, "ProcMgrDrvUsr_close");

    /* TBD: Protection for refCount. */
    ProcMgrDrvUsr_refCount--;
    if (ProcMgrDrvUsr_refCount == 0) {
        osStatus = close (ProcMgrDrvUsr_handle);
        if (osStatus != 0) {
            perror ("ProcMgr driver close: " ProcMgr_DRIVER_NAME);
            /*! @retval ProcMgr_E_OSFAILURE Failed to open ProcMgr driver
                                            with OS */
            status = ProcMgr_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrvUsr_close",
                                 status,
                                 "Failed to close ProcMgr driver with OS!");
        }
        else {
            ProcMgrDrvUsr_handle = 0;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "ProcMgrDrvUsr_close", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successfully completed. */
#endif
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
Int ProcMgrDrvUsr_ioctl(UInt32 cmd, Ptr args)
{
    Int     status = ProcMgr_S_SUCCESS;
    int     osStatus = 0;

    GT_2trace(curTrace, GT_ENTER, "ProcMgrDrvUsr_ioctl", cmd, args);

#if 0
    if (ProcMgrDrvUsr_handle < 0) {
        /* Need to open the driver handle. It was not opened from this process. */
        driverOpened = TRUE;
        status = ProcMgrDrvUsr_open ();
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrvUsr_ioctl",
                                 status,
                                 "Failed to open OS driver handle!");
        }
    }
#endif
//    GT_assert (curTrace, (ProcMgrDrvUsr_refCount > 0));

    if (status >= 0) {
        switch(cmd) {

            case CMD_PROCMGR_ATTACH: {
                iov_t iov[2];
                ProcMgr_CmdArgsAttach *cargs = (ProcMgr_CmdArgsAttach *)args;

                SETIOV(&iov[0], cargs, sizeof(ProcMgr_CmdArgsAttach));
                SETIOV(&iov[1], cargs->params, sizeof(ProcMgr_AttachParams));

                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_ATTACH, 2, 1,
                    iov, iov, NULL);
            }
            break;

            case CMD_PROCMGR_DELETE: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_DELETE,
                    (ProcMgr_CmdArgsDelete *)args,
                    sizeof(ProcMgr_CmdArgsDelete), NULL);
            }
            break;

            case CMD_PROCMGR_DETACH: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_DETACH,
                    (ProcMgr_CmdArgsDetach*) args,
                    sizeof(ProcMgr_CmdArgsDetach), NULL);
            }
            break;

            case CMD_PROCMGR_CLOSE: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_CLOSE,
                    (ProcMgr_CmdArgsClose*) args,
                    sizeof(ProcMgr_CmdArgsClose), NULL);
            }
            break;

            case CMD_PROCMGR_CREATE: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_CREATE,
                    (ProcMgr_CmdArgsCreate*) args,
                    sizeof(ProcMgr_CmdArgsCreate), NULL);
            }
            break;

            case CMD_PROCMGR_CONTROL: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_CONTROL,
                    (ProcMgr_CmdArgsControl*) args,
                    sizeof(ProcMgr_CmdArgsControl), NULL);
            }
            break;

            case CMD_PROCMGR_DESTROY: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_DESTROY,
                    (ProcMgr_CmdArgsDestroy*) args,
                    sizeof(ProcMgr_CmdArgsDestroy), NULL);
            }
            break;

            case CMD_PROCMGR_GETCONFIG: {
                ProcMgr_CmdArgsGetConfig *cargs =
                    (ProcMgr_CmdArgsGetConfig *)args;
                iov_t proc_iov[2];
                SETIOV( &proc_iov[0], args, sizeof(ProcMgr_CmdArgsGetConfig) );
                SETIOV( &proc_iov[1], cargs->cfg, sizeof(ProcMgr_Config));

                /* the osal_drv handle is used instead of ipcdrv_handle as
                 * the ipcdrc_handle is not yet initialized */
                osStatus = devctlv(OsalDrv_handle, DCMD_PROCMGR_GETCONFIG,
                    1, 2, proc_iov, proc_iov, NULL);
            }
            break;

            case CMD_PROCMGR_GETPROCINFO: {
                ProcMgr_CmdArgsGetProcInfo *cargs =
                    (ProcMgr_CmdArgsGetProcInfo *)args;
                iov_t proc_iov[2];
                SETIOV( &proc_iov[0], args, sizeof(ProcMgr_CmdArgsGetProcInfo) );
                SETIOV( &proc_iov[1], cargs->procInfo, sizeof(ProcMgr_ProcInfo)
                		+ (cargs->maxMemEntries *
                        sizeof(ProcMgr_MappedMemEntry)));

                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_GETPROCINFO,
                    1, 2, proc_iov, proc_iov, NULL);
            }
            break;

            case CMD_PROCMGR_GETATTACHPARAMS: {
                iov_t iov[2];
                ProcMgr_CmdArgsGetAttachParams * cargs =
                    (ProcMgr_CmdArgsGetAttachParams*) args;

                SETIOV(&iov[0], cargs, sizeof(ProcMgr_CmdArgsGetAttachParams));
                SETIOV( &iov[1], cargs->params, sizeof(ProcMgr_AttachParams));

                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_GETATTACHPARAMS,
                    1, 2, iov, iov, NULL);
            }
            break;

            case CMD_PROCMGR_GETSECTIONDATA: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_GETATTACHPARAMS,
                    (ProcMgr_CmdArgsGetSectionData*) args,
                    sizeof(ProcMgr_CmdArgsGetSectionData), NULL);
            }
            break;

            case CMD_PROCMGR_GETSECTIONINFO: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_GETSECTIONINFO,
                    (ProcMgr_CmdArgsGetSectionInfo*) args,
                    sizeof(ProcMgr_CmdArgsGetSectionInfo), NULL);
            }
            break;

            case CMD_PROCMGR_GETSTARTPARAMS: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_GETSTARTPARAMS,
                    (ProcMgr_CmdArgsGetStartParams*) args,
                    sizeof(ProcMgr_CmdArgsGetStartParams), NULL);
            }
            break;

            case CMD_PROCMGR_GETSTATE: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_GETSTATE,
                    (ProcMgr_CmdArgsGetState*) args,
                    sizeof(ProcMgr_CmdArgsGetState), NULL);
            }
            break;

            case CMD_PROCMGR_GETSYMBOLADDRESS: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_GETSYMBOLADDR,
                    (ProcMgr_CmdArgsGetSymbolAddress*) args,
                    sizeof(ProcMgr_CmdArgsGetSymbolAddress), NULL);
            }
            break;

            case CMD_PROCMGR_LOAD: {
                ProcMgr_CmdArgsLoad *cargs;
                Int sp, i, len;
                SizeT iovsz, size;
                iov_t *iov;

                cargs = (ProcMgr_CmdArgsLoad *)args;
                sp = (cargs->argc > 0 ? (cargs->argc + 3) : 2);
                iovsz = sp * sizeof(iov_t);
                iov = (iov_t *)Memory_calloc(NULL, iovsz, 4, NULL);

                /* compute size on word boundary */
                size = (sizeof(ProcMgr_CmdArgsLoad) + 3) & ~0x3;
                SETIOV(&iov[0], cargs, size);

                /* compute length on word boundary */
                size = (cargs->imagePathLen + 3) & ~0x3;
                SETIOV(&iov[1], cargs->imagePath, size);

                if (cargs->argc > 0) {
                    /* compute array size on word boundary */
                    size = (((cargs->argc + 1) * sizeof(char *)) + 3) & ~0x3;
                    SETIOV(&iov[2], cargs->argv, size);

                    /* marshal strings, size is unaligned */
                    for (i = 0; i < cargs->argc; i++) {
                        len = String_len(cargs->argv[i]) + 1;
                        SETIOV(&iov[i+3], cargs->argv[i], len);
                    }
                }

                /* TODO: how to marshal params? */

                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_LOAD,
                    sp, 1, iov, iov, NULL);

                Memory_free(NULL, (Ptr)iov, iovsz);
            }
            break;

            case CMD_PROCMGR_UNLOAD: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_UNLOAD,
                    (ProcMgr_CmdArgsUnload*) args,
                    sizeof(ProcMgr_CmdArgsUnload), NULL);
            }
            break;

            case CMD_PROCMGR_MAP: {
                iov_t iov[2];
                ProcMgr_CmdArgsMap *cargs = (ProcMgr_CmdArgsMap *)args;

                SETIOV(&iov[0], cargs, sizeof(ProcMgr_CmdArgsMap));
                SETIOV(&iov[1], cargs->addrInfo, sizeof(ProcMgr_AddrInfo));
                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_MAP, 2, 1,
                    iov, iov, NULL);
            }
            break;

            case CMD_PROCMGR_UNMAP: {
                iov_t iov[2];
                ProcMgr_CmdArgsUnmap *cargs = (ProcMgr_CmdArgsUnmap *)args;

                SETIOV(&iov[0], cargs, sizeof(ProcMgr_CmdArgsUnmap));
                SETIOV(&iov[1], cargs->addrInfo, sizeof(ProcMgr_AddrInfo));

                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_UNMAP,
                    2, 1, iov, iov, NULL);
            }
            break;

            case CMD_PROCMGR_OPEN: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_OPEN,
                    (ProcMgr_CmdArgsOpen*) args, sizeof(ProcMgr_CmdArgsOpen),
                    NULL);
            }
            break;

            case CMD_PROCMGR_READ: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_READ,
                    (ProcMgr_CmdArgsRead*) args,
                    sizeof(ProcMgr_CmdArgsRead), NULL);
            }
            break;

            case CMD_PROCMGR_WRITE: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_WRITE,
                    (ProcMgr_CmdArgsWrite*) args,
                    sizeof(ProcMgr_CmdArgsWrite), NULL);
            }
            break;

            case CMD_PROCMGR_SETUP: {
                ProcMgr_CmdArgsSetup *cargs = (ProcMgr_CmdArgsSetup *)args;
                iov_t proc_iov[2];
                SETIOV( &proc_iov[0], args, sizeof(ProcMgr_CmdArgsSetup) );
                SETIOV( &proc_iov[1], cargs->cfg, sizeof(ProcMgr_Config) );
                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_SETUP,
                    2, 1, proc_iov, proc_iov, NULL);
            }
            break;

            case CMD_PROCMGR_START: {
                ProcMgr_CmdArgsStart *cargs = (ProcMgr_CmdArgsStart *)args;
                iov_t proc_iov[2];
                SETIOV( &proc_iov[0], args, sizeof(ProcMgr_CmdArgsStart) );
                SETIOV( &proc_iov[1], cargs->params, sizeof(ProcMgr_Params) );
                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_START,
                    2, 1, proc_iov, proc_iov, NULL);
            }
            break;

            case CMD_PROCMGR_STOP: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_STOP,
                    (ProcMgr_CmdArgsStop*) args,
                    sizeof(ProcMgr_CmdArgsStop), NULL);
            }
            break;

            case CMD_PROCMGR_TRANSLATEADDR: {
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_TRANSLATEADDR,
                    (ProcMgr_CmdArgsTranslateAddr*) args,
                    sizeof(ProcMgr_CmdArgsTranslateAddr), NULL);
            }
            break;

            case CMD_PROCMGR_PARAMS_INIT: {
                ProcMgr_CmdArgsParamsInit *cargs =
                    (ProcMgr_CmdArgsParamsInit *)args;
                iov_t proc_iov[2];
                SETIOV( &proc_iov[0], args, sizeof(ProcMgr_CmdArgsParamsInit) );
                SETIOV( &proc_iov[1], cargs->params, sizeof(ProcMgr_Params) );
                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_PARAMS_INIT,
                    2, 1, proc_iov, proc_iov, NULL);
            }
            break;

            case CMD_PROCMGR_GETLOADEDFILEID:
            {
#if 0
                ProcMgr_CmdArgsGetLoadedFileId *cargs =
                    (ProcMgr_CmdArgsGetLoadedFileId *)args;
                iov_t proc_iov[2];
                SETIOV(&proc_iov[0], args,
                    sizeof(ProcMgr_CmdArgsGetLoadedFileId) );
                SETIOV(&proc_iov[1], cargs->params, sizeof(ProcMgr_Params) );
                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_PARAMS_INIT,
                    2, 1, proc_iov, proc_iov, NULL);
#endif
                osStatus = devctl(IpcDrv_handle, DCMD_PROCMGR_GETLOADEDFILEID,
                    (ProcMgr_CmdArgsGetLoadedFileId*) args,
                    sizeof(ProcMgr_CmdArgsGetLoadedFileId), NULL);
            }
            break;

            case CMD_PROCMGR_CONFIGSYSMEMMAP: {
                ProcMgr_CmdArgsSetup *cargs = (ProcMgr_CmdArgsSetup *)args;
                iov_t proc_iov[5];

                SETIOV( &proc_iov[0], args, sizeof(ProcMgr_CmdArgsSetup) );
                SETIOV( &proc_iov[1], cargs->cfg, sizeof(ProcMgr_Config) );
                SETIOV( &proc_iov[2], cargs->cfg->sysMemMap,
                    sizeof(SysLink_MemoryMap) );
                SETIOV( &proc_iov[3], cargs->cfg->sysMemMap->memBlocks,
                    (sizeof(SysLink_MemEntry_Block) *
                    cargs->cfg->sysMemMap->numBlocks) );
                SETIOV(&proc_iov[4], cargs->cfg->params, cargs->cfg->paramsLen);

                osStatus = devctlv(IpcDrv_handle, DCMD_PROCMGR_CONFIGSYSMEMMAP,
                    5, 1, proc_iov, proc_iov, NULL);
            }
            break;

            default:
                fprintf(stderr,
                    "USER:Invalid DEVCTL command for ProcMgr %x \n", cmd);
                break;

        } /* switch */

        if (osStatus < 0) {
            /*! @retval ProcMgr_E_OSFAILURE Driver ioctl failed */
            status = ProcMgr_E_OSFAILURE;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrvUsr_ioctl",
                                 status,
                                 "Driver ioctl failed!");

        }
        else {
            /* First field in the structure is the API status. */
            status = ((ProcMgr_CmdArgs *) args)->apiStatus;
        }

        GT_1trace (curTrace,
                   GT_1CLASS,
                   "    ProcMgrDrvUsr_ioctl: API Status [0x%x]",
                   status);
    }

#if 0
    if (driverOpened == TRUE) {
        /* If the driver was temporarily opened here, close it. */
        tmpStatus = ProcMgrDrvUsr_close ();
        if ((status > 0) && (tmpStatus < 0)) {
            status = tmpStatus;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrvUsr_ioctl",
                                 status,
                                 "Failed to close OS driver handle!");
        }
        ProcMgrDrvUsr_handle = -1;
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "ProcMgrDrvUsr_ioctl", status);

    /*! @retval ProcMgr_S_SUCCESS Operation successfully completed. */
    return status;
}
