/*
 *  @file   resMgr_devctl.c
 *
 *  @brief      devctl handler for resource manager
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
#include <ti/syslink/inc/MultiProcDrvDefs.h>
#include "dcmd_syslink.h"


/**
 * Handler for devctl() messages.
 *
 * Handles special devctl() messages that we export for control.
 *
 * \param ctp   Thread's associated context information.
 * \param msg   The actual devctl() message.
 * \param ocb   OCB associated with client's session.
 *
 * \return POSIX errno value.
 *
 * \retval EOK          Success.
 * \retval ENOTSUP      Unsupported devctl().
 */
int syslink_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
    int status = 0;
    int commandClass;

    commandClass = (unsigned char)(msg->i.dcmd >> 8);
    iofunc_unlock_ocb_default(ctp, msg, &ocb->ocb);

    switch (commandClass) {
        case _DCMD_SYSLINK_MULTIPROC:
            status = syslink_multiproc_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_NAMESERVER:
            status = syslink_nameserver_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_SHAREDREGION:
            status = syslink_sharedregion_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_NOTIFY:
            status = syslink_notify_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_HEAPBUFMP:
            status = syslink_heapbufmp_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_LISTMP:
            status = syslink_listmp_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_MESSAGEQ:
            status = syslink_messageq_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_IPC:
            status = syslink_ipc_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_HEAPMEMMP:
            status = syslink_heapmemmp_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_GATEMP:
            status = syslink_gatemp_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_OSALMEM:
            status = syslink_osalmem_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_PROCMGR:
            status = syslink_procmgr_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_CLIENTNOTIFYMGR:
            status = syslink_clientnotifymgr_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_FRAMEQ:
            status = syslink_frameq_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_FRAMEQBUFMGR:
            status = syslink_frameqbufmgr_devctl(ctp, msg, ocb);
            break;

        case _DCMD_SYSLINK_IPCMEMMGR:
            status = syslink_ipcmemmgr_devctl(ctp, msg, ocb);
            break;

        default:
            fprintf(stderr, "Command Class not supported 0x%x\n",
                (unsigned int)commandClass);
            status = -1;
            break;
    } /* switch */

    iofunc_lock_ocb_default(ctp, msg, &ocb->ocb);
    return(status);
}
