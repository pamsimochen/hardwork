/*
 *  @file   FrameQ_devctl.c
 *
 *  @brief      OS-specific implementation of FrameQ driver for Qnx
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
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/inc/FrameQDrvDefs.h>
#include <ti/syslink/FrameQ_ShMem.h>
#include <ti/syslink/inc/_FrameQ_ShMem.h>


/* Function prototypes */
int syslink_frameq_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_put(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_putv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_get(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_getv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_get_numframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_get_vnumframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_control(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_set_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_reset_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_shmem_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_shmem_memreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_get_numfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_get_vnumfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_reg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_frameq_unreg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

/*!
 *  @brief  Internal utility function to check if address is required
 */
static Bool _is_virt2phys_translationRequired(Ptr addr);

/*!
 *  @brief  Internal utility function to check if address is required
 */
static Bool _is_phys2virt_translationRequired(Ptr addr);

/**
 * Handler for devctl() messages for frameQ module.
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
int syslink_frameq_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	switch (msg->i.dcmd)
	{

	 	  case DCMD_FRAMEQ_GETCONFIG:
		  {
				return syslink_frameq_getconfig(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_SETUP:
		  {
				return syslink_frameq_setup(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_DESTROY:
		  {
				return syslink_frameq_destroy(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_PARAMS_INIT:
		  {
				return syslink_frameq_params_init(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_CREATE:
		  {
				return syslink_frameq_create(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_DELETE:
		  {
				return syslink_frameq_delete(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_OPEN:
		  {
				return syslink_frameq_open(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_CLOSE:
		  {
				return syslink_frameq_close(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_PUT:
		  {
				return syslink_frameq_put(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_PUTV:
		  {
				return syslink_frameq_putv(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_GET:
		  {
				return syslink_frameq_get(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_GETV:
		  {
				return syslink_frameq_getv(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_GET_NUMFRAMES:
		  {
				return syslink_frameq_get_numframes(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_GET_VNUMFRAMES:
		  {
				return syslink_frameq_get_vnumframes(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_CONTROL:
		  {
				return syslink_frameq_control(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_SET_NOTIFYID:
		  {
				return syslink_frameq_set_notifyid(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_RESET_NOTIFYID:
		  {
				return syslink_frameq_reset_notifyid(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_SHMEM_PARAMS_INIT:
		  {
				return syslink_frameq_shmem_params_init(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_SHMEM_MEMREQ:
		  {
				return syslink_frameq_shmem_memreq(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_GET_NUMFREEFRAMES:
		  {
				return syslink_frameq_get_numfreeframes(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_GET_VNUMFREEFRAMES:
		  {
				return syslink_frameq_get_vnumfreeframes(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_REG_NOTIFIER:
		  {
				return syslink_frameq_reg_notifier(ctp,msg,ocb);
	 	  }

		  case DCMD_FRAMEQ_UNREG_NOTIFIER:
		  {
				return syslink_frameq_unreg_notifier(ctp,msg,ocb);
	 	  }

		default:
		fprintf( stderr, "Invalid DEVCTL for frameQ 0x%x \n", msg->i.dcmd);
		break;

	}

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (FrameQDrv_CmdArgs)));

}

/**
 * Handler for frameq getconfig API.
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
int syslink_frameq_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

			FrameQ_Config *config = (FrameQ_Config*)(cargs+1);

  			FrameQ_getConfig (config);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQDrv_CmdArgs));
			SETIOV(&ctp->iov[1], config, sizeof(FrameQ_Config));

			return _RESMGR_NPARTS(2);
}


int syslink_frameq_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQ_Config *config = (FrameQ_Config*)(cargs+1);

            out->apiStatus = FrameQ_setup (config);

			GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}


int syslink_frameq_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQ_destroy ();

			GT_assert (curTrace, (	out->apiStatus  >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			//Linux side implementation commented out

			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQ_E_NOTIMPLEMENTED;

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}

/*
 *  ======== syslink_frameq_create ========
 */
int syslink_frameq_create(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    FrameQDrv_CmdArgs *         cargs;
    Ptr                         params = NULL;
    FrameQ_ShMem_Params *       shMemParams;
    Ptr                         mgrshMem;
    Ptr                         gateShMem;
    String                      name = NULL;
    UInt16                      index;
    SharedRegion_SRPtr          srptr;
    Ptr                         gateAttrs;
    Int                         status;

    status = 0;
    cargs = (FrameQDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    params = (Ptr)(cargs+1);

    if (cargs->args.create.nameLen > 0) {
        name = ((FrameQ_Params*)params)->commonCreateParams.name =
            (String)((FrameQ_ShMem_Params*)params+1);
    }

    if (cargs->args.create.interfaceType == FrameQ_INTERFACE_SHAREDMEM) {
        shMemParams = (FrameQ_ShMem_Params*)params;
        shMemParams->sharedAddr =
            SharedRegion_getPtr((SharedRegion_SRPtr)shMemParams->sharedAddr);

        if ((SharedRegion_SRPtr)shMemParams->frameQBufMgrSharedAddr ==
            SharedRegion_INVALIDSRPTR) {
            shMemParams->frameQBufMgrSharedAddr = NULL;
        }
        else {
            shMemParams->frameQBufMgrSharedAddr =
                SharedRegion_getPtr((SharedRegion_SRPtr)
                    shMemParams->frameQBufMgrSharedAddr);
        }
        /* Copy FrameQ BufMgr Name */
        if (cargs->args.create.nameLen > 0) {
            if (cargs->args.create.fQBMnameLen > 0) {
                shMemParams->frameQBufMgrName =
                    (String)(name + cargs->args.create.nameLen);
            }
        }
        else if (cargs->args.create.fQBMnameLen > 0) {
            shMemParams->frameQBufMgrName =
                (String)((FrameQ_ShMem_Params*)params+1);
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

    /* Call FrameQ_create  with the updated params */
    if (status >= 0) {
        cargs->args.create.handle = FrameQ_create(params);
    }
    else {
        cargs->args.create.handle = NULL;
    }

    if (cargs->args.create.handle != NULL) {
        /* Get the required info to be able to create ClientNotify
         * Mgr instane at user space
         */
        mgrshMem = _FrameQ_getCliNotifyMgrShAddr(cargs->args.create.handle);
        index = SharedRegion_getId(mgrshMem);
        cargs->args.create.cliNotifyMgrSharedMem =
            SharedRegion_getSRPtr(mgrshMem, index);
        gateShMem=_FrameQ_getCliNotifyMgrGateShAddr(cargs->args.create.handle);
        index = SharedRegion_getId(gateShMem);
        cargs->args.create.cliGateSharedMem =
            SharedRegion_getSRPtr(gateShMem, index);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(FrameQDrv_CmdArgs);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

/*
 *  ======== syslink_frameq_delete ========
 */
int syslink_frameq_delete(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    FrameQDrv_CmdArgs * cargs;
    FrameQ_Handle *     handlePtr;
    GateMP_Handle       gate;

    cargs = (FrameQDrv_CmdArgs *)(_DEVCTL_DATA(msg->i));
    handlePtr = (FrameQ_Handle *)(&cargs->args.deleteInstance.handle);

    /* save gate handle, need to close it after delete */
    gate = FrameQ_getGate(*handlePtr);

    /* invoke the requested api */
    cargs->apiStatus = FrameQ_delete(handlePtr);

    GT_assert(curTrace, (cargs->apiStatus >= 0));

    /* close the gate instance if not the default gate */
    if (gate != GateMP_getDefaultRemote()) {
        GateMP_close(&gate);
    }

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.nbytes = sizeof(FrameQDrv_CmdArgs);

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

int syslink_frameq_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			Ptr                     openParams = NULL;
			UInt16                  index;
            FrameQ_Handle           handle;
            Ptr                     mgrshMem;
            Ptr                     gateShMem;

			openParams = (Ptr)(cargs+1);

			/* Convert the shared control address to virtual format */
             ((FrameQ_OpenParams *)openParams)->
                        commonOpenParams.sharedAddr =
                                     SharedRegion_getPtr ((SharedRegion_SRPtr)
                                           ((FrameQ_OpenParams *)openParams)->commonOpenParams.sharedAddr);

			if (cargs->args.open.nameLen > 0)
				((FrameQ_OpenParams *)openParams)->commonOpenParams.name = (String)(((FrameQ_ShMem_OpenParams*)openParams) + 1);

			out->apiStatus = FrameQ_open (openParams,&handle);
			GT_assert (curTrace, (out->apiStatus >= 0));

            out->args.open.handle = handle;

            if (cargs->args.open.handle != NULL) {

               mgrshMem = _FrameQ_getCliNotifyMgrShAddr(cargs->args.open.handle);
               index = SharedRegion_getId(mgrshMem);
               out->args.open.cliNotifyMgrSharedMem = SharedRegion_getSRPtr(mgrshMem, index);
               gateShMem = _FrameQ_getCliNotifyMgrGateShAddr(cargs->args.open.handle);
               index = SharedRegion_getId(gateShMem);
               out->args.open.cliGateSharedMem = SharedRegion_getSRPtr(gateShMem, index);

            }

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQ_close ((FrameQ_Handle *)&(cargs->args.close.handle));
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}

int syslink_frameq_put(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQ_Frame            frame;
            FrameQ_FrameBufInfo     *frameBufInfo;
            Ptr                     virtPtr;
            Ptr                     addr;
			int						i;

            /* Convert frame address and frame buffer address in to knl virtual
             		 * format.
             		 */
            frame = Memory_translate (cargs->args.put.frame,
                                      Memory_XltFlags_Phys2Virt);

            GT_assert(curTrace,(frame != NULL));

            frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);

            /* Convert frame address and frame buffer address in to knl virtual
             * format only when frame-buffer shared region's createHeap is TRUE.
             * Otherwise leave the addresses in the buffer-header as is,
             * which means FrameQ_put function will not expect virtual
             * frame-buffer memory addresses, and will
             * convert pointers to Portable pointers in place accordingly.
             */
            if (_is_phys2virt_translationRequired((Ptr)frameBufInfo[0].bufPtr)){
            
	            for(i = 0; i < frame->numFrameBuffers; i++) {
	                addr = (Ptr)frameBufInfo[i].bufPtr;
	                virtPtr = Memory_translate(addr, Memory_XltFlags_Phys2Virt);
	                GT_assert(curTrace,(virtPtr != NULL));
	                frameBufInfo[i].bufPtr = (UInt32)virtPtr;
	            }
	        }
	        
            out->apiStatus = FrameQ_put (cargs->args.put.handle, frame);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_putv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQ_Frame            *framePtr      = NULL;
            UInt32                  *filledQueueId = NULL;
            FrameQ_FrameBufInfo     *frameBufInfo;
            Ptr                     virtPtr;
            Ptr                     addr;
            UInt32                  j;
			int						i;

			framePtr = (FrameQ_Frame*)(cargs+1);

			filledQueueId = (UInt32*)(framePtr + cargs->args.putv.numFrames);

             for (i = 0; i < cargs->args.putv.numFrames; i++) {
                  /* Convert frame address and frame buffer address in to knl
                   			  * virtual format.
                   			  */
                  framePtr[i] = Memory_translate (framePtr[i],
                                            Memory_XltFlags_Phys2Virt);
                  GT_assert(curTrace,(framePtr[i] != NULL));
                  frameBufInfo = (FrameQ_FrameBufInfo *)
                                          &(framePtr[i]->frameBufInfo[0]);
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

             out->apiStatus = FrameQ_putv (cargs->args.putv.handle,
                                          (FrameQ_Frame*)framePtr,
                                          (Ptr)filledQueueId,
                                          cargs->args.putv.numFrames);

             GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_get(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQ_Frame        frame;
            FrameQ_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              j;

            out->apiStatus = FrameQ_get (cargs->args.get.handle, &frame);

            if (out->apiStatus >= 0) {
                /* Convert Frame and frame buffer address in frame to physical
                 		  * address formatso that user space api convert this physical
                 		  * address in to user space virtual format.
                		  */
                frameBufInfo = (FrameQ_FrameBufInfo *)
                                                      &(frame->frameBufInfo[0]);

                /* check to see if address translation is required (MK) */
                if (_is_virt2phys_translationRequired(
		                        (Ptr)frameBufInfo[0].bufPtr)) {
                    /* translate frame buffer addresses when createHeap=true */
	                for(j = 0; j < frame->numFrameBuffers; j++) {
	                    addr = (Ptr)frameBufInfo[j].bufPtr;
	                    phyPtr = Memory_translate(addr, Memory_XltFlags_Virt2Phys);
	                    GT_assert(curTrace,(phyPtr != NULL));
	                    frameBufInfo[j].bufPtr = (UInt32)phyPtr;
	                }
	            }
                /* frame ptr is in the header, so always translate */
                out->args.get.frame = Memory_translate(
                                                    frame,
                                                    Memory_XltFlags_Virt2Phys);
            }
            else {
                out->args.get.frame = NULL;
            }

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_getv(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			FrameQ_Frame        *framePtr = NULL;
            UInt32              *filledQueueId = NULL;
            FrameQ_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              j;
			int					i;

			framePtr = (FrameQ_Frame*)(cargs+1);

			filledQueueId = (UInt32*)(framePtr + cargs->args.getv.numFrames);

            out->apiStatus = FrameQ_getv (cargs->args.getv.handle,
                                          framePtr,
                                          (Ptr)filledQueueId,
                                          &cargs->args.getv.numFrames);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQDrv_CmdArgs));

            if ((out->apiStatus >= 0) && (cargs->args.getv.numFrames > 0)) {

                for (i = 0; i < cargs->args.getv.numFrames; i++) {
                    /* Convert Frame and frame buffer address in frame
                     * to physical address formatso that user space api
                     * convert this physical address in to user space
                     * virtual format.
                     */
                    frameBufInfo = (FrameQ_FrameBufInfo *)
                                          &(framePtr[i]->frameBufInfo[0]);

                    /* check to see if address translation is required*/
                    if (_is_virt2phys_translationRequired(
                        (Ptr)frameBufInfo[0].bufPtr)) {
                        for(j = 0; j < framePtr[i]->numFrameBuffers; j++) {
                            addr = (Ptr)frameBufInfo[j].bufPtr;
                            phyPtr = Memory_translate(
                                                 addr,
                                                 Memory_XltFlags_Virt2Phys);
                            GT_assert(curTrace,(phyPtr != NULL));
                            frameBufInfo[j].bufPtr = (UInt32)phyPtr;
                        }
                    }
                    framePtr[i] = Memory_translate(
                                           framePtr[i],
                                           Memory_XltFlags_Virt2Phys);
                }

			    SETIOV(&ctp->iov[1], framePtr, sizeof (UInt32) * 
				    cargs->args.getv.numFrames);

                return _RESMGR_NPARTS(2);
            }

			return _RESMGR_NPARTS(1);
}


int syslink_frameq_get_numframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			UInt32  numFrames;

            out->apiStatus = FrameQ_getNumFrames(cargs->args.getNumFrames.handle,&numFrames);
            out->args.getNumFrames.numFrames = numFrames;

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}

int syslink_frameq_get_vnumframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			/* Get the available frames in filled queue 0 of FrameQ */
            UInt32  *filledQueueId = NULL;
            UInt32  *numFrames;

            numFrames = (UInt32*)(cargs+1);

			filledQueueId = (UInt32*)(numFrames + cargs->args.getvNumFrames.numFilledQids);

			out->apiStatus = FrameQ_getvNumFrames(cargs->args.getvNumFrames.handle,
                                        numFrames,
                                        (Ptr)filledQueueId,
                                        cargs->args.getvNumFrames.numFilledQids);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQDrv_CmdArgs));
            SETIOV(&ctp->iov[1], numFrames, sizeof (UInt32) * cargs->args.getvNumFrames.numFilledQids);

            return _RESMGR_NPARTS(2);

}

int syslink_frameq_control(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			Ptr arg;

			arg = (Ptr)(cargs+1);

            out->apiStatus = FrameQ_control( cargs->args.control.handle,
                                             cargs->args.control.cmd,
                                             arg);

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_set_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = _FrameQ_setNotifyId (cargs->args.setNotifyId.handle, cargs->args.setNotifyId.notifyId);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}

int syslink_frameq_reset_notifyid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = _FrameQ_resetNotifyId (cargs->args.resetNotifyId.handle,cargs->args.resetNotifyId.notifyId);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}

int syslink_frameq_shmem_params_init(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

			FrameQ_ShMem_Params *params = (FrameQ_ShMem_Params*)(cargs+1);

			FrameQ_ShMem_Params_init(params);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQDrv_CmdArgs));
			SETIOV(&ctp->iov[1], params, sizeof(FrameQ_ShMem_Params));

			return _RESMGR_NPARTS(2);

}

int syslink_frameq_shmem_memreq(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));

			FrameQ_ShMem_Params *params = NULL;
            UInt32 *memreq;

			memreq = (UInt32*)(cargs+1);
			params = (FrameQ_ShMem_Params*)(memreq+1);

            *memreq = FrameQ_ShMem_sharedMemReq(params);

            SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQDrv_CmdArgs));
			SETIOV(&ctp->iov[1], memreq, sizeof (UInt32));

			return _RESMGR_NPARTS(2);

}

int syslink_frameq_get_numfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			UInt32  numFreeFrames;

            out->apiStatus = FrameQ_getNumFreeFrames(cargs->args.getNumFreeFrames.handle,&numFreeFrames);
            out->args.getNumFreeFrames.numFreeFrames = numFreeFrames;

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));

}

int syslink_frameq_get_vnumfreeframes(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb) {

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			/* Get the available frames in filled queue 0 of FrameQ */
            UInt32  *freeQueueId = NULL;
            UInt32  *numFreeFrames;

			numFreeFrames = (UInt32*)(cargs+1);

            freeQueueId = (UInt32*)(numFreeFrames + cargs->args.getvNumFreeFrames.numFreeQids);

			out->apiStatus = FrameQ_getvNumFreeFrames(
                                        cargs->args.getvNumFreeFrames.handle,
                                        numFreeFrames,
                                        (Ptr)freeQueueId,
                                        cargs->args.getvNumFreeFrames.numFreeQids);

			SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(FrameQDrv_CmdArgs));
			SETIOV(&ctp->iov[1], numFreeFrames, sizeof (UInt32) * cargs->args.getvNumFreeFrames.numFreeQids);

			return _RESMGR_NPARTS(2);

}

int syslink_frameq_reg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQ_registerNotifier (cargs->args.regNotifier.handle,&cargs->args.regNotifier.notifyParams);
            GT_assert (curTrace, (out->apiStatus >= 0));

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
}

int syslink_frameq_unreg_notifier(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb){

			FrameQDrv_CmdArgs *cargs = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->i));
			FrameQDrv_CmdArgs *out = (FrameQDrv_CmdArgs *) (_DEVCTL_DATA (msg->o));

			out->apiStatus = FrameQ_unregisterNotifier (cargs->args.unregNotifier.handle);

			return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(FrameQDrv_CmdArgs)));
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
