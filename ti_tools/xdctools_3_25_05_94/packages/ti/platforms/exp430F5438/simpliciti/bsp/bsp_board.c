/**************************************************************************************************
  Revised:        $Date: 2007-07-06 11:19:00 -0700 (Fri, 06 Jul 2007) $
  Revision:       $Revision: 13579 $

  Copyright 2007 Texas Instruments Incorporated.  All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights granted under
  the terms of a software license agreement between the user who downloaded the software,
  his/her employer (which must be your employer) and Texas Instruments Incorporated (the
  "License"). You may not use this Software unless you agree to abide by the terms of the
  License. The License limits your use, and you acknowledge, that the Software may not be
  modified, copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio frequency
  transceiver, which is integrated into your product. Other than for the foregoing purpose,
  you may not use, reproduce, copy, prepare derivative works of, modify, distribute,
  perform, display or sell this Software and/or its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED ôAS ISö
  WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY
  WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
  IN NO EVENT SHALL TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE
  THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY
  INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST
  DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY
  THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
**************************************************************************************************/

/* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 *   BSP (Board Support Package)
 *   Target : Texas Instruments MSP-EXP430F5438
 *            "MSP430F5438 Experimenter Board"
 *   Top-level board code file.
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

/* ------------------------------------------------------------------------------------------------
 *                                            Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "bsp.h"
#include "bsp_config.h"
#include "bsp_button_defs.h"
#include "PMM.c"

/* ------------------------------------------------------------------------------------------------
 *                                            Defines
 * ------------------------------------------------------------------------------------------------
 */
#define BSP_TIMER_CLK_MHZ   (BSP_CONFIG_CLOCK_MHZ_SELECT)
#define BSP_DELAY_MAX_USEC  (0xFFFF/BSP_TIMER_CLK_MHZ)

/*******************************************************************************
*******************
 * @fn          BSP_EARLY_INIT
 *
 * @brief       This function is called by start-up code before doing the normal
 initialization
 *              of data segments. If the return value is zero, initialization is
 not performed.
 *              The global macro label "BSP_EARLY_INIT" gets #defined in the bsp
_msp430_defs.h
 *              file, according to the specific compiler environment (CCE or IAR
). In the CCE
 *              environment this macro invokes "_system_pre_init()" and in the I
AR environment
 *              this macro invokes "__low_level_init()".
 *
 * @param       None
 *
 * @return      0 - don't intialize data segments / 1 - do initialization
 *******************************************************************************
*******************
*/
BSP_EARLY_INIT(void)
{
  /* Disable watchdog timer */
  WDTCTL = WDTPW | WDTHOLD;

  /* Return 1 - run seg_init */
  return (1);
}

/**************************************************************************************************
 * @fn          BSP_InitBoard
 *
 * @brief       Initialize the board.
 *
 * @param       none
 *
 * @return      none
 **************************************************************************************************
 */
void BSP_InitBoard(void)
{	
  volatile int i; 
	
  /* Setup XT1 crystal */ 
  P7DIR = 0xFE;                             // Enable xtal pins 
  P7SEL |= 0x03;                            
  while ( (SFRIFG1 & OFIFG) )
  {    
    UCSCTL7 &= ~(XT1LFOFFG + DCOFFG);
    SFRIFG1 &= ~OFIFG;
  }
  UCSCTL6 &= ~(XT1DRIVE_3);                 // Xtal is now stable, reduce drive
                                            // strength
  
  /* Set appropriate core voltage */
  if(BSP_CONFIG_CLOCK_MHZ_SELECT <= 8)       SetVCore(0);                               
  else if(BSP_CONFIG_CLOCK_MHZ_SELECT <= 12) SetVCore(1);                               
  else if(BSP_CONFIG_CLOCK_MHZ_SELECT <= 20) SetVCore(2);                               
  else                                       SetVCore(3);                                 
  
  /* Initialize the UCS module*/ 
  __bis_SR_register(SCG0);                  // Disable the FLL control loop
  UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx 
  UCSCTL1 = BSP_CONFIG_MSP430_DCORSEL;       // Select suitable range 
  UCSCTL2 = BSP_CONFIG_MSP430_FLLDx + BSP_CONFIG_MSP430_N; // Select divider, multiplier bits
  UCSCTL3 = 0;                              // FLL Reference Clock = XT1
  UCSCTL4 = SELS__DCOCLK  |  SELM__DCOCLK;  // MCLK & SMCLK = DCOCLK 
  UCSCTL4 |= SELA__XT1CLK;                  // ACLK = XT1 (32 kHz REFO) 
  
  UCSCTL6 &= ~XT1DRIVE_3;                   // Set DCO drive to consume least power  
  UCSCTL6 |= XCAP_3 ;                       // Setup internal caps for crystal  
  __bic_SR_register(SCG0);
  
  /* Give FLL enough time to settle */ 
  if(BSP_CONFIG_CLOCK_MHZ_SELECT <= 8)       __delay_cycles(250000);                                
  else if(BSP_CONFIG_CLOCK_MHZ_SELECT <= 12) __delay_cycles(375000);                    
  else if(BSP_CONFIG_CLOCK_MHZ_SELECT <= 20) __delay_cycles(625000);                    
  else                                       __delay_cycles(812500);

  /* Tie unused ports */ 
  P11DIR = 0xFF;
  P11OUT = 0;
  P11SEL = 0;
  
  PADIR  = 0x3FFF;                          // P2.6 & P2.7 button inputs 
  PAOUT  = 0;
  PASEL  = 0;
  __bsp_BUTTON1_CONFIG_def__();             // Enable pull-ups              
  __bsp_BUTTON2_CONFIG_def__();             // Enable pull-ups 
  PBDIR  = 0xFFFF;
  PBOUT  = 0;
  PBSEL  = 0;
  PCDIR  = 0xFFFF;
  PCOUT  = 0;
  PCSEL  = 0;
  PDDIR  = 0xFFFF;
  PDOUT  = 0;
  PDSEL  = 0;
  PEDIR  = 0xFFFF;
  PEOUT  = 0;
  PESEL  = 0;
     
  P6OUT = BIT3;                             //BIT3 = Active Low Enable Pin for 
                                            //       audio output amplifier

  P7DIR = 0xFE;
  P7OUT &= ~0xFE;
  P7SEL = 0x03;                             //XT1 
  
  /* Configure TimerA for use by the delay function */
  TA1CTL |= TACLR;                          // Reset the timer 
  TA1CTL = 0x0;                             // Clear all settings 
  TA1CTL |= TASSEL_2;                       // Select the clk source to be 
                                            //     - SMCLK (Sub-Main CLK)

}

/**************************************************************************************************
 * @fn          BSP_Delay
 *
 * @brief       Sleep for the requested amount of time.
 *
 * @param       # of microseconds to sleep.
 *
 * @return      none
 **************************************************************************************************
 */
void BSP_Delay(uint16_t usec)
{
  BSP_ASSERT(usec < BSP_DELAY_MAX_USEC);

  TA1R = 0; //initial count
  TA1CCR0 = BSP_TIMER_CLK_MHZ*usec; //compare count. (delay in ticks)

  /* Start the timer in UP mode */
  TA1CTL |= MC_1;

  /* Loop till compare interrupt flag is set */
  while(!(TA1CCTL0 & CCIFG));

  /* Stop the timer */
  TA1CTL &= ~(MC_1);

  /* Clear the interrupt flag */
  TA1CCTL0 &= ~CCIFG;
}

/**************************************************************************************************
 *                                  Compile Time Integrity Checks
 **************************************************************************************************
 */
// Error if FLL multiplier > 127
#if (!defined BSP_CONFIG_MSP430_N) || \
    (BSP_CONFIG_MSP430_N == 0) || (BSP_CONFIG_MSP430_N > 127)
#error "ERROR: Missing or illegal value for N (see register UCSCTL2)."
#endif

// Error if Frequency Loop Divisor > 4
#if (!defined BSP_CONFIG_MSP430_FLLDx) || (BSP_CONFIG_MSP430_FLLDx > FLLD_2)
#error "ERROR: Missing or illegal value for FLLDx (see register UCSCTL2)."
#endif

// Error if DCORSEL > 7 
#if (!defined BSP_CONFIG_MSP430_DCORSEL) || (BSP_CONFIG_MSP430_DCORSEL > DCORSEL_7)
#error "ERROR: Missing or illegal value for FLLDx (see register SCFI0)."
#endif


/**************************************************************************************************
*/



