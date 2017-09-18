/*
 *  Copyright (c) 2008 Texas Instruments. All rights reserved.
 *  This program and the accompanying materials are made available under the
 *  terms of the Eclipse Public License v1.0 and Eclipse Distribution License
 *  v. 1.0 which accompanies this distribution. The Eclipse Public License is
 *  available at http://www.eclipse.org/legal/epl-v10.html and the Eclipse
 *  Distribution License is available at
 *  http://www.eclipse.org/org/documents/edl-v10.php.
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 * */

/*
 *  ======== SysBuf.xs ========
 */

var Memory = null;
 
/*
 *  ======== module$static$init ========
 */
function module$static$init(obj, params)
{
    /* set size and placement of SysBuf's output buffer, outbuf */
    obj.outbuf.length = params.bufSize;
    Memory.staticPlace(obj.outbuf, 0, params.sectionName);

    /* clear the output buffer */
    for (var i = 0; i < obj.outbuf.length; i++) {
        obj.outbuf[i] = 0;
    }

    /* define a convenient alias for the buffer */
    if (this.bufName != null) {
        Program.global[this.bufName] = obj.outbuf;
    }

    /* initialize output index state */
    obj.outidx = 0;
}

/*
 *  ======== module$use ========
 */
function module$use(obj, params)
{
    Memory = xdc.module('xdc.runtime.Memory');
}
