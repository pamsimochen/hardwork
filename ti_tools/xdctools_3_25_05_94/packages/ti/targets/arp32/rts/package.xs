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
function getLibs(prog)
{
    var libs = "lib/" + this.$name + ".a" + prog.build.target.suffix
        + ";lib/boot.a" + prog.build.target.suffix;
    return (libs);
}
/*
 *  @(#) ti.targets.arp32.rts; 1, 0, 0,48; 12-20-2013 16:13:21; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

