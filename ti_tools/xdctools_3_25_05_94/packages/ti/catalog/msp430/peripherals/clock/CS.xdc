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

/*!
 *  ======== CS ========
 *  MSP430 Clock System
 */
metaonly module CS inherits IClock
{

instance:

    config Float VLOCLKHz = 8300;
    config Float WATCHCRYSTALCLKHz = 32768;
    config Float XT1CLKHz = 0;
    config Float XT2CLKHz = 0;
}
