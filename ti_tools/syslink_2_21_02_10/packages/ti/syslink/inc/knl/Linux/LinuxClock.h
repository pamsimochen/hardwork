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
 *  @file   ti/syslink/inc/knl/Linux/LinuxClock.h
 */

#ifndef LinuxClock_H_0xbbec
#define LinuxClock_H_0xbbec

#if defined (__cplusplus)
extern "C" {
#endif

#include <ti/syslink/inc/ClockOps.h>

/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Function to Create a clock handle */
ClockOps_Handle LinuxClock_create(Void);
/* Function deletes the clock object */
Int LinuxClock_delete (ClockOps_Handle handle);
/* Function to get handle to a clock given its name */
Ptr LinuxClock_get(String clkname);
/* Function to release a Clock. */
Void LinuxClock_put(Ptr clkHandle);
/* Function to enable a clock. */
Int32 LinuxClock_enable(Ptr clkHandle);
/* Function to disable a clock */
Void LinuxClock_disable(Ptr clkHandle);
/* Function to get clock speed */
ULong LinuxClock_getRate(Ptr clkHandle);
/* Function to set clock speed */
Int32 LinuxClock_setRate(Ptr clkHandle, ULong rate);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif
