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

package ti.platforms.ts430F5529;

/*!
 *  ======== Platform ========
 *  Platform support for the ts430F5529
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
	id:		"0",
	boardName:	"ts430F5529",
	boardFamily:	"ts430F5529",
	boardRevision:	null
    };

    /*!
     *  ======== CPU ========
     *  The CPU supported by this platform.
     */
    readonly config xdc.platform.IExeContext.Cpu CPU = {	
	id:		"0",
	clockRate:	8.0,
	catalogName:	"ti.catalog.msp430.f55xx",
	deviceName:	"MSP430F5529",
	revision:	"",
    };

instance:

    /*!
     *  ======== deviceName ========
     *  The CPU supported by this platform.
     *
     *  This parameter is optional. If it's not set, then the Platform module
     *  parameter CPU.deviceName is used.
     */
    config string deviceName;

    override config string codeMemory = "FLASH";
    
    override config string dataMemory = "RAM";

    override config string stackMemory = "RAM";
};
