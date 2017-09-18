/* --COPYRIGHT--,ESD
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
 * --/COPYRIGHT--*/
/*
 *  ======== LoggerUart.xdc ========
 */

/*!
 *  ======== LoggerUart ========
 *  A logger that writes events to a UART
 *
 *  @a(Examples)
 *  Configuration example: The following XDC configuration statements
 *  create a logger instance, assign it as the default logger for all
 *  modules, and enable `USER1` logging in all modules of the package
 *  `my.pkg`. See the `{@link Diags#setMaskMeta Diags.setMaskMeta()}` function
 *  for details on specifying the module names.
 *
 *  @p(code)
 *  var Defaults = xdc.useModule('xdc.runtime.Defaults');
 *  var Diags = xdc.useModule('xdc.runtime.Diags');
 *  var LoggerUart = xdc.useModule('xdc.runtime.LoggerUart');
 *
 *  var LoggerUartParams = new LoggerUart.Params();
 *  Defaults.common$.logger = LoggerUart.create(LoggerUartParams);
 *  Diags.setMaskMeta("my.pkg.%", Diags.USER1, Diags.RUNTIME_ON);
 *  @p
 */

module LoggerUart inherits xdc.runtime.ILogger {

    /*!
     *  ======== ITimestampProxy ========
     *  User supplied time-stamp proxy
     *
     *  This proxy allows `LoggerUart` to use a timestamp server different
     *  from the server used by `{@link xdc.runtime.Timestamp}`. However, if
     *  not supplied by a user, this proxy defaults to whichever timestamp
     *  server is used by `Timestamp`.
     */
    proxy TimestampProxy inherits xdc.runtime.ITimestampClient;

instance:

    /*!
     *  ======== create ========
     *  Create a `LoggerUart` logger
     *     
     *  The logger instance will route all log events it receives to
     *  the {@link System#printf} function.
     */
    create();
 
internal:
    
    struct Instance_State {
        Bool enabled;
    };
}
