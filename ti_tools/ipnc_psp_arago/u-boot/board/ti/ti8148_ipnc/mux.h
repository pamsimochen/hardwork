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
/* 1-MMC1_CMD */
/* 1-4 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -8 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -12 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 14 usb1_drvvbus, fn8, pulldn enable  15-AIC_RSTn */
/* -16 */	BIT(0), BIT(7), BIT(7), BIT(0),  
/* -20 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -24 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -28 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -32 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -36 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -40 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -44 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -48 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -52 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -56 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -60 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -64 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -68 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 69-RS485_RDE */
/* -72 */	BIT(7), BIT(0), BIT(0), BIT(0),
/* 74-I2C[2]_SCL 75-I2C[2]_SDA 76-UART1_TXD */
/* -76 */	BIT(0), BIT(5), BIT(5), BIT(2),
/* 77-UART1_RXD 78-HDMI_SCL 79-HDMI_SDA */
/* -80 */	BIT(2), BIT(1), BIT(1), BIT(0),
/* -84 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 85-GP1[16] */
/* -88 */	BIT(7), BIT(0), BIT(0), BIT(0),
/* -92 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -96 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -100 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -104 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -108 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 111-HDMI_CEC 112-HDMI_HPDET */
/* -112 */	BIT(0), BIT(0), BIT(4), BIT(4),
/* 115 TIM6_IO 116 TIM7_IO */
/* -116 */	BIT(0), BIT(0), BIT(6), BIT(6),
/* -120 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -124 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -128 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -132 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -136 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -140 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -144 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -148 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 150 MMC_CD 151 MMC_WP */
/* -152 */	BIT(0), BIT(7), BIT(7), BIT(0), 
/* 153 CAM_RST 154-mmc1_pow 155 -ENET_RST 156-CAM_D[8] */
/* -156 */	BIT(7), BIT(7), BIT(7), BIT(1), 
/* 157-CAM_D[9] 158-CAM_D[10] 159-CAM_D[11] 160-CAM_D[12] */
/* -160 */	BIT(1), BIT(1), BIT(1), BIT(1),
/* 161-CAM_D[13] 162-CAM_D[14] 163-CAM_D[15] 164-CAM_D[7] */
/* -164 */	BIT(1), BIT(1), BIT(1), BIT(1),
/* 165-CAM_D[6] 166-CAM_D[5] 167-CAM_D[4] 168-CAM_D[3] */
/* -168 */	BIT(1), BIT(1), BIT(1), BIT(1),
/* 169-CAM_D[2] 170-CAM_D[1] 171-CAM_D[0] 172-CAM_HS */
/* -172 */	BIT(1), BIT(1), BIT(1), BIT(1),
/* 173-CAM_VS 174-gp0_28 175-CAM_PCLK  */
/* -176 */	BIT(1), BIT(7), BIT(1), BIT(0),
/* 179-RE_SETING 180-gp2_22(WLAN_EN) */
/* -180 */	BIT(0), BIT(0), BIT(7), BIT(7),
/* 181-LED3_ON */
/* -184 */	BIT(7), BIT(0), BIT(0), BIT(0),
/* 188-gp2_24(WLAN_IRQ) */
/* -188 */	BIT(0), BIT(0), BIT(0), BIT(7),
/* -192 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 196-gp2_26(BT_EN) */
/* -196 */	BIT(0), BIT(0), BIT(0), BIT(7),
/* -200 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -204 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -208 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -212 */	BIT(0), BIT(0), BIT(0), BIT(0), 
/* 215-ARM_IN  216-ARM_OUT */
/* -216 */	BIT(0), BIT(0), BIT(7), BIT(7), 
/* 217-ARM_RST 218-MODE0 219-MODE1 220-LED1_ON */
/* -220 */	BIT(7), BIT(7), BIT(7), BIT(7), 
/* 221-LED2_ON */
/* -224 */	BIT(7), BIT(0), BIT(0), BIT(0),
/* 228-IRDA_DET */
/* -228 */	BIT(0), BIT(0), BIT(0), BIT(7),
/* -232 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -236 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -240 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -244 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -248 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 251-UART4_RXD 252-UART4_TXD */
/* -252 */	BIT(0), BIT(0), BIT(5), BIT(5),
/* 253-UART4_CTS 254-UART4_RTS */
/* -256 */	BIT(5), BIT(5), BIT(0), BIT(0),
/* -260 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -264 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* -268 */	BIT(0), BIT(0), BIT(0), BIT(0),
/* 270 usb0_drvvbus, fn1, pulldn enable */
/* -271 */	BIT(0), BIT(0), BIT(0),
