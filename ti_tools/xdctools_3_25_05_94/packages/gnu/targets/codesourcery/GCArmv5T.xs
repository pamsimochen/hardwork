/*
 *  Copyright 2013 by Texas Instruments Incorporated.
 *
 */

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

