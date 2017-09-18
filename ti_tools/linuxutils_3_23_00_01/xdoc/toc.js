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
function toggle(e)
{
    if (e.className == 'open') {
        e.className = 'closed';
    }
    else if (e.className == 'closed') {
        e.className = 'open';
    }
}

function cancel(e)
{
    if (window.event) {
        window.event.cancelBubble = true;   // ie
    }
    else if (e.stopPropagation)  {
        e.stopPropagation();                // firefox
    }
}
/*
 *  @(#) linuxutils_3_23_00_01; 1, 0, 0,1; 5-30-2012 11:51:37; /db/atree/library/trees/linuxutils/linuxutils-l01/src/ xlibrary

 */

