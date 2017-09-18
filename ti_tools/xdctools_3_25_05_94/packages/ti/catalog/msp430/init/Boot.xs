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
 *  ======== Boot.xs ========
 *
 *! Revision History
 *! ================
 *! 01-Dec-2009 sasha   using xdc.runtime.Reset to add a reset Fxn
 *! 20-Oct-2009 sg      Initial.
 */

var Boot = null;
var Program = null;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    /* Only process during "cfg" phase */
    if (xdc.om.$name != "cfg") {
        return;
    }
    Boot = this;
}

/*
 *  ======== module$use ========
 */
function module$use()
{
    Program = xdc.useModule('xdc.cfg.Program');

    /* set configureDCO to false for all non-UCS clock based devices */
    var device = Program.cpu.attrs.peripherals["clock"];
    if (device == null || 
        (device.$module.$name != "ti.catalog.msp430.peripherals.clock.UCS" &&
         device.$module.$name != "ti.catalog.msp430.peripherals.clock.CS_A")) {
        Boot.configureDCO = false;
    }

    if (Program.build.rtsName !== null
       && (Boot.disableWatchdog || Boot.configureDCO) == true) {

        /* Install Boot_init as one of Reset functions */
        var Reset = xdc.useModule('xdc.runtime.Reset');
        Reset.fxns[Reset.fxns.length++] = '&ti_catalog_msp430_init_Boot_init';
    }
}

/*
 *  ======== viewInitModule ========
 *  Display the module properties in ROV
 */
function viewInitModule(view, obj)
{
    var Program = xdc.useModule('xdc.rov.Program');
    var Boot = xdc.useModule('ti.catalog.msp430.init.Boot');
    var bootModConfig = Program.getModuleConfig(Boot.$name);

    view.disableWatchdog = bootModConfig.disableWatchdog;
}
