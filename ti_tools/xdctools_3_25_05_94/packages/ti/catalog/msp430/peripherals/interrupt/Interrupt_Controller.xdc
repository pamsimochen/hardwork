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
 *  ======== Interrupt_Controller ========
 *  MSP430 Interrupt_Controller
 */
metaonly module Interrupt_Controller inherits xdc.platform.IPeripheral {

instance:

    config UInt baseAddr;
    config UInt numInterrupts;
}
