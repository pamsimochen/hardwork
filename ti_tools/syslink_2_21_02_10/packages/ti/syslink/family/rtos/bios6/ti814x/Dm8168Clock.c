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
 *  @file   ti/syslink/family/rtos/bios6/ti814x/Dm8168Clock.c
 *
 *  @brief  RTOS side Clock interface for DM8168
 */

/* Module level headers */
#include <xdc/std.h>
#include <ti/syslink/inc/ClockOps.h>
#include <ti/syslink/inc/Dm8168Clock.h>
#include <xdc/runtime/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <string.h>



/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

#define REG(x)              *((volatile UInt32 *) (x))
#define MEM(x)              *((volatile UInt32 *) (x))


/*!
 *  @brief  Sets bit at given position.
 *          This macro is independent of operand width. User must ensure
 *          correctness.
 */
#define SET_BIT(num,pos)            ((num) |= (1u << (pos)))

/*!
 *  @brief  Clears bit at given position.
 *          This macro is independent of operand width. User must ensure
 *          correctness.
 */
#define CLEAR_BIT(num,pos)          ((num) &= ~(1u << (pos)))

/*!
 *  @brief  Tests if bit at given position is set.
 *          This macro is independent of operand width. User must ensure
 *          correctness.
 */
#define TEST_BIT(num,pos)           ((((num) & (1u << (pos))) >> (pos)) & 0x01)


/* PRCM base address */
#define PRCM_BASE_ADDR              0x48180000
/* PRCM size */
#define PRCM_SIZE                   0x00003000
/* Control module base address */
#define CTRL_MODULE_BASE_ADDR       0x48140000
/* Ducati mmu conifguration memory address */
#define DUCATI_MMU_CFG              0x55080000

/* DSP Boot address */
#define DSP_BOOT_ADDR               (CTRL_MODULE_BASE_ADDR + 0x0048)
/* GEM L2RAM config address */
#define DSPMEM_SLEEP                (CTRL_MODULE_BASE_ADDR + 0x0650)
/* DSP config address */
#define DSP_IDLE_CFG                (CTRL_MODULE_BASE_ADDR + 0x061c)
/* DSP /GEM clocks */
#define CM_ACTIVE_GEM_CLKCTRL       (PRCM_BASE_ADDR + 0x0420)
#define CM_GEM_CLKSTCTRL            (PRCM_BASE_ADDR + 0x0400)

/* Ducati general control base address */
#define GENERAL_CONTROL_BASE         0x55020000
/* VIDEO-M3 Boot address */
#define VIDEOM3_BOOT_ADDR           (GENERAL_CONTROL_BASE + 0x4)
/* VPSS-M3 Boot address */
#define VPSSM3_BOOT_ADDR            (GENERAL_CONTROL_BASE + 0x4)

/* Spinbox clock control address */
#define CM_ALWON_SPINBOX_CLKCTRL    (PRCM_BASE_ADDR + 0x1598)
/* Mailbox clock control address */
#define CM_ALWON_MAILBOX_CLKCTRL    (PRCM_BASE_ADDR + 0x1594)
#define CM_ALWON_L3_SLOW_CLKSTCTRL  (PRCM_BASE_ADDR + 0x1400)
#define CM_ALWON_L4_SLOW_CLKSTCTRL  (PRCM_BASE_ADDR + 0x1400)
#define CM_ALWON_TIMER_2_CLKCTRL    (PRCM_BASE_ADDR + 0x1574)
#define CM_ALWON_TIMER_3_CLKCTRL    (PRCM_BASE_ADDR + 0x1578)
#define CM_DEFAULT_DUCATI_CLKSTCTRL (PRCM_BASE_ADDR + 0x0518)
#define CM_DEFAULT_DUCATI_CLKCTRL   (PRCM_BASE_ADDR + 0x0574)
#define CM_MMUCFG_CLKSTCTRL         (PRCM_BASE_ADDR + 0x1410)
#define CM_MMU_CLKSTCTRL            (PRCM_BASE_ADDR + 0x140C)
#define CM_ALWON_MMUDATA_CLKCTRL    (PRCM_BASE_ADDR + 0x159C)
#define CM_ALWON_MMUCFG_CLKCTRL     (PRCM_BASE_ADDR + 0x15A8)
#define PM_ACTIVE_PWRSTST           (PRCM_BASE_ADDR + 0x0A04)

/*!
 *  @brief  Reset control for GEM
 */
#define RM_ACTIVE_RSTCTRL           (PRCM_BASE_ADDR + 0x0A10)
/*!
 *  @brief  Reset control for Ducati
 */
#define RM_DEFAULT_RSTCTRL          (PRCM_BASE_ADDR + 0x0B10)
#define RM_DEFAULT_RSTST            (PRCM_BASE_ADDR + 0x0B14)

#define SPINLOCK  "spinbox_ick"
#define MAILBOX   "mailbox_ick"
#define GPT4ICK   "gpt4_ick"
#define GPT4FCK   "gpt4_fck"
#define GPT3ICK   "gpt3_ick"
#define GPT3FCK   "gpt3_fck"
#define MMUCFGICK "mmu_cfg_ick"
#define MMUICK    "mmu_ick"
#define GEMICK    "gem_ick"
#define DUCATICLK "ducati_ick"

/* =============================================================================
 * APIs
 * =============================================================================
 */

/*!
 *  @brief      Function creates the clock object
 *
 *  @sa         DM8168CLOCK_create
 */
ClockOps_Handle
DM8168CLOCK_create(Void)
{
    ClockOps_Object *    handle = NULL;

    /* Allocate memory for the handle */
    handle = (ClockOps_Object *) Memory_calloc (NULL,
                                                sizeof (ClockOps_Object),
                                                0,
                                                NULL);
    GT_assert(curlTrace, (handle != NULL));

    handle->clkFxnTable.get          = &DM8168CLOCK_get;
    handle->clkFxnTable.put          = &DM8168CLOCK_put;
    handle->clkFxnTable.enable       = &DM8168CLOCK_enable;
    handle->clkFxnTable.disable      = &DM8168CLOCK_disable;
    handle->clkFxnTable.getRate      = &DM8168CLOCK_getRate;
    handle->clkFxnTable.setRate      = &DM8168CLOCK_setRate;

    return ((ClockOps_Handle) handle);
}

 /*!
 *  @brief      Function deletes the clock object
 *
 *  @sa         DM8168CLOCK_delete
 */
Int
DM8168CLOCK_delete (ClockOps_Handle handle)
{
    Int                  status = 0;
    Memory_free (NULL,
                 handle,
                 sizeof (ClockOps_Object));
    handle = NULL;
    return (status);
}

/*!
 *  @brief      Function returns the clock handle .
 *
 *  @clkHandle   clk handle returned to corresponding clk name
 *  @clkname     name of clk for which handle is to be obtained
 *
 *  @sa         DM8168CLOCK_put
 */
Ptr
DM8168CLOCK_get(String clkname)
{
    Ptr handle = NULL;

    if(strcmp(clkname,SPINLOCK) == 0)
    {
           handle =(Ptr)(CM_ALWON_SPINBOX_CLKCTRL);
    }
    else if(strcmp(clkname,MAILBOX) == 0)
    {
           handle =(Ptr)(CM_ALWON_MAILBOX_CLKCTRL);
    }
    else if(strcmp(clkname,GPT4ICK) == 0)
    {
           handle =(Ptr)(CM_ALWON_TIMER_3_CLKCTRL);
    }
    else if(strcmp(clkname,GPT4FCK) == 0)
    {
           handle =(Ptr)(CM_ALWON_TIMER_3_CLKCTRL);
    }
    else if(strcmp(clkname,GPT3ICK) == 0)
    {
           handle =(Ptr)(CM_ALWON_TIMER_2_CLKCTRL);
    }
    else if(strcmp(clkname,GPT3FCK) == 0)
    {
           handle =(Ptr)(CM_ALWON_TIMER_2_CLKCTRL);
    }
    else if(strcmp(clkname,MMUCFGICK) == 0)
    {
           handle =(Ptr)(CM_MMUCFG_CLKSTCTRL);
    }
    else if(strcmp(clkname,MMUICK) == 0)
    {
           handle =(Ptr)(CM_MMU_CLKSTCTRL);
    }
    else if(strcmp(clkname,GEMICK) == 0)
    {
           handle =(Ptr)(CM_GEM_CLKSTCTRL);
    }
    else if(strcmp(clkname,DUCATICLK) == 0)
    {
           handle =(Ptr)(CM_DEFAULT_DUCATI_CLKCTRL);
    }
    else {
           handle = NULL;
    }
    return (handle);
}

/*!
 *  @brief       Function to release a Clock
 *
 *  @clkHandle   Handle to the clock
 *
 *  @sa         DM8168CLOCK_put
 */
Void
DM8168CLOCK_put(Ptr clkHandle)
{
    /* Nothing to be done */
}

/*!
 *  @brief       Function to enable a Clock
 *
 *  @clkHandle   Handle to the clock
 *
 *  @sa         DM8168CLOCK_put
 */
Int32
DM8168CLOCK_enable(Ptr clkHandle)
{
    int    status  = 0;
    UInt32 address = (UInt32)clkHandle;
    UInt32 value   = 0x2;

    switch(address){
        case CM_ALWON_SPINBOX_CLKCTRL:
        {
            REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = value;
            REG(CM_ALWON_SPINBOX_CLKCTRL) = value;
        }
        break;
        case CM_ALWON_MAILBOX_CLKCTRL:
        {
            REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = value;
            REG(CM_ALWON_MAILBOX_CLKCTRL) = value;
        }
        break;
        case CM_ALWON_TIMER_3_CLKCTRL:
        {
           REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = value;
           REG(CM_ALWON_TIMER_3_CLKCTRL) = value;
        }
        break;
        case CM_ALWON_TIMER_2_CLKCTRL:
        {
           REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = value;
           REG(CM_ALWON_TIMER_2_CLKCTRL) = value;
        }
        break;
        case CM_MMUCFG_CLKSTCTRL:
        {
           REG(CM_MMUCFG_CLKSTCTRL) = value;
           REG(CM_ALWON_MMUCFG_CLKCTRL) = value;
        }
        break;
        case CM_MMU_CLKSTCTRL:
        {
           REG(CM_MMU_CLKSTCTRL) = value;
           REG(CM_ALWON_MMUDATA_CLKCTRL) = value;
        }
        break;
        case CM_GEM_CLKSTCTRL:
        {
           REG(CM_GEM_CLKSTCTRL) = value;
           REG(CM_ACTIVE_GEM_CLKCTRL) = value;
        }
        break;
        case CM_DEFAULT_DUCATI_CLKCTRL:
        {
           REG(CM_DEFAULT_DUCATI_CLKSTCTRL) = value;
           REG(CM_DEFAULT_DUCATI_CLKCTRL) = value;
        }
        break;
        default:
        {
             status = ClockOps_E_FAIL;
        }
        break;
    }

    return (status);
}

/*!
 *  @brief       Function to disable a Clock
 *
 *  @clkHandle   Handle to the clock
 *
 *  @sa         DM8168CLOCK_put
 */
Void
DM8168CLOCK_disable(Ptr clkHandle)
{
    UInt32 address = (UInt32)clkHandle;
    UInt32 value   = 0x0;

    switch(address){
        case CM_ALWON_SPINBOX_CLKCTRL:
        {
           REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = 0x1;
           REG(CM_ALWON_SPINBOX_CLKCTRL) = 0x0;
        }
        break;
        case CM_ALWON_MAILBOX_CLKCTRL:
        {
           REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = 0x1;
           REG(CM_ALWON_MAILBOX_CLKCTRL) = 0x0;
        }
        break;
        case CM_ALWON_TIMER_3_CLKCTRL:
        {
           REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = 0x1;
           REG(CM_ALWON_TIMER_3_CLKCTRL) = 0x0;
        }
        break;
        case CM_ALWON_TIMER_2_CLKCTRL:
        {
           REG(CM_ALWON_L3_SLOW_CLKSTCTRL) = 0x1;
           REG(CM_ALWON_TIMER_2_CLKCTRL) = 0x0;
        }
        break;
        case CM_MMUCFG_CLKSTCTRL:
        {
           REG(CM_MMUCFG_CLKSTCTRL) = 0x0;
           REG(CM_ALWON_MMUCFG_CLKCTRL) = 0x0;
        }
        break;
        case CM_MMU_CLKSTCTRL:
        {
           REG(CM_MMU_CLKSTCTRL) = value;
           REG(CM_ALWON_MMUDATA_CLKCTRL) = value;
        }
        break;
        case CM_GEM_CLKSTCTRL:
        {
           REG(CM_GEM_CLKSTCTRL) = 0x01;
           REG(CM_ACTIVE_GEM_CLKCTRL) = 0x01;
        }
        break;
        case CM_DEFAULT_DUCATI_CLKCTRL:
        {
           REG(CM_DEFAULT_DUCATI_CLKSTCTRL) = 0x1;
           REG(CM_DEFAULT_DUCATI_CLKCTRL) = 0x0;
        }
        break;
        default:
        {
         /* Nothing to do */
        }
        break;
    }
}

/*!
 *  @brief      Function gets the Clock speed .
 *
 *  @handle     Ptr to the clockOps object.conaining the clock function.table
 *  @clkHandle   Handle to the clock
 *
 *  @sa         DM8168CLOCK_put
 */
ULong
DM8168CLOCK_getRate(Ptr clkHandle)
{
    return(0);
}

/*!
 *  @brief       Function sets the dlock speed
 *
 *  @handle      Ptr to the clockOps object.conaining the clock function.table
 *  @clkHandle   Handle to the clock
 *  @rate        Clock speed to be set
 *
 *  @sa         DM8168CLOCK_put
 */
Int32
DM8168CLOCK_setRate(Ptr clkHandle, ULong rate)
{
    return (0);
}
