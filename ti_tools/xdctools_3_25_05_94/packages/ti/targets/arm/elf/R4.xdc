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

/*!
 *  ======== R4.xdc ========
 *  Cortex R4 little endian thumb2 (ELF)
 */
metaonly module R4 inherits ti.targets.arm.elf.IR4 {
    override readonly config string name        = "R4";
    override readonly config string suffix      = "er4";
    override readonly config string rts         = "ti.targets.arm.rtsarm";

    override readonly config xdc.bld.ITarget.Model model = {
        endian: "little",
        shortEnums: true
    };

    override readonly config xdc.bld.ITarget2.Command cc = {
        cmd:  "armcl -c",
        opts: "--endian=little -mv7R4 --abi=eabi"
    };

    override readonly config xdc.bld.ITarget2.Command asm = {
        cmd:  "armcl -c",
        opts: "--endian=little -mv7R4 --abi=eabi"
    };
}
/*
 *  @(#) ti.targets.arm.elf; 1, 0, 0,376; 12-19-2013 15:12:31; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

