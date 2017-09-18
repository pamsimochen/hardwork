/*
 *  @file   OsalMem_devctl.c
 *
 *  @brief      devctl handler for OsalMem layer of OSAL.
 *
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
#include <ti/syslink/inc/TraceDrvDefs.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/inc/TraceDrv.h>

/* QNX specific header include */
#include "proto.h"
#include "dcmd_syslink.h"
#include <arm/mmu.h>



/* Function prototypes */
int syslink_trace_settrace(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/**
 * Handler for devctl() messages for OsalMem module.
 *
 * \param ctp   Thread's associated context information.
 * \param msg   The actual devctl() message.
 * \param ocb   OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK      Success.
 * \retval ENOTSUP  Unsupported devctl().
 */
int syslink_osalmem_devctl(resmgr_context_t *ctp, io_devctl_t *msg,
    syslink_ocb_t *ocb)
{
    OsalMemMap_CmdArgs *in  = (OsalMemMap_CmdArgs *) (_DEVCTL_DATA (msg->i));
    OsalMemMap_CmdArgs *out = (OsalMemMap_CmdArgs *) (_DEVCTL_DATA (msg->o));
    Int                 rc;
    Memory_MapInfo      minfo;
    Memory_UnmapInfo    uminfo;

    pid_t pid; // pid connecting to us
    pid = ocb->pid; // we have this stored in the ocb

    switch (msg->i.dcmd)
    {
        case DCMD_OSALMEM_MMAP:
            minfo.src      = in->paddr;
            minfo.size     = in->len;
            minfo.isCached = in->isCached;

            rc = MemoryOS_peerMap(&minfo, pid);
            if (rc != MEMORYOS_S_SUCCESS) {
                out->status = -1;
            }
            out->vaddr = (void *)minfo.dst;

            return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(*out)));
            break;

        /* maybe we don't need this */
		case DCMD_OSALMEM_MUNMAP:  
            uminfo.addr = (UInt32)in->vaddr;
            uminfo.size = in->len;
            uminfo.isCached = in->isCached;

            rc = MemoryOS_peerUnmap(&uminfo, pid);
            if (rc != MEMORYOS_S_SUCCESS) {
                out->status = -1;
            }
            return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(*out)));
            break;

        case DCMD_TRACEDRV_SETTRACE:
            return syslink_trace_settrace( ctp, msg, ocb);
            break;

        default:
            out->status = ENOSYS;
            fprintf( stderr, "Invalid DEVCTL for OsalMem 0x%x \n", msg->i.dcmd);
    }
 	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + 
        sizeof(OsalMemMap_CmdArgs)));
}


/**
 * Handler for messageq put API.
 *
 * \param ctp   Thread's associated context information.
 * \param msg   The actual devctl() message.
 * \param ocb   OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK      Success.
 * \retval ENOTSUP  Unsupported devctl().
 */
int syslink_trace_settrace(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

    TraceDrv_CmdArgs *      cargs = (TraceDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    cargs->args.setTrace.type = GT_TraceType_Kernel;

    TraceDrv_ioctl(CMD_TRACEDRV_SETTRACE, cargs);

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(TraceDrv_CmdArgs)));

}

