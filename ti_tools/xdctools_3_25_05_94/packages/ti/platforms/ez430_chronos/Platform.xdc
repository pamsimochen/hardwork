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

package ti.platforms.ez430_chronos;

/*!
 *  ======== Platform ========
 *  Platform support for the ez430_chronos
 *
 *  This module implements xdc.platform.IPlatform and defines configuration
 *  parameters that correspond to this platform's Cpu's, Board's, etc.
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
	boardName:	"ez430_chronos",
	boardFamily:	"ez430",
	boardRevision:	null
    };

    /*!
     *  ======== CPU ========
     *  The CPU supported by this platform.
     *
     *  See {@link http://www.ti.com/lit/gpn/CC430F6137} and
     *  {@link http://www.ti.com/lit/pdf/slau259} for complete details.
     */
    readonly config xdc.platform.IExeContext.Cpu CPU = {	
	id:		"0",
	clockRate:	8.0,
	catalogName:	"ti.catalog.msp430.f55xx",
	deviceName:	"CC430F6137",
	revision:	"",
    };

instance:

    config String deviceName;

    /*!
     *  ======== genFullCmdFile ========
     *  Enable generation of full linker command file
     *
     *  If set to `true`, the platform will generate the complete linker
     *  command file rather than require the user to supply one on the
     *  link line.
     *
     *  If an instance of this platform is created with the name
     *  "`genFullCmdFile`", this `genFullCmdFile` will be set to `true`.
     *  This enables one to easily create a platform instance for which this
     *  parameter is `true`.
     */
    config Bool genFullCmdFile = true;

    override config String codeMemory = "FLASH";
    
    override config String dataMemory = "RAM";

    override config String stackMemory = "RAM";
};
