/* 
 *  Copyright (c) 2013 Texas Instruments and others.
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
 *  ======== Main.xs ========
 *  Main entry point
 */
/*
 *  ======== File ========
 *  Used for performing file operations
 */
var File = xdc.useModule("xdc.services.io.File");

/*
 *  ======== opts ========
 *  command line options
 */
var opts = {};

/*
 *  ======== run ========
 *  Main function, executed from the command line.
 */
function run(cmdr, args)
{
    /* put config parameters in a local copy so mappings() can add to them */
    for (var p in this) {
        opts[p] = this[p];
    }

    /* check if all the required options are provided */
    if (opts["compileOptions"] == null || opts["linkOptions"] == null
        || opts["compiler"] == null || opts["rootDir"] == null
        || opts["device"] == null || opts["compileOptions"] == null) {
        cmdr.usage();

        cmdr.error("One or more required options were not provided"); 
    }

    /* convert Windows paths to Linux paths */
    for (var p in this) {
        opts[p] = opts[p].replace(/\\/g, '/');
    }

    /* find the config file */
    var cfg = "";
    if (args.length > 0) {
        cfg = args[0].replace(/\\/g, '/');
    }
    else {
        cfg = findCfg(cmdr);
    }
    print("\nUsing RTSC configuration file: " + cfg + "...\n");

    /* check the output directory */
    if (opts["output"] == "") {
        opts["output"] = cfg.substring(0, cfg.lastIndexOf('/')) + "/configPkg";
    }

    /* fetch the target/platform/profile */
    mappings(cmdr);

    /* execute xdc.tools.configuro */
    var configuro = xdc.useModule('xdc.tools.configuro.Main');
    configuro.exec(["-o",  opts["output"], "-t", opts["t"], "-p",  opts["p"],
        "-r", opts["profile"], "-c", opts["rootDir"], "--linkOptions", 
        opts["linkOptions"], cfg]);
}

/*
 *  ======== findCfg ========
 *  Searches  for config file in the directory provided 
 */
function findCfg(cmdr)
{
    var cfg = "";
    var projDir = "";

    try {
        projDir = opts["projFile"];
        projDir = projDir.substring(0, projDir.lastIndexOf('/'));
        var ewp = File.open(opts["projFile"], "r"); 

        var line = null;
        while ((line = ewp.readLine()) != null) {
            if (line.match(/\w\.cfg/)) {
                /* Get filename from "<name>$PROJ_DIR$\app.cfg</name>" */ 
                cfg = line.substring(line.lastIndexOf('$') + 2, line.lastIndexOf('<'));

                /* Has it been excluded from Build? */
                if (((line = ewp.readLine()) != null) && line.match(/excluded/)) {
                    var config = /Release/;
                    if (opts["compileOptions"].match(/--debug/)) {
                        config = /Debug/;
                    }

                    if (((line = ewp.readLine()) != null) && line.match(config)) {
                        cfg = "";
                        continue;
                    }
                }
                break;
            }
        }
        
        if (cfg == "") {
            throw(new Error(".cfg file not found"));
        }
    }
    catch (e) {
        cmdr.error("RTSC configuration file (.cfg) was not found");
    }

    return (projDir + "/" + cfg);
}

/*
 *  ======== mappings ========
 *  Maps compiler options to target/platform/profile
 */
function mappings(cmdr)
{
    /* MSP430 */
    if (opts["compiler"].match(/icc430/)) {
        opts["t"] = "iar.targets.msp430.MSP430X_small";
        opts["p"] = "ti.platforms.msp430";
    }
    /* ARM */
    else if (opts["compiler"].match(/iccarm/)) {
        opts["t"] = "iar.targets.arm.M3";
        if (opts["compileOptions"].match(/Cortex-M4F /)) {
            opts["t"] = "iar.targets.arm.M4F";
        }
        else if (opts["compileOptions"].match(/Cortex-M4 /)) {
            opts["t"] = "iar.targets.arm.M4";
        }
       
        opts["p"] = "ti.platforms.tiva";
        if (opts["device"].match(/^CC26/)) {
            opts["p"] = "ti.platforms.cc26xx";
        }
    }
    else {
        cmdr.error("The compiler '" + opts["compiler"] + "' is not supported.");
    }

    /* profile */
    if (opts["profile"] == "") {
        if (opts["compileOptions"].match(/--debug/)) {
            opts["profile"] = "debug";
        }
        else {
            opts["profile"] = "release";
        }
    }

    /* Add _full to profile to link full library configuration */
    if (opts["compileOptions"].match(/dl430xsff/) 
        || opts["compileOptions"].match(/DLib_Config_Full.h/)) { 
        opts["profile"] += "_full";
    }

    /* parse and add the device to the platform */
    opts["p"] += ":" + opts["device"];
}
/*
 *  @(#) iar.tools.configuro; 1, 0, 0,17; 12-7-2013 14:15:42; /db/ztree/library/trees/xdctools/xdctools-f41x/src/
 */

