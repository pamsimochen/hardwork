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
 *  ======== Comparator_Aplus.xs ========
 */

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
}

/*
 *  ======== instance$meta$init ========
 */
function instance$meta$init()
{
    this.interruptSource[0].registerName = "CAIE";
    this.interruptSource[0].registerDescription = "Comparator_A+ interrupt enable";
    this.interruptSource[0].interruptEnable = false;
    this.interruptSource[0].interruptHandler = false;
}
