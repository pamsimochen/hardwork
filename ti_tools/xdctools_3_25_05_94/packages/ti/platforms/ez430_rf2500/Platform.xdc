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
 *  ======== Platform.xdc ========
 */

/*!
 *  ======== Platform ========
 *  Platform support for the eZ430-RF2500 target board
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
        boardName:      "ez430_rf2500",
        boardFamily:    "ez430",
        boardRevision:  null
    };

    /*!
     *  ======== CPU ========
     *  The CPU provided by this platform.
     */
    readonly config xdc.platform.IExeContext.Cpu CPU = {        
        id:             "0",
        clockRate:      1.0,
        catalogName:    "ti.catalog.msp430.f22xx",
        deviceName:     "MSP430F2274",
        revision:       "",
    };

instance:
    /*!
     *  ======== dssPath ========
     *  Path to the Debug Script Server (DSS) product
     *
     *  This platform uses the DSS support delivered as part of Code
     *  Composer Studio or as a standalone product.  `dssPath` must be set
     *  to an absolute path to the installation directory of DSS.
     *
     *  For example, if you've installed Code Composer Essentials 3.1 in
     *  the default installation directory, `dssPath` should be set to
     *  `"C:/Program Files/Texas Instruments/CC Essentials v3.1/DebugServer"`.
     */
    config String dssPath;
    
    override config String codeMemory = "FLASH";
    
    override config String dataMemory = "RAM";

    override config String stackMemory = "RAM";
};
