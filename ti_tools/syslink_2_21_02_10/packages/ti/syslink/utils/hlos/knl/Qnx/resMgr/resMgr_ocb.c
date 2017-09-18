/*
 *  @file   resMgr_ocb.c
 *
 *  @brief      resMgr ocb support functions
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


#include "proto.h"
#include <ti/syslink/utils/List.h>
#include "dcmd_syslink.h"

#include <ti/syslink/utils/MemoryDefs.h>
#include <ti/syslink/inc/GateMPDrvDefs.h>
#include <ti/syslink/inc/_HeapBufMP.h>
#include <ti/syslink/inc/HeapBufMPDrvDefs.h>
#include <ti/syslink/inc/HeapMemMPDrvDefs.h>
#include <ti/syslink/inc/MessageQDrvDefs.h>
#include <ti/syslink/inc/NotifyDrvDefs.h>
#include <ti/syslink/inc/SharedRegionDrvDefs.h>
#include <ti/syslink/inc/ProcMgrDrvDefs.h>
#include <ti/syslink/inc/MultiProcDrvDefs.h>
#include <ti/syslink/inc/_NameServer.h>
#include <ti/syslink/inc/NameServerDrvDefs.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/syslink/inc/ListMPDrvDefs.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/IpcHost.h>

#ifdef CONFIG_SYSLINK_RECOVERY
#include "atomic_qnx.h"
#include "OsalEvent.h"

extern Atomic ipc_cref;
extern OsalEvent_Handle ipc_comp;
extern OsalEvent_Handle ipc_open_comp;
extern Bool recover;
#endif
void add_ocb_res(syslink_ocb_t *ocb, int cmd, int data, void * args)
{
    syslink_res_info_t *info = calloc(1, sizeof(*info));
    Int ret;

    List_elemClear(&info->elem);
    info->cmd = cmd;
    info->data = data;
    info->args = args;

    ret = pthread_mutex_lock(&ocb->resLock);
    GT_assert (curTrace, (ret == 0));
    List_putHead(ocb->res, (List_Elem *)info);
    ret = pthread_mutex_unlock(&ocb->resLock);
    GT_assert (curTrace, (ret == 0));
}

void remove_ocb_res(syslink_ocb_t *ocb, int cmd, int data, void * args)
{
    List_Elem *tmp = NULL;
    syslink_res_info_t *elem = NULL;
    UInt32 * resArgs = (UInt32 *)args;
    UInt32 * elemArgs = NULL;
    Int ret;

    ret = pthread_mutex_lock(&ocb->resLock);
    GT_assert (curTrace, (ret == 0));
    List_traverse(tmp, ocb->res) {
        elem = (syslink_res_info_t *)tmp;
        if (elem->cmd == cmd && elem->data == data) {
        	/* special check for ProcMgr map case */
        	if (elem->args) {
        		elemArgs = (UInt32 *)elem->args;
                if (elemArgs[0] == resArgs[0] &&
                    elemArgs[2] == resArgs[2] &&
                    !memcmp((void *)elemArgs[1], (void *)resArgs[1], sizeof(ProcMgr_AddrInfo))) {
                	Memory_free(NULL, (Ptr)elemArgs[2], sizeof(ProcMgr_AddrInfo));
                	Memory_free(NULL, (Ptr)elemArgs, sizeof(UInt32) * 3);
                }
                else {
                	continue;
                }
        	}
            List_remove(ocb->res, tmp);
        	free(tmp);
        	break;
        }
    }
    ret = pthread_mutex_unlock(&ocb->resLock);
    GT_assert (curTrace, (ret == 0));
}
extern Int NotifyDrv_detach (UInt32 pid, Bool force);
void syslink_ocb_release_resources(syslink_ocb_t *ocb)
{
	syslink_res_info_t *elem = NULL;

    while ((elem = (syslink_res_info_t *)List_dequeue(ocb->res)) != NULL) {
        /* Clean up the resource. */
        switch (elem->cmd) {
            /* GateMP Cleanup Commands */
            case DCMD_GATEMP_DESTROY:
    		    GateMP_destroy();
    		    break;
            case DCMD_GATEMP_DELETE:
    		    GateMP_delete((GateMP_Handle *)&elem->data);
    		    break;
            case DCMD_GATEMP_CLOSE:
    		    GateMP_close((GateMP_Handle *)&elem->data);
    		    break;

            /* HeapBufMP Cleanup Commands */
    	    case DCMD_HEAPBUFMP_DESTROY:
    	    	HeapBufMP_destroy();
    	    	break;
    	    case DCMD_HEAPBUFMP_DELETE:
    	    	HeapBufMP_delete((HeapBufMP_Handle *)&elem->data);
    	    	break;
    	    case DCMD_HEAPBUFMP_CLOSE:
    	    	HeapBufMP_close((HeapBufMP_Handle *)&elem->data);
    	    	break;

    	    /* HeapMemMP Cleanup Commands */
    	    case DCMD_HEAPMEMMP_DESTROY:
    	    	HeapMemMP_destroy();
    	    	break;
    	    case DCMD_HEAPMEMMP_DELETE:
    	    	HeapMemMP_delete((HeapMemMP_Handle *)&elem->data);
    	    	break;
    	    case DCMD_HEAPMEMMP_CLOSE:
    	    	HeapMemMP_close((HeapMemMP_Handle *)&elem->data);
    	    	break;

            /* HeapMemMP Cleanup Commands */
    	    case DCMD_LISTMP_DESTROY:
    	    	ListMP_destroy();
    	    	break;
    	    case DCMD_LISTMP_DELETE:
    	    	ListMP_delete((ListMP_Handle *)&elem->data);
    	    	break;
    	    case DCMD_LISTMP_CLOSE:
    	    	ListMP_close((ListMP_Handle *)&elem->data);
    	    	break;

            /* MessageQ Cleanup Commands */
            case DCMD_MESSAGEQ_DESTROY:
            	MessageQ_destroy();
                break;
            case DCMD_MESSAGEQ_DELETE:
                MessageQ_delete((MessageQ_Handle *)&elem->data);
                break;
            case DCMD_MESSAGEQ_CLOSE:
                MessageQ_close((MessageQ_QueueId *)&elem->data);
                break;
            case DCMD_MESSAGEQ_UNREGISTERHEAP:
            	MessageQ_unregisterHeap((UInt16)elem->data);
                break;
            case DCMD_MESSAGEQ_DETACH:
                MessageQ_detach((UInt16)elem->data);
                break;

            /* Notify Cleanup Commands */
            case DCMD_NOTIFY_DESTROY:
            	Notify_destroy();
            	break;
            case DCMD_NOTIFY_THREADDETACH:
            	NotifyDrv_detach((UInt32)elem->data, TRUE);
            	break;

            /* SharedRegion Cleanup Commands */
            case DCMD_SHAREDREGION_DESTROY:
            	SharedRegion_destroy();
            	break;
            case DCMD_SHAREDREGION_CLEARENTRY:
            	SharedRegion_clearEntry((UInt16)elem->data);
            	break;

            /* ProcMgr Cleanup Commands */
            case DCMD_PROCMGR_DESTROY:
            	ProcMgr_destroy();
            	break;
            case DCMD_PROCMGR_DETACH:
            	ProcMgr_detach((ProcMgr_Handle)elem->data);
            	break;
            case DCMD_PROCMGR_STOP:
            	ProcMgr_stop((ProcMgr_Handle)elem->data);
            	break;
            case DCMD_PROCMGR_UNMAP:
            {
            	UInt32 * unmapArgs = (UInt32 *)elem->args;

            	ProcMgr_unmap((ProcMgr_Handle)elem->data,
            			      (ProcMgr_MapMask)unmapArgs[0],
            			      (ProcMgr_AddrInfo *)unmapArgs[1],
            			      (ProcMgr_AddrType)unmapArgs[2]);
                Memory_free(NULL, (Ptr)unmapArgs[2], sizeof(ProcMgr_AddrInfo));
                Memory_free(NULL, (Ptr)unmapArgs, sizeof(UInt32) * 3);
            	break;
            }
            case DCMD_PROCMGR_CLOSE:
            	ProcMgr_close((ProcMgr_Handle *)&elem->data);
            	break;
            case DCMD_PROCMGR_DELETE:
            	ProcMgr_delete((ProcMgr_Handle *)&elem->data);
            	break;

            /* MultiProc Cleanup Commands */
            case DCMD_MULTIPROC_DESTROY:
            	MultiProc_destroy();
            	break;

            /* NameServer Cleanup Commands */
            case DCMD_NAMESERVER_DESTROY:
            	NameServer_destroy();
            	break;
            case DCMD_NAMESERVER_DELETE:
            	NameServer_delete((NameServer_Handle *)&elem->data);
            	break;

            case Ipc_CONTROLCMD_STOPCALLBACK:
            	Ipc_control ((UInt16)elem->data,
                             Ipc_CONTROLCMD_STOPCALLBACK,
                             NULL);
                break;
    	}
        free(elem);
    }
}

int syslink_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra)
{
#ifdef CONFIG_SYSLINK_RECOVERY
	if (recover) {
		Osal_printf("syslink_ocb_calloc: trying to open file while recovering...\n");
		if (ctp->info.flags & O_NONBLOCK ||
			OsalEvent_wait(ipc_open_comp, (UInt32)(-1)) != OSALEVENT_SUCCESS) {
			Osal_printf("syslink_ocb_calloc: unable to open file...\n");
			return EIO;
		}
	}
#endif

    return iofunc_open_default(ctp, msg, handle, extra);
}
syslink_ocb_t *syslink_ocb_calloc(resmgr_context_t *ctp, iofunc_attr_t *attr)
{
    syslink_ocb_t *ocb;
		struct _msg_info cl_info;
    List_Params params;
    Error_Block                    eb;

    if (NULL == (ocb = calloc(1, sizeof(*ocb)))) {
        return NULL;
		}
		MsgInfo(ctp->rcvid, &cl_info);
		ocb->pid = cl_info.pid;
    List_Params_init (&params);
    ocb->res = List_create(&params, &eb);
    if (ocb->res == NULL) {
        free(ocb);
        return NULL;
    }
    if (EOK != pthread_mutex_init(&ocb->resLock, NULL)) {
        List_delete(&ocb->res);
        free(ocb);
        return NULL;
    }
  	ocb->mqRes = List_create(&params, &eb);
    if (ocb->mqRes == NULL) {
        List_delete(&ocb->res);
        free(ocb);
        return NULL;
    }

#ifdef CONFIG_SYSLINK_RECOVERY
	if (ocb)
		Atomic_inc_return(&ipc_cref);
#endif

    return ocb;
}

void syslink_ocb_free(syslink_ocb_t *ocb)
{
    Int ret;

    if (NULL != ocb) {
        if (NULL != ocb->mqRes)
        	List_delete(&ocb->mqRes);

        if (NULL != ocb->res) {
            ret = pthread_mutex_lock(&ocb->resLock);
            GT_assert (curTrace, (ret == 0));
            syslink_ocb_release_resources(ocb);
            List_delete(&ocb->res);
            ret = pthread_mutex_unlock(&ocb->resLock);
            GT_assert (curTrace, (ret == 0));
        }

        pthread_mutex_destroy(&ocb->resLock);

        free(ocb);
    }

#ifdef CONFIG_SYSLINK_RECOVERY
	if (!Atomic_dec_return(&ipc_cref)) {
		if (recover)
		    OsalEvent_set(ipc_comp);
	}
#endif
	return;
}

/**
 * Stops the QNX resource manager framework.
 *
 * Shuts down the thread pool and unregisters the device name. Once
 * successfully complete, the DSP Resource Manager will stop responding to
 * external requests.
 *
 * \note Can be used to "pause" the resource manager and later restarted with
 *       resmgr_start().
 *
 * \param mgr	Handle to the resource manager framework.
 *
 * \return POSIX errno value.
 *
 * \retval EOK	Success.
 */
int
resmgr_stop (void)
{
#if 0
	int i;
#if defined (CHNL_COMPONENT)
	Uint32 	status;
	MemFreeAttrs freeAttrs ;
#endif /* if defined (CHNL_COMPONENT) */

	DSP_INFO(3, "Stop Dsplink Resource Manager....");

#if defined (CHNL_COMPONENT)
	freeAttrs.physicalAddress = DRV_MemAllocAttrs.physicalAddress ;
	freeAttrs.size = sizeof (Uint32) * MAX_DSPS * MAX_CHANNELS ;
	status = MEM_Free ((Pvoid *) &DRV_ChnlIdToPoolId, &freeAttrs) ;
	if ( DSP_FAILED( status ) )
		return(EINVAL);
#endif /* if defined (CHNL_COMPONENT) */

	pthread_mutex_lock( &sync_mutex );
	for (i = 0; i < num_CsObjs; i++){
		if (sync_info[i].sync_object_name[0] != '\0'){
			shm_unlink(sync_info[i].sync_object_name);
			memset (&sync_info[i], NULL, sizeof(cs_object_info_t));
		}
	}
	num_CsObjs = 0;
	pthread_mutex_unlock( &sync_mutex );

	pthread_mutex_destroy( &sync_mutex );
	close( MEM_Largemap_fd );

	/* Stop the thread pool */
#if defined (NOTIFY_COMPONENT)
	UEVENT_Finalize () ;
#endif

	thread_pool_destroy (mgr->ThreadPool);

	return (EOK);
/*}}}1*/
#endif

    return (EOK);
}
