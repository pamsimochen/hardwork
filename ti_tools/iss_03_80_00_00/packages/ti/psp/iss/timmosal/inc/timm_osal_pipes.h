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
 *! 1-June-2009 Aditya Monga: admonga@ti.com Added comments.
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

#ifndef _TIMM_OSAL_PIPES_H_
#define _TIMM_OSAL_PIPES_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "timm_osal_types.h"
#include "timm_osal_error.h"


/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_CreatePipe() - Creates a new counting semaphore instance.
 *
 *  @ param pPipe               :Handle of the pipe to be created.
 *
 *  @ param pipeSize            :Size of the pipe (number of elements multiplied
 *                               by size of each element).
 *
 *  @ param messageSize         :Size of each element.
 *
 *  @ param isFixedMessage      :Non zero for fixed message size.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreatePipe (TIMM_OSAL_PTR *pPipe,
                                          TIMM_OSAL_U32  pipeSize,
                                          TIMM_OSAL_U32  messageSize,
                                          TIMM_OSAL_U8   isFixedMessage);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_DeletePipe() - Deletes a previously created pipe instance. 
 *
 *  @ param pPipe               :Handle of the pipe to be deleted.
 */
/* ===========================================================================*/                                          
TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeletePipe (TIMM_OSAL_PTR pPipe);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_TIMM_OSAL_WriteToPipe() - Write to back of pipe.
 *
 *  @ param pPipe              :Handle of the pipe.
 *
 *  @ param pMessage           :Pointer to the message that has to be written
 *                              to the pipe.
 *
 *  @ param size               :Size of the message.
 *
 *  @ param timeout            :Time (in millisec) to wait until write to pipe
 *                              succeeds.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_WriteToPipe (TIMM_OSAL_PTR pPipe,
                                           TIMM_OSAL_PTR pMessage,
                                           TIMM_OSAL_U32 size,
                                           TIMM_OSAL_S32 timeout);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_TIMM_OSAL_WriteToFrontOfPipe() - Write to front of pipe.
 *
 *  @ param pPipe              :Handle of the pipe.
 *
 *  @ param pMessage           :Pointer to the message that has to be written
 *                              to the pipe.
 *
 *  @ param size               :Size of the message.
 *
 *  @ param timeout            :Time (in millisec) to wait until write to pipe
 *                              succeeds.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_WriteToFrontOfPipe (TIMM_OSAL_PTR pPipe,
                                                  TIMM_OSAL_PTR pMessage,
                                                  TIMM_OSAL_U32 size,
                                                  TIMM_OSAL_S32 timeout);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_TIMM_OSAL_ReadFromPipe() - Read from front of pipe.
 *
 *  @ param pPipe            :Handle of the pipe.
 *
 *  @ param pMessage         :Pointer to the message that has to be read
 *                            from the pipe.
 *
 *  @ param size             :Expected size of the message.
 *
 *  @ param actualSize       :Actual size of the message. This is out parameter.
 *
 *  @ param timeout          :Time (in millisec) to wait until read from pipe
 *                            succeeds.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_ReadFromPipe (TIMM_OSAL_PTR pPipe,
                                            TIMM_OSAL_PTR pMessage,
                                            TIMM_OSAL_U32 size,
                                            TIMM_OSAL_U32 *actualSize,
                                            TIMM_OSAL_S32 timeout);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_ClearPipe() - Clear the pipe of its contents.
 *
 *  @ param pPipe               :Handle of the pipe to be cleared.
 */
/* ===========================================================================*/                                                  
TIMM_OSAL_ERRORTYPE TIMM_OSAL_ClearPipe (TIMM_OSAL_PTR pPipe);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_IsPipeReady() - Check if the pipe has any elements.
 *
 *  @ param pPipe               :Handle of the pipe.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_IsPipeReady (TIMM_OSAL_PTR pPipe);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_GetPipeReadyMessageCount() - Get the number of elements in the
 *                                            pipe.
 *
 *  @ param pPipe               :Handle of the pipe.
 *
 *  @ param count               :Number of elements in the pipe. This is out 
 *                               parameter.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_GetPipeReadyMessageCount (TIMM_OSAL_PTR pPipe,
                                                        TIMM_OSAL_U32 *count);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_PIPES_H_ */
