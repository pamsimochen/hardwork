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
 *  ======== idle ========
 */
void idle(void)
{
    __bis_SR_register(LPM0_bits + GIE); /* Enter LPM0, interrupts enabled */
}

/*
 *  ======== USCI0RX_ISR ========
 */
__interrupt void USCI0RX_ISR(void)
{
    char c;
        
    while (!(IFG2 & UCA0TXIFG)) {   /* USCI_A0 TX buffer ready? */
        ;
    }

    c = UCA0RXBUF;

    if (c == 'W') {
        __bic_SR_register_on_exit(LPM0_bits);   /* Clear LPM0 bits from 0(SR) */
    }
    if (SysUart_inputFxn != NULL) {
        SysUart_inputFxn(&c);
    }
}

__asm("\t.global USCI0RX_ISR");
__asm("\t.sect   \".usci_rx\"");
__asm("\t.align  2");
__asm("_USCI0_rx_vector:\n\t.field USCI0RX_ISR, 16");

