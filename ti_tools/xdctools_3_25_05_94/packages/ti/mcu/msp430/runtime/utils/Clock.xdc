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
/*
 *  ======== Clock.xdc ========
 */

/*!
 *  ======== Clock ========
 *  Basic "wall clock" functions
 */
@ModuleStartup

module Clock {
    /*! 
     *  ======== TimeValue ========
     *  Time value type
     */
    typedef UInt16 TimeValue;
 
    /*!
     *  ======== TimerRegs ========
     *  Timer peripheral register definition
     */
    struct TimerRegs {
        Bits16  CTL;
        Bits16  CCTL[7];
        Bits16  R;
        Bits16  CCR[7];
        Bits16  EX0;
        Bits16  IV;
    };
    
    const TimerRegs *TA2  = 0x160;
    const TimerRegs *TA3  = 0x160;
    const TimerRegs *TA5  = 0x160;

    const TimerRegs *TB3  = 0x180;
    const TimerRegs *TB7  = 0x180;
    const TimerRegs *T1A2 = 0x180;

    const TimerRegs *T0A3 = 0x340;
    const TimerRegs *T0A5 = 0x340;

    const TimerRegs *T1A3 = 0x380;
    const TimerRegs *T1A5 = 0x380;

    const TimerRegs *T2A3 = 0x400;

    const TimerRegs *T0B7 = 0x3d0;

    /*!
     *  ======== TIMER ========
     *  Base address of timer peripheral to use
     */
    config TimerRegs *TIMER = TB3;    /*! Timer B on 2274 */
    
    /*!
     *  ======== fastClockKHz ========
     *  Main System clock frequency (KHz)
     */
    config Int fastClockKHz = 8000;

    /*!
     *  ======== slowClockHz ========
     *  Auxiliary clock frequency (Hz)
     */
    config Int slowClockHz = 12000;
    
    /*!
     *  ======== delay ========
     *  Spin for specified number of micro seconds
     */
    Void delay(UInt16 usec);

    /*!
     *  ======== sleep ========
     *  Idle the CPU for specified number of micro seconds
     */
    Void sleep(UInt16 usec, UInt lpm);

    /*!
     *  ======== getTime ========
     *  Get the current time in Auxiliary clock ticks
     */
    TimeValue getTime();
}
