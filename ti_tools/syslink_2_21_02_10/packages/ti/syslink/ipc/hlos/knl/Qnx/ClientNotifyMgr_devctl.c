/*
 *  @file   ClientNotifyMgr_devctl.c
 *
 *  @brief      OS-specific implementation of ClientNotifyMgr driver for Qnx
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
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr_config.h>
#include <ti/syslink/inc/ClientNotifyMgrDrvDefs.h>

extern GateMP_Handle ClientNotifyMgr_getGate(ClientNotifyMgr_Handle handle);

/* Function prototypes */
int syslink_clientnotifymgr_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/**
 * Handler for devctl() messages for ClientNotifyMgr module.
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
int syslink_clientnotifymgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

				case DCMD_CLIENTNOTIFYMGR_CREATE :
				{
					return syslink_clientnotifymgr_create( ctp, msg, ocb);
				}
				break;

				case DCMD_CLIENTNOTIFYMGR_GETCONFIG:
				{
					return syslink_clientnotifymgr_getconfig( ctp, msg, ocb);
				}
				break;

				case DCMD_CLIENTNOTIFYMGR_SETUP:
				{
					return syslink_clientnotifymgr_setup( ctp, msg, ocb);
				}
				break;

				case DCMD_CLIENTNOTIFYMGR_DESTROY:
				{
					return syslink_clientnotifymgr_destroy( ctp, msg, ocb);
				}
				break;

				case DCMD_CLIENTNOTIFYMGR_PARAMS_INIT:
				{
					return syslink_clientnotifymgr_params_init( ctp, msg, ocb);
				}
				break;

				case DCMD_CLIENTNOTIFYMGR_DELETE:
				{
					return syslink_clientnotifymgr_delete( ctp, msg, ocb);
				}
				break;

				case DCMD_CLIENTNOTIFYMGR_CLOSE :				{
					return syslink_clientnotifymgr_close( ctp, msg, ocb);
				}
				break;

				case  DCMD_CLIENTNOTIFYMGR_SHAREDMEMREQ:
				{
					return syslink_clientnotifymgr_sharedmemreq( ctp, msg, ocb);
				}
				break;

	 	default:
				fprintf( stderr, "Invalid DEVCTL for client notify mgr 0x%x \n", msg->i.dcmd);
		break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (ClientNotifyMgrDrv_CmdArgs)));

}
/**
 * Handler for client notify mgr create
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
int syslink_clientnotifymgr_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)

{
	ClientNotifyMgrDrv_CmdArgs *		cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *		out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	ClientNotifyMgr_Params *	 		params = NULL;
	String 								paramsName = NULL;
	Ptr                                 gateSharedAddr = NULL;
	Int                                 status = 0;

	params = (ClientNotifyMgr_Params *)(cargs + 1);
	if (cargs->args.create.nameLen > 0) {
		paramsName = (String)(params + 1);
		params->name = paramsName;
	}

	params->sharedAddr = SharedRegion_getPtr (
								 cargs->args.create.sharedAddrSrPtr);
	gateSharedAddr = SharedRegion_getPtr(cargs->args.create.gateMPSrPtr);
	status = GateMP_openByAddr(gateSharedAddr, &params->gate);
	GT_assert (curTrace, (status >= 0));

	cargs->args.create.handle = ClientNotifyMgr_create (params);
	if (cargs->args.create.handle != NULL) {
		cargs->args.create.Id =
				 ClientNotifyMgr_getId (cargs->args.create.handle);
	}
	GT_assert (curTrace, (cargs->args.create.handle != NULL));

	out->apiStatus = ClientNotifyMgr_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs)));
}

/**
 * Handler forclient notify mgr getconfig
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
int syslink_clientnotifymgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *	cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *	out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	ClientNotifyMgr_Config *		config = (ClientNotifyMgr_Config *)(cargs + 1);

	ClientNotifyMgr_getConfig (config);

	out->apiStatus = ClientNotifyMgr_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs));
	SETIOV(&ctp->iov[1], config, sizeof(ClientNotifyMgr_Config));

	return _RESMGR_NPARTS(2);
}
/**
 * Handler for client notify mgr setup
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
int syslink_clientnotifymgr_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *		cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *		out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	ClientNotifyMgr_Config *			config;

	config = (ClientNotifyMgr_Config *)(cargs + 1);

	out->apiStatus = ClientNotifyMgr_setup (config);
	GT_assert (curTrace, (out->apiStatus >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs)));
}
/**
 * Handler for client notify mgr destroy
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
int syslink_clientnotifymgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *			out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = ClientNotifyMgr_destroy ();
	GT_assert (curTrace, (out->apiStatus >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs)));
}

/**
 * Handler for client notify mgr params init
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
int syslink_clientnotifymgr_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *			cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *			out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	ClientNotifyMgr_Params *			 	params= (ClientNotifyMgr_Params *)(cargs + 1);

	ClientNotifyMgr_Params_init (params);

	out->apiStatus = ClientNotifyMgr_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs));
	SETIOV(&ctp->iov[1], params, sizeof(ClientNotifyMgr_Params));

	return _RESMGR_NPARTS(2);

}

/**
 * Handler for client notify mgr delete
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
int syslink_clientnotifymgr_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *			cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *			out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    ClientNotifyMgr_Handle                  clientNotifyMgrHandle = cargs->args.deleteInstance.handle;
    GateMP_Handle                           gateHandle = ClientNotifyMgr_getGate(clientNotifyMgrHandle);

	out->apiStatus = ClientNotifyMgr_delete (&cargs->args.deleteInstance.handle);

	if (out->apiStatus >= 0) {
	    out->apiStatus = GateMP_close(&gateHandle);
	}

	GT_assert (curTrace, (out->apiStatus >= 0));
    memset(&msg->o, 0, sizeof(msg->o));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs)));
}

/**
 * Handler for client notify mgr close
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
int syslink_clientnotifymgr_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *			cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *			out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = ClientNotifyMgr_close (&cargs->args.close.handle);
	GT_assert (curTrace, (out->apiStatus >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs)));
}

/**
 * Handler for client notify mgr shared mem req
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
int syslink_clientnotifymgr_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ClientNotifyMgrDrv_CmdArgs *	cargs = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ClientNotifyMgrDrv_CmdArgs *	out   = (ClientNotifyMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	ClientNotifyMgr_Params *		params;

	params = (ClientNotifyMgr_Params *)(cargs + 1);

	cargs->args.sharedMemReq.bytes =
				ClientNotifyMgr_sharedMemReq (params ); /* Need to check : cargs->args.sharedMemReq.params*/

	out->apiStatus = ClientNotifyMgr_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ClientNotifyMgrDrv_CmdArgs)));
}
