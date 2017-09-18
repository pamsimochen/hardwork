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
 *  ======== Stack.c ========
 */
#include <xdc/runtime/Startup.h>

#include "package/internal/Stack.xdc.h"

/*
 *  The following symbols are defined by the linker and delimit the stack.
 *  For example:
 *      &_stack       = 0x508,
 *      &__STACK_SIZE = 0xf0
 *      &__STACK_END  = 0x5f8,
 *  where the .stack section starts at 0x508 and has length 0xf0.
 *
 */
extern Int _stack;    
extern Int __STACK_END;
extern Int __STACK_SIZE;

/*
 *  ======== FILL ========
 *  The distinguished fill value for the stack
 */
#define FILL 0xCAFE

/*
 *  ======== Stack_Module_startup ========
 *  Called early in the startup (before main)
 */
Int Stack_Module_startup(Int state)
{
    Stack_fill();
    return (Startup_DONE);
}

/*
 *  ======== Stack_check ========
 */
Bool Stack_check(void)
{
    Int sp;
    return (&sp > &_stack);
}
/*
 *  ======== Stack_fill ========
 */
Void Stack_fill(void)
{
    int *sp;

    /* fill stack down to the address of our automatic variable sp
     * with an initial value (FILL).  We subtract 4 as a precaution to
     * avoid corrupting a portion of stack that a compiler might use.
     */
    for (sp = &_stack; sp < ((int *)&sp - 4); sp++) {
        *sp = FILL;
    }
}

/*
 *  ======== Stack_getUnused ========
 */
Int Stack_getUnused(Void)
{
    Int *sp;

    /* for many CPUs (including MSP430) the stack grows to lower addresses.
     * check from the top down until the initial value (FILL) has been
     * overwritten.
     */
    for (sp = &_stack; sp < &__STACK_END; sp++) {
        if (*sp != FILL) {
            break;
        }
    }

    return (sp - &_stack);
}

/*
 *  ======== Stack_getStatus ========
 */
Void Stack_getStatus(Stack_Status *stat)
{
    volatile Int i;
    
    stat->unused = Stack_getUnused();
    stat->used = ((IArg)&__STACK_SIZE / sizeof (IArg)) - stat->unused;
    stat->curDepth = &i - &_stack;
}
