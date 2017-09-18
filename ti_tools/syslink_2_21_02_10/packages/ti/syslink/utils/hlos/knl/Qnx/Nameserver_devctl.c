/*
 *  @file   Nameserver_devctl.c
 *
 *  @brief      OS-specific implementation of NameServer driver for Qnx
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
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/NameServerDrvDefs.h>

/* Function prototypes */
int syslink_nameserver_add(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_adduint32(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_get(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_getlocal(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_match(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_remove(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_removeentry(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_gethandle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

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
int syslink_nameserver_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

    switch (msg->i.dcmd)
    {

	  case DCMD_NAMESERVER_ADD:
	  {
		 return syslink_nameserver_add( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_ADDUINT32:
	  {
		   return syslink_nameserver_adduint32( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_GET:
	  {
	  	return syslink_nameserver_get( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_GETLOCAL:
	  {
			 return syslink_nameserver_getlocal( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_MATCH:
	  {
		     return syslink_nameserver_match( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_REMOVE:
	  {
		     return syslink_nameserver_remove( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_REMOVEENTRY:
	  {
		     return syslink_nameserver_removeentry( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_PARAMS_INIT:
	  {
		 	 return syslink_nameserver_params_init( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_CREATE:
	  {
		   return syslink_nameserver_create( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_DELETE:
	  {
		    return syslink_nameserver_delete( ctp, msg, ocb);
	  }
	  break;

        case DCMD_NAMESERVER_OPEN: {
            return(syslink_nameserver_open(ctp, msg, ocb));
        }
        break;

        case DCMD_NAMESERVER_CLOSE: {
            return(syslink_nameserver_close(ctp, msg, ocb));
        }
        break;

	  case DCMD_NAMESERVER_GETHANDLE:
	  {
		    return syslink_nameserver_gethandle( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_SETUP:
	  {
		   return syslink_nameserver_setup( ctp, msg, ocb);
	  }
	  break;

	  case DCMD_NAMESERVER_DESTROY:
	  {
		   return syslink_nameserver_destroy( ctp, msg, ocb);
	  }
	  break;

	 default:
		fprintf( stderr, "Invalid DEVCTL for nameserver 0x%x \n", msg->i.dcmd);
		break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver add API.
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
int syslink_nameserver_add(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	String name = (String)(cargs+1);
	Ptr buf = (Ptr)((sizeof(char) * cargs->args.add.nameLen) + (char *)(name));
	Ptr	entry;

	entry = NameServer_add (cargs->args.add.handle,
									 name,
									 buf,
									 cargs->args.add.len);
	GT_assert (curTrace, (entry != NULL));
    out->args.add.entry = entry;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver addunit32 API.
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
int syslink_nameserver_adduint32(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	NameServerDrv_CmdArgs * 	cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs * 	out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	String name = (String)(cargs+1);

	        Ptr    entry;

            GT_assert (curTrace, (name != NULL));

            entry = NameServer_addUInt32 (cargs->args.addUInt32.handle,
                                          name,
                                          cargs->args.addUInt32.value);
            GT_assert (curTrace, (entry != NULL));

			out->args.addUInt32.entry = entry;

return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver get  API.
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
int syslink_nameserver_get(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	char * value = (char *)(cargs + 1);
	char * name  = (char *)(value + (sizeof(char) * cargs->args.get.len));
	UInt16 *procIds = NULL;

	if(cargs->args.get.procLen > 0)
		procIds = (UInt16 *)(name + (sizeof(char) * cargs->args.get.nameLen));

	out->apiStatus = NameServer_get (cargs->args.get.handle,
							 (String)name,
							 (Ptr)value,
							 &cargs->args.get.len,
							 procIds);
	/* Do not assert. This can return NameServer_E_NOTFOUND
	 * as a valid runtime failure.
	 */

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(NameServerDrv_CmdArgs));
	SETIOV(&ctp->iov[1], (Ptr)value, cargs->args.get.len);

	return _RESMGR_NPARTS(2);

}

/**
 * Handler for nameserver get local ID API.
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
int syslink_nameserver_getlocal(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	String name = (String)(cargs+1);
	Ptr value = (Ptr)(cargs->args.getLocal.len + (char *)(name));

            out->apiStatus = NameServer_getLocal (cargs->args.getLocal.handle,
                                          name,
                                          value,
                                          &cargs->args.getLocal.len);
            GT_assert (curTrace, (out->apiStatus >= 0));

        	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(NameServerDrv_CmdArgs));
        	SETIOV(&ctp->iov[1], name, sizeof(char) * cargs->args.getLocal.nameLen);
        	SETIOV(&ctp->iov[2], value, cargs->args.getLocal.len);

        	return _RESMGR_NPARTS(3);
}

/**
 * Handler for nameserver match API.
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
int syslink_nameserver_match(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	String name = (String)(cargs+1);

			   out->args.match.count = NameServer_match (
													  cargs->args.match.handle,
													  name,
													  &cargs->args.match.value);
			   GT_assert (curTrace, (cargs->args.match.count >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver remove API.
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
int syslink_nameserver_remove(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	String name = (String)(cargs+1);

    NameServer_remove (cargs->args.remove.handle, name);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver remove entry API.
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
int syslink_nameserver_removeentry(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

 	out->apiStatus = NameServer_removeEntry (cargs->args.removeEntry.handle,
                                    cargs->args.removeEntry.entry);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));


}

/**
 * Handler for nameserver params init API.
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
int syslink_nameserver_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

	NameServer_Params *params = (NameServer_Params *)(cargs+1);

	NameServer_Params_init (params);

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(NameServerDrv_CmdArgs));
	SETIOV(&ctp->iov[1], params, sizeof(NameServer_Params));

	return _RESMGR_NPARTS(2);
}

/**
 * Handler for nameserver create API.
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
int syslink_nameserver_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	NameServer_Params * params = (NameServer_Params *)(cargs+1);
	String name = (String)(params+1);

				out->args.create.handle = NameServer_create (name, params);
				GT_assert (curTrace, (out->args.create.handle != NULL));
    if (out->args.create.handle != NULL) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_NAMESERVER_DELETE, (int)out->args.create.handle, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/*
 *  ======== syslink_nameserver_open ========
 */
int syslink_nameserver_open(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{

    NameServerDrv_CmdArgs * cmdArgs;
    NameServer_Handle *     handlePtr;
    String                  name;
    Int                     status;

    /* extract input data */
    cmdArgs = (NameServerDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = &cmdArgs->args.open.handle;
    name = (String)(cmdArgs+1);

    /* invoke the requested API */
    status = NameServer_open(name, handlePtr);

    /* add this call to the list to be cleaned-up */
    if (status >= 0) {
//      add_ocb_res(ocb, DCMD_NAMESERVER_CLOSE, (int)(*handlePtr), NULL);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = sizeof(NameServerDrv_CmdArgs);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/**
 * Handler for nameserver delete API.
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
int syslink_nameserver_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs * 	cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs * 	out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
    NameServer_Handle           temp  = cargs->args.delete.handle;

	out->apiStatus = NameServer_delete (&(cargs->args.delete.handle));
	GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_NAMESERVER_DELETE, (int)temp, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));
}

/*
 *  ======== syslink_nameserver_close ========
 */
int syslink_nameserver_close(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    NameServerDrv_CmdArgs * cmdArgs;
    NameServer_Handle *     handlePtr;
    Int                     status;

    /* extract input data */
    cmdArgs = (NameServerDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = &cmdArgs->args.close.handle;

    /* remove this call from the list to be cleaned-up */
//  remove_ocb_res(ocb, DCMD_NAMESERVER_CLOSE, (int)(*handle), NULL);

    /* invoke the requested API */
    status = NameServer_close(handlePtr);

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = 0;

    return (_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/**
 * Handler for nameserver get handle API.
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
int syslink_nameserver_gethandle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		cargs = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	String name = (String)(cargs+1);

	out->args.getHandle.handle = NameServer_getHandle (name);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver setup  API.
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
int syslink_nameserver_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = NameServer_setup ();
    GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_NAMESERVER_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

/**
 * Handler for nameserver destroy API.
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
int syslink_nameserver_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	NameServerDrv_CmdArgs *		out  = (NameServerDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = NameServer_destroy ();
	GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_NAMESERVER_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(NameServerDrv_CmdArgs)));

}

