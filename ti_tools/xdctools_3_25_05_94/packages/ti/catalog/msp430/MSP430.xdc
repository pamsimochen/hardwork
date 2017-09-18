/* --COPYRIGHT--,EPL
 *  Copyright (c) 2010 Texas Instruments and others.
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
 *  ======== MSP430.xdc ========
 *  Generic MSP430 Cpu definition
 *
 *  This device represents all MSP430 devices independent from the ISA. The
 *  parameter `{@link #isa}` is set to a JavaScript regular expression that
 *  covers all MSP430 ISAs.
 */

metaonly module MSP430 inherits IMSP430
{
instance:
    override config string   cpuCore    = "MSP430.*";
    override config string   isa        = "430.*";
    override config Float    vcc        = 1.8;
}
