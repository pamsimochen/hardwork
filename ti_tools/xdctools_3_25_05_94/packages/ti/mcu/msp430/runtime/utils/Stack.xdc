/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * --/COPYRIGHT--*/
import xdc.runtime.Log;
import xdc.runtime.Diags;

/*!
 *  ======== Stack ========
 *  Stack monitoring functions
 *
 *  This module provides simple stack monitoring operations that enables an
 *  application efficiently monitor the system's worst-case stack usage.
 *
 *  Stack overruns are a common problem in embedded development and they
 *  result in intermittent and difficult to reproduce failures.  The ability
 *  to quickly determine of unexpected behavious is the result of a stack
 *  overrun can save hours of debug time.
 */
@ModuleStartup
module Stack
{
    /*!
     *  ======== Status ========
     *  Stack status information buffer
     */
    struct Status {
        Int unused;     /*! number of words never used (so far) */
        Int used;       /*! worst-case number of words used (so far) */
        Int curDepth;   /*! current number of words on the stack */
    }
    
    /*!
     *  ======== UNUSED ========
     *  Unused stack space event
     */
    config Log.Event UNUSED = {
        mask: Diags.USER1,
        msg: "unused stack space = %d words"
    };

    /*!
     *  ======== check ========
     *  Return non-zero if stack pointer is within allocated stack
     *
     *  If this function returns 0, the current stack pointer is pointing
     *  to a location _outside_ the caller's allocated stack.
     */
    @DirectCall
    Bool check();

    /*!
     *  ======== fill ========
     *  Fill unused stack with initial value
     *
     *  This function is called at startup and may be called at runtime
     *  to re-initialize the stack.  However, interrupts must be disabled
     *  during this process (to prevent corruption of ISR state).
     */
    Void fill();
    
    /*!
     *  ======== getUnused ========
     *  Get number of words of unused stack space
     */
    Int getUnused();

    /*!
     *  ======== getStatus ========
     *  Get number of words of unused stack space
     */
    Void getStatus(Status *stat);
}
