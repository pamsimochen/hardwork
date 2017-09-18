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
 *  ======== SysUart.xdc ========
 */
import xdc.runtime.Types;

/*!
 *  ======== SysUart ========
 *  Implementation of ISystemSupport that sends output to a UART
 *
 *  This module provides an implementation of the `{@link ISystemSupport}`
 *  interface that simply writes each character to the MSP430's UART.
 */
@ModuleStartup
@Template("./SysUart.xdt")
module SysUart inherits xdc.runtime.ISystemSupport {
 
    /*!
     *  ======== GetLineFxn ========
     *  Input a single line
     */
     typedef Void (*GetLineFxn)(Char[], Int);

    /*!
     *  ======== getLineFxn ========
     *  User suplied character input function
     *
     *  If this parameter is set to a non-`null` value, the specified
     *  function will be called to input a line of chars received by
     *  `SysUart`.
     *
     *  For example, if you define a function named `myGetLineFxn`, the
     *  following configuration fragment will cause `SysUart` to call
     *  `myGetLineFxn` whenever a line is received.
     *  @p(code)
     *      var SysUart = xdc.useModule("xdc.runtime.SysUart");
     *      SysUart.getLineFxn = "&myGetLineFxn";
     *  @p
     *
     *  If this parameter is not set, a default function will be used which
     *  simply drops the input.
     *
     *  @see #GetLineFxn
     */
    config GetLineFxn getLineFxn = null;

    /*!
     *  ======== lineSize ========
     *  The maximum line buffer size
     *
     *  The maximum number of characters to buffer before calling 
     *  `getLineFxn`.  If set to zero, no buffering occurs and each
     *  character received triggers `getLineFxn` (if it's defined).
     */
    config Int lineSize = 0;

internal:
    
    /*!
     *  ======== rxtxInit ========
     *  Initialize peripheral HW at startup
     */
    void rxtxInit();

    /*!
     *  ======== tx ========
     *  Transmit one character
     */
    void tx(char c);
    
    /*!
     *  ======== rx ========
     *  Receive one character
     */
    void rx(char c);

    config Char lineBuf[];

    struct Module_State {
        UInt lineIdx;
    }
}
