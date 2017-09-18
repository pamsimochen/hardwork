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
 *  ======== Boot.xdc ========
 */

package ti.catalog.msp430.init;

import xdc.rov.ViewInfo;

/*!
 *  ======== Boot ========
 *  Boot time initialization code manager
 *
 *  The Boot module supports boot initialization for the MSP430 devices.
 *  A special boot init function is created based on the configuration
 *  settings for this module.  This function is hooked into the
 *  `{@link xdc.runtime.Reset#fxns xdc.runtime.Reset.fxns[]}` array and
 *  is called very early at boot time (prior to C runtime initialization).
 * 
 *  The code to support the boot module is placed in a separate section
 *  named `".text:bootCodeSection"` to allow placement of this section in
 *  the linker .cmd file if necessary. This section is a subsection of the
 *  `".text"` section so this code will be placed into the .text section
 *  unless explicitly placed, either through
 *  `{@link xdc.cfg.Program#sectMap Program.sectMap}` or through a linker
 *  command file.
 */
@Template("./Boot.xdt")
metaonly module Boot
{
    metaonly struct ModuleView {
        Bool         disableWatchdog;
        UInt         watchdogAddress;
        Bool         configureDCO;
        Bool         useLFXT;
    }

    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
            [
                'Module',
                {
                    type: ViewInfo.MODULE,
                    viewInitFxn: 'viewInitModule',
                    structName: 'ModuleView'
                }
            ],
            ]
        });
    
    /*! 
     *  ======== disableWatchdog ========
     *  Watchdog disable flag
     *
     *  The default is `true`.  Set to `false` to not automatically disable the
     *  watchdog timer.
     */
    config Bool disableWatchdog = true;

    /*! 
     *  ======== watchdogAddress ========
     *  Watchdog control register address
     *
     *  The default (0x15c) is set for MSP430F54xx and MSP430F552x devices.
     *  Change for other devices.
     */
    config UInt watchdogAddress = 0x15c;

    /*! 
     *  ======== configureDCO ========
     *  Configure DCO flag
     *
     *  The default is `false`.  Set to `true` to initialize the DCO at boot.
     *
     */
    config Bool configureDCO = false;

    /*! 
     *  ======== useLFXT ========
     *  Use a low frequency crystal (LFXT) reference
     *
     *  The default is `false`.  Set to `true` to indicate a 32768Hz low 
     *  frequency crystal is present, and available for configuring the clock
     *  system. 
     *
     *  This configuration parameter is utilized for only a subset of MSP430
     *  device types, and only when `configureDCO` is set to `true`. 
     *
     *  For FR58xx/FR59xx devices: if `useLFXT` is set to `true`, an LFXT
     *  will be used as reference, for generating more accurate clock 
     *  frequencies.  Othewise, and by default, MODOSC will be used as 
     *  reference.
     *
     *  For all other MSP430 device types: at present, the `useLFXT` parameter 
     *  will be ignored, and an internal reference will be used instead.
     */
    config Bool useLFXT = false;
};
