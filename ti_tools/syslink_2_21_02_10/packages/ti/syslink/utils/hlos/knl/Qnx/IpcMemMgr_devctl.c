/*
 *  @file   IpcMemMgr_devctl.c
 *
 *  @brief      Driver for IpcMemMgr on HLOS side
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
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* QNX specific header include */
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/proto.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>

/* Module specific header files */
#include <ti/syslink/utils/IpcMemMgr.h>
#include <ti/syslink/inc/usr/IpcMemMgrDrv.h>
#include <ti/syslink/inc/IpcMemMgrDrvDefs.h>

/* Function prototypes */
int syslink_ipcmemmgr_setup(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_setloadid(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_alloc(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_free(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_acquire(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);
int syslink_ipcmemmgr_release(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb);


/**
 * Handler for devctl() messages for IpcMemMgr module.
 *
 * Handles special devctl() messages that we export for control.
 *
 * \param ctp   Thread's associated context information.
 * \param msg   The actual devctl() message.
 * \param ocb   OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK          Success.
 * \retval ENOTSUP      Unsupported devctl().
 */
int syslink_ipcmemmgr_devctl(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{

    switch (msg->i.dcmd) {
        case DCMD_IPCMEMMGR_GETCONFIG:
            return(syslink_ipcmemmgr_getconfig(ctp, msg, ocb));

        case DCMD_IPCMEMMGR_SETUP:
            return(syslink_ipcmemmgr_setup(ctp, msg, ocb));

        case DCMD_IPCMEMMGR_DESTROY:
            return(syslink_ipcmemmgr_destroy(ctp, msg, ocb));

        case DCMD_IPCMEMMGR_ALLOC:
            return(syslink_ipcmemmgr_alloc(ctp, msg, ocb));

        case DCMD_IPCMEMMGR_FREE:
            return(syslink_ipcmemmgr_free(ctp, msg, ocb));

        case DCMD_IPCMEMMGR_ACQUIRE:
            return(syslink_ipcmemmgr_acquire(ctp, msg, ocb));

        case DCMD_IPCMEMMGR_RELEASE:
            return(syslink_ipcmemmgr_release(ctp, msg, ocb));

        default:
            fprintf(stderr, "Invalid DEVCTL for IpcMemMgr 0x%x", msg->i.dcmd);
            break;
    }

    return(_RESMGR_PTR(ctp, &msg->o,
        sizeof(msg->o) + sizeof(IpcMemMgrDrv_CmdArgs)));
}


/**
 * Handler for ipcmemmgr get config API.
 *
 * \param ctp   Thread's associated context information.
 * \param msg   The actual devctl() message.
 * \param ocb   OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK          Success.
 * \retval ENOTSUP      Unsupported devctl().
 */
int syslink_ipcmemmgr_getconfig(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    IpcMemMgr_Config *config = (IpcMemMgr_Config *)(_DEVCTL_DATA(msg->o));

    /* invoke the requested api */
    IpcMemMgr_getConfig(config);

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = IpcMemMgr_S_SUCCESS;
    msg->o.nbytes = sizeof(IpcMemMgr_Config);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}


/*
 *  ======== syslink_ipcmemmgr_setup ========
 */
int syslink_ipcmemmgr_setup(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    IpcMemMgr_Config *  config;
    Int                 status;

    /* extract input data */
    config = (IpcMemMgr_Config *)(_DEVCTL_DATA(msg->i));

    /* invoke the requested api */
    status = IpcMemMgr_setup(config);

    if (status >= 0) {
        /* add object to the cleanup list */
        add_ocb_res(ocb, DCMD_IPCMEMMGR_DESTROY, (int)NULL, NULL);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = sizeof(IpcMemMgr_Config);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_ipcmemmgr_destroy ========
 */
int syslink_ipcmemmgr_destroy(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    Int status;

    /* remove object from cleanup list */
    remove_ocb_res(ocb, DCMD_IPCMEMMGR_DESTROY, (int)NULL, NULL);

    /* invoke the requested api */
    status = IpcMemMgr_destroy();

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = 0;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_ipcmemmgr_alloc ========
 */
int syslink_ipcmemmgr_alloc(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    IpcMemMgrDrv_CmdArgs *  cmdArgs;
    SizeT                   size;
    Ptr                     addr;
    Int                     status;

    /* extract input data */
    cmdArgs = (IpcMemMgrDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    size = cmdArgs->args.alloc.size;

    /* invoke the requested api */
    addr = IpcMemMgr_alloc(size);

    status = (addr == NULL ? IpcMemMgr_E_MEMORY : IpcMemMgr_S_SUCCESS);

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = sizeof(IpcMemMgrDrv_CmdArgs);
    cmdArgs->args.alloc.offset = IpcMemMgr_getOffset(addr);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_ipcmemmgr_free ========
 */
int syslink_ipcmemmgr_free(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    IpcMemMgrDrv_CmdArgs *  cmdArgs;
    SizeT                   size;
    Ptr                     addr;

    /* extract input data */
    cmdArgs = (IpcMemMgrDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    addr = IpcMemMgr_getPtr(cmdArgs->args.free.offset);
    size = cmdArgs->args.free.size;

    /* invoke the requested api */
    IpcMemMgr_free(addr, size);

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = IpcMemMgr_S_SUCCESS;
    msg->o.nbytes = 0;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_ipcmemmgr_acquire ========
 */
int syslink_ipcmemmgr_acquire(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    IpcMemMgrDrv_CmdArgs *  cmdArgs;
    String                  name;
    SizeT                   size;
    Ptr                     addr;
    Bool                    newAlloc;
    Int                     status;

    /* extract input data */
    cmdArgs = (IpcMemMgrDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    name = (String)(cmdArgs+1);
    size = cmdArgs->args.acquire.size;

    /* invoke the requested api */
    status = IpcMemMgr_acquire(name, size, &addr, &newAlloc);

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = sizeof(IpcMemMgrDrv_CmdArgs);

    cmdArgs->args.acquire.offset = IpcMemMgr_getOffset(addr);
    cmdArgs->args.acquire.newAlloc = newAlloc;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_ipcmemmgr_release ========
 */
int syslink_ipcmemmgr_release(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    String  name;
    Int     status;

    /* extract input data */
    name = (String)(_DEVCTL_DATA(msg->i));

    /* invoke the requested api */
    status = IpcMemMgr_release(name);

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = 0;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}
