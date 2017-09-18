/*
 *  ======== bsp_board_defs.h ========
 *
 *   BSP (Board Support Package)
 *   Target : Texas Instruments MSP-EXP430F5529
 *            "MSP430F5529 Experimenter Board"
 *   Board definition file.
 */

#ifndef BSP_BOARD_DEFS_H
#define BSP_BOARD_DEFS_H

#define BSP_BOARD_EXP5529

#include "bsp_msp430_defs_hack.h" /* patched version to allow use of stdint.h */

#include "bsp_config.h"

#define __bsp_CLOCK_MHZ__         BSP_CONFIG_CLOCK_MHZ

#define BSP_BOARD_C               "bsp_board.c"
#define BSP_INIT_BOARD()          BSP_InitBoard()
#define BSP_DELAY_USECS(x)        BSP_Delay(x)

void BSP_InitBoard(void);
void BSP_Delay(uint16_t usec);

#endif
