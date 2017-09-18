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

import xdc.runtime.IGateProvider;

/*! 
 *  ======== GateGIE ========
 *  Global interrupt disable/enable implementation of `{@link IGateProvider}`
 *
 *  Instances of this gate implementation disable all interrupts and should only
 *  be used for modules in which the data structures protected by module-level
 *  gates are held for fixed short durations.
 */
module GateGIE inherits IGateProvider {

instance:
    /*!
     *  ======== create ========
     *  Create a `GateGIE` gate
     */
    create();

internal:
    /*!
     *  ======== Instance_State ========
     */
    struct Instance_State { /* nothing needed for this implementation */
    };
}
