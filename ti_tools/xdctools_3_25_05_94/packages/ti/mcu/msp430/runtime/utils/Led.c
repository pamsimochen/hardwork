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
 *  ======== Led.c ========
 */
#include <xdc/std.h>
#include <xdc/runtime/Startup.h>

#include "package/internal/Led.xdc.h"
#include "HWI.h"

/*
 *  ======== Led_Module_startup ========
 */
Int Led_Module_startup(Int state)
{
    *(Bits8 *)Led_DIRCFG |= Led_RED | Led_GREEN;
    return (Startup_DONE);
}

/*
 *  ======== Led_on ========
 */
Void Led_on(Bits8 mask)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *(Bits8 *)Led_PORT |= mask;

    HWI_set_interrupt_state(sr);
}

/*
 *  ======== Led_off ========
 */
Void Led_off(Bits8 mask)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *(Bits8 *)Led_PORT &= ~mask;

    HWI_set_interrupt_state(sr);
}

/*
 *  ======== Led_toggle ========
 */
Void Led_toggle(Bits8 mask)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *(Bits8 *)Led_PORT ^= mask;

    HWI_set_interrupt_state(sr);
}
