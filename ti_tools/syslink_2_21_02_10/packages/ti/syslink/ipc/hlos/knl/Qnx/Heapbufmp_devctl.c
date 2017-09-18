/*
 *  @file   Heapbufmp_devctl.c
 *
 *  @brief      OS-specific implementation of Heapbufmp driver for Qnx
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
#include <ti/syslink/inc/_HeapBufMP.h>
#include <ti/ipc/HeapBufMP.h>
#include <ti/syslink/inc/HeapBufMPDrvDefs.h>


/* Function prototypes */
int syslink_heapbufmp_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_getstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_getextendedstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/**
 * Handler for devctl() messages for heap buf mp module.
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
int syslink_heapbufmp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

						case DCMD_HEAPBUFMP_ALLOC:
						{
							return syslink_heapbufmp_alloc( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_FREE:
						{
							return syslink_heapbufmp_free( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_PARAMS_INIT:
						{
							return syslink_heapbufmp_params_init( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_CREATE:
						{
							return syslink_heapbufmp_create( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_DELETE:
						{
							return syslink_heapbufmp_delete( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_OPEN:
						{
							return syslink_heapbufmp_open( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_OPENBYADDR:
						{
							return syslink_heapbufmp_openbyaddr( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_CLOSE:
						{
 							return syslink_heapbufmp_close( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_SHAREDMEMREQ:
						{
							return syslink_heapbufmp_sharedmemreq( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_GETCONFIG:
						{
							return syslink_heapbufmp_getconfig( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_SETUP:
						{
							return syslink_heapbufmp_setup( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_DESTROY:
						{
							return syslink_heapbufmp_destroy( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_GETSTATS:
						{
							return syslink_heapbufmp_getstats( ctp, msg, ocb);
						}
						break;

						case DCMD_HEAPBUFMP_GETEXTENDEDSTATS:
						{
							return syslink_heapbufmp_getextendedstats( ctp, msg, ocb);
						}
						break;

		default:
			fprintf( stderr, "Invalid DEVCTL for HeapBufMP 0x%x \n", msg->i.dcmd);
			break;
	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (HeapBufMPDrv_CmdArgs)));

}

/**
 * Handler for heapbufmp alloc API.
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
int syslink_heapbufmp_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	Char *             block;
    SharedRegion_SRPtr blockSrPtr = SharedRegion_INVALIDSRPTR;
    UInt16              index = SharedRegion_INVALIDREGIONID;
    block = HeapBufMP_alloc (cargs->args.alloc.handle,
                             cargs->args.alloc.size,
                             cargs->args.alloc.align);
    if (block != NULL) {
        index = SharedRegion_getId (block);
        blockSrPtr = SharedRegion_getSRPtr (block, index);
    }

    out->args.alloc.blockSrPtr = blockSrPtr;
    out->apiStatus = HeapBufMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));

}

/**
 * Handler for heapbufmp free API.
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
int syslink_heapbufmp_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    Char * block;
	block = SharedRegion_getPtr (cargs->args.free.blockSrPtr);
	HeapBufMP_free (cargs->args.free.handle,
					block,
					cargs->args.free.size);

	cargs->apiStatus = HeapBufMP_S_SUCCESS;
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}

/**
 * Handler for heapbuf mp params init API.
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
int syslink_heapbufmp_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMP_Params *local_params = (HeapBufMP_Params *)(cargs+1);

	HeapBufMP_Params_init (local_params);
	cargs->apiStatus = HeapBufMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapBufMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], local_params, sizeof(HeapBufMP_Params));

   return _RESMGR_NPARTS(2);
}

/*
 *  ======== syslink_heapbufmp_create ========
 */
int syslink_heapbufmp_create(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    HeapBufMPDrv_CmdArgs *      cargs;
    HeapBufMP_Params *          params;
    SharedRegion_SRPtr          srptr;
    Ptr                         gateAttrs;
    HeapBufMP_Handle            handle;
    Int                         status;

    status = 0;
    handle = NULL;

    /* extract input data */
    cargs = (HeapBufMPDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    params = (HeapBufMP_Params *)(cargs+1);
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
        handle = HeapBufMP_create(params);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(HeapBufMPDrv_CmdArgs);
    cargs->args.create.handle = handle;

    if (handle == NULL) {
        cargs->apiStatus = HeapBufMP_E_FAIL;
    }
    else {
        cargs->apiStatus = HeapBufMP_S_SUCCESS;
        /* add object to the cleanup list */
        add_ocb_res(ocb, DCMD_HEAPBUFMP_DELETE, (int)handle, NULL);
    }

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_heapbufmp_delete ========
 */
int syslink_heapbufmp_delete(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    HeapBufMPDrv_CmdArgs *      cargs;
    HeapBufMP_Handle *          handlePtr;
    GateMP_Handle               gate;
    Int                         status;

    /* extract input data */
    cargs = (HeapBufMPDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = (HeapBufMP_Handle *)(&cargs->args.deleteInstance.handle);

    /* save gate handle, need to close it after delete */
    gate = HeapBufMP_getGate(*handlePtr);

    /* remove object from cleanup list */
    remove_ocb_res(ocb, DCMD_HEAPBUFMP_DELETE, (int)(*handlePtr), NULL);

    /* invoke the requested api */
    status = HeapBufMP_delete(handlePtr);

    /* close the gate instance if not the default gate */
    if (gate != GateMP_getDefaultRemote()) {
        GateMP_close(&gate);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(HeapBufMPDrv_CmdArgs);
    cargs->apiStatus = status;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}


/**
 * Handler for heapbuf mp open API.
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
int syslink_heapbufmp_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	String name  = (String)(cargs+1);

	HeapBufMP_Handle handle;

	out->apiStatus = HeapBufMP_open (name, &handle);
	/* HeapBufMP_open can fail at run-time, so don't assert on its
	 * failure.
	 */
	out->args.open.handle = handle;
    if (out->apiStatus >= 0 && out->args.open.handle) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_HEAPBUFMP_CLOSE, (int)out->args.open.handle, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}


/**
 * Handler for ip heapbufmp openby addr API.
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
int syslink_heapbufmp_openbyaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	 HeapBufMP_Handle handle;
     Ptr              sharedAddr;

	sharedAddr = SharedRegion_getPtr (cargs->args.openByAddr.sharedAddrSrPtr);

	out->apiStatus = HeapBufMP_openByAddr (sharedAddr, &handle);
	/* HeapBufMP_open can fail at run-time, so don't assert on its
	 * failure.
	 */
	out->args.openByAddr.handle = handle;
    if (out->apiStatus >= 0 && out->args.open.handle) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_HEAPBUFMP_CLOSE, (int)out->args.openByAddr.handle, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}


/**
 * Handler for heapbuf mp close API.
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
int syslink_heapbufmp_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));
	HeapBufMP_Handle            temp = cargs->args.close.handle;

	out->apiStatus = HeapBufMP_close ((HeapBufMP_Handle *)
                                                &(cargs->args.close.handle));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_HEAPBUFMP_CLOSE, (int)temp, NULL);
    }
	GT_assert (curTrace, (out->apiStatus  >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}


/**
 * Handler for heapbuf mp shared mem req API.
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
int syslink_heapbufmp_sharedmemreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    HeapBufMP_Params *params = (HeapBufMP_Params *)(cargs+1);

    out->args.sharedMemReq.bytes =
                       HeapBufMP_sharedMemReq (params);
    out->apiStatus = HeapBufMP_S_SUCCESS;

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}


/**
 * Handler for heapbuf mp get config API.
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
int syslink_heapbufmp_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    HeapBufMP_Config *config = (HeapBufMP_Config *)(cargs+1);

	HeapBufMP_getConfig (config);
	cargs->apiStatus = HeapBufMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapBufMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], config, sizeof(HeapBufMP_Config));

   return _RESMGR_NPARTS(2);
}


/**
 * Handler for heapbuf mp setup API.
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
int syslink_heapbufmp_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	HeapBufMP_Config *config = (HeapBufMP_Config *)(cargs+1);

	out->apiStatus = HeapBufMP_setup (config);

	GT_assert (curTrace, (out->apiStatus >= 0));

    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_HEAPBUFMP_DESTROY, (int)NULL, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}


/**
 * Handler for heap buf mp destroy API.
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
int syslink_heapbufmp_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	HeapBufMPDrv_CmdArgs *		out  = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	out->apiStatus = HeapBufMP_destroy ();
	GT_assert (curTrace, (out->apiStatus >= 0));
    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_HEAPBUFMP_DESTROY, (int)NULL, NULL);
    }

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(HeapBufMPDrv_CmdArgs)));
}


/**
 * Handler for heap buf mp get stats API.
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
int syslink_heapbufmp_getstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	Memory_Stats *stats = (Memory_Stats *)(cargs+1);

	HeapBufMP_getStats (cargs->args.getStats.handle,
                                stats);
	cargs->apiStatus = HeapBufMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapBufMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], stats, sizeof(Memory_Stats));

	return _RESMGR_NPARTS(2);
}


/**
 * Handler for heap buf mp get extended stats API.
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
int syslink_heapbufmp_getextendedstats(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

	HeapBufMPDrv_CmdArgs *		cargs = (HeapBufMPDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
	HeapBufMP_ExtendedStats *stats = (HeapBufMP_ExtendedStats *)(cargs+1);

	HeapBufMP_getExtendedStats ( cargs->args.getExtendedStats.handle,
                                         stats);
	cargs->apiStatus = HeapBufMP_S_SUCCESS;

	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(HeapBufMPDrv_CmdArgs));
	SETIOV(&ctp->iov[1], stats, sizeof(HeapBufMP_ExtendedStats));

	return _RESMGR_NPARTS(2);
}
