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
 *  ======== Led ========
 *  Simple LED manager
 */
@ModuleStartup
module Led
{
    const Ptr PORT0 = 0x11;  /* only defined on msp430x33x devices */
    const Ptr PORT1 = 0x21;
    const Ptr PORT2 = 0x29;
    const Ptr PORT3 = 0x19;
    const Ptr PORT4 = 0x1D;
    const Ptr PORT5 = 0x31;
    const Ptr PORT6 = 0x35;
    const Ptr PORT7 = 0x3A;  /* = 0x39 for msp430x41x2 devices */
    const Ptr PORT8 = 0x3B;

    const Ptr PORTA_L = 0x202;  /* port 1 */
    const Ptr PORTA_H = 0x203;  /* port 2 */
    const Ptr PORTB_L = 0x222;  /* port 3 */
    const Ptr PORTB_H = 0x223;  /* port 4 */
    const Ptr PORTC_L = 0x242;  /* port 5 */
    const Ptr PORTC_H = 0x243;  /* port 6 */
    const Ptr PORTD_L = 0x262;  /* port 7 */
    const Ptr PORTD_H = 0x263;  /* port 8 */
    const Ptr PORTE_L = 0x282;  /* port 9 */
    const Ptr PORTE_H = 0x283;  /* port 10 */
    const Ptr PORTF_L = 0x2A2;  /* port 11 */
    
    const Ptr PORT0DIR = 0x12;  /* only defined on msp430x33x devices */
    const Ptr PORT1DIR = 0x22;
    const Ptr PORT2DIR = 0x2A;
    const Ptr PORT3DIR = 0x1A;
    const Ptr PORT4DIR = 0x1E;
    const Ptr PORT5DIR = 0x32;
    const Ptr PORT6DIR = 0x36;
    const Ptr PORT7DIR = 0x3C;  /* = 0x3A for msp430x41x2 devices */
    const Ptr PORT8DIR = 0x3D;

    const Ptr PORTADIR_L = 0x204;
    const Ptr PORTADIR_H = 0x205;
    const Ptr PORTBDIR_L = 0x224;
    const Ptr PORTBDIR_H = 0x225;
    const Ptr PORTCDIR_L = 0x244;
    const Ptr PORTCDIR_H = 0x245;
    const Ptr PORTDDIR_L = 0x264;
    const Ptr PORTDDIR_H = 0x265;
    const Ptr PORTEDIR_L = 0x284;
    const Ptr PORTEDIR_H = 0x285;
    const Ptr PORTFDIR_L = 0x2A4;
    
    /*!
     *  ======== PORT ========
     *  Address of 8-bit output port to use
     */
    config Ptr PORT = PORT1;

    /*!
     *  ======== DIRCFG ========
     *  Address of GPIO configuration register
     */
    config Ptr DIRCFG = PORT1DIR;
    
    /*!
     *  ======== RED ========
     *  Port mask for a red LED
     */
    config Bits8 RED = 0x1;             /* platform dependent pin */

    /*!
     *  ======== GREEN ========
     *  Port mask for a green LED
     */
    config Bits8 GREEN = 0x2;           /* platform dependent pin */

    /*!
     *  ======== off ========
     *  Turn specified LED off
     */
    @DirectCall
    Void off(Bits8 mask);

    /*!
     *  ======== on ========
     *  Turn specified LED on
     */
    @DirectCall
    Void on(Bits8 mask);

    /*!
     *  ======== toggle ========
     *  Toggle the state of specified LED
     */
    @DirectCall
    Void toggle(Bits8 mask);
}
