/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
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
 *
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

/*
*  @file timm_osal_internal.h
*  Internal osal header file for osal internal functions.
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 11-Mar-2009 Aditya Monga: admonga@ti.com Initial version
 * ========================================================================= */

#ifndef _TIMM_OSAL_INTERNAL_H_
#define _TIMM_OSAL_INTERNAL_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*To get tick period*/
#include <ti/sysbios/knl/Clock.h>

#include "timm_osal_types.h"


/* ========================================================================== */
/**
* @fn _TIMM_OSAL_GetTicks() - Convert milliseconds to system clock ticks.
*                             Inline function since only a one line function.
*
*  @ param mSec               :Time in msec.
*/
/* ========================================================================== */
inline TIMM_OSAL_U32 _TIMM_OSAL_GetTicks (TIMM_OSAL_U32 mSec)
{
	/*Clock_tickPeriod gives period of each tick in micro seconds*/
	return (mSec * 1000 / Clock_tickPeriod);
    
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_EVENTS_H_ */
