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
 *  ======== TMS320DM357.xdc ========
 *
 */

metaonly module TMS320DM357 inherits ti.catalog.ICpuDataSheet
{
instance:
    override config string cpuCore           = "Arm9";
    override config string isa               = "v5T";
    override config string cpuCoreRevision   = "1.0";
    override config int    minProgUnitSize   = 1;
    override config int    minDataUnitSize   = 1;
    override config int    dataWordSize      = 4;

    /*!
     *  ======== memMap ========
     *  The memory map returned be getMemoryMap().
     */
    config xdc.platform.IPlatform.Memory memMap[string] = TMS320DM355.memMap;
}
/*
 *  @(#) ti.catalog.arm; 1, 0, 1,332; 12-20-2013 12:45:43; /db/ztree/library/trees/platform/platform-o37x/src/
 */
