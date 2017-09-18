/*
 *  ======== bsp_config.h ========
 *
 *   BSP (Board Support Package)
 *   Target : Texas Instruments MSP-EXP430F5529
 *            "MSP430F5529 Experimenter Board"
 *   Board configuration file.
 */

#ifndef BSP_CONFIG_H
#define BSP_CONFIG_H

#define BSP_CONFIG_CLOCK_MHZ_SELECT     8  /* approximate MHz */

#if (BSP_CONFIG_CLOCK_MHZ_SELECT == 8)
#define BSP_CONFIG_CLOCK_MHZ      7.9954
#elif (BSP_CONFIG_CLOCK_MHZ_SELECT == 16)
#define BSP_CONFIG_CLOCK_MHZ      15.9908
#else
#error "ERROR: Unsupported clock speed.  Custom clock speeds are possible.  See comments in code."
#endif

#endif
