/*
 *  Copyright 2013 by Texas Instruments Incorporated.
 *
 */

var BIOS = xdc.module('ti.sysbios.BIOS');

BIOS.libType = BIOS.LibType_Custom;
BIOS.buildingAppLib = false;
BIOS.assertsEnabled = true;
BIOS.logsEnabled = true;

var SourceDir = xdc.module("xdc.cfg.SourceDir");
SourceDir.verbose = 1;

/* suppress un-placed sections warning from m3 Hwi.meta$init() */
if (Program.sectMap[".vecs"] !== undefined) {
    Program.sectMap[".vecs"].type = "DSECT";
}
if (Program.sectMap[".resetVecs"] !== undefined) {
    Program.sectMap[".resetVecs"].type= "DSECT";
}

/*
 *  @(#) ti.sdo.ipc; 1, 0, 0, 0,; 5-10-2013 12:29:44; /db/vtree/library/trees/ipc/ipc-i15/src/ xlibrary

 */

