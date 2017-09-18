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
 *  ======== package.xs ========
 *
 *! Revision History
 *! ================
 *! 02-Mar-2010 sg      Initial.
 */

/*
 *  ======== Package.getLibs ========
 */
function getLibs(prog)
{
    var libs = "lib/ti.catalog.msp430.init.a" + prog.build.target.suffix;

    return (libs);
}
