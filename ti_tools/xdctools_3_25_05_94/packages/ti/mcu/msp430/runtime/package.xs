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
 *  ======== close ========
 */
function close()
{
    var System = xdc.module("xdc.runtime.System");

    /* if no one defined the System gate, define it here */
    if (System.common$.gate == null && Program.build.target.isa == "430") {
        print("using " + this.$name + ".GateGIE to protect System");
        var GateGIE = xdc.useModule(this.$name + ".GateGIE");
        System.common$.gate = GateGIE.create();
    }
    
    var Memory = xdc.module("xdc.runtime.Memory");
    if (Memory.$used && Memory.HeapProxy == null) {
        print("using " + this.$name + ".HeapNull to proxy Memory");
        Memory.HeapProxy = xdc.useModule(this.$name + ".HeapNull");
    }
}
