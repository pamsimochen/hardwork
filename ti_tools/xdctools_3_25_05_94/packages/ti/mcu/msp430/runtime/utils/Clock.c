/* --COPYRIGHT--,ESD
 *  Copyright (c) 2008 Texas Instruments. All rights reserved. 
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
 *  ======== Clock.c ========
 */
#include <xdc/runtime/Startup.h>

#include "package/internal/Clock.xdc.h"
#include "HWI.h"

#define MC_1        0x10u       /* Timer up mode  */
#define MC_2        0x20u       /* Timer contineous mode  */
#define TBSSEL_1    0x0100u     /* Clock Source (MCLK, ACLK) */
#define CCIFG       0x0001      /* Capture/compare interrupt flag */

#define CAP         0x0100       /* Comparator Capture mode */

/*
 *  ======== Clock_Module_startup ========
 */
Int Clock_Module_startup(Int State)
{
    Clock_TIMER->CCTL[0] |= CAP;        /* put comparator 0 in cap mode */
    Clock_TIMER->CCR[0] = 0xffff;       /* maximum period */

    Clock_TIMER->R = 0;                 /* initial count */
    Clock_TIMER->CTL = TBSSEL_1 + MC_2; /* ACLK, continuous mode */

    return (Startup_DONE);
}

/*
 *  ======== Clock_getTime ========
 */
Clock_TimeValue Clock_getTime()
{
    return (Clock_TIMER->R);
}

/*
 *  ======== Clock_delay ========
 */
Void Clock_delay(UInt16 usec)
{
    Clock_sleep(usec, 0);
}

/*
 *  ======== Clock_sleep ========
 */
Void Clock_sleep(UInt16 usec, UInt lpm)
{
    unsigned short sr;
    
    /* compute the delay increment in clock ticks */
    unsigned int inc = (Clock_slowClockHz * usec + 500000L) / 1000000;
    if (inc == 0) {
        return;
    }
    
    /* atomically clear interrupt flag and set new compare value */
    sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();
    Clock_TIMER->CCTL[0] &= ~CCIFG;             /* clear the interrupt flag */
    Clock_TIMER->CCR[0] = Clock_TIMER->R + inc; /* set new compare value */
    HWI_set_interrupt_state(sr);

    /* loop until interrupt flag is set */
    while(!(Clock_TIMER->CCTL[0] & CCIFG)) {
       HWI_bis_SR_register(lpm);
    }
}

/*
 *  ======== Clock_wakeup ========
 */
Void Clock_wakeup()
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    Clock_TIMER->CCTL[0] |= CCIFG;

    HWI_set_interrupt_state(sr);
}
