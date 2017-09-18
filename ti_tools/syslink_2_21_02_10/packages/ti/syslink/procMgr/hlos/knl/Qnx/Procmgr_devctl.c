/*
 *  @file   Procmgr_devctl.c
 *
 *  @brief      OS-specific implementation of ProcMgr driver for Linux
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
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* QNX specific header include */
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/proto.h>
#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/dcmd_syslink.h>

/* Module specific header files */
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/_ProcMgr.h>
#include <ti/syslink/inc/knl/ProcMgrDrv.h>
#include <ti/syslink/inc/ProcMgrDrvDefs.h>
#include <ti/syslink/inc/knl/Platform.h>


/* Function prototypes */
int syslink_procmgr_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_start(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_stop(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getattachparams(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_load(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_unload(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_map(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_unmap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_ctrl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getsectdata(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getstate(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getstartparams(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getsectinfo(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getsymbaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getprocinfo(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_read(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_write(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_tarnslateaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_paramsinit(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_getloadedfileid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);
int syslink_procmgr_configsysmemmap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb);

Int32				ret;


/**
 * Handler for devctl() messages for MultiProc module.
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


int syslink_procmgr_devctl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	int	status = 0;

	switch (msg->i.dcmd)
	{

	case DCMD_PROCMGR_ATTACH:
	{
		return syslink_procmgr_attach( ctp, msg, ocb);
	}
	break;
	case DCMD_PROCMGR_DELETE:
	{
		return syslink_procmgr_delete( ctp, msg, ocb);
	}
	break;
	case DCMD_PROCMGR_DETACH:
	{
		return syslink_procmgr_detach( ctp, msg, ocb);
	}
	break;
	case DCMD_PROCMGR_CLOSE:
	{
		return syslink_procmgr_close( ctp, msg, ocb);
	}
	break;

	case DCMD_PROCMGR_CREATE:
	{
		return syslink_procmgr_create( ctp, msg, ocb);
	}
	break;

	case DCMD_PROCMGR_CONTROL:
	{
		return syslink_procmgr_ctrl( ctp, msg, ocb);
	}
	break;

	case DCMD_PROCMGR_DESTROY:
	{
		return syslink_procmgr_destroy( ctp, msg, ocb);
	}
	break;

	case DCMD_PROCMGR_GETCONFIG:
	{
			return syslink_procmgr_getconfig( ctp, msg, ocb);
  	}
	  break;

	  case DCMD_PROCMGR_GETPROCINFO:
	   	{
			return syslink_procmgr_getprocinfo( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_GETATTACHPARAMS:
	   	{
			return syslink_procmgr_getattachparams( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_GETSECTIONDATA:
	   	{
			return syslink_procmgr_getsectdata( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_GETSECTIONINFO:
	   	{
			return syslink_procmgr_getsectinfo( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_GETSTARTPARAMS:
	   	{
			return syslink_procmgr_getstartparams( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_GETSTATE:
	   	{
			return syslink_procmgr_getstate( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_GETSYMBOLADDR:
	   	{
			return syslink_procmgr_getsymbaddr( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_LOAD:
	   	{
			return syslink_procmgr_load( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_UNLOAD:
	   	{
			return syslink_procmgr_unload( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_MAP:
	   	{
			return syslink_procmgr_map( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_UNMAP:
	   	{
			return syslink_procmgr_unmap( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_OPEN:
	   	{
			return syslink_procmgr_open( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_READ:
	   	{
			return syslink_procmgr_read( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_WRITE:
	   	{
			return syslink_procmgr_write( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_SETUP:
	   	{
			return syslink_procmgr_setup( ctp, msg, ocb);
	  	}
	  break;

	  case DCMD_PROCMGR_START:
	{
			return syslink_procmgr_start( ctp, msg, ocb);
		}
	  break;

	  case DCMD_PROCMGR_STOP:
	{
			return syslink_procmgr_stop( ctp, msg, ocb);
	}
	  break;

	case DCMD_PROCMGR_TRANSLATEADDR:

	{
		return syslink_procmgr_tarnslateaddr( ctp, msg, ocb);
	}
	break;
	case DCMD_PROCMGR_PARAMS_INIT:
	{
		return syslink_procmgr_paramsinit( ctp, msg, ocb);
	}
	break;
	case DCMD_PROCMGR_GETLOADEDFILEID:
	{
		return syslink_procmgr_getloadedfileid( ctp, msg, ocb);
	}
	break;

	case DCMD_PROCMGR_CONFIGSYSMEMMAP:
	{
		return syslink_procmgr_configsysmemmap( ctp, msg, ocb);
	}
	break;

	default:
		fprintf( stderr, "Invalid DEVCTL for MultiProc 0x%x \n", msg->i.dcmd);
		break;

	}
//Ramesh return a error value from here. Check with team.
	return status;
//	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof (NameServerDrv_CmdArgs)));

}

int syslink_procmgr_attach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ProcMgr_CmdArgsAttach *		cargs = (ProcMgr_CmdArgsAttach *) (_DEVCTL_DATA (msg->i));
	cargs->params = (ProcMgr_AttachParams*)(cargs + 1);

	cargs->commonArgs.apiStatus = ProcMgr_attach(cargs->handle, cargs->params);

	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));

	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsAttach)));
}

int syslink_procmgr_detach(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
ProcMgr_CmdArgsDetach*		cargs = ( ProcMgr_CmdArgsDetach *) (_DEVCTL_DATA (msg->i));

cargs->commonArgs.apiStatus = ProcMgr_detach(cargs->handle);
GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
    if (cargs->commonArgs.apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_PROCMGR_DETACH, (int)cargs->handle, NULL);
    }
return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsDetach)));
}

int syslink_procmgr_start(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsStart* cargs = (ProcMgr_CmdArgsStart*) (_DEVCTL_DATA (msg->i));
	ProcMgr_StartParams* params = (ProcMgr_StartParams *)(cargs+1);

	cargs->commonArgs.apiStatus = ProcMgr_start (cargs->handle, params);
					/* This check is needed at run-time also to propagate the
					* status to user-side. This must not be optimized out.
					*/
#if !defined(SYSLINK_BUILD_OPTIMIZE)
				if (cargs->commonArgs.apiStatus < 0) {
						/* This does not impact return status of this function,
						* so retVal comment is not used.
						*/
						GT_setFailureReason (curTrace,
											GT_4CLASS,
											"ProcMgrDrv_ioctl",
											cargs->commonArgs.apiStatus,
											"Kernel-side ProcMgr_start failed");
				}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsStart)));
}

int syslink_procmgr_stop(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsStop* cargs = (ProcMgr_CmdArgsStop*) (_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus = ProcMgr_stop(cargs->handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
				if (cargs->commonArgs.apiStatus < 0) {
						/* This does not impact return status of this function,
						* so retVal comment is not used.
						*/
						GT_setFailureReason (curTrace,
											GT_4CLASS,
											"ProcMgrDrv_ioctl",
											cargs->commonArgs.apiStatus,
											"Kernel-side ProcMgr_start failed");
				}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsStop)));
}

int syslink_procmgr_getattachparams(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetAttachParams *cargs = (ProcMgr_CmdArgsGetAttachParams *)(_DEVCTL_DATA (msg->i));
	ProcMgr_AttachParams *params = (ProcMgr_AttachParams *)(cargs+1);
	ProcMgr_getAttachParams(cargs->handle, params);
	cargs->commonArgs.apiStatus = ProcMgr_S_SUCCESS;
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsGetAttachParams));
	SETIOV(&ctp->iov[1], params, (sizeof(ProcMgr_AttachParams)));
	return _RESMGR_NPARTS(2);
}

/*
 *  ======== syslink_procmgr_load ========
 */
int syslink_procmgr_load(
    resmgr_context_t *  ctp,
    io_devctl_t *       msg,
    syslink_ocb_t *     ocb)
{
    ProcMgr_CmdArgsLoad *cargs;
    String imagePath;
    Char **argv;
    Char *cp;
    SizeT size;
    Int i, status;

    /* extract input data */
    cargs = (ProcMgr_CmdArgsLoad *)(_DEVCTL_DATA(msg->i));

    /* compute size on word boundary */
    size = (sizeof(ProcMgr_CmdArgsLoad) + 3) & ~0x3;
    imagePath = (String)((Char *)cargs + size);

    if (cargs->argc > 0) {
        /* compute length on word boundary */
        size = (cargs->imagePathLen + 3) & ~0x3;
        argv = (Char **)((Char *)imagePath + size);

        /* compute array size on word boundary */
        size = (((cargs->argc + 1) * sizeof(char *)) + 3) & ~0x3;
        cp = (Char *)((Char *)argv + size);

        /* unmarshal strings, size is unaligned */
        for (i = 0; i < cargs->argc; i++) {
            argv[i] = cp;
            cp += (String_len(cp) + 1);
        }
    }
    else {
        argv = NULL;
    }

    /* invoke the requested api */
    status = ProcMgr_load(cargs->handle, imagePath, cargs->argc, argv,
        cargs->params, &(cargs->fileId));

    /* clear the return message, setup return values */
    memset(&msg->o, 0, sizeof(msg->o));
    msg->o.ret_val = status;
    msg->o.nbytes = sizeof(ProcMgr_CmdArgsLoad);
    cargs->commonArgs.apiStatus = status;

    return(_RESMGR_PTR(ctp, &msg->o, sizeof(msg->o) + msg->o.nbytes));
}

int syslink_procmgr_unload(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsUnload * cargs = (ProcMgr_CmdArgsUnload *)(_DEVCTL_DATA (msg->i));
	cargs->commonArgs.apiStatus = ProcMgr_unload(cargs->handle, cargs->fileId);
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsUnload)));

}
int syslink_procmgr_map(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsMap* cargs = (ProcMgr_CmdArgsMap *) (_DEVCTL_DATA (msg->i));
	ProcMgr_AddrInfo* addrInfo = (ProcMgr_AddrInfo*)(cargs+1);

	cargs->commonArgs.apiStatus = ProcMgr_map (cargs->handle,
										cargs->mapType,
										addrInfo,
										cargs->srcAddrType);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
			if (cargs->commonArgs.apiStatus < 0) {
					/* This does not impact return status of this function,
					* so retVal comment is not used.
					*/
					GT_setFailureReason (curTrace,
										GT_4CLASS,
										"ProcMgrDrv_ioctl",
										cargs->commonArgs.apiStatus,
										"Kernel-side ProcMgr_map failed");
			}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

//	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsMap));
//	SETIOV(&ctp->iov[1], addrInfo, (sizeof(ProcMgr_AddrInfo)));
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));

//	return _RESMGR_NPARTS(2);
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsMap)));
}

int syslink_procmgr_unmap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsUnmap *cargs = (ProcMgr_CmdArgsUnmap *) (_DEVCTL_DATA (msg->i));
	ProcMgr_AddrInfo* addrInfo = (ProcMgr_AddrInfo*)(cargs+1);

	cargs->commonArgs.apiStatus = ProcMgr_unmap (cargs->handle,
										cargs->mapType,
										addrInfo,
										cargs->srcAddrType);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
				if (cargs->commonArgs.apiStatus < 0) {
						/* This does not impact return status of this function,
						* so retVal comment is not used.
						*/
						GT_setFailureReason (curTrace,
											GT_4CLASS,
											"ProcMgrDrv_ioctl",
											cargs->commonArgs.apiStatus,
											"Kernel-side ProcMgr_map failed");
				}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

//	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsMap));
//	SETIOV(&ctp->iov[1], addrInfo, (sizeof(ProcMgr_AddrInfo)));
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));

//	return _RESMGR_NPARTS(2);
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsUnmap)));
}
int syslink_procmgr_open(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsOpen *cargs = (ProcMgr_CmdArgsOpen*) (_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus = ProcMgr_open (&(cargs->handle), cargs->procId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
			if (cargs->commonArgs.apiStatus < 0) {
					/* This does not impact return status of this function,
					* so retVal comment is not used.
					*/
				GT_setFailureReason (curTrace,
									GT_4CLASS,
									"ProcMgrDrv_ioctl",
									cargs->commonArgs.apiStatus,
									"Kernel-side ProcMgr_open failed");
				}
			else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

	if (cargs->handle != NULL) {
		cargs->maxMemoryRegions =
		    ProcMgr_getMaxMemoryRegions(cargs->handle);
	}

#if !defined(SYSLINK_BUILD_OPTIMIZE)
			}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

	if (cargs->commonArgs.apiStatus >= 0) {
	    /* Add this call to the list to be cleaned-up */
	    add_ocb_res(ocb, DCMD_PROCMGR_CLOSE, (int)cargs->handle, NULL);
	}
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsOpen)));

}
int syslink_procmgr_close(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsClose *cargs = (ProcMgr_CmdArgsClose*) (_DEVCTL_DATA (msg->i));
	ProcMgr_Handle        temp  = cargs->handle;

	cargs->commonArgs.apiStatus= ProcMgr_close (&(cargs->handle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
		if (cargs->commonArgs.apiStatus < 0) {
				/* This does not impact return status of this function,
				* so retVal comment is not used.
				*/
				GT_setFailureReason (curTrace,
									GT_4CLASS,
									"ProcMgrDrv_ioctl",
									cargs->commonArgs.apiStatus,
									"Kernel-side ProcMgr_close failed");
		}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    if (cargs->commonArgs.apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_PROCMGR_CLOSE, (int)temp, NULL);
    }
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsClose)));


}

int syslink_procmgr_ctrl(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsControl*cargs = (ProcMgr_CmdArgsControl*)(_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus= ProcMgr_control (cargs->handle,
										cargs->cmd,
										cargs->arg);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
			if (cargs->commonArgs.apiStatus < 0) {
					/* This does not impact return status of this function,
					* so retVal comment is not used.
					*/
					GT_setFailureReason (curTrace,
										GT_4CLASS,
										"ProcMgrDrv_ioctl",
										cargs->commonArgs.apiStatus,
										"Kernel-side ProcMgr_control failed");
			}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsControl)));


}
int syslink_procmgr_create(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsCreate* cargs = (ProcMgr_CmdArgsCreate*) (_DEVCTL_DATA (msg->i));
	ProcMgr_Params *params = (ProcMgr_Params *)(cargs+1);

	cargs->handle = ProcMgr_create (cargs->procId, params);
			if (cargs->handle == NULL) {
				cargs->commonArgs.apiStatus = ProcMgr_E_FAIL;
			}
    else {
        cargs->maxMemoryRegions = ProcMgr_getMaxMemoryRegions(cargs->handle);
    	cargs->commonArgs.apiStatus = ProcMgr_S_SUCCESS;
    }
    if (cargs->commonArgs.apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_PROCMGR_DELETE, (int)cargs->handle, NULL);
    }
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsCreate)));
}
int syslink_procmgr_delete(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsDelete * cargs = ( ProcMgr_CmdArgsDelete*) (_DEVCTL_DATA (msg->i));
	ProcMgr_Handle          temp  = cargs->handle;

	cargs->commonArgs.apiStatus = ProcMgr_delete (&(cargs->handle));
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
    if (cargs->commonArgs.apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_PROCMGR_DELETE, (int)temp, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsDelete)));
}
int syslink_procmgr_setup(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsSetup * cargs = ( ProcMgr_CmdArgsSetup *) (_DEVCTL_DATA (msg->i));
	ProcMgr_Config         *cfg = (ProcMgr_Config *)(cargs+1);

	cargs->commonArgs.apiStatus = ProcMgr_setup (cfg);
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
    if (cargs->commonArgs.apiStatus >= 0) {
        /* Add this call to the list to be cleaned-up */
        add_ocb_res(ocb, DCMD_PROCMGR_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsSetup)));

}
int syslink_procmgr_destroy(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsDestroy* cargs = ( ProcMgr_CmdArgsDestroy *) (_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus = ProcMgr_destroy ();
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
    if (cargs->commonArgs.apiStatus >= 0) {
        /* Remove this call from the list to be cleaned-up */
        remove_ocb_res(ocb, DCMD_PROCMGR_DESTROY, (int)NULL, NULL);
    }
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsDestroy)));

}
int syslink_procmgr_getconfig(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetConfig *  cargs = ( ProcMgr_CmdArgsGetConfig *) (_DEVCTL_DATA (msg->i));
	ProcMgr_Config              *cfg = (ProcMgr_Config *)(cargs+1);
	ProcMgr_getConfig (cfg);
	cargs->commonArgs.apiStatus = ProcMgr_S_SUCCESS;
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsGetConfig));
	SETIOV(&ctp->iov[1], cfg, (sizeof(ProcMgr_Config)));
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return _RESMGR_NPARTS(2);

}
int syslink_procmgr_getsectdata(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetSectionData * cargs = (ProcMgr_CmdArgsGetSectionData *)(_DEVCTL_DATA (msg->i));
	ProcMgr_SectionInfo *sectionInfo = (ProcMgr_SectionInfo *)(cargs + 1);
	void* buffer = (void *)(sectionInfo +1);

	cargs->commonArgs.apiStatus = ProcMgr_getSectionData(cargs->handle, cargs->fileId, sectionInfo, buffer);
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsGetSectionData));
	SETIOV(&ctp->iov[1], buffer, sizeof(msg->o) + sizeof(buffer));
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return _RESMGR_NPARTS(2);

}
int syslink_procmgr_getstate(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetState *cargs = (ProcMgr_CmdArgsGetState *)(_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus = ProcMgr_getState(cargs->handle);
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsGetState)));
}
int syslink_procmgr_getstartparams(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetStartParams * cargs = (ProcMgr_CmdArgsGetStartParams *)(_DEVCTL_DATA (msg->i));
	ProcMgr_StartParams *params = (ProcMgr_StartParams *)(cargs + 1);

	ProcMgr_getStartParams(cargs->handle, params);
	cargs->commonArgs.apiStatus = ProcMgr_S_SUCCESS;
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsGetStartParams));
	SETIOV(&ctp->iov[1], params, sizeof(msg->o) + sizeof(ProcMgr_StartParams));
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return _RESMGR_NPARTS(2);

}
int syslink_procmgr_getsectinfo(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetSectionInfo * cargs = (ProcMgr_CmdArgsGetSectionInfo *)(_DEVCTL_DATA (msg->i));
	ProcMgr_SectionInfo * sectionInfo = (ProcMgr_SectionInfo *)(cargs + 1);
	String sectionName = (String)(sectionInfo + 1);

	cargs->commonArgs.apiStatus = ProcMgr_getSectionInfo(cargs->handle, cargs->fileId, sectionName, sectionInfo);
	SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsGetSectionInfo));
	SETIOV(&ctp->iov[1], sectionInfo, sizeof(msg->o) + sizeof(ProcMgr_SectionInfo));

	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return _RESMGR_NPARTS(2);
}
int syslink_procmgr_getsymbaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{

	ProcMgr_CmdArgsGetSymbolAddress *cargs = (ProcMgr_CmdArgsGetSymbolAddress *) (_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus = ProcMgr_getSymbolAddress (cargs->handle,cargs->fileId,
									cargs->symbolName,
									&(cargs->symValue));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
			if (cargs->commonArgs.apiStatus < 0) {
					/* This does not impact return status of this function,
					* so retVal comment is not used.
					*/
					GT_setFailureReason (curTrace,
								GT_4CLASS,
								"ProcMgrDrv_ioctl",
								cargs->commonArgs.apiStatus,
								"Kernel-side ProcMgr_getSymbolAddress failed");
			}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsGetSymbolAddress)));
}

int syslink_procmgr_getprocinfo(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
    ProcMgr_CmdArgsGetProcInfo *cargs = (ProcMgr_CmdArgsGetProcInfo *) (_DEVCTL_DATA (msg->i));
    Int procInfoSize;
    UInt32 maxMemoryRegions;
    ProcMgr_ProcInfo * procInfo = (ProcMgr_ProcInfo *)(cargs+1);

    maxMemoryRegions = ProcMgr_getMaxMemoryRegions(cargs->handle);
    procInfoSize = sizeof(ProcMgr_ProcInfo) +
        (maxMemoryRegions * sizeof(ProcMgr_MappedMemEntry));
    if (procInfo == NULL) {
        GT_setFailureReason (curTrace,
        GT_4CLASS,
        "ProcMgrDrv_ioctl",
        cargs->commonArgs.apiStatus,
        "Kernel-side Memory_alloc failed");
    }
    else {
        cargs->commonArgs.apiStatus = ProcMgr_getProcInfo (cargs->handle,
                                                           procInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (cargs->commonArgs.apiStatus < 0) {
            /* This does not impact return status of this function,
             * so retVal comment is not used.
             */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrv_ioctl",
                                 cargs->commonArgs.apiStatus,
                                 "Kernel-side ProcMgr_getProcInfo failed");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            SETIOV(&ctp->iov[0], &msg->o, sizeof(msg->o) + sizeof(ProcMgr_CmdArgsGetProcInfo));
            SETIOV(&ctp->iov[1], procInfo, procInfoSize);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));

    return _RESMGR_NPARTS(2);
}

int syslink_procmgr_read(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
return 0;
}
int syslink_procmgr_write(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
return 0;
}
int syslink_procmgr_tarnslateaddr(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsTranslateAddr * cargs = (ProcMgr_CmdArgsTranslateAddr*) (_DEVCTL_DATA (msg->i));

	cargs->commonArgs.apiStatus = ProcMgr_translateAddr (cargs->handle,&(cargs->dstAddr),
									cargs->dstAddrType,
									cargs->srcAddr,
									cargs->srcAddrType);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
			if (cargs->commonArgs.apiStatus < 0) {
						/* This does not impact return status of this function,
						* so retVal comment is not used.
						*/
					GT_setFailureReason (curTrace,
									GT_4CLASS,
									"ProcMgrDrv_ioctl",
									cargs->commonArgs.apiStatus,
									"Kernel-side ProcMgr_translateAddr failed");
			}
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
	GT_assert (curTrace, (cargs->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsTranslateAddr)));
}
int syslink_procmgr_paramsinit(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsParamsInit* cargs = (ProcMgr_CmdArgsParamsInit*) (_DEVCTL_DATA (msg->i));
	ProcMgr_Params *params = (ProcMgr_Params *)(cargs+1);
	ProcMgr_Params_init (cargs->handle, params);
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsParamsInit)));
}
int syslink_procmgr_getloadedfileid(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
	ProcMgr_CmdArgsGetLoadedFileId* cargs = (ProcMgr_CmdArgsGetLoadedFileId *) (_DEVCTL_DATA (msg->i));
	ProcMgr_CmdArgsGetLoadedFileId* out   = (ProcMgr_CmdArgsGetLoadedFileId *) (_DEVCTL_DATA (msg->o));

	cargs->fileId = ProcMgr_getLoadedFileId (cargs->handle);

	out->commonArgs.apiStatus = ProcMgr_S_SUCCESS;

	GT_assert (curTrace, (out->commonArgs.apiStatus >= 0));
	return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsGetLoadedFileId)));
}

int syslink_procmgr_configsysmemmap(resmgr_context_t *ctp, io_devctl_t *msg, syslink_ocb_t *ocb)
{
    ProcMgr_CmdArgsSetup * 	cargs = (ProcMgr_CmdArgsSetup *) (_DEVCTL_DATA (msg->i));
    ProcMgr_CmdArgsSetup *	out   = (ProcMgr_CmdArgsSetup *) (_DEVCTL_DATA (msg->o));
    char *                  retVal;

    ProcMgr_Config *		cfg   = (ProcMgr_Config *)(cargs + 1);
    SysLink_MemoryMap *	sysMemMap = (SysLink_MemoryMap *)(cfg + 1);

    SysLink_MemEntry_Block * memBlocks;
    char *params;

    if (sysMemMap->numBlocks > 0) {
        memBlocks = (SysLink_MemEntry_Block *)(sysMemMap + 1);
        params = (char *)(memBlocks + sysMemMap->numBlocks);
    }
    else {
        memBlocks = NULL;
        params = (char *)(SysLink_MemEntry_Block *)(sysMemMap + 1);
    }

    cfg->sysMemMap = sysMemMap;
    cfg->sysMemMap->memBlocks = memBlocks;

    _ProcMgr_configSysMap(cfg);

    out->commonArgs.apiStatus = ProcMgr_S_SUCCESS;

    if(cfg->paramsLen > 0) {
        /* This memory is freed in ProcMgr when receiving next
         * param string or in ProcMgr_destroy().
         */
        cfg->params = (String)Memory_calloc(NULL,
            (cfg->paramsLen * sizeof(Char)), 0u, NULL);

        GT_assert(curTrace, (cfg->params != NULL));
        retVal = strncpy(cfg->params, params, cfg->paramsLen);

        GT_assert(curTrace, (cfg->paramsLen == (strlen(cfg->params) + 1)));

        if (retVal == NULL) {
            GT_1trace (curTrace, GT_1CLASS,
                       "    ProcMgrDrv_ioctl: strncpy call "
                       "    failed\n  status [%d]", retVal);
            out->commonArgs.apiStatus = ProcMgr_E_FAIL;
        }
        else {
            _ProcMgr_saveParams(cfg->params, cfg->paramsLen);
        }
    }
    else {
        /* make call to free previous params string */
        _ProcMgr_saveParams(cfg->params, cfg->paramsLen);
    }

    return (_RESMGR_PTR (ctp, &msg->o, sizeof (msg->o) + sizeof(ProcMgr_CmdArgsSetup)));
}

