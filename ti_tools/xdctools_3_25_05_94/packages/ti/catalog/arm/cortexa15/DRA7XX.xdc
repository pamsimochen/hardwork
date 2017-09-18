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
 *  ======== DRA7XX.xdc ========
 *
 */

metaonly module DRA7XX inherits ti.catalog.ICpuDataSheet
{
instance:
    config ti.catalog.peripherals.hdvicp2.HDVICP2.Instance hdvicp0;

    override config string cpuCore           = "v7A15";
    override config string isa               = "v7A15";
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
            len:        0x00010000,
            space:      "code/data",
            access:     "RWX"
            }],

        /* 
             * On-chip RAM memory 
             */
            ["OCMC_RAM1", {
                name: "OCMC_RAM1",
                base: 0x40300000, 
            len:  0x00080000
        }],

        /* 
             * On-chip RAM memory 
             */
            ["OCMC_RAM2", {
                name: "OCMC_RAM2",
                base: 0x40400000, 
            len:  0x00100000
        }],

        /* 
             * On-chip RAM memory 
             */
            ["OCMC_RAM3", {
                name: "OCMC_RAM3",
                base: 0x40500000, 
            len:  0x00100000
        }]
    ];
}
/*
 *  @(#) ti.catalog.arm.cortexa15; 1, 0, 0,55; 12-20-2013 12:45:42; /db/ztree/library/trees/platform/platform-o37x/src/
 */

