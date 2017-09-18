/* 
 *  Copyright (c) 2011 Texas Instruments and others.
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

/*!
 *  ======== Bionic.compile ========
 */
function compile(goal) {

    if (this.libcDir == undefined) {
        throw new Packages.xdc.services.global.XDCException(
            "xdc.bld.TARGET_CONFIG_ERROR", "The parameter 'libcDir' must be"
            + " defined for the target google.targets.arm.Bionic.");
    }

    if (goal.opts.incs != undefined) {
        goal.opts.incs += "-I" + this.libcDir + "/libc/arch-arm/include ";
    }
    else {
        goal.opts.incs = "-I" + this.libcDir + "/libc/arch-arm/include ";
    }

    goal.opts.incs += "-I" + this.libcDir + "/libc/include ";
    goal.opts.incs += "-I" + this.libcDir + "/libstdc++/include ";
    goal.opts.incs += "-I" + this.libcDir + "/libc/kernel/common ";
    goal.opts.incs += "-I" + this.libcDir + "/libc/kernel/arch-arm ";
    goal.opts.incs += "-I" + this.libcDir + "/libm/include ";
    goal.opts.incs += "-I" + this.libcDir + "/libm/include/arch/arm ";

    return (this.$super.compile.$fxn.call(this, goal));
}

var firstLink = true;
/*
 *  ======== Bionic.link ========
 */
function link(goal) {
    /* Set up WinCE profiles to include the right lib directories */
    if (this.fsDir == undefined) {
        throw new Packages.xdc.services.global.XDCException(
            "xdc.bld.TARGET_CONFIG_ERROR", "The parameter 'fsDir' must be"
            + " defined for the target google.targets.arm.Bionic.");
    }

    /* Be careful and don't do this more than once */
    if (firstLink) {
        this.lnkOpts.suffix += " -L" + this.fsDir + "/lib";
        this.lnkOpts.suffix += " -Wl,-rpath-link=" + this.fsDir + "/lib";
        this.lnkOpts.suffix += " " + this.fsDir + "/lib/crtbegin_dynamic.o";
        this.lnkOpts.suffix += " " + this.rootDir + "/lib/gcc/arm-eabi/" + this.GCCVERS + "/libgcc.a";
        this.lnkOpts.suffix += " " + this.fsDir + "/lib/crtend_android.o";
        this.lnkOpts.suffix += " " + this.fsDir + "/lib/libc.so";

        /* use 'bfd' linker - there were some issues with the "gold" linker */
        this.lnkOpts.suffix += " -fuse-ld=bfd ";

        firstLink = false;
}

    return (this.$super.link.$fxn.call(this, goal));
}
/*
 *  @(#) google.targets.arm; 1, 0, 0,138; 12-19-2013 15:12:11; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

