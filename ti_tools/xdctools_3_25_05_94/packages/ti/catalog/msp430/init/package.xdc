requires xdc.runtime;

/*!
 *  ======== ti.catalog.msp430.init ========
 *  MSP430 boot time initialization support
 *
 *  This package contains a `{@link Boot}` module that supplies minimal
 *  boot time initialization code.  This code optionally disables the
 *  watchdog timer (to ensure normal C initialization can complete before
 *  the watchdog resets the device) and configures the CPU clock.
 */
package ti.catalog.msp430.init[1,0,0] {
    module Boot;
}
