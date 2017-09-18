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
 *  @file   ti/syslink/family/hlos/knl/Linux/LinuxClock.c
 *
 *  @brief  Linux specific clock interface common across platforms
 */

/* OS headers */
#include <linux/clk.h>
#include <plat/cpu.h>
#include <ti/syslink/Std.h>
#include <ti/syslink/inc/ClockOps.h>
/* Module level headers */
#include <ti/syslink/inc/knl/Linux/LinuxClock.h>
#include <ti/syslink/utils/Memory.h>


/* =============================================================================
 * APIs
 * =============================================================================
 */
 /*!
 *  @brief      Function creates the clock object
 *
 *  @sa         LinuxClock_delete
 */
ClockOps_Handle LinuxClock_create(Void)
{
    ClockOps_Object *      handle = NULL;

    /* Allocate memory for the handle */
    handle = (ClockOps_Object *) Memory_calloc(NULL, sizeof(ClockOps_Object),
            0, NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "LinuxClock_create",
                ClockOps_E_MEMORY, "LinuxClock_create failed");
    }
    else {
#endif
        handle->clkFxnTable.get          = &LinuxClock_get;
        handle->clkFxnTable.put          = &LinuxClock_put;
        handle->clkFxnTable.enable       = &LinuxClock_enable;
        handle->clkFxnTable.disable      = &LinuxClock_disable;
        handle->clkFxnTable.getRate      = &LinuxClock_getRate;
        handle->clkFxnTable.setRate      = &LinuxClock_setRate;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

     return ((ClockOps_Handle)handle);
}

 /*!
 *  @brief      Function deletes the clock object
 *
 *  @sa         LinuxClock_delete
 */
Int LinuxClock_delete(ClockOps_Handle handle)
{
    Int                  status = 0;

    Memory_free(NULL, handle, sizeof(ClockOps_Object));

    handle = NULL;

    return (status);
}

/*!
 *  @brief      Function returns the clock handle .
 *
 *  @clkname     name of clk for which handle is to be obtained
 *
 *  @sa         LinuxClock_put
 */
Ptr LinuxClock_get(String clkname)
{
    return ((Ptr)clk_get(NULL,(const char *)clkname));
}

/*!
 *  @brief       Function to release a Clock
 *
 *  @clkHandle   Handle to the clock
 *
 *  @sa         LinuxClock_put
 */
Void LinuxClock_put(Ptr clkHandle)
{
    clk_put((struct clk *)clkHandle);
}

/*!
 *  @brief       Function to enable a Clock
 *
 *  @clkHandle   Handle to the clock
 *
 *  @sa         LinuxClock_put
 */
Int32 LinuxClock_enable(Ptr clkHandle)
{
    return (clk_enable((struct clk *)clkHandle));
}

/*!
 *  @brief       Function to disable a Clock
 *
 *  @handle      Ptr to the clockOps object.conaining the clock function.table
 *  @clkHandle   Handle to the clock
 *
 *  @sa         LinuxClock_put
 */
Void LinuxClock_disable(Ptr clkHandle)
{
    clk_disable((struct clk *)clkHandle);
}

/*!
 *  @brief      Function gets the Clock speed .
 *
 *  @handle     Ptr to the clockOps object.conaining the clock function.table
 *  @clkHandle   Handle to the clock
 *
 *  @sa         LinuxClock_put
 */
ULong LinuxClock_getRate(Ptr clkHandle)
{
    return(clk_get_rate((struct clk *)clkHandle));
}

/*!
 *  @brief       Function sets the dlock speed
 *
 *  @handle      Ptr to the clockOps object.conaining the clock function.table
 *  @clkHandle   Handle to the clock
 *  @rate        Clock speed to be set
 *
 *  @sa         LinuxClock_put
 */
Int32 LinuxClock_setRate(Ptr clkHandle, ULong rate)
{
    return (clk_set_rate((struct clk *)clkHandle, rate));
}
