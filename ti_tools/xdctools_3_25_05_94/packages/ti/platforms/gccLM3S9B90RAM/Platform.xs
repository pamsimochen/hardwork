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
 *
 */

var Utils = xdc.useModule('xdc.platform.Utils');

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    if (xdc.om.$name != 'cfg') {
        return;
    }
}

/*
 *  ======== module$use ========
 */
function module$use()
{
}

/*
 *  ======== Platform.getCpuDataSheet ========
 *  This Platform's implementation xdc.platform.IPlatform.getCpuDataSheet.
 *
 *  Return the xdc.platform.ICpuDataSheet.Instance object that corresponds
 *  to the device that runs executables loaded on the specified cpuId.
 */
function getCpuDataSheet(cpuId)
{
    return (Utils.getCpuDataSheet(this.$private.cpuAttrs));
}

/*
 *  ======== Platform.getExeContext ========
 *  This Platform's implementation xdc.platform.IPlatform.getExeContext.
 *
 *  Return the xdc.platform.IExeContext.Instance object that will run the
 *  specified program prog.
 */
function getExeContext(prog)
{
    /* create a default ExeContext execution context */
    var ExeContext = xdc.useModule('xdc.platform.ExeContext');

    var cpu = ExeContext.create(this.$module.CPU, this.$module.BOARD);

    /* Set the initial memory map from the cpu datasheet. */
    cpu.memoryMap = Utils.assembleMemoryMap(cpu, this);

    if (this.codeMemory == undefined) {
        this.codeMemory = "FRAM";
    }

    if (this.dataMemory == undefined) {
        this.dataMemory = "IRAM";
    }

    if (this.stackMemory == undefined) {
        this.stackMemory = "IRAM";
    }

    // check for the overlap in the memory map
    var overlap = Utils.checkOverlap(cpu.memoryMap);
    
    if (overlap != null) {
        this.$module.$logError("Memory objects " + overlap + " overlap", this,
            overlap);
    }

    Utils.checkDefaults(this, cpu.memoryMap);

    // temporary hack: declare CPU rate here
    cpu.clockRate = 80;

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
    return ("ti/platforms/gccLM3S9B90RAM/linkcmd.xdt");
}

/*
 *  ======== Platform.Instance.init ========
 *  This function is called to initialize a newly created instance of a
 *  platform.  Platform instances are created just prior to running
 *  program configuration scripts.
 *
 *  @param(name)        the name used to identify this instance (without
 *                      the package name prefix)
 */
function instance$meta$init(name)
{
    /* setup clocks */
    Boot = xdc.useModule('ti.catalog.arm.cortexm3.lm3init.Boot');

    Boot.configureClock = true;
    Boot.oscSrc = Boot.OSCSRC_MAIN;     /* use main oscillator */
    Boot.pllBypass = false;             /* use PLL to drive sys clock */
    Boot.pllOutEnable = false;          /* no PLL output */
    Boot.ioscDisable = false;           /* IOSC is enabled */
    Boot.moscDisable = false;           /* MOSC is enabled */

    Boot.configureLdo = false;
    Boot.sysClockDiv = Boot.SYSDIV_2_5; /* 80 MHz */
    Boot.pwmClockDiv = Boot.PWMDIV_1;
    Boot.xtal = Boot.XTAL_16MHZ ;       /* XTAL = 16 MHz */
}

/*
 *  @(#) ti.platforms.gccLM3S9B90RAM; 1, 0, 1, 1,69; 12-20-2013 12:46:47; /db/ztree/library/trees/platform/platform-o37x/src/
 */

