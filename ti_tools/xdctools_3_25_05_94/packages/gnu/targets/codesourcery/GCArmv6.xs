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
 *  ======== GCArmv6.getISAChain ========
 *  GCArmv6 implementation for ITarget.getISAChain()
 */
function getISAChain (isa)
{
    var myChain = [this.$package.GCArmv5T.isa, this.isa];
    var isaIn = (isa == null ? this.isa : isa)

    for (var i = 0; i < myChain.length; i++) {
        if (myChain[i] == isaIn) {
            break;
        }
    }

    if (i == myChain.length) {
        return (null);
    }
    else {
        return (myChain.slice(0, i + 1));
    }
}

/*
 *  ======== getVersion ========
 */
function getVersion()
{
    print("Warning: gnu.targets.codesourcery targets are deprecated. Please "
        + "use\ngnu.targets.arm targets instead. See the documentation for "
        + "gnu.targets.arm\nand gnu.targets.codesourcery packages for more "
        + "information.");
    return this.$super.getVersion();
}
/*
 *  @(#) gnu.targets.codesourcery; 1, 0, 0,484; 12-19-2013 15:12:06; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

