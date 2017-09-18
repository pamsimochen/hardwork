/*
 *  Copyright (c) 2013 by Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * */

/*
 *  ======== TMS570LS20216.getMemoryMap ========
 *  Returns the memory map as seen by a program running on this device
 *  when the registers are set as specified by the regs hash.
 */
function getMemoryMap(regs)
{
    if ("flash_lo" in regs) {
        var flash_lo = regs.flash_lo;
    }
    else {
        var flash_lo = true;
    }
    if (flash_lo == true) {
        delete this.memMap["LO_IRAM"];
        delete this.memMap["HI_FLASH"];
    }
    else {
        delete this.memMap["HI_IRAM"];
        delete this.memMap["LO_FLASH"];
    }
    return (this.memMap);
}

/*
 *  @(#) ti.catalog.arm.cortexr4; 1, 0, 0,200; 12-20-2013 12:45:47; /db/ztree/library/trees/platform/platform-o37x/src/
 */

