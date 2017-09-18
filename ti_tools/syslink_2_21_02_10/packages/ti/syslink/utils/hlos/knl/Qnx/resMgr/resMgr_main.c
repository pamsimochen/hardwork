/*
 *  @file   resMgr_main.c
 *
 *  @brief      resource manager main
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


struct _iofunc_attr;
#define RESMGR_HANDLE_T struct _iofunc_attr
struct syslink_ocb;
#define IOFUNC_OCB_T struct syslink_ocb
#define RESMGR_OCB_T struct syslink_ocb
#define THREAD_POOL_PARAM_T dispatch_context_t

#include "proto.h"
#include <stdio.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/procmgr.h>
#include <sys/neutrino.h>
#include <sys/siginfo.h>
#include <sys/mman.h>
#include <signal.h>
#include <ti/syslink/inc/knl/IpcKnl.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/MemoryOS.h>
#include <ti/syslink/utils/_Cache_qnx.h>

int syslink_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_read(resmgr_context_t *ctp, io_read_t *msg, syslink_ocb_t *ocb);
extern void _NotifyDrv_setup (void);
int init_syslink_device(syslink_dev_t *dev)
{
//	struct stat             sbuf;
	iofunc_attr_t  *attr;
	resmgr_attr_t        resmgr_attr;

	memset(&resmgr_attr, 0, sizeof resmgr_attr);
  	resmgr_attr.nparts_max = 10;
        resmgr_attr.msg_max_size = 8192;
	memset(&dev->syslink.mattr, 0, sizeof(iofunc_mount_t));
	dev->syslink.mattr.flags = ST_NOSUID | ST_NOEXEC;
	dev->syslink.mattr.conf = IOFUNC_PC_CHOWN_RESTRICTED | IOFUNC_PC_NO_TRUNC | IOFUNC_PC_SYNC_IO;
	dev->syslink.mattr.funcs = &dev->syslink.mfuncs;
	memset(&dev->syslink.mfuncs, 0, sizeof(iofunc_funcs_t));
	dev->syslink.mfuncs.nfuncs = _IOFUNC_NFUNCS;
	dev->syslink.mfuncs.ocb_calloc = syslink_ocb_calloc;
	dev->syslink.mfuncs.ocb_free = syslink_ocb_free;
	iofunc_func_init(_RESMGR_CONNECT_NFUNCS, &dev->syslink.cfuncs, _RESMGR_IO_NFUNCS, &dev->syslink.iofuncs);
	iofunc_attr_init(attr = &dev->syslink.cattr, S_IFCHR | 0555, NULL, NULL);
	dev->syslink.iofuncs.devctl = syslink_devctl;
	dev->syslink.iofuncs.read = syslink_notify_read;
	attr->mount = &dev->syslink.mattr;
//	attr->rdev = _S_QNX_SPECIAL;
	iofunc_time_update(attr);
	pthread_mutex_init(&dev->syslink.mutex, NULL);
#if 0
  if (-1 != stat(SYSLINK_DEVICE_PATH, &sbuf)) {
      return (-1);
  }
#endif

  if (-1 == (dev->syslink.resmgr_id = resmgr_attach(dev->dpp, &resmgr_attr, SYSLINK_DEVICE_PATH, _FTYPE_ANY, 0, &dev->syslink.cfuncs, &dev->syslink.iofuncs, attr))) {
		return(-1);
	}
	return(0);
}

int init_devices(syslink_dev_t *dev)
{
		if (init_syslink_device(dev) < 0) {
      		fprintf( stderr, "syslink: syslink device init failed\n");
			return(-1);
		}

	return(0);
}

int init_ipc()
{
    int status = 0;
//  int *vaddr = NULL;
    Ipc_Config iCfg;

#if 0
    /* Map a contiguous memory section for Ducati */
    vaddr = mmap(NULL, 16 * 1024 * 1024, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_PHYS, NOFD, 0x8F000000);
#endif

#if 0   /* -rvh */
    /* Map a contiguous memory section for CMEM */
    vaddr = mmap(NULL, 10 * 1024 * 1024, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_PHYS, NOFD, 0x8A600000);

    /* Map a contiguous memory section for DSP */
    vaddr = mmap(NULL, 32 * 1024 * 1024, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_PHYS, NOFD, 0x8B000000);

    /* Map a contiguous memory section for SR1 */
    vaddr = mmap(NULL, 12 * 1024 * 1024, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_PHYS, NOFD, 0x8D000000);

    /* Map a contiguous memory section for SR0 */
    vaddr = mmap(NULL, 16 * 1024 * 1024, PROT_NOCACHE | PROT_READ | PROT_WRITE, MAP_PHYS, NOFD, 0x8E000000);

    if (!vaddr)
        status = -1;
#endif
    if (status >= 0) {
        status = MemoryOS_setup();
    }

    if (status >= 0) {
        status = Cache_setup();
    }

    if (status >= 0) {
        _NotifyDrv_setup();
    }

    if (status >= 0) {
        iCfg.procSync = Ipc_ProcSync_PAIR;
        status = Ipc_setup(&iCfg);
    }

    return(status);
}

int main(int argc, char *argv[])
{
    syslink_dev_t    *dev = NULL;
    thread_pool_attr_t  tattr;
    int status;
    int error=0;
    sigset_t set;
    UInt32  tattr_custom[4]={2,4,1,10};
#if defined (SYSLINK_TRACE_ENABLE)
    Char *  trace              = FALSE;
    Bool    enableTrace        = FALSE;
    Char *  traceEnter         = FALSE;
    Bool    enableTraceEnter   = FALSE;
    Char *  traceFailure       = FALSE;
    Bool    enableTraceFailure = FALSE;
    Char *  traceClass         = NULL;
    UInt32  enableTraceClass   = 0;
    UInt32  traceMask          = 0;
#endif

    /* Obtain I/O privity */
    error = ThreadCtl_r (_NTO_TCTL_IO, 0);
    if (error == -1) {
        fprintf(stderr, "Unable to obtain I/O privity \n");
        return (error);
    }

    /* allocate the device structure */
    if (NULL == (dev = calloc(1, sizeof(syslink_dev_t)))) {
        fprintf(stderr, "syslink: calloc() failed\n");
        return (-1);
    }

    /* create the dispatch structure */
    if (NULL == (dev->dpp = dispatch_create())) {
        fprintf(stderr, "syslink:  dispatch_create() failed\n");
        return(-1);
    }

    /* Initialize the thread pool */
    memset (&tattr, 0x00, sizeof (thread_pool_attr_t));
    tattr.handle = dev->dpp;
    tattr.context_alloc = dispatch_context_alloc;
    tattr.context_free = dispatch_context_free;
    tattr.block_func = dispatch_block;
    tattr.unblock_func = dispatch_unblock;
    tattr.handler_func = dispatch_handler;
    tattr.lo_water = 2;
    tattr.hi_water = 4;
    tattr.increment = 1;
    tattr.maximum = 10;

    if (argc==2) {
        int count=0;
        count = sscanf(argv[1], "-d%d,%d,%d,%d", &tattr_custom[0],
                &tattr_custom[1], &tattr_custom[2], &tattr_custom[3]);
        if (count==4) {
            tattr.lo_water = tattr_custom[0];
            tattr.hi_water = tattr_custom[1];
            tattr.increment = tattr_custom[2];
            tattr.maximum = tattr_custom[3];
        }
    }

    if ((dev->tpool = thread_pool_create(&tattr, POOL_FLAG_USE_SELF)) == NULL) {
        fprintf(stderr, "syslink: thread pool create failed\n");
        return(-1);
    }

    status = init_devices(dev);
    if (status < 0) {
        fprintf(stderr, "syslink: device init failed\n");
        return(-1);
    }
    status = init_ipc(dev);
    if (status < 0) {
        fprintf(stderr, "syslink: IPC init failed\n");
        return(-1);
    }

    if (-1 == procmgr_daemon(EXIT_SUCCESS,
            PROCMGR_DAEMON_NOCLOSE | PROCMGR_DAEMON_NODEVNULL)) {
        fprintf( stderr, "syslink:  procmgr_daemon() failed\n");
        return(-1);
    }

#if defined (SYSLINK_TRACE_ENABLE)
    /*Adding Trace feature for resMgr */
    trace = getenv ("TRACE");
    /* Enable/disable levels of tracing. */
    if (trace != NULL) {
        enableTrace = strtol (trace, NULL, 16);
        if ((enableTrace != 0) && (enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (enableTrace == TRUE) {
            Osal_printf ("Trace enabled\n");
            traceMask = GT_TraceState_Enable;
        }
        else if (enableTrace == FALSE) {
            Osal_printf ("Trace disabled\n");
            traceMask = GT_TraceState_Disable;
        }
    }

    traceEnter = getenv ("TRACEENTER");
    if (traceEnter != NULL) {
        enableTraceEnter = strtol (traceEnter, NULL, 16);
        if (	(enableTraceEnter != 0)
                &&	(enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (enableTraceEnter == TRUE) {
            Osal_printf ("Trace entry/leave prints enabled\n");
            traceMask |= GT_TraceEnter_Enable;
        }
    }

    traceFailure = getenv ("TRACEFAILURE");
    if (traceFailure != NULL) {
        enableTraceFailure = strtol (traceFailure, NULL, 16);
        if (	(enableTraceFailure != 0)
                &&	(enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (enableTraceFailure == TRUE) {
            Osal_printf ("Trace SetFailureReason enabled\n") ;
            traceMask |= GT_TraceSetFailure_Enable;
        }
    }

    traceClass = getenv ("TRACECLASS");
    if (traceClass != NULL) {
        enableTraceClass = strtol (traceClass, NULL, 16);
        if (	(enableTraceClass != 1)
                &&	(enableTraceClass != 2)
                &&	(enableTraceClass != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            Osal_printf ("Trace class %s\n", traceClass) ;
            enableTraceClass =
                enableTraceClass << (32 - GT_TRACECLASS_SHIFT);
            traceMask |= enableTraceClass;
        }
    }

    /* Set the desired trace value. */
    GT_setTrace (traceMask, GT_TraceType_Kernel);
#endif

    if (EOK != thread_pool_start(dev->tpool)){
        Osal_printf("Thread pool start failed\n");
    }

    /* Mask out unecessary signals */
    sigfillset (&set);
    sigdelset (&set, SIGINT);
    sigdelset (&set, SIGTERM);
    pthread_sigmask (SIG_BLOCK, &set, NULL);

    /* Wait for one of these signals */
    sigemptyset (&set);
    sigaddset (&set, SIGINT);
    sigaddset (&set, SIGQUIT);
    sigaddset (&set, SIGTERM);

    fprintf( stderr, "Starting SysLink resource manager \n");

    /* Wait for a signal */
    while (1) {
        switch (SignalWaitinfo (&set, NULL)) {
            case SIGTERM:
            case SIGQUIT:
            case SIGINT:
                error = EOK;
                goto done;

            default:
                break;
        }
    }

    error = EOK;
done:
    // TODO: resmgr_stop(mgr);
    return (EOK);
}
