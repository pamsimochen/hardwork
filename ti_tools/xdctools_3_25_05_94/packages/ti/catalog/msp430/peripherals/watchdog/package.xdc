requires ti.catalog.msp430.peripherals.clock;
requires ti.catalog.msp430.peripherals.special_function;

/*!
 *  ======== ti.catalog.msp430.peripherals.watchdog ========
 *  MSP430 WDT+ (watchdog timer) peripheral
 */
package ti.catalog.msp430.peripherals.watchdog [1, 0, 0] {
    module WDTplus;
    module WDT_A;
    interface IWDT;
}
