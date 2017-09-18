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
*  @file timm_osal_defines.h
*  The osal header file defines 
*  @path
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

#ifndef _TIMM_OSAL_MUTEX_H_
#define _TIMM_OSAL_MUTEX_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* Includes
*******************************************************************************/

#include "timm_osal_types.h"
#include "timm_osal_error.h"



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_MutexCreate() - Creates a new mutex instance.
 *
 *  @ param pMutex               :Handle of the mutex to be created.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexCreate(TIMM_OSAL_PTR *pMutex);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_MutexDelete() - Deltes a previously created mutex instance.
 *
 *  @ param pMutex               :Handle of the mutex to be deleted.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexDelete(TIMM_OSAL_PTR pMutex);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_MutexObtain() - Obtain the mutex. If the mutex if free, the 
 *                               calling task obtains the mutex else it will
 *                               wait.
 *
 *  @ param pMutex               :Handle of the mutex.
 *
 *  @ param uTimeOut             :Wait till timeout (millisec) to obtain the 
 *                                mutex.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexObtain(TIMM_OSAL_PTR pMutex, TIMM_OSAL_U32 uTimeOut);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_MutexRelease() - Release the mutex. The mutex is now avalable
 *                                for other tasks.
 *
 *  @ param pMutex                :Handle of the mutex.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_MutexRelease(TIMM_OSAL_PTR pMutex);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_MUTEX_H_ */
