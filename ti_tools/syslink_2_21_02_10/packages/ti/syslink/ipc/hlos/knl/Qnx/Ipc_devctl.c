/*
 *  @file   Ipc_devctl.c
 *
 *  @brief      OS-specific implementation of Ipc driver for Qnx
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
#include <ti/syslink/IpcHost.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* QNX specific header include */
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/proto.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>

/* Module specific header files */
#include <ti/ipc/Ipc.h>
#include <ti/syslink/inc/IpcDrvDefs.h>

/* Function prototypes */
int syslink_ipc_control(resmgr_context_t *ctp, io_devctl_t *msg,
        syslink_ocb_t *ocb);
int syslink_ipc_readconfig(resmgr_context_t *ctp, io_devctl_t *msg,
        syslink_ocb_t *ocb);
int syslink_ipc_writeconfig(resmgr_context_t *ctp, io_devctl_t *msg,
        syslink_ocb_t *ocb);
int syslink_ipc_isattached(resmgr_context_t *ctp, io_devctl_t *msg,
        syslink_ocb_t *ocb);

/**
 * Handler for devctl() messages for ipc module.
 *
 * Handles special devctl() messages that we export for control.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_ipc_devctl(resmgr_context_t *ctp, io_devctl_t *msg,
        syslink_ocb_t *ocb)
{
    switch (msg->i.dcmd) {
        case DCMD_IPC_CONTROL:
            return(syslink_ipc_control(ctp, msg, ocb));

        case DCMD_IPC_READCONFIG:
            return(syslink_ipc_readconfig(ctp, msg, ocb));

        case DCMD_IPC_WRITECONFIG:
            return(syslink_ipc_writeconfig(ctp, msg, ocb));

        case DCMD_IPC_ISATTACHED:
            return(syslink_ipc_isattached(ctp, msg, ocb));

        default:
            fprintf(stderr, "Invalid DEVCTL for IPC 0x%x\n", msg->i.dcmd);
            break;
    }

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o)+sizeof(IpcDrv_CmdArgs)));
}

/**
 * Handler for ipc control API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_ipc_control(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	IpcDrv_CmdArgs *		cargs = (IpcDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	IpcDrv_CmdArgs *		out  = (IpcDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));


    if (cargs->args.control.arg != NULL) {
	    out->apiStatus = Ipc_control (cargs->args.control.procId,
								      cargs->args.control.cmdId,
								      &(cargs->args.control.arg));
	}
	else {
    	out->apiStatus = Ipc_control (cargs->args.control.procId,
								      cargs->args.control.cmdId,
								      NULL);
    }
    GT_assert (curTrace, (out->apiStatus >= 0));
    if (cargs->args.control.cmdId == Ipc_CONTROLCMD_STARTCALLBACK) {
        add_ocb_res(ocb, Ipc_CONTROLCMD_STOPCALLBACK, cargs->args.control.procId, NULL);
    }
    else if (cargs->args.control.cmdId == Ipc_CONTROLCMD_STOPCALLBACK) {
    	remove_ocb_res(ocb, Ipc_CONTROLCMD_STOPCALLBACK, cargs->args.control.procId, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(IpcDrv_CmdArgs)));

}

/**
 * Handler for ipc read config API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_ipc_readconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	IpcDrv_CmdArgs *		cargs = (IpcDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	IpcDrv_CmdArgs *		out  = (IpcDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	Ptr cfg = (Ptr)(cargs+1);

	GT_assert (curTrace, (cfg != NULL));

	out->apiStatus = Ipc_readConfig (cargs->args.readConfig.remoteProcId,
										cargs->args.readConfig.tag,
										cfg,
										cargs->args.readConfig.size);
	GT_assert (curTrace, (out->apiStatus >= 0));

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(IpcDrv_CmdArgs));
	SETIOV(&ctp->iov[1], cfg, cargs->args.readConfig.size);

	return _RESMGR_NPARTS(2);
}

/**
 * Handler for ip write config API.
 *
 * \param ctp	Thread's associated context information.
 * \param msg	The actual devctl() message.
 * \param ocb	OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK		Success.
 * \retval ENOTSUP	Unsupported devctl().
 */
int syslink_ipc_writeconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	IpcDrv_CmdArgs *		cargs = (IpcDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	IpcDrv_CmdArgs *		out  = (IpcDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	Ptr cfg = (Ptr)(cargs+1);

    out->apiStatus = Ipc_writeConfig (cargs->args.writeConfig.remoteProcId,
										 cargs->args.writeConfig.tag,
										 cfg,
										 cargs->args.writeConfig.size);
	GT_assert (curTrace, (out->apiStatus  >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(IpcDrv_CmdArgs)));
}

/**
 * Handler for ipc isAttached API.
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
int syslink_ipc_isattached(resmgr_context_t *ctp, io_devctl_t *msg,
        syslink_ocb_t *ocb)
{
    IpcDrv_CmdArgs *cargs = (IpcDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    IpcDrv_CmdArgs *out  = (IpcDrv_CmdArgs *)(_DEVCTL_DATA(msg->o));
    Bool attached;

    attached = Ipc_isAttached(cargs->args.isAttached.remoteProcId);

    out->args.isAttached.attached = attached;
    out->apiStatus = 0;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o)+sizeof(IpcDrv_CmdArgs)));
}
