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
 *  ======== EUSCI_A1.xs ========
 */

/*
 *  ======== instance$meta$init ========
 */
function instance$meta$init(clock)
{
    this.clock = clock;
    
    this.interruptSource[0].registerName = "UCTXIE";
    this.interruptSource[0].registerDescription = "eUSCI_A1 SPI/UART Transmit Interrupt Enable";
    this.interruptSource[0].interruptEnable = false;
    this.interruptSource[0].interruptHandler = false;
    
    this.interruptSource[1].registerName = "UCRXIE";
    this.interruptSource[1].registerDescription = "eUSCI_A1 SPI/UART Receive Interrupt Enable";
    this.interruptSource[1].interruptEnable = false;
    this.interruptSource[1].interruptHandler = false;
}

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
}
