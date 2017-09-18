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

requires xdc.platform;

/*!
 *  ======== ti.catalog.arm.cortexr4  ========
 *  Package of devices for the arm Cortex R4 family of Arm processors.
 *
 *  Each module in this package implements the xdc.platform.ICpuDataSheet
 *  interface. This interface is used by platforms (modules that implement
 *  xdc.platform.IPlatform) to obtain the memory map supported by each CPU.
 */
package ti.catalog.arm.cortexr4 [1,0,0] {
    module TMS570PSF762, TMS570LS20216;
}
/*
 *  @(#) ti.catalog.arm.cortexr4; 1, 0, 0,200; 12-20-2013 12:45:47; /db/ztree/library/trees/platform/platform-o37x/src/
 */

