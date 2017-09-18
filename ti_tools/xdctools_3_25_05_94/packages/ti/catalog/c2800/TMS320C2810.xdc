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
 *  ======== TMS320C2810.xdc ========
 *
 */
package ti.catalog.c2800;

/*!
 *  ======== TMS320C2810 ========
 *  The C2810 device data sheet module.
 *
 *  This module implements the xdc.platform.ICpuDataSheet interface and is used
 *  by platforms to obtain "data sheet" information about this device.
 */
metaonly module TMS320C2810 inherits ITMS320C28xx
{
instance:
    override config string   cpuCoreRevision = "1.0";

    /*!
     *  ======== memMap ========
     *  The default memory map for this device
     */
   
    config xdc.platform.IPlatform.Memory memMap[string]  = TMS320C2812.memMap;
};
/*
 *  @(#) ti.catalog.c2800; 1, 0, 0, 0,446; 12-20-2013 12:45:49; /db/ztree/library/trees/platform/platform-o37x/src/
 */

