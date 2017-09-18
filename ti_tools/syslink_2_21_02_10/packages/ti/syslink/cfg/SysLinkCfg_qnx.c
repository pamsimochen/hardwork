/*
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
 */

/**
 *  @file   ti/syslink/cfg/SysLinkCfg_qnx.c
 *
 *  @brief  Default SysLink RTOS-side configuration
 */

/*-------------------------    XDC  specific includes ----------------------  */
#if defined (SYSLINK_BUILD_HLOS)
#include <ti/syslink/Std.h>
#include <ti/syslink/utils/IGateProvider.h>
#endif

#if defined (SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
#endif

/*-------------------------    RingIO specific includes --------------------- */
#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/_RingIOShm.h>

#define SYSLINK_RINGIO_NUMTYPES 1


/* =============================================================================
 * Configuration globals
 * =============================================================================
 */

/* -----------------------------------------------------------------------------
 * RingIO
 * -----------------------------------------------------------------------------
 */
#if defined (__KERNEL__) || defined (SYSLINK_BUILD_RTOS) || (defined (__QNX__) && !defined(RESOURCE_MANAGER))
/* RingIO function configuration for specific implementation */

/* naming conventions - variable names all small in cfg */
RingIO_CfgFxns SysLink_RingIO_cfgFxns[SYSLINK_RINGIO_NUMTYPES] =
{
   {
       .create       = &RingIOShm_create,
       .sharedMemReq = &RingIOShm_sharedMemReq,
       .openByAddr   = &RingIOShm_openByAddr
   }
};


#endif
