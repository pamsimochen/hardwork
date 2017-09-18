/*
 *  ======== mrfi_board.c ========
 *
 *   MRFI (Minimal RF Interface)
 *   Board code file.
 *   Target : Texas Instruments MSP-EXP430F5529
 *            "MSP430F5529 Experimenter Board"
 *   Radios : CC1100, CC1101, CC2500
 */

#include "bsp.h"

extern void MRFI_GpioIsr(void);

BSP_ISR_FUNCTION( BSP_GpioPort12sr, PORT2_VECTOR )
{
  /*
   *  This ISR is easily replaced.  The new ISR must simply
   *  include the following function call.
   */
  MRFI_GpioIsr();
}

#include "mrfi_board_defs.h"

#if ( MRFI_GDO0_INT_VECTOR != PORT2_VECTOR )
#error "ERROR:  Mismatch with specified vector and actual ISR."
/*
 *  The most likely fix is to modify the vector in the above ISR.
 *  This compile time check would need to be updated too.
 */
#endif

