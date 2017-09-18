/*!
 *  ======== ti.mcu.msp430.runtime ========
 *  Local extensions to the xdc.runtime modules
 *
 *  This package illustrates how to extend the xdc.runtime runtime
 *  for a particular platform.  In this case, the modules in this package
 *  enable full use of the xdc.runtime for the EZ430-RF2500 hardare.
 *
 *  Although the modules in this package are designed to support the
 *  EZ430-RF2500, large parts of the implementations are portable to
 *  many similar platforms.
 */
package ti.mcu.msp430.runtime [1, 0, 0] {
    module SysBuf;
    module SysUart;
    module SysCallback;
    module LoggerUart;
    module LoggerCallback;
    module GateGIE;
    module HeapNull;
}
