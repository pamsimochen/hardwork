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
 *  ======== Package.getLibs ========
 */
function getLibs(prog)
{
    var libs = "lib/" + this.$name + ".a" + Program.build.target.suffix;

    libs += ";lib/boot.a" + Program.build.target.suffix;
    libs += ";lib/syscalls.a" + Program.build.target.suffix; 

    return (libs);
}
/*
 *  @(#) gnu.targets.arm.rtsv7M; 1, 0, 0,69; 12-20-2013 16:12:07; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

