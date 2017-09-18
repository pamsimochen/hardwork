/* 
 *  Copyright (c) 2008 Texas Instruments and others.
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
 *  ======== C64T.xdc ========
 *
 */

/*!
 *  ======== C64T ========
 *  TI C64T little endian (ELF)
 */
metaonly module C64T inherits ITarget {
    override readonly config string name                 = "C64T";
    override readonly config string suffix               = "e64T";
    override readonly config string isa                  = "64T";
    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little",
        shortEnums: false
    };
    override readonly config xdc.bld.ITarget.Module base = ti.targets.C62;

    override config string platform     = "ti.platforms.sdp4430";

    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd: "cl6x -c",
        opts: "-mv=tesla --abi=eabi"
    };

    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd: "cl6x -c",
        opts: "-mv=tesla --abi=eabi"
    };

    override readonly config xdc.bld.ITarget.StdTypes stdTypes = 
        ti.targets.elf.C64P.stdTypes;

    override readonly config Int bitsPerChar = 
        ti.targets.C62.bitsPerChar;
}
/*
 *  @(#) ti.targets.elf; 1, 0, 0,394; 12-19-2013 15:12:37; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */
