/* 
 *  Copyright (c) 2011 Texas Instruments and others.
 *  All rights reserved. This program and the accompanying materials
 *  are made available under the terms of the Eclipse Public License v1.0
 *  which accompanies this distribution, and is available at
 *  http://www.eclipse.org/legal/epl-v10.html
 *
 *  Contributors:
 *      Texas Instruments - initial implementation
 *
 * 
 */
requires xdc.bld;
requires gnu.targets;

/*!
 *  ======== google.targets.arm ========
 *  GCC-based code generation targets for Arm
 *
 *  This package offers a generic support for Google Arm compilers.
 *  For the targets in this package, a user must set configuration parameter
 *  `LONGNAME` in `config.bld`. That parameter specifies the relative path to
 *  the compiler executable from the directory specified in `rootDir`.
 *
 */
package google.targets.arm [1, 0, 0] {
    module Bionic;
}
/*
 *  @(#) google.targets.arm; 1, 0, 0,138; 12-19-2013 15:12:11; /db/ztree/library/trees/xdctargets/xdctargets-g41x/src/ xlibrary

 */

