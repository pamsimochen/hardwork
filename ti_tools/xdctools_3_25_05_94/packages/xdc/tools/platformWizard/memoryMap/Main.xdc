/*
 *  Copyright 2013 by Texas Instruments Incorporated.
 *
 */

package xdc.tools.platformWizard.memoryMap;

/*
 * @_nodoc
 */
metaonly module Main inherits xdc.tools.ICmd {

    override config String usage[] = [
        ' -m <Module Name>',
        ' -r <Register Settings>'
    ];

instance:
        
        @CommandOption("m")
        config String deviceModuleName = null;

        @CommandOption("r")
        config String registerSettings = null;
        
    override Any run(xdc.tools.Cmdr.Instance cmdr, String args[]);

}
/*
 *  @(#) xdc.tools.platformWizard.memoryMap; 1,0,0,145; 12-7-2013 14:16:33; /db/ztree/library/trees/xdctools/xdctools-f41x/src/
 */

