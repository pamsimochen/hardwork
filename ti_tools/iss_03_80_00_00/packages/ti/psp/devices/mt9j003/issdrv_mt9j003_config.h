/** ==================================================================
 *  @file   issdrv_mt9j003_config.h
 *
 *  @path   /ti/psp/devices/mt9j003/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _ISSDRV_MT9J003_CONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSDRV_MT9J003_CONFIG_H_

#define MT9J003_EXTCLK                  (10)               // Mhz

#define PSP_I2C_MAX_REG_RD_WR           (0x100)

#define MT9J_003_ADDR                   (0x36)             // CCI write
                                                           // address

#define MT9J_003_BOARD_PRECONFIG        (TRUE)             // TURE/FALSE

#define MT9J_003_REG_COUNT              (65)

typedef struct {
    UInt32 i2cInstId;

    UInt32 numArgs;
    UInt16 regAddr[PSP_I2C_MAX_REG_RD_WR];
    UInt16 regValue[PSP_I2C_MAX_REG_RD_WR];
    UInt32 numRegs;
} I2c_Ctrl;

typedef struct {
    Int32 fps;
    Int32 x_size;
    Int32 y_size;
    Bool binning_en;
    Int32 bin_window_size_x;
    Int32 bin_window_size_y;
    Bool skipping_en;
    Bool flip_en;
    Bool mirror_en;
    Int32 global_gain;
    Bool test_mode_en;
    Int32 update_mask;
} SensorConfig;

#define vt_pix_clk_div 0x0300

UInt32 SensorConfigScript30fps[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},
    {0x0300, 2, 0x3},
    {0x0302, 2, 0x1},
    {0x0304, 2, 0x2},
    {0x0306, 2, 48},
    {0x0308, 2, 0x0C},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0104, 2, 0x01},
    {0x3064, 2, 0x805},
    {0x3016, 2, 0x121},
    {0x0344, 2, 0x012},
    {0x0348, 2, 0xF13},
    {0x0346, 2, 0x8},
    {0x034A, 2, 0x879},
    {0x3040, 2, 0x28C3},
    {0x0400, 2, 0x0002},
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x782},
    {0x034E, 2, 0x43A},
    {0x0342, 2, 0x0881},
    {0x0340, 2, 0x04c9},
    {0x0202, 2, 1212},
    {0x3014, 2, 0x0603},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    {0x30D4, 2, 0xB080},
    {0x306E, 2, 0x90B0},
    {0x3E00, 2, 0x0010},
    {0x3070, 2, 0x0000},
    {0x31C6, 2, 0x8400},
    {0x3E00, 2, 0x0010},
    {0x3EDC, 2, 0xD8E4},
    {0x3178, 2, 0x0070},
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    {0x301A, 2, 0x0010},
    {0x3064, 2, 0x0805},
    {0x301E, 2, 0x00A8},
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1040},
    {0x0202, 2, 1175},
    {10, 0, 0}
};

UInt32 SensorConfigScript[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},
    {0x0300, 2, 0x3},
    {0x0302, 2, 0x1},
    {0x0304, 2, 0x1},
    {0x0306, 2, 48},
    {0x0308, 2, 0x0C},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0104, 2, 0x01},
    {0x3064, 2, 0x805},
    {0x3016, 2, 0x121},
    {0x0344, 2, 0x012},
    {0x0348, 2, 0xF13},
    {0x0346, 2, 0x8},
    {0x034A, 2, 0x879},
    {0x3040, 2, 0x28C3},
    {0x0400, 2, 0x0002},
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x782},
    {0x034E, 2, 0x43A},
    {0x0342, 2, 0x0881},
    {0x0340, 2, 0x04c9},
    {0x0202, 2, 1212},
    {0x3014, 2, 0x0603},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    {0x30D4, 2, 0xB080},
    {0x306E, 2, 0x90B0},
    {0x3E00, 2, 0x0010},
    {0x3070, 2, 0x0000},
    {0x31C6, 2, 0x8400},
    {0x3E00, 2, 0x0010},
    {0x3EDC, 2, 0xD8E4},
    {0x3178, 2, 0x0070},
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    {0x301A, 2, 0x0010},
    {0x3064, 2, 0x0805},
    {0x301E, 2, 0x00A8},
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1040},
    {0x0202, 2, 1175},
    {10, 0, 0}
};

UInt32 SensorConfigScript5MP[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},
    {0x0300, 2, 0x3},
    {0x0302, 2, 0x1},
    {0x0304, 2, 0x1},
    {0x0306, 2, 48},
    {0x0308, 2, 0x0C},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0104, 2, 0x01},
    {0x3064, 2, 0x805},
    {0x3016, 2, 0x121},
    {0x0344, 2, 0x298},
    {0x0348, 2, 0xC97},
    {0x0346, 2, 0x1A6},
    {0x034A, 2, 0x925},
    {0x3040, 2, 0x0041},
    {0x0400, 2, 0x0002},
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x0A00},
    {0x034E, 2, 0x0780},
    {0x0342, 2, 0x0B04},
    {0x0340, 2, 0x080F},
    {0x0202, 2, 0x0},
    {0x3014, 2, 0x0603},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    {0x30D4, 2, 0xB080},
    {0x306E, 2, 0x90B0},
    {0x3E00, 2, 0x0010},
    {0x3070, 2, 0x0000},
    {0x31C6, 2, 0x8400},
    {0x3E00, 2, 0x0010},
    {0x3EDC, 2, 0xD8E4},
    {0x3178, 2, 0x0000},
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    {0x301A, 2, 0x0010},
    {0x3064, 2, 0x0805},
    {0x301E, 2, 0x00A8},
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1040},
    {0x0202, 2, 0x0},
    {10, 0, 0}
};

UInt32 SensorConfigScript8MP[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},
    {0x0300, 2, 0x3},
    {0x0302, 2, 0x1},
    {0x0304, 2, 0x1},
    {0x0306, 2, 48},
    {0x0308, 2, 0x0C},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0104, 2, 0x01},
    {0x3064, 2, 0x805},
    {0x3016, 2, 0x121},
    {0x0344, 2, 0x138},
    {0x0348, 2, 0xDF7},
    {0x0346, 2, 0x09E},
    {0x034A, 2, 0xA2D},
    {0x3040, 2, 0x0041},
    {0x0400, 2, 0x2},
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x0CC0},
    {0x034E, 2, 0x0990},
    {0x0342, 2, 0x0D30},
    {0x0340, 2, 0x0BB7},
    {0x0202, 2, 0x0},
    {0x3014, 2, 0x0603},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    {0x30D4, 2, 0xB080},
    {0x306E, 2, 0x90B0},
    {0x3E00, 2, 0x0010},
    {0x3070, 2, 0x0000},
    {0x31C6, 2, 0x8400},
    {0x3E00, 2, 0x0010},
    {0x3EDC, 2, 0xD8E4},
    {0x3178, 2, 0x0000},
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    {0x301A, 2, 0x0010},
    {0x3064, 2, 0x0805},
    {0x301E, 2, 0x00A8},
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1040},
    {0x0202, 2, 0x0},
    {10, 0, 0}
};

UInt32 SensorConfigScript10MP[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    /* Disable Streaming */
    {0x0100, 2, 0x00},
    {100, 0, 100},
    /* PLL */
    {0x0300, 2, 0x3},
    {0x0302, 2, 0x1},
    {0x0304, 2, 0x1},
    {0x0306, 2, 48},
    {0x0308, 2, 0x0C},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    /* Group Hold */
    {0x0104, 2, 0x01},
    /* Row Speed */
    {0x3064, 2, 0x805},
    {0x3016, 2, 0x121},
    /* FOV */
    {0x0344, 2, 0x070},
    {0x0348, 2, 0xEBF},
    {0x0346, 2, 0x008},
    {0x034A, 2, 0xAC3},
    /*  Binning/Summing */
    {0x3040, 2, 0x0041},
    {0x0400, 2, 0x2},
    /* Scaling */
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x0E50},
    {0x034E, 2, 0x0ABC},
    /* Timing */
    {0x0342, 2, 0x0EC3},
    {0x0340, 2, 0x0B4B},
    {0x0202, 2, 0x0},
    {0x3014, 2, 0x0603},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    /* Column */
    {0x30D4, 2, 0xB080},
    {0x306E, 2, 0x90B0},
    {0x3E00, 2, 0x0010},
    /* Disable Test Pattern */
    {0x3070, 2, 0x0000},
    /* HiSPi */
    {0x31C6, 2, 0x8400},
    {0x3E00, 2, 0x0010},
    {0x3EDC, 2, 0xD8E4},
    /* Summing */
    {0x3178, 2, 0x0000},
    /* Low Power */
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    /* Default Changes */
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    /* RGB Gain */
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    /* Data Pedestal */
    {0x301A, 2, 0x0010},
    {0x3064, 2, 0x0805},
    {0x301E, 2, 0x00A8},
    /* Bayer Resumpling */
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    /* HiSPi */
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    /* Group Hold */
    {0x0104, 2, 0x00},
        {0x0120, 2, 0x00},
    /* Global Gain */
    {0x305e, 2, 0x1040},
    {0x0202, 2, 0x0},
    {10, 0, 0}
};

UInt32 SensorConfigScript_30[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},
    {0x0300, 2, 0x3},                                      // vt_pix_clk_div
    {0x0302, 2, 0x1},                                      // vt_sys_clk_div
    {0x0304, 2, 0x1},                                      // pre_pll_clk_div
    {0x0306, 2, 0x30},                                     // pll_multiplier
    {0x0308, 2, 0x0C},                                     // op_pix_clk_div
    {0x030A, 2, 0x01},                                     // op_sys_clk_div
    {100, 0, 100},
    {0x0104, 2, 0x01},
    {0x3064, 2, 0x805},
    {0x3178, 2, 0x0000},
    {0x3ED0, 2, 0x1B24},
    {0x3EDC, 2, 0xC3E4},
    {0x3EE8, 2, 0x0000},
    {0x3E00, 2, 0x0010},
    {0x3016, 2, 0x111},                                    // row_speed
    {0x0344, 2, 0x3D8},
    {0x0348, 2, 0xB57},
    {0x0346, 2, 0x34A},
    {0x034A, 2, 0x781},
    {0x3040, 2, 0x0041},
    {0x0400, 2, 0x0000},
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x800},
    {0x034E, 2, 0x450},
    {0x0342, 2, 0x0880},
    {0x0340, 2, 0x04C9},
    {0x0202, 2, 0x0010},
    {0x3014, 2, 0x03F2},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    {0x30D4, 2, 0x1080},
    {0x306E, 2, 0x90B0},
    {0x3070, 2, 0x0000},
    {0x31C6, 2, 0x8400},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x301A, 2, 0x0010},                                   // Unlock data
                                                           // pedestal
                                                           // register
    {0x3064, 2, 0x0805},                                   // Disable
                                                           // embedded data
    {0x301E, 2, 0x00A8},                                   // Set data
                                                           // pedestal
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1840},                                   // 2x
    {0x0202, 2, 2450},                                     // 1/30s
    {10, 0, 0}
};

UInt32 SensorConfigScript_30_bin[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},                                         // Mode Select =
                                                           // 0x0
    {0x0300, 2, 0x3},                                      // vt_pix_clk_div
                                                           // = 0x3
    {0x0302, 2, 0x1},                                      // vt_sys_clk_div
                                                           // = 0x1
    {0x0304, 2, 0x2},                                      // pre_pll_clk_div
                                                           // = 0x2
    {0x0306, 2, 96},                                       // pll_multiplier
                                                           // = 0x60
    {0x0308, 2, 0x0C},                                     // op_pix_clk_div
                                                           // = 0xC
    {0x030A, 2, 0x01},                                     // op_sys_clk_div
                                                           // = 0x1
    {100, 0, 100},
    {0x0104, 2, 0x01},                                     // Grouped
                                                           // Parameter Hold
                                                           // = 0x1
    {0x3064, 2, 0x805},                                    // RESERVED_MFR_3064
                                                           // = 0x805
    {0x3016, 2, 0x121},                                    // Row Speed =
                                                           // 0x111
    {0x0344, 2, 0x012},                                    // Column Start =
                                                           // 0x18
    {0x0348, 2, 0xF13},                                    // Column End =
                                                           // 0xF10
    {0x0346, 2, 0x8},                                      // Row Start =
                                                           // 0x12E
    {0x034A, 2, 0x879},                                    // Row End = 0x99B
    {0x3040, 2, 0x28C3},                                   // Read Mode =
                                                           // 0xC3
    {0x0400, 2, 0x0002},                                   // Scaling Mode =
                                                           // 0x0
    {0x0404, 2, 0x0010},                                   // Scale_M = 0x10
    {0x034C, 2, 0x782},                                    // Output Width =
                                                           // 0x780
    {0x034E, 2, 0x43A},                                    // Output Height =
                                                           // 0x438
    {0x0342, 2, 0x0880},                                   // Line Length =
                                                           // 0x884
    {0x0340, 2, 0x0992},                                   // Frame Lines =
                                                           // 0x4C7
    {0x0202, 2, 1212},                                     // Coarse
                                                           // Integration
                                                           // Time (OPTIONAL)
                                                           // = 0x10
    {0x3014, 2, 0x3F2},                                    // Fine
                                                           // Integration
                                                           // Time = 0x3F2
    {0x3010, 2, 0x009C},                                   // Fine Correction
                                                           // = 0x9C
    {0x3018, 2, 0x0000},                                   // Extra Delay =
                                                           // 0x0
    {0x30D4, 2, 0xB080},                                   // Cols Dbl
                                                           // Sampling =
                                                           // 0x1080
    {0x306E, 2, 0x90B0},                                   // Scalar
                                                           // Re-sampling =
                                                           // 0x90B0
    {0x3E00, 2, 0x0010},                                   // Low Power
                                                           // On_Off = 0x10
    {0x3070, 2, 0x0000},                                   // 0:Normal
                                                           // operation:
                                                           // Generate output
                                                           // data from pixel
                                                           // array
    {0x31C6, 2, 0x8400},                                   // 01: Packetized
                                                           // SP
    {0x3E00, 2, 0x0010},                                   // Low Power
                                                           // On_Off = 0x10
    {0x3EDC, 2, 0xD8E4},                                   // Summing
                                                           // Procedure 3 =
                                                           // 0xC3E4
    {0x3178, 2, 0x0070},
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    {0x301A, 2, 0x0010},                                   // Unlock data
                                                           // pedestal
                                                           // register
    {0x3064, 2, 0x0805},                                   // Disable
                                                           // embedded data
    {0x301E, 2, 0x00A8},                                   // Set data
                                                           // pedestal
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1040},                                   // 1x
    {0x0202, 2, 2450},                                     // 1/30s
    {10, 0, 0}
};

UInt32 SensorConfigScript_60_bin[69][3] = {
    {0x030A, 2, 0x00},
    {100, 0, 100},
    {0x030A, 2, 0x01},
    {100, 0, 100},
    {0x0100, 2, 0x00},
    {100, 0, 100},                                         // Mode Select =
                                                           // 0x0
    {0x0300, 2, 0x3},                                      // vt_pix_clk_div
                                                           // = 0x3
    {0x0302, 2, 0x1},                                      // vt_sys_clk_div
                                                           // = 0x1
    {0x0304, 2, 0x2},                                      // pre_pll_clk_div
                                                           // = 0x2
    {0x0306, 2, 96},                                       // pll_multiplier
                                                           // = 0x60
    {0x0308, 2, 0x0C},                                     // op_pix_clk_div
                                                           // = 0xC
    {0x030A, 2, 0x01},                                     // op_sys_clk_div
                                                           // = 0x1
    {100, 0, 100},
    {0x0104, 2, 0x01},                                     // Grouped
                                                           // Parameter Hold
                                                           // = 0x1
    {0x3064, 2, 0x805},                                    // RESERVED_MFR_3064
                                                           // = 0x805
    {0x3016, 2, 0x121},                                    // Row Speed =
                                                           // 0x111
    {0x0344, 2, 0x012},                                    // Column Start =
                                                           // 0x18
    {0x0348, 2, 0xF13},                                    // Column End =
                                                           // 0xF10
    {0x0346, 2, 0x8},                                      // Row Start =
                                                           // 0x12E
    {0x034A, 2, 0x879},                                    // Row End = 0x99B
    {0x3040, 2, 0x28C3},                                   // Read Mode =
                                                           // 0xC3
    {0x0400, 2, 0x0002},                                   // Scaling Mode =
                                                           // 0x0
    {0x0404, 2, 0x0010},                                   // Scale_M = 0x10
    {0x034C, 2, 0x782},                                    // Output Width =
                                                           // 0x780
    {0x034E, 2, 0x43A},                                    // Output Height =
                                                           // 0x438
    {0x0342, 2, 0x0881},                                   // Line Length =
                                                           // 0x884
    {0x0340, 2, 0x04c9},                                   // Frame Lines =
                                                           // 0x4C7
    {0x0202, 2, 1212},                                     // Coarse
                                                           // Integration
                                                           // Time (OPTIONAL)
                                                           // = 0x10
    {0x3014, 2, 0x0603},                                   // Fine
                                                           // Integration
                                                           // Time = 0x3F2
    {0x3010, 2, 0x009C},                                   // Fine Correction
                                                           // = 0x9C
    {0x3018, 2, 0x0000},                                   // Extra Delay =
                                                           // 0x0
    {0x30D4, 2, 0xB080},                                   // Cols Dbl
                                                           // Sampling =
                                                           // 0x1080
    {0x306E, 2, 0x90B0},                                   // Scalar
                                                           // Re-sampling =
                                                           // 0x90B0
    {0x3E00, 2, 0x0010},                                   // Low Power
                                                           // On_Off = 0x10
    {0x3070, 2, 0x0000},                                   // 0:Normal
                                                           // operation:
                                                           // Generate output
                                                           // data from pixel
                                                           // array
    {0x31C6, 2, 0x8400},                                   // 01: Packetized
                                                           // SP
    {0x3E00, 2, 0x0010},                                   // Low Power
                                                           // On_Off = 0x10
    {0x3EDC, 2, 0xD8E4},                                   // Summing
                                                           // Procedure 3 =
                                                           // 0xC3E4
    {0x3178, 2, 0x0070},
    {0x3170, 2, 0x00E5},
    {0x3ECC, 2, 0x0FE4},
    {0x316C, 2, 0x0429},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x3EE2, 2, 0xA4BF},
    {0x3EEC, 2, 0x1C21},
    {0x3056, 2, 0x1040},
    {0x3058, 2, 0x1040},
    {0x305A, 2, 0x1040},
    {0x305C, 2, 0x1040},
    {0x301A, 2, 0x0010},                                   // Unlock data
                                                           // pedestal
                                                           // register
    {0x3064, 2, 0x0805},                                   // Disable
                                                           // embedded data
    {0x301E, 2, 0x00A8},                                   // Set data
                                                           // pedestal
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1040},                                   // 1x
    {0x0202, 2, 1175},                                     // 1/60s
    {10, 0, 0}
};

UInt32 SensorConfigScript_60[57][3] = {
    {0x0100, 2, 0x00},
    {100, 0, 100},
    {0x0300, 2, 0x3},                                      // vt_pix_clk_div
    {0x0302, 2, 0x1},                                      // vt_sys_clk_div
    {0x0304, 2, 0x1},                                      // pre_pll_clk_div
    {0x0306, 2, 0x30},                                     // pll_multiplier
    {0x0308, 2, 0x0C},                                     // op_pix_clk_div
    {0x030A, 2, 0x01},                                     // op_sys_clk_div
    {100, 0, 100},
    {0x0104, 2, 0x01},
    {0x3064, 2, 0x805},
    {0x3178, 2, 0x0000},
    {0x3ED0, 2, 0x1B24},
    {0x3EDC, 2, 0xC3E4},
    {0x3EE8, 2, 0x0000},
    {0x3E00, 2, 0x0010},
    {0x3016, 2, 0x111},                                    // row_speed
    {0x0344, 2, 0x3D8},
    {0x0348, 2, 0xB57},
    {0x0346, 2, 0x34A},
    {0x034A, 2, 0x781},
    {0x3040, 2, 0x0041},
    {0x0400, 2, 0x0000},
    {0x0404, 2, 0x0010},
    {0x034C, 2, 0x800},
    {0x034E, 2, 0x450},
    {0x0342, 2, 0x0880},
    {0x0340, 2, 0x04C9},
    {0x0202, 2, 0x0010},
    {0x3014, 2, 0x03F2},
    {0x3010, 2, 0x009C},
    {0x3018, 2, 0x0000},
    {0x30D4, 2, 0x1080},
    {0x306E, 2, 0x90B0},
    {0x3070, 2, 0x0000},
    {0x31C6, 2, 0x8400},
    {0x3174, 2, 0x8000},
    {0x3E40, 2, 0xDC05},
    {0x3E42, 2, 0x6E22},
    {0x3E44, 2, 0xDC22},
    {0x3E46, 2, 0xFF00},
    {0x3ED4, 2, 0xF998},
    {0x3ED6, 2, 0x9789},
    {0x3EDE, 2, 0xE438},
    {0x3EE0, 2, 0xA43F},
    {0x301A, 2, 0x0010},                                   // Unlock data
                                                           // pedestal
                                                           // register
    {0x3064, 2, 0x0805},                                   // Disable
                                                           // embedded data
    {0x301E, 2, 0x00A8},                                   // Set data
                                                           // pedestal
    {0x0400, 2, 0x0000},
    {0x306E, 2, 0x9080},
    {0x31AE, 2, 0x0304},
    {0x301A, 2, 0x001C},
    {0x0104, 2, 0x00},
    {0x0120, 2, 0x00},
    {0x305e, 2, 0x1840},                                   // 2x
    {0x0202, 2, 2450},                                     // 1/30s
    {10, 0, 0}
};
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
