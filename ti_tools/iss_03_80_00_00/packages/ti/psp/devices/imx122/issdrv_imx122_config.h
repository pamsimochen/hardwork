/** ==================================================================
 *  @file   issdrv_imx122_config.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/imx122/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _ISSDRV_IMX122_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_IMX122_CONFIG_H_

#define IMX122_BOARD_PRECONFIG		(TRUE)             // TURE/FALSE
#define FRAME_LENGTH 1125


#define IMX122_WRITE_CMD		0x02
#define IMX122_READ_CMD			0x82

#define IMX122_GAIN_MAX			0x8C

/* Register definition */
#define STANDBY					0x00
#define VREVERSE				0x01
#define MODE					0x02
#define HMAX_L					0x03
#define HMAX_H					0x04
#define VMAX_L					0x05
#define VMAX_H					0x06
#define SHS1_L					0x08		/* Integration time adjustment, designated in line units */
#define SHS1_H					0x09
#define SPL_L					0x0D		/* Integration time adjustment(Low Speed Shutter), designated in frame units */
#define SPL_H					0x0E
#define SVS_L					0x0F		/* Integration time adjustment(Low Speed Shutter), designated in frame units */
#define SVS_H					0x10	

#define GAIN					0x1E		/* From 0x00(0dB) ~ 0x8C(13.8dB) */

typedef struct
{
    unsigned char regAddr;
    unsigned char regValue;

}Iss_RegPair;


const Iss_RegPair IMX_122_INIT_DATA[] = 
{
	{0x00, 0x31},		//STANDBY, TESTEN
	{0x11, 0x80}, 		//OPORTSEL
	{0x2d, 0x40},		//DCKDLY
	{0x02, 0x0F},		//MODE
	{0x16, 0x3c},		//WINPV_L
	{0x17, 0x00},		//WINPV_H
	{0x14, 0x00},		//WINPH_L
	{0x15, 0x00},		//WINPH_H 
	{0x1A, 0x51},		//WINWV_L 
	{0x1B, 0x04},		//WINWV_H  
	{0x18, 0xc0},		//WINWH_L	
	{0x19, 0x07},		//WINWH_H	
	{0xCE, 0x16},		//PRES
	{0xCF, 0x82},		//DRES_L
	{0xD0, 0x00},		//DRES_H
	{0x9a, 0x26},		//12B1080P_L
	{0x9b, 0x02},		//12B1080P_H
	{0x01, 0x00},		//VREVERSE
	{0x12, 0x82},		//ADRES
	{0x11, 0x80},		//FRSEL
	{0x0f, 0x00},		//SVS_L
	{0x10, 0x00},		//SVS_H
	{0x0d, 0x00},		//SPL_L
	{0x0e, 0x00},		//SPL_H
	{0x08, 0x00},		//SHS1_L
	{0x09, 0x00},		//SHS1_H
	{0x1e, 0x00},		//GAIN
	{0x20, 0x3c},		//BLKLEVEL_L
	{0x21, 0x00},		//BLKLEVEL_H,XHSLNG
	{0x22, 0x00},		//XVSLNG
	{0x05, 0x65},		//VMAX_L
	{0x06, 0x04},		//VMAX_H
	{0x03, 0x4c},		//HMAX_L
	{0x04, 0x04},		//HMAX_H
	{0x2c, 0x00},		//XMSTA
						//Initial Settings
	{0x00, 0x00},		/* Standby cancel */

};
const Iss_RegPair IMX_122_720P_INIT_DATA[] = 
{
	{0x00, 0x31},		/* Standby*/ 		
	//{0x11, 0x88},
	{0x11, 0x80},
	{0x2d, 0x40},
	{0x02, 0x01},//0xF
	{0x16, 0xF0},//0x3c WINPV
	{0x17, 0x00},//0x00
	{0x14, 0x40},//0x00 WINPH
	{0x15, 0x01},//0x00
	{0x1A, 0xe9},//0x51 WINWV
	{0x1B, 0x02},//0x04
	{0x18, 0x40},//0xc0 WINWH
	{0x19, 0x05},//0x07
	{0x01, 0x00},		/* V-reverse */
//	{0x01, 0x01},		/* V-reverse */
	{0x12, 0x82},//12bit resolution
	//{0x11, 0x88},
	{0x11, 0x81},//0x80
	{0x0f, 0x00},
	{0x10, 0x00},
	{0x0d, 0x00},
	{0x0e, 0x00},
	{0x08, 0x00},
	{0x09, 0x00},
	{0x1e, 0x00},
//		{0x20, 0x0},		/* black off */
//		{0x20, 0x3c},		/* black off */
		{0x20, 0x3c},		/* black off */
	{0x22, 0x80},//0x00
	{0x05, 0xee},//0x65
	{0x06, 0x02},//0x04

	{0x03, 0x72},//0x4c
	{0x04, 0x06},//0x04
	//{0x03, 0x98},
	//{0x04, 0x08},

	//{0x2c, 0x01},		/*  Slave */
	{0x2c, 0x00},		/*  Slave */
	{0x9a, 0x4c},//0x26
	{0x9b, 0x04},//0x02
	{0x00, 0x00},		/* Standby cancel */

};

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
