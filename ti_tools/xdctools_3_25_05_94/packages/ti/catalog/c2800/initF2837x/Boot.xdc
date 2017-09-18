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
 *  ======== Boot.xdc ========
 *
 */

package ti.catalog.c2800.initF2837x;

import xdc.rov.ViewInfo;

/*!
 *  ======== Boot ========
 *  Soprano Boot Support.
 *
 *  The Boot module supports boot initialization for the C28 Soprano cores.
 *  A special boot init function is created based on the configuration
 *  settings for this module.  This function is hooked into the
 *  xdc.runtime.Reset.fxns[] array and called very early at boot time (prior
 *  to cinit processing).
 * 
 *  The code to support the boot module is placed in a separate section
 *  named `".text:.bootCodeSection"` to allow placement of this section in
 *  the linker .cmd file if necessary. This section is a subsection of the
 *  `".text"` section so this code will be placed into the .text section unless
 *  explicitly placed, either through
 *  `{@link xdc.cfg.Program#sectMap Program.sectMap}` or through a linker
 *  command file.
 */
@Template("./Boot.xdt")
@NoRuntime
module Boot
{
    /*! System PLL Fractional Multiplier (SPLLFMULT) value */
    metaonly enum FractMult {
        Fract_0  = 0x000,       /*! Fractional multiplier is 0    */
        Fract_25 = 0x100,       /*! Fractional multiplier is 0.25 */
        Fract_50 = 0x200,       /*! Fractional multiplier is 0.5  */
        Fract_75 = 0x300        /*! Fractional multiplier is 0.75 */
    }

    metaonly struct ModuleView {
        Bool configureClocks;
        UInt OSCCLK;
        UInt SPLLIMULT;
        String SPLLFMULT;
        String SYSCLKDIVSEL;
        Bool bootCPU2;
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
     *  Clock configuration flag, default is false.
     *
     *  Set to true to configure the PLL and system and M3 subsystem clock 
     *  dividers.
     */
    config Bool configureClocks = false;

    /*!
     *  Watchdog disable flag, default is false.
     *
     *  Set to true to disable the watchdog timer.
     */
    metaonly config Bool disableWatchdog = false;

    /*!
     *  OSCCLK input frequency to PLL, in MHz. Default is 10 MHz.
     *
     *  This is the frequency of the oscillator clock (OSCCLK) input to the
     *  PLL.
     */
    metaonly config UInt OSCCLK = 20;

    /*! System PLL Integer Multiplier (SPLLIMULT) value */
    metaonly config UInt SPLLIMULT = 1;

    /*! System PLL Fractional Multiplier (SPLLFMULT) value */
    metaonly config FractMult SPLLFMULT = Fract_0;

    /*! System Clock Divider Select (SYSCLKDIVSEL) value */
    metaonly config UInt SYSCLKDIVSEL = 2;

    /*! 
     *  Flash controller wait states configuration flag, default is true.
     *
     *  Set to true to configure the Flash controller wait states.  The number
     *  of wait states is computed based upon the CPU frequency.
     */
    metaonly config Bool configureFlashWaitStates = true;

    /*! 
     *  Flash controller program cache enable flag, default is true.
     *
     *  Set to true to enable the Flash controller's program cache.
     */
    metaonly config Bool enableFlashProgramCache = true;

    /*! 
     *  Flash controller data cache enable flag, default is true.
     *
     *  Set to true to enable the Flash controller's data cache.
     */
    metaonly config Bool enableFlashDataCache = true;

    /*!
     *  Function to be called when Limp mode is detected.
     *
     *  This function is called when the Boot module is about to configure
     *  the PLL, but finds the device operating in Limp mode (i.e., the mode
     *  when a missing OSCCLK input has been detected).
     *
     *  If this function is not specified by the application, a default
     *  function will be used, which spins in an infinite loop.
     */
    metaonly config Fxn limpAbortFunction;

    /*!
     *  Boot from Flash flag.  Default is true. 
     *
     *  Set to true to enable booting CPU1 from Flash. 
     */
    metaonly config Bool bootFromFlash = true;

    /*!
     *  Initiate booting of the CPU2 processor.  Default is false. 
     *
     *  Set to true to enable CPU1 to initiate boot of CPU2.  
     *
     *  If enabled, this will occur after the optional clock configuration 
     *  step, enabled by `{@link #configureClocks}`.
     */
    metaonly config Bool bootCPU2 = false;

    /*!
     *  Configure Shared RAM regions before booting the C28 processor.  
     *  Default is true.
     *
     *  Set to true to enable Shared RAM regions S0-S7, to set the
     *  owner of each region and the write access permissions for the onwer.
     */
    metaonly config Bool configSharedRAMs = true;

   /*!
     *  ======== sharedMemoryOwnerMask ========
     *  Shared RAM owner select mask.
     *
     *  This parameter is used for writing the GSxMSEL register.
     *  By default, each value of each shared RAM select bit is '0'.
     *  This means the CPU1 is the owner and has write access.
     *  Setting a '1' in any bit position makes CPU2 the owner of that
     *  shared RAM segment.
     */
    metaonly config Bits32 sharedMemoryOwnerMask = 0;

    /*!
     *  @_nodoc
     *  ======== getFrequency ========
     *  Gets the resulting M3 CPU frequency (in Hz) given the Clock 
     *  configuration parameters. 
     *
     */
    UInt32 getFrequency();
    
    /*!
     *  @_nodoc    
     *  ======== registerFreqListener ========
     *  Register a module to be notified whenever the frequency changes.
     *
     *  The registered module must have a function named 'fireFrequencyUpdate'
     *  which takes the new frequency as an argument.
     */
    function registerFreqListener();

internal:
    
    /* The computed timestamp frequency */
    metaonly config UInt timestampFreq;

    /* Used to display the computed M3 frequency value in the Grace page. */
    metaonly config String displayFrequency;

    /* The computed Flash wait states */
    metaonly config UInt flashWaitStates = 3;

};
/*
 *  @(#) ti.catalog.c2800.initF2837x; 1, 0, 0,3; 12-20-2013 12:45:50; /db/ztree/library/trees/platform/platform-o37x/src/
 */

