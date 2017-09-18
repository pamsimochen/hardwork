#ifndef _ISSDRV_IMX140_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_IMX140_CONFIG_H_
#define PSP_I2C_MAX_REG_RD_WR       	(0x200)
#define FRAME_LENGTH 1125
typedef struct
{
    UInt32 i2cInstId;

    UInt32 numArgs;
    UInt16 regAddr[PSP_I2C_MAX_REG_RD_WR];
    UInt16 regValue[PSP_I2C_MAX_REG_RD_WR];
	UInt32 numRegs;
} I2c_Ctrl;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

