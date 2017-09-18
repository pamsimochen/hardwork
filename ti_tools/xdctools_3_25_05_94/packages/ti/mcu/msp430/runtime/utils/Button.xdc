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
 *  ======== Button ========
 *  Simple momentary button switch manager
 */
@ModuleStartup
module Button
{
    /* PORT0 only defined on msp430x3[123]x devices */
    metaonly readonly config Ptr PORT0 = 0x11;  

    metaonly readonly config Ptr PORT1 = 0x21;
    metaonly readonly config Ptr PORT2 = 0x29;

    metaonly readonly config Ptr PORT0_IE  = 0x15;
    metaonly readonly config Ptr PORT0_IFG = 0x13;
    metaonly readonly config Ptr PORT0_OUT = 0x11;
    metaonly readonly config Ptr PORT0_REN = 0x11;  /* does not exist */

    metaonly readonly config Ptr PORT1_IE  = 0x25;
    metaonly readonly config Ptr PORT1_IFG = 0x23;
    metaonly readonly config Ptr PORT1_OUT = 0x21;
    metaonly readonly config Ptr PORT1_REN = 0x27;

    metaonly readonly config Ptr PORT2_IE  = 0x2D;
    metaonly readonly config Ptr PORT2_IFG = 0x2B;
    metaonly readonly config Ptr PORT2_OUT = 0x29;
    metaonly readonly config Ptr PORT2_REN = 0x2F;

    metaonly readonly config Ptr PORTA_IE_L  = 0x21A;
    metaonly readonly config Ptr PORTA_IFG_L = 0x21C;
    metaonly readonly config Ptr PORTA_OUT_L = 0x202;
    metaonly readonly config Ptr PORTA_REN_L = 0x206;

    metaonly readonly config Ptr PORTA_IE_H  = 0x21B;
    metaonly readonly config Ptr PORTA_IFG_H = 0x21D;
    metaonly readonly config Ptr PORTA_OUT_H = 0x203;
    metaonly readonly config Ptr PORTA_REN_H = 0x207;

    /*!
     *  ======== PORT ========
     *  Address of 8-bit output port to use
     */
    config Ptr PORT = PORT1;        /* platform dependent port number */

    /*!
     *  ======== PORT_IE ========
     *  Address of Interrupt Enable register
     */
    config Ptr PORT_IE = PORT1_IE;

    /*!
     *  ======== PORT_OUT ========
     *  Address of output configuration register
     */
    config Ptr PORT_OUT = PORT1_OUT;

    /*!
     *  ======== PORT_REN ========
     *  Address of pullup/down resister enable configuration register 
     */
    config Ptr PORT_REN = PORT1_REN;

    /*!
     *  ======== PORT_IFG ========
     *  Address of Interrupt Flag register
     */
    config Ptr PORT_IFG = PORT1_IFG;
    
    /*!
     *  ======== SWITCH1 ========
     *  Port mask for switch #1
     */
    config Bits8 SWITCH1 = 0x1;         /* platform dependent pin */

    /*!
     *  ======== SWITCH2 ========
     *  Port mask for switch #2
     */
    config Bits8 SWITCH2 = 0x2;         /* platform dependent pin */

    /*!
     *  ======== enable ========
     *  Enable the specified buttons 
     */
    @DirectCall
    Void enable(Bits8 mask);

    /*!
     *  ======== disable ========
     *  Disable the specified buttons 
     */
    @DirectCall
    Void disable(Bits8 mask);

    /*!
     *  ======== read ========
     *  Atomically read and clear the specified buttons
     *
     *  This function returns the state of the buttons and clears
     *  their state.  Only the state of the buttons specified by
     *  `mask` are affected.
     */
    @DirectCall
    Bits8 read(Bits8 mask);
}
