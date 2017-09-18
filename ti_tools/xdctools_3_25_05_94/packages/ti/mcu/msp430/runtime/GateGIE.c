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
 *  ======== GateGIE.c ========
 */
#include  <ti/mcu/msp430/runtime/utils/HWI.h>

#include "package/internal/GateGIE.xdc.h"

/*
 *  ======== enter ========
 */
IArg GateGIE_enter(GateGIE_Object *obj)
{
    IArg key = HWI_get_interrupt_state(); /* get current interrupt state */
    HWI_disable_interrupts();             /* disable interrupts */

    return (key);
}

/*
 *  ======== leave ========
 */
Void GateGIE_leave(GateGIE_Object *obj, IArg key)
{
    /* re-enable interrupts _only_ if we disabled them */
    HWI_set_interrupt_state((unsigned short)key);
}

/*
 *  ======== query ========
 */
Bool GateGIE_query(Int qual)
{
    return (FALSE);
}
