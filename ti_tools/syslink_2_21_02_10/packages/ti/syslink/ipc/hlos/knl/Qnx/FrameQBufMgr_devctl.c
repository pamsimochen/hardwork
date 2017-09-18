/*
 *  @file   FrameQBufMgr_devctl.c
 *
 *  @brief      OS-specific implementation of FrameQBufMgr driver for Qnx
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
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/FrameQBufMgrDefs.h>
#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>
#include <ti/syslink/ipc/_FrameQBufMgr_ShMem.h>
#include <ti/syslink/inc/FrameQBufMgrDrvDefs.h>


/* Function prototypes */
int syslink_frameqbufmgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_gethandle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_allocv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_dup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_dupv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_set_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_reset_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_translate(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_shmem_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_get_numfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_get_vnumfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_getid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_freev(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_reg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_unreg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameqbufmgr_control(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/*!
 *  @brief  Internal utility function to check if address is required
 */
static Bool _is_virt2phys_translationRequired(Ptr addr);

/*!
 *  @brief  Internal utility function to check if address is required
 */
static Bool _is_phys2virt_translationRequired(Ptr addr);

/**
 * Handler for devctl() messages for frameQBufMgr module.
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
int syslink_frameqbufmgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	switch (msg->i.dcmd)
	{

	 	  case DCMD_FRAMEQBUFMGR_GETCONFIG:
		  {
				return syslink_frameqbufmgr_getconfig(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_SETUP:
		  {
				return syslink_frameqbufmgr_setup(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_DESTROY:
		  {
				return syslink_frameqbufmgr_destroy(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_PARAMS_INIT:
		  {
				return syslink_frameqbufmgr_params_init(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_CREATE:
		  {
				return syslink_frameqbufmgr_create(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_OPEN:
		  {
				return syslink_frameqbufmgr_open(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_CLOSE:
		  {
				return syslink_frameqbufmgr_close(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_GETHANDLE:
		  {
				return syslink_frameqbufmgr_gethandle(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_ALLOC:
		  {
				return syslink_frameqbufmgr_alloc(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_ALLOCV:
		  {
				return syslink_frameqbufmgr_allocv(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_FREE:
		  {
				return syslink_frameqbufmgr_free(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_DUP:
		  {
				return syslink_frameqbufmgr_dup(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_DUPV:
		  {
				return syslink_frameqbufmgr_dupv(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_SET_NOTIFYID:
		  {
				return syslink_frameqbufmgr_set_notifyid(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_RESET_NOTIFYID:
		  {
				return syslink_frameqbufmgr_reset_notifyid(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_TRANSLATE:
		  {
				return syslink_frameqbufmgr_translate(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT:
		  {
				return syslink_frameqbufmgr_shmem_params_init(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES:
		  {
				return syslink_frameqbufmgr_get_numfreeframes(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES:
		  {
				return syslink_frameqbufmgr_get_vnumfreeframes(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_DELETE:
		  {
				return syslink_frameqbufmgr_delete(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_GETID:
		  {
				return syslink_frameqbufmgr_getid(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_FREEV:
		  {
				return syslink_frameqbufmgr_freev(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_REG_NOTIFIER:
		  {
				return syslink_frameqbufmgr_reg_notifier(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_UNREG_NOTIFIER:
		  {
				return syslink_frameqbufmgr_unreg_notifier(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQBUFMGR_CONTROL:
		  {
				return syslink_frameqbufmgr_control(ctp,msg,ocb);
		  }

		  default:
			fprintf( stderr, "Invalid DEVCTL for frameQBugMgr 0x%x \n", msg->i.dcmd);
			break;

	}
    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

/**
 * Handler for frameqbufmgr getconfig API.
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

int syslink_frameqbufmgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Config *config = (FrameQBufMgr_Config*)(cargs+1);

  			out->apiStatus = FrameQBufMgr_getConfig (config);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs));
			SETIOV(&ctp->iov[1], config, sizeof(FrameQBufMgr_Config));

			return _RESMGR_NPARTS(2);

}

int syslink_frameqbufmgr_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Config *config = (FrameQBufMgr_Config*)(cargs+1);

            out->apiStatus = FrameQBufMgr_setup (config);

			GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQBufMgr_destroy ();

            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));

}

int syslink_frameqbufmgr_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			//No Implementation on the Linux side

			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQBufMgr_E_NOTIMPLEMENTED;

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

/*
 *  ======== syslink_frameqbufmgr_create ========
 */
int syslink_frameqbufmgr_create(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    FrameQBufMgrDrv_CmdArgs *   cargs;
    Ptr                         params = NULL;
    FrameQBufMgr_ShMem_Params * shMemParams = NULL;
    Ptr                         cliNotifyMgrshMem;
    Ptr                         cliGateShMem;
    UInt32                      i;
    String                      name = NULL;
    UInt16                      index;
    SharedRegion_SRPtr          srptr;
    Ptr                         gateAttrs;
    Int                         status;

    status = 0;
    cargs = (FrameQBufMgrDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    params = (Ptr)(cargs + 1);

    if (cargs->args.create.interfaceType == FrameQBufMgr_INTERFACE_SHAREDMEM) {
        shMemParams = (FrameQBufMgr_ShMem_Params*)params;
        shMemParams->sharedAddr =
            SharedRegion_getPtr((SharedRegion_SRPtr)shMemParams->sharedAddr);

        /* shMemParams->sharedAddr is null if user has not provided memory */
        shMemParams->sharedAddrHdrBuf =
            SharedRegion_getPtr((SharedRegion_SRPtr)
            shMemParams->sharedAddrHdrBuf);
        shMemParams->sharedAddrFrmBuf =
            SharedRegion_getPtr((SharedRegion_SRPtr)
            shMemParams->sharedAddrFrmBuf);

        for (i = 0; i < shMemParams->numFreeFramePools;i++) {
            if (i == 0) {
                shMemParams->frameBufParams[i] =
                    (FrameQBufMgr_ShMem_FrameBufParams*)(shMemParams + 1);
            }
            else {
                shMemParams->frameBufParams[i] =
                    (FrameQBufMgr_ShMem_FrameBufParams*)
                    (shMemParams->frameBufParams[i-1] +
                    shMemParams->numFrameBufsInFrame[i-1]);
            }
        }

        if (cargs->args.create.nameLen > 0) {
            name = (String)(shMemParams->frameBufParams[i-1] +
                shMemParams->numFrameBufsInFrame[i-1]);
            ((FrameQBufMgr_Params*)params)->commonCreateParams.name = name;
        }

        /* open the gate instance to get a handle */
        srptr = (SharedRegion_SRPtr)shMemParams->gate;
        if (srptr != SharedRegion_INVALIDSRPTR) {
            gateAttrs = SharedRegion_getPtr(srptr);
            status = GateMP_openByAddr(gateAttrs, &shMemParams->gate);
        }
        else {
            shMemParams->gate = NULL;
        }
    }

    /* Call FrameQBufMgr_create  with the updated params */
    if (status >= 0) {
        cargs->args.create.handle = FrameQBufMgr_create(params);
    }
    else {
        cargs->args.create.handle = NULL;
    }

    if (cargs->args.create.handle == NULL) {
        cargs->apiStatus = FrameQBufMgr_E_FAIL;
        GT_setFailureReason(curTrace, GT_4CLASS, "FrameQBufMgrDrv_ioctl",
            cargs->apiStatus, "FrameQBufMgr_create failed");
    }
    else if (cargs->args.create.handle != NULL) {
        cliNotifyMgrshMem =
            _FrameQBufMgr_getCliNotifyMgrShAddr(cargs->args.create.handle);
        index = SharedRegion_getId(cliNotifyMgrshMem);
        cargs->args.create.cliNotifyMgrSharedMem =
            SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
        cliGateShMem =
            _FrameQBufMgr_getCliNotifyMgrGateShAddr(cargs->args.create.handle);
        index = SharedRegion_getId(cliGateShMem);
        cargs->args.create.cliGateSharedMem =
            SharedRegion_getSRPtr(cliGateShMem, index);
        cargs->args.create.instId =
            FrameQBufMgr_getId(cargs->args.create.handle);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(FrameQBufMgrDrv_CmdArgs);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

int syslink_frameqbufmgr_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			Ptr                 openParams = NULL;
            FrameQBufMgr_Handle handle;
            Ptr                 cliNotifyMgrshMem;
            Ptr                 cliGateShMem;
			UInt16              index;

			openParams = (Ptr)(cargs+1);

			((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.sharedAddr =
				SharedRegion_getPtr ((SharedRegion_SRPtr)((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.sharedAddr);

            if (cargs->args.open.nameLen > 0)
                ((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.name = (String)((FrameQBufMgr_ShMem_OpenParams*)openParams + 1);


            out->apiStatus = FrameQBufMgr_open (&handle, openParams);

            GT_assert (curTrace, (out->apiStatus >= 0));

            out->args.open.handle = handle;

            if (cargs->args.open.handle != NULL) {
                cliNotifyMgrshMem = _FrameQBufMgr_getCliNotifyMgrShAddr(cargs->args.open.handle);
                index = SharedRegion_getId(cliNotifyMgrshMem);
                out->args.open.cliNotifyMgrSharedMem =
                SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
                cliGateShMem = _FrameQBufMgr_getCliNotifyMgrGateShAddr(cargs->args.open.handle);
                index = SharedRegion_getId(cliGateShMem);
                out->args.open.cliGateSharedMem = SharedRegion_getSRPtr(cliGateShMem, index);
                out->args.open.instId = FrameQBufMgr_getId(cargs->args.open.handle);
            }

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));

}

int syslink_frameqbufmgr_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQBufMgr_close (&cargs->args.close.handle);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_gethandle(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	 		out->args.getHandle.handle = FrameQBufMgr_getHandle (cargs->args.getHandle.instId);

	 		return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_alloc(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Frame  frame;
            FrameQBufMgr_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              j;

            out->apiStatus = FrameQBufMgr_alloc (cargs->args.alloc.handle, &frame);

            if (frame != NULL) {
                /* Convert Frame and frame buffer address in frame to physical
                 		  * address formatso that user space api convert this physical
                 		  * address in to user space virtual format.
                 		  */
                frameBufInfo = (FrameQBufMgr_FrameBufInfo *)&(frame->frameBufInfo[0]);
                /* check to see if address translation is required (MK) */
                if (_is_virt2phys_translationRequired(
                        (Ptr)frameBufInfo[0].bufPtr)) {
	                for(j = 0; j < frame->numFrameBuffers; j++) {
	                    addr = (Ptr)frameBufInfo[j].bufPtr;
	                    phyPtr = Memory_translate(addr, Memory_XltFlags_Virt2Phys);
	                    GT_assert(curTrace,(phyPtr != NULL));
	                    frameBufInfo[j].bufPtr = (UInt32)phyPtr;
	                }
	            }

                /* frame ptr is in the header, so always translate */
                out->args.alloc.frame = Memory_translate(frame,Memory_XltFlags_Virt2Phys);
            }
            else {
                out->args.alloc.frame = NULL;
            }

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));

}

int syslink_frameqbufmgr_allocv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Frame  *framePtr = NULL;
            FrameQBufMgr_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              *pFreeQId = NULL;
            UInt32              i;
            UInt32              j;
            Bool                virt2Phys_isRequired;
            
            framePtr = (FrameQBufMgr_Frame*)(cargs+1);

			pFreeQId = (UInt32*)(framePtr + cargs->args.allocv.numFrames);

            out->apiStatus = FrameQBufMgr_allocv (cargs->args.allocv.handle,
                                                 framePtr,
                                                 (Ptr)pFreeQId,
                                                 (UInt8*)&cargs->args.allocv.numFrames);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs));

             if ((out->apiStatus >= 0) && (cargs->args.allocv.numFrames > 0)) {

                  /* check if address translation is required (MK) */
                  virt2Phys_isRequired = _is_virt2phys_translationRequired(
                        (Ptr)framePtr[0]->frameBufInfo[0].bufPtr);
                                
                  for (i = 0; i < cargs->args.allocv.numFrames; i++) {

                            /* Convert Frame and frame buffer address in frame to
                             * physical address formatso that user space api convert
                             * this physical address in to user space virtual format.
                             */
                            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                &(framePtr[i]->frameBufInfo[0]);
                            if (virt2Phys_isRequired) {
                                for(j = 0; j < framePtr[i]->numFrameBuffers; j++) {
                                    addr = (Ptr)frameBufInfo[j].bufPtr;
                                    phyPtr = Memory_translate(
                                                     addr,
                                                     Memory_XltFlags_Virt2Phys);
                                    GT_assert(curTrace,(phyPtr != NULL));
                                    frameBufInfo[j].bufPtr = (UInt32)phyPtr;
                                }
                            }

			                /* always translate header pointers */
                            framePtr[i] = Memory_translate(
                                                     framePtr[i],
                                                     Memory_XltFlags_Virt2Phys);
                        }

 						SETIOV(&ctp->iov[1], framePtr, (sizeof (UInt32) * cargs->args.allocv.numFrames));
						return _RESMGR_NPARTS(2);

                    }

			 return _RESMGR_NPARTS(1);
}



int syslink_frameqbufmgr_free(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Frame          frame;
            FrameQBufMgr_FrameBufInfo   *frameBufInfo;
            Ptr                         virtPtr;
            Ptr                         addr;
            UInt32                      i;

            /* Convert frame address and frame buffer address in to knl virtual
             		  * format.
             		  */
            frame = Memory_translate (cargs->args.free.frame,
                                      Memory_XltFlags_Phys2Virt);
            GT_assert(curTrace,(frame != NULL));
            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                                     &(frame->frameBufInfo[0]);

            /* Frame buffer addresses will only be converted when Heap alloc'd*/
            if (_is_phys2virt_translationRequired(
                               (Ptr)frameBufInfo[0].bufPtr)) {
	            for(i = 0; i < frame->numFrameBuffers; i++) {
	                addr = (Ptr)frameBufInfo[i].bufPtr;
	                virtPtr = Memory_translate(addr, 
					    Memory_XltFlags_Phys2Virt);
	                GT_assert(curTrace,(virtPtr != NULL));
	                frameBufInfo[i].bufPtr = (UInt32)virtPtr;
	            }
            }
            
            out->apiStatus = FrameQBufMgr_free (cargs->args.free.handle, frame);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));

}

int syslink_frameqbufmgr_dup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Frame          frame;
            FrameQBufMgr_Frame          *dupedFramePtr;
            UInt32                      i;

            dupedFramePtr =  (FrameQBufMgr_Frame*)(cargs+1);

                /* Convert original frame address and frame buffer address in to
                 		  * knl virtual format.
                 		  */
                frame = Memory_translate (cargs->args.dup.frame,
                                          Memory_XltFlags_Phys2Virt);
                GT_assert(curTrace,(frame != NULL));


                out->apiStatus = FrameQBufMgr_dup (cargs->args.dup.handle,
                                           frame,
                                           dupedFramePtr,
                                           cargs->args.dup.numDupedFrames);

				SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs));

                if (out->apiStatus >= 0) {
                    for (i = 0; i < cargs->args.dup.numDupedFrames; i++) {
                        /* Convert Frame and frame buffer address in frame to
                         			  * physical address formatso that user space api convert
                         			  * this physical address in to user space virtual format.
                         			  */
                        dupedFramePtr[i] =  Memory_translate(
                                                 dupedFramePtr[i],
                                                 Memory_XltFlags_Virt2Phys);
                    }

					SETIOV(&ctp->iov[1], dupedFramePtr, (  sizeof (UInt32) * cargs->args.dup.numDupedFrames));

					return _RESMGR_NPARTS(2);
                }

				return _RESMGR_NPARTS(1);

}


int syslink_frameqbufmgr_dupv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Frame          *framePtr = NULL;
            FrameQBufMgr_Frame          **dupedFramePtr = NULL;
            FrameQBufMgr_FrameBufInfo   *frameBufInfo ;
            Ptr                         virtPtr;
            Ptr                         phyPtr;
            Ptr                         addr;
            UInt32                      i;
            UInt32                      j;
            UInt32                      k;

			framePtr = (FrameQBufMgr_Frame*)(cargs + 1);

			for (i = 0;i < cargs->args.dupv.numFrames; i++) {

				if(i == 0)
					dupedFramePtr[i] = (FrameQBufMgr_Frame*)(framePtr + cargs->args.dupv.numFrames);
				else
					dupedFramePtr[i] = (FrameQBufMgr_Frame*)(dupedFramePtr[i-1] + cargs->args.dupv.numDupedFrames);
			}


            for (i = 0u; i < cargs->args.dupv.numFrames; i++) {
                    /* Convert original frame address and frame buffer address
                     		  * in to knl virtual format.
                     		  */
                    framePtr[i] = Memory_translate (framePtr[i],Memory_XltFlags_Phys2Virt);
                    GT_assert(curTrace,(framePtr[i] != NULL));
                    frameBufInfo = (FrameQBufMgr_FrameBufInfo *)&(framePtr[i]->frameBufInfo[0u]);

                    for(i = 0u; i < framePtr[i]->numFrameBuffers; i++) {
                        addr = (Ptr)frameBufInfo[i].bufPtr;
                        virtPtr = Memory_translate(addr,
                                                   Memory_XltFlags_Phys2Virt);
                        GT_assert(curTrace,(virtPtr != NULL));
                        frameBufInfo[i].bufPtr = (UInt32)virtPtr;
                    }
                }
                out->apiStatus = FrameQBufMgr_dupv (cargs->args.dupv.handle,
                                            framePtr,
                                            dupedFramePtr,
                                            cargs->args.dupv.numDupedFrames,
                                            cargs->args.dupv.numFrames);

				SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs));

                if (out->apiStatus >= 0u) {
                    for (i = 0u;i < cargs->args.dupv.numFrames; i++) {
                        for (j = 0u; j < cargs->args.dup.numDupedFrames; j++) {
                            /* Convert Frame and frame buffer address in frame
                             			 * to physical address formatso that user space api
                             			 * convert this physical address in to user space
                             			 * virtual format.
                             			 */
                            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)&(dupedFramePtr[i][j]->frameBufInfo[0u]);
                            for(k = 0u;k < dupedFramePtr[i][j]->numFrameBuffers; k++) {
                                addr = (Ptr)frameBufInfo[j].bufPtr;
                                phyPtr = Memory_translate(
                                                     addr,
                                                     Memory_XltFlags_Virt2Phys);
                                GT_assert(curTrace,(phyPtr != NULL));
                                frameBufInfo[k].bufPtr = (UInt32)phyPtr;
                            }
                            dupedFramePtr[i][j] =  Memory_translate(
                                                     dupedFramePtr[i][j],
                                                     Memory_XltFlags_Virt2Phys);
                        }


						SETIOV(&ctp->iov[i+1], dupedFramePtr[i],(  sizeof (UInt32) * cargs->args.dupv.numDupedFrames));

                    }
					return _RESMGR_NPARTS(i+1);
                }

           return _RESMGR_NPARTS(1);
}

int syslink_frameqbufmgr_set_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = _FrameQBufMgr_setNotifyId (cargs->args.setNotifyId.handle,cargs->args.setNotifyId.notifyId);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_reset_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = _FrameQBufMgr_resetNotifyId (cargs->args.resetNotifyId.handle,cargs->args.resetNotifyId.notifyId);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_translate(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			Ptr     srcAddr;
            Ptr     dstAddr;

            srcAddr = (Ptr) Memory_translate ((Ptr)cargs->args.translate.srcAddr,
                                              Memory_XltFlags_Phys2Virt);
            GT_assert (curTrace, (srcAddr != NULL));
            out->apiStatus = FrameQBufMgr_translateAddr (
                             cargs->args.translate.handle,
                            &dstAddr,
                            cargs->args.translate.dstAddrType,
                            srcAddr,
                            cargs->args.translate.srcAddrType,
                            cargs->args.translate.bufType);
            if (out->apiStatus >= 0) {
                out->args.translate.dstAddr = Memory_translate(
                                                    (Ptr)dstAddr,
                                                     Memory_XltFlags_Virt2Phys);
            }

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_shmem_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

			FrameQBufMgr_ShMem_Params* params;
			params = (FrameQBufMgr_ShMem_Params*)(cargs + 1);

            FrameQBufMgr_ShMem_Params_init(params);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs));
			SETIOV(&ctp->iov[1], params, sizeof(FrameQBufMgr_ShMem_Params));

			return _RESMGR_NPARTS(2);

}

int syslink_frameqbufmgr_get_numfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			UInt32  numFreeFrames;

            out->apiStatus = FrameQBufMgr_getNumFreeFrames(cargs->args.getNumFreeFrames.handle,&numFreeFrames);
            out->args.getNumFreeFrames.numFreeFrames = numFreeFrames;

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_get_vnumfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			/* Get the available frames in filled queue 0 of FrameQBufMgr */
            UInt32  *freeQueueId = NULL;
            UInt32  *numFreeFrames;

            numFreeFrames= (UInt32*)(cargs+1);

			freeQueueId  = (UInt32*)(numFreeFrames + cargs->args.getvNumFreeFrames.numFreeQids);

            out->apiStatus = FrameQBufMgr_getvNumFreeFrames(
                                        cargs->args.getvNumFreeFrames.handle,
                                        numFreeFrames,
                                        (Ptr)freeQueueId,
                                        cargs->args.getvNumFreeFrames.numFreeQids);


			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs));
			SETIOV(&ctp->iov[1], numFreeFrames, (sizeof (UInt32) * cargs->args.getvNumFreeFrames.numFreeQids));

			return _RESMGR_NPARTS(2);


}

/*
 *  ======== syslink_frameqbufmgr_delete ========
 */
int syslink_frameqbufmgr_delete(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    FrameQBufMgrDrv_CmdArgs *   cargs;
    FrameQBufMgr_Handle *       handlePtr;
    GateMP_Handle               gate;

    cargs = (FrameQBufMgrDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = (FrameQBufMgr_Handle *)(&cargs->args.deleteInstance.handle);

    /* save gate handle, need to close it after delete */
    gate = FrameQBufMgr_getGate(*handlePtr);

    /* invoke the requested api */
    cargs->apiStatus = FrameQBufMgr_delete(handlePtr);

    GT_assert(curTrace, (cargs->apiStatus >= 0));

    /* close the gate instance if not the default gate */
    if (gate != GateMP_getDefaultRemote()) {
        GateMP_close(&gate);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(FrameQBufMgrDrv_CmdArgs);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

int syslink_frameqbufmgr_getid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

	 		out->args.getId.instId = FrameQBufMgr_getId (cargs->args.getId.handle);

	 		return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_freev(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQBufMgr_Frame          *framePtr;
            FrameQBufMgr_FrameBufInfo   *frameBufInfo;
            Ptr                         virtPtr;
            Ptr                         addr;
            UInt32                      i;
            UInt32                      j;

            framePtr =  (FrameQBufMgr_Frame*)(cargs+1);

                for (i = 0; i < cargs->args.freev.numFrames; i++) {
                    /* Convert frame address and frame buffer address in to knl
                     		  * virtual format.
                     		  */
                    framePtr[i] = Memory_translate (framePtr[i],
                                              Memory_XltFlags_Phys2Virt);
                    GT_assert(curTrace,(framePtr[i] != NULL));
                    frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                                &(framePtr[i]->frameBufInfo[0]);
	                /* Convert Frame buffer addresses only when Heap alloc'd*/
	                if (_is_phys2virt_translationRequired(
	                                   (Ptr)frameBufInfo[0].bufPtr)) {
	                    for(j = 0; j < framePtr[i]->numFrameBuffers; j++) {
	                        addr = (Ptr)frameBufInfo[j].bufPtr;
	                        virtPtr = Memory_translate(addr,
	                            Memory_XltFlags_Phys2Virt);
	                        GT_assert(curTrace,(virtPtr != NULL));
	                        frameBufInfo[j].bufPtr = (UInt32)virtPtr;
	                    }
	                }
                }

                out->apiStatus = FrameQBufMgr_freev (cargs->args.freev.handle,
                                             framePtr,
                                             cargs->args.freev.numFrames);

                GT_assert (curTrace, (out->apiStatus >= 0));

				return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));


}

int syslink_frameqbufmgr_reg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQBufMgr_registerNotifier (cargs->args.regNotifier.handle,&cargs->args.regNotifier.notifyParams);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_unreg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){


			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQBufMgr_unregisterNotifier (cargs->args.unregNotifier.handle);

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

int syslink_frameqbufmgr_control(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQBufMgrDrv_CmdArgs *cargs = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQBufMgrDrv_CmdArgs *out = (FrameQBufMgrDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			Ptr arg;

			arg = (Ptr)(cargs + 1);

            out->apiStatus = FrameQBufMgr_control(cargs->args.control.handle,
                                                  cargs->args.control.cmd,
                                                  arg);

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQBufMgrDrv_CmdArgs)));
}

/*
 *  Internal utility function:
 *  NOTE:   Translate frame buffer pointers only when createHeap is TRUE.
 *          otherwise do no buffer pointer translation,  expect Physical
 *          addresses from the user-mode and leave them as-is, since they
 *          are no longer mapped to Master Kernel Virtual.
 *          This also means that Kernel APIs can not access the frame-buffer
 *          and will only serve to convert them to Portable pointers where
 *          necessary.
 */
static Bool _is_phys2virt_translationRequired(Ptr addr)
{
    UInt16              regionId;

    /*  this should always obtain a valid region id for both
     *  heap  or no-heap SR configuration.
     */
    regionId = _SharedRegion_getIdPhys(addr);

    /*
     * If frame buffer shared region has createHeap == FALSE
     * then we will not convert virtual ptrs to physical.
     * In this case virtual == physical.
     */
    if (SharedRegion_getHeap(regionId) == NULL) {
        /*
         * region has no heap => so the address is already Physical
         * No translation is required.
         */
        return FALSE;

    } else {
        /*
         * region has heap => so the address is virtual and it needs to
         * to be translated virtual to physical.
         */
        return TRUE;
    }
}
 
/*
 *  Internal utility function
 */
static Bool _is_virt2phys_translationRequired(Ptr addr)
{
    UInt16              regionId;
    /*
     * SharedRegion_getId()  should always obtain a valid region id whether
     * Shared Region was created with or without a heap.
     */
    regionId = SharedRegion_getId(addr);

    /*
     * If frame buffer shared region has createHeap == FALSE
     * then we will not convert virtual ptrs to physical.
     * In this case virtual == physical.
     */
    if (SharedRegion_getHeap(regionId) == NULL) {
        /*
         * region has no heap => so the address is already Physical
         * No translation is required.
         */
        return FALSE;

    } else {
        /*
         * region has heap => so the address is virtual and it needs to
         * to be translated virtual to physical.
         */
        return TRUE;
    }
}