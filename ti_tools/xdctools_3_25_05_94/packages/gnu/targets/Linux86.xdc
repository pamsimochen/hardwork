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
 *  ======== Linux86.xdc ========
 *  Native x86 Linux target
 *
 *  This module defines the native target for Linux on PC.
 */
metaonly module Linux86 inherits gnu.targets.ITarget {
    override readonly config string name                = "Linux86";    
    override readonly config string os                  = "Linux";      
    override readonly config string suffix              = "86U";
    override readonly config string isa                 = "i686";       
    override readonly config xdc.bld.ITarget.Model model= {
        endian: "little"
    };
    override readonly config string rts                 = "gnu.targets.rts86U";
    override config string platform                     = "host.platforms.PC";
    
    /*
     *  ======== ar ========
     */
    override readonly config xdc.bld.ITarget2.Command ar = {
        cmd: "$(rootDir)/bin/ar",
        opts: "cr"
    };

    /*
     *  ======== ccOpts ========
     */
    override config xdc.bld.ITarget2.Options ccOpts = {
        prefix: "-fPIC -Wunused",
        suffix: "-Dfar="
    };

    /*
     *  ======== lnkOpts ========
     */
    override config xdc.bld.ITarget2.Options lnkOpts = {
        prefix: "",
        suffix: "-Wl,-Map=$(XDCCFGDIR)/$@.map -lstdc++ -L$(rootDir)/lib"
    };
        
    /*
     *  ======== includeOpts ========
     *  $(rootDir)/include        - contains compiler specific headers
     *
     *  GCC compilers for Linux automatically add this directory, but at least
     *  in some cases, the directory is added searched after standard system
     *  directories. This way, we ensure $(rootDir)/include is searched
     *  before standard system directories.
     */
    override config string includeOpts = "-isystem $(rootDir)/include";

    /*
     *  ======== stdTypes ========
     */
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
        t_LDouble       : { size: 12, align: 4 },
        t_LLong         : { size: 8, align: 4 },
        t_Ptr           : { size: 4, align: 4 },
        t_Short         : { size: 2, align: 2 },
        t_Size          : { size: 4, align: 4 },
    };
}
/*
 *  @(#) gnu.targets; 1, 0, 1,630; 12-19-2013 15:11:59; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

