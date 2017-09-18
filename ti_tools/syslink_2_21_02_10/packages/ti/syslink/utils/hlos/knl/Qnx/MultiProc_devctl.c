/*
 *  @file   MultiProc_devctl.c
 *
 *  @brief     Driver for MultiProc on HLOS side
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


/*  Defined to include MACROS EXPORT_SYMBOL. This must be done before including
 *  module.h
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
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/MultiProcDrvDefs.h>

/* Function prototypes */
int syslink_multiproc_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_multiproc_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_multiproc_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_multiproc_setloadid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);


/**
 * Handler for devctl() messages for MultiProc module.
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
int syslink_multiproc_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{
		case DCMD_MULTIPROC_SETUP:
			return syslink_multiproc_setup( ctp, msg, ocb);
			break;

		case DCMD_MULTIPROC_DESTROY:
			return syslink_multiproc_destroy( ctp, msg, ocb);
			break;

		case DCMD_MULTIPROC_GETCONFIG:
			return syslink_multiproc_getconfig( ctp, msg, ocb);
			break;

		case DCMD_MULTIPROC_SETLOCALID:
			return syslink_multiproc_setloadid( ctp, msg, ocb);
			break;
		default:
			fprintf( stderr, "Invalid DEVCTL for MultiProc 0x%x \n", msg->i.dcmd);
			break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (MultiProcDrv_CmdArgs)));

}

/**
 * Handler for multiproc setup API.
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
int syslink_multiproc_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MultiProcDrv_CmdArgs *		cargs = (MultiProcDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MultiProcDrv_CmdArgs *		out  = (MultiProcDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	MultiProc_Config *config = (MultiProc_Config *)(cargs+1);

	out->apiStatus = MultiProc_setup (config);
	GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_MULTIPROC_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MultiProcDrv_CmdArgs)));

}

/**
 * Handler for multiproc destroy API.
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
int syslink_multiproc_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MultiProcDrv_CmdArgs *		out  = (MultiProcDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = MultiProc_destroy ();
    GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_MULTIPROC_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MultiProcDrv_CmdArgs)));

}

/**
 * Handler for multiproc get config API.
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
int syslink_multiproc_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

    MultiProcDrv_CmdArgs *      cargs = (MultiProcDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
    MultiProc_Config *          config = (MultiProc_Config *)(cargs+1);

    MultiProc_getConfig (config);

    SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(MultiProcDrv_CmdArgs));
    SETIOV(&ctp->iov[1], config, sizeof(MultiProc_Config));

    return _RESMGR_NPARTS(2); // MsgReplv( ctp->rcvid, ctp->status, ctp->iov, 2 );
}

/**
 * Handler for multiproc set load ID API.
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
int syslink_multiproc_setloadid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {
	MultiProcDrv_CmdArgs *		cargs = (MultiProcDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MultiProcDrv_CmdArgs *		out  = (MultiProcDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

   	out->apiStatus = MultiProc_setLocalId(cargs->args.setLocalId.id);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MultiProcDrv_CmdArgs)));

}

