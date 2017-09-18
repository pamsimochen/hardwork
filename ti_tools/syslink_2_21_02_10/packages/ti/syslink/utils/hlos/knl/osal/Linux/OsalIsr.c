/*
 *  @file   OsalIsr.c
 *
 *  @brief      Linux kernel ISR interface implementation.
 *
 *              This abstracts the ISR interface on kernel side code.
 *              Installs the handler for individual IRQs and handlers
 *              are invoked as the interrupts occur. This is used for
 *              communication with DSP.
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

/* OSAL and kernel utils */
#include <ti/syslink/inc/knl/OsalIsr.h>
#include <ti/syslink/inc/knl/OsalThread.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* Linux specific header files */
#include <asm/system.h>
#include <linux/interrupt.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/version.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief   Defines object to encapsulate the interrupt service routine.
 *           The definition is OS/platform specific.
 */
typedef struct OsalIsr_Object_tag {
    UInt32                     irq;
    /*!< The IRQ number handled by this ISR object. */
    OsalThread_Handle          bottomHalf;
    /*!< OsalThread_Object which is being invoked by the ISR handler */
    OsalIsr_CheckAndClearFxn   checkFunc;
    /*!< Argument for the CheckAndClear function */
    Ptr                        checkFuncArg;
    /*!< CheckAndClear function registered for this interrupt */
    Bool                       isSharedInt;
    /*!< Is the interrupt a shared interrupt? */
    OsalIsr_State              isrState;
    /*!< Current state of the ISR. */
} OsalIsr_Object;


/* =============================================================================
 * Forward declarations of internal functions
 * =============================================================================
 */
/* Callback function registered with the Operating System. */
static irqreturn_t OsalIsr_callback (int irq, Ptr arg, struct pt_regs *);


/* =============================================================================
 *  Globals
 * =============================================================================
 */
#if defined  (SYSLINK_INT_LOGGING)
extern UInt32 SysLogging_intCount;
extern UInt32 SysLogging_checkAndClearCount;
extern UInt32 SysLogging_isrCount;
extern UInt32 SysLogging_threadCount;
extern UInt32 SysLogging_NotifyCallbackCount;
#endif /* if defined  (SYSLINK_INT_LOGGING) */


/* =============================================================================
 * APIs
 * =============================================================================
 */
/*!
 *  @brief      Creates an ISR object.
 *
 *  @param      fxn     ISR handler function to be registered
 *  @param      fxnArgs Optional parameter associated with the ISR handler
 *  @param      params  Parameters with information about the interrupt to be
 *                      registered.
 *
 *  @sa         OsalIsr_delete, Memory_alloc
 */
OsalIsr_Handle
OsalIsr_create (OsalIsr_CallbackFxn fxn,
                Ptr                 fxnArgs,
                OsalIsr_Params *    params)
{
    OsalIsr_Object *        isrObj =  NULL;
    OsalThread_Params       threadParams;

    GT_3trace (curTrace, GT_ENTER, "OsalIsr_create", fxn, fxnArgs, params);

    GT_assert (curTrace, (fxn != NULL));
    /* fxnArgs are optional and may be passed as NULL. */
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (fxn == NULL) {
        /*! @retval NULL NULL provided for argument fxn */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_create",
                             OSALISR_E_INVALIDARG,
                             "NULL provided for argument fxn");
    }
    else if (params == NULL) {
        /*! @retval NULL NULL provided for argument params */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_create",
                             OSALISR_E_INVALIDARG,
                             "NULL provided for argument params");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        isrObj = Memory_alloc (NULL, sizeof (OsalIsr_Object), 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (isrObj == NULL) {
            /*! @retval NULL Failed to allocate memory for ISR object. */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OsalIsr_create",
                                 OSALISR_E_MEMORY,
                                 "Failed to allocate memory for ISR object.");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Create the thread object used for the interrupt handler. */
            threadParams.priority     = OsalThread_Priority_High;
            threadParams.priorityType = OsalThread_PriorityType_Generic;
            threadParams.once         = FALSE;
            isrObj->bottomHalf = OsalThread_create ((OsalThread_CallbackFxn)
                                                    fxn,
                                                    fxnArgs,
                                                    &threadParams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (isrObj->bottomHalf == NULL) {
                /*! @retval NULL Failed to create thread for ISR handler. */
                GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "OsalIsr_create",
                                    OSALISR_E_THREAD,
                                    "Failed to create thread for ISR handler.");
                Memory_free (NULL, isrObj, sizeof (OsalIsr_Object));
                isrObj = NULL;
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Copy the creation parameters for further use */
                isrObj->irq = params->intId;
                isrObj->isSharedInt = params->sharedInt;
                isrObj->checkFunc = params->checkAndClearFxn;
                isrObj->checkFuncArg = params->fxnArgs;

                /* Initialize state to uninstalled. */
                isrObj->isrState = OsalIsr_State_Uninstalled;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_create", isrObj);

    /*! @retval ISR-handle Operation successfully completed. */
    return (OsalIsr_Handle) isrObj;
}


/*!
 *  @brief      Delete the ISR object.
 *
 *  @param      isrHandle   ISR object handle which needs to be deleted.
 *
 *  @sa         OsalIsr_create, Memory_free
 */
Int
OsalIsr_delete (OsalIsr_Handle * isrHandle)
{
    Int                 status = OSALISR_SUCCESS;
    OsalIsr_Object *    isrObj = NULL;

    GT_1trace (curTrace, GT_ENTER, "OsalIsr_delete", isrHandle);

    GT_assert (curTrace, (isrHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (isrHandle == NULL) {
        /*! @retval OSALISR_E_INVALIDARG NULL provided for argument isrHandle.*/
        status = OSALISR_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_delete",
                             status,
                             "NULL provided for argument isrHandle");
    }
    else if (*isrHandle == NULL) {
        /*! @retval OSALISR_E_HANDLE NULL ISR handle provided. */
        status = OSALISR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_delete",
                             status,
                             "NULL ISR handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        isrObj = (OsalIsr_Object*) (*isrHandle);
        /* Delete the thread used for the ISR handler */
        if (isrObj->bottomHalf != NULL) {
            OsalThread_delete (&(isrObj->bottomHalf));
        }

        /* Free the ISR object. */
        Memory_free (NULL, isrObj, sizeof (OsalIsr_Object));
        *isrHandle = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_delete", status);

    /*! @retval OSALISR_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief      Install an interrupt service routine.
 *
 *              This function calls the request_irq () function and installs
 *              the specified interrupt service routine in non-shared, non-fiq
 *              mode.
 *
 *  @param      isrHandle   ISR object handle to be installed
 *
 *  @sa         OsalIsr_uninstall
 */
Int
OsalIsr_install (OsalIsr_Handle isrHandle)
{
    Int                 status   = OSALISR_SUCCESS;
    Int                 osStatus = 0;
    OsalIsr_Object *    isrObj   = (OsalIsr_Object *) isrHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalIsr_install", isrHandle);

    GT_assert (curTrace, (isrHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (isrHandle == NULL) {
        /*! @retval OSALISR_E_HANDLE NULL ISR handle provided. */
        status = OSALISR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_install",
                             status,
                             "NULL ISR handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (isrObj->isSharedInt == TRUE) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22)
            osStatus = request_irq (isrObj->irq,
                                    (Void*) &OsalIsr_callback,
                                    SA_SHIRQ,
                                    "SYSLINK",
                                    (Void *) isrObj);
#else /* if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,22) */
            osStatus = request_irq (isrObj->irq,
                                    (Void*) &OsalIsr_callback,
                                    IRQF_SHARED,
                                    "SYSLINK",
                                    (Void *) isrObj);
#endif /* if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,22) */
            isrObj->isrState = OsalIsr_State_Installed;
        }
        else {
            osStatus = request_irq (isrObj->irq,
                                    (Void*) &OsalIsr_callback,
                                    0,
                                    "SYSLINK",
                                    (Void *) isrObj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (osStatus != 0) {
                /*! @retval OSALISR_E_IRQINSTALL request_irq failed. */
                status = OSALISR_E_IRQINSTALL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OsalIsr_install",
                                     status,
                                     "request_irq failed");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                isrObj->isrState = OsalIsr_State_Installed;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_install", status);

    /*! @retval OSALISR_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief      Uninstalls an interrupt service routine.
 *
 *  @param      isrHandle   ISR object handle to be uninstalled.
 *
 *  @sa         OsalIsr_install
 */
Int
OsalIsr_uninstall (OsalIsr_Handle isrHandle)
{
    Int                 status  = OSALISR_SUCCESS;
    OsalIsr_Object *    isrObj  = (OsalIsr_Object *) isrHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalIsr_uninstall", isrHandle);

    GT_assert (curTrace, (isrHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (isrHandle == NULL) {
        /*! @retval OSALISR_E_HANDLE NULL ISR handle provided. */
        status = OSALISR_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_uninstall",
                             status,
                             "NULL ISR handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        free_irq (isrObj->irq, isrObj);
        isrObj->isrState = OsalIsr_State_Uninstalled;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_uninstall", status);

    /*! @retval OSALISR_SUCCESS Operation successfully completed. */
    return status;
}


/*!
 *  @brief      Disables the specified ISR.
 *
 *              This function calls disable_irq () to disable the ISR.
 *              disble_irq() function doesn't return any value so this
 *              function assumes it was successful.
 *
 *  @param      isrHandle   ISR object handle for the interrupt to be disabled.
 *
 *  @sa         OsalIsr_enableIsr
 */
Void
OsalIsr_disableIsr (OsalIsr_Handle isrHandle)
{
    OsalIsr_Object * isrObj = (OsalIsr_Object *) isrHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalIsr_disableIsr", isrHandle);

    GT_assert (curTrace, (isrHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (isrHandle == NULL) {
        /* Void function, so status is not set. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_disableIsr",
                             OSALISR_E_HANDLE,
                             "NULL ISR handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (isrObj->isSharedInt != TRUE) {
            disable_irq (isrObj->irq);
            isrObj->isrState = OsalIsr_State_Disabled;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalIsr_disableIsr");
}


/*!
 *  @brief      Enables the specified ISR.
 *
 *              This function calls enable_irq () to enable the ISR.
 *              enble_irq() function doesn't return any value so this function
 *              assumes it was successful.
 *
 *  @param      isrHandle   ISR object handle for the interrupt to be enabled.
 *
 *  @sa         OsalIsr_disableIsr
 */
Void
OsalIsr_enableIsr (OsalIsr_Handle isrHandle)
{
    OsalIsr_Object * isrObj = (OsalIsr_Object *) isrHandle;

    GT_1trace (curTrace, GT_ENTER, "OsalIsr_enableIsr", isrHandle);

    GT_assert (curTrace, (isrHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (isrHandle == NULL) {
        /* Void function, so status is not set. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_enableIsr",
                             OSALISR_E_HANDLE,
                             "NULL ISR handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (isrObj->isSharedInt != TRUE) {
            enable_irq (isrObj->irq);
            isrObj->isrState = OsalIsr_State_Installed;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "OsalIsr_enableIsr");
}


/*!
 *  @brief      Disables all interrupts. Returns flags that must be passed to
 *              the corresponding restore API.
 *
 *  @sa         OsalIsr_restore
 */
UInt32 OsalIsr_disable (Void)
{
    unsigned long flags = 0;

    GT_0trace (curTrace, GT_ENTER, "OsalIsr_disable");

    local_irq_save (flags);

    GT_1trace (curTrace, GT_1CLASS, "   ISR disable flags [0x%x]", flags);
    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_disable", flags);

    /*! @retval flags State of the interrupts before disable was called. */
    return flags;
}


/*!
 *  @brief      Restores all interrupts to their status before disable was
 *              called.
 *
 *  @param      flags   Flags indicating interrupts state before disable was
 *                      called. These are the flags returned from
 *                      OsalIsr_disable.
 *
 *  @sa         OsalIsr_disable
 */
Void OsalIsr_restore (UInt32 flags)
{
    GT_1trace (curTrace, GT_ENTER, "OsalIsr_restore", flags);

    local_irq_restore ((unsigned long) flags);

    GT_0trace (curTrace, GT_LEAVE, "OsalIsr_restore");
}


/*!
 *  @brief      Returns the state of an ISR.
 *
 *  @param      ISR object handle.
 *  @param      returned ISR object state.
 */
OsalIsr_State
OsalIsr_getState (OsalIsr_Handle isrHandle)
{
    OsalIsr_Object * isrObj   = (OsalIsr_Object *) isrHandle;
    OsalIsr_State    isrState = OsalIsr_State_EndValue;

    GT_1trace (curTrace, GT_ENTER, "OsalIsr_enableIsr", isrHandle);

    GT_assert (curTrace, (isrHandle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (isrHandle == NULL) {
        /*! @retval OsalIsr_State_EndValue NULL ISR handle provided. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_getState",
                             OSALISR_E_HANDLE,
                             "NULL ISR handle provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        isrState = isrObj->isrState;
        GT_1trace (curTrace, GT_1CLASS, "   ISR state [%d]", isrState);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_getState", isrState);

    /*! @retval ISR-state State of the ISR */
    return isrState;
}


/*!
 *  @brief      Returns the state of whether the current context is ISR context.
 *
 *  @sa
 */
Bool
OsalIsr_inIsr (Void)
{
    Bool inIsr = FALSE;

    GT_0trace (curTrace, GT_ENTER, "OsalIsr_inIsr");

    if (in_interrupt() != 0) {
        inIsr = TRUE;
    }

    GT_1trace (curTrace, GT_1CLASS, "   In ISR context [%d]", inIsr);
    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_inIsr", inIsr);

    /*! @retval FALSE Not in ISR context. */
    return inIsr;
}


/*!
 *  @brief      ISR handler for given IRQ id.
 *
 *  @param      irq - interrupt id.
 *  @param      arg a index into the list of IRQ handlers.
 *  @param      regs - register details.
 */
irqreturn_t
OsalIsr_callback (int irq, Ptr arg, struct pt_regs * regs)
{
    OsalIsr_Object * isrObj     = (OsalIsr_Object *) arg;
    Int              handled    = 1;
    Bool             isAsserted = FALSE;

    GT_3trace (curTrace, GT_ENTER, "OsalIsr_callback", irq, arg, regs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (arg == NULL) {
        /*! @retval FALSE ISR was not handled. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalIsr_callback",
                             OSALISR_E_INVALIDARG,
                             "NULL arg provided.");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
#if defined  (SYSLINK_INT_LOGGING)
        SysLogging_isrCount++;
#endif /* if defined  (SYSLINK_INT_LOGGING) */
        isAsserted = (*isrObj->checkFunc) (isrObj->checkFuncArg);
        if (isAsserted == TRUE) {
            OsalThread_activate (isrObj->bottomHalf);
        }
        else {
            /*! @retval FALSE ISR was not handled. */
            handled = 0;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_2CLASS, "   ISR handled", handled);
    GT_1trace (curTrace, GT_LEAVE, "OsalIsr_callback", IRQ_RETVAL (handled));

    /*! @retval TRUE ISR was handled. */
    return IRQ_RETVAL (handled);
}
