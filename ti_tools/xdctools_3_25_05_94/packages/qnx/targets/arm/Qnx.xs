/* 
 *  Copyright (c) 2012 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * 
 */

var _bldUtils = xdc.useModule('xdc.bld.Utils');

/*
 *  ======== ITarget.getVersion ========
 *  Note that because we inherit from gnu.targets.ITarget, and
 *  use its $super methods, we need to set the following vars here:
 *     target.GCCVERS
 *     target.GCCTARG
 *     target.BINVERS
    target.$unseal("GCCVERS");
    target.$unseal("GCCTARG");
    target.$unseal("BINVERS");
 */
function getVersion()
{
    var target = this;

    /* map 2.3.4 to "1,0,2.3,4", 2.3 to "1,0,2.3", and 2 to "1,0,2" */

    var result = target.$name + "{";
    var key = "1,0";

    var va = target.GCCVERS.split('.');
    for (var i = 0; i < va.length; i++) {
        key += (i == 1 ? '.' : ',') + va[i];
    }

    result += key;
//print("returning getVersion - " + result);
    return (result);
}


/*
 *  ======== Qnx.archive ========
 */
function archive(goal)
{/*
print("archive - goal:\n" +
"base       - " + goal.base + "\n" +
"dstPrefix  - " + goal.dstPrefix + "\n" +
"dstSuffix  - " + goal.dstSuffix + "\n" +
"files      - " + goal.files + "\n" +
"profile    - " + goal.profile + "\n" +
"opts       - " + goal.opts + "\n"
);
*/

    var target = this;
    var result = new xdc.om['xdc.bld'].ITarget.CommandSet;

    var envs = [
        "QNX_HOST=" + target.QNX_HOST,
        "QNX_TARGET=" + target.QNX_TARGET,
        "QNX_CONFIGURATION=" + target.QNX_CONFIGURATION
    ];
    result.envs = envs;

    var cmdString = target.ar.cmd +
        " $(AROPTS_P) " + target.ar.opts + " $@ $(AROPTS_S) " +
        "$(aropts) $(files)";

    result.cmds = _bldUtils.expandString(cmdString, {
        AROPTS_P:   target.arOpts.prefix,
        AROPTS_S:   target.arOpts.suffix,
        aropts:     goal.opts,
        files:      goal.files
    });

    result.msg = "archiving " + goal.files + " into $@ ...";

    return (result);
}

/*
 *  ======== Qnx.compile ========
 */
function compile(goal) {
/*
print("compile - goal:\n" +
"base       - " + goal.base + "\n" +
"dstPrefix  - " + goal.dstPrefix + "\n" +
"dstSuffix  - " + goal.dstSuffix + "\n" +
"srcSuffix  - " + goal.srcSuffix + "\n" +
"srcPrefix  - " + goal.srcPrefix + "\n" +
"profile    - " + goal.profile + "\n" +
"opts.aopts - " + goal.opts.aopts + "\n" +
"opts.copts - " + goal.opts.copts + "\n" +
"opts.cfgcopts - " + goal.opts.cfgcopts + "\n" +
"opts.defs - " + goal.opts.defs + "\n" +
"opts.incs - " + goal.opts.incs + "\n" +
"configOpts - " + goal.configOpts + "\n"
);*/

    var target = this;
    var result = new xdc.om['xdc.bld'].ITarget.CommandSet;

    var envs = [
        "QNX_HOST=" + target.QNX_HOST,
        "QNX_TARGET=" + target.QNX_TARGET,
        "QNX_CONFIGURATION=" + target.QNX_CONFIGURATION
    ];
    result.envs = envs;

    var cmdString = target.cc.cmd +
        " $(COPTS_P) " + target.cc.opts + " $(COPTS_S) " +
        "$(defs) $(copts) $(incs) $(XDCINCS) " +
//        "-o $@ -x c $<";
        "-o $@ $<";

    result.cmds = _bldUtils.expandString(cmdString, {
        COPTS_P:    target.ccOpts.prefix,
        COPTS_S:    target.ccOpts.suffix,
        copts:      goal.configOpts ? goal.opts.cfgcopts : goal.opts.copts,
        defs:       goal.opts.defs,
        incs:       goal.opts.incs
    });

    result.msg = "cl" + target.suffix + " $< ...";

    return (result);
}


/*
 *  ======== Qnx.link ========
 */
function link(goal) {
/*print("link - goal:\n" +
"base       - " + goal.base + "\n" +
"dstPrefix  - " + goal.dstPrefix + "\n" +
"dstSuffix  - " + goal.dstSuffix + "\n" +
"srcSuffix  - " + goal.srcSuffix + "\n" +
"srcPrefix  - " + goal.srcPrefix + "\n" +
"profile    - " + goal.profile + "\n" +
"opts.aopts - " + goal.opts.aopts + "\n" +
"opts.copts - " + goal.opts.copts + "\n" +
"opts.cfgcopts - " + goal.opts.cfgcopts + "\n" +
"opts.defs - " + goal.opts.defs + "\n" +
"opts.incs - " + goal.opts.incs + "\n" +
"configOpts - " + goal.configOpts + "\n"
);*/

    var target = this;
    var result = new xdc.om['xdc.bld'].ITarget.CommandSet;

    var envs = [
        "QNX_HOST=" + target.QNX_HOST,
        "QNX_TARGET=" + target.QNX_TARGET,
        "QNX_CONFIGURATION=" + target.QNX_CONFIGURATION
    ];
    result.envs = envs;

    /* TODO - do we need to also remove the generated .map file here? */
    var cmdString = target.lnk.cmd +
        " $(LOPTS_P) $(lopts) " + target.lnk.opts + " $(LOPTS_S) " +
//        "-o $@ -x c $<";
        "-o $@ $(files)";

    result.cmds = _bldUtils.expandString(cmdString, {
        LOPTS_P:    target.lnkOpts.prefix,
        LOPTS_S:    target.lnkOpts.suffix,
        lopts:      goal.opts,
        files:      goal.files
    });

    result.msg = "lnk" + target.suffix + " $@ ...";

    return (result);
}

/*
 *  ======== Qnx.scompile ========
 */
function scompile(goal) {
/*print("scompile - goal:\n" +
"base       - " + goal.base + "\n" +
"dstPrefix  - " + goal.dstPrefix + "\n" +
"dstSuffix  - " + goal.dstSuffix + "\n" +
"srcSuffix  - " + goal.srcSuffix + "\n" +
"srcPrefix  - " + goal.srcPrefix + "\n" +
"profile    - " + goal.profile + "\n" +
"opts.aopts - " + goal.opts.aopts + "\n" +
"opts.copts - " + goal.opts.copts + "\n" +
"opts.cfgcopts - " + goal.opts.cfgcopts + "\n" +
"opts.defs - " + goal.opts.defs + "\n" +
"opts.incs - " + goal.opts.incs + "\n" +
"configOpts - " + goal.configOpts + "\n"
);*/

    var target = this;
    var result = new xdc.om['xdc.bld'].ITarget.CommandSet;

    var envs = [
        "QNX_HOST=" + target.QNX_HOST,
        "QNX_TARGET=" + target.QNX_TARGET,
        "QNX_CONFIGURATION=" + target.QNX_CONFIGURATION
    ];
    result.envs = envs;

    result.cmds = "obfuscatedscompilecmd";

    return (result);
}


/*
 *  @(#) qnx.targets.arm; 1, 0, 0,128; 12-19-2013 15:12:23; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

