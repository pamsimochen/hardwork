#ifndef _ISSDRV_MT9M034_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_MT9M034_CONFIG_H_


#define PSP_I2C_MAX_REG_RD_WR       	(0x200)

#define MT_9M034_ADDR			(0x10)  // CCI write address

#define MT_9M034_BOARD_PRECONFIG		(TRUE)	//TURE/FALSE

typedef struct
{
    UInt32 i2cInstId;

    UInt32 numArgs;
    UInt16 regAddr[PSP_I2C_MAX_REG_RD_WR];
    UInt16 regValue[PSP_I2C_MAX_REG_RD_WR];
	UInt32 numRegs;
} I2c_Ctrl;

/* mt9m034 register addresses */
#define MT9M034_CHIP_VERSION		0x3000
#define MT9M034_ROW_START_A		0x3002
#define MT9M034_COLUMN_START_A	0x3004
#define MT9M034_ROW_END_A			0x3006
#define MT9M034_COLUMN_END_A		0x3008
#define MT9M034_FRAME_LINES_A		0x300A
//#define MT9M034_LINE_LENGTH		0x300C
#define MT9M034_COARSE_IT_TIME_A	0x3012
#define MT9M034_FINE_IT_TIME_A	0x3014
#define MT9M034_COARSE_IT_TIME_B	0x3016
#define MT9M034_FINE_IT_TIME_B	0x3018
#define MT9M034_ENABLE_STREAMING	0x301a
#define MT9M034_ROW_SPEED			0x3028
#define MT9M034_VT_PIX_CLK_DIV	0x302A
#define MT9M034_VT_SYS_CLK_DIV	0x302C
#define MT9M034_PRE_PLL_CLK_DIV	0x302C
#define MT9M034_PLL_MULTIPLIER	0x3030
#define MT9M034_SCALING_MODE		0x3032
#define MT9M034_READ_MODE			0x3040
#define MT9M034_DARK_CONTROL		0x3044
#define MT9M034_GLOABL_GAIN		0x305E
#define MT9M034_EMBEDDED_DATA		0x3064
#define MT9M034_OP_MODE_A			0x3082
#define MT9M034_OP_MODE_B			0x3084
#define MT9M034_COLUMN_START_B	0x308A
#define MT9M034_ROW_START_B		0x308C
#define MT9M034_COLUMN_END_B		0x308E
#define MT9M034_ROW_END_B			0x3090
#define MT9M034_Y_ODD_INC_A		0x30A6
#define MT9M034_Y_ODD_INC_B		0x30A8
#define MT9M034_FRAME_LINES_B		0x30AA
#define MT9M034_DIGITAL_TEST		0x30B0
#define MT9M034_DAC_LD_24_25    0x3EE4
#define MT9M034_AE_CTRL_REG		0x3100

//Xiangdong: Add definition for digital gain registers
#define MT9M034_DGAIN_GREEN1        0x3056
#define MT9M034_DGAIN_BLUE      0x3058
#define MT9M034_DGAIN_RED       0x305A
#define MT9M034_DGAIN_GREEN2        0x305C


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

