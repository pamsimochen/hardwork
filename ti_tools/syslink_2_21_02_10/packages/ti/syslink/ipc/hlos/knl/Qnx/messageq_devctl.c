/*
 *  @file   messageq_devctl.c
 *
 *  @brief      OS-specific implementation of Messageq driver for Qnx
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
#include <ti/ipc/MessageQ.h>
#include <ti/syslink/inc/MessageQDrvDefs.h>

/* Function prototypes */
int syslink_messageq_put(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_get(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_count(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_unblock(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_registerheap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_unregisterheap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);


/**
 * Handler for devctl() messages for messageQ module.
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
int syslink_messageq_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

	 	  case DCMD_MESSAGEQ_PUT:
		  {
				return syslink_messageq_put( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_GET:
		  {
				return syslink_messageq_get( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_COUNT:
		  {
				return syslink_messageq_count( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_ALLOC:
		  {
				return syslink_messageq_alloc( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_FREE:
		  {
				return syslink_messageq_free( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_PARAMS_INIT :
		  {
				return syslink_messageq_params_init( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_CREATE:
		  {
				return syslink_messageq_create( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_DELETE:
		  {
				return syslink_messageq_delete( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_OPEN:
		  {
				return syslink_messageq_open( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_CLOSE:
		  {
				return syslink_messageq_close( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_UNBLOCK:
		  {
				return syslink_messageq_unblock( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_GETCONFIG:
		  {
				return syslink_messageq_getconfig( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_SETUP:
		  {
				return syslink_messageq_setup( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_DESTROY:
		  {
				return syslink_messageq_destroy( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_REGISTERHEAP:
		  {
				return syslink_messageq_registerheap( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_UNREGISTERHEAP:
		  {
				return syslink_messageq_unregisterheap( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_SHAREDMEMREQ:
		  {
				return syslink_messageq_sharedmemreq( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_ATTACH:
		  {
				return syslink_messageq_attach( ctp, msg, ocb);
		  }
		  break;

		  case DCMD_MESSAGEQ_DETACH:
		  {
				return syslink_messageq_detach( ctp, msg, ocb);
		  }
		  break;

	 default:
		fprintf( stderr, "Invalid DEVCTL for messageQ 0x%x \n", msg->i.dcmd);
		break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (MessageQDrv_CmdArgs)));

}

/**
 * Handler for messageq put API.
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
int syslink_messageq_put(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    MessageQ_Msg lmsg;

    lmsg = SharedRegion_getPtr (cargs->args.put.msgSrPtr);
    out->apiStatus = MessageQ_put (cargs->args.put.queueId,
                                   lmsg);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));

}

/**
 * Handler for messageq get API.
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
int syslink_messageq_get(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	MessageQ_Msg        lmsg      = NULL;
    SharedRegion_SRPtr  msgSrPtr  = SharedRegion_INVALIDSRPTR;
    UInt16              index;

    out->apiStatus = MessageQ_get (cargs->args.get.handle,
                                   &lmsg,
                                   cargs->args.get.timeout);
    if (out->apiStatus >= 0) {
        index    = SharedRegion_getId (lmsg);
        msgSrPtr = SharedRegion_getSRPtr (lmsg, index);
    }

    out->args.get.msgSrPtr = msgSrPtr;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));

}

/**
 * Handler for messageq count info API.
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
int syslink_messageq_count(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->args.count.count = MessageQ_count (cargs->args.count.handle);
    if (out->args.count.count < 0)
    	cargs->apiStatus = out->args.count.count;
    else
    	cargs->apiStatus = MessageQ_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));


}

/**
 * Handler for messageq alloc API.
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
int syslink_messageq_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	MessageQ_Msg		lmsg;
	SharedRegion_SRPtr	msgSrPtr = SharedRegion_INVALIDSRPTR;
	UInt16				index;

	lmsg = MessageQ_alloc (cargs->args.alloc.heapId,
						  cargs->args.alloc.size);
	if (lmsg != NULL) {
		index = SharedRegion_getId (lmsg);
		msgSrPtr = SharedRegion_getSRPtr (lmsg, index);
        cargs->apiStatus = MessageQ_S_SUCCESS;
	}
	else {
        cargs->apiStatus = MessageQ_E_FAIL;
	}

	out->args.alloc.msgSrPtr = msgSrPtr;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}

/**
 * Handler for messageq free API.
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
int syslink_messageq_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	MessageQ_Msg lmsg;
    lmsg = SharedRegion_getPtr (cargs->args.free.msgSrPtr);
    out->apiStatus = MessageQ_free (lmsg);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));


}

/**
 * Handler for messageq params init API.
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
int syslink_messageq_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

	MessageQ_Params *local_params = (MessageQ_Params *)(cargs+1);

  	MessageQ_Params_init (local_params);
    cargs->apiStatus = MessageQ_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(MessageQDrv_CmdArgs));
	SETIOV(&ctp->iov[1], local_params, sizeof(*local_params));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for messageq create API.
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
int syslink_messageq_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	MessageQ_Params *local_createparams = NULL;
	String local_createname = NULL;
	out->apiStatus = MessageQ_S_SUCCESS;

	if (cargs->args.create.params) {
		local_createparams = (MessageQ_Params *)(cargs+1);
		if (cargs->args.create.name)
			local_createname = (String)(local_createparams+1);
	}
	else {
		if (cargs->args.create.name)
			local_createname = (String)(cargs+1);
	}

    out->args.create.handle = MessageQ_create (local_createname, local_createparams);
    GT_assert (curTrace, (out->args.create.handle != NULL));

    /* Set failure status if create has failed. */
    if (out->args.create.handle == NULL) {
        out->apiStatus = MessageQ_E_FAIL;
    }
    else {
    	out->apiStatus = MessageQ_S_SUCCESS;
    }

    if (out->args.create.handle != NULL) {
        out->args.create.queueId = MessageQ_getQueueId (
                                             out->args.create.handle);
    }

    if (out->apiStatus >= 0) {
        /* At this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_MESSAGEQ_DELETE, (int)out->args.create.handle, NULL);
    }

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(MessageQDrv_CmdArgs));
	return _RESMGR_NPARTS(1);
}

/**
 * Handler for messageq delete API.
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
int syslink_messageq_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	MessageQ_Handle             temp  = cargs->args.deleteMessageQ.handle;

	out->apiStatus = MessageQ_delete ((MessageQ_Handle *)
                                            &(cargs->args.deleteMessageQ.handle));
    GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* At this call to the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_MESSAGEQ_DELETE, (int)temp, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));

}

/**
 * Handler for messageq open API.
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
int syslink_messageq_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	MessageQ_QueueId queueId = MessageQ_INVALIDMESSAGEQ;
	String local_openname = (String)(cargs+1);

	out->apiStatus = MessageQ_open (local_openname, &queueId);
	GT_1trace (curTrace,
				GT_2CLASS,
				"    MessageQ_open ioctl queueId [0x%x]",
				queueId);

	out->args.open.queueId = queueId;
    if (out->apiStatus >= 0) {
        /* At this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_MESSAGEQ_CLOSE, (int)out->args.open.queueId, NULL);
    }

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(MessageQDrv_CmdArgs));

	return _RESMGR_NPARTS(1);
}

/**
 * Handler for messageq close API.
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
int syslink_messageq_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out   = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	MessageQ_QueueId queueId = cargs->args.close.queueId;

	out->apiStatus = MessageQ_S_SUCCESS;
	MessageQ_close (&queueId);

	out->args.close.queueId = queueId;

    if (out->apiStatus >= 0) {
        /* At this call to the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_MESSAGEQ_CLOSE, (int)out->args.close.queueId, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));

}

/**
 * Handler for messageq unblock API.
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
int syslink_messageq_unblock(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQ_unblock (cargs->args.unblock.handle);
    cargs->apiStatus = MessageQ_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));

}

/**
 * Handler for messageq getconfig API.
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
int syslink_messageq_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
    MessageQDrv_CmdArgs * cargs =
            (MessageQDrv_CmdArgs *)(_DEVCTL_DATA (msg->i));
	MessageQ_Config local_config;
  MessageQ_getConfig (&local_config);

    cargs->apiStatus = MessageQ_S_SUCCESS;
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(MessageQDrv_CmdArgs));
	SETIOV(&ctp->iov[1], &local_config, sizeof(MessageQ_Config));

	return _RESMGR_NPARTS(2);

}

/**
 * Handler for messageq setup API.
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
int syslink_messageq_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	MessageQ_Config *local_setupconfig;
	local_setupconfig = (MessageQ_Config *)(cargs+1);

	out->apiStatus = MessageQ_setup (local_setupconfig);

	GT_assert (curTrace, (out->apiStatus  >= 0));
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_MESSAGEQ_DESTROY, NULL, NULL);
    }

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(MessageQDrv_CmdArgs));

	return _RESMGR_NPARTS(1);
}

/**
 * Handler for messageq setup API.
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
int syslink_messageq_registerheap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = MessageQ_registerHeap (cargs->args.registerHeap.heap,
											   cargs->args.registerHeap.heapId);
    if (out->apiStatus >= 0) {
        /* At this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_MESSAGEQ_UNREGISTERHEAP, (int)cargs->args.registerHeap.heapId, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}

/**
 * Handler for messageq destroy API.
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
int syslink_messageq_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = MessageQ_destroy ();
    GT_assert (curTrace, (	out->apiStatus  >= 0));
    if (out->apiStatus >= 0)
    	remove_ocb_res(ocb, DCMD_MESSAGEQ_DESTROY, NULL, NULL);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}

/**
 * Handler for messageq unregisterheap API.
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
int syslink_messageq_unregisterheap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = MessageQ_unregisterHeap(cargs->args.unregisterHeap.heapId);
    if (out->apiStatus >= 0)
    	remove_ocb_res(ocb, DCMD_MESSAGEQ_UNREGISTERHEAP, (int)cargs->args.unregisterHeap.heapId, NULL);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}

/**
 * Handler for messageq shared mem request  API.
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
int syslink_messageq_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	Ptr sharedAddr;
	sharedAddr = SharedRegion_getPtr (
							cargs->args.sharedMemReq.sharedAddrSrPtr);
	out->args.sharedMemReq.memReq = MessageQ_sharedMemReq (sharedAddr);
	out->apiStatus = MessageQ_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}

/**
 * Handler for messageq attach API.
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
int syslink_messageq_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	Ptr sharedAddr;
	sharedAddr = SharedRegion_getPtr (
									cargs->args.attach.sharedAddrSrPtr);
	out->apiStatus = MessageQ_attach (cargs->args.attach.remoteProcId,
							  sharedAddr);
    if (out->apiStatus >= 0) {
        /* At this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_MESSAGEQ_DETACH, (int)cargs->args.attach.remoteProcId, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}

/**
 * Handler for messageq detach API.
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
int syslink_messageq_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	MessageQDrv_CmdArgs *		cargs = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	MessageQDrv_CmdArgs *		out  = (MessageQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = MessageQ_detach (cargs->args.attach.remoteProcId);
    if (out->apiStatus >= 0)
    	remove_ocb_res(ocb, DCMD_MESSAGEQ_DETACH, (int)cargs->args.attach.remoteProcId, NULL);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(MessageQDrv_CmdArgs)));
}


