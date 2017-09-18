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
 *  ======== OMAP5430.xdc ========
 *
 */

metaonly module OMAP5430 inherits ti.catalog.ICpuDataSheet
{
instance:
    config ti.catalog.peripherals.hdvicp2.HDVICP2.Instance hdvicp0;

    override config string cpuCore           = "v7A";
    override config string isa               = "v7A";
    override config string cpuCoreRevision   = "1.0";
    override config int    minProgUnitSize   = 1;
    override config int    minDataUnitSize   = 1;
    override config int    dataWordSize      = 4;

    /*!
     *  ======== memMap ========
     *  The memory map returned be getMemoryMap().
     */
    config xdc.platform.IPlatform.Memory memMap[string]  = [
        ["SRAM", {
            comment:    "On-Chip SRAM",
            name:       "SRAM",
            base:       0x402F0000,
            len:        0x00008000,
            space:      "code/data",
            access:     "RWX"
        }],
    ];
}
/*
 *  @(#) ti.catalog.arm.cortexa15; 1, 0, 0,55; 12-20-2013 12:45:42; /db/ztree/library/trees/platform/platform-o37x/src/
 */

