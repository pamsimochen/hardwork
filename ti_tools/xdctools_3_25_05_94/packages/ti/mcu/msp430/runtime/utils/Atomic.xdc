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
/*!
 *  ======== Atomic ========
 *  Simple atomic operations on integers
 *
 *  This module provides simple "atomic" operations on integral variables.
 *  These operations reliably read, modify, and store a specified value
 *  even in the face of preempting threads that modify the same variables.
 *
 *  Atomic operations are used as a simple counting semaphore between an
 *  appliation and an interrupt service ruotine, for example.
 */
module Atomic
{
    /*!
     *  ======== inc8 ========
     *  Atomically increment val by 1
     */
    @DirectCall
    Void inc8(UInt8 *val);

    /*!
     *  ======== dec8 ========
     *  Atomically decrement val by 1
     */
    @DirectCall
    Void dec8(UInt8 *val);
}
