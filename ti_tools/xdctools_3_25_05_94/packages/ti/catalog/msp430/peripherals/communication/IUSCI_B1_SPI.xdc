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
 * Universal Serial Communication Interface B1 SPI 2xx
 */
metaonly interface IUSCI_B1_SPI inherits IUSCI_SPI {

instance:
    /*! Control Register 0 */
    config UCxCTL0_t UCB1CTL0 = {
        UCCKPH      : UCCKPH_OFF,
        UCCKPL      : UCCKPL_OFF,
        UCMSB       : UCMSB_OFF,
        UC7BIT      : UC7BIT_OFF,
        UCMST       : UCMST_OFF,
        UCMODE      : UCMODE_0,
        UCSYNC      : UCSYNC,
    };

    /*! Control Register 1 */       
    config UCxCTL1_t UCB1CTL1 = {
        UCSSEL      : UCSSEL_0,
        UCSWRST     : UCSWRST,
    };

    /*! Status Register */
    config UCxSTAT_t UCB1STAT = {
        UCLISTEN    : UCLISTEN_OFF,
        UCFE        : UCFE_OFF,
        UCOE        : UCOE_OFF,
        UCBUSY      : UCBUSY,
    };

    /*! Bit Rate Control Register 0 */
    config Bits8 UCB1BR0 = 0;
    
    /*! Bit Rate Control Register 1 */
    config Bits8 UCB1BR1 = 0;

    /*! Receive Buffer Register */    
    config Bits8 UCB1RXBUF = 0;
    
    /*! Transmit Buffer Register */    
    config Bits8 UCB1TXBUF = 0;
}
