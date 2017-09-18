/* --COPYRIGHT--,EPL
 *  Copyright (c) 2008 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 * 
 *  Contributors:
 *      Texas Instruments - initial implementation
 * 
 * --/COPYRIGHT--*/

/*
 *  ======== init ========
 */
function init()
{
    if (xdc.om.$name != "cfg") {
        return;
    }

    getPlatformDefaults(this);
}

/*
 *  ======== getPlatformDefaults ========
 */
function getPlatformDefaults(pkg)
{
    var platInfoFile = Program.platform.$package.packageBase + "/defaults.xs";

    try {
        var grace = xdc.loadCapsule(platInfoFile);
    }
    catch (e) {
//        print(e);
        return;     /* ignore errors */
    }

    for each (var mod in pkg.$modules) {
        print("looking for platform defaults for " + mod + " ...");
        var defaults = grace.defaults[mod.$name];
        for (var i in defaults) {
            if (i in mod) {
                var val = defaults[i];
                if (typeof val == "string" && val[0] == '#') {
                    var tmp = val.substr(1);
                    mod[i] = tmp in mod ? mod[tmp] : val;
                }
                else {
                    mod[i] = val;
                }
                print("    set " + i + " = " + val + "(" + (mod[i] - 0) + ")");
            }
        }
    }
}
