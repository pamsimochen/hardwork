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
import xdc.bld.ITarget2;

/*!
 *  ======== Qnx.xdc ========
 *  Embedded little endian ARM v7 Qnx target
 *
 *  This module defines an embedded target for Qnx on ARM.
 *
 */
@TargetHeader("xdc/bld/stddefs.xdt")
metaonly module Qnx inherits xdc.bld.ITarget2 {
    override readonly config string name                = "Qnx";
    override readonly config string os                  = "Qnx";
    override readonly config string suffix              = "qv7";
    override readonly config string isa                 = "v7A";
    override readonly config xdc.bld.ITarget.Model model= {
        endian: "little"
    };

    override readonly config string rts = "qnx.targets.arm.rts";
    override config string platform     = "host.platforms.arm";

    config String QNX_HOST = "";
    config String QNX_TARGET = "";
    config String QNX_CONFIGURATION = "";
    config String GCCVERS = "";

    override readonly config String stdInclude = "qnx/targets/arm/std.h";

    override readonly config ITarget2.Command cc = {
        cmd: "$(rootDir)/bin/qcc -Vgcc_ntoarmv7 -EL -c",
        opts: ""
    };

    override config ITarget2.Options ccOpts = {
//        prefix: "-fPIC -Wunused",
        prefix: "",
        suffix: "-Dfar= "
    };

    override readonly config ITarget2.Command asm = {
        cmd: "$(rootDir)/bin/qcc -c -x assembler",
        opts: ""
    };

    override readonly config ITarget2.Command ar = {
        cmd: "$(rootDir)/bin/ntoarm-ar ",
        opts: "cr"
    };

    override readonly config ITarget2.Command lnk = {
        cmd: "$(rootDir)/bin/qcc -Vgcc_ntoarmv7 -EL ",
        opts: ""
    };

    override config ITarget2.Options lnkOpts = {
        prefix: "",
        suffix: ""
//        suffix: "-Wl,-Map=$(XDCCFGDIR)/$@.map"
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
 *  @(#) qnx.targets.arm; 1, 0, 0,128; 12-19-2013 15:12:23; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

