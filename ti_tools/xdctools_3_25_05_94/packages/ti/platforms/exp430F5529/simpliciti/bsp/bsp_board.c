/*
 *  ======== bsp_board.c ========
 *
 *   BSP (Board Support Package)
 *   Target : Texas Instruments MSP-EXP430F5529
 *            "MSP430F5529 Experimenter Board"
 *   Top-level board code file.
 */

#include "bsp.h"
#include "bsp_config.h"

#define BSP_TIMER_CLK_MHZ   (BSP_CONFIG_CLOCK_MHZ_SELECT)
#define BSP_DELAY_MAX_USEC  (0xFFFF/BSP_TIMER_CLK_MHZ)

/*
 *  ======== BSP_EARLY_INIT ========
 */
BSP_EARLY_INIT(void)
{
  /* Disable watchdog timer */
  WDTCTL = WDTPW | WDTHOLD;

  return (1);
}

/*
 *  ======== BSP_InitBoard ========
 */
void BSP_InitBoard(void)
{	
  volatile int i; 
	
  /* Configure TimerA for use by the delay function */
  TA1CTL |= TACLR;                          // Reset the timer 
  TA1CTL = 0x0;                             // Clear all settings 
  TA1CTL |= TASSEL_2;                       // Select clk source to be SMCLK

}

/*
 *  ======== BSP_Delay ========
 *  Delay for the specified number of microseconds.
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

