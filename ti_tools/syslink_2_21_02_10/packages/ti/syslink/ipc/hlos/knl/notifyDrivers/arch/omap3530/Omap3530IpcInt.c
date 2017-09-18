/*
 *  @file   Omap3530IpcInt.c
 *
 *  @brief      OMAP3530 DSP IPC interrupts.
 *              Defines necessary functions for Interrupt Handling.
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

/* OSAL headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>

/*  OS Headers */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/io.h>

/* OSAL and utils headers */
#include <ti/syslink/inc/knl/OsalIsr.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/Memory.h>

/* Hardware Abstraction Layer */
#include <ti/syslink/inc/knl/_ArchIpcInt.h>
#include <ti/syslink/inc/knl/_Omap3530IpcInt.h>
#include <ti/syslink/inc/knl/Omap3530IpcInt.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define OMAP3530IPCINT_MAKE_MAGICSTAMP(x) \
                                    ((OMAP3530IPCINT_MODULEID << 12u) | (x))

/*!
 *  @def    REG
 *  @brief  Regsiter access method.
 */
#define REG(x)          *((volatile UInt32 *) (x))
#define REG32(x)        (*(volatile UInt32 *) (x))


/*!
 *  @def    CORE_CM_BASE
 *  @brief  configuraion address.
 */
#define CORE_CM_BASE                  0x48004A00
/*!
 *  @def    CORE_CM_SIZE
 *  @brief  size to be ioremapped.
 */
#define CORE_CM_SIZE                  0x2000
/*!
 *  @def    MAILBOX_BASE
 *  @brief  MAILBOX_BASE address.
 */
#define MAILBOX_BASE                  0x48094000

/*!
 *  @def    MAILBOX_SIZE
 *  @brief  size to be ioremapped.
 */
#define MAILBOX_SIZE                  0x1000
/*!
 *  @def    MAILBOX_SYSCONFIG_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_SYSCONFIG_OFFSET      0x10

/*!
 *  @def    MAILBOX_SYSSTATUS_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_SYSSTATUS_OFFSET      0x14

/*!
 *  @def    MAILBOX_MESSAGE_0_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_MESSAGE_0_OFFSET      0x40

/*!
 *  @def    MAILBOX_MESSAGE_1_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_MESSAGE_1_OFFSET      0x44

/*!
 *  @def    MAILBOX_MSGSTATUS_0_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_MSGSTATUS_0_OFFSET    0xC0

/*!
 *  @def    MAILBOX_MSGSTATUS_1_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_MSGSTATUS_1_OFFSET    0xC4

/*!
 *  @def    MAILBOX_IRQSTATUS_0_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_IRQSTATUS_0_OFFSET    0x100

/*!
 *  @def    MAILBOX_IRQENABLE_0_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_IRQENABLE_0_OFFSET    0x104

/*!
 *  @def    MAILBOX_IRQSTATUS_1_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_IRQSTATUS_1_OFFSET    0x108

/*!
 *  @def    MAILBOX_IRQENABLE_1_OFFSET
 *  @brief  Offset from the Mailbox base address.
 */
#define MAILBOX_IRQENABLE_1_OFFSET    0x10C

/*!
 *  @def    MAILBOX_IRQENABLE_1_OFFSET
 *  @brief  Offsets from the CORE_CM base address.
 */
#define CM_ICLKEN1_CORE_OFFSET        0x10


/*!
 *  @brief  Device specific object
 *          It can be populated as per device need and it is used internally in
 *          the device specific implementation only.
 */
typedef struct Omap3530IpcInt_ModuleObject_tag {
    Atomic         isrRefCount;
    /* ISR Reference count */
    UInt16         procId;
    /* Processor ID of destination processor */
    UInt32         recvIntId;
    /* recevive interrupt id */
    OsalIsr_Handle isrHandle;
    /*!< Handle to the OsalIsr object */
    UInt32         archCoreCmBase;
    /*!< configuration mgmt base */
    UInt32         mailboxBase;
    /*!< configuration mgmt base */

} Omap3530IpcInt_ModuleObject;

/*!
 *  @brief  Forward declaration of check and clear function
 */
static
Bool
_Omap3530IpcInt_checkAndClearFunc (Ptr arg);


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @brief  State object for Omap3530IpcInt
 */
Omap3530IpcInt_ModuleObject Omap3530IpcInt_state;

/*!
 *  @brief  Function table for OMAP3530
 */
ArchIpcInt_FxnTable Omap3530IpcInt_fxnTable = {
    Omap3530IpcInt_interruptRegister,
    Omap3530IpcInt_interruptUnregister,
    Omap3530IpcInt_interruptEnable,
    Omap3530IpcInt_interruptDisable,
    Omap3530IpcInt_waitClearInterrupt,
    Omap3530IpcInt_sendInterrupt,
    Omap3530IpcInt_clearInterrupt,
};


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*!
 *  @brief      Function to initialize the Omap3530IpcInt module.
 *
 *  @param      cfg  Configuration for setup
 *
 *  @sa         Omap3530IpcInt_destroy
 */
Void
Omap3530IpcInt_setup (Omap3530IpcInt_Config * cfg)
{
    Memory_MapInfo mapInfo;
    Int32          status;

    GT_1trace (curTrace, GT_ENTER, "Omap3530IpcInt_setup", cfg);

    GT_assert (curTrace, (cfg != NULL));

    /* The setup will be called only once, either from SysMgr or from
     * archipcomap3530 module. Hence it does not need to be atomic.
     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "Omap3530IpcInt_setup",
                        OMAP3530IPCINT_E_FAIL,
                        "config for driver specific setup can not be NULL");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Omap3530IpcInt_state.procId         = cfg->procId;
        Omap3530IpcInt_state.recvIntId      = cfg->recvIntId;

        /* Map general control base */
        mapInfo.src      = CORE_CM_BASE;
        mapInfo.size     = CORE_CM_SIZE;
        mapInfo.isCached = FALSE;
        status = Memory_map (&mapInfo);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Omap3530IpcInt_setup",
                                 status,
                                 "Failure in Memory_map for general ctrl base");
            Omap3530IpcInt_state.archCoreCmBase = 0;
        }
        else {
            Omap3530IpcInt_state.archCoreCmBase = mapInfo.dst;
            /* Map mailboxBase */
            mapInfo.src      = MAILBOX_BASE;
            mapInfo.size     = MAILBOX_SIZE;
            mapInfo.isCached = FALSE;
            status = Memory_map (&mapInfo);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Omap3530IpcInt_setup",
                                     status,
                                     "Failure in Memory_map for mailboxBase");
                Omap3530IpcInt_state.mailboxBase = 0;
            }
            else {
                Omap3530IpcInt_state.mailboxBase = mapInfo.dst;
            }
        }

        if (status >= 0) {
            ArchIpcInt_object.fxnTable          = &Omap3530IpcInt_fxnTable;
            ArchIpcInt_object.obj               = &Omap3530IpcInt_state;
            ArchIpcInt_object.isSetup           = TRUE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Omap3530IpcInt_setup");
}


/*!
 *  @brief      Function to finalize the Omap3530IpcInt module
 *
 *  @sa         Omap3530IpcInt_setup
 */
Void
Omap3530IpcInt_destroy (Void)
{
    GT_0trace (curTrace, GT_ENTER, "Omap3530IpcInt_destroy");

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));

    ArchIpcInt_object.isSetup  = FALSE;
    ArchIpcInt_object.obj      = NULL;
    ArchIpcInt_object.fxnTable = NULL;
    iounmap((unsigned int *)Omap3530IpcInt_state.archCoreCmBase);
    iounmap((unsigned int *)Omap3530IpcInt_state.mailboxBase);

    GT_0trace (curTrace, GT_ENTER, "Omap3530IpcInt_destroy");
}


/*!
 *  @brief      Function to register the interrupt.
 *
 *  @param      procId  destination procId.
 *  @param      intId   interrupt id.
 *  @param      fxn     callback funxction to be called on receiving interrupt.
 *  @param      fxnArgs arguments to the callback function.
 *
 *  @sa         Omap3530IpcInt_interruptUnregister
 */

Int32
Omap3530IpcInt_interruptRegister  (UInt16                     procId,
                                   UInt32                     intId,
                                   ArchIpcInt_CallbackFxn     fxn,
                                   Ptr                        fxnArgs)
{
    Int32          status = OMAP3530IPCINT_SUCCESS;
    OsalIsr_Params isrParams;

    GT_4trace (curTrace,
               GT_ENTER,
               "Omap3530IpcInt_interruptRegister",
               procId,
               intId,
               fxn,
               fxnArgs);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));
    GT_assert (curTrace, (fxn != NULL));

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&Omap3530IpcInt_state.isrRefCount,
                            OMAP3530IPCINT_MAKE_MAGICSTAMP(0),
                            OMAP3530IPCINT_MAKE_MAGICSTAMP(0));

    /* This is a normal use-case, so should not be inside
     * SYSLINK_BUILD_OPTIMIZE.
     */
    if (   Atomic_inc_return (&Omap3530IpcInt_state.isrRefCount)
        != OMAP3530IPCINT_MAKE_MAGICSTAMP(1u)) {
        /*! @retval OMAP3530IPCINT_S_ALREADYREGISTERED ISR already registered!
         */
        status = OMAP3530IPCINT_S_ALREADYREGISTERED;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "ISR already registered!");
    }
    else {
        /* ---------------------------------------------------------------------
         * Register the interrupt with OS
         * ---------------------------------------------------------------------
         */
        isrParams.sharedInt        = FALSE;
        isrParams.checkAndClearFxn = &_Omap3530IpcInt_checkAndClearFunc;
        isrParams.fxnArgs          = fxnArgs;
        isrParams.intId            = intId;

        Omap3530IpcInt_state.isrHandle = OsalIsr_create (fxn,
                                                         fxnArgs,
                                                         &isrParams);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (Omap3530IpcInt_state.isrHandle == NULL) {
            /*! @retval OMAP3530IPCINT_E_FAIL OsalIsr_create failed */
            status = OMAP3530IPCINT_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Omap3530IpcInt_interruptRegister",
                                 status,
                                 "OsalIsr_create failed");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            status = OsalIsr_install (Omap3530IpcInt_state.isrHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Omap3530IpcInt_interruptRegister",
                                     status,
                                     "OsalIsr_install failed");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* -------------------------------------------------------------
                 * Enable the hardware mailbox interrupt
                 * -------------------------------------------------------------
                 */
                /* Enable Mailboxes interface clock control. Set bit 7 */
                REG (   Omap3530IpcInt_state.archCoreCmBase
                     +  CM_ICLKEN1_CORE_OFFSET) |= 0x80;

                /* Perform software reset of the Mailbox module, set bit 1 */
                REG (   Omap3530IpcInt_state.mailboxBase
                     +  MAILBOX_SYSCONFIG_OFFSET) |= 0x02;

                /* Wait for reset complete. If Bit 0 == 0, it's still resetting */
                while (  (  REG (Omap3530IpcInt_state.mailboxBase
                          + MAILBOX_SYSSTATUS_OFFSET) & 1)
                       == 0) {
                    /* Wait for reset to complete */
                }

                /* Enable autoidle, set bit 0 */
                SET_BIT (   REG (  Omap3530IpcInt_state.mailboxBase
                                 + MAILBOX_SYSCONFIG_OFFSET),
                         0);

                /*
                 *  Configure the Mail BOX IRQENABLE register for DSP.
                 *  DSP receives on Mailbox 0.
                 */
                REG (   Omap3530IpcInt_state.mailboxBase
                     +  MAILBOX_IRQENABLE_1_OFFSET) = 0x1 ;
                /*
                 *  Configure the Mail BOX IRQENABLE register for GPP.
                 *  GPP receives on Mailbox 1.
                 */
                REG (   Omap3530IpcInt_state.mailboxBase
                     +  MAILBOX_IRQENABLE_0_OFFSET) = 0x4 ;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "Omap3530IpcInt_interruptRegister", status);

    /*! @retval OMAP3530IPCINT_SUCCESS Interrupt successfully registered */
    return status;
}


/*!
 *  @brief      Function to unregister interrupt.
 *
 *  @param      procId  destination procId
 *
 *  @sa         Omap3530IpcInt_interruptRegister
 */
Int32 Omap3530IpcInt_interruptUnregister(UInt16 procId, UInt32 intId,
        Ptr fxnArgs)
{
    Int32 status    = OMAP3530IPCINT_SUCCESS;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32 tmpStatus = OMAP3530IPCINT_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,GT_ENTER,"Omap3530IpcInt_interruptUnregister", procId);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert(curTrace, (procId < MultiProc_MAXPROCESSORS));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(Omap3530IpcInt_state.isrRefCount),
                                  OMAP3530IPCINT_MAKE_MAGICSTAMP(0),
                                  OMAP3530IPCINT_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval OMAP3530IPCINT_E_INVALIDSTATE ISR was not registered */
        status = OMAP3530IPCINT_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Omap3530IpcInt_interruptUnregister",
                             status,
                             "ISR was not registered!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* This is a normal use-case, so should not be inside
         * SYSLINK_BUILD_OPTIMIZE.
         */
        if (   Atomic_dec_return (&Omap3530IpcInt_state.isrRefCount)
            == OMAP3530IPCINT_MAKE_MAGICSTAMP(0)) {
            /* -----------------------------------------------------------------
             * Disable the hardware mailbox interrupt
             * -----------------------------------------------------------------
             */
            /*
             *  Disable the Mail BOX IRQENABLE register for DSP.
             *  DSP receives on Mailbox 0.
             */
            REG (Omap3530IpcInt_state.mailboxBase + MAILBOX_IRQENABLE_1_OFFSET)
                = 0x0;
            /*
             *  Disable the Mail BOX IRQENABLE register for GPP.
             *  GPP receives on Mailbox 1.
             */
            REG (Omap3530IpcInt_state.mailboxBase + MAILBOX_IRQENABLE_0_OFFSET)
                = 0x0;
            status = OsalIsr_uninstall (Omap3530IpcInt_state.isrHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Omap3530IpcInt_interruptUnregister",
                                     status,
                                     "OsalIsr_uninstall failed");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            tmpStatus =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                OsalIsr_delete (&(Omap3530IpcInt_state.isrHandle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if ((status >= 0) && (tmpStatus < 0)) {
                status = tmpStatus;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Omap3530IpcInt_interruptUnregister",
                                     status,
                                     "OsalIsr_delete failed");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Omap3530IpcInt_interruptUnregister",
               status);

    /*! @retval OMAP3530IPCINT_SUCCESS Interrupt successfully unregistered */
    return status;
}



/*!
 *  @brief      Function to enable the specified interrupt
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omap3530IpcInt_interruptDisable
 */
Void
Omap3530IpcInt_interruptEnable (UInt16 procId, UInt32 intId)
{
    GT_2trace (curTrace, GT_ENTER, "Omap3530IpcInt_interruptEnable",
               procId, intId);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    GT_assert (curTrace, (Omap3530IpcInt_state.isrHandle != NULL));
    OsalIsr_enableIsr (Omap3530IpcInt_state.isrHandle);

    GT_0trace (curTrace, GT_LEAVE, "Omap3530IpcInt_interruptEnable");
}


/*!
 *  @brief      Function to disable the specified interrupt
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omap3530IpcInt_interruptEnable
 */
Void
Omap3530IpcInt_interruptDisable (UInt16 procId, UInt32 intId)
{
    GT_2trace (curTrace, GT_ENTER, "Omap3530IpcInt_interruptDisable",
               procId, intId);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    GT_assert (curTrace, (Omap3530IpcInt_state.isrHandle != NULL));
    OsalIsr_disableIsr (Omap3530IpcInt_state.isrHandle);

    GT_0trace (curTrace, GT_LEAVE, "Omap3530IpcInt_interruptDisable");
}


/*!
 *  @brief      Function to wait for interrupt to be cleared.
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omap3530IpcInt_sendInterrupt
 */
Int32
Omap3530IpcInt_waitClearInterrupt (UInt16 procId, UInt32 intId)
{
    Int32 status = OMAP3530IPCINT_SUCCESS;

    GT_2trace (curTrace,GT_ENTER,"Omap3530IpcInt_waitClearInterrupt",
               procId, intId);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* Wait for GPP to clear the previous interrupt */
    while ((REG32((Omap3530IpcInt_state.mailboxBase + MAILBOX_MSGSTATUS_1_OFFSET)) & 7 ));

    GT_1trace (curTrace,GT_LEAVE,"Omap3530IpcInt_waitClearInterrupt", status);

    /*! @retval OMAP3530IPCINT_SUCCESS Wait for interrupt clearing successfully
                completed. */
    return status;
}


/*!
 *  @brief      Function to send a specified interrupt to the DSP.
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *  @param      value   Value to be sent with the interrupt
 *
 *  @sa         Omap3530IpcInt_waitClearInterrupt
 */
Int32
Omap3530IpcInt_sendInterrupt (UInt16 procId, UInt32 intId, UInt32 value)
{
    Int32 status = OMAP3530IPCINT_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "Omap3530IpcInt_sendInterrupt",
               procId, intId, value);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* Put into the DSP's mailbox to generate the interrupt. */
    REG32(Omap3530IpcInt_state.mailboxBase + MAILBOX_MESSAGE_0_OFFSET) = value;

    GT_1trace (curTrace, GT_LEAVE, "Omap3530IpcInt_sendInterrupt", status);

    /*! @retval OMAP3530IPCINT_SUCCESS Interrupt successfully sent */
    return status;
}


/*!
 *  @brief      Function to clear the specified interrupt received from the DSP.
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omap3530IpcInt_sendInterrupt
 */
UInt32
Omap3530IpcInt_clearInterrupt (UInt16 procId, UInt32 intId)
{
    UInt32 retVal;

    GT_2trace (curTrace,GT_ENTER,"Omap3530IpcInt_clearInterrupt",
               procId, intId);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* Read the register to get the entry from the mailbox FIFO */
    retVal = REG32(Omap3530IpcInt_state.mailboxBase + MAILBOX_MESSAGE_1_OFFSET) ;

    /* Clear the IRQ status.
     * If there are more in the mailbox FIFO, it will re-assert.
     */
    REG32(Omap3530IpcInt_state.mailboxBase + MAILBOX_IRQSTATUS_0_OFFSET) = 0x4 ;/* mailbox 1*/

    GT_0trace (curTrace, GT_LEAVE, "Omap3530IpcInt_clearInterrupt");

    /*! @retval Value Value received with the interrupt. */
    return retVal ;
}


/*!
 *  @brief      Function to check and clear the remote proc interrupt
 *
 *  @param      arg     Optional argument to the function.
 *
 *  @sa
 */
static
Bool
_Omap3530IpcInt_checkAndClearFunc (Ptr arg)
{
    Omap3530IpcInt_clearInterrupt (Omap3530IpcInt_state.procId,
                                   Omap3530IpcInt_state.recvIntId);

    /* This is not a shared interrupt, so interrupt has always occurred */
    /*! @retval TRUE Interrupt has occurred. */
    return (TRUE);
}
