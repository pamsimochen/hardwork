/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/ar0330/src/issdrv_ar0330Priv.h>
#include <ti/psp/devices/ar0330/issdrv_ar0330_config.h>
#include <ti/psp/platforms/iss_platform.h>
#define LONG_EXPO                       0x3012
#define LONG_GAIN                       0x305e


#define PLL_MULTIPLIER (0x0062)

/* Global object storing all information related to all
  AR0330 driver handles */
extern ti2aControlParams_t gTi2aControlParams;
static Iss_Ar0330CommonObj 	gIss_Ar0330CommonObj;
static I2c_Ctrl 				gAr0330_I2c_ctrl;
extern ti2a_output 		ti2a_output_params;

Int32 Iss_Ar0330FrameRateSet(Iss_Ar0330Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs);

Uint16    AR0330_GAIN_TAB[AR0330_GAIN_MAX +1]= {
/*0*/ 0,
/*1*/ 1,
/*2*/ 2,
/*3*/ 3,
/*4*/ 5,
/*5*/ 6,
/*6*/ 8,
/*7*/ 9,
/*8 */ 11,
/*9 */ 13,
/*10*/ 15,
/*11*/ 17,
/*12*/ 18,
/*13*/ 19,
/*14*/ 21,
/*15*/ 22,
/*16*/ 24,
/*17*/ 25,
/*18*/ 27,
/*19*/ 29,
/*20*/ 31,
/*21*/ 49,
/*22*/ 50,
/*23*/ 51,
/*24*/ 53,
/*25*/ 54,
/*26*/ 56,
/*27*/ 57,
/*28*/ 59,
/*29*/ 61,
/*30*/ 63,
/*31*/ 113,
/*32*/ 114,
/*33*/ 115,
/*34*/ 117,
/*35*/ 118,
/*36*/ 120,
/*37*/ 121,
/*38*/ 123,
/*39*/ 125,
/*40*/ 127
};

#define OTPM_VERSION_REG1 0x300E
#define OTPM_VERSION_REG2 0x30F0
#define OTPM_VERSION_REG3 0x3072
#define RESET_REG_ADDR    0x301A
#define RESET_REG_VAL     0x59
#define MIPI_4_LANE       0x1
//#define MIPI_2_LANE       0x1
//#define MIPI_1_LANE       0x1

UInt AR0330_OTPMVsettingsV1()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x30BA;  regValue[i] = 0x2C  ; 
	; i++ ; regAddr[i] = 0x30FE;  regValue[i] = 0x0080; 
	; i++ ; regAddr[i] = 0x31E0;  regValue[i] = 0x0000; 
	; i++ ; regAddr[i] = 0x3ECE;  regValue[i] = 0xFF00; //Only 1 bytes to be modified at 0x3ECE //Lowest address in the highest order byte lane to match the byte ordering on the bus
	; i++ ; regAddr[i] = 0x3ED0;  regValue[i] = 0xE4F6; 
	; i++ ; regAddr[i] = 0x3ED2;  regValue[i] = 0x0146;
	; i++ ; regAddr[i] = 0x3ED4;  regValue[i] = 0x8F6C; 
	; i++ ; regAddr[i] = 0x3ED6;  regValue[i] = 0x66CC; 
	; i++ ; regAddr[i] = 0x3ED8;  regValue[i] = 0x8C42;
	; i++ ; regAddr[i] = 0x3EDA;  regValue[i] = 0x8822; 
	; i++ ; regAddr[i] = 0x3EDC;  regValue[i] = 0x2222; 
	; i++ ; regAddr[i] = 0x3EDE;  regValue[i] = 0x22C0;
	; i++ ; regAddr[i] = 0x3EE0;  regValue[i] = 0x1500; 
	; i++ ; regAddr[i] = 0x3EE6;  regValue[i] = 0x0080; 
	; i++ ; regAddr[i] = 0x3EE8;  regValue[i] = 0x2027;
	; i++ ; regAddr[i] = 0x3EEA;  regValue[i] = 0x001D; 
	; i++ ; regAddr[i] = 0x3F06;  regValue[i] = 0x046A;
	; i++ ; regAddr[i] = 0x3088;  regValue[i] = 0x8000;	// SEQ_CTRL_PORT
	; i++; regAddr[i] = 0x3088;  regValue[i] = 0x4540;	// SEQ_CTRL_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6134;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4A31;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4342;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4560;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2714;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3DFF;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3DFF;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3DEA;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2704;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3D10;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2705;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3D10;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2715;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3527;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x053D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4027;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0427;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x143D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFF3D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFF3D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xEA62;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2728;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3627;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x083D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6444;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C2C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C2C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4B01;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x432D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4643;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1647;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x435F;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4F50;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2604;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2684;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2027;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFC53;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0D5C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0D60;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5754;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1709;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5556;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4917;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x145C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0945;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8026;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xA627;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF817;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0227;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFA5C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0B5F;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5307;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5302;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4D28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6C4C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0928;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x294E;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1718;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x26A2;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5C03;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1744;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2809;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x27F2;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1714;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2808;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x164D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1A26;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8317;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0145;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xA017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0727;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF317;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2945;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0827;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF217;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x285D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x27FA;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x170E;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2681;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5300;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x17E6;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5302;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1710;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2683;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2682;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4827;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF24D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4E28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x094C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0B17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6D28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0817;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x014D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1A17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0126;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x035C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0045 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4027 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9017 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2A4A ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0A43 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x160B ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4327 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9445 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6017 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0727 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9517 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2545 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4017 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0827 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x905D ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2808 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x530D ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2645 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5C01 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2798 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4B12;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4452;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5117;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0260;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x184A;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0343;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1604;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4316;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5843;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1659;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4316;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5A43;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x165B;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4327;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9C45;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0727;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9D17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2545;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1027;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9817;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2022;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4B12;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x442C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C2C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C00;	// SEQ_DATA_PORT

	;i++;
	return i;
}						   
						   
UInt AR0330_OTPMVsettingsV2()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

    regAddr[i] = 0x30BA;  regValue[i] = 0x2C;
	; i++ ; regAddr[i] = 0x30FE;  regValue[i] = 0x0080; 
	; i++ ; regAddr[i] = 0x31E0;  regValue[i] = 0x0000; 
	; i++ ; regAddr[i] = 0x3ECE;  regValue[i] = 0xFF00; //Only 1 bytes to be modified at 0x3ECE //Lowest address in the highest order byte lane to match the byte ordering on the bus
	; i++ ; regAddr[i] = 0x3ED0;  regValue[i] = 0xE4F6; 
	; i++ ; regAddr[i] = 0x3ED2;  regValue[i] = 0x0146;
	; i++ ; regAddr[i] = 0x3ED4;  regValue[i] = 0x8F6C; 
	; i++ ; regAddr[i] = 0x3ED6;  regValue[i] = 0x66CC; 
	; i++ ; regAddr[i] = 0x3ED8;  regValue[i] = 0x8C42;
	; i++ ; regAddr[i] = 0x3EDA;  regValue[i] = 0x889B; 
	; i++ ; regAddr[i] = 0x3EDC;  regValue[i] = 0x8863; 
	; i++ ; regAddr[i] = 0x3EDE;  regValue[i] = 0xAA04;
	; i++ ; regAddr[i] = 0x3EE0;  regValue[i] = 0x15F0; 
	; i++ ; regAddr[i] = 0x3EE6;  regValue[i] = 0x008C; 
	; i++ ; regAddr[i] = 0x3EE8;  regValue[i] = 0x2024;
	; i++ ; regAddr[i] = 0x3EEA;  regValue[i] = 0xFF1F; 
	; i++ ; regAddr[i] = 0x3F06;  regValue[i] = 0x046A;
	; i++ ; regAddr[i] = 0x3088;  regValue[i] = 0x8000;	// SEQ_CTRL_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4A03;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4316;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0443;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1645;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x404B;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1244;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6134;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4A31;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4342;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4560;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2714;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3DFF;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3DFF;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3DEA;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2704;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3D10;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2705;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3D10;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2715;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3527;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x053D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4027;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0427;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x143D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFF3D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFF3D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xEA62;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2728;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x3627;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x083D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6444;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C2C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C2C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4B01;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x432D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4643;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1647;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x435F;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4F50;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2604;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2684;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2027;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFC53;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0D5C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0D57;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5417;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0955;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5649;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5307;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5302;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4D28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6C4C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0928;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x294E;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5C09;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0045;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8026;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xA627;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF817;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0227;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFA5C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0B17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1826;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xA25C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0317;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4427;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF25F;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2809;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1714;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2808;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1701;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4D1A;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2683;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1701;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x27FA;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x45A0;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1707;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x27FB;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1729;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4580;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1708;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x27FA;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1728;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5D17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0E26;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8153;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0117;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xE653;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0217;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1026;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8326;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x8248;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4D4E;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2809;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4C0B ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6017 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2027 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xF217 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x535F ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2808 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x164D ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1A17 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0127 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0xFA26 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x035C ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0145 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4027 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9817 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2A4A ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0A43 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x160B ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4327 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9C45 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6017 ;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0727;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9D17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2545;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0827;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x985D;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2645;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4B17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0A28;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0853;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0D52;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5112;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4460;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x184A;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0343;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1604;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4316;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5843;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1659;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4316;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x5A43;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x165B;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4327;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9C45;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x6017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x0727;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9D17;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2545;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4017;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x1027;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x9817;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2022;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x4B12;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x442C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C2C;	// SEQ_DATA_PORT
	; i++; regAddr[i] = 0x3086;  regValue[i] = 0x2C00;	// SEQ_DATA_PORT
	; i++;
	return i;
}

UInt AR0330_OTPMVsettingsV3()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x3ED2;  regValue[i] = 0x0146;  
	; i++ ; regAddr[i] = 0x3ED4;  regValue[i] = 0x8F6C;  
	; i++ ; regAddr[i] = 0x3ED6;  regValue[i] = 0x66CC;  
	; i++ ; regAddr[i] = 0x3ED8;  regValue[i] = 0x8C42;  
	; i++;
	return i;
}

UInt AR0330_OTPMVsettingsV4()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	 regAddr[i] = 0x3ED2;  regValue[i] = 0x0146;  
	; i++ ; regAddr[i] = 0x3ED6;  regValue[i] = 0x66CC;  
	; i++ ; regAddr[i] = 0x3ED8;  regValue[i] = 0x8C42; 
	; i++;
	return i;
}

UInt AR0330_OTPMVsettingsV5()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x3ED2;  regValue[i] = 0x0146; 
	; i++;
	return i;
}


/* PLL for 1 lane MIPI:                                         */
/* 1 x CLK_OP = 2 x CLK_PIX = Pixel Rate                        */
/* (max: 65 Mpixels/se for 12 bits and 76 Mpixel/s for 10 bits) */

UInt AR0330_setPLLRges1Lane()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x302C;  regValue[i] = 0x0004;		        // VT_SYS_CLK_DIV
	; i++ ; regAddr[i] = 0x302E;  regValue[i] = 0x0001;		// PRE_PLL_CLK_DIV
	; i++ ; regAddr[i] = 0x3030;  regValue[i] = 0x0020;		// PLL_MULTIPLIER
	;i++;

	return i;
}

/* PLL for 2 lane MIPI:                                         */
/* 2 x CLK_OP = 2 x CLK_PIX = Pixel Rate (max: 98 Mpixel/s)     */
UInt AR0330_setPLLRges2Lane()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x302C;  regValue[i] = 0x0002;      // VT_SYS_CLK_DIV
	; i++ ; regAddr[i] = 0x302E;  regValue[i] = 0x0004;		// PRE_PLL_CLK_DIV
	; i++ ; regAddr[i] = 0x3030;  regValue[i] = 0x0062;		// PLL_MULTIPLIER //62
	;i++;

	return i;
}

/* PLL for 4 lane MIPI:                                         */
/* 4 x CLK_OP = 2 x CLK_PIX = Pixel Rate (max: 196 Mpixel/s)    */
UInt AR0330_setPLLRges4Lane()
{
	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x302C;  regValue[i] = 0x0001;      // VT_SYS_CLK_DIV
	; i++ ; regAddr[i] = 0x302E;  regValue[i] = 0x0004;		// PRE_PLL_CLK_DIV
	; i++ ; regAddr[i] = 0x3030;  regValue[i] = 0x0049;		// PLL_MULTIPLIER //62
	;i++;

	return i;
}

UInt AR0330_SetOtherRegs()
{

	UInt i=0;
	Uint16 * regAddr=gAr0330_I2c_ctrl.regAddr;
	Uint16 * regValue=gAr0330_I2c_ctrl.regValue;

	regAddr[i] = 0x302A;  regValue[i] = 0x0006; 			// VT_PIX_CLK_DIV
	; i++ ; regAddr[i] = 0x3036;  regValue[i] = 0x000C;		// OP_PIX_CLK_DIV
	; i++ ; regAddr[i] = 0x3038;  regValue[i] = 0x0001;		// OP_SYS_CLK_DIV
	; i++ ; regAddr[i] = 0x31AC;  regValue[i] = 0x0C0C;		//DATA_FORMAT_BITS = 3084
	; i++ ; regAddr[i] = 0x31B0;  regValue[i] = 0x0028; 	// FRAME_PREAMBLE
	; i++ ; regAddr[i] = 0x31B2;  regValue[i] = 0x000E;		// LINE_PREAMBLE
	; i++ ; regAddr[i] = 0x31B4;  regValue[i] = 0x2743;		// MIPI_TIMING_0
	; i++ ; regAddr[i] = 0x31B6;  regValue[i] = 0x114E;		// MIPI_TIMING_1
	; i++ ; regAddr[i] = 0x31B8;  regValue[i] = 0x2049;		// MIPI_TIMING_2
	; i++ ; regAddr[i] = 0x31BA;  regValue[i] = 0x0186;		// MIPI_TIMING_3
	; i++ ; regAddr[i] = 0x31BC;  regValue[i] = 0x8005;		// MIPI_TIMING_4

	//[Timing_settings]		
	; i++ ; regAddr[i] = 0x31AE;  regValue[i] = 0x0204;		//SERIAL_FORMAT = 514
	; i++ ; regAddr[i] = 0x3002;  regValue[i] = 0x00EA;		//Y_ADDR_START = 234
	; i++ ; regAddr[i] = 0x3004;  regValue[i] = 0x00C6;		//X_ADDR_START = 198
	; i++ ; regAddr[i] = 0x3006;  regValue[i] = 0x0521;		//Y_ADDR_END = 1313
	; i++ ; regAddr[i] = 0x3008;  regValue[i] = 0x0845;		//X_ADDR_END = 2117
	; i++ ; regAddr[i] = 0x300A;  regValue[i] = 0x0508;		//FRAME_LENGTH_LINES = 1288
	; i++ ; regAddr[i] = 0x300C;  regValue[i] = 0x04DA;		//LINE_LENGTH_PCK = 1242
	; i++ ; regAddr[i] = 0x3012;  regValue[i] = 0x0284;		//COARSE_INTEGRATION_TIME = 644
	; i++ ; regAddr[i] = 0x3014;  regValue[i] = 0x0000;		//FINE_INTEGRATION_TIME = 0
	; i++ ; regAddr[i] = 0x30A2;  regValue[i] = 0x0001;		//X_ODD_INC = 1
	; i++ ; regAddr[i] = 0x30A6;  regValue[i] = 0x0001;		//Y_ODD_INC = 1
	; i++ ; regAddr[i] = 0x308C;  regValue[i] = 0x0006;		//Y_ADDR_START_CB = 6
	; i++ ; regAddr[i] = 0x308A;  regValue[i] = 0x0006;		//X_ADDR_START_CB = 6
	; i++ ; regAddr[i] = 0x3090;  regValue[i] = 0x0605;		//Y_ADDR_END_CB = 1541
	; i++ ; regAddr[i] = 0x308E;  regValue[i] = 0x0905;		//X_ADDR_END_CB = 2309
	; i++ ; regAddr[i] = 0x30AA;  regValue[i] = 0x0A04;		//FRAME_LENGTH_LINES_CB = 2564
	; i++ ; regAddr[i] = 0x303E;  regValue[i] = 0x04E0;		//LINE_LENGTH_PCK_CB = 1248
	; i++ ; regAddr[i] = 0x3016;  regValue[i] = 0x0A03;		//COARSE_INTEGRATION_TIME_CB = 2563
	; i++ ; regAddr[i] = 0x3018;  regValue[i] = 0x0000;		//FINE_INTEGRATION_TIME_CB = 0
	; i++ ; regAddr[i] = 0x30AE;  regValue[i] = 0x0001;		//X_ODD_INC_CB = 1
	; i++ ; regAddr[i] = 0x30A8;  regValue[i] = 0x0001;		//Y_ODD_INC_CB = 1
	; i++ ; regAddr[i] = 0x3040;  regValue[i] = 0x0000;		//READ_MODE = 0
	; i++ ; regAddr[i] = 0x3042;  regValue[i] = 0x0130;		//EXTRA_DELAY = 304
	; i++ ; regAddr[i] = 0x30BA;  regValue[i] = 0x002C;		//DIGITAL_CTRL = 44
	; i++ ; regAddr[i] = 0x3088;  regValue[i] = 0x80BA;		//SEQ_CTRL_PORT = 32954
	; i++ ; regAddr[i] = 0x3086;  regValue[i] = 0xE653;		//SEQ_DATA_PORT = 58963

	; i++ ; regAddr[i] = 0x3012;  regValue[i] = 0x051C;     // COARSE_INTEGRATION_TIME
	; i++ ; regAddr[i] = 0x3060;  regValue[i] = 0x7f7f;     // gain
	; i++;

	return i;
}

static UInt16 AR0330_GainTableMap(Uint32 again)
{	
   	if( again > AR0330_GAIN_MAX)
	{
		again=(AR0330_GAIN_MAX);
	} else if (again < 1)
	{
		again=0;
	}
	
	return AR0330_GAIN_TAB[again];
}




int Transplant_DRV_imgsSetEshutter()
{

  int status;
  Uint16 regAddr;
  Uint16 regValue;
//  printf("current eshutterInUsec:%d-----\n",eshutterInUsec); 
  Int32 devAddr = AR_0330_ADDR;

  regAddr = AR0330_COARSE_IT_TIME_A;
  
  status = Iss_Ar0330Lock();
  if (status == FVID2_SOK)
  {
	regValue = ti2a_output_params.sensorExposure/AR0330_ROW_TIME;
	status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr, &regValue, 1);
  }
  Iss_Ar0330Unlock();
   
  if(status!=FVID2_SOK) {
    Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                        __LINE__ );
    return status;
  }
  return status;
}

/* Control API that gets called when FVID2_control is called

  This API does handle level semaphore locking

  handle - AR0330 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Iss_Ar0330Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Iss_Ar0330Obj *pObj = ( Iss_Ar0330Obj * ) handle;
    Int32 status;
    
    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Iss_Ar0330LockObj ( pObj );

    switch ( cmd )
    {
        case FVID2_START:
       //     status = Iss_Ar0330Start ( pObj );
            break;

        case FVID2_STOP:
      //      status = Iss_Ar0330Stop ( pObj );
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
      //      status = Iss_Ar0330GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_ISS_SENSOR_RESET:
      //      status = Iss_Ar0330Reset ( pObj );
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
      //      status = Iss_Ar0330RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
     //       status = Iss_Ar0330RegRead ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
             status = Iss_Ar0330UpdateExpGain ( pObj, cmdArgs);
	      //Transplant_DRV_imgsSetEshutter();
        break;
         
		case IOCTL_ISS_SENSOR_UPDATE_FRAMERATE:
            {
                Ptr createArgs;
                Iss_CaptFrameRate *framerateParams = (Iss_CaptFrameRate *)cmdArgs;
                createArgs = &(framerateParams->FrameRate);
                status = Iss_Ar0330FrameRateSet(pObj, createArgs, cmdStatusArgs);
            }
            break;


        
		case IOCTL_ISS_SENSOR_UPDATE_ITT:
//            status = Iss_Ar0331UpdateItt(pObj, cmdArgs);
			status = FVID2_SOK;
            break;
			
        case IOCTL_ISS_SENSOR_PWM_CONFIG:
        {
        /*	Iss_SensorPwmParm *pPwmParm = (Iss_SensorPwmParm *)cmdArgs;
			if( pPwmParm->Id == ISS_SENSOR_PWM_VIDEO )
			{
        		Iss_Ar0331PwmVideoSet(pPwmParm->period, pPwmParm->duty);
        	}else if( pPwmParm->Id == ISS_SENSOR_PWM_DC ){
				Iss_Ar0331PwmDcSet(pPwmParm->period, pPwmParm->duty);
        	}
		*/
			status = FVID2_SOK;
        	break;
		}	
        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Ar0330UnlockObj ( pObj );

    return status;
}

void Iss_Ar0330EnableStreaming()
{
	int status = FVID2_SOK;
	/* Start streaming */
	gAr0330_I2c_ctrl.regAddr[0]  = RESET_REG_ADDR;
	gAr0330_I2c_ctrl.regValue[0] = 0x5C;
	status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,AR_0330_ADDR,&gAr0330_I2c_ctrl.regAddr[0],&gAr0330_I2c_ctrl.regValue[0],gAr0330_I2c_ctrl.numRegs);	
	if(status != FVID2_SOK)
	{
		Vps_rprintf("I2C write Error,index:%d\n",0);
		return;
	}	
	Task_sleep(100);
}

/*
  Create API that gets called when FVID2_create is called

  This API does not configure the AR0330is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  AR0330

  drvId - driver ID, must be FVID2_ISS_VID_DEC_AR0330_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Fdrv_Handle Iss_Ar0330Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
    Iss_Ar0330Obj *pObj;
    Iss_SensorCreateParams *sensorCreateArgs
        = ( Iss_SensorCreateParams * ) createArgs;

    Iss_SensorCreateStatus *sensorCreateStatus
        = ( Iss_SensorCreateStatus * ) createStatusArgs;

	Vps_printf ( "Iss_Ar0330Create entered........ \n" );
    /*
     * check parameters
     */
    if ( sensorCreateStatus == NULL )
        return NULL;

    sensorCreateStatus->retVal = FVID2_SOK;

    if ( drvId != FVID2_ISS_SENSOR_AR0330_DRV
         || instanceId != 0 || sensorCreateArgs == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    if ( sensorCreateArgs->deviceI2cInstId >= ISS_DEVICE_I2C_INST_ID_MAX )
    {
        sensorCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Iss_Ar0330AllocObj (  );
    if ( pObj == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

   Iss_Ar0330Lock (  );
	
		
		//Transplant_AR0330_HDR_Enable(0);
   Iss_Ar0330Unlock (  );
    /*
     * copy parameters to allocate driver handle
     */
    memcpy ( &pObj->createArgs, sensorCreateArgs,  sizeof ( *sensorCreateArgs ) );

    Iss_Ar0330ResetRegCache(pObj);

    /*
     * return driver handle object pointer
     */
    return pObj;
}

/*
  Delete function that is called when FVID2_delete is called

  This API
  - free's driver handle object

  handle - driver handle
  deleteArgs - NOT USED, set to NULL

*/
Int32 Iss_Ar0330Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Iss_Ar0330Obj *pObj = ( Iss_Ar0330Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Ar0330FreeObj ( pObj );

    return FVID2_SOK;
}



int Transplant_DRV_imgsSetRegs()
{
	int status = FVID2_SOK;

	gAr0330_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	Int32 devAddr = AR_0330_ADDR;
	UInt16 regAddr;
	UInt i;
	UInt j;

	Uint16 readRegAddr[20];
	memset(readRegAddr,0,20);

	Uint16 readRegValue[20];
	memset(readRegValue,0,20);

	gAr0330_I2c_ctrl.numRegs = 1;

	/* Reset sensor */
	gAr0330_I2c_ctrl.regAddr[0]  = RESET_REG_ADDR;
	gAr0330_I2c_ctrl.regValue[0] = RESET_REG_VAL;;
	status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&gAr0330_I2c_ctrl.regAddr[0],&gAr0330_I2c_ctrl.regValue[0],gAr0330_I2c_ctrl.numRegs);

	if(status != FVID2_SOK)
	{
		Vps_rprintf("I2C write Error,index:%d\n",0);
		return status;
	}	

	Task_sleep(100);// Every write to RESET_REG_ADDR is followed by 100ms of delay

	/* Get the Sensor/OTPM version number. Register and sequncer programming is */
	/* different for different revision of sensor/OTPM                          */
	regAddr = OTPM_VERSION_REG1;
	Iss_deviceRead16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr,&readRegValue[0],1);
	readRegValue[0] = readRegValue[0] >> 8; //Only uppper 8 bits are of interest 

	regAddr = OTPM_VERSION_REG2;
	Iss_deviceRead16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr,&readRegValue[1],1);

	regAddr = OTPM_VERSION_REG3;
	Iss_deviceRead16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr,&readRegValue[2],1);

	if(readRegValue[2] == 0x0000)
	{
		if((readRegValue[0] == 0x10) && (readRegValue[1] == 0x1200)) {i = AR0330_OTPMVsettingsV1();}
		else if(readRegValue[1] == 0x1208) {i = AR0330_OTPMVsettingsV2();}
		else   status = FVID2_EFAIL;
	}
	else
	{
		if((readRegValue[0] != 0x20) || (readRegValue[1] != 0x1208))
		{
			status = FVID2_EFAIL;
		}
		else
		{
			if(readRegValue[2] == 0x06)      {i = AR0330_OTPMVsettingsV3();}
			else if(readRegValue[2] == 0x07) {i = AR0330_OTPMVsettingsV4();}
			else if (readRegValue[2] == 0x08) {i = AR0330_OTPMVsettingsV5();}
			else status = FVID2_EFAIL;
		}
	}

	if(status != FVID2_SOK)
	{
		Vps_rprintf("Driver doesn't support this sesnor version\n");
		return status;
	}	

	for(j=0; j < i; j++)
	{
		//For revision 1 and 2, only bits 0-7 needs to be updated for 0x3ECE register address
		if(gAr0330_I2c_ctrl.regAddr[j] == 0x3ECE){
            Iss_deviceRead16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&gAr0330_I2c_ctrl.regAddr[j],&readRegValue[0],1);
			readRegValue[0] = readRegValue[0] & 0x00FF;
			gAr0330_I2c_ctrl.regValue[j] = readRegValue[0] | gAr0330_I2c_ctrl.regValue[j];
		}
		
		status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&gAr0330_I2c_ctrl.regAddr[j],&gAr0330_I2c_ctrl.regValue[j],gAr0330_I2c_ctrl.numRegs);

		if(status != FVID2_SOK)
		{
			Vps_rprintf("I2C write Error,index:%d\n",j);
			return status;
		}	
	}

	//PLL, timing and address settings
	i = AR0330_SetOtherRegs();
	for(j=0; j < i; j++)
	{
		status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&gAr0330_I2c_ctrl.regAddr[j],&gAr0330_I2c_ctrl.regValue[j],gAr0330_I2c_ctrl.numRegs);

		if(status != FVID2_SOK)
		{
			Vps_rprintf("I2C write Error,index:%d\n",j);
			return status;
		}	
	}

#ifdef MIPI_1_LANE
	//PLL settings for 1 lane 
	i = AR0330_setPLLRges1Lane();
#endif 

#ifdef MIPI_2_LANE
	//PLL settings for 2 lane 
	i = AR0330_setPLLRges2Lane();
#endif

#ifdef MIPI_4_LANE
	//PLL settings for 4 lane 
	i = AR0330_setPLLRges4Lane();
#endif //

	for(j=0; j < i; j++)
	{
		status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&gAr0330_I2c_ctrl.regAddr[j],&gAr0330_I2c_ctrl.regValue[j],gAr0330_I2c_ctrl.numRegs);

		if(status != FVID2_SOK)
		{
			Vps_rprintf("I2C write Error,index:%d\n",j);
			return status;
		}	
	}
	//printf("Finished Demo Init with AR0330\n");
	return FVID2_SOK;
}

int Transplant_AR0330_HDR_Enable(int enable)
{
	Uint16 regAddr[32];
	Uint16 regValue[32];
       int status = 0;
       int i = 0;
       int j = 0;
	   int delay = 100000;
	   gAr0330_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
		Int32 devAddr = AR_0330_ADDR;

if(enable)
{			/*rev1,32X*/
/*
		       regAddr[i] = 0x3082; regValue[i] = 0x000c; i++; 	
		       regAddr[i] = 0x31d0; regValue[i] = 0x0001; i++; 
		       regAddr[i] = 0x301a; regValue[i] = 0x10de; i++; 	
		       regAddr[i] = 0x318c; regValue[i] = 0xc001; i++; 
		       regAddr[i] = 0x31d0; regValue[i] = 0x0000; i++; 	
		       regAddr[i] = 0x2400; regValue[i] = 0x0002; i++; 
		       regAddr[i] = 0x3064; regValue[i] = 0x1882; i++; 
*/
			/*rev2 4x*/
			regAddr[i]= 0x3082;regValue[i]= 0x0004;i++;
			regAddr[i]= 0x305E;regValue[i]= 0x0040;i++;

			//Delay=60

			regAddr[i]= 0x318C; regValue[i]=0xC001;i++;
			regAddr[i]= 0x3198; regValue[i]=0x061E;i++;

			//[ALTM Enabled]
			 regAddr[i]=0x301A;  regValue[i]=0x10D8;i++;//stopping streaming
			 
			 regAddr[i]= 0x2442; regValue[i]=0x0080;i++;
			 regAddr[i]= 0x2444; regValue[i]=0x0000;i++;
			 regAddr[i]= 0x2446; regValue[i]=0x0004;i++;
			 regAddr[i]= 0x2440; regValue[i]=0x0002;i++;
			 regAddr[i]= 0x2438; regValue[i]=0x0010;i++;
			 regAddr[i]= 0x243A; regValue[i]=0x0020;i++;
			 regAddr[i]= 0x243C; regValue[i]=0x0000;i++;
			 regAddr[i]= 0x243E; regValue[i]=0x0200;i++;
			 regAddr[i]= 0x31D0; regValue[i]=0x0000;i++;
			 regAddr[i]= 0x301E; regValue[i]=0x0000;i++;
			/*LOAD= ALTM Enabled Devware Color Setup*/
			 regAddr[i]= 0x2400; regValue[i]=0x0002;i++;
			 //regAddr[i]= 0x2400; regValue[i]=0x0000;i++;

			 regAddr[i]=0x301A;regValue[i]=0x10DC;i++;// Enable Streaming


			regAddr[i]= 0x30FE; regValue[i]=0x0000;i++;
			regAddr[i]= 0x31E0; regValue[i]=0x0200;i++;
			regAddr[i]= 0x320A; regValue[i]=0x0000;i++;
			regAddr[i]= 0x2450; regValue[i]=0x0000;i++;
			regAddr[i]= 0x301E; regValue[i]=0x0000;i++;
			regAddr[i]= 0x318A; regValue[i]=0x0E10;i++;
			regAddr[i]= 0x3064; regValue[i]=0x1982;i++;
			regAddr[i]= 0x3064; regValue[i]=0x1982;i++;

			regAddr[i]=0x301A;regValue[i]=0x10DE;i++;

			//[ADACD Enabled]
			regAddr[i]= 0x3202; regValue[i]=0x00A0;i++;
			regAddr[i]= 0x3206; regValue[i]=0x1810;i++;
			regAddr[i]= 0x3208; regValue[i]=0x2820;i++;
			regAddr[i]= 0x3200; regValue[i]=0x0002;i++;	
}
else
{
		       regAddr[i] = 0x3082; regValue[i] = 0x0009; i++; 	
		       regAddr[i] = 0x31d0; regValue[i] = 0x0000; i++; 
		       regAddr[i] = 0x301a; regValue[i] = 0x10dc; i++; 	
		       regAddr[i] = 0x318c; regValue[i] = 0x0000; i++; 
		       regAddr[i] = 0x31d0; regValue[i] = 0x0000; i++; 	
		       regAddr[i] = 0x2400; regValue[i] = 0x0003; i++; 
		       regAddr[i] = 0x3064; regValue[i] = 0x1082; i++; 
}

gAr0330_I2c_ctrl.numRegs = 1;
for(j=0; j < i; j++)
{
	status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr[j],&regValue[j],gAr0330_I2c_ctrl.numRegs);
   
	if(status != FVID2_SOK)
	{
	   //printf("I2C write Error,index:%d\n",j);
	   return status;
	}	
	if(regAddr[j] == 0x301A)
	{
		while(delay--){}
		delay = 100000;
	}

}


	return status;
}


/*
  System init for AR0330 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Iss_deviceInit()
*/


Int32 Iss_Ar0330Init (  )
{
    Semaphore_Params semParams;
    Int32 status = FVID2_SOK;
	//Int32 count, delay = 100000;
	//Int32 devAddr;
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gIss_Ar0330CommonObj, 0, sizeof ( gIss_Ar0330CommonObj ) );
    //DBG_PrintforTest(0x0331,"Now I am in Iss_Ar0330Init  !")
    //Vps_rprintf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ in Iss_Ar0330Init ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^! \n");

    /*
     * Create global AR0330 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Ar0330CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gIss_Ar0330CommonObj.lock == NULL )
        status = FVID2_EALLOC;


	Transplant_DRV_imgsSetRegs();
    if ( status == FVID2_SOK )
    {
        gIss_Ar0330CommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Iss_Ar0330Create;
        gIss_Ar0330CommonObj.fvidDrvOps.delete = Iss_Ar0330Delete;
        gIss_Ar0330CommonObj.fvidDrvOps.control = Iss_Ar0330Control;
        gIss_Ar0330CommonObj.fvidDrvOps.queue = NULL;
        gIss_Ar0330CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Ar0330CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Ar0330CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Ar0330CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_AR0330_DRV;

        status = FVID2_registerDriver ( &gIss_Ar0330CommonObj.fvidDrvOps );

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gIss_Ar0330CommonObj.lock );
        }
    }

    if ( status != FVID2_SOK )
    {
        Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__, __LINE__ );
    }

    return status;
}

/*
  System de-init for AR0330 driver

  This API
  - de-registers driver from FVID2 sub-system
  - delete's allocated semaphore locks
  - gets called as part of Iss_deviceDeInit()
*/
Int32 Iss_Ar0330DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gIss_Ar0330CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gIss_Ar0330CommonObj.lock );

    return 0;
}

Int32 Iss_Ar0330PinMux (  )
{
	/* setup CAM input pin mux */
	*PINCNTL156 = 0x00050002;				// select function 2 with receiver enabled and pullup/down disabled  - only works in supervisor mode
	*PINCNTL157 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL158 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL159 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL160 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL161 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL162 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL163 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL164 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL165 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL166 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL167 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL168 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL169 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL170 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL171 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL172 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL173 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL174 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL175 = 0x00050002;				// select function 2  - only works in supervisor mode

    #if defined(TI_8107_BUILD)
    *PINCNTL74 = 0xE0040;   /* i2c2_scl_mux0 */
    *PINCNTL75 = 0xE0040;   /* i2c2_sda_mux0 */
    *PINCNTL135 = 0xE0040;   /* i2c2_scl_mux0 */
    *PINCNTL136 = 0xE0040;   /* i2c2_sda_mux0 */
    #endif
    #if defined(TI_814X_BUILD)
    *PINCNTL74 = 0x00020;
    *PINCNTL75 = 0x00020;
    #endif
	/*
    *PINCNTL85 = 0x00000080;
    *PINCNTL86 = 0x00000080;
    *PINCNTL88 = 0x00000080;

    *GIO_INPUT_OUTPUT_DIR &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));
    *GIO_ENABLE_DISABLE_WAKEUP &= ~((0x1 << 16) + (0x1 << 17) + (0x1 << 26));
	*/
	return 0;
}
/*
  Handle level lock
*/
Int32 Iss_Ar0330LockObj ( Iss_Ar0330Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Handle level unlock
*/
Int32 Iss_Ar0330UnlockObj ( Iss_Ar0330Obj * pObj )
{
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 Iss_Ar0330Lock (  )
{
    Semaphore_pend ( gIss_Ar0330CommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
Int32 Iss_Ar0330Unlock (  )
{
    Semaphore_post ( gIss_Ar0330CommonObj.lock );

    return FVID2_SOK;
}

/*
  Allocate driver object

  Searches in list of driver handles and allocate's a 'NOT IN USE' handle
  Also create's handle level semaphore lock

  return NULL in case handle could not be allocated
*/
Iss_Ar0330Obj *Iss_Ar0330AllocObj (  )
{
    UInt32 handleId;
    Iss_Ar0330Obj *pObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Ar0330Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++ )
    {

        pObj = &gIss_Ar0330CommonObj.Ar0330Obj[handleId];

        if ( pObj->state == ISS_AR0330_OBJ_STATE_UNUSED )
        {
            /*
             * free object found
             */

            /*
             * init to 0's
             */
            memset ( pObj, 0, sizeof ( *pObj ) );

            /*
             * init state and handle ID
             */
            pObj->state = ISS_AR0330_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /*
             * create driver object specific semaphore lock
             */
            Semaphore_Params_init ( &semParams );

            semParams.mode = Semaphore_Mode_BINARY;

            pObj->lock = Semaphore_create ( 1u, &semParams, NULL );

            found = TRUE;

            if ( pObj->lock == NULL )
            {
                /*
                 * Error - release object
                 */
                found = FALSE;
                pObj->state = ISS_AR0330_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Iss_Ar0330Unlock (  );

    if ( found )
        return pObj;    /* Free object found return it */

    /*
     * free object not found, return NULL
     */
    return NULL;
}

/*
  De-Allocate driver object

  Marks handle as 'NOT IN USE'
  Also delete's handle level semaphore lock
*/
Int32 Iss_Ar0330FreeObj ( Iss_Ar0330Obj * pObj )
{
    /*
     * take global lock
     */
    Iss_Ar0330Lock (  );

    if ( pObj->state != ISS_AR0330_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unused
         */
        pObj->state = ISS_AR0330_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Iss_Ar0330Unlock (  );

    return FVID2_SOK;
}

double loc_Ar0330RealRowTime =AR0330_ROW_TIME ;
Int32 Iss_Ar0330FrameRateSet(Iss_Ar0330Obj * pObj, Ptr createArgs,
                              Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;

    UInt32 i2cInstId = Iss_platformGetI2cInstId();

    Int32 devAddr = AR_0330_ADDR;

    Int32 count = 0;

    Int32 framerate = *(Int32 *) createArgs;

    gAr0330_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
    devAddr = AR_0330_ADDR;

	if (framerate == 60)
    {
        gTi2aControlParams.maxExposure=16667;
        loc_Ar0330RealRowTime=AR0330_ROW_TIME/2.0;
        gAr0330_I2c_ctrl.regAddr[count] = 0x3030;
        gAr0330_I2c_ctrl.regValue[count] = PLL_MULTIPLIER ;
        count++;
        //*(Int32 *) cmdStatusArgs = (1100 *PLL_MULTIPLIER)/0x4a;
    }
    else if (framerate == 30 )
    {
        loc_Ar0330RealRowTime=AR0330_ROW_TIME;
        gTi2aControlParams.maxExposure=33333;
        gAr0330_I2c_ctrl.regAddr[count] = 0x3030;
        gAr0330_I2c_ctrl.regValue[count] = PLL_MULTIPLIER/2;
        count++;
        //*(Int32 *) cmdStatusArgs = (2200 *PLL_MULTIPLIER)/0x4a;
		gTi2aControlParams.update |= TI2A_UPDATE_CONTROL_PARAMS_2A;
    }
    else if (framerate == 15)
    {
        gTi2aControlParams.maxExposure=16667*12/5;
        loc_Ar0330RealRowTime=AR0330_ROW_TIME*2;
        gAr0330_I2c_ctrl.regAddr[count] = 0x3030;
        gAr0330_I2c_ctrl.regValue[count] = PLL_MULTIPLIER/4;
        count++;
        //*(Int32 *) cmdStatusArgs = (2640 *PLL_MULTIPLIER)/0x4a;
    }

    else
    {
        status = FVID2_EFAIL;
        return status;
    }

	gTi2aControlParams.update |= TI2A_UPDATE_CONTROL_PARAMS_2A;
    /** take global lock **/
    Iss_Ar0330Lock();

    gAr0330_I2c_ctrl.numRegs = count;
    status =
        Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId, devAddr,
                          &gAr0330_I2c_ctrl.regAddr[0],
                          &gAr0330_I2c_ctrl.regValue[0],
                          gAr0330_I2c_ctrl.numRegs);

  /** release global lock **/
    Iss_Ar0330Unlock();

    return status;
}


/*
  Update exposure and gain value from the 2A
*/
Int32 Iss_Ar0330UpdateExpGain ( Iss_Ar0330Obj * pObj, Ptr createArgs )
{
	//DBG_PrintforTest(0x,"Iss_Ar0330UpdateExpGain() &&&&&&&&&&&&&&&&&&&&&&&!");
	Int32 status = FVID2_SOK;
	Int32 exp_time_rows;
	Int32 exp_time_rows_max;
	UInt16 g_int=0;
	//Int32 c_int;
	//Int32 f_int;
	//Int32 PixelClock;
	//Int32 LineLength;
	UInt32 i2cInstId = Iss_platformGetI2cInstId();
	Int32 devAddr = AR_0330_ADDR;
	//Int32 count = 0;
	//int thedelay=10000;
	//static int indexReg=0;

	//Int16 regsVtoWrite[1024];
	//Int16 regsVread[1024];

	
    
	gAr0330_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	devAddr = AR_0330_ADDR;

	//PixelClock = 160;//AR_0330_PIXEL_CLOCK;
	//LineLength = 2177;
	exp_time_rows_max = 2200;//AR_0330_COARSE_INT_TIME_MAX;
	g_int =AR0330_GainTableMap(ti2a_output_params.sensorGain);

	//c_int = (ti2a_output_params.sensorExposure*PixelClock)/LineLength;  // exposure is in unit of 1/16 line
	//f_int = PixelClock - (c_int * LineLength)/(ti2a_output_params.sensorExposure);

	exp_time_rows = exp_time_rows_max;
	// clamp the calculated exposure time to its maximum value
	if( exp_time_rows > exp_time_rows_max )
	{
		exp_time_rows = exp_time_rows_max;
	}

	/*
     * take global lock
     */
    Iss_Ar0330Lock (  );
    {
	//Vps_printf (" windsorDBG: I am in Iss_Ar0330UpdateExpGain and after Iss_Ar0330Lock (  ) \r\n");
	UInt16 regAddr= AR0330_ANALOG_GAIN;
	//regAddr = AR0330_ANALOG_GAIN;
	status = Iss_deviceWrite16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr,&g_int,1);

	/*
	if(indexReg<1024)
	{

		indexReg++;
		
	}
	else
	{

		while (thedelay)
		{
			thedelay--;
		}
		
		thedelay=10000;

	
		g_int=0;
		status = Iss_deviceRead16(gAr0330_I2c_ctrl.i2cInstId,devAddr,&regAddr,&g_int,1);
		while (thedelay)
		{
			thedelay--;
		}
		
		thedelay=10000;

		
		indexReg=0;
		*/
	}

     /*
     * release global lock
     */
    Iss_Ar0330Unlock (  );

	return status;
}
