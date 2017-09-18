/*
 *  @file   Gatemp_devctl.c
 *
 *  @brief      OS-specific implementation of Gatemp driver for Qnx
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
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/GateMPDrvDefs.h>


/* Function prototypes */
int syslink_gatemp_enter(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_leave(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_getdefaultremote(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_getsharedaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/**
 * Handler for devctl() messages for gate MP module.
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
int syslink_gatemp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

	 		case DCMD_GATEMP_ENTER:
			{
				return syslink_gatemp_enter( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_LEAVE:
			{
				 return syslink_gatemp_leave( ctp, msg, ocb);
			}
			break;

#if 0
			case DCMD_GATEMP_GETCONFIG:
			{
				 return syslink_gatemp_getconfig( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_SETUP:
			{
			 	return syslink_gatemp_setup( ctp, msg, ocb);
			}
			break;
#endif

			case DCMD_GATEMP_DESTROY:
			{
				 return syslink_gatemp_destroy( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_PARAMS_INIT:
			{
				 return syslink_gatemp_params_init( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_CREATE:
			{
				 return syslink_gatemp_create( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_DELETE:
			{
				 return syslink_gatemp_delete( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_OPEN:
			{
				 return syslink_gatemp_open( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_OPENBYADDR:
			{
				 return syslink_gatemp_openbyaddr( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_CLOSE:
			{
				 return syslink_gatemp_close( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_SHAREDMEMREQ:
			{
				 return syslink_gatemp_sharedmemreq( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_GETDEFAULTREMOTE:
			{
				 return syslink_gatemp_getdefaultremote( ctp, msg, ocb);
			}
			break;

			case DCMD_GATEMP_GETSHAREDADDR:
			{
				 return syslink_gatemp_getsharedaddr( ctp, msg, ocb);
			}
			break;

		 default:
			fprintf( stderr, "Invalid DEVCTL for Gate MP 0x%x \n", msg->i.dcmd);
			break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (GateMPDrv_CmdArgs)));

}

/**
 * Handler for gate mp enter API.
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
int syslink_gatemp_enter(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->args.enter.flags = GateMP_enter (cargs->args.enter.handle);
    cargs->apiStatus = GateMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));

}

/**
 * Handler for gate mp leave API.
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
int syslink_gatemp_leave(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

	GateMP_leave (cargs->args.enter.handle,
                                cargs->args.enter.flags);
	cargs->apiStatus = GateMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));

}

#if 0
/**
 * Handler for gate mp get config  API.
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
int syslink_gatemp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMP_Config local_config;

  GateMP_getConfig (&local_config);
	cargs->apiStatus = GateMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(GateMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], &local_config, sizeof(GateMP_Config));

	return _RESMGR_NPARTS(2);
}
#endif

#if 0
/**
 * Handler for gate mp setup API.
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
int syslink_gatemp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	GateMP_Config *local_config = (GateMP_Config *)(cargs+1);

	out->apiStatus = GateMP_setup (local_config);

	GT_assert (curTrace, (out->apiStatus  >= 0));
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_GATEMP_DESTROY, (int)NULL, NULL);
    }

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(GateMPDrv_CmdArgs));

	return _RESMGR_NPARTS(1);
}
#endif

/**
 * Handler for gate mp destroy API.
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
int syslink_gatemp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = GateMP_destroy ();
	GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_GATEMP_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));
}

/**
 * Handler for gate mp params init API.
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
int syslink_gatemp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out   = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
//	GateMP_Params local_params;
	GateMP_Params *local_params   = (GateMP_Params *)(cargs + 1);

//	GateMP_Params_init (&local_params);
	GateMP_Params_init (local_params);
	out->apiStatus = GateMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(GateMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], local_params, sizeof(GateMP_Params));
//	SETIOV(&ctp->iov[1], &local_params, sizeof(GateMP_Params));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for gate mp create API.
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
int syslink_gatemp_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	GateMP_Params *local_params = (GateMP_Params *)(cargs+1);
	String local_name = (String)(local_params+1);
	local_params->name = local_name;

	local_params->sharedAddr = SharedRegion_getPtr (
								cargs->args.create.sharedAddrSrPtr);
	out->args.create.handle = GateMP_create (local_params);
	GT_assert (curTrace, (cargs->args.create.handle != NULL));

	/* Set failure status if create has failed. */
	if (out->args.create.handle == NULL) {
		out->apiStatus = GateMP_E_FAIL;
	}
	else {
		out->apiStatus = GateMP_S_SUCCESS;
	}

    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_GATEMP_DELETE, (int)out->args.create.handle, NULL);
    }

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(GateMPDrv_CmdArgs));

	return _RESMGR_NPARTS(1);
}

/**
 * Handler for gate Mp delete API.
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
int syslink_gatemp_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	GateMP_Handle           temp  = cargs->args.deleteInstance.handle;

	   out->apiStatus = GateMP_delete ((GateMP_Handle *)
                                            &cargs->args.deleteInstance.handle);
       GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_GATEMP_DELETE, (int)temp, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));

}

/**
 * Handler for gate mp open API.
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
int syslink_gatemp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	String local_name = (String)(cargs+1);

	out->apiStatus = GateMP_open (local_name, &cargs->args.open.handle);
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_GATEMP_CLOSE, (int)cargs->args.open.handle, NULL);
    }
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(GateMPDrv_CmdArgs));

	return _RESMGR_NPARTS(1);
}

/**
 * Handler for gate mp open by addr API.
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
int syslink_gatemp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	Ptr 		  sharedAddr = NULL;

	/* For openByAddr by name, the sharedAddrSrPtr may be invalid. */
	if ( cargs->args.openByAddr.sharedAddrSrPtr !=	SharedRegion_INVALIDSRPTR) {
		sharedAddr = SharedRegion_getPtr ( cargs->args.openByAddr.sharedAddrSrPtr);
	}
	out->apiStatus = GateMP_openByAddr (sharedAddr, &cargs->args.openByAddr.handle);
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_GATEMP_CLOSE, (int)cargs->args.openByAddr.handle, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));
}

/**
 * Handler for gatemp close API.
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
int syslink_gatemp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	GateMP_Handle           temp = cargs->args.close.handle;

	out->apiStatus = GateMP_close ((GateMP_Handle *)
												  &(cargs->args.close.handle));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_GATEMP_CLOSE, (int)temp, NULL);
    }
	GT_assert (curTrace, (out->apiStatus >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));

}

/**
 * Handler for gate mp shared memory request API.
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
int syslink_gatemp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	GateMP_Params *local_params = (GateMP_Params *)(cargs+1);

	out->args.sharedMemReq.retVal = GateMP_sharedMemReq (local_params);
	cargs->apiStatus = GateMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(GateMPDrv_CmdArgs));

	return _RESMGR_NPARTS(1);
}

/**
 * Handler for gate mp get default remote API.
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
int syslink_gatemp_getdefaultremote(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->args.getDefaultRemote.handle = GateMP_getDefaultRemote ();
    out->apiStatus = GateMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));
}

int syslink_gatemp_getsharedaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	GateMPDrv_CmdArgs *		cargs = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	GateMPDrv_CmdArgs *		out  = (GateMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->args.getSharedAddr.retVal = GateMP_getSharedAddr (cargs->args.getSharedAddr.handle);
	out->apiStatus = GateMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(GateMPDrv_CmdArgs)));
}
