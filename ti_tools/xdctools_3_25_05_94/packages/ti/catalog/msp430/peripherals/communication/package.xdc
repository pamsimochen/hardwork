requires ti.catalog.msp430.peripherals.clock;

/*!
 *  ======== ti.catalog.msp430.peripherals.communication ========
 *  MSP430 USCI peripherals
 */
package ti.catalog.msp430.peripherals.communication [1, 0, 0]
{
    interface IUSI;
    interface IUSCI , IUSCI_I2C , IUSCI_SPI , IUSCI_UART;
    interface IUSCI_A0_UART , IUSCI_A0_SPI , IUSCI_A1_UART;
    interface IUSCI_B0_SPI , IUSCI_B0_I2C;
    interface IEUSCI;
    module USCI_A0_UART_2xx;
    module USCI_A0_SPI_2xx;
    module USCI_B0_SPI_2xx;
    module USCI_B0_I2C_2xx;
    module USCI_A1_UART_2xx;
    module USCI_A1_SPI_2xx;
    module USCI_B1_SPI_2xx;
    module USCI_B1_I2C_2xx;
    module USI;
    module EUSCI_A0;
    module EUSCI_A1;
    module EUSCI_B0;
}
