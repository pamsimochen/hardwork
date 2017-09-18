#ifndef __AIC26_H__
#define __AIC26_H__

//Page 0 Registers
#define AIC26_DAT_BAT1  0x80A0  //0x05
#define AIC26_DAT_BAT2  0x80C0  //0x06
#define AIC26_DAT_AUX   0x80E0  //0x07
#define AIC26_DAT_TMP1  0x8120  //0x09
#define AIC26_DAT_TMP2  0x8140  //0x0A

//Page 1 Registers
#define AIC26_ADC      0x8800  //0x00
#define AIC26_STA      0x8820  //0x01
#define AIC26_REF      0x8860  //0x03
#define AIC26_RST      0x8880  //0x04

//Page 2 Registers
#define AIC26_AUD_CNT1 0x9000  //0x00
#define AIC26_ADC_GAIN 0x9020  //0x01
#define AIC26_DAC_GAIN 0x9040  //0x02
#define AIC26_SIDETONE 0x9060  //0x03
#define AIC26_AUD_CNT2 0x9080  //0x04
#define AIC26_PWR_CNT  0x90A0  //0x05
#define AIC26_AUD_CNT3 0x90C0  //0x06
#define AIC26_BBFC_LN0 0x90E0  //0x07
#define AIC26_BBFC_LN1 0x9100  //0x08
#define AIC26_BBFC_LN2 0x9120  //0x09
#define AIC26_BBFC_LN3 0x9140  //0x0A
#define AIC26_BBFC_LN4 0x9160  //0x0B
#define AIC26_BBFC_LN5 0x9180  //0x0C
#define AIC26_BBFC_LD1 0x91A0  //0x0D
#define AIC26_BBFC_LD2 0x91C0  //0x0E
#define AIC26_BBFC_LD4 0x91E0  //0x0F
#define AIC26_BBFC_LD5 0x9200  //0x10
#define AIC26_BBFC_RN0 0x9220  //0x11
#define AIC26_BBFC_RN1 0x9240  //0x12
#define AIC26_BBFC_RN2 0x9260  //0x13
#define AIC26_BBFC_RN3 0x9280  //0x14
#define AIC26_BBFC_RN4 0x92A0  //0x15
#define AIC26_BBFC_RN5 0x92C0  //0x16
#define AIC26_BBFC_RD1 0x92E0  //0x17
#define AIC26_BBFC_RD2 0x9300  //0x18
#define AIC26_BBFC_RD4 0x9320  //0x19
#define AIC26_BBFC_RD5 0x9340  //0x1A
#define AIC26_PLL_PRG1 0x9360  //0x1B
#define AIC26_PLL_PRG2 0x9380  //0x1C
#define AIC26_AUD_CNT4 0x93A0  //0x1D
#define AIC26_AUD_CNT5 0x93C0  //0x1E

/*-------------------------------------------------------------------------------------------------
 *    Control for Touch-Screen
 *-------------------------------------------------------------------------------------------------*/
#define ADST_NORM         0x0000      //Touch screen ADC is in Normal Mode.
#define ADST_PWRDWN       0x4000      //Touch screen ADC power down.  Powerdown immediately.

#define ADSCM_NONE        0x0000      //No scan
#define ADSCM_BAT1        0x1800      //Only BAT1 is converted.
#define ADSCM_BAT2        0x1C00      //Only BAT2 is converted.
#define ADSCM_AUX         0x2000      //Only AUX is converted.
#define ADSCM_AUXSCAN     0x2400      //AUX is converted and continues to be converted until stop bit.
#define ADSCM_TEMP1SCAN   0x2800      //TEMP1 is converted and continues to be converted until stop bit.
#define ADSCM_PORTSCAN    0x2C00      //BAT1, BAT2, AUX inputs are converted.
#define ADSCM_TEMP2SCAN   0x3000      //TEMP2 is converted and continues to be converted until stop bit.

#define RESOL_12BIT       0x0000      //Resolution of Touchscreen ADC = 12 bit.
#define RESOL_8BIT        0x0100      //Resolution of Touchscreen ADC = 8  bit.
#define RESOL_10BIT       0x0200      //Resolution of Touchscreen ADC = 10 bit.

#define ADAVG_NONE        0x0000      //No Averaging done
#define ADAVG_4DA         0x0040      //4 or 5 data average based on filter selection
#define ADAVG_8DA         0x0080      //8 or 9 data average based on filter selection
#define ADAVG_16DA        0x00C0      //16 or 15 data average based on filter selection

#define ADCR_8MHZ         0x0000      //Conversion Rate = 8Mhz (valid for 8bit only)
#define ADCR_4MHZ         0x0010      //Conversion Rate = 4Mhz (valid for 8/10bit only)
#define ADCR_2MHZ         0x0020      //Conversion Rate = 2Mhz 
#define ADCR_1MHZ         0x0030      //Conversion Rate = 1Mhz

#define AVGFS_MEAN 		  0x0000      //Filter Touch screen data through Mean Filter
#define AVGFS_MEDIAN      0x0001      //Filter Touch screen data through Median Filter

/*-------------------------------------------------------------------------------------------------
 *    Control for Status Register (Interrupt Control)
 *-------------------------------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------
 *    Control for Reference
 *-------------------------------------------------------------------------------------------------*/
#define REF_VREFM_EXT		0x0000     //VREF is external
#define REF_VREFM_INT       0x0010     //VREF is internal

#define REF_RPWUDL_0        0x0000     //Reference powerup delay = 0us
#define REF_RPWUDL_100      0x0004     //Reference powerup delay = 100us
#define REF_RPWUDL_500      0x0008     //Reference powerup delay = 500us
#define REF_RPWUDL_1000     0x000C     //Reference powerup delay = 1000us

#define REF_RPWDN_0         0x0000     //Reference is powered up always.
#define REF_RPWDN_1         0x0002     //Reference is powered down between conversions.

#define REF_IREFV_125       0x0000     //Reference is VREF = 1.25V
#define REF_IREFV_250       0x0001     //Reference is VREF = 2.50V




//Smapling rate
#define AIC26_11K		0
#define AIC26_22K		1
#define AIC26_44K		2
#define AIC26_8K			3
#define AIC26_12K		4
#define AIC26_16K		5
#define AIC26_24K		6
#define AIC26_32K		7
#define AIC26_48K		8

#endif /* __AIC26_H__ */
