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
 * Universal Serial Communication Interface B0 SPI 2xx
 */
metaonly interface IUSCI_B0_SPI inherits IUSCI_SPI {

instance:
    /*! Control Register 0 */
    config UCxCTL0_t UCB0CTL0 = {
        UCCKPH      : UCCKPH_OFF,
        UCCKPL      : UCCKPL_OFF,
        UCMSB       : UCMSB_OFF,
        UC7BIT      : UC7BIT_OFF,
        UCMST       : UCMST_OFF,
        UCMODE      : UCMODE_0,
        UCSYNC      : UCSYNC,
    };

    /*! Control Register 1 */       
    config UCxCTL1_t UCB0CTL1 = {
        UCSSEL      : UCSSEL_0,
        UCSWRST     : UCSWRST,
    };

    /*! Status Register */
    config UCxSTAT_t UCB0STAT = {
        UCLISTEN    : UCLISTEN_OFF,
        UCFE        : UCFE_OFF,
        UCOE        : UCOE_OFF,
        UCBUSY      : UCBUSY,
    };

    /*! Bit Rate Control Register 0 */
    config Bits8 UCB0BR0 = 0;
    
    /*! Bit Rate Control Register 1 */
    config Bits8 UCB0BR1 = 0;

    /*! Receive Buffer Register */    
    config Bits8 UCB0RXBUF = 0;
    
    /*! Transmit Buffer Register */    
    config Bits8 UCB0TXBUF = 0;
}
