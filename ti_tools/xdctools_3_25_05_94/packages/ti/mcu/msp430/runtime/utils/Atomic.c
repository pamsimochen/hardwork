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
 *  ======== Atomic.c ========
 */

#include "package/internal/Atomic.xdc.h"
#include "HWI.h"

/*
 *  ======== Atomic_dec8 ========
 */
Void Atomic_dec8(UInt8 *val)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *val = *val - 1;

    HWI_set_interrupt_state(sr);
}

/*
 *  ======== Atomic_inc8 ========
 */
Void Atomic_inc8(UInt8 *val)
{
    unsigned short sr = HWI_get_interrupt_state();
    HWI_disable_interrupts();

    *val = *val + 1;

    HWI_set_interrupt_state(sr);
}

