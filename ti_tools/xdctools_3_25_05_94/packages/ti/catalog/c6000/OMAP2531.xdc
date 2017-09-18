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
 *  ======== OMAP2531.xdc ========
 *
 */
package ti.catalog.c6000;

/*!
 *  ======== OMAP2531 ========
 *  OMAP2531 device data sheet module
 *
 *  This module implements the xdc.platform.ICpuDataSheet interface and is 
 *  used by platforms to obtain "data sheet" information about this device.
 */
metaonly module OMAP2531 inherits IOMAP2x3x
{

instance:

    override config string   cpuCoreRevision = "1.0";

};
/*
 *  @(#) ti.catalog.c6000; 1, 0, 0, 0,444; 12-20-2013 12:45:52; /db/ztree/library/trees/platform/platform-o37x/src/
 */

