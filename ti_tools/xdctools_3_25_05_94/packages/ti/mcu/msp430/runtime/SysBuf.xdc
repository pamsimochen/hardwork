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
 *  ======== SysBuf.xdc ========
 */
import xdc.runtime.Types;

/*!
 *  ======== SysBuf ========
 *  Minimal implementation of functions required by System
 *
 *  `SysBuf` writes all output to a circular memory buffer configured by
 *  the user.
 *
 *  This module provides a simple but 100% portable implementation of the
 *  `{@link ISystemSupport}` interface.
 */
module SysBuf inherits xdc.runtime.ISystemSupport {
 
    /*!
     *  ======== bufSize ========
     *  Size (in MAUs) of the output.
     *
     *  An internal buffer of this size is allocated. All output is stored 
     *  in this internal buffer. 
     *
     *  If 0 is specified for the size, no buffer is created and all output
     *  is simply dropped.
     */
    config SizeT bufSize = 1024;

    /*!
     *  ======== bufName ========
     *  Global variable name containing pointer to output buffer
     *
     *  This variable will be created so that it is easy to view the
     *  contents of the output buffer within a debugger.  If `bufName` is
     *  set to `null` or `undefined`, the variable will not be created.
     */
    metaonly config String bufName = "SYSBUF";
    
    /*!
     *  ======== sectionName ========
     *  Section where the internal buffer managed by `SysBuf` is placed.
     *
     *  The default is to have no explicit placement.
     */
    metaonly config String sectionName = null;
   
internal:
    
    struct Module_State {
        Char outbuf[];  /* output buffer */
        UInt outidx;    /* index of next available output character */
    }
}
