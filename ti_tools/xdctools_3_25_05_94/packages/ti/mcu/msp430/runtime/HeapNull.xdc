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
 *  ======== HeapNull.xdc ========
 */

import xdc.runtime.Error;

/*!
 *  ======== HeapNull ========
 *  Empty heap
 */
module HeapNull inherits xdc.runtime.IHeap {

instance:

    /*!
     *  ======== create ========
     *  Create a `HeapNull` heap
     *
     *  This heap is an always empty heap that is intended to be used by
     *  systems that never allocate objects or free memory but need a heap
     *  to satisfy linkage requirements of third-party libraries.
     */
    create();
    
    /*!
     *  ======== alloc ========
     *  Allocate a block of memory from the heap.
     *
     *  `HeapNull_alloc()` always fails.  
     */
    override Ptr alloc(SizeT size, SizeT align, Error.Block *eb);    

    /*!
     *  ======== free ========
     *  Free a block of memory back to the heap.
     *
     *  Calling the `HeapNull_free` function has no effect.
     */
    override Void free(Ptr block, SizeT size);
    
    /*!
     *  ======== isBlocking ========
     *  Can this heap block the caller
     *
     *  @a(returns)
     *  `HeapNull` always returns `FALSE` since it never blocks on a
     *  resource.
     */
    override Bool isBlocking();    

internal:
    /*!
     *  ======== Instance_State ========
     */
    struct Instance_State { /* nothing needed for this implementation */
    };
}
