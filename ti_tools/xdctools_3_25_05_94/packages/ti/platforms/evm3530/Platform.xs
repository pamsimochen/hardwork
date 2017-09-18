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
 *  Platform support for the evm3530
 *
 */

var Utils = xdc.useModule('xdc.platform.Utils');

/*
 *  ======== Platform.getCpuDataSheet ========
 *  This Platform's implementation xdc.platform.IPlatform.getCpuDataSheet.
 *
 *  Return the xdc.platform.ICpuDataSheet.Instance object that corresponds
 *  to the device that runs executables loaded on the specified cpuId.
 */
function getCpuDataSheet(cpuId)
{
    if (cpuId == "0") {
        return (Utils.getCpuDataSheet(this.$module.DSP));
    }
    else if (cpuId == "1") {
        return (Utils.getCpuDataSheet(this.$module.GPP));
    }
    else {
        this.$module.$logError("The platform " + this.$module.$name +
            " does not contain cpu with cpuId: " + cpuId, this.$module, null);
    }
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
    var args = this.$private.args;

    var ExeContext = xdc.useModule('xdc.platform.ExeContext');

    /* create a default ExeContext execution context */
    var cpu;
    var core;
    /* We need to add v5t, v5T, v5TE because of several changes we made for
     * the name of that architecture.
     */
    var armChain = ["470", "v4T", "v5T", "v5TE", "v5t", "v6", "v7A", "v7A8"];
    var dspChain = ["62", "64", "64P"];
    for (var i = 0; i < dspChain.length; i++) {
        if (dspChain[i] == prog.build.target.isa) {
            xdc.loadPackage(this.$module.DSP.catalogName);
            cpu = ExeContext.create(this.$module.DSP, this.$module.BOARD);
            core = "DSP";
        }
    }
    for (var i = 0; i < armChain.length; i++) {
        if (armChain[i] == prog.build.target.isa) {
            xdc.loadPackage(this.$module.GPP.catalogName);
            cpu = ExeContext.create(this.$module.GPP, this.$module.BOARD);
            core = "GPP";
        }
    }
    if (cpu == undefined) {
        this.$module.$logError("The build target " + prog.build.target.$name +
            " is incompatible with this platform.", this.$module, null);
    }

    cpu.memoryMap = Utils.assembleMemoryMap(cpu, this);

    if (this.codeMemory == undefined) {
        if (core == "GPP") {
            this.codeMemory = "DDR2";
        }
        else {
            this.codeMemory = "IRAM";
        }
    }

    // check for the overlap in the memory map
    var overlap = Utils.checkOverlap(cpu.memoryMap);

    if (overlap != null) {
        this.$module.$logError("Memory objects " + overlap + " overlap", this,
            overlap);
    }

    Utils.checkDefaults(this, cpu.memoryMap);
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
           "execute " + prog.name + " on " + os + "\n" + updateComment +
           "\n\t:");
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

    /* Use bare metal linker command file if appropriate */
    if (prog.build.target.$name.match(/gnu.targets.arm.A/)) {
        var templateName = tpkg.replace(/\./g, "/") + "/linkcmd_bm_v7a.xdt";
    }
    else {
        var templateName = tpkg.replace(/\./g, "/") + "/linkcmd.xdt";
    }

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
 *  Platform instances may also be created in the build domain.
 *
 *  @param(name)        the name used to identify this instance (without
 *                      the package name prefix).
 */
function instance$meta$init(name)
{
}
/*
 *  @(#) ti.platforms.evm3530; 1, 0, 0,316; 12-20-2013 12:46:05; /db/ztree/library/trees/platform/platform-o37x/src/
 */

