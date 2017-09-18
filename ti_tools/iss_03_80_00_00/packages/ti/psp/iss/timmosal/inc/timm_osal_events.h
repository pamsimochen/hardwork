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

#ifndef _TIMM_OSAL_EVENTS_H_
#define _TIMM_OSAL_EVENTS_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "timm_osal_types.h"
#include "timm_osal_error.h"


/* ===========================================================================*/
/**
 *  TIMM_OSAL_EVENT_OPERATION - Different operations possible while retrieving
 *                              or setting events.
 *
 *  @ param TIMM_OSAL_EVENT_AND          :The event will be retreived if all 
 *                                        the events specified in the mask
 *                                        are active. The events will not be 
 *                                        consumed. 
 *                                        While setting events, the flag 
 *                                        specified will be ANDed with the
 *                                        current flag.
 *
 *  @ param TIMM_OSAL_EVENT_AND_CONSUME  :The event will be retreived if all 
 *                                        the events specified in the mask
 *                                        are active. All events in the 
 *                                        mask will be consumed. 
 *                                        Not valid while setting events.
 *
 *  @ param TIMM_OSAL_EVENT_OR           :The event will be retreived if any 
 *                                        the events specified in the mask
 *                                        are active. The events will not be 
 *                                        consumed.
 *                                        While setting events, the flag 
 *                                        specified will be ORed with the
 *                                        current flag.
 *
 *  @ param TIMM_OSAL_EVENT_OR_CONSUME   :The event will be retreived if any 
 *                                        the events specified in the mask
 *                                        are active. All active events in the 
 *                                        mask will be consumed. 
 *                                        Not valid while setting events.
 */
/* ===========================================================================*/
typedef enum TIMM_OSAL_EVENT_OPERATION 
{
   TIMM_OSAL_EVENT_AND,
   TIMM_OSAL_EVENT_AND_CONSUME,
   TIMM_OSAL_EVENT_OR,
   TIMM_OSAL_EVENT_OR_CONSUME

} TIMM_OSAL_EVENT_OPERATION;



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_EventCreate() - Creates a new Event instance.
 *
 *  @ param pEvents              :Handle of the Event to be created.                                              
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventCreate(TIMM_OSAL_PTR *pEvents);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_EventDelete() - Deletes a previously created Event instance.
 *
 *  @ param pEvents              :Handle of the Event to be deleted.                                              
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventDelete(TIMM_OSAL_PTR pEvents);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_EventSet() - Signals the requested Event. Tasks waiting on
 *                            this event will wake up.
 *
 *  @ param pEvents           :Handle of previously created Event instance.
 *
 *  @ param uEventFlag        :Mask of Event IDs to set.
 *
 *  @ param eOperation        :Operation while setting events.                                    
 */
/* ===========================================================================*/   
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventSet(TIMM_OSAL_PTR pEvents,
                                      TIMM_OSAL_U32 uEventFlag,
                                      TIMM_OSAL_EVENT_OPERATION eOperation);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_EventRetrieve() - Waits for event.
 *
 *  @ param pEvents                :Handle of previously created Event instance
 *
 *  @ param uRequestedEvents       :Mask of Event IDs to wait on
 *
 *  @ param eOperation             :Operation for the wait
 *
 *  @ param pRetreivedEvents       :Mask of the Event IDs retreived on success
 *
 *  @ param uTimeOut               :Time in millisec to wait for the event
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_EventRetrieve(TIMM_OSAL_PTR pEvents,
                                          TIMM_OSAL_U32 uRequestedEvents,
                                          TIMM_OSAL_EVENT_OPERATION eOperation,
                                          TIMM_OSAL_U32 *pRetrievedEvents,
                                          TIMM_OSAL_U32 uTimeOut);                              

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_EVENTS_H_ */
