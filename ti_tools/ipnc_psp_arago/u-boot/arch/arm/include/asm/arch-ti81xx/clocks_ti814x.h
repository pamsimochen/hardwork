/*
 * (C) Copyright 2006-2008
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR /PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef _CLOCKS_TI814X_H_
#define _CLOCKS_TI814X_H_

/*
 *	Device Operating Points (OPP MODE)
 *
 *		OPP			    ARM		HDVICP2	    HDVPSS	    ISS		Media_Ctrl	    L3/L4/Core		DDR
 *		OPP_100	        600		  220		 200	    400		  200	    	200			    400
 *		OPP_120	        720		  290		 200	    400		  200	    	200			    400
 *		OPP_TURBO	    970		  410	     240	    480		  240	    	240			    533
 *		OPP_NITRO	   1000	      450		 280	    560		  280	    	240			    533
 */
#define DDR_PLL_400     /* Values supported 400,533 */

#define OSC_0_FREQ	20

#define DCO_HS2_MIN	500
#define DCO_HS2_MAX	1000
#define DCO_HS1_MIN	1000
#define DCO_HS1_MAX	2000

#define SELFREQDCO_HS2	0x00000801
#define SELFREQDCO_HS1	0x00001001
#define ADPLLJ_CLKCRTL_CLKDCO    0x201a0000

/* Put the pll config values over here */
#define AUDIO_N		19
#define AUDIO_M		500
#define AUDIO_M2	2
#define AUDIO_CLKCTRL	0x801

#define MODENA_N	0x1
#ifdef CONFIG_TI813X
#define MODENA_M	(opp_val_ti813x(0x3C, 0x3C))
#else
//#define MODENA_M	(opp_val_dm385(0x3C, 0x3C))
//#define MODENA_M	(opp_val_dm385(72, 72))//720MHz
#define MODENA_M	(opp_val_dm385(100, 100))//1000MHz
#endif
#define MODENA_M2	1
#define MODENA_CLKCTRL	0x1

#define L3_N		19
#ifdef CONFIG_TI813X
#define L3_M		(opp_val_ti813x(800, 800))
#else
//#define L3_M		(opp_val_dm385(800, 800))
#define L3_M		(opp_val_dm385(960, 960))//240MHz
#endif
#define L3_M2		4
#define L3_CLKCTRL	0x801

#define DDR_N		19
#ifdef CONFIG_TI813X
/* DDR PLL */
/* For 400 MHz */
#if defined(DDR_PLL_400)
#define DDR_M		(opp_val_ti813x(800, 800))
#endif
/* For 533 MHz */
#if defined(DDR_PLL_533)
#define DDR_M		(opp_val_ti813x(1066, 1066))
#endif
#elif defined(CONFIG_DM385)
#if defined(CONFIG_DM385_DDR3_533)
#define DDR_M	(opp_val_dm385(1066, 1066))
#endif
#if defined(CONFIG_DM385_DDR3_400)
#define DDR_M	(opp_val_dm385(800, 800))
#endif
#else
#define DDR_M	(pg_val_ti814x(800, 800))
#endif
#define DDR_M2		2
#define DDR_CLKCTRL	0x801

#define DSP_N		19
#define DSP_M		500
#define DSP_M2		1
#define DSP_CLKCTRL	0x801

#define DSS_N		19
#ifdef CONFIG_TI813X
#define DSS_M		(opp_val_ti813x(800, 800))
#else
#define DSS_M		(opp_val_dm385(800, 800))
#endif
#define DSS_M2		4
#define DSS_CLKCTRL	0x801

#define IVA_N		19
#ifdef CONFIG_TI813X
#define IVA_M		(opp_val_ti813x(532, 640))
#define IVA_M2		2
#else
#define IVA_M		(opp_val_dm385(880, 900))
#define IVA_M2		(opp_val_dm385(4, 2))
#endif
#define IVA_CLKCTRL	0x801

#define ISS_N		19
//#define ISS_M		800
//#define ISS_M		960//480MHz
#define ISS_M		1120//560MHz
#ifdef CONFIG_TI813X
#define ISS_M2		(opp_val_ti813x(4, 2))
#else
#define ISS_M2		(opp_val_dm385(2, 2))
#endif
#define ISS_CLKCTRL	0x801

#define USB_N		19
#define USB_M		960
#if defined(CONFIG_TI813X)
#define USB_M2		(opp_val_ti813x(5, 5))
#elif defined(CONFIG_DM385)
#define USB_M2		(opp_val_dm385(5, 5))
#else
#define USB_M2		(pg_val_ti814x(1, 5))
#endif
#define USB_CLKCTRL	0x200a0801

#define HDMI_N		19
#define HDMI_M		1485
#define HDMI_M2		10
#define HDMI_CLKCTRL	0x290a1001

#define VIDEO0_N		19
#define VIDEO0_M		540
#define VIDEO0_M2		10
#define VIDEO0_CLKCTRL	0x09000801

#define VIDEO1_N		19
#define VIDEO1_M		1485
#define VIDEO1_M2		10
#define VIDEO1_CLKCTRL	0x290a1001
#endif	/* endif _CLOCKS_TI814X_H_ */
