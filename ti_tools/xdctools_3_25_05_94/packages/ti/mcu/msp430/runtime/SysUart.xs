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
 *  ======== SysUart.xs ========
 */

/*
 *  ======== module$static$init ========
 */
function module$static$init(obj, params)
{
    /* initialize module's internal state */
    obj.lineIdx = 0;
    if (params.lineSize > 0) {
        params.lineBuf.length = params.lineSize + 1;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    xdc.useModule("xdc.runtime.Startup");
    xdc.useModule("ti.mcu.msp430.runtime.utils.Led");
    
    /* need to initialize the UART's Rx interrupt vector */
    Program.sectMap[".usci_rx"] = {loadSegment: "INT07"};
    Program.sectMap[".reset"] = {loadSegment: "RESET"};
}
