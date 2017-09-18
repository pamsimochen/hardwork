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
 *  ======== IUSI.xs ========
 */

var USI;
var GetSet;

/*
 *  ======== module$meta$init ========
 */
function module$meta$init()
{
    USI = this;
    GetSet = xdc.module("xdc.services.getset.GetSet");
}
