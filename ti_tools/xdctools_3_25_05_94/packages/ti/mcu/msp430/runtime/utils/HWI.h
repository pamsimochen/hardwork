/* --COPYRIGHT--,ESD
 *  Copyright (c) 2012 Texas Instruments. All rights reserved.
 *  This program and the accompanying materials are made available under the
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * --/COPYRIGHT--*/
/*
 *  ======== HWI.h ========
 */
#ifndef ti_mcu_msp430_runtime_utils_HWI__include
#define ti_mcu_msp430_runtime_utils_HWI__include

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__MSP430__)
    /* ======== TI compiler ======== */
    #include <intrinsics.h>
    
    #define HWI_bis_SR_register _bis_SR_register
    #define HWI_disable_interrupts _disable_interrupts
    #define HWI_get_interrupt_state _get_interrupt_state
    #define HWI_set_interrupt_state _set_interrupt_state     

#elif defined(__ICC430__)
    /* ======== IAR compiler ======== */
    #include <intrinsics.h>

    #define HWI_bis_SR_register __bis_SR_register
    #define HWI_disable_interrupts __disable_interrupt
    #define HWI_get_interrupt_state __get_interrupt_state
    #define HWI_set_interrupt_state __set_interrupt_state     

#endif 

#ifdef __cplusplus
}
#endif

#endif
