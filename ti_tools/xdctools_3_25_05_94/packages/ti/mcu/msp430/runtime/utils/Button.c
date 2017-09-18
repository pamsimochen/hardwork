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
 *  ======== Button.c ========
 */
#include <xdc/std.h>
#include <xdc/runtime/Startup.h>

#include "package/internal/Button.xdc.h"
#include "HWI.h"
/*
 *  ======== Button_Module_startup ========
 */
Int Button_Module_startup(Int state)
{
    *(Bits8 *)Button_PORT_OUT |= Button_SWITCH1 | Button_SWITCH2;
    *(Bits8 *)Button_PORT_REN |= Button_SWITCH1 | Button_SWITCH2;
    return (Startup_DONE);
}

/*
 *  ======== Button_enable ========
 *  Enable the specified buttons 
 */
Void Button_enable(Bits8 mask)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *(Bits8 *)Button_PORT_IE |= mask;

    HWI_set_interrupt_state(sr);
}

/*
 *  ======== Button_disable ========
 *  Disable the specified buttons 
 */
Void Button_disable(Bits8 mask)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *(Bits8 *)Button_PORT_IE &= ~mask;

    HWI_set_interrupt_state(sr);
}

/*
 *  ======== Button_read ========
 *  Atomically read and clear the specified buttons
 */
Bits8 Button_read(Bits8 mask)
{
    Bits8 old;
    
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    old = *(Bits8 *)Button_PORT_IFG;
    *(Bits8 *)Button_PORT_IFG = old & ~mask;

    HWI_set_interrupt_state(sr);

    return (old & mask);
}
