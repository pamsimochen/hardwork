/** 
 *  @file   proto.h
 *
 *  @brief      common include for resource manager.
 *
 *
 *
 */
/* 
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



#ifndef _SYSLINK_PROTO_H_INCLUDED
#define _SYSLINK_PROTO_H_INCLUDED

struct _iofunc_attr;
#define RESMGR_HANDLE_T struct _iofunc_attr
struct syslink_ocb;
#define IOFUNC_OCB_T struct syslink_ocb
#define RESMGR_OCB_T struct syslink_ocb
#define THREAD_POOL_PARAM_T dispatch_context_t

/* QNX specific header files */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <hw/inout.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <devctl.h>
#include <atomic.h>
#include <sys/slogcodes.h>
#include <ti/syslink/Std.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>

#include <ti/syslink/utils/List.h>

//#define SYSLINK_DEVICE_PATH        "/dev/syslink"
#define SYSLINK_DEVICE_PATH         "/dev/syslinkipc_Osal"

typedef struct named_device {
    iofunc_mount_t      mattr;
    iofunc_attr_t       cattr;
    int                 resmgr_id;
		pthread_mutex_t     mutex;
		iofunc_funcs_t      mfuncs;
		resmgr_connect_funcs_t  cfuncs;
		resmgr_io_funcs_t   iofuncs;
		char device_name[_POSIX_PATH_MAX];
} named_device_t;

typedef struct syslink_dev {
    dispatch_t          *dpp;
		thread_pool_t     *tpool;
		named_device_t syslink;
} syslink_dev_t;

typedef struct syslink_mq_res_info {
    List_Elem elem;
    int    rcvid;
    void * handle;
} syslink_mq_res_info_t;

typedef struct syslink_res_info {
    List_Elem  elem;
    int        cmd;
    int        data;
    void *     args;
} syslink_res_info_t;
typedef struct syslink_ocb {
	iofunc_ocb_t       ocb;
	pid_t              pid;
	List_Handle        res;
	pthread_mutex_t    resLock;
	List_Handle        mqRes;
} syslink_ocb_t;

void add_ocb_res(syslink_ocb_t *ocb, int cmd, int data, void * args);
void remove_ocb_res(syslink_ocb_t *ocb, int cmd, int data, void * args);
int syslink_open(resmgr_context_t *ctp, io_open_t *msg, RESMGR_HANDLE_T *handle, void *extra);
syslink_ocb_t *syslink_ocb_calloc(resmgr_context_t *ctp, iofunc_attr_t *attr);
void syslink_ocb_free(syslink_ocb_t *ocb);
int syslink_unblock(resmgr_context_t *ctp, io_pulse_t *msg, syslink_ocb_t *ocb);
int syslink_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_multiproc_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_nameserver_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_notify_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapbufmp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_listmp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_messageq_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_ipc_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_heapmemmp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_gatemp_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_osalmem_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_ringio_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_ringioshm_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_clientnotifymgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_ipcmemmgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

int resmgr_stop (void);
int syslink_osalmem_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
void *mmap64_peer(pid_t pid, void *addr, size_t len, int prot, int flags, int fd, off64_t off);
void *mmap_peer(pid_t pid, void *addr, size_t len, int prot, int flags, int fd, off_t off);
int munmap_peer(pid_t pid, void *addr, size_t len);

#endif
