/* --COPYRIGHT--,EPL
 *  Copyright (c) 2012 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * --/COPYRIGHT--*/

function getLibs (prog)
{
    var suffix = prog.build.target.suffix;
    var lib = "lib/" + this.profile + "/" + this.$name + ".a" + suffix;

    if (suffix == null) {
        return (null);
    } 

    if (!(java.io.File(this.packageBase + lib).exists())) {
        prog.$logWarning("Rts library for " + this. profile  + " profile is not"  
            + " available. Instead a rts library for release profile will" 
            + " be linked", prog); 
        lib = "lib/release/" + this.$name + ".a" + suffix;
        
    }

    return (lib);
}
