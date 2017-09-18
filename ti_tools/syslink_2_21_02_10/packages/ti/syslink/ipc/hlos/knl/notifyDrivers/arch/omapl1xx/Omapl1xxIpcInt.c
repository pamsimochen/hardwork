/*
 *  @file   Omapl1xxIpcInt.c
 *
 *  @brief      OMAPL1xx DSP IPC interrupts.
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
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/irq.h>

/* OSAL and utils headers */
#include <ti/syslink/inc/knl/OsalIsr.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/utils/Memory.h>

/* Hardware Abstraction Layer */
#include <ti/syslink/inc/knl/_ArchIpcInt.h>
#include <ti/syslink/inc/knl/_Omapl1xxIpcInt.h>
#include <ti/syslink/inc/knl/Omapl1xxIpcInt.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define OMAPL1XXIPCINT_MAKE_MAGICSTAMP(x) \
                                    ((OMAPL1XXIPCINT_MODULEID << 12u) | (x))

/*!
 *  @def    REG
 *  @brief  Regsiter access method.
 */
#define REG(x)          *((volatile UInt32 *) (x))
#define REG32(x)        (*(volatile UInt32 *) (x))

/*!
 *  @def    OFFSET_SYSTEM_MODULE
 *  @brief  Offset of the system module registers from the base of the CFG
 *          memory.
 */
#define  OFFSET_SYSTEM_MODULE   0x14000u

/*!
 *  @def    OFFSET_CHIPSIG
 *  @brief  Offset of the register to set interrupt from system module base.
 */
#define  OFFSET_CHIPSIG          0x174u

/*!
 *  @def    OFFSET_CHIPSIG_CLR
 *  @brief  Offset of the register to clear interrupt from system module base.
 */
#define  OFFSET_CHIPSIG_CLR      0x178u

/*!
 *  @def    NUM_ARM_TO_DSP_INT
 *  @brief  Defines the number of ARM-DSP interrupts supported by the
 *          module.
 */
#define  NUM_ARM_TO_DSP_INT     2u

/*!
 *  @def    NUM_ARM_TO_DSP_INT
 *  @brief  Defines the number of DSP-ARM interrupts supported by the
 *          module.
 */
#define  NUM_DSP_TO_ARM_INT     2u

/*!
 *  @def    BITPOS_ARM2DSPINTSET
 *  @brief  Start position of the ARM2DSP interrupt set bits in the CHIPSIG
 *          register.
 */
#define  BITPOS_ARM2DSPINTSET   2u

/*!
 *  @def    BITPOS_DSP2ARMINTCLR
 *  @brief  Start position of the DSP2ARM interrupt clear bits in the
 *          CHIPSIG_CLR register.
 */
#define  BITPOS_DSP2ARMINTCLR   0

/*!
 *  @def    BASE_ARM2DSP_INTID
 *  @brief  Interrupt number of the first ARM2DSP interrupt.
 */
#define  BASE_ARM2DSP_INTID     5u

/*!
 *  @def    ARM2DSP_INT_INDEX
 *  @brief  Index of the ARM2DSP interrupt (0/1) based on the interrupt
 *          number.
 *          Different logic as CHIPINT2 is 5 and CHIPINT3 is 67
 */
#define  ARM2DSP_INT_INDEX(intId)  ((intId  == 5u) ? 0u : 1u)

/*!
 *  @def    BASE_DSP2ARM_INTID
 *  @brief  Interrupt number of the first DSP2ARM interrupt.
 */
#define  BASE_DSP2ARM_INTID     28u

/*!
 *  @def    DSP2ARM_INT_INDEX
 *  @brief  Index of the DSP2ARM interrupt (0/1) based on the interrupt number.
 */
#define  DSP2ARM_INT_INDEX(intId)  ((intId) - (BASE_DSP2ARM_INTID))


/*!
 *  @brief  Device specific object
 *          It can be populated as per device need and it is used internally in
 *          the device specific implementation only.
 */
typedef struct Omapl1xxIpcInt_ModuleObject_tag {
    Atomic         isrRefCount[2];
    /* ISR Reference count */
    UInt16         procId;
    /* Processor ID of destination processor */
    OsalIsr_Handle isrHandle[2];
    /*!< Handle to the OsalIsr object */
    UInt32         baseCfgBus;
    /*!< Virtual address of the cfg bus base */
} Omapl1xxIpcInt_ModuleObject;

/*!
 *  @brief  Forward declaration of check and clear function
 */
static
Bool
_Omapl1xxIpcInt_checkAndClearFunc (Ptr arg);


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @brief  State object for Omapl1xxIpcInt
 */
Omapl1xxIpcInt_ModuleObject Omapl1xxIpcInt_state = {
    isrHandle: { NULL, NULL },
    baseCfgBus: 0x0
};

/*!
 *  @brief  Function table for OMAPL1XX
 */
ArchIpcInt_FxnTable Omapl1xxIpcInt_fxnTable = {
    Omapl1xxIpcInt_interruptRegister,
    Omapl1xxIpcInt_interruptUnregister,
    Omapl1xxIpcInt_interruptEnable,
    Omapl1xxIpcInt_interruptDisable,
    Omapl1xxIpcInt_waitClearInterrupt,
    Omapl1xxIpcInt_sendInterrupt,
    Omapl1xxIpcInt_clearInterrupt,
};


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*!
 *  @brief      Function to initialize the Omapl1xxIpcInt module.
 *
 *  @param      cfg  Configuration for setup
 *
 *  @sa         Omapl1xxIpcInt_destroy
 */
Void
Omapl1xxIpcInt_setup (Omapl1xxIpcInt_Config * cfg)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int            status = OMAPL1XXIPCINT_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Memory_MapInfo mapInfo;

    GT_1trace (curTrace, GT_ENTER, "Omapl1xxIpcInt_setup", cfg);

    GT_assert (curTrace, (cfg != NULL));

    /* The setup will be called only once, either from SysMgr or from
     * archipcomap3530 module. Hence it does not need to be atomic.
     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                        GT_4CLASS,
                        "Omapl1xxIpcInt_setup",
                        OMAPL1XXIPCINT_E_FAIL,
                        "config for driver specific setup can not be NULL");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Omapl1xxIpcInt_state.procId = cfg->procId;

        mapInfo.src = IO_PHYS;
        mapInfo.size = IO_SIZE;
        mapInfo.isCached = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Memory_map (&mapInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Omapl1xxIpcInt_setup",
                                 status,
                                 "Memory_map failed");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            Omapl1xxIpcInt_state.baseCfgBus = mapInfo.dst;
            ArchIpcInt_object.fxnTable     = &Omapl1xxIpcInt_fxnTable;
            ArchIpcInt_object.obj          = &Omapl1xxIpcInt_state;
            ArchIpcInt_object.isSetup      = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Omapl1xxIpcInt_setup");
}


/*!
 *  @brief      Function to finalize the Omapl1xxIpcInt module
 *
 *  @sa         Omapl1xxIpcInt_setup
 */
Void
Omapl1xxIpcInt_destroy (Void)
{
    Memory_UnmapInfo unmapInfo;

    GT_0trace (curTrace, GT_ENTER, "Omapl1xxIpcInt_destroy");

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));

    ArchIpcInt_object.isSetup  = FALSE;
    ArchIpcInt_object.obj      = NULL;
    ArchIpcInt_object.fxnTable = NULL;
    if (Omapl1xxIpcInt_state.baseCfgBus != (UInt32) NULL) {
        unmapInfo.addr = Omapl1xxIpcInt_state.baseCfgBus;
        unmapInfo.size = IO_SIZE;
        unmapInfo.isCached = FALSE;
        Memory_unmap (&unmapInfo);
        Omapl1xxIpcInt_state.baseCfgBus = (UInt32) NULL;
    }

    GT_0trace (curTrace, GT_ENTER, "Omapl1xxIpcInt_destroy");
}


/*!
 *  @brief      Function to register the interrupt.
 *
 *  @param      procId  destination procId.
 *  @param      intId   interrupt id.
 *  @param      fxn     callback funxction to be called on receiving interrupt.
 *  @param      fxnArgs arguments to the callback function.
 *
 *  @sa         Omapl1xxIpcInt_interruptUnregister
 */
Int32 Omapl1xxIpcInt_interruptRegister(UInt16 procId, UInt32 intId,
        ArchIpcInt_CallbackFxn fxn, Ptr fxnArgs)
{
    Int32           status = OMAPL1XXIPCINT_SUCCESS;
    OsalIsr_Params  isrParams;
    Int             isrIdx;

    GT_4trace(curTrace, GT_ENTER, "Omapl1xxIpcInt_interruptRegister",
            procId, intId, fxn, fxnArgs);

    GT_assert(curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert(curTrace, (procId < MultiProc_MAXPROCESSORS));
    GT_assert(curTrace, (fxn != NULL));

    /* compute the interrupt index value */
    isrIdx = DSP2ARM_INT_INDEX(intId);

    /* initialize ref counter if not already initialized */
    Atomic_cmpmask_and_set(&Omapl1xxIpcInt_state.isrRefCount[isrIdx],
            OMAPL1XXIPCINT_MAKE_MAGICSTAMP(0),
            OMAPL1XXIPCINT_MAKE_MAGICSTAMP(0));

    if (Atomic_inc_return(&Omapl1xxIpcInt_state.isrRefCount[isrIdx])
            != OMAPL1XXIPCINT_MAKE_MAGICSTAMP(1u)) {

        status = OMAPL1XXIPCINT_S_ALREADYREGISTERED;
        GT_0trace(curTrace, GT_2CLASS, "ISR already registered!");
    }
    else {
        isrParams.sharedInt        = FALSE;
        isrParams.checkAndClearFxn = &_Omapl1xxIpcInt_checkAndClearFunc;
        isrParams.fxnArgs          = (Ptr) intId;
        isrParams.intId            = intId;

        Omapl1xxIpcInt_state.isrHandle[isrIdx] = OsalIsr_create(fxn, fxnArgs,
                &isrParams);

        if (Omapl1xxIpcInt_state.isrHandle[isrIdx] == NULL) {
            status = OMAPL1XXIPCINT_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS,
                    "Omapl1xxIpcInt_interruptRegister", status,
                    "OsalIsr_create failed");
        }
        else {
            status = OsalIsr_install(Omapl1xxIpcInt_state.isrHandle[isrIdx]);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "Omapl1xxIpcInt_interruptRegister", status,
                        "OsalIsr_install failed");
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "Omapl1xxIpcInt_interruptRegister", status);

    return(status);
}


/*!
 *  @brief      Function to unregister interrupt.
 *
 *  @param      procId  destination procId
 *
 *  @sa         Omapl1xxIpcInt_interruptRegister
 */
Int32 Omapl1xxIpcInt_interruptUnregister(UInt16 procId, UInt32 intId,
        Ptr fxnArgs)
{
    Int32   status = OMAPL1XXIPCINT_SUCCESS;
    Int32   tmpStatus = OMAPL1XXIPCINT_SUCCESS;
    Int     isrIdx;

    GT_1trace(curTrace, GT_ENTER,"Omapl1xxIpcInt_interruptUnregister", procId);

    GT_assert(curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert(curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* compute the interrupt index value */
    isrIdx = DSP2ARM_INT_INDEX(intId);

    if (Atomic_cmpmask_and_lt(&(Omapl1xxIpcInt_state.isrRefCount[isrIdx]),
            OMAPL1XXIPCINT_MAKE_MAGICSTAMP(0),
            OMAPL1XXIPCINT_MAKE_MAGICSTAMP(1))) {

        status = OMAPL1XXIPCINT_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS,
                "Omapl1xxIpcInt_interruptUnregister", status,
                "ISR was not registered!");
    }
    else {
        if (Atomic_dec_return(&Omapl1xxIpcInt_state.isrRefCount[isrIdx])
                == OMAPL1XXIPCINT_MAKE_MAGICSTAMP(0)) {

            status = OsalIsr_uninstall(Omapl1xxIpcInt_state.isrHandle[isrIdx]);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "Omapl1xxIpcInt_interruptUnregister", status,
                        "OsalIsr_uninstall failed");
            }

            tmpStatus = OsalIsr_delete(&Omapl1xxIpcInt_state.isrHandle[isrIdx]);

            if ((status >= 0) && (tmpStatus < 0)) {
                status = tmpStatus;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "Omapl1xxIpcInt_interruptUnregister", status,
                        "OsalIsr_delete failed");
            }
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "Omapl1xxIpcInt_interruptUnregister", status);

    return(status);
}


/*!
 *  @brief      Function to enable the specified interrupt
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omapl1xxIpcInt_interruptDisable
 */
Void Omapl1xxIpcInt_interruptEnable(UInt16 procId, UInt32 intId)
{
    Int isrIdx;

    GT_2trace(curTrace, GT_ENTER, "Omapl1xxIpcInt_interruptEnable",
            procId, intId);

    GT_assert(curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert(curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* compute the interrupt index value */
    isrIdx = DSP2ARM_INT_INDEX(intId);

    OsalIsr_enableIsr(Omapl1xxIpcInt_state.isrHandle[isrIdx]);

    GT_0trace(curTrace, GT_LEAVE, "Omapl1xxIpcInt_interruptEnable");
}


/*!
 *  @brief      Function to disable the specified interrupt
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omapl1xxIpcInt_interruptEnable
 */
Void Omapl1xxIpcInt_interruptDisable(UInt16 procId, UInt32 intId)
{
    Int isrIdx;

    GT_2trace(curTrace, GT_ENTER, "Omapl1xxIpcInt_interruptDisable",
            procId, intId);

    GT_assert(curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert(curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* compute the interrupt index value */
    isrIdx = DSP2ARM_INT_INDEX(intId);

    OsalIsr_disableIsr(Omapl1xxIpcInt_state.isrHandle[isrIdx]);

    GT_0trace(curTrace, GT_LEAVE, "Omapl1xxIpcInt_interruptDisable");
}


/*!
 *  @brief      Function to wait for interrupt to be cleared.
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omapl1xxIpcInt_sendInterrupt
 */
Int32
Omapl1xxIpcInt_waitClearInterrupt (UInt16 procId, UInt32 intId)
{
    Int32 status = OMAPL1XXIPCINT_SUCCESS;

    GT_2trace (curTrace,GT_ENTER,"Omapl1xxIpcInt_waitClearInterrupt",
               procId, intId);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    /* Wait for DSP to clear the previous interrupt */
    /* TBD: Not used currently. */

    GT_1trace (curTrace,GT_LEAVE,"Omapl1xxIpcInt_waitClearInterrupt", status);

    /*! @retval OMAPL1XXIPCINT_SUCCESS Wait for interrupt clearing successfully
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
 *  @sa         Omapl1xxIpcInt_waitClearInterrupt
 */
Int32
Omapl1xxIpcInt_sendInterrupt (UInt16 procId, UInt32 intId, UInt32 value)
{
    Int32  status = OMAPL1XXIPCINT_SUCCESS;
    UInt32 index;
    UInt32 addr;

    GT_3trace (curTrace, GT_ENTER, "Omapl1xxIpcInt_sendInterrupt",
               procId, intId, value);

    GT_assert (curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert (curTrace, (procId < MultiProc_MAXPROCESSORS));

    index = ARM2DSP_INT_INDEX (intId);
    addr  =  Omapl1xxIpcInt_state.baseCfgBus
           + OFFSET_SYSTEM_MODULE
           + OFFSET_CHIPSIG;

    /* Send the interrupt */
    SET_BIT (*((UInt32 *) addr), (index + BITPOS_ARM2DSPINTSET));

    GT_1trace (curTrace, GT_LEAVE, "Omapl1xxIpcInt_sendInterrupt", status);

    /*! @retval OMAPL1XXIPCINT_SUCCESS Interrupt successfully sent */
    return status;
}


/*!
 *  @brief      Function to clear the specified interrupt received from the DSP.
 *
 *  @param      procId  Remote processor ID
 *  @param      intId   interrupt id
 *
 *  @sa         Omapl1xxIpcInt_sendInterrupt
 */
UInt32 Omapl1xxIpcInt_clearInterrupt(UInt16 procId, UInt32 intId)
{
    UInt32 retVal = 0x0;
    UInt32 index;
    UInt32 addr;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
    struct irq_data *irq_data;
#else
    struct irq_desc *irq_desc;
#endif

    GT_2trace(curTrace,GT_ENTER,"Omapl1xxIpcInt_clearInterrupt", procId, intId);

    GT_assert(curTrace, (ArchIpcInt_object.isSetup == TRUE));
    GT_assert(curTrace, (procId < MultiProc_MAXPROCESSORS));

    addr = Omapl1xxIpcInt_state.baseCfgBus + OFFSET_SYSTEM_MODULE
            + OFFSET_CHIPSIG_CLR;
    index = DSP2ARM_INT_INDEX(intId);

    /* clear the interrupt */
    *(volatile unsigned int *)(addr) = (1 << (index + BITPOS_DSP2ARMINTCLR));

    /*
     * ACK intr to AINTC.
     *
     * It has already been ack'ed by the kernel before calling
     * this function, but since the ARM<->DSP interrupts in the
     * CHIPSIG register are "level" instead of "pulse" variety,
     * we need to ack it after taking down the level else we'll
     * be called again immediately after returning.
     */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
    irq_data = irq_get_irq_data(intId);
    irq_data->chip->irq_ack(irq_data);
#else
    irq_desc = irq_to_desc(intId);
    irq_desc->chip->ack(intId);
#endif

    GT_0trace(curTrace, GT_LEAVE, "Omapl1xxIpcInt_clearInterrupt");

    /*! @retval Value Value received with the interrupt. */
    return(retVal);
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
_Omapl1xxIpcInt_checkAndClearFunc (Ptr arg)
{
    Omapl1xxIpcInt_clearInterrupt (Omapl1xxIpcInt_state.procId,
                                   (UInt32) arg);

    /* This is not a shared interrupt, so interrupt has always occurred */
    /*! @retval TRUE Interrupt has occurred. */
    return (TRUE);
}
