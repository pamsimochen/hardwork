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
 *  ======== SysCallback.xdc ========
 */
import xdc.runtime.Types;

/*!
 *  ======== SysCallback ========
 *  ISystemSupport provider that "calls back" to application defined functions
 *
 *  This module provides an implementation of the `{@link ISystemSupport}`
 *  interface that simply calls back to user defined functions to enable the
 *  System module's functionality.
 */
@ModuleStartup
@Template("./SysCallback.xdt")
module SysCallback inherits xdc.runtime.ISystemSupport {
    /*!
     *  ======== PutCharFxn ========
     *  Putchar function signature
     */
    typedef Void (*PutCharFxn)(Char);

    /*!
     *  ======== AtExitFxn ========
     *  Function called as part of normal application exit
     */
    typedef Int (*AtExitFxn)(void (*)(void));

    /*!
     *  ======== AbortFxn ========
     *  Abort function signature
     */
    typedef Void (*AbortFxn)(void);

    /*!
     *  ======== InitFxn ========
     *  Init function signature
     */
    typedef Void (*InitFxn)(void);

    /*!
     *  ======== ExitFxn ========
     *  Exit function signature
     */
    typedef Void (*ExitFxn)(int);

    /*!
     *  ======== putCharFxn ========
     *  User suplied character output function
     *
     *  This function is called whenever the `System` module needs to output
     *  a character; e.g., during `System_printf()` or `System_putch()`.
     *
     *  If this function is set to `null`, output characters are simply
     *  ignored (i.e., they are not output).
     */
    config PutCharFxn putCharFxn = null;

    /*!
     *  ======== atExitFxn ========
     *  User supplied atexit function to be called when `atexit()` is called
     *
     *  This function is called when the application calls `atexit()` and its
     *  argument is passed to this function.
     *
     *  If this function is set to `null`, the function passed to `atexit()`
     *  is ignored and `atexit()` will return 0.
     */
    config AtExitFxn atExitFxn = null;

    /*!
     *  ======== abortFxn ========
     *  User supplied abort function
     *
     *  This function is called when `abort()` is called.  If it returns to
     *  the caller, `abort()` will enter into a self loop to prevent futher
     *  execution.
     *
     *  If this function is set to `null`, `abort()` will enter into a self
     *  loop to prevent futher execution.
     */
    config AbortFxn abortFxn = null;

    /*!
     *  ======== initFxn ========
     *  User supplied initialization function
     *
     *  If this function is set to non-`null` value, it is called prior to
     *  `main()` during this module's startup function.
     */
    config InitFxn initFxn = null;

    /*!
     *  ======== exitFxn ========
     *  User supplied exit function
     *
     *  This function is called when `exit()` is called.  If it returns to
     *  the caller, `exit()` will enter into a self loop to prevent futher
     *  execution.
     *
     *  If this function is set to `null`, `exit()` will enter into a self
     *  loop to prevent futher execution.
     */
    config ExitFxn exitFxn = null;
}
