/*
 * Copyright (C) 2009, Texas Instruments, Incorporated
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <asm/arch/nand.h>
#include <linux/mtd/nand.h>
#include <miiphy.h>
#include <nand.h>
#include <spi.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <mmc.h>
#include <i2c.h>
#include <asm/arch/sys_proto.h>

#ifdef CONFIG_RTC_TPS65911
#include <rtc.h>
#endif
#ifdef CONFIG_CODEC_AIC26
#include <aic26.h>
#endif

typedef int		Bool;
#define TRUE		((Bool) 1)
#define FALSE	((Bool) 0)

#define E_FAIL 1
#define E_PASS	0

#define SPI_CAN_READ	(1)
#define TST_RS485_ON_PC	(1)

#define DEFAULT_NAME	"serial"
#ifdef CONFIG_DM385
#define RS485_NAME	"eserial2"
#else
#define RS485_NAME	"eserial1"
#endif

typedef enum{
	RS485_IN_DIR,
	RS485_OUT_DIR
}RS485_dir_t;

#ifdef CONFIG_CODEC_AIC26
Bool DRVfnAudio_AIC26RecvData(u16 spiAdr, u16 *spiVal);
#endif
#if defined(CONFIG_CODEC_AIC26) || defined(CONFIG_CODEC_AIC3104)
extern int Audio_HW_Reset(void);
#endif

extern void dss_pll_config(void);
extern void hdmi_pll_config(void);
extern void video0_pll_config(void);
extern void video1_pll_config(void);

struct spi_slave *slave = NULL;
static int gbM3Initialized = 0;
static int gbDSSInitialized = 0;

void SetRS485_Dir(RS485_dir_t dir)
{
	switch(dir){
		case RS485_IN_DIR:
#ifdef CONFIG_DM385
			__raw_writel((1<<12), 0x48032190);  //GP0_12 output low
#else
			__raw_writel((1<<1), 0x4804c190);  //GP1_1 output low
#endif
			break;
		case RS485_OUT_DIR:
#ifdef CONFIG_DM385
			__raw_writel((1<<12), 0x48032194);  //GP0_12 output high
#else
			__raw_writel((1<<1), 0x4804c194);  //GP1_1 output high
#endif
			break;
		default:
			fprintf(stderr, "Invalid parameter for %s\n", __func__);
			break;
	}
}

void ShowMainMenu(void)
{	
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n 1: Memory R/W Test"
			"\r\n 2: Ethernet Test"
			"\r\n 3: Audio bypass Test"
			"\r\n 4: RTC Test"
			"\r\n 5: GIO Test"
			"\r\n 6: LED Test"
			"\r\n 7: Output Test"
			"\r\n 8: Sensor Test"
			"\r\n 9: Misc menu"
			//"\r\n : Image Calibration"
			"\r\n 0: Exit"
			"\r\n"
			"\r\n Select No.: "
			);
}

void ShowRS485Menu(void)
{
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n 1: RS485 read test"
			"\r\n 2: RS485 write test"
			"\r\n 0: Exit to Main menu"
			"\r\n"
			"\r\n Select No.: "
			);
}

void ShowMiscMenu(void)
{
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n 1: SD/MMC Test"
			"\r\n 2: RS485 Test"
			"\r\n 3: WIFI Adapater Test"
			"\r\n 0: Exit to Main menu"
			"\r\n"
			"\r\n Select No.: "
			);
}

void ShowMemoryMenu(void)
{	
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n 1: DDR R/W Test"
			"\r\n 2: NAND R/W Test"
			"\r\n 0: Exit to Main menu"
			"\r\n"
			"\r\n Select No.: "
			);
}

void ShowOutputMenu(void)
{	
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n 1: HDMI Color bar Output Test"
			"\r\n 2: TV Color bar Output Test"
//			"\r\n 3: VideoM3 reset"
//			"\r\n 4: VideoM3 run"
			"\r\n 0: Exit to Main menu"
			"\r\n"
			"\r\n Select No.: "
			);
}

void ShowLEDMenu(void)
{	
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n  1: LED1 ON"
			"\r\n  2: LED1 OFF"
			"\r\n  0: Exit to Main menu"
			"\r\n"
			"\r\n Select No.: "
			);
}

void ShowGIOMenu(void)
{	
	printf(	"\r\n *******************************"
  			"\r\n  TI8148 Diagnostic Program v1.0.0"
			"\r\n *******************************"
			"\r\n  1: DIPSW Status"
			"\r\n  2: RESET SW Status"
			"\r\n  3: ALM-OUT Test"
			"\r\n  4: ALM-IN Status"
			"\r\n  5: ALM-RST Status"
			"\r\n  0: Exit to Main menu"
			"\r\n"
			"\r\n Select No.: "
			);
}

void hdmipinmux_cec_ddc(void)
{
	writel(0x60010,     0x481409B8);  /*hdmi_cec_mux0 pinmmr111[4] */
	writel(0xE0002,     0x48140934); /*hdmi_ddc_scl_mux0 pinmmr78[1]*/
	writel(0xE0002,     0x48140938);  /*hdmi_ddc_sda_mux0 pinmmr79[1] */
	writel(0x40010,     0x481409BC);  //hdmi_hpd_mux0 pinmmr112[4]
}

void configure_hdmi_phy(void)
{
	unsigned int temp , temp1;
	printf("\tinitialise hdmi phy  config... \n");


	*(unsigned int *)(0x481815B0) =  0x2;
	printf("\t48 Mhz Clock input to HDMI ie SDIO clock output from PRCM done... \n");

	writel(0x8, 0x46C00040);


	while((readl(0x46C00040) & 0x00000003) != 2);

	printf("\tHDMI PLL CONTROL MODULE IN on STAE DONE... \n");
	writel(0x4A, 0x46C00040);

	while((readl(0x46C00040) & 0x000000FF)  != 0x5A);


	writel(0x8A, 0x46C00040);
	printf("\tinse hdmi reg  config... \n");

	while((readl(0x46C00040) & 0xFF)  != 0xAA);
	printf("\tinitialise   config... \n");


	printf("\tHDMI PHY  IN on STAE DONE... \n");
	temp =   readl(0x46C00300);			/* Dummy read to PHY base to complete the SCP reset process HDMI_PHY_U_BAS*/


	temp = readl(0x46C00300) ;
	temp1 = ((temp & 0x3FFFFFFF) | 0x40000000);
	writel(temp1, 0x46C00300);

	temp = readl(0x46C0030C) ;
	temp1 = ((temp & 0x000FFFFF) | 0x85400000);
	writel(temp1, 0x46C0030C);


	writel(0xF0000000, 0x46C00304);

	printf("\tHDMI PHY  TMDS CLOCK ENABLE DONE ... \n");


	temp = 0;
	while(temp ++ < 20) {}

	printf("\tinitialise  hdmi phy  done  \n");
}

void configure_hdvenc_1080p60(void) 
{
	writel(0x4003A033,     0x48106000); 
	writel(0x003F0275,     0x48106004); 
	writel(0x1EA500BB,     0x48106008); 
	writel(0x1F9901C2,     0x4810600C); 
	writel(0x1FD71E67,     0x48106010); 
	writel(0x004001C2,     0x48106014); 
	writel(0x00200200,     0x48106018); 
	writel(0x1B6C0C77,     0x4810601C); 
	writel(0x1C0C0C30,     0x48106020); 
	writel(0x1C0C0C30,     0x48106024); 
	writel(0x84465898,     0x48106028); 
	writel(0x3F000028,     0x4810602C); 
	writel(0x587800BF,     0x48106030); 
	writel(0x00000460,     0x48106034); 
	writel(0x000C39E7,     0x48106038); 
	writel(0x58780118,     0x4810603C); 
	writel(0x0002A86D,     0x48106040); 
	writel(0x00438000,     0x48106044); 
	writel(0x05000000,     0x48106048); 
	writel(0x00003000,     0x4810604C); 
	writel(0x00000000,     0x48106050); 
	writel(0x58780110,     0x48106054); 
	writel(0x0002A86D,     0x48106058); 
	writel(0x00438000,     0x4810605c); 
	writel(0x05000000,     0x48106060); 
	writel(0x00003000,     0x48106064); 
	writel(0x00000000,     0x48106068); 
	writel(0x00000000,     0x4810606c);
}

void DSSClkEnable(void)
{
	int i;
	*(unsigned int *)0x48180E10 = 0;
	printf("\tPower Enabled.....  \n");
	/* Enabling DSS Clocks */
	*(unsigned int *)0x48180800 = 2 ;
	for(i = 0; i < 200; i++);
	*(unsigned int *)0x48180820 = 2 ;
	for(i = 0; i < 200; i++);
	*(unsigned int *)0x48180824 = 2 ;
	for(i = 0; i < 200; i++);


	printf("\tPRCM for DSS in Progress, Please wait.....  \n");
	while((*(unsigned int *)0x48180800 & 0x100) != 0x100) {
	}

	/*Deasserting resets */

	*(unsigned int *)0x48180E10 = 0;
	printf("\tPRCM for DSS Done Successfully.....  \n");

}

void ISS_A8_Enable(void)
{
	writel(0x2, 0x48180D00); //PM_ISP_PWRSTCTRL
	writel(0x3, 0x48180D10); //RM_ISP_RSTCTRL
	writel(0x2, 0x48180700) ; //CM_ISP_CLKSTCTRL
	writel(0x2, 0x48180720) ;  //CM_ISP_ISP_CLKCTRL
	writel(0x2, 0x48180724) ;  //CM_ISP_FDIF_CLKCTRL

	printf("\tPRCM for ISS Done Successfully.....  \n");
}

#define CM_DEFAULT_DUCATI_CLKSTCTRL	(PRCM_BASE + 0x0518)
#define RM_DEFAULT_RSTCTRL		(PRCM_BASE + 0x0B10)
#define CM_DEFAULT_DUCATI_CLKCTRL	(PRCM_BASE + 0x0574)
#define RM_DEFAULT_RSTST		(PRCM_BASE + 0x0B14)
#define DUCATI_BASE_ADDR	    	0x55020000
#define DUCATI_MMU_CFG			0x55080000


void DucatiClkEnable(void)
{
	printf("\tPRCM for DucatiSS is in Progress  \n");
	printf("\tTests Executed: %x \n", *(unsigned int *)CM_DEFAULT_DUCATI_CLKSTCTRL);

	writel(0xEF, RM_DEFAULT_RSTCTRL);			/*Enable the Ducati Logic*/

	writel(2, CM_DEFAULT_DUCATI_CLKSTCTRL);     /*Enable Power Domain Transition*/
	writel(2, CM_DEFAULT_DUCATI_CLKCTRL);       /*Enable Ducati Clocks*/


	/*Check CLKIN200TR & CLKINTR  are active*/
	while(((readl(CM_DEFAULT_DUCATI_CLKSTCTRL) & 0x300) >> 8) != 3) ;
	printf("\tClock is Active  \n");

	//----------------------writel(RM_DEFAULT_RSTCTRL,   0xEF); /*Enable the Ducati Logic*/
	//--writel(RM_DEFAULT_RSTCTRL,   0xE3); /*Enable the Ducati Logic*/
	printf("\tTests Executed: %x \n", *(unsigned int *)RM_DEFAULT_RSTCTRL);
	printf("\tTests Executed: %x \n", *(unsigned int *)RM_DEFAULT_RSTST);
	/*Write Ducate IRAM Boot Image */
	writel(0x10000, DUCATI_BASE_ADDR);
	writel(0x9, DUCATI_BASE_ADDR + 0x4);
	writel(0xE7FEE7FE, DUCATI_BASE_ADDR + 0x8);


	/*Large Page Translations */

	writel(0x40000000, DUCATI_MMU_CFG + 0x800);
	writel(0x80000000, DUCATI_MMU_CFG + 0x804);
	writel(0xC0000000, DUCATI_MMU_CFG + 0x808);
	writel(0x60000000, DUCATI_MMU_CFG + 0x80C);

	writel(0x40000000, DUCATI_MMU_CFG + 0x820);
	writel(0x80000000, DUCATI_MMU_CFG + 0x824);
	writel(0xC0000000, DUCATI_MMU_CFG + 0x828);
	writel(0x60000000, DUCATI_MMU_CFG + 0x82C);

	writel(0x00000007, DUCATI_MMU_CFG + 0x840);
	writel(0x000B0007, DUCATI_MMU_CFG + 0x844);
	writel(0x00030007, DUCATI_MMU_CFG + 0x848);
	writel(0x00020007, DUCATI_MMU_CFG + 0x84C);

	/*Medium Page*/

	writel(0x00300000, DUCATI_MMU_CFG + 0x860);
	writel(0x00400000, DUCATI_MMU_CFG + 0x864);

	writel(0x40300000, DUCATI_MMU_CFG + 0x8A0);
	writel(0x40400000, DUCATI_MMU_CFG + 0x8A4);

	writel(0x00000007, DUCATI_MMU_CFG + 0x8E0);
	writel(0x00020007, DUCATI_MMU_CFG + 0x8E4);
	/*Small Page*/

	writel(0x00000000, DUCATI_MMU_CFG + 0x920);
	writel(0x40000000, DUCATI_MMU_CFG + 0x924);
	writel(0x00004000, DUCATI_MMU_CFG + 0x928);
	writel(0x00008000, DUCATI_MMU_CFG + 0x92C);
	writel(0x0000C000, DUCATI_MMU_CFG + 0x930);

	writel(0x55020000, DUCATI_MMU_CFG + 0x9A0);
	writel(0x55080000, DUCATI_MMU_CFG + 0x9A4);
	writel(0x55024000, DUCATI_MMU_CFG + 0x9A8);
	writel(0x55028000, DUCATI_MMU_CFG + 0x9AC);
	writel(0x5502C000, DUCATI_MMU_CFG + 0x9B0);

	writel(0x0001000B, DUCATI_MMU_CFG + 0xA20);
	writel(0x0000000B, DUCATI_MMU_CFG + 0xA24);
	writel(0x00010007, DUCATI_MMU_CFG + 0xA28);
	writel(0x00000007, DUCATI_MMU_CFG + 0xA2C);
	writel(0x00000007, DUCATI_MMU_CFG + 0xA30);
	printf("\tMMU has been written  \n");

	writel(0xE3, RM_DEFAULT_RSTCTRL);           //0xE3

	/*Check for Ducati M3_0 & M3_1 out of Reset*/
	while(((readl(RM_DEFAULT_RSTST) & 0x1C)) != 0x1C) ;

	/*Check Module is in Functional Mode */
	while(((readl(CM_DEFAULT_DUCATI_CLKCTRL) & 0x30000) >> 16) != 0) ;

	printf("\tPRCM for DucatiSS is Done Successfully  \n");
}

Bool DDR_RangeVerify(int* pStart, int nBytes)
{
	volatile int *ddrSr;
	int cnt, temp, maxCount;
	int i = 0;

#if defined(PHYS_DRAM_2) && defined(PHYS_DRAM_2_SIZE)
#define MAX_PHY_ADDR (PHYS_DRAM_2+PHYS_DRAM_2_SIZE -0x1)
#else
#define MAX_PHY_ADDR (PHYS_DRAM_1+PHYS_DRAM_1_SIZE -0x1)
#endif

	/* Size must be divide by 4 */
	if(nBytes & 0x3)
		return FALSE;
	maxCount  = (nBytes >> 2);
	//write data to DDR free area
	ddrSr = pStart;
	for(cnt=0;cnt<maxCount;cnt++){
		if(ddrSr > (volatile int *)MAX_PHY_ADDR)
			return FALSE;
			
		if((cnt%0x10000)==0) {
			printf("w");
			i++;
			if(i >= 60) {
				i = 0;
				printf("\r\n");
			}
		}		
		*ddrSr = cnt;		
           	ddrSr++;
		
	}
	//verify DDR free area
	printf("\r\n");
	i = 0;
	ddrSr = pStart;
	for(cnt=0;cnt<maxCount;cnt++){
		if((cnt%0x10000)==0) {
			printf("r");
			i++;
			if(i >= 60) {
				i = 0;
				printf("\r\n");
			}
		}
		temp = *ddrSr;
		ddrSr++;		
		if(temp != cnt)
			return E_FAIL;
	}
	printf("\r\n");

	return E_PASS;

}
Bool DDR_Verify(void)
{
	Bool retVal;
	retVal = DDR_RangeVerify((int *)CONFIG_SYS_MEMTEST_START,
							CONFIG_SYS_MEMTEST_SIZE - (CONFIG_SYS_MEMTEST_START - PHYS_DRAM_1));
#if defined(PHYS_DRAM_2)
	if(retVal == E_PASS)
		retVal = DDR_RangeVerify((int *)PHYS_DRAM_2, CONFIG_SYS_MEMTEST_SIZE);
#endif

	return retVal;
}

#ifdef CONFIG_CODEC_AIC26
Bool DRVfnAudio_AIC26SendData(u16 spiAdr, u16 spiVal)
{
	u8 SendData[4];
	int i=0;
#if SPI_CAN_READ && defined(CHECK_VAL)
	u16 temp = 0xFFFF;
#endif
	unsigned long flags = SPI_XFER_BEGIN | SPI_XFER_END;
	int ret;
	/* Make command to write */
	SendData[i++] = ((spiAdr & 0x7FFF) >> 8);
	SendData[i++] = (spiAdr & 0x00FF);
	SendData[i++] = (spiVal >> 8);
	SendData[i++] = (spiVal & 0x00FF);
	/* SPI write */
	ret = spi_xfer(slave, i * 8, SendData, NULL, flags);
	if (ret) {
		printf("Failed to send command %04x: %d\n", spiAdr, ret);
	}
#if SPI_CAN_READ && defined(CHECK_VAL)
	DRVfnAudio_AIC26RecvData(spiAdr, &temp);
	if(temp != spiVal && AIC26_RST != spiAdr){
		printf("Failed to Set reg %04x\n", spiAdr);
		printf("Try to set %04x but get %04x\n", spiVal, temp);
	}
#endif
	return ret;
}

Bool DRVfnAudio_AIC26RecvData(u16 spiAdr, u16 *spiVal)
{
#if SPI_CAN_READ
	u8 TempData[2];
	int i = 0;
	int ret;
	TempData[i++] = (u8) ((spiAdr & 0xFF00) >> 8);
	TempData[i++] = (u8) (spiAdr & 0x00FF);
	/* SPI write */
	ret = spi_xfer(slave, i * 8, TempData, NULL, SPI_XFER_BEGIN);
	if (ret) {
		printf("Failed to send command %04x: %d\n", spiAdr, ret);
	}
	/* SPI read */
	ret = spi_xfer(slave, i * 8, NULL, TempData, SPI_XFER_END);
	if (ret) {
		printf("Failed to read %04x: %d\n", spiAdr, ret);
	}
	*spiVal = (TempData[0] << 8) | TempData[1];
	return ret;
#else
	return E_PASS;
#endif
}

Bool DRVfnAudio_AIC26_Read_TEST(void)
{
	u16 i;
	u16 temp = 0xFFFF;
	Bool ret = E_PASS;
	for(i = AIC26_DAT_BAT1;i <= AIC26_DAT_TMP2; i += 0x20){
		ret = DRVfnAudio_AIC26RecvData(i, &temp);
		if(ret != E_PASS)
			return ret;
		printf("AIC26 read reg %04x = %04x\n", i, temp);
	}
	for(i = AIC26_ADC;i <= AIC26_RST; i += 0x20){
		DRVfnAudio_AIC26RecvData(i, &temp);
		if(ret != E_PASS)
			return ret;
		printf("AIC26 read reg %04x = %04x\n", i, temp);
	}
	for(i = AIC26_AUD_CNT1;i <= AIC26_AUD_CNT5; i += 0x20){
		DRVfnAudio_AIC26RecvData(i, &temp);
		if(ret != E_PASS)
			return ret;
		printf("AIC26 read reg %04x = %04x\n", i, temp);
	}
	return ret;
}

Bool DRVfnAudio_SOFT_RESET(void)
{
	return DRVfnAudio_AIC26SendData(AIC26_RST, 0xBB00);	//Sam 2007 06 12 disable keyclick not use
}

Bool DRVfnAudio_AIC26_INIT(void)
{
	Bool ret = E_PASS;
#if SPI_CAN_READ
	u16 temp = 0xFFFF;
	int timeout = 100;
#endif
	ret = DRVfnAudio_SOFT_RESET();
	if(ret != E_PASS)
		return ret;
	udelay(5000);

	// Power down the VGND first
	ret = DRVfnAudio_AIC26SendData(AIC26_PWR_CNT, 0x2FC0);
	if(ret != E_PASS)
		return ret;

	ret = DRVfnAudio_AIC26SendData(AIC26_AUD_CNT3, 0x2808);	// master mode
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_AUD_CNT1, 0x0100);	// mic I/P, 16bit, DSP mode, ADC/DAC FS=Fsref/1=44.1kHz
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_PLL_PRG1, 0x921C);	// MCLK=24.576M PLL Enable, Q=2(not use), P=2, J=7, 44.1kHz
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_PLL_PRG2, 0x36B0);	// D=3500
	if(ret != E_PASS)
		return ret;
	// Keep Driver Pop Reduction settings at the default and power up headphone driver
	ret = DRVfnAudio_AIC26SendData(AIC26_AUD_CNT5, 0xFE04);
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_PWR_CNT, 0x3FC0);
	if(ret != E_PASS)
		return ret;

	// Power up Sidetone; but keep it muted
	ret = DRVfnAudio_AIC26SendData(AIC26_SIDETONE, 0xC580);	// Analog/Digital sidetone muted
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_PWR_CNT, 0x17C0);
	if(ret != E_PASS)
		return ret;

#if SPI_CAN_READ
	ret = DRVfnAudio_AIC26RecvData(AIC26_PWR_CNT, &temp);
	if(ret != E_PASS)
		return ret;
	while(temp != 0x17C0 && timeout){
		udelay(1000);
		timeout--;
		ret = DRVfnAudio_AIC26RecvData(AIC26_PWR_CNT, &temp);
		if(ret != E_PASS)
			return ret;
	}
	if(timeout == 0){
		printf("Error on AIC26 power control\n");
		return E_FAIL;
	}
#else
	udelay(1000);
#endif

	// Enable DAC pop reduction features, and set it to slow and long options
	ret = DRVfnAudio_AIC26SendData(AIC26_AUD_CNT4, 0x0030);
	if(ret != E_PASS)
		return ret;

	// Power up DAC and then un-mute both channles
	ret = DRVfnAudio_AIC26SendData(AIC26_DAC_GAIN, 0xFFFF);
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_PWR_CNT, 0x1380);
	if(ret != E_PASS)
		return ret;
#if SPI_CAN_READ && defined(CHECK_VAL)
	ret = DRVfnAudio_AIC26RecvData(AIC26_PWR_CNT, &temp);
	if(ret != E_PASS)
		return ret;
	while(temp != 0x1380){
		udelay(1000);
		ret = DRVfnAudio_AIC26RecvData(AIC26_PWR_CNT, &temp);
		if(ret != E_PASS)
			return ret;
	}
	printf("0x1380 check OK\n");
#endif
	ret = DRVfnAudio_AIC26SendData(AIC26_ADC_GAIN, 0x0000);	// DAC both channles un-muted Sam Added
	return ret;
}

Bool DRVfnAudio_AIC26_BYPASS_TEST(void)
{
	Bool ret = E_PASS;
#if SPI_CAN_READ && defined(CHECK_VAL)
	u16 temp = 0xFFFF;
#endif
	ret = DRVfnAudio_AIC26SendData(AIC26_AUD_CNT1, (0x0100|(0x0 << 12)));		// Single-ended input MIC
	if(ret != E_PASS)
		return ret;
	ret = DRVfnAudio_AIC26SendData(AIC26_PWR_CNT, 0x1100);		// turn on sidetone power
	if(ret != E_PASS)
		return ret;
#if SPI_CAN_READ && defined(CHECK_VAL)
	ret = DRVfnAudio_AIC26RecvData(AIC26_PWR_CNT, &temp);
	if(ret != E_PASS)
		return ret;
	while(temp != 0x1100){
		udelay(1000);
		ret = DRVfnAudio_AIC26RecvData(AIC26_PWR_CNT, &temp);
		if(ret != E_PASS)
			return ret;
	}
#endif
	ret = DRVfnAudio_AIC26SendData(AIC26_SIDETONE, 0x4581);
//	ret = DRVfnAudio_AIC26SendData(AIC26_SIDETONE, 0x1D81);
	return ret;
}

Bool Aic26_test(void)
{
	Bool retVal = E_PASS;
	/* use rising latch to send command, but falling latch for receiving data */
	printf("%s\n", __func__);
	if(slave == NULL){
		slave = (struct spi_slave *)spi_setup_slave(0, 0, (100*1000), 1);
		if(slave == NULL)
			return E_FAIL;
	}
	retVal = spi_claim_bus(slave);
	if(retVal != E_PASS)
		return retVal;
	retVal = Audio_HW_Reset();
	if(retVal != E_PASS){
		printf("Hardware reset fail\n");
		return retVal;
	}
#if SPI_CAN_READ
	printf("Before config ------------------------------------------------\n");
	retVal = DRVfnAudio_AIC26_Read_TEST();
	if(retVal != E_PASS){
		printf("Read data fail\n");
		return retVal;
	}
#endif
	retVal = DRVfnAudio_AIC26_INIT();
	if(retVal != E_PASS){
		printf("Aic26 initialize fail\n");
		return retVal;
	}

	retVal = DRVfnAudio_AIC26_BYPASS_TEST();
	if(retVal != E_PASS){
		printf("Aic26 bypass config fail.\n");
		return retVal;
	}

#if SPI_CAN_READ
	printf("After config -------------------------------------------------\n");
	retVal = DRVfnAudio_AIC26_Read_TEST();
	if(retVal != E_PASS){
		printf("Read data fail\n");
		return retVal;
	}else
#endif
		printf("Done with success.\n");

	return retVal;
}
#endif /* #ifdef CONFIG_CODEC_AIC26 */

#ifdef CONFIG_CODEC_AIC3104

#define AIC3104_BUS_NUM 0
#define AIC3104_SLAVE_ADDR 0x18

/* AIC3X register space */
#define AIC3X_CACHEREGNUM		103

/* Page select register */
#define AIC3X_PAGE_SELECT		0
/* Software reset register */
#define AIC3X_RESET			1
/* Codec Sample rate select register */
#define AIC3X_SAMPLE_RATE_SEL_REG	2
/* PLL progrramming register A */
#define AIC3X_PLL_PROGA_REG		3
/* PLL progrramming register B */
#define AIC3X_PLL_PROGB_REG		4
/* PLL progrramming register C */
#define AIC3X_PLL_PROGC_REG		5
/* PLL progrramming register D */
#define AIC3X_PLL_PROGD_REG		6
/* Codec datapath setup register */
#define AIC3X_CODEC_DATAPATH_REG	7
/* Audio serial data interface control register A */
#define AIC3X_ASD_INTF_CTRLA		8
/* Audio serial data interface control register B */
#define AIC3X_ASD_INTF_CTRLB		9
/* Audio serial data interface control register C */
#define AIC3X_ASD_INTF_CTRLC		10
/* Audio overflow status and PLL R value programming register */
#define AIC3X_OVRF_STATUS_AND_PLLR_REG	11
/* Audio codec digital filter control register */
#define AIC3X_CODEC_DFILT_CTRL		12
/* Headset/button press detection register */
#define AIC3X_HEADSET_DETECT_CTRL_A	13
#define AIC3X_HEADSET_DETECT_CTRL_B	14
/* ADC PGA Gain control registers */
#define LADC_VOL			15
#define RADC_VOL			16
/* MIC3 control registers */
#define MIC3LR_2_LADC_CTRL		17
#define MIC3LR_2_RADC_CTRL		18
/* Line1 Input control registers */
#define LINE1L_2_LADC_CTRL		19
#define LINE1R_2_LADC_CTRL		21
#define LINE1R_2_RADC_CTRL		22
#define LINE1L_2_RADC_CTRL		24
/* Line2 Input control registers */
#define LINE2L_2_LADC_CTRL		20
#define LINE2R_2_RADC_CTRL		23
/* MICBIAS Control Register */
#define MICBIAS_CTRL			25

/* AGC Control Registers A, B, C */
#define LAGC_CTRL_A			26
#define LAGC_CTRL_B			27
#define LAGC_CTRL_C			28
#define RAGC_CTRL_A			29
#define RAGC_CTRL_B			30
#define RAGC_CTRL_C			31

/* DAC Power and Left High Power Output control registers */
#define DAC_PWR				37
#define HPLCOM_CFG			37
/* Right High Power Output control registers */
#define HPRCOM_CFG			38
/* DAC Output Switching control registers */
#define DAC_LINE_MUX			41
/* High Power Output Driver Pop Reduction registers */
#define HPOUT_POP_REDUCTION		42
/* DAC Digital control registers */
#define LDAC_VOL			43
#define RDAC_VOL			44
/* Left High Power Output control registers */
#define LINE2L_2_HPLOUT_VOL		45
#define PGAL_2_HPLOUT_VOL		46
#define DACL1_2_HPLOUT_VOL		47
#define LINE2R_2_HPLOUT_VOL		48
#define PGAR_2_HPLOUT_VOL		49
#define DACR1_2_HPLOUT_VOL		50
#define HPLOUT_CTRL			51
/* Left High Power COM control registers */
#define LINE2L_2_HPLCOM_VOL		52
#define PGAL_2_HPLCOM_VOL		53
#define DACL1_2_HPLCOM_VOL		54
#define LINE2R_2_HPLCOM_VOL		55
#define PGAR_2_HPLCOM_VOL		56
#define DACR1_2_HPLCOM_VOL		57
#define HPLCOM_CTRL			58
/* Right High Power Output control registers */
#define LINE2L_2_HPROUT_VOL		59
#define PGAL_2_HPROUT_VOL		60
#define DACL1_2_HPROUT_VOL		61
#define LINE2R_2_HPROUT_VOL		62
#define PGAR_2_HPROUT_VOL		63
#define DACR1_2_HPROUT_VOL		64
#define HPROUT_CTRL			65
/* Right High Power COM control registers */
#define LINE2L_2_HPRCOM_VOL		66
#define PGAL_2_HPRCOM_VOL		67
#define DACL1_2_HPRCOM_VOL		68
#define LINE2R_2_HPRCOM_VOL		69
#define PGAR_2_HPRCOM_VOL		70
#define DACR1_2_HPRCOM_VOL		71
#define HPRCOM_CTRL			72
/* Mono Line Output Plus/Minus control registers */
#define LINE2L_2_MONOLOPM_VOL		73
#define PGAL_2_MONOLOPM_VOL		74
#define DACL1_2_MONOLOPM_VOL		75
#define LINE2R_2_MONOLOPM_VOL		76
#define PGAR_2_MONOLOPM_VOL		77
#define DACR1_2_MONOLOPM_VOL		78
#define MONOLOPM_CTRL			79
/* Class-D speaker driver on tlv320aic3007 */
#define CLASSD_CTRL			73
/* Left Line Output Plus/Minus control registers */
#define LINE2L_2_LLOPM_VOL		80
#define PGAL_2_LLOPM_VOL		81
#define DACL1_2_LLOPM_VOL		82
#define LINE2R_2_LLOPM_VOL		83
#define PGAR_2_LLOPM_VOL		84
#define DACR1_2_LLOPM_VOL		85
#define LLOPM_CTRL			86
/* Right Line Output Plus/Minus control registers */
#define LINE2L_2_RLOPM_VOL		87
#define PGAL_2_RLOPM_VOL		88
#define DACL1_2_RLOPM_VOL		89
#define LINE2R_2_RLOPM_VOL		90
#define PGAR_2_RLOPM_VOL		91
#define DACR1_2_RLOPM_VOL		92
#define RLOPM_CTRL			93
/* GPIO/IRQ registers */
#define AIC3X_STICKY_IRQ_FLAGS_REG	96
#define AIC3X_RT_IRQ_FLAGS_REG		97
#define AIC3X_GPIO1_REG			98
#define AIC3X_GPIO2_REG			99
#define AIC3X_GPIOA_REG			100
#define AIC3X_GPIOB_REG			101
/* Clock generation control register */
#define AIC3X_CLKGEN_CTRL_REG		102

/* Page select register bits */
#define PAGE0_SELECT		0
#define PAGE1_SELECT		1

/* Software reset register bits */
#define SOFT_RESET		0x80

/* Route bits */
#define ROUTE_ON		0x80

/* Mute bits */
#define UNMUTE			0x08
#define MUTE_ON			0x80

#define INVERT_VOL(val)   (0x7f - val)

/* Default output volume (inverted) */
#define DEFAULT_VOL     INVERT_VOL(0x50)

/* Default input volume */
#define DEFAULT_GAIN    0x30

Bool DRVfnAudio_AIC3104SendData(u8 addr, u8 val)
{
	int ret;
	int old_bus;

	old_bus = I2C_GET_BUS();
	if(old_bus != AIC3104_BUS_NUM)
		I2C_SET_BUS(AIC3104_BUS_NUM);
	ret = i2c_write(AIC3104_SLAVE_ADDR, addr, 0x1, &val, 0x1);
	if(ret != 0) {
		puts("Error writing AIC3104.\n");
	}
	if(old_bus != AIC3104_BUS_NUM)
		I2C_SET_BUS(old_bus);
	return ret;
}

Bool DRVfnAudio_AIC3104RecvData(u8 addr, u8 *buf, int length)
{
	int old_bus;

	old_bus = I2C_GET_BUS();
	if(old_bus != AIC3104_BUS_NUM)
		I2C_SET_BUS(AIC3104_BUS_NUM);
	if(i2c_read(AIC3104_SLAVE_ADDR, addr, 0x1, buf, length) != 0) {
		puts("Error reading AIC3104.\n");
		return -1;
	}
	if(old_bus != AIC3104_BUS_NUM)
		I2C_SET_BUS(old_bus);
	return 0;
}

Bool Aic3104_Init(void)
{
	Bool retVal = E_FAIL;
	u8 reg;
	printf("%s ... ", __func__);
	retVal = DRVfnAudio_AIC3104SendData(AIC3X_PAGE_SELECT, PAGE0_SELECT);
	if(E_PASS == retVal) {
		DRVfnAudio_AIC3104SendData(AIC3X_RESET, SOFT_RESET);
		udelay(1000);

		/* DAC default volume and mute */
		DRVfnAudio_AIC3104SendData(LDAC_VOL, DEFAULT_VOL | MUTE_ON);
		DRVfnAudio_AIC3104SendData(RDAC_VOL, DEFAULT_VOL | MUTE_ON);

		/* DAC to HP default volume and route to Output mixer */
		DRVfnAudio_AIC3104SendData(DACL1_2_HPLOUT_VOL, ROUTE_ON);
		DRVfnAudio_AIC3104SendData(DACR1_2_HPROUT_VOL, ROUTE_ON);
		/* Bias set to 2.5V */
		DRVfnAudio_AIC3104SendData(MICBIAS_CTRL, 0x80);
		DRVfnAudio_AIC3104SendData(DACL1_2_HPLCOM_VOL, DEFAULT_VOL | ROUTE_ON);
		DRVfnAudio_AIC3104SendData(DACR1_2_HPRCOM_VOL, DEFAULT_VOL | ROUTE_ON);
		/* DAC to Line Out default volume and route to Output mixer */
		DRVfnAudio_AIC3104SendData(DACL1_2_LLOPM_VOL, DEFAULT_VOL | ROUTE_ON);
		DRVfnAudio_AIC3104SendData(DACR1_2_RLOPM_VOL, DEFAULT_VOL | ROUTE_ON);

		/* unmute all outputs */
		DRVfnAudio_AIC3104RecvData(LLOPM_CTRL, &reg, 1);
		DRVfnAudio_AIC3104SendData(LLOPM_CTRL, reg | UNMUTE);
		DRVfnAudio_AIC3104RecvData(RLOPM_CTRL, &reg, 1);
		DRVfnAudio_AIC3104SendData(RLOPM_CTRL, reg | UNMUTE);
		DRVfnAudio_AIC3104RecvData(HPLOUT_CTRL, &reg, 1);
		DRVfnAudio_AIC3104SendData(HPLOUT_CTRL, reg | UNMUTE);
		DRVfnAudio_AIC3104RecvData(HPROUT_CTRL, &reg, 1);
		DRVfnAudio_AIC3104SendData(HPROUT_CTRL, reg | UNMUTE);
		DRVfnAudio_AIC3104RecvData(HPLCOM_CTRL, &reg, 1);
		DRVfnAudio_AIC3104SendData(HPLCOM_CTRL, reg | UNMUTE);
		DRVfnAudio_AIC3104RecvData(HPRCOM_CTRL, &reg, 1);
		DRVfnAudio_AIC3104SendData(HPRCOM_CTRL, reg | UNMUTE);

		/* ADC default volume and unmute */
		DRVfnAudio_AIC3104SendData(LADC_VOL, DEFAULT_GAIN);
		DRVfnAudio_AIC3104SendData(RADC_VOL, DEFAULT_GAIN);
		/* By default route Line1 to ADC PGA mixer */
		DRVfnAudio_AIC3104SendData(LINE1L_2_LADC_CTRL, 0x0);
		DRVfnAudio_AIC3104SendData(LINE1R_2_RADC_CTRL, 0x0);

		/* PGA to HP Bypass default volume, disconnect from Output Mixer */
		DRVfnAudio_AIC3104SendData(PGAL_2_HPLOUT_VOL, DEFAULT_VOL);
		DRVfnAudio_AIC3104SendData(PGAR_2_HPROUT_VOL, DEFAULT_VOL);
		DRVfnAudio_AIC3104SendData(PGAL_2_HPLCOM_VOL, DEFAULT_VOL);
		DRVfnAudio_AIC3104SendData(PGAR_2_HPRCOM_VOL, DEFAULT_VOL);
		/* PGA to Line Out default volume, disconnect from Output Mixer */
		DRVfnAudio_AIC3104SendData(PGAL_2_LLOPM_VOL, DEFAULT_VOL);
		DRVfnAudio_AIC3104SendData(PGAR_2_RLOPM_VOL, DEFAULT_VOL);
	}
	if(retVal != E_PASS)
		printf("fail\n");
	else
		printf("success\n");
	return retVal;
}


Bool Aic3104_BypassTest(void)
{
	Bool retVal = E_PASS;
	u8 nRegReadVal, i;
	printf("%s ... ", __func__);
	retVal |= DRVfnAudio_AIC3104SendData(LINE1L_2_LADC_CTRL, 0x78);
	retVal |= DRVfnAudio_AIC3104SendData(LINE1R_2_LADC_CTRL, 0x80);
	retVal |= DRVfnAudio_AIC3104SendData(LINE1R_2_RADC_CTRL, 0x80);
	retVal |= DRVfnAudio_AIC3104SendData(PGAR_2_HPLOUT_VOL, 0x80);
	retVal |= DRVfnAudio_AIC3104SendData(HPLOUT_CTRL, 0x0d);
	retVal |= DRVfnAudio_AIC3104SendData(PGAR_2_HPROUT_VOL, 0x80);
	retVal |= DRVfnAudio_AIC3104SendData(HPROUT_CTRL, 0x0d);

	if(retVal != E_PASS)
		printf("fail\n");
	else
		printf("success\n");
	printf("Register dump-------------------------------------------------\n");
	for(i = 0; i < AIC3X_CACHEREGNUM; i++) {
		DRVfnAudio_AIC3104RecvData(i, &nRegReadVal, 1);
		printf("%2x:%5x\n", i, nRegReadVal);
	}
	return retVal;
}

Bool Aic3104_test(void)
{
	Bool retVal = E_FAIL;
	printf("%s\n", __func__);
	retVal = Audio_HW_Reset();
	if(retVal == E_PASS) {
		retVal = Aic3104_Init();
	}
	if(retVal == E_PASS) {
		retVal = Aic3104_BypassTest();
	}
	return retVal;
}
#endif /* #ifdef CONFIG_CODEC_AIC3104 */


Bool Audio_test(void)
{
	Bool retVal = E_FAIL;
#ifdef CONFIG_CODEC_AIC26
#ifndef CONFIG_AUDIO_TEST
#define CONFIG_AUDIO_TEST
#endif
#ifdef CONFIG_DM385
		retVal = Aic26_test();
#else
	if(PG1_0 == get_cpu_rev()){
		printf("Chip 1.0 detected.\n");
		retVal = Aic26_test();
	}
#endif /* #ifdef CONFIG_DM385 */
#endif /* #ifdef CONFIG_CODEC_AIC26 */
#ifdef CONFIG_CODEC_AIC3104
#ifndef CONFIG_AUDIO_TEST
#define CONFIG_AUDIO_TEST
#endif
#ifdef CONFIG_DM385
		retVal = Aic3104_test();
#else
	if(PG2_1 == get_cpu_rev()){
		printf("Chip 2.1 detected.\n");
		retVal = Aic3104_test();
	}
#endif /* #ifdef CONFIG_DM385 */
#endif /* #ifdef CONFIG_CODEC_AIC3104 */
#ifndef CONFIG_AUDIO_TEST
	printf("Audio test not implement yet!!!\n");
	retVal = E_FAIL;
#endif
	return retVal;
}

Bool DDR_test(void)
{
	Bool ret = E_PASS, retVal = E_PASS;
	int CheckData;
	CheckData = 0;

	printf(" \r\n***Starting DDR R/W TEST*** \r\n");
	ret = DDR_Verify();
	if(ret == E_PASS)
		printf("**DDR Access Pass !**");
	else {
		printf("**DDR Access Error !**");
		return E_FAIL;
	}
	return retVal;
}

Bool eth_test(void)
{
	unsigned char j, start, end;
	unsigned int oui;
	unsigned char model;
	unsigned char rev;
	char *devname;
	devname = miiphy_get_current_dev();
	start=0;
	end=31;
	for (j = start; j <= end; j++) {
		if (miiphy_info (devname, j, &oui, &model, &rev) == 0) {			
			printf("PHY 0x%02X: "
				"OUI = 0x%04X, "
				"Model = 0x%02X, "
				"Rev = 0x%02X, "
				"%3dbase%s, %s\n",
				j, oui, model, rev,
				miiphy_speed (devname, j),
				miiphy_is_1000base_x (devname, j)
					? "X" : "T",
				(miiphy_duplex (devname, j) == FULL)
					? "FDX" : "HDX");
		}
	}

	return 0;
}
#ifdef CONFIG_RTC_TPS65911
Bool RTC_Verify(void)
{
	struct rtc_time tm;
	int rcode = 0;
	rcode = rtc_get (&tm);

		if (rcode) {
			printf("## Get date failed\n");
			return E_FAIL;
		}
		printf ("Date: %4d-%02d-%02d    Time: %2d:%02d:%02d\n",
			tm.tm_year, tm.tm_mon, tm.tm_mday,			
			tm.tm_hour, tm.tm_min, tm.tm_sec);

	return E_PASS;
}	
Bool RTC_test(void)
{
	Bool ret = E_PASS;
	printf("\r\n Starting RTC TEST.......");
	ret = RTC_Verify();
	if(ret == E_PASS)
		printf(" Pass !!");
	else {
		printf(" Fail !!");
		return E_FAIL;
	}
	return 0;
}
#else /* #ifdef CONFIG_RTC_TPS65911 */
Bool RTC_test(void)
{
	printf("RTC test not implement yet!!!\n");
	return E_FAIL;
}
#endif /* #ifdef CONFIG_RTC_TPS65911 */
Bool NAND_test(void)
{
	int ret = 0;
	u_char c_in = 0;
	//struct rtc_time tm_start,tm_end;
	nand_info_t *nand = &nand_info[nand_curr_device];
	if(!nand_info[nand_curr_device].name) {
		printf("ERROR: NAND Device not available\n");
		printf("\tUse # nand device to set current device.\n");
		return -1;
	}
	printf("WARNING: All data in MTD5:\"Free\" partition will be lost.\n");
	printf("make sure you want to continue <y/N>:");
	c_in = getc();putc(c_in);
	if (c_in != 'y') {
		puts("\ntest aborted.\n");
		return -1;
	}
#define NANDTEST_SIZE	(0x2800000)
	loff_t	offset	= 0xCEE0000; // MTD5
	size_t	length	= NANDTEST_SIZE;
	ulong	address	= 0x81000000; // Input / Output Address
	printf("\nNAND reading test %dMByte...\n", NANDTEST_SIZE >> 20);
	//rtc_get (&tm_start);
	ret = nand_read_skip_bad(nand, offset, &length, (u_char *)address);
	//rtc_get (&tm_end);
	//long timepass = (tm_end.tm_min - tm_start.tm_min) * 60 + (tm_end.tm_sec - tm_start.tm_sec);
	printf(" %zu bytes read: %s\n", length, ret ? "ERROR" : "OK");
	if (ret != 0) return ret;
	length	= NANDTEST_SIZE;
	printf("\nNAND writing test %dMByte...\n", NANDTEST_SIZE >> 20);
	ret = nand_write_skip_bad(nand, offset, &length, (u_char *)address);
	printf(" %zu bytes written: %s\n", length, ret ? "ERROR" : "OK");
	return ret;
}

void memory_test(void)
{
	char cmd = 57;

	while( cmd != 48) {
		ShowMemoryMenu();		
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				DDR_test();
				break;
			case '2':
				NAND_test();
				break;
			default:
				break;
		}
	}
}
void GIO_test(void)
{
	char cmd = 57;
	u32  add, val;
	int checkdata, i;
	while( cmd != 48) {
		ShowGIOMenu();		
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				add=0x481ae138;  		 	//GPIO_DATAIN Data Input Register
				val = __raw_readl(add);
				val &= (1<<10);    				//GP3_10-MODE0
				if(val)
					printf("\r\n  DIPSW1-2 : 2 - DC IRIS ");
				else 
					printf("\r\n  DIPSW1-2 : 1 - AES ");
				val =  __raw_readl(add);
				val &= (1<<11);    				//GP3_11-MODE1
				if(val)
					printf("\r\n  DIPSW1-2 : 4 - STATIC IP ");
				else 
					printf("\r\n  DIPSW3-4 : 3 - DHCP ");
				break;
			case '2':
				printf("\r\n  Please press RESET key to exit");
				checkdata=0;
				while(!checkdata){
					add=0x481ac138;  		 	//GPIO_DATAIN Data Input Register
					val = __raw_readl(add);
					val &= (1<<21);    				//GP2_21-RE_SETING
					checkdata=val;
					udelay(1000);
				}
				break;
			case '3':
				for(i=0; i<5; i++) {
#ifdef CONFIG_DM385
					__raw_writel((1<<13), 0x4803213c);  //GP0_13 output high
#else
					__raw_writel((1<<8), 0x481ae13c);  //GP3_8 output high
#endif
					udelay(300000);
#ifdef CONFIG_DM385
					__raw_writel((1<<13), 0x48032190);  //GP0_13 output low
#else
					__raw_writel((1<<8), 0x481ae190);  //GP3_8 output low
#endif
					udelay(300000);
				}
				break;
			case '4':
				add=0x481ae138;  		 	//GPIO_DATAIN Data Input Register
				val = __raw_readl(add);
				printf("\r\n  Please press ALM-IN key to exit");
				val = 1;
				while(val) {
					val = __raw_readl(add) & (1<<7);//GP3_7
					udelay(1000);
				}
				break;
			case '5':
				add=0x481ae138;  		 	//GPIO_DATAIN Data Input Register
				val = __raw_readl(add);
				printf("\r\n  Please press ALM-RST key to exit");
				val = 1;
				while(val) {
					val = __raw_readl(add) & (1<<9);//GP3_9
					udelay(1000);
				}
				break;
			default:
				break;
		}
	}
}
Bool HDMI_Colorbar_test(void)
{
	/* These code is copied from TI's gel */
	unsigned int temp , temp1;
//	DucatiClkEnable();
//	DSSClkEnable();
//	ISS_A8_Enable();
	writel(0x3, 0x48180324);   //192Mhz from usb pll divide by 4 to get 48Mhz ddc_cec clock
	writel(0x01031FFF, 0x48100100);
	writel(0xC010F, 0x48100114);
	writel(0xF, 0x48100118);
	*(unsigned int *)0x48100000 = 0x01000000 ;	 //enable dss start of frame interrupt
	printf("\t ****  CENTAURUS ALL ADPLL INIT IS COMPLETE  ******* \n");
	hdmipinmux_cec_ddc();
	printf("\t ****  hdmi pin mux  complete  ******* \n");
	temp = readl(0x46C00010) ;
	temp1 = ((temp & 0xFFFFFFFE) | 0x1);
	writel(temp1, 0x46C00300);
	temp = 0;
	while(temp ++ < 20) {}
	printf("\t ****  wrapper soft reset complete  ******* \n");
	configure_hdmi_phy();
	printf("\t ****  configure hdmi phy  complete  ******* \n");

	temp = readl(0x46C00070) ;
	temp1 = temp | 0x00000218;
	writel(temp1, 0x46C00070);
	printf("\t ****  cec clock divider config   complete  ******* \n");
	temp = readl(0x46C00044) ;
	temp1 = temp | 0x00001414;
	writel(temp1, 0x46C00044);
	printf("\t ****  wrapper debounce  config   complete  ******* \n");
	temp = readl(0x46C00050) ;
	temp1 = temp | 0x100;
	writel(temp1, 0x46C00050);
	printf("\t ****  packing mode configuration    complete  ******* \n");
	writel(0x0, 0x46C00080);
	printf("\t ****  disable audio    complete  ******* \n");
	writel(0x1, 0x46C00414);
	writel(0x1, 0x46C00424);
	printf("\t ****  release HDMI IP CORE reset and release power down of core complete  ******* \n");
	writel(0x0, 0x46C00524);
	printf("\t ****  video action  config of hdmi  complete  ******* \n");
	writel(0x7, 0x46C00420);
	printf("\t ****  config input data bus width done   ******* \n");
	writel(0x0, 0x46C00528);	//VID_MODE  CONFIG
	writel(0x1, 0x46C004CC);	//DATA ENABLE CNTRL
	writel(0x37, 0x46C00420);	//ENABLE VSYNC AND HSYNC
	writel(0x0, 0x46C004F8);	//iadjust config to enable vsync
	writel(0x10, 0x46C00520);	// csc is bt709
	writel(0x21, 0x46C009BC);	//enable hdmi

	writel(0x20, 0x46C00608);	//tmds_ctrl
	writel(0x0, 0x46C00904);	//disable n/cts of actrl
	writel(0x0, 0x46C00950);	//disable audio
	writel(0x0, 0x46C00414);	//   keep audio  operation in reset state
	printf("\t ****  configuring AVI INFOFRAME   ******* \n");
	writel(0x82, 0x46C00A00);
	writel(0x2, 0x46C00A04);
	writel(0xD, 0x46C00A08);
	writel(0x1, 0x46C00A10);
	writel(0xA0, 0x46C00A14);
	writel(0x8F, 0x46C00A1C);
	printf("\t ****  configuring AVI INFOFRAME done   ******* \n");
	writel(0x3, 0x46C00538);	//DISABLE DEEP COLOR MODE IN DC PACKET
	writel(0x10, 0x46C009C0);
	writel(0x3, 0x46C009F8);	//ENABLE AND REPEAT AVI INFOFRAM TRANSMISSON
	writel(0xF, 0x46C009FC);	//ENABLE AND REPEAT GENEERAL PACKET TRANSMISSION
	configure_hdvenc_1080p60();
	printf("\t ****  configuring hdvenc for 1080p60 complete   ******* \n");
	return E_PASS;
}
typedef struct {
    __u32 raddr;
    __u32 rvalue;
} reg_t;
static reg_t register_table[] = {
    { 0x48100114, 0x000C000F }, //CLKC Video Encoder Clock Select
    { 0x48100100, 0x01031BFF }, //CLKC Module Clock Enable
    //{ 0x48100108, 0x40000104 }, //CLKC Main Data Path Select
    //{ 0x4810010C, 0x40000000 }, //CLKC VIN0 Data Path Select
    { 0x48140670, 0x018800db }, //SD_DAC_CTRL_REG
    { 0x48105F50, 0x01000000 }, //SD_VENC DACSEL
    { 0x48105200, 0x03000000 }, //COMP Compositor Status
    { 0x48105210, 0x0000e420 }, //COMP SD Settings
    //{ 0x48105214, 0x04010040 }, //COMP Background Color
    //{ 0x48105E84, 0x4 }, //SD_VENC ECTL
    //{ 0x48105E10, 0x4 }, //SD_VENC POL
    { 0x48105E14, 0x00040000 }, //SD_VENC DTVS0
    { 0x48105E18, 0x00090005 }, //SD_VENC DTVS1
    { 0x48105E1C, 0x000c0006 }, //SD_VENC DTVS2
    { 0x48105e20, 0x00000008 }, //SD_VENC DTVS3
    { 0x48105e24, 0x200c000e }, //SD_VENC DTVS4
    { 0x48105e28, 0x069200f2 }, //SD_VENC DTVS5
    { 0x48105e2c, 0x02060024 }, //SD_VENC DTVS6
    { 0x48105e30, 0x02070025 }, //SD_VENC DTVS7
    { 0x48105E04, 0x15 }, //SD_VENC VMOD
    { 0x48100118, 0xF }, //CLKC Video Encoder Enable
};
Bool TV_Colorbar_test(void)
{
    unsigned int t_length,i;
    t_length = sizeof(register_table) / sizeof(reg_t);
    printk("===== table size: %d\n", t_length);
	//DSSClkEnable();
	for (i = 0; i < t_length; i++) {
	    writel(register_table[i].rvalue, register_table[i].raddr);
	}
	//writel(0x3, 0x48180324);   //192Mhz from usb pll divide by 4 to get 48Mhz ddc_cec clock
	//writel(0x000C000F, 0x48100114);
	//writel(0x01031BFF, 0x48100100);
	return E_PASS;
}
#define GENERAL_CONTROL_BASE   0x55020000
#define VIDEOM3_BOOT_ADDR       (GENERAL_CONTROL_BASE + 0x4)

#define ENTRYPOINT_OF_M3_CODE		(0x80003535)

Bool VideoM3Reset(void)
{
	/*Put ONLY Ducati M3_0 to Reset*/
	writel(readl(RM_DEFAULT_RSTCTRL) | 0x4, RM_DEFAULT_RSTCTRL);
	/* clear the status bit only if it is set*/
	if(readl(RM_DEFAULT_RSTST)  & 0x4)
		writel(0x4, RM_DEFAULT_RSTST);
	return E_PASS;
}

Bool VideoM3Run(void)
{
	/*Bring ONLY Ducati M3_0 out of Reset*/
	writel(readl(RM_DEFAULT_RSTCTRL) & (~0x4), RM_DEFAULT_RSTCTRL);
	/*Check for Ducati M3_0 out of Reset*/
	while((((readl(RM_DEFAULT_RSTST) & 0x04)) != 0x04) &&
	        (((readl(RM_DEFAULT_RSTST) & 0x14)) != 0x14) &&
	        (((readl(RM_DEFAULT_RSTST) & 0x1C)) != 0x1C)) ;
	/*Check Module is in Functional Mode */
	while(((readl(CM_DEFAULT_DUCATI_CLKCTRL) & 0x30000) >> 16) != 0) ;
	return E_PASS;
}

Bool SetVideoM3EntryPoint(unsigned long addr)
{
	/* copy _c_int00 addresses to 0x4th location of Ducati
	   Last bit at this mem location has to be explicitly set to 1 to
	   ensure that M3 is in THUMB mode*/
	writel(0x0, VIDEOM3_BOOT_ADDR);
	writel(addr | 0x1, VIDEOM3_BOOT_ADDR);
	return E_PASS;
}

typedef enum _script_cmd_type {
	WRITE,
	READ,
	DELAY,
	END_SCIRPT
} script_cmd_type;

typedef struct _write_param {
	unsigned int addr;
	unsigned int val;
} write_param_t;

typedef struct _read_param {
	unsigned int addr;
	unsigned short *pVal;
} read_param_t;

typedef struct _delay_param {
	unsigned int uSec;
} delay_param_t;

typedef union _script_param {
	write_param_t write_param;
	read_param_t read_param;
	delay_param_t delay_param;
} script_param_t;


typedef struct _script_type {
	script_cmd_type cmd;
	unsigned int para_base;
	unsigned int para2;
} script_type;

unsigned int gScriptReadVal;

#ifdef CONFIG_SENSOR_MT9J003
#define HISPI_CAM_I2C_BUS_NUM	0x2
#define HISPI_CAM_SLAVE_ADDR	(0x6C>>1)
#define SENSOR_ID	(0x2C01)

script_type script_list[] = {
	{READ, 0x0000, (unsigned int)&gScriptReadVal},
	//1. start_regs
	{WRITE, 0x0100, 0x0000},
	{DELAY, 1000},
	//2. pll_regs
	{WRITE, 0x0300, 0x3},
	{WRITE, 0x0302, 0x1},
	{WRITE, 0x0304, 0x1},//0x1 ==>160Mhz
	{WRITE, 0x0306, 48},//48
	{WRITE, 0x0308, 0x0C},
	{WRITE, 0x030A, 0x01},//0x1
	{DELAY, 1000},
	{WRITE, 0x0104, 0x01}, //Grouped Parameter Hold = 0x1
	//default_regs
	{WRITE, 0x3064, 0x805},//RESERVED_MFR_3064 = 0x805
	{WRITE, 0x3178, 0x0000},//Summing Procedure 1 = 0x0
	{WRITE, 0x3ED0, 0x1B24},//Summing Procedure 2 = 0x1B24
	{WRITE, 0x3EDC, 0xC3E4},//Summing Procedure 3 = 0xC3E4
	{WRITE, 0x3EE8, 0x0000}, //Summing Procedure 4 = 0x0
	{WRITE, 0x3E00, 0x0010},//Low Power On_Off = 0x10
	{WRITE, 0x3016, 0x111}, //Row Speed = 0x111
	{WRITE, 0x0344, 0x3D8},//Column Start = 0x3D8
	{WRITE, 0x0348, 0xB57},//Column End = 0xB57
	{WRITE, 0x0346, 0x34A},//Row Start = 0x34A
	{WRITE, 0x034A, 0x781},//Row End = 0x781
	{WRITE, 0x3040, 0x0041}, //Read Mode = 0x41
	{WRITE, 0x0400, 0x0000},//Scaling Mode = 0x0
	{WRITE, 0x0404, 0x0010}, //Scale_M = 0x10
	{WRITE, 0x034C, 0x780},//Output Width = 1920
	{WRITE, 0x034E, 0x438},//Output Height = 1080
	{WRITE, 0x0342, 0x0880},//Line Length = 0x880
	{WRITE, 0x0340, 0x04C9},//Frame Lines = 0x4C9
	{WRITE, 0x0202, 0x0010}, //Coarse Integration Time (OPTIONAL) = 0x10
	{WRITE, 0x3014, 0x03F2},//Fine Integration Time = 0x3F2
	{WRITE, 0x3010, 0x009C}, //Fine Correction = 0x9C
	{WRITE, 0x3018, 0x0000},//Extra Delay = 0x0
	{WRITE, 0x30D4, 0x1080},//Cols Dbl Sampling = 0x1080
	{WRITE, 0x306E, 0x90B0},//Scalar Re-sampling = 0x90B0
	{WRITE, 0x3070, 0x0000},// test_pattern_mode	0 : Normal operation: Generate output data from pixel array
	{WRITE, 0x31C6, 0x8400},// packetized SP. test mode off
	{WRITE, 0x3174, 0x8000},
	{WRITE, 0x3E40, 0xDC05},
	{WRITE, 0x3E42, 0x6E22},
	{WRITE, 0x3E44, 0xDC22},
	{WRITE, 0x3E46, 0xFF00},
	{WRITE, 0x3ED4, 0xF998},
	{WRITE, 0x3ED6, 0x9789},
	{WRITE, 0x3EDE, 0xE438},
	{WRITE, 0x3EE0, 0xA43F},
	//hispi_regs
	{WRITE, 0x31AE, 0x0304},
	{WRITE, 0x301A, 0x001C},
	//end_regs
	{WRITE, 0x0104, 0x00},
	{READ, 0x0100, (unsigned int)&gScriptReadVal},
	{WRITE, 0x0120, 0x00}, //0x120 , DGAIN
	{WRITE, 0x0204, 60}, //0x204 , AGAIN
	{WRITE, 0x0202, 0x400}, //Coarse Integration Time (OPTIONAL) = 0x10
	{END_SCIRPT}
};

Bool SensorRead(unsigned short addr, unsigned short *val)
{
	Bool ret;
	u8 dBuf[2];
	ret = i2c_read(HISPI_CAM_SLAVE_ADDR, addr, 2, dBuf, 2);
	if(ret == 0)
		*val = (dBuf[0] << 8) | dBuf[1];
	return ret;
}

Bool SensorWrite(unsigned short addr, unsigned short val)
{
	u8 dBuf[2];
	dBuf[0] = val >> 8;
	dBuf[1] = val & 0xFF;
	return i2c_write(HISPI_CAM_SLAVE_ADDR, addr, 2, dBuf, 2);
}

unsigned short GetSensorID(void)
{
	unsigned short nSensorId;
	if(SensorRead(0, &nSensorId))
		return E_FAIL;
	printf("ID = 0x%x\n", nSensorId);
	return nSensorId;
}

Bool ShowSensorInfo(void)
{
	unsigned short val1, val2;
	printf("Got sensor\n");
	SensorRead(2, &val1);
	SensorRead(0x31FE, &val2);
	if(val1 == 0x20) {
		if(val2 == 0x02) {
			printf("MT9J001\n");
		} else if(val2 == 0x32) {
			printf("MT9J003: (Engineering Sample, Mass Production)\n");
		}
	} else if(val1 == 0x30) {
		printf("MT9J003: (Original Samples)\n");
	}
//	printf("\n2:0x%x\n", val1);
//	printf("0x31FE: 0x%x\n", val2);
	return E_PASS;
}

#elif defined(CONFIG_SENSOR_AR0331)
#define HISPI_CAM_I2C_BUS_NUM	0x2
#define SENSOR_ID	(0x2602)
#define HISPI_CAM_SLAVE_ADDR	(0x20>>1)

script_type script_list[] = {
#if 0
	//[Parallel Linear 1080p30]
	{DELAY,200000},
	{WRITE, 0x301A, 0x0001}, 	// RESET_REGISTER 
	{WRITE, 0x301A, 0x10D8}, 	// RESET_REGISTER
	{DELAY,200000},
	// [AR0331 Linear sequencer load - 1.0]
	{WRITE, 0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{WRITE, 0x3086, 0x4A03}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0443}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1645}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4045}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5045}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x404B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6134}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4A31}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4342}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2714}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3DEA}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2704}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2705}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2715}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3527}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x053D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1045}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0427}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x143D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xEA62}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2728}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3627}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x083D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6444}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4B01}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x432D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4643}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1647}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x435F}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4F50}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2604}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2694}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFC53}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0D5C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0D57}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5417}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0955}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5649}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5307}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5303}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4D28}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6C4C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0928}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C28}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x294E}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C09}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4500}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4580}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x26B6}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27F8}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C0B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1718}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x26B2}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C03}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1744}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27F2}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2809}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1628}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x084D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x9316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFA45}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xA017}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0727}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFB17}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2945}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x8017}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0827}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFA17}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x285D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x170E}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2691}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5301}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1740}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5302}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2693}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2692}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x484D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4E28}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x094C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0B17}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5F27}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xF217}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1428}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0816}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4D1A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2603}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C01}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x172A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4A0A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0B43}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1725}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1708}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5D53}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0D26}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x455C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x014B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5251}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6018}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4A03}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0443}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1658}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5943}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x165A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5B43}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1725}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1720}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x224B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{DELAY,200000},
	{WRITE, 0x301A, 0x0058}, 	// RESET_REGISTER 			
	{WRITE, 0x30B0, 0x0000}, 	// DIGITAL_TEST 			
	{WRITE, 0x30BA, 0x06EC}, 	// DIGITAL_CTRL		
	{WRITE, 0x31AC, 0x0C0C}, 	// DATA_FORMAT_BITS
	{DELAY,200000},
	// [PLL_settings - Parallel]	
	{WRITE, 0x302A, 0x0006}, 	// VT_PIX_CLK_DIV 		
	{WRITE, 0x302C, 0x0001}, 	// VT_SYS_CLK_DIV 		
	{WRITE, 0x302E, 0x0004}, 	// PRE_PLL_CLK_DIV 		
	{WRITE, 0x3030, 0x004A}, 	// PLL_MULTIPLIER 		
	{WRITE, 0x3036, 0x000C}, 	// OP_PIX_CLK_DIV 		
	{WRITE, 0x3038, 0x0001}, 	// OP_SYS_CLK_DIV
	{DELAY,200000},
	// [Hispi Linear 1080p60]			
	{WRITE, 0x3002, 0x00E6}, 	// Y_ADDR_START 			
	{WRITE, 0x3004, 0x0042}, 	// X_ADDR_START 			
	{WRITE, 0x3006, 0x0521}, 	// Y_ADDR_END 			
	{WRITE, 0x3008, 0x07C9}, 	// X_ADDR_END 		
	{WRITE, 0x300A, 0x0461}, 	// FRAME_LENGTH_LINES 		
	{WRITE, 0x300C, 0x044C}, 	// LINE_LENGTH_PCK 	
	{WRITE, 0x3012, 0x045C}, 	// COARSE_INTEGRATION_TIME 			
	{WRITE, 0x30A2, 0x0001}, 	// X_ODD_INC 			
	{WRITE, 0x30A6, 0x0001}, 	// Y_ODD_INC 			
	{WRITE, 0x3040, 0x0000}, 	// READ_MODE 		
	{DELAY,200000},
	// [Linear Mode]
	{WRITE, 0x3082, 0x000D}, 	// OPERATION_MODE_CTRL
	{DELAY,200000},
	// [2D motion compensation OFF]			
	{WRITE, 0x318C, 0x0000}, 	// HDR_MC_CTRL2			
	{WRITE, 0x3190, 0x0000}, 	// HDR_MC_CTRL4			
	{WRITE, 0x301E, 0x00A8}, 	// DATA_PEDESTAL                             
	{WRITE, 0x30FE, 0x0080}, 	// RESERVED_MFR_30FE		
	{WRITE, 0x320A, 0x0080}, 	// ADACD_PEDESTAL
	{DELAY,200000},
	// [ALTM Bypassed]		
	{WRITE, 0x301A, 0x0058}, 	// RESET_REGISTER 			
	{WRITE, 0x2400, 0x0003}, 	// ALTM_CONTROL		
	{WRITE, 0x2450, 0x0000}, 	// ALTM_OUT_PEDESTAL			
	{WRITE, 0x301E, 0x00A8}, 	// DATA_PEDESTAL
	{DELAY,200000},
	{WRITE, 0x301A, 0x005C}, 	// RESET_REGISTER
	{DELAY,200000},
	// [ADACD Disabled]	
	{WRITE, 0x3200, 0x0000}, 	// ADACD_CONTROL
	{DELAY,200000},
	// [Companding Disabled]		
	{WRITE, 0x31D0, 0x0000}, 	// COMPANDING
	{DELAY,200000},
	{WRITE, 0x31E0, 0x0200}, 	// RESERVED_MFR_31E0			
	{WRITE, 0x3060, 0x0006}, 	// ANALOG_GAIN
	{DELAY,200000},
	// [Disable Embedded Data and Stats]
	{WRITE, 0x3064, 0x1802}, 	// SMIA_TEST	
	{WRITE, 0x3064, 0x1802}, 	// SMIA_TEST
	{DELAY,200000},
	{WRITE, 0x31AE, 0x0304}, 	// SERIAL_FORMAT
	{WRITE, 0x306E, 0x9010}, 	// DATAPATH_SELECT
	{DELAY,200000},
	// [Analog Settings]
	{WRITE, 0x3180, 0x8089}, 	// DELTA_DK_CONTROL
	{WRITE, 0x30F4, 0x4000}, 	// RESERVED_MFR_30F4
	{WRITE, 0x3ED4, 0x8F6C}, 	// RESERVED_MFR_3ED4
	{WRITE, 0x3ED6, 0x6666}, 	// RESERVED_MFR_3ED6
	{WRITE, 0x3EDA, 0x8899}, 	// RESERVED_MFR_3EDA
	{WRITE, 0x3EE6, 0x00F0}, 	// RESERVED_MFR_3EE6
	{DELAY,200000},
	{WRITE, 0x3ED2, 0x9F46}, 	// DAC_LD_6_7 		
	{WRITE, 0x301A, 0x005C}, 	// RESET_REGISTER
#else
//
//	Register Log created on Friday, December 30, 2011 : 11:17:36
//
//	EXTCLK=24Mhz, 59.8FPS, 1080P

//	[Register Log 12/30/11 11:17:24]
	{DELAY,200000},
	{WRITE, 0x301A, 0x0001}, 	// RESET_REGISTER
	{WRITE, 0x301A, 0x10D8}, 	// RESET_REGISTER
	{DELAY,200000},
//	[AR0331 Linear sequencer load - 1.0]
	{WRITE, 0x3088, 0x8000}, 	// SEQ_CTRL_PORT
	{WRITE, 0x3086, 0x4A03}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0443}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1645}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4045}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6017}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5045}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x404B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6134}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4A31}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4342}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2714}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3DFF}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3DEA}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2704}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2705}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3D10}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2715}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3527}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x053D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1045}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4027}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0427}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x143D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFF3D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xEA62}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2728}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x3627}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x083D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6444}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4B01}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x432D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4643}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1647}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x435F}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4F50}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2604}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2694}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2027}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFC53}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0D5C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0D57}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5417}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0955}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5649}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5307}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5303}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4D28}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6C4C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0928}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C28}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x294E}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C09}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4500}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4580}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x26B6}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27F8}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C0B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1718}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x26B2}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C03}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1744}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27F2}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2809}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1628}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x084D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1A26}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x9316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1627}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFA45}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xA017}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0727}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFB17}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2945}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x8017}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0827}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xFA17}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x285D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x170E}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2691}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5301}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1740}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5302}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2693}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2692}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x484D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4E28}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x094C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0B17}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5F27}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0xF217}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1428}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0816}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4D1A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1616}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x27FA}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2603}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5C01}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x172A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4A0A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0B43}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1725}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1708}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5D53}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0D26}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x455C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x014B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5251}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1702}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x6018}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4A03}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x0443}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1658}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5943}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x165A}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4316}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x5B43}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4560}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1707}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x279D}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1725}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x4540}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1710}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2798}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1720}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x224B}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x1244}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT
	{WRITE, 0x3086, 0x2C2C}, 	// SEQ_DATA_PORT

	{WRITE, 0x301A, 0x0058}, 	// RESET_REGISTER
	{WRITE, 0x30B0, 0x0000}, 	// DIGITAL_TEST
	{WRITE, 0x30BA, 0x06EC}, 	// DIGITAL_CTRL		
	{WRITE, 0x31AC, 0x0C0C},		//DATA_FORMAT_BITS = 3084

	{WRITE, 0x302A, 0x0006},		//VT_PIX_CLK_DIV = 6
	{WRITE, 0x302C, 0x0001},		//VT_SYS_CLK_DIV = 1
	{WRITE, 0x302E, 0x0004},		//PRE_PLL_CLK_DIV = 4
	{WRITE, 0x3030, 0x004A},		//PLL_MULTIPLIER = 74
	{WRITE, 0x3036, 0x000C},		//OP_PIX_CLK_DIV = 12
	{WRITE, 0x3038, 0x0001},		//OP_SYS_CLK_DIV = 1


	{WRITE, 0x31AE, 0x0304},		//SERIAL_FORMAT = 772
//	[hispiS (packet) protocol]
	{WRITE, 0x31C6, 0x8400}, 	// HISPI_CONTROL_STATUS
	{WRITE, 0x3002, 0x00E8},		//Y_ADDR_START = 232
	{WRITE, 0x3004, 0x0046},		//X_ADDR_START = 70
	{WRITE, 0x3006, 0x051F},		//Y_ADDR_END = 1311
	{WRITE, 0x3008, 0x07C5},		//X_ADDR_END = 1989
	{WRITE, 0x300A, 0x045D},		//FRAME_LENGTH_LINES = 1117
	{WRITE, 0x300C, 0x0453},		//LINE_LENGTH_PCK = 1107

	{WRITE, 0x30A2, 0x0001}, 	// X_ODD_INC
	{WRITE, 0x30A6, 0x0001}, 	// Y_ODD_INC
	{WRITE, 0x3040, 0x0000}, 	// READ_MODE
	{WRITE, 0x3082, 0x0009}, 	// OPERATION_MODE_CTRL

	{WRITE, 0x3012, 0x045A},	//COARSE_INTEGRATION_TIME = 1114
	{WRITE, 0x305E, 0x0080}, 	// GLOBAL_GAIN
	{DELAY,60000},
//	[2D motion compensation OFF]			
	{WRITE, 0x318C, 0x0000}, 	// HDR_MC_CTRL2			
	{WRITE, 0x3190, 0x0000}, 	// HDR_MC_CTRL4			
	{WRITE, 0x301E, 0x00A8}, 	// DATA_PEDESTAL                             
	{WRITE, 0x30FE, 0x0080}, 	// RESERVED_MFR_30FE		
	{WRITE, 0x320A, 0x0080}, 	// ADACD_PEDESTAL

//	[ALTM Bypassed]		
	{WRITE, 0x301A, 0x0058}, 	// RESET_REGISTER 			
	{WRITE, 0x2400, 0x0003}, 	// ALTM_CONTROL		
	{WRITE, 0x2450, 0x0000}, 	// ALTM_OUT_PEDESTAL			
	{WRITE, 0x301E, 0x00A8}, 	// DATA_PEDESTAL

	{WRITE, 0x301A, 0x005C}, 	// RESET_REGISTER

//	[ADACD Disabled]	
	{WRITE, 0x3200, 0x0000},		// ADACD_CONTROL

//	[Companding Disabled]		
	{WRITE, 0x31D0, 0x0000}, 	// COMPANDING

	{WRITE, 0x31E0, 0x0200}, 	// RESERVED_MFR_31E0			
	{WRITE, 0x3060, 0x0006}, 	// ANALOG_GAIN

//	[Enable Embedded Data and Stats]
	{WRITE, 0x3064, 0x1982}, 	// SMIA_TEST	
	{WRITE, 0x3064, 0x1982}, 	// SMIA_TEST
	{WRITE, 0x301A, 0x005E}, 	// RESET_REGISTER

//	[Analog Settings]
	{WRITE, 0x3180, 0x8089}, 	// DELTA_DK_CONTROL
	{WRITE, 0x30F4, 0x4000}, 	// RESERVED_MFR_30F4
	{WRITE, 0x3ED4, 0x8F6C}, 	// RESERVED_MFR_3ED4
	{WRITE, 0x3ED6, 0x6666}, 	// RESERVED_MFR_3ED6
	{WRITE, 0x3EDA, 0x8899}, 	// RESERVED_MFR_3EDA
	{WRITE, 0x3EE6, 0x00F0}, 	// RESERVED_MFR_3EE6

	{WRITE, 0x3ED2, 0x9F46}, 	// DAC_LD_6_7
	{WRITE, 0x301A, 0x005C}, 	// RESET_REGISTER



#endif
	{END_SCIRPT}
};

Bool SensorRead(unsigned short addr, unsigned short *val)
{
	Bool ret;
	u8 dBuf[2];
	ret = i2c_read(HISPI_CAM_SLAVE_ADDR, addr, 2, dBuf, 2);
	if(ret == 0)
		*val = (dBuf[0] << 8) | dBuf[1];
	return ret;
}

Bool SensorWrite(unsigned short addr, unsigned short val)
{
	u8 dBuf[2];
	dBuf[0] = val >> 8;
	dBuf[1] = val & 0xFF;
	return i2c_write(HISPI_CAM_SLAVE_ADDR, addr, 2, dBuf, 2);
}

unsigned short GetSensorID(void)
{
	unsigned short nSensorId;
	if(SensorRead(0x3000, &nSensorId))
		return E_FAIL;
	printf("ID = 0x%x\n", nSensorId);
	return nSensorId;
}

#else
script_type script_list[] = {
	{END_SCIRPT}
};

#define SensorWrite(reg, val) E_FAIL
#define SensorRead(reg, val) E_FAIL
#endif

Bool SensorInit(void)
{
	Bool ret = E_FAIL, done = 0;
	int i, len = ARRAY_SIZE(script_list);
	script_param_t *pParam;
	for(i = 0; i < len && !done; i++) {
		pParam = (script_param_t *)&script_list[i].para_base;
		switch(script_list[i].cmd) {
			case WRITE:
				ret = SensorWrite(pParam->write_param.addr, pParam->write_param.val);
				printf("Write 0x%04x : 0x%04x\n", pParam->write_param.addr, pParam->write_param.val);
				break;
			case READ:
				ret = SensorRead(pParam->read_param.addr, pParam->read_param.pVal);
				printf("Read 0x%04x : 0x%04x\n", pParam->read_param.addr, gScriptReadVal);
				break;
			case DELAY:
				printf("DELAY %d usec\n", pParam->delay_param.uSec);
				udelay(pParam->delay_param.uSec);
				ret = E_PASS;
				break;
			case END_SCIRPT:
				done = 1;
				ret = E_PASS;
				break;
			default:
				printf("Unkown command for script.\n");
				ret = E_FAIL;
				break;
		}
		if(ret != E_PASS){
			printf("FAIL!!!\n");
			break;
		}
	}
	return ret;
}

Bool Sensor_test(void)
{
	/* Prepare to load code */
	if(!gbM3Initialized){
		DucatiClkEnable();
		ISS_A8_Enable();
		gbM3Initialized = 1;
	}
#ifdef CONFIG_SENSOR_MT9J003
	unsigned int old_bus;
	Bool ret = E_PASS;
	old_bus = I2C_GET_BUS();
	if(I2C_SET_BUS(HISPI_CAM_I2C_BUS_NUM) != 0)
		return E_FAIL;
	if(SENSOR_ID == GetSensorID()) {
		ShowSensorInfo();
		ret = SensorInit();
	} else
		ret = E_FAIL;
	I2C_SET_BUS(old_bus);
	if(ret == E_PASS)
		printf("Done with success.\n");
	return ret;
#elif defined(CONFIG_SENSOR_AR0331)
	unsigned int old_bus;
	Bool ret = E_PASS;
	old_bus = I2C_GET_BUS();
	if(I2C_SET_BUS(HISPI_CAM_I2C_BUS_NUM) != 0)
		return E_FAIL;
	if(SENSOR_ID == GetSensorID()){
		printf("ID check ok\n");
		ret = SensorInit();
		if(GetSensorID() == SENSOR_ID)
			printf("ID recheck ok\n");
	}else{
		printf("ID check fail\n");
	}
	I2C_SET_BUS(old_bus);
	if(ret == E_PASS)
		printf("Done with success.\n");
	return ret;
#else /* no sensor */
	return E_FAIL;
#endif /* end no sensor */
}
Bool WIFI_BOARD_test(void)
{
	struct mmc *mmc;
	mmc = find_mmc_device(1);
	if (mmc) {
		if (mmc_init(mmc) == 0){
			printf("Device: %s\n", mmc->name);
			printf("Vendor ID: 0x%08x\n", mmc->cid[0]);
			return 0;
		}
		else
			printf("On-board WIFI adapter not found!\n");
	}
	else
		printf("On-board SDIO interface not found!\n");
	return 1;
}
Bool SD_test(void)
{
	struct mmc *mmc;
	mmc = find_mmc_device(0);
	if (mmc) {
		if (mmc_init(mmc) == 0){
			printf("Device: %s\n", mmc->name);	
			printf("Tran Speed: %d\n", mmc->tran_speed);
			printf("Rd Block Len: %d\n", mmc->read_bl_len);
			printf("%s version %d.%d\n", IS_SD(mmc) ? "SD" : "MMC",
					(mmc->version >> 4) & 0xf, mmc->version & 0xf);

			printf("High Capacity: %s\n", mmc->high_capacity ? "Yes" : "No");
			printf("Capacity: %lld MB\n", mmc->capacity/(1024*1024));
			return 0;
		}
		else
			puts("No MMC/SD card found\n");
	}
	else
		puts("MMC Device not found\n");
	
	return 1;
}
void Output_test(void)
{
	char cmd = 57;
	if(!gbM3Initialized){
		DucatiClkEnable();
		ISS_A8_Enable();
		gbM3Initialized = 1;
	}
	if (!gbDSSInitialized) {
		dss_pll_config();
		DSSClkEnable();
		video0_pll_config();
		video1_pll_config();
		hdmi_pll_config();
		gbDSSInitialized = 1;
	}
	while( cmd != 48 ) {
		ShowOutputMenu();		
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				HDMI_Colorbar_test();
				break;
			case '2':
				TV_Colorbar_test();
				break;
#if 0
			case '3':
				VideoM3Reset();
				break;
			case '4':
				SetVideoM3EntryPoint(ENTRYPOINT_OF_M3_CODE);
				VideoM3Run();
				break;
#endif
			default:
				break;
		}
	}
}
void LED_test(void)
{
	char cmd = 57;
	u32  add, val;
	while( cmd != 48 ) {
		ShowLEDMenu();
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				add=0x481ae13c;  		 	//GPIO_DATAOUT Data Output Register
				val = __raw_readl(add);
				val |= (1<<12);    				//GP3_12-LED1_ON 
				__raw_writel(val, add);
				break;
			case '2':
				add=0x481ae190;  		 	//GPIO_CLEARDATAOUT Clear Data Output Register
				val = __raw_readl(add);
				val |= (1<<12);    				//GP3_12-LED1_ON 
				__raw_writel(val, add);
				break;
			default:
				break;
		}
	}
}

void RS485_Read_Test(void)
{
	char ch;
	SetRS485_Dir(RS485_IN_DIR);
	printf("\nPlease enter a character from RS485");
	console_assign(stdin, RS485_NAME);
	ch = getc();
	console_assign(stdin, DEFAULT_NAME);
#if TST_RS485_ON_PC
	/* Some USB <-> RS485 cable DO NOT generate real RS485 signal. */
	printf("\n Got a character from RS485");
#else
	printf("\n Got %c(0x%x) from RS485", ch, ch);
#endif
}

void RS485_Write_Test(void)
{
	SetRS485_Dir(RS485_OUT_DIR);
	console_assign(stderr, RS485_NAME);
	fprintf(stderr, "\r\n *******************************");
	fprintf(stderr, "\r\n  RS-485 TEST");
	fprintf(stderr, "\r\n ******************************* \r\n");
	console_assign(stderr, DEFAULT_NAME);
}

void RS485_Test(void)
{
	char cmd = 0;
	while(cmd != '0'){
		ShowRS485Menu();
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				RS485_Read_Test();
				break;
			case '2':
				RS485_Write_Test();
				break;
			default:
				break;
		}
	}
}

void MiscTest(void)
{
	char cmd = 0;
	while(cmd != '0'){
		ShowMiscMenu();
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				SD_test();
				break;
			case '2':
				RS485_Test();
				break;
			case '3':
				WIFI_BOARD_test();
				break;
			default:
				break;
		}
	}
}

static int do_autotest(cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if (argc > 1)
		goto usage;
	char cmd;
	while(1) {
		ShowMainMenu();
		cmd =getc();
		printf("\ninput = %c\n",cmd);
		switch(cmd) {
			case '1':
				memory_test();
				break;
			case '2':
				eth_test();
				break;
			case '3':
				Audio_test();
				break;
			case '4':
				RTC_test();
				break;
			case '5':
				GIO_test();
				break;
			case '6':
				LED_test();
				break;
			case '7':
				Output_test();
				break;
			case '8':
				Sensor_test();
				break;
			case '9':
				MiscTest();
				break;
			case '0':
				/* maybe do something before exit. */
				return 0;
			default:
				break;
		}
	}
	return 0;

usage:
	printf ("Usage: autotest %s ", cmdtp->usage);
	return 1;
}

U_BOOT_CMD(
	autotest, 1, 1,	do_autotest,
	"self test command",
);
