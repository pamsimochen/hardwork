/*
 *  @file   Heapmemmp_devctl.c
 *
 *  @brief      OS-specific implementation of Heapmemmp driver for Qnx
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
#include <ti/syslink/utils/IHeap.h>
#include <ti/ipc/HeapMemMP.h>
#include <ti/syslink/inc/HeapMemMPDrvDefs.h>

/* Function prototypes */
int syslink_heapmemmp_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_getstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_getextendedstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_restore(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);


/**
 * Handler for devctl() messages for heap mem mp module.
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
int syslink_heapmemmp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

						case DCMD_HEAPMEMMP_ALLOC:
						{
							return syslink_heapmemmp_alloc( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_FREE:
						{
							return syslink_heapmemmp_free( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_PARAMS_INIT:
						{
							return syslink_heapmemmp_params_init( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_CREATE:
						{
							return syslink_heapmemmp_create( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_DELETE:
						{
							return syslink_heapmemmp_delete( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_OPEN:
						{
							return syslink_heapmemmp_open( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_OPENBYADDR:
						{
							return syslink_heapmemmp_openbyaddr( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_CLOSE:
						{
 							return syslink_heapmemmp_close( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_SHAREDMEMREQ:
						{
							return syslink_heapmemmp_sharedmemreq( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_GETCONFIG:
						{
							return syslink_heapmemmp_getconfig( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_SETUP:
						{
							return syslink_heapmemmp_setup( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_DESTROY:
						{
							return syslink_heapmemmp_destroy( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_GETSTATS:
						{
							return syslink_heapmemmp_getstats( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_GETEXTENDEDSTATS:
						{
							return syslink_heapmemmp_getextendedstats( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPMEMMP_RESTORE:
						{
							return syslink_heapmemmp_restore( ctp, msg, ocb);
						}
						break;

		default:
			fprintf( stderr, "Invalid DEVCTL for HeapMemMP 0x%x \n", msg->i.dcmd);
			break;
	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (HeapMemMPDrv_CmdArgs)));

}

/**
 * Handler for heapmemmp alloc API.
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
int syslink_heapmemmp_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMPDrv_CmdArgs *		out  = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	Char *             block;

            SharedRegion_SRPtr blockSrPtr = SharedRegion_INVALIDSRPTR;
    UInt16              index = SharedRegion_INVALIDREGIONID;
            block = HeapMemMP_alloc (cargs->args.alloc.handle,
                                     cargs->args.alloc.size,
                                     cargs->args.alloc.align);
            if (block != NULL) {
                index = SharedRegion_getId (block);
                GT_assert (curTrace, (index != SharedRegion_INVALIDREGIONID));
                blockSrPtr = SharedRegion_getSRPtr (block, index);
                GT_assert (curTrace, (blockSrPtr != SharedRegion_INVALIDSRPTR));
            }

            out->args.alloc.blockSrPtr = blockSrPtr;
    out->apiStatus = HeapMemMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));

}

/**
 * Handler for heapmemmp free API.
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
int syslink_heapmemmp_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

	Char * block;
			  block = SharedRegion_getPtr (cargs->args.free.blockSrPtr);
			  GT_assert (curTrace, (block != NULL));
			  HeapMemMP_free (cargs->args.free.handle,
							  block,
							  cargs->args.free.size);

    cargs->apiStatus = HeapMemMP_S_SUCCESS;
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));

}

/**
 * Handler for heapmem mp params init API.
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
int syslink_heapmemmp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

	HeapMemMP_Params *params = (HeapMemMP_Params *)(cargs+1);

	HeapMemMP_Params_init (params);
	cargs->apiStatus = HeapMemMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapMemMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], params, sizeof(HeapMemMP_Params));		// TODO: do we need to extend the HeapMemMP_Params struct or use devctlv?

   return _RESMGR_NPARTS(2);
}

/*
 *  ======== syslink_heapmemmp_create ========
 */
int syslink_heapmemmp_create(
    resmgr_context_t *ctp,
    io_devctl_t *msg,
    syslink_ocb_t *ocb)
{

    HeapMemMPDrv_CmdArgs *  cargs;
    HeapMemMP_Params *      params;
    SharedRegion_SRPtr      srptr;
    Ptr                     gateAttrs;
    HeapMemMP_Handle        handle;
    Int                     status;

    status = 0;
    handle = NULL;

    /* extract input data */
    cargs = (HeapMemMPDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    params = (HeapMemMP_Params *)(cargs+1);
    params->name = (String)(params+1);

    /* compute shared region address */
    srptr = cargs->args.create.sharedAddrSrPtr;
    if (srptr != SharedRegion_INVALIDSRPTR) {
        params->sharedAddr = SharedRegion_getPtr (srptr);
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
        handle = HeapMemMP_create(params);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(HeapMemMPDrv_CmdArgs);
    cargs->args.create.handle = handle;

    if (handle == NULL) {
        cargs->apiStatus = HeapMemMP_E_FAIL;
    }
    else {
    	cargs->apiStatus = HeapMemMP_S_SUCCESS;
        /* add object to the cleanup list */
        add_ocb_res(ocb, DCMD_HEAPMEMMP_DELETE, (int)handle, NULL);
    }

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_heapmemmp_delete ========
 */
int syslink_heapmemmp_delete(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{

    HeapMemMPDrv_CmdArgs *  cargs;
    HeapMemMP_Handle *      handlePtr;
    GateMP_Handle           gate;
    Int                     status;

    /* extract input data */
    cargs = (HeapMemMPDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = (HeapMemMP_Handle *)(&cargs->args.deleteInstance.handle);

    /* save gate handle, need to close it after delete */
    gate = HeapMemMP_getGate(*handlePtr);

    /* remove object from cleanup list */
    remove_ocb_res(ocb, DCMD_HEAPMEMMP_DELETE, (int)(*handlePtr), NULL);

    /* invoke the requested api */
    status = HeapMemMP_delete(handlePtr);

    /* close the gate instance if not the default gate */
    if (gate != GateMP_getDefaultRemote()) {
        GateMP_close(&gate);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(HeapMemMPDrv_CmdArgs);
    cargs->apiStatus = status;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/**
 * Handler for heapmem mp open API.
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
int syslink_heapmemmp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMPDrv_CmdArgs *		out  = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

  	String           name = (String) (cargs + 1);

    HeapMemMP_Handle handle;

    out->apiStatus = HeapMemMP_open (name, &handle);
    /* HeapMemMP_open can fail at run-time, so don't assert on its
         * failure.
         */
    if (out->apiStatus >= 0 && handle) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_HEAPMEMMP_CLOSE, (int)handle, NULL);
    }
    out->args.open.handle = handle;
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


/**
 * Handler for ip heapmemmp openby addr API.
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
int syslink_heapmemmp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMPDrv_CmdArgs *		out  = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	HeapMemMP_Handle handle;

		  Ptr	sharedAddr;

		  sharedAddr = SharedRegion_getPtr (cargs->args.openByAddr.sharedAddrSrPtr);

		  out->apiStatus = HeapMemMP_openByAddr (sharedAddr, &handle);
		  /* HeapMemMP_open can fail at run-time, so don't assert on its
		   * failure.
		   */
    if (out->apiStatus >= 0 && handle) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_HEAPMEMMP_CLOSE, (int)handle, NULL);
    }
		  out->args.openByAddr.handle = handle;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


/**
 * Handler for heapmem mp close API.
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
int syslink_heapmemmp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMPDrv_CmdArgs *		out  = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	HeapMemMP_Handle            temp = cargs->args.close.handle;

	out->apiStatus = HeapMemMP_close ((HeapMemMP_Handle *)
                                                &(cargs->args.close.handle));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_HEAPMEMMP_CLOSE, (int)temp, NULL);
    }
    GT_assert (curTrace, (out->apiStatus >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


/**
 * Handler for heapmem mp shared mem req API.
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
int syslink_heapmemmp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMPDrv_CmdArgs *		out  = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    HeapMemMP_Params *params = (HeapMemMP_Params*)(cargs+1);

            if (params->sharedAddr != NULL) {
                params->sharedAddr = SharedRegion_getPtr (
                                             cargs->args.create.sharedAddrSrPtr);
            }

            out->args.sharedMemReq.bytes =
                       HeapMemMP_sharedMemReq (params);
	out->apiStatus = HeapMemMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


/**
 * Handler for heapmem mp get config API.
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
int syslink_heapmemmp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

	HeapMemMP_Config *config = (HeapMemMP_Config *)(cargs+1);

    HeapMemMP_getConfig (config);
    cargs->apiStatus = HeapMemMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapMemMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], config, sizeof(HeapMemMP_getConfig));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for heapmem mp setup API.
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
int syslink_heapmemmp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMPDrv_CmdArgs *		out  = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	HeapMemMP_Config *config = (HeapMemMP_Config*)(cargs+1);

    out->apiStatus = HeapMemMP_setup (config);

	GT_assert (curTrace, (out->apiStatus >= 0));

    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_HEAPMEMMP_DESTROY, (int)NULL, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


/**
 * Handler for heap mem mp destroy API.
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
int syslink_heapmemmp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		out = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->apiStatus = HeapMemMP_destroy ();

	GT_assert (curTrace, (out->apiStatus >= 0));

    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_HEAPMEMMP_DESTROY, (int)NULL, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


/**
 * Handler for heap mem mp get stats API.
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
int syslink_heapmemmp_getstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	Memory_Stats *stats = (Memory_Stats *)(cargs+1);

    HeapMemMP_getStats (cargs->args.getStats.handle,
                                (Ptr)stats);
    cargs->apiStatus = HeapMemMP_S_SUCCESS;
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapMemMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], stats, sizeof(Memory_Stats));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for heap mem mp get extended stats API.
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
int syslink_heapmemmp_getextendedstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapMemMP_ExtendedStats *stats = (HeapMemMP_ExtendedStats *)(cargs+1);

    HeapMemMP_getExtendedStats (
                                       cargs->args.getExtendedStats.handle,
                                       (Ptr)stats);
    cargs->apiStatus = HeapMemMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapMemMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], stats, sizeof(HeapMemMP_ExtendedStats));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for heap mem mp restore API.
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
int syslink_heapmemmp_restore(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapMemMPDrv_CmdArgs *		cargs = (HeapMemMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    HeapMemMP_restore (cargs->args.restore.handle);
    cargs->apiStatus = HeapMemMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapMemMPDrv_CmdArgs)));
}


