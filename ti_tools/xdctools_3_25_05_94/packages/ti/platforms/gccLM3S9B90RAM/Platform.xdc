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
 *  ======== Platform.xdc ========
 */

package ti.platforms.gccLM3S9B90RAM;

/*!
 *  ======== Platform ========
 *  Platform support with GCC for an LM3S9B90 using RAM only
 *
 *  This module implements xdc.platform.IPlatform and defines configuration
 *  parameters that correspond to this platform's Cpu's, Board's, etc.
 *
 *  The configuration parameters are initialized in this package's
 *  configuration script (package.cfg) and "bound" to the TCOM object
 *  model.  Once they are part of the model, these parameters are
 *  queried by a program's configuration script.
 *
 *  This particular platform has a single Cpu, and therefore, only
 *  declares a single CPU configuration object.  Multi-CPU platforms
 *  would declare multiple Cpu configuration parameters (one per
 *  platform CPU).
 */
metaonly module Platform inherits xdc.platform.IPlatform
{
    /*!
     *  ======== BOARD ========
     *  This platform's board attributes
     */
    readonly config xdc.platform.IPlatform.Board BOARD = {
        id:             "0",
        boardName:      "gccLM3S9B90RAM",
        boardFamily:    "gccLM3S9B90RAM",
        boardRevision:  null
    };

    /*!
     *  ======== CPU ========
     *  The  Cortex M3 core
     */
    readonly config xdc.platform.IExeContext.Cpu CPU = {        
        id:             "0",
        clockRate:      80.0,
        catalogName:    "ti.catalog.arm.cortexm3",
        deviceName:     "LM3S9B90",
        revision:       "",
    };

instance:

};
/*
 *  @(#) ti.platforms.gccLM3S9B90RAM; 1, 0, 1, 1,69; 12-20-2013 12:46:47; /db/ztree/library/trees/platform/platform-o37x/src/
 */

