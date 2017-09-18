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
 *  ======== Platform.xs ========
 *  Platform support for evmLM3S8962
 *
 */

var Boot = null;
var Utils = xdc.module("xdc.platform.Utils");

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    if (xdc.om.$name != "cfg") {
        return;                                                
    }                                                                           
    Boot = xdc.useModule('ti.catalog.arm.cortexm3.lm3init.Boot');

    Boot.configureClock = true;
    Boot.sysClockDiv = Boot.SYSDIV_4;   /* 50 MHz */
    Boot.pwmClockDiv = Boot.PWMDIV_8;   /* div by 8 */
    Boot.xtal = Boot.XTAL_8MHZ ;        /* XTAL = 8 MHz */
    Boot.oscSrc = Boot.OSCSRC_MAIN;     /* use main oscillator */
    Boot.pllBypass = false;             /* use PLL to drive sys clock */
    Boot.pllOutEnable = false;          /* no PLL output */
    Boot.ioscDisable = false;           /* IOSC is enabled */
    Boot.moscDisable = false;           /* MOSC is enabled */

    Boot.configureLdo = true;
    Boot.ldoOut = Boot.LDOPCTL_2_75V;   /* LDO = 2.75V */
}

/*
 *  ======== Platform.getExeContext ========
 *  This Platform's implementation xdc.platform.IPlatform.getExeContext.
 *
 *  Return the xdc.platform.IPlatform.ExeContext object that will run the 
 *  specified program prog.
 */
function getExeContext(prog)
{
    var ExeContext = xdc.useModule('xdc.platform.ExeContext');

    /* create a default ExeContext execution context */
    var cpu;
    var core;
    var armChain = ["v7M"];
    var dspChain = ["62", "64", "64P", "674"];
    for (var i = 0; i < dspChain.length; i++) {
        if (dspChain[i] == prog.build.target.isa) {
            xdc.loadPackage(this.$module.GEM.catalogName);
            cpu = ExeContext.create(this.$module.GEM, this.$module.BOARD);
            core = "GEM";
        }
    }
    for (var i = 0; i < armChain.length; i++) {
        if (armChain[i] == prog.build.target.isa) {
            xdc.loadPackage(this.$module.M3.catalogName);
            cpu = ExeContext.create(this.$module.M3, this.$module.BOARD);
            core = "M3";
        }
    }
    if (cpu == undefined) {
        this.$module.$logError("The build target " + prog.build.target.$name +
            " is incompatible with this platform.", this.$module, null);
    }
    
    /* Set the initial memory map from the cpu datasheet. */
    cpu.memoryMap = Utils.assembleMemoryMap(cpu, this);

    // check for the overlap in the memory map
    var overlap = Utils.checkOverlap(cpu.memoryMap);

    if (overlap != null) {
        this.$module.$logError("Memory objects " + overlap + " overlap", this,
            overlap);
    }

    this.$seal();

    return (cpu);
}

/*
 *  ======== Platform.getExecCmd ========
 *  This Platform's implementation xdc.platform.IPlatform.getExecCmd.
 */
function getExecCmd(prog)
{
    var os = environment["xdc.hostOS"];
    var updateComment = "@$(ECHO) Check for updates to this package at:\n" +
    "@$(ECHO) https://www-a.ti.com/downloads/sds_support/targetcontent/rtsc/index.html";

     return("@$(ECHO) " + this.$package.$name + " platform package cannot " +
         "execute " + prog.name + " on " + os + "\n" + updateComment + "\n\t:");
}

/*
 *  ======== Platform.getLinkTemplate ========
 *  This is Platform's implementation xdc.platform.IPlatform.getLinkTemplate
 */
function getLinkTemplate(prog)
{
    /* use the target's linker command template */
    /* we compute the target because this same platform is supported by
     * many different tool chains; e.g., ti.targets, gnu.targets, ...
     */
    var tname = prog.build.target.$name;
    var tpkg = tname.substring(0, tname.lastIndexOf('.'));
    var templateName = tpkg.replace(/\./g, "/") + "/linkcmd.xdt"; 

    if (xdc.findFile(templateName) != null) {
        return (templateName);
    }
    else if (tname.indexOf("ti.targets.") == 0) {
        return ("ti/targets/linkcmd.xdt");
    }
    else {
        throw new Packages.xdc.services.global.XDCException(
            this.$package.$name + ".LINK_TEMPLATE_ERROR",
            "Target package " + tpkg + " does not contain linker command "
            + "template 'linkcmd.xdt'.");
    }
}

/*
 *  ======== Platform.Instance.init ========
 *  This function is called to initialize a newly created instance of a
 *  platform.  Platform instances are created just prior to running
 *  program configuration scripts.
 *
 *  @param(name)        the name used to identify this instance (without
 *                      the package name prefix).
 */
function instance$meta$init(name)
{
}
/*
 *  @(#) ti.platforms.evmLM3S8962; 1, 0, 0,179; 12-20-2013 12:46:28; /db/ztree/library/trees/platform/platform-o37x/src/
 */

