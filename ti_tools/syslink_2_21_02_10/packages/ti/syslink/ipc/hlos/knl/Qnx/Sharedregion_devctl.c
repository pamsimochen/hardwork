/*
 *  @file   Sharedregion_devctl.c
 *
 *  @brief      OS-specific implementation of Sharedregion driver for Qnx
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
#include <ti/syslink/utils/Gate.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/SharedRegionDrvDefs.h>

/* Function prototypes */
int syslink_sharedregion_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_getregioninfo(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_start(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_stop(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_setentry(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_clearentry(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_getheap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_sharedregion_reservememory(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) ;
int syslink_sharedregion_clearreservedmemory(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) ;

/**
 * Handler for devctl() messages for SHAREDREGION module.
 *
 * Handles special devctl() messages that we export for control.
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
int syslink_sharedregion_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

    switch (msg->i.dcmd)
    {

     case DCMD_SHAREDREGION_GETCONFIG:
     {
         return syslink_sharedregion_getconfig( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_SETUP:
     {
         return syslink_sharedregion_setup( ctp, msg, ocb);
     }
     break;


     case DCMD_SHAREDREGION_GETREGIONINFO:
     {
         return syslink_sharedregion_getregioninfo( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_DESTROY:
     {
         return syslink_sharedregion_destroy( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_START:
     {
         return syslink_sharedregion_start( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_STOP:
     {
         return syslink_sharedregion_stop( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_ATTACH:
     {
         return syslink_sharedregion_attach( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_DETACH:
     {
         return syslink_sharedregion_detach( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_SETENTRY:
     {
         return syslink_sharedregion_setentry( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_CLEARENTRY:
     {
         return syslink_sharedregion_clearentry( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_GETHEAP:
     {
         return syslink_sharedregion_getheap( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_RESERVEMEMORY:
     {
         return syslink_sharedregion_reservememory( ctp, msg, ocb);
     }
     break;

     case DCMD_SHAREDREGION_CLEARRESERVEDMEMORY:
     {
         return syslink_sharedregion_clearreservedmemory( ctp, msg, ocb);
     }
     break;

     default:
        fprintf( stderr, "Invalid DEVCTL for Sharedregion 0x%x \n", msg->i.dcmd);
        break;

    }

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (SharedRegionDrv_CmdArgs)));

}

/**
 * Handler for shared region get config API.
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
int syslink_sharedregion_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    SharedRegion_Config *config = (SharedRegion_Config *)(cargs+1);

    SharedRegion_getConfig (config);
    cargs->apiStatus = SharedRegion_S_SUCCESS;

    SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(SharedRegionDrv_CmdArgs));
    SETIOV(&ctp->iov[1], config, sizeof(SharedRegion_Config));

    return _RESMGR_NPARTS(2);
}

/**
 * Handler for shared region setup API.
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
int syslink_sharedregion_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
    SharedRegionDrv_CmdArgs *       out  = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    SharedRegion_Config   *config = (SharedRegion_Config*)(cargs+1);
    SharedRegion_Region   *region = (SharedRegion_Region *)(config+1);
    UInt16           i;

    out->apiStatus = SharedRegion_setup (config);
    GT_assert (curTrace, (out->apiStatus >= 0));

    for (i = 0; i < config->numEntries; i++) {

        SharedRegion_getRegionInfo (i, &region[i]);

        if (region[i].entry.isValid == TRUE) {
            /* MK: If createHeap==FALSE, pass the base unaltered
             *     since it is the Physical address already.
             */
            if (region[i].entry.createHeap == TRUE) {
                /* Convert the kernel virtual address to physical
                 * addresses */
                region[i].entry.base = MemoryOS_translate (
                                        (Ptr)region[i].entry.base,
                                        Memory_XltFlags_Virt2Phys);
            }
            GT_assert (curTrace, (region[i].entry.base != NULL));
         }
     }

    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_SHAREDREGION_DESTROY, (int)NULL, NULL);
    }

    SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(SharedRegionDrv_CmdArgs));
    SETIOV(&ctp->iov[1], config, sizeof(SharedRegion_Config) );
    SETIOV(&ctp->iov[2], region, config->numEntries*sizeof(SharedRegion_Region));

    return _RESMGR_NPARTS(3);

}

/**
 * Handler for shared region get region info API.
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
int syslink_sharedregion_getregioninfo(
        resmgr_context_t *      ctp,
        io_devctl_t *           msg,
        syslink_ocb_t *         ocb)
{
    SharedRegionDrv_CmdArgs *   cmdArgs;
    SharedRegion_Region   *     region;
    SharedRegion_Config         config;
    UInt16                      i;

    cmdArgs = (SharedRegionDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    region = (SharedRegion_Region *)(cmdArgs + 1);
    SharedRegion_getConfig(&config);

    for (i = 0; i < config.numEntries; i++) {
        SharedRegion_getRegionInfo(i, &region[i]);

        if (region[i].entry.isValid) {
            if (region[i].entry.createHeap) {
                /* convert the kernel virtual address to physical addresses */
                region[i].entry.base = MemoryOS_translate(
                    (Ptr)region[i].entry.base, Memory_XltFlags_Virt2Phys);
                GT_assert(curTrace, (region[i].entry.base != NULL));
            }
            /* else, nothing to do, entry.base is already physical address */
        }
        else {
            region[i].entry.base = NULL;
        }
    }

    SETIOV(&ctp->iov[0], &msg->o,
        sizeof(msg->o) + sizeof(SharedRegionDrv_CmdArgs));
    SETIOV(&ctp->iov[1], region,
        (sizeof(SharedRegion_Region) * config.numEntries));

    return(_RESMGR_NPARTS(2));  /* config.numEntries */
}

/**
 * Handler for shared region destroy API.
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
int syslink_sharedregion_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


    SharedRegionDrv_CmdArgs *       out = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

     out->apiStatus = SharedRegion_destroy ();
     GT_assert (curTrace, (out->apiStatus >= 0));

    if (out->apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_SHAREDREGION_DESTROY, (int)NULL, NULL);
    }

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));
}

/**
 * Handler for shared region start API.
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
int syslink_sharedregion_start(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


    SharedRegionDrv_CmdArgs *       out = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->apiStatus  = SharedRegion_start ();
    GT_assert (curTrace, (out->apiStatus >= 0));

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));


}

/**
 * Handler for shared region stop API.
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
int syslink_sharedregion_stop(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {
    SharedRegionDrv_CmdArgs *       out = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->apiStatus = SharedRegion_stop ();
    GT_assert (curTrace, (out->apiStatus >= 0));

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));


}


/**
 * Handler for shared region attach API.
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
int syslink_sharedregion_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
    SharedRegionDrv_CmdArgs *       out  = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->apiStatus = SharedRegion_attach (cargs->args.attach.remoteProcId);
    GT_assert (curTrace, (out->apiStatus >= 0));

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));

}

/**
 * Handler for shared region detach API.
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
int syslink_sharedregion_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
    SharedRegionDrv_CmdArgs *       out  = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    out->apiStatus = SharedRegion_detach (cargs->args.attach.remoteProcId);
    GT_assert (curTrace, (out->apiStatus >= 0));

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));

}

/**
 * Handler for shared region set entry API.
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
int syslink_sharedregion_setentry(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
    SharedRegionDrv_CmdArgs *       out  = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    SharedRegion_Entry *entry = (SharedRegion_Entry*)(cargs+1);

    if (entry->createHeap == TRUE) {
        entry->base = (Ptr)Memory_translate (
                    (Ptr)cargs->args.setEntry.entry.base,
                    Memory_XltFlags_Phys2Virt);
    } 
    else {
        entry->base = (Ptr)cargs->args.setEntry.entry.base;
    }


    GT_assert (curTrace, (entry->base != (UInt32)NULL));
    out->apiStatus = SharedRegion_setEntry (cargs->args.setEntry.id,
                                            entry);
    GT_assert (curTrace, (out->apiStatus >= 0));

    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_SHAREDREGION_CLEARENTRY, (int)cargs->args.setEntry.id, NULL);
    }

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));
}

/**
 * Handler for shared region clear entry API.
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
int syslink_sharedregion_clearentry(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


        SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
        SharedRegionDrv_CmdArgs *       out  = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

        out->apiStatus = SharedRegion_clearEntry (cargs->args.clearEntry.id);
    if (out->apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_SHAREDREGION_CLEARENTRY, (int)cargs->args.clearEntry.id, NULL);
    }
        return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));

}

/**
 * Handler for shared region get heap API.
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
int syslink_sharedregion_getheap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
    SharedRegionDrv_CmdArgs *       out  = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

    IHeap_Handle  heapHandle = NULL;

    heapHandle = (IHeap_Handle) SharedRegion_getHeap (
                                                    cargs->args.getHeap.id);
    GT_assert (curTrace, (heapHandle != NULL));

    out->args.getHeap.heapHandle = heapHandle;

    if (out->args.getHeap.heapHandle != NULL)
        cargs->apiStatus = SharedRegion_S_SUCCESS;
    else
        cargs->apiStatus = SharedRegion_E_FAIL;

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));

}

/**
 * Handler for shared region reserve memory API.
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
int syslink_sharedregion_reservememory(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {


    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    SharedRegion_reserveMemory (cargs->args.reserveMemory.id,
                                        cargs->args.reserveMemory.size);
    cargs->apiStatus = SharedRegion_S_SUCCESS;

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));

}

/**
 * Handler for shared region clear reserved memory API.
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
int syslink_sharedregion_clearreservedmemory(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {
    SharedRegionDrv_CmdArgs *       cargs = (SharedRegionDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

    SharedRegion_clearReservedMemory ();
    cargs->apiStatus = SharedRegion_S_SUCCESS;

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(SharedRegionDrv_CmdArgs)));
}
