/*
 *  ======== bsp_driver_defs.h ========
 *
 *   BSP (Board Support Package)
 *   Target : Texas Instruments MSP-EXP430F5529
 *            "MSP430F5529 Experimenter Board"
 *   Driver definition file.
 */

#ifndef BSP_DRIVER_DEFS_H
#define BSP_DRIVER_DEFS_H

#define BSP_DRIVERS_C               "bsp_drivers.c"
#define BSP_INIT_DRIVERS()          BSP_InitDrivers()

void BSP_InitDrivers(void);

#endif
