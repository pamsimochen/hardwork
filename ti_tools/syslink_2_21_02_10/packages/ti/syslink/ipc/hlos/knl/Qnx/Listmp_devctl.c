/*
 *  @file   Listmp_devctl.c
 *
 *  @brief      OS-specific implementation of Listmp driver for Qnx
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
#include <ti/syslink/inc/_ListMP.h>
#include <ti/ipc/ListMP.h>
#include <ti/syslink/inc/ListMPDrvDefs.h>

/* Function prototypes */
int syslink_listmp_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_isempty(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_putail(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_getahead(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_next(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_prev(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) ;
int syslink_listmp_insert(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_remove(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) ;
int syslink_listmp_gettail(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_puthead(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) ;
int syslink_listmp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/**
 * Handler for devctl() messages for list mp module.
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
int syslink_listmp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

			case DCMD_LISTMP_CREATE:
			{
				return syslink_listmp_create( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_DELETE:
			{
				return syslink_listmp_delete( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_OPEN:
			{
				return syslink_listmp_open( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_OPENBYADDR:
			{
				return syslink_listmp_openbyaddr( ctp, msg, ocb);

			}
			break;

			case DCMD_LISTMP_CLOSE:
			{
				return syslink_listmp_close( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_SHAREDMEMREQ:
			{
				return syslink_listmp_sharedmemreq( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_ISEMPTY :
			{
				return syslink_listmp_isempty( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_PUTTAIL :
			{
				return syslink_listmp_putail( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_GETHEAD:
			{
				return syslink_listmp_getahead( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_NEXT:
			{
				return syslink_listmp_next( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_PREV:
			{
				return syslink_listmp_prev( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_INSERT:
			{
				return syslink_listmp_insert( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_REMOVE:
			{
				return syslink_listmp_remove( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_GETTAIL:
			{
				return syslink_listmp_gettail( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_PUTHEAD:
			{
				return syslink_listmp_puthead( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_GETCONFIG:
			{
				return syslink_listmp_getconfig( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_SETUP:
			{
				return syslink_listmp_setup( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_DESTROY:
			{
				return syslink_listmp_destroy( ctp, msg, ocb);
			}
			break;

			case DCMD_LISTMP_PARAMS_INIT:
			{
				return syslink_listmp_params_init( ctp, msg, ocb);
			}
			break;

	 	default:
			fprintf( stderr, "Invalid DEVCTL for list mp 0x%x \n", msg->i.dcmd);
		break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (ListMPDrv_CmdArgs)));

}

/*
 *  ======== syslink_listmp_create ========
 */
int syslink_listmp_create(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{

    ListMPDrv_CmdArgs * cargs;
    ListMP_Params *     params;
    SharedRegion_SRPtr  srptr;
    Ptr                 gateAttrs;
    ListMP_Handle       handle;
    Int                 status;

    status = 0;
    handle = NULL;

    /* extract input data */
    cargs = (ListMPDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    params = (ListMP_Params *)(cargs+1);
    params->name = (String)(params+1);

    /* compute shared region address */
    srptr = cargs->args.create.sharedAddrSrPtr;
    if (srptr != SharedRegion_INVALIDSRPTR) {
        params->sharedAddr = SharedRegion_getPtr(srptr);
    }
    else {
        params->sharedAddr = NULL;
    }

    /* open the gate instance to get a handle */
    srptr = cargs->args.create.gateAttrs;
    if (srptr != SharedRegion_INVALIDSRPTR) {
        gateAttrs = SharedRegion_getPtr(srptr);
        status = GateMP_openByAddr(gateAttrs, &params->gate);
    }
    else {
        params->gate = NULL;
    }

    /* invoke the requested api */
    if (status >= 0) {
        handle = ListMP_create(params);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(ListMPDrv_CmdArgs);
    cargs->args.create.handle = handle;

    if (handle == NULL) {
        cargs->apiStatus = ListMP_E_FAIL;
    }
    else {
        cargs->apiStatus = ListMP_S_SUCCESS;
        /* add object to the cleanup list */
        add_ocb_res(ocb, DCMD_LISTMP_DELETE, (int)handle, NULL);
    }

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_listmp_delete ========
 */
int syslink_listmp_delete(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{

    ListMPDrv_CmdArgs * cargs;
    ListMP_Handle *     handlePtr;
    GateMP_Handle       gate;
    Int                 status;

    /* extract input data */
    cargs = (ListMPDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = (ListMP_Handle *)(&cargs->args.deleteInstance.handle);

    /* save gate handle, need to close it after delete */
    gate = ListMP_getGate(*handlePtr);

    /* remove object from cleanup list */
    remove_ocb_res(ocb, DCMD_LISTMP_DELETE, (int)(*handlePtr), NULL);

    /* invoke the requested api */
    status = ListMP_delete(handlePtr);

    /* close the gate instance if not the default gate */
    if (gate != GateMP_getDefaultRemote()) {
        GateMP_close(&gate);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(ListMPDrv_CmdArgs);
    cargs->apiStatus = status;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/**
 * Handler for list mp open API.
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
int syslink_listmp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	String        name = (String)(cargs+1);
	ListMP_Handle handle;

	out->apiStatus = ListMP_open (name, &handle);
	out->args.open.handle = handle;
    if (out->apiStatus >= 0 && handle) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_LISTMP_CLOSE, (int)handle, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));


}

/**
 * Handler for list mp open by address API.
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
int syslink_listmp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));


	Ptr 	sharedAddr;
	ListMP_Handle handle;

			  /* Initialize to NULL. name gets precedence. */
			  sharedAddr = NULL;

			  /* For openByAddr by name, the sharedAddr may be invalid. */
			  sharedAddr = SharedRegion_getPtr (
										 cargs->args.openByAddr.sharedAddrSrPtr);

			  out->apiStatus = ListMP_openByAddr (sharedAddr, &handle);
			  out->args.openByAddr.handle = handle;
    if (out->apiStatus >= 0 && handle) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_LISTMP_CLOSE, (int)handle, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}

/**
 * Handler for listmp close API.
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
int syslink_listmp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	ListMP_Handle           temp = cargs->args.close.handle;

	out->apiStatus = ListMP_close ((ListMP_Handle *)
												&cargs->args.close.handle);
	GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_LISTMP_CLOSE, (int)temp, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));


}

/**
 * Handler for listmp shared memory request API.
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
int syslink_listmp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

          ListMP_Params *params = (ListMP_Params *)(cargs+1);

            /* For ListMP_sharedMemReq, the sharedAddr may be invalid. */
            params->sharedAddr = (Ptr) SharedRegion_getPtr (
                                    cargs->args.sharedMemReq.sharedAddrSrPtr);

            out->args.sharedMemReq.bytes = ListMP_sharedMemReq (params);
	out->apiStatus = ListMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));


}


/**
 * Handler for listmp isempty API.
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
int syslink_listmp_isempty(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->apiStatus = ListMP_empty (cargs->args.isEmpty.handle);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));

}

/**
 * Handler for listmp put tail API.
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
int syslink_listmp_putail(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

		 ListMP_Elem *	elem;

		 elem = (ListMP_Elem *) SharedRegion_getPtr (cargs->args.putTail.elemSrPtr);

		 out->apiStatus= ListMP_putTail (cargs->args.putTail.handle, elem);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));

}

/**
 * Handler for listmp get ahead API.
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
int syslink_listmp_getahead(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

      ListMP_Elem *  elem;
      UInt16         index;

            elem = ListMP_getHead (cargs->args.getHead.handle);
            if (elem != NULL) {
                index = SharedRegion_getId (elem);
                out->args.getHead.elemSrPtr = SharedRegion_getSRPtr (elem,
                                                                       index);
            }
            else {
                /* Set to invalid in case of failure. */
                out->args.getHead.elemSrPtr = SharedRegion_INVALIDSRPTR;
            }
	out->apiStatus = ListMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}

/**
 * Handler for listmp next API.
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
int syslink_listmp_next(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


		ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
		ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

        ListMP_Elem * elem;
            ListMP_Elem * retElem;
            UInt16        index;

            /* elem can be NULL for ListMP_next. */
            elem = (ListMP_Elem *) SharedRegion_getPtr(
                                                    cargs->args.next.elemSrPtr);
            retElem  = ListMP_next (cargs->args.next.handle, elem);
            index = SharedRegion_getId (retElem);
            out->args.next.nextElemSrPtr = SharedRegion_getSRPtr (retElem,
                                                                    index);
	out->apiStatus = ListMP_S_SUCCESS;

		return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));

}

/**
 * Handler forlistmp prev API.
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
int syslink_listmp_prev(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	ListMP_Elem * elem;
			 ListMP_Elem * retElem;
			 UInt16 	   index;

			 /* elem can be NULL for ListMP_prev. */
			 elem = (ListMP_Elem *) SharedRegion_getPtr(
													 cargs->args.prev.elemSrPtr);
			 retElem  = ListMP_prev (cargs->args.prev.handle, elem);
			 index = SharedRegion_getId (retElem);
			 out->args.prev.prevElemSrPtr = SharedRegion_getSRPtr (retElem,
																	 index);
	out->apiStatus = ListMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));

}

/**
 * Handler for listmp insert API.
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
int syslink_listmp_insert(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	   ListMP_Elem *  newElem;
				ListMP_Elem *  curElem;
				newElem = (ListMP_Elem *) SharedRegion_getPtr (
												   cargs->args.insert.newElemSrPtr);
				curElem = (ListMP_Elem *) SharedRegion_getPtr (
												   cargs->args.insert.curElemSrPtr);
				out->apiStatus = ListMP_insert (cargs->args.insert.handle,
											 newElem,
											 curElem);

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));

}

/**
 * Handler for list mp remove API.
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
int syslink_listmp_remove(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	   ListMP_Elem * elem;

				elem = (ListMP_Elem *) SharedRegion_getPtr (
													cargs->args.remove.elemSrPtr);

				out->apiStatus = ListMP_remove (cargs->args.remove.handle, elem);


	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}

/**
 * Handler for list mp gettail API.
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
int syslink_listmp_gettail(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	ListMP_Elem *  elem;
	UInt16 		index;

			 elem = ListMP_getTail (cargs->args.getTail.handle);
			 index = SharedRegion_getId (elem);
			 out->args.getTail.elemSrPtr = SharedRegion_getSRPtr (elem,
																	index);
	out->apiStatus = ListMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}


/**
 * Handler for list mp put head API.
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
int syslink_listmp_puthead(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	  ListMP_Elem * elem;

				elem = (ListMP_Elem *) SharedRegion_getPtr (
													 cargs->args.putHead.elemSrPtr);

				out->apiStatus = ListMP_putHead (cargs->args.putHead.handle, elem);


	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}


/**
 * Handler for list mp get config API.
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
int syslink_listmp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMP_Config *config = (ListMP_Config *)(cargs+1);

    ListMP_getConfig (config);
    cargs->apiStatus = ListMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ListMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], config, sizeof(ListMP_Config));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for list mp setup API.
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
int syslink_listmp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	ListMP_Config *config = (ListMP_Config *)(cargs+1);

 	out->apiStatus = ListMP_setup (config);
	GT_assert (curTrace, (out->apiStatus >= 0));

    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_LISTMP_DESTROY, (int)NULL, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}


/**
 * Handler for list mp destroy API.
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
int syslink_listmp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		out  = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = ListMP_destroy ();
	GT_assert (curTrace, (out->apiStatus  >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_LISTMP_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ListMPDrv_CmdArgs)));
}


/**
 * Handler for list mp params init API.
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
int syslink_listmp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	ListMPDrv_CmdArgs *		cargs = (ListMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	ListMP_Params *params = (ListMP_Params *)(cargs+1);

  ListMP_Params_init (params);
    cargs->apiStatus = ListMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ListMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], params, sizeof(ListMP_Params));

	return _RESMGR_NPARTS(2);
}




