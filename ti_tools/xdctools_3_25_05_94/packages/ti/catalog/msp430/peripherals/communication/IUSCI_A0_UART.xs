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

/*
 *  ======== IUSCI_A0_UART.xs ========
 */

/*
 *  ======== module$meta$init ========
 */
function module$meta$init() 
{
}

/* Sets UCxxBR0 register value based on which module */
function setUCxxBR0(value) {
    this.UCA0BR0 = value;
}

/* Returns UCxxBR0 register value based on which module */
function getUCxxBR0() {
    return this.UCA0BR0;
}

/* Sets UCxxBR1 register value based on which module */
function setUCxxBR1(value) {
    this.UCA0BR1 = value;
}

/* Returns UCxxBR1 register value based on which module */
function getUCxxBR1() {
    return this.UCA0BR1;
}
