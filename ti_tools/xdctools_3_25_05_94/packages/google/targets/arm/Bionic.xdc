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
import xdc.bld.ITarget2;

/*!
 *  ======== Bionic.xdc ========
 *  Embedded little endian ARM v5TE Bionic target
 *
 *  This module defines an embedded target for Bionic on ARM. The target
 *  generates code compatible with the "v5TE" architecture.
 *
 */
metaonly module Bionic inherits google.targets.ITarget {
    override readonly config string name                = "Bionic";
    override readonly config string os                  = "Linux";
    override readonly config string suffix              = "bnc";
    override readonly config string isa                 = "v5T";
    override readonly config xdc.bld.ITarget.Model model= {
        endian: "little"
    };

    override readonly config string rts = "google.targets.arm.rts";
    override config string platform     = "host.platforms.arm";

    override config string LONGNAME;
    override config string libcDir;
    override config string fsDir;

    override readonly config String stdInclude = "google/targets/arm/std.h";

    override readonly config ITarget2.Command cc = {
        cmd: "$(rootDir)/$(LONGNAME) -c -MD -MF $@.dep",
        opts: "-march=armv5t -fno-short-enums "
    };

    override config ITarget2.Options ccOpts = {
        prefix: "-fPIC -Wunused",
        suffix: "-Dfar=  -D_ANDROID_"
    };

    override readonly config ITarget2.Command asm = {
        cmd: "$(rootDir)/$(LONGNAME) -c -x assembler",
        opts: "-march=armv5t"
    };

    override config ITarget2.Options lnkOpts = {
        prefix: "",
        suffix: "-Wl,-Map=$(XDCCFGDIR)/$@.map "
    };

    override readonly config xdc.bld.ITarget.StdTypes stdTypes = {
        t_IArg          : { size: 4, align: 4 },
        t_Char          : { size: 1, align: 1 },
        t_Double        : { size: 8, align: 4 },
        t_Float         : { size: 4, align: 4 },
        t_Fxn           : { size: 4, align: 4 },
        t_Int           : { size: 4, align: 4 },
        t_Int8          : { size: 1, align: 1 },
        t_Int16         : { size: 2, align: 2 },
        t_Int32         : { size: 4, align: 4 },
        t_Int64         : { size: 8, align: 4 },
        t_Long          : { size: 4, align: 4 },
        t_LDouble       : { size: 8, align: 4 },
        t_LLong         : { size: 8, align: 4 },
        t_Ptr           : { size: 4, align: 4 },
        t_Short         : { size: 2, align: 2 },
                t_Size          : { size: 4, align: 4 },
    };
}
/*
 *  @(#) google.targets.arm; 1, 0, 0,138; 12-19-2013 15:12:11; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

