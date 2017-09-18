/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file csi2.h
 *
 * This file contains structures and constants  for CSI2 module of Ducati subsystem  in OMAP4/Monica
 *
 * @path drv_csi2/
 *
 * @rev 1.0
 *
 * @developer:
 */
/*========================================================================*/

#ifndef _CSI2L_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSI2L_H_

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/

#include "inc/csi2_utils.h"
#include "inc/csi2_reg.h"

#include <xdc/runtime/System.h>
#define CSI2_RETRY_CNT  50                                 /* 50 times to try 
                                                            * before error
                                                            * flag in CSI2
                                                            * RESET */
#define CSI2_IRQ_DISABLE 0
#define CSI2_IRQ_ENABLE 1
#define MAX_CSI2_INTERRUPTS 15

#define MAX_CSI2_COMPLEXIO_INTERFACES 2
#define MAX_CSI2_CONTEXTS 8
#define MAX_CSI2_GLOBAL_INTERRUPTS 15
#define MAX_CSI2_CTX_INTERRUPTS 8
#define MAX_CSI2_COMPLEXIO_INTERRUPTS 27

typedef uint32 complexio_irq_bitmask;

typedef uint16 csi2_irq_bitmask;

typedef uint32 csi2_context_irq_bitmask;

typedef CSL_CSI2Regs *csi2_regs_ovly;

typedef uint32 csi2_phyaddress_t;

typedef enum {
    CSI2_CONTEXT_0 = 0,
    CSI2_CONTEXT_1 = 1,
    CSI2_CONTEXT_2 = 2,
    CSI2_CONTEXT_3 = 3,
    CSI2_CONTEXT_4 = 4,
    CSI2_CONTEXT_5 = 5,
    CSI2_CONTEXT_6 = 6,
    CSI2_CONTEXT_7 = 7,
    CSI2_CONTEXT_MAX = 8
} CSI2_CONTEXT_T;

typedef uint16 csi2_context_number;

typedef enum {
    CSI2_COMPLEX_IO_IRQ_MAX = 27,
    CSI2_STATEALLULPMEXIT_IRQ = 26,
    CSI2_STATEALLULPMENTER_IRQ = 25,
    CSI2_STATEULPM5_IRQ = 24,
    CSI2_STATEULPM4_IRQ = 23,
    CSI2_STATEULPM3_IRQ = 22,
    CSI2_STATEULPM2_IRQ = 21,
    CSI2_STATEULPM1_IRQ = 20,
    CSI2_ERRCONTROL5_IRQ = 19,
    CSI2_ERRCONTROL4_IRQ = 18,
    CSI2_ERRCONTROL3_IRQ = 17,
    CSI2_ERRCONTROL2_IRQ = 16,
    CSI2_ERRCONTROL1_IRQ = 15,
    CSI2_ERRESC5_IRQ = 14,
    CSI2_ERRESC4_IRQ = 13,
    CSI2_ERRESC3_IRQ = 12,
    CSI2_ERRESC2_IRQ = 11,
    CSI2_ERRESC1_IRQ10 = 10,
    CSI2_ERRSOTSYNCHS5_IRQ = 9,
    CSI2_ERRSOTSYNCHS4_IRQ = 8,
    CSI2_ERRSOTSYNCHS3_IRQ = 7,
    CSI2_ERRSOTSYNCHS2_IRQ = 6,
    CSI2_ERRSOTSYNCHS1_IRQ = 5,
    CSI2_ERRSOTHS5_IRQ = 4,
    CSI2_ERRSOTHS4_IRQ = 3,
    CSI2_ERRSOTHS3_IRQ = 2,
    CSI2_ERRSOTHS2_IRQ = 1,
    CSI2_ERRSOTHS1_IRQ = 0
} csi2_complexio_interrup_id_t;

/* ======================================================================= */
/**
 * CSI2_CTX_INTERRUPT_ID_T enumaration for the context-specific interrupt ids of the CSI2
 * Mapped according to the bit-fields in CSI2_CTX_IRQSTATUS register
 */
/* ======================================================================= */

typedef enum {

    CSI2_CONTEXT_IRQ_MAX = 9,
    CSI2_CTX_ECC_CORRECTION_IRQ = 8,
    CSI2_LINE_NUMBER_IRQ = 7,
    CSI2_FRAME_NUMBER_IRQ = 6,
    CSI2_CS_IRQ = 5,
    CSI2_Reserved = 4,
    CSI2_LE_IRQ = 3,
    CSI2_LS_IRQ = 2,
    CSI2_FE_IRQ = 1,
    CSI2_FS_IRQ = 0
} CSI2_CTX_INTERRUPT_ID_T;

/* ================================================================ */
/* Description:-
 * ================================================================== */

typedef struct {

    uint8 opened;

} csi2_dev_data_t;

/* ======================================================================= */
/**
 * CSI2_DEVICE_T  enumeration for selection of one of the 2 I2c's
 *
 * @param CSI2_OFF  power-off state
 * @param CSI2_ON   power-on state
 * @param CSI2_ULP  ultra-low-power state
 */
/* ======================================================================= */

typedef enum {

    CSI2_DEVICE_A = 0,
    CSI2_DEVICE_B = 1,
    CSI2_DEVICE_MAX = 2
} CSI2_DEVICE_T;

/* ======================================================================= */
/**
 * CSI2_CTX_TRANSCODE_MODE_T  enumeration for selection of power state of CSI2
 *
 * @param CSI2_OFF  power-off state
 * @param CSI2_ON   power-on state
 * @param CSI2_ULP  ultra-low-power state
 */
/* ======================================================================= */

typedef enum {
    CSI2_DISABLE_TRANSCODE = 0,
    CSI2_DPCM_RAW10 = 1,
    CSI2_DPCM_RAW12 = 2,
    CSI2_ALAW_RAW10 = 3,
    CSI2_UNCOMPRESSED_RAW8 = 4,
    CSI2_UNCOMPRESSED_RAW10_EXP16 = 5,
    CSI2_UNCOMPRESSED_RAW10_PACKED = 6,
    CSI2_UNCOMPRESSED_RAW12_EXP16 = 7,
    CSI2_UNCOMPRESSED_RAW12_PACKED = 8,
    CSI2_UNCOMPRESSED_RAW14 = 9
} CSI2_CTX_TRANSCODE_MODE_T;

/*******************************************************************
 * CSI2 context:transcode configuration data-structure
 *
 *******************************************************************/
typedef struct {

    /* TRANSCODE MODE */
    CSI2_CTX_TRANSCODE_MODE_T transcode_mode;

    /* Pixels to output per line when the values is between 1 and 8191. */
    uint16 hcount;

    /* Number of pixels to skip horizontally */
    uint16 hskip;

    /* Pixels to output per line when the values is between 1 and 8191. */
    uint16 vcount;

    /* NUmber of pixels to skip vertically */
    uint16 vskip;

} csi2_ctx_transcode_cfg_t;

/* ======================================================================= */
/**
 * CSI2_CONTROL_FEATURE_BIT_T enumeration for selectin of status of the bit
 *
 * @param CSI2_DISABLED_BIT  disables the bit
 * @param CSI2_ENABLED_BIT	  enables the bit
 */
/* ======================================================================= */
typedef enum {
    CSI2_DISABLED_BIT = 0,
    CSI2_ENABLED_BIT = 1
} CSI2_CONTROL_FEATURE_BIT_T;

/* ======================================================================= */
/**
 * CSI2_BUFFER_SELECTION_T enumeration for selectin of buffer used 
 *
 * @param CSI2_PING  PING buffer is used
 * @param CSI2_PONG PONG buffer is used
 */
/* ======================================================================= */
typedef enum {
    CSI2_PING = 0,
    CSI2_PONG = 1
} CSI2_BUFFER_SELECTION_T;

/* ======================================================================= */
/**
 * CSI2_USER_DEFINED_MAPPING_T  enumeration for selection of user specified RAW format
 *
 * @param CSI2_USER_RAW6  6-bit RAW is used
 * @param CSI2_USER_RAW7  7-bit RAW is used
 * @param CSI2_USER_RAW8  8-bit RAW is used
 */
/* ======================================================================= */

typedef enum {
    CSI2_USER_RAW6 = 0,
    CSI2_USER_RAW7 = 1,
    CSI2_USER_RAW8 = 2
} CSI2_USER_DEFINED_MAPPING_T;

/* ======================================================================= */
/**
 * CSI2_VIRTUAL_CHANNEL_ID_T enumeration for selection of specific virtual channel
 *
 * @param CSI2_CHANNEL_0 channel '0' is used
 * @param CSI2_CHANNEL_1 channel '1' is used
 * @param CSI2_CHANNEL_2 channel '2' is used
 * @param CSI2_CHANNEL_3 channel '3' is used
 */
/* ======================================================================= */

typedef enum {
    CSI2_CHANNEL_0 = 0,
    CSI2_CHANNEL_1 = 1,
    CSI2_CHANNEL_2 = 2,
    CSI2_CHANNEL_3 = 3
} CSI2_VIRTUAL_CHANNEL_ID_T;

/* ======================================================================= */
/**
 * CSI2_DPCM_PREDICTOR_TYPE_T  enumeration for selection of simple/advanced dpcm method
 *
 * @param CSI2_SIMPLE_DPCM  simple dpcm method is used
 * @param CSI2_ADVANCED_DPCM  advanced dpcm method is used

*/
/* ======================================================================= */
typedef enum {
    CSI2_ADVANCED_DPCM = 0,
    CSI2_SIMPLE_DPCM = 1
} CSI2_DPCM_PREDICTOR_TYPE_T;

/* ======================================================================= */
/**
 * CSI2_DATA_FORMAT_TYPE_T enumeration for selection of the data format supported by CSI2 
*/
/* ======================================================================= */
typedef enum {

    CSI2_OTHERS = 0x000,
    CSI2_EMBEDDED_8BIT_NON_IMAGE_DATA = 0x012,
    CSI2_YUV420_8bit = 0x018,
    CSI2_YUV420_10bit = 0x019,
    CSI2_YUV420_8bit_legacy = 0x01A,
    CSI2_YUV420_8bit_CSPS = 0x01C,
    CSI2_YUV420_10bit_CSPS = 0x01D,
    CSI2_YUV422_8bit = 0x01E,
    CSI2_YUV422_10bit = 0x01F,
    CSI2_RGB565 = 0x022,
    CSI2_RGB888 = 0x024,
    CSI2_RAW6 = 0x028,
    CSI2_RAW7 = 0x029,
    CSI2_RAW8 = 0x02A,
    CSI2_RAW10 = 0x02B,
    CSI2_RAW12 = 0x02C,
    CSI2_RAW14 = 0x02D,
    CSI2_RGB666_EXP32_24 = 0x033,

    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_1 = 0x040,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_2 = 0x041,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_3 = 0x042,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_4 = 0x043,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_5 = 0x044,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_6 = 0x045,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_7 = 0x046,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_8 = 0x047,

    CSI2_RAW6_EXP8 = 0x068,
    CSI2_RAW7_EXP8 = 0x069,

    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_1_EXP8 = 0x080,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_2_EXP8 = 0x081,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_3_EXP8 = 0x082,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_4_EXP8 = 0x083,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_5_EXP8 = 0x084,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_6_EXP8 = 0x085,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_7_EXP8 = 0x086,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_8_EXP8 = 0x087,

    CSI2_YUV422_8bit_VP = 0x09E,
    CSI2_RGB444_EXP16 = 0x0A0,
    CSI2_RGB555_EXP16 = 0x0A1,
    CSI2_RAW10_EXP16 = 0x0AB,
    CSI2_RAW12_EXP16 = 0x0AC,
    CSI2_RAW14_EXP16 = 0x0AD,
    CSI2_YUV422_8bit_VP_Data_16bit = 0x0DE,
    CSI2_RGB666_EXP32 = 0x0E3,
    CSI2_RGB888_EXP32 = 0x0E4,
    CSI2_RAW6_DPCM10_VP = 0x0E8,
    CSI2_RAW8_VP = 0x12A,
    CSI2_RAW12_VP = 0x12C,
    CSI2_RAW14_VP = 0x12D,
    CSI2_RAW10_VP = 0x12F,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_1_DPCM12_VP = 0x140,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_2_DPCM12_VP = 0x141,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_3_DPCM12_VP = 0x142,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_4_DPCM12_VP = 0x143,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_5_DPCM12_VP = 0x144,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_6_DPCM12_VP = 0x145,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_7_DPCM12_VP = 0x146,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_8_DPCM12_VP = 0x147,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_1_DPCM12_EXP16 = 0x1C0,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_2_DPCM12_EXP16 = 0x1C1,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_3_DPCM12_EXP16 = 0x1C2,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_4_DPCM12_EXP16 = 0x1C3,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_5_DPCM12_EXP16 = 0x1C4,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_6_DPCM12_EXP16 = 0x1C5,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_7_DPCM12_EXP16 = 0x1C6,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_8_DPCM12_EXP16 = 0x1C7,
    CSI2_RAW7_DPCM10_EXP16 = 0x229,
    CSI2_RAW6_DPCM10_EXP16 = 0x2A8,
    CSI2_RAW8_DPCM10_EXP16 = 0x2AA,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_1_DPCM10_EXP16 = 0x2C0,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_2_DPCM10_EXP16 = 0x2C1,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_3_DPCM10_EXP16 = 0x2C2,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_4_DPCM10_EXP16 = 0x2C3,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_5_DPCM10_EXP16 = 0x2C4,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_6_DPCM10_EXP16 = 0x2C5,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_7_DPCM10_EXP16 = 0x2C6,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_8_DPCM10_EXP16 = 0x2C7,
    CSI2_RAW7_DPCM10_VP = 0x329,
    CSI2_RAW8_DPCM10_VP = 0x32A,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_1_DPCM10_VP = 0x340,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_2_DPCM10_VP = 0x341,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_3_DPCM10_VP = 0x342,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_4_DPCM10_VP = 0x343,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_5_DPCM10_VP = 0x344,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_6_DPCM10_VP = 0x345,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_7_DPCM10_VP = 0x346,
    CSI2_USER_DEFINED_8_BIT_DATA_TYPE_8_DPCM10_VP = 0x347,
    CSI2_RAW6_DPCM12_VP = 0x368,
    CSI2_RAW7_DPCM12_EXP16 = 0x369,
    CSI2_RAW8_DPCM12_EXP16 = 0x36A,
    CSI2_RAW6_DPCM12_EXP16 = 0x3A8,
    CSI2_RAW7_DPCM12_VP = 0x3A9,
    CSI2_RAW8_DPCM12_VP = 0x3AA
} CSI2_DATA_FORMAT_TYPE_T;

/*******************************************************************
 * CSI2 context configuration data-structure
 *
 *******************************************************************/

typedef struct {
    /* BYTESWAP facility 0:Disabled 1:Enabled */
    CSI2_CONTROL_FEATURE_BIT_T byteswap;

    /* GENERIC mode of operation-- 0:Disabled 1:Enabled */
    CSI2_CONTROL_FEATURE_BIT_T generic;

    /* FEC_number: Number of Frame-end codes to be received;shall be used
     * only in interlace mode, otherwise set to '1' */
    uint8 fec_number;

    /* count:Sets the number of frame to acquire */
    uint8 count;

    /* end of frame signal 0: not asserted 1: asserted */
    CSI2_CONTROL_FEATURE_BIT_T eof_en;

    /* end of line signal 0: not asserted 1: asserted */
    CSI2_CONTROL_FEATURE_BIT_T eol_en;

    /* CheckSum 0: Disabled 1: Enabled */
    CSI2_CONTROL_FEATURE_BIT_T cs_en;

    /* Writes to count field are locked---0:locked 1:unlocked */
    CSI2_CONTROL_FEATURE_BIT_T count_unlock;

    /* vp_firce: Forces sending of the data to both VPORT and OCP 0:Disabled
     * 1:Enabled */
    CSI2_CONTROL_FEATURE_BIT_T vp_force;

    /* Line modulo configuration bit */
    CSI2_CONTROL_FEATURE_BIT_T line_modulo;

    /* pixel format of USER_DEFINED 0:RAW6; 1:RAW7; 2:RAW8 */
    CSI2_USER_DEFINED_MAPPING_T pixel_format;

    /* Channel Id:0,1,2,3 */
    CSI2_VIRTUAL_CHANNEL_ID_T channel_number;

    /* 0:Advanced; 1:Simple */
    CSI2_DPCM_PREDICTOR_TYPE_T predictor;

    /* Data format selection. */
    CSI2_DATA_FORMAT_TYPE_T data_format;

    /* Alpha value for RGB888, RGB666 and RBG444. */
    uint16 alpha;

    /* Number of lines to be completed after which an interrupt generated */
    uint16 line_number;

    /* Interrupt-flag settings */
    csi2_context_irq_bitmask irq_enable;

} csi2_ctx_cfg_t;

typedef csi2_ctx_cfg_t *csi2_ctx_config_ptr;

/*===========================================================*/
/* COMPLEX-I/O SPECIFIC DATA-STRUCTURES */
/*===========================================================*/

/* ======================================================================= */
/**
 * CSI2_COMPLEXIO_NUMBER_T  enumeration for selection of complex_io interface to be used
 *
 * @param CSI2_IO_1 Complex-IO numbered '1' is used
 * @param CSI2_IO_2 Complex-IO numbered '2' is used 
 */
/* ======================================================================= */
typedef enum {
    CSI2_IO_1 = 1,
    CSI2_IO_2 = 2,
    CSI2_IO_MAX = 3
} CSI2_COMPLEXIO_NUMBER_T;

/* ======================================================================= */
/**
 * CSI2_POWER_COMMAND_T  enumeration for selection of power state of CSI2
 *
 * @param CSI2_OFF  power-off state
 * @param CSI2_ON   power-on state
 * @param CSI2_ULP  ultra-low-power state
 */
/* ======================================================================= */
typedef enum {
    CSI2_OFF = 0,
    CSI2_ON = 1,
    CSI2_ULP = 2
} CSI2_POWER_COMMAND_T;

/* ======================================================================= */
/**
 * CSI2_POWER_SWITCH_MODE_T enumeration for selection of mode of power state change of CSI2
 *
 * @param CSI2_MANUAL manually power state is changed from CSI2_ON->CSI2_ULP
 * @param CSI2_AUTO     power state changed automatically to CSI2_ULP when all the channels are idle
 */
/* ======================================================================= */

typedef enum {
    CSI2_MANUAL = 0,
    CSI2_AUTO = 1
} CSI2_POWER_SWITCH_MODE_T;

/* ======================================================================= */
/**
 * CSI2_LANE_POLARITY_ORDER_T enumeration for selection of polarity order of the channel
 *
 * @param CSI2_PLUS_MINUS  +/- polarity
 * @param CSI2_MINUS_PLUS  -/+ polarity
 */
/* ======================================================================= */

typedef enum {
    CSI2_PLUS_MINUS = 0,
    CSI2_MINUS_PLUS = 1
} CSI2_LANE_POLARITY_ORDER_T;

/* ======================================================================= */
/**
 * CSI2_LANE_POSITION_T enumeration for selection of position of the channel
 *
 * @param CSI2_NOT_USED channel is not used
 * @param CSI2_POSITION_1 channel is at lane 1
 * @param CSI2_POSITION_2 channel is at lane 2
 * @param CSI2_POSITION_3 channel is at lane 3
 * @param CSI2_POSITION_4 channel is at lane 4
 * @param CSI2_POSITION_5 channel is at lane 5
 */
/* ======================================================================= */

typedef enum {
    CSI2_NOT_USED = 0,
    CSI2_POSITION_1 = 1,
    CSI2_POSITION_2 = 2,
    CSI2_POSITION_3 = 3,
    CSI2_POSITION_4 = 4,
    CSI2_POSITION_5 = 5
} CSI2_LANE_POSITION_T;

/*******************************************************************
 * CSI2 Complex-IO configuration data-structure
 *
 *******************************************************************/

typedef struct {
    /* Controls the reset of the complex IO-- 0:Reset active; 1:reset
     * de-asserted */
    CSI2_CONTROL_FEATURE_BIT_T reset_ctrl;

    /* power state--0:CSI2_OFF, 1:CSI2_ON, 2:CSI2_ULP */
    CSI2_POWER_COMMAND_T power_state;

    /* Automatic switch between CSI2_ULP and CSI2_ON states--0:disable;
     * 1:enable */
    CSI2_POWER_SWITCH_MODE_T power_mode;

    /* data lane and clock lane polarity 0:+/- pin order; 1:-/+ pin order */
    CSI2_LANE_POLARITY_ORDER_T data4_polarity;
    CSI2_LANE_POLARITY_ORDER_T data3_polarity;
    CSI2_LANE_POLARITY_ORDER_T data2_polarity;
    CSI2_LANE_POLARITY_ORDER_T data1_polarity;
    CSI2_LANE_POLARITY_ORDER_T clock_polarity;

    /* data lane and clock lane positions: 1,2,3,4,5 */
    CSI2_LANE_POSITION_T data4_position;
    CSI2_LANE_POSITION_T data3_position;
    CSI2_LANE_POSITION_T data2_position;
    CSI2_LANE_POSITION_T data1_position;
    CSI2_LANE_POSITION_T clock_position;

} csi2_complexio_cfg_t;

/*******************************************************************
 * CSI2 PHY interface configuration data-structure
 *
 *******************************************************************/

typedef struct {

    uint32 phy_00;
    uint32 phy_01;
    uint32 phy_02;
    uint32 phy_03;
    uint32 phy_04;
    uint32 phy_05;
    uint32 phy_06;
    uint32 phy_07;
    uint32 phy_08;
    uint32 phy_09;
    uint32 phy_10;
    uint32 phy_11;
    uint32 phy_12;
    uint32 phy_13;
    uint32 phy_14;
    uint32 phy_15;

} csi2_phy_cfg_t;

/* ======================================================================= */
/**
 * CSI2_MFLAG_LEVEL_T enumeration for mflag value selection in CSI2 control register,
 *  Controls assertion of the MFlag[1:0] OCP sideband signal.
 *
 * @param CSI2_FIFO_8_BY_8 8/8 of the FIFO size
 * @param CSI2_FIFO_7_BY_8 7/8 of the FIFO size
 * @param CSI2_FIFO_6_BY_8 6/8 of the FIFO size
 * @param CSI2_FIFO_5_BY_8 5/8 of the FIFO size
 * @param CSI2_FIFO_4_BY_8 4/8 of the FIFO size
 * @param CSI2_FIFO_3_BY_8 3/8 of the FIFO size
 * @param CSI2_FIFO_2_BY_8 2/8 of the FIFO size
 * @param CSI2_FIFO_1_BY_8 1/8 of the FIFO size
 */
/* ======================================================================= */

typedef enum {
    CSI2_FIFO_8_BY_8 = 0x0,
    CSI2_FIFO_7_BY_8 = 0x1,
    CSI2_FIFO_6_BY_8 = 0x2,
    CSI2_FIFO_5_BY_8 = 0x3,
    CSI2_FIFO_4_BY_8 = 0x4,
    CSI2_FIFO_3_BY_8 = 0x5,
    CSI2_FIFO_2_BY_8 = 0x6,
    CSI2_FIFO_1_BY_8 = 0x7
} CSI2_MFLAG_LEVEL_T;

/* ======================================================================= */
/**
 * CSI2_DMA_BURST_SIZE_T enumeration for the burst size of the DMA on L3 interconnect
 *
 * @param CSI2_SIZE_1x64 burst size of 1*64 bit
 * @param CSI2_SIZE_2x64 burst size of 2*64 bit
 * @param CSI2_SIZE_4x64 burst size of 4*64 bit
 * @param CSI2_SIZE_8x64 burst size of 8*64 bit
 * @param CSI2_SIZE_16x64 burst size of 16*64 bit
 */
/* ======================================================================= */
typedef enum {
    CSI2_SIZE_1x64 = 0,
    CSI2_SIZE_2x64 = 1,
    CSI2_SIZE_4x64 = 2,
    CSI2_SIZE_8x64 = 3,
    CSI2_SIZE_16x64 = 4
} CSI2_DMA_BURST_SIZE_T;

/* ======================================================================= */
/**
 * CSI2_ENDIANNESS_T for the different endianness present in the data
 *
 * @param CSI2_NATIVE_MIPI use the native MIPI endiannness
 * @param CSI2_LITTLE_ENDIAN use the little endian convention
 */
/* ======================================================================= */

typedef enum {
    CSI2_NATIVE_MIPI = 0,
    CSI2_LITTLE_ENDIAN = 1
} CSI2_ENDIANNESS_T;

/* ======================================================================= */
/**
 * CSI2_DISABLE_MODE_T enumaration for either immediate/proper disablement of CSI2
 *
 * @param CSI2_DISABLE_IMMEDIATE disables immediately w/o waiting for FEC_SYNCH codes to be completed
 * @param CSI2_DISABLE_WAIT_FOR_FEC_SYNCH disables after waiting for FEC_SYNCH codes to be completed
 */
/* ======================================================================= */
typedef enum {
    CSI2_DISABLE_IMMEDIATE = 0,
    CSI2_DISABLE_WAIT_FOR_FEC_SYNCH = 1
} CSI2_DISABLE_MODE_T;

/* ======================================================================= */
/**
 * CSI2_STREAMING_MODE_T enumaration for streaming mode configuration of CSI2
 *
 * @param CSI2_DISABLE_STREAMING  disables the streaming mode of CSI2 
 * @param CSI2_STREAMING_32BIT enables the 32-bit streaming mode
*  @param CSI2_STREAMING_64BIT enables the 64-bit streaming mode
 */
/* ======================================================================= */
typedef enum {
    CSI2_DISABLE_STREAMING = 0,
    CSI2_STREAMING_32BIT = 1,
    CSI2_STREAMING_64BIT = 2
} CSI2_STREAMING_MODE_T;

/* ======================================================================= */
/**
 * CSI2_VP_CLK_MODE_T enumaration for streaming mode configuration of CSI2
 *
 * @param CSI2_DISABLE_VP_CLK  disables the VP clock 
 * @param CSI2_OCPCLK sets the VP_CLK= OCP_CLK
*  @param CSI2_OCPCLK_BY_2 sets the VP_CLK= OCP_CLK/2
 * @param CSI2_OCPCLK_BY_3 sets the VP_CLK= OCP_CLK/3
  * @param CSI2_OCPCLK_BY_4 sets the VP_CLK= OCP_CLK/4
 */
/* ======================================================================= */
typedef enum {
    CSI2_DISABLE_VP_CLK = 0,
    CSI2_OCPCLK = 1,
    CSI2_OCPCLK_BY_2 = 2,
    CSI2_OCPCLK_BY_3 = 3,
    CSI2_OCPCLK_BY_4 = 4
} CSI2_VP_CLK_MODE_T;

/*******************************************************************
 * CSI2 Control  register configuration data-structure
 *
 *******************************************************************/
typedef struct CSI2_CTRL {

    /* M-flag level High */
    CSI2_MFLAG_LEVEL_T level_h;

    /* M-flag level Low */
    CSI2_MFLAG_LEVEL_T level_l;

    /* Sets the DMA burst size on the L3 interconnect. */
    CSI2_DMA_BURST_SIZE_T dma_burst_size;

    /* video clock mode: enabled/disabled + VP clock frequency */
    CSI2_VP_CLK_MODE_T video_clock_mode;

    /**Streaming mode enabled/disabled + 32/64bit*/
    CSI2_STREAMING_MODE_T streaming;

    /* Not Posted Writes: enabled/disabled */
    CSI2_CONTROL_FEATURE_BIT_T non_posted_write;

    /* VP only enable-- 0:VP and OCP enabled; 1: Only VP enabled */
    CSI2_CONTROL_FEATURE_BIT_T vp_only_enable;

    /* Enables the debug mode. */
    CSI2_CONTROL_FEATURE_BIT_T dbg_enable;

    /* Select endianness for YUV422 8 bit and YUV420 legacy formats. */
    CSI2_ENDIANNESS_T little_big_endian;

    /* Enables the Error Correction Code check for the received header (short 
     * and long packets for all virtual channel ids */
    CSI2_CONTROL_FEATURE_BIT_T ecc_enable;

    /* Secure bit changed by secure request only */
    CSI2_CONTROL_FEATURE_BIT_T secure_mode;

} csi2_ctrl_cfg_t;

/* ======================================================================= */
/**
 * CSI2_STOP_STATE_X16 enumaration for multiplication factor for the number of L3 cycles defined in 
 *  STOP_STATE_COUNTER bit-field
 *
 * @param CSI2_COUNTER_x1 multiplied by 1
 * @param COUNTER_x4 multiplied by 4
 * @param CSI2_COUNTER_x16 multiplied by 16
 */
/* ======================================================================= */
typedef enum {
    CSI2_COUNTER_x1 = 0,
    CSI2_COUNTER_x04 = 1,
    CSI2_COUNTER_x16 = 2
} CSI2_STOP_STATE_MULTIPLIER_T;

/*******************************************************************
 * CSI2  Timing configuration data-structure
 *
 *******************************************************************/
typedef struct {
    /* Asserts/De-asserts FORCE_RX_MODE */
    CSI2_CONTROL_FEATURE_BIT_T force_rx_mode_io1;

    /* Multiplication factor for the counter defined in the
     * STOP_STATE_COUNTER field */
    CSI2_STOP_STATE_MULTIPLIER_T multiplier_io1;

    /* Stop State counter for monitoring */
    uint16 stop_state_counter_io1;

    /* Asserts/De-asserts FORCE_RX_MODE */
    CSI2_CONTROL_FEATURE_BIT_T force_rx_mode_io2;

    /* Multiplication factor for the counter defined in the
     * STOP_STATE_COUNTER field */
    CSI2_STOP_STATE_MULTIPLIER_T multiplier_io2;

    /* Stop State counter for monitoring */
    uint16 stop_state_counter_io2;
} csi2_timing_cfg_t;

/* ======================================================================= */
/**
 * CSI2_MASTER_STANDBY_MODE_T enumaration for the  master stanndby mode of the CSI2
 *
 * @param CSI2_FORCE_STANDBY MStandby is only asserted when the module is disabled
 * @param CSI2_NO_STANDBY MStandby is never asserted. 
 * @param CSI2_SMART_STANDBY MStandby is asserted based on the activity of the module
 */
/* ======================================================================= */
typedef enum {
    CSI2_FORCE_STANDBY = 0,
    CSI2_NO_STANDBY = 1,
    CSI2_SMART_STANDBY = 2
} CSI2_MASTER_STANDBY_MODE_T;

/* ======================================================================= */
/**
 * CSI2_OCP_CLOCK_MODE_T enumaration for the  OCP Clock  mode of the CSI2
 *
 * @param CSI2_FREE_RUNNING  OCP clock is never gated
 * @param  CSI2_AUTO_IDLE       OCP clock is gated based on interface activity
  */
/* ======================================================================= */
typedef enum {
    CSI2_FREE_RUNNING = 0,
    CSI2_AUTO_IDLE = 1
} CSI2_OCP_CLOCK_MODE_T;

/*******************************************************************
 * csi2_cfg_t: Top-most data-structure for CSI2 configuration
 *
 *******************************************************************/
typedef struct {

    /* Master standby mode of CSI2 */
    CSI2_MASTER_STANDBY_MODE_T master_standby_mode;

    /* Internal OCP gating strategy defined; 0:free-running OCP clk, 1:gated
     * OCP clk based on interface activity */
    CSI2_OCP_CLOCK_MODE_T auto_idle_ocp_clock;

    /* Control structure of CSI2 module */
    csi2_ctrl_cfg_t *control;

    /* Interrupt enable configuration flag of CSI2 */
    csi2_irq_bitmask csi2_irq_enable;

    /* Complex-IO -1 and COMPLEXIO-2 timing configuration */
    csi2_timing_cfg_t *timing_io;

    /* Flag which indicates which of the 8 contexts are being set */
    uint8 context_set_flag;

    /* Context configuration-settings pointer array */
    csi2_ctx_config_ptr ctx_cfg_ptr_array[8];

    /* Complex-IO -1 configuration */
    csi2_complexio_cfg_t *complexio_cfg_1;

    /* Complex-IO -2 configuration */
    csi2_complexio_cfg_t *complexio_cfg_2;

    /* Complex-IO -1 interrupt configuration flag: */
    complexio_irq_bitmask complexio_irq_enable_1;

    /* Complex-IO -2 interrupt configuration flag */
    complexio_irq_bitmask complexio_irq_enable_2;

    /* CSI2 PHY configuration */
    csi2_phy_cfg_t *physical;

} csi2_cfg_t;

/*******************************************************************
 * Data-structure for Interrupt Handling in CSI2
 *
 *******************************************************************/

typedef struct {

    /* Software Interrupt handle */
    Swi_Handle swi_handle;

    /* Hardware Interrupt handle */
    Hwi_Handle hwi_handle;

} csi2_dev_data;

/* ======================================================================= */
/**
 * CSI2_GLOBAL_INTERRUPT_ID_T enumaration for the global interrupt ids of the CSI2
 * Mapped according to the bit-fields in CSI2_IRQ register
 */
/* ======================================================================= */

typedef enum {

    CSI2_GLOBAL_INTERRUPT_MAX = 15,
    CSI2_OCP_ERR_IRQ = 14,
    CSI2_SHORT_PACKET_IRQ = 13,
    CSI2_ECC_CORRECTION_IRQ = 12,
    CSI2_ECC_NO_CORRECTION_IRQ = 11,
    CSI2_COMPLEXIO2_ERR_IRQ = 10,
    CSI2_COMPLEXIO1_ERR_IRQ = 9,
    CSI2_FIFO_OVF_IRQ = 8,
    CSI2_CONTEXT7_IRQ = 7,
    CSI2_CONTEXT6_IRQ = 6,
    CSI2_CONTEXT5_IRQ = 5,
    CSI2_CONTEXT4_IRQ = 4,
    CSI2_CONTEXT3_IRQ = 3,
    CSI2_CONTEXT2_IRQ = 2,
    CSI2_CONTEXT1_IRQ = 1,
    CSI2_CONTEXT0_IRQ = 0
} CSI2_GLOBAL_INTERRUPT_ID_T;

typedef void (*csi2_irq_callback_t) (CSI2_RETURN status, uint32 arg1,
                                     void *arg2);
typedef struct {

    csi2_irq_callback_t cbk_func;
    uint32 arg1;
    void *arg2;

} csi2_callback_info_t;

/* ======================================================================= */
/**
 * CSI2_CTX_INTERRUPT_ID_T enumaration for the complex-io specific interrupt ids of the CSI2
 * Mapped according to the bit-fields in CSI2_COMPLEXIO_IRQSTATUS register
 */
/* ======================================================================= */
typedef enum {

    CSI2_COMPLEXIO_STATEALLULPMEXIT_IRQ = 26,
    CSI2_COMPLEXIO_STATEALLULPMENTER_IRQ = 25,

    CSI2_COMPLEXIO_STATEULPM5_IRQ = 24,
    CSI2_COMPLEXIO_STATEULPM4_IRQ = 23,
    CSI2_COMPLEXIO_STATEULPM3_IRQ = 22,
    CSI2_COMPLEXIO_STATEULPM2_IRQ = 21,
    CSI2_COMPLEXIO_STATEULPM1_IRQ = 20,

    CSI2_COMPLEXIO_ERRCONTROL5_IRQ = 19,
    CSI2_COMPLEXIO_ERRCONTROL4_IRQ = 18,
    CSI2_COMPLEXIO_ERRCONTROL3_IRQ = 17,
    CSI2_COMPLEXIO_ERRCONTROL2_IRQ = 16,
    CSI2_COMPLEXIO_ERRCONTROL1_IRQ = 15,

    CSI2_COMPLEXIO_ERRESC5_IRQ = 14,
    CSI2_COMPLEXIO_ERRESC4_IRQ = 13,
    CSI2_COMPLEXIO_ERRESC3_IRQ = 12,
    CSI2_COMPLEXIO_ERRESC2_IRQ = 11,
    CSI2_COMPLEXIO_ERRESC1_IRQ = 10,

    CSI2_COMPLEXIO_ERRSOTSYNCHS5_IRQ = 9,
    CSI2_COMPLEXIO_ERRSOTSYNCHS4_IRQ = 8,
    CSI2_COMPLEXIO_ERRSOTSYNCHS3_IRQ = 7,
    CSI2_COMPLEXIO_ERRSOTSYNCHS2_IRQ = 6,
    CSI2_COMPLEXIO_ERRSOTSYNCHS1_IRQ = 5,

    CSI2_COMPLEXIO_ERRSOTHS5_IRQ = 4,
    CSI2_COMPLEXIO_ERRSOTHS4_IRQ = 3,
    CSI2_COMPLEXIO_ERRSOTHS3_IRQ = 2,
    CSI2_COMPLEXIO_ERRSOTHS2_IRQ = 1,
    CSI2_COMPLEXIO_ERRSOTHS1_IRQ = 0
} CSI2_COMPLEXIO_INTERRUPT_ID_T;

/* Call-back function prototype define */
typedef void (*csi2_callback_t) (CSI2_RETURN status, uint32 arg1, void *arg2);

/* CSI2 Interrupt request handler datastructure */
typedef struct {
    /* Function to be called when interrupt occurs */
    csi2_callback_t callback_func;

    /* Number of arguments to be passed to the callback function */
    uint32 arg1;

    /* starting address of the argument list */
    void *arg2;

} csi2_irq_handle_t;

/***************************************************************************/
/* FUNCTION PROTOTYPES */
/***************************************************************************/

/* ================================================================ */
/**
 *  csi2_reset() resets the CSI2 module. This is a software reset, implemented through SYSCONFIG register
 *  bit. Complex-IO interfaces have been reset separately here. It will return whether RESET was successful or 
 *  a failure.
 *
 *  @param  
 *  @return    status            	      Success/failure of the Reset
 *================================================================== */
/* ===================================================================
 *  @func     csi2_reset                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_reset(CSI2_DEVICE_T device_num, uint32 reset_complexio);

/* ================================================================ */
/* 
 *  Description:- Init should be called before calling any other function
 *  
 *
 *  @param   none
 
 *  @return    CSI2_RETURN     
 *================================================================== */
/* ===================================================================
 *  @func     csi2_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_init();

/* ================================================================ */
/* 
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable  clk
 *  
 *
 *  @param   :- none
 
 *  @return  :- CSI2_RETURN       
 *================================================================== */
/* ===================================================================
 *  @func     csi2_open                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_open();

/* ================================================================ */
/* 
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler 
 *  
 *
 *  @param   none
 
 *  @return   CSI2_RETURN      
 *================================================================== */
/* ===================================================================
 *  @func     csi2_close                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_close();

/* ================================================================ */
/* 
 *  Description:- This function validates the configuration structure for errors before actually configuring CSI2. 
 *
 *  @param   csi2_cfg_t *
 
 *  @return    CSI2_RETURN     
 *================================================================== */
/* ===================================================================
 *  @func     csi2_validate_params                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_validate_params(CSI2_DEVICE_T device_num,
                                 csi2_cfg_t * csi2_config);

/* ================================================================ */
/**
 *  csi2_config_power() configures the power settings of the CSI2 module. 
 *
 *  @param  master_standby_mode Standby-mode of the CSI2 module
 *  @param  auto_idle_ocp_clock OCP clock..either free running/gated
 *  @return    status            	      Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_power                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_power(CSI2_DEVICE_T device_num,
                              CSI2_MASTER_STANDBY_MODE_T master_standby_mode,
                              CSI2_OCP_CLOCK_MODE_T auto_idle_ocp_clock);

/* ================================================================ */
/**
 *  csi2_config_control() configures the control register of CSI2. This function should be called only when CSI2 
 *  Interface has been DISABLED!!!Interface is not enabled in this function, CSI2 Control register is only configured.
 *
 *  @param  cfg  a pointer to an instance of csi2_ctrl_cfg_t structure which stores the configuration settings
 *  @return    status            	      Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_control                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_control(CSI2_DEVICE_T device_num,
                                csi2_ctrl_cfg_t * cfg);

/* ================================================================ */
/**
 *  csi2_config_complexio() configures the control register of CSI2 Complex-IO. 
 *  
 *  @param  number indicates which Complex-IO is to be configured (1 or 2)
 *  @param  cfg  a pointer to an instance of csi2_complexio_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_complexio                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_complexio(CSI2_DEVICE_T device_num,
                                  CSI2_COMPLEXIO_NUMBER_T number,
                                  csi2_complexio_cfg_t * cfg);

/* ===================================================================
 *  @func     csi2_set_video_port                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void csi2_set_video_port(CSI2_DEVICE_T device_num, uint32 vp_only_enable,
                         uint32 video_clock_enable);

/* ================================================================ */
/**
 *  csi2_config_complexio_irq() configures the interrupt settings of the CSI2 Complex-IO. This function is made
 *  obsolete by the functions in csi2_irq.c which enable/disable a particular interrupt within complex-io. This function can
 *  be used to configure the interrupts in one-go after filling the data-structure with appropriate values.
 *  
 *  @param  number indicates which Complex-IO is to be configured (1 or 2)
 *  @param  irq_num  is a enumearated number of complexio- interrupt
 *  @param   on_off  indicates whether to enable/disable the interrupt
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_complexio_irq                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_complexio_irq(CSI2_DEVICE_T device_num,
                                      CSI2_COMPLEXIO_NUMBER_T number,
                                      CSI2_COMPLEXIO_INTERRUPT_ID_T irq_num,
                                      CSI2_CONTROL_FEATURE_BIT_T on_off);

/* ================================================================ */
/**
 *  csi2_config_phy() configures the PHY settings of the CSI2. 
 *  
 *  @param  cfg  a pointer to an instance of csi2_phy_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_phy                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_phy(CSI2_DEVICE_T device_num, csi2_phy_cfg_t * cfg);

/* ================================================================ */
/**
 *  csi2_config_timing() configures the timing registers of the complex-io interface of  the CSI2 module. 
 *  It primarily sets the Stop_State_Counter in terms of number of OCP clock cycles. 
 *
 *  @param  number indicates which Complex-IO is to be configured (1 or 2)
 *  @param  cfg  a pointer to an instance of csi2_timing_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_timing                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_timing(CSI2_DEVICE_T device_num,
                               csi2_timing_cfg_t * cfg);

/* ================================================================ */
/* 
 *  csi2_config_context_irq() configures the interrupt settings of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  irq_id  is the enumerated interrupt id 
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_context_irq                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_context_irq(CSI2_DEVICE_T device_num,
                                    csi2_context_number number,
                                    CSI2_CTX_INTERRUPT_ID_T irq_id,
                                    CSI2_CONTROL_FEATURE_BIT_T on_off);

/* ================================================================ */
/**
 *  csi2_init() configures the CSI2 receiver. It configures CSI2 receiver, complex-ios and PHY interface w/o
 *  enabling it. 
 *  
 *  @param  aConfig  a pointer to an instance of csi2_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config(CSI2_DEVICE_T device_num, csi2_cfg_t * aConfig);

/* ================================================================ */
/* 
 *  csi2_config_context_ping_buffer() configures the ping-address of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  address is a csi2_phyaddress_t structure which stores the ping-address
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_context_buffer                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_context_buffer(CSI2_DEVICE_T device_num,
                                       csi2_context_number number,
                                       CSI2_BUFFER_SELECTION_T buffer,
                                       csi2_phyaddress_t address,
                                       csi2_phyaddress_t offset);

/* ================================================================ */
/* 
 *  csi2_config_context_ping_buffer() configures the ping-address of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  address is a csi2_phyaddress_t structure which stores the ping-address
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_context_ping_pong_buffer                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_context_ping_pong_buffer(CSI2_DEVICE_T device_num,
                                                 csi2_context_number number,
                                                 CSI2_BUFFER_SELECTION_T buffer,
                                                 csi2_phyaddress_t address);

/* ================================================================ */
/* 
 *  csi2_config_context_transcode() configures the transcode settings of the context of the CSI2. 
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  cfg is a csi2_ctx_transcode_cfg_t structure which stores the pong-address
 *  @return   status   Success/failure of the configuration
 *================================================================== */
/* ===================================================================
 *  @func     csi2_config_context_transcode                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_context_transcode(CSI2_DEVICE_T device_num,
                                          csi2_context_number number,
                                          csi2_ctx_transcode_cfg_t * cfg);

/* ================================================================ */
/**
 *  csi2_config_context() configures the context of the CSI2. Context is not enabled in this function. It is
 *  only configured.
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @param  cfg  a pointer to an instance of csi2_ctx_cfg_t structure which stores the configuration settings
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_context                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_context(CSI2_DEVICE_T device_num,
                                csi2_context_number number,
                                csi2_ctx_cfg_t * cfg);

/* ================================================================ */
/**
 *  csi2_config_global_interrupt() configures the global interrupt settings of the CSI2 receiver.  
 *  
 *  @param  irq_id is the enumerated interrupt id
 *  @param   on_off  indicates whether to enable/disable the interrupt
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_config_global_interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_config_global_interrupt(CSI2_DEVICE_T device_num,
                                         CSI2_GLOBAL_INTERRUPT_ID_T irq_id,
                                         CSI2_CONTROL_FEATURE_BIT_T on_off);

/* ================================================================ */
/**
 *  csi2_enable_context() enables the specific context of the CSI2. 
 *
 *  @param  number indicates which context  is to be enabled (0-7)
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_enable_context                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_enable_context(CSI2_DEVICE_T device_num,
                                csi2_context_number number);

/* ================================================================ */
/**
 *  csi2_disable_context() disables the specific context of the CSI2. 
 *
 *  @param  number indicates which context  is to be disabled (0-7)
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_disable_context                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_disable_context(CSI2_DEVICE_T device_num,
                                 csi2_context_number number);

/* ================================================================ */
/**
 *  csi2_start() enables the CSI2 module, by enabling the complex-io interfaces and CSI2 receiver. User 
 *  as to separately enable the contexts (csi2_enable_context() function)which he wants to enable. 
 *  This function does not enables the contexts. For proper functioning, first enable the required context and then
 *  enable csi2 module.
 *
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_start                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_start(CSI2_DEVICE_T device_num);

/* ================================================================ */
/**
 *  csi2_disable() disables the CSI2 module, by disabling the physical interface and receiver.
 *
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_disable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_disable(CSI2_DEVICE_T device_num);

/* ================================================================ */
/**
 *  csi2_disable_immediate() disables the CSI2 module immediately, without waiting for the FEC 
 *  synch codes to be received.
 *
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_disable_immediate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_disable_immediate(CSI2_DEVICE_T device_num);

/* ================================================================ */
/**
 *  csi2_get_latest_buffer() returns the last buffer which was used to store a frame. 
 *  
 *  @param  number  indicates context number (0-7)
 *  @return   buffer indicates the buffer used (ping/pong)
 *================================================================== */

/* ===================================================================
 *  @func     csi2_get_latest_buffer                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_BUFFER_SELECTION_T csi2_get_latest_buffer(CSI2_DEVICE_T device_num,
                                               csi2_context_number number);

/* ================================================================ */
/**
 *   csi2_context_irq_enable_interrupt()
 *  Description:- This routine will enable the context interrupt specified by int_id, in the csi2 peripheral instance specified by dev_num
 *                      and the context specified in context_num.
 *  
 *
 *  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
 *  @param CSI2_CONTEXT_T context_num, is the complex io inside each csi2 peripheral instance.
 *  @param  CSI2_CTX_INTERRUPT_ID_T int_id, is the context interrupt.
 *  @return    CSI2_RETURN
 */
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_enable_interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_context_irq_enable_interrupt(CSI2_DEVICE_T dev_num,
                                              CSI2_CONTEXT_T context_num,
                                              CSI2_CTX_INTERRUPT_ID_T int_id);

/* ================================================================ */
/**
*   csi2_context_irq_disable_interrupt()
 *  Description:- This routine will disable the context interrupt specified by int_id, in the csi2 peripheral instance specified by dev_num
 *                      and the context specified in context_num.
 *  
 *
 *  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
 *  @param CSI2_CONTEXT_T context_num, is the complex io inside each csi2 peripheral instance.
 *  @param  CSI2_CTX_INTERRUPT_ID_T int_id, is the context interrupt.
 *  @return    CSI2_RETURN
 */
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_disable_interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_context_irq_disable_interrupt(CSI2_DEVICE_T dev_num,
                                               CSI2_CONTEXT_T context_num,
                                               CSI2_CTX_INTERRUPT_ID_T int_id);

/* ================================================================ */
/**
 *  csi2_setIntr_lineNum() sets interrupt line number
 *  only configured.
 *
 *  @param  number indicates which context  is to be configured (0-7)
 *  @return   status   Success/failure of the configuration
 *================================================================== */

/* ===================================================================
 *  @func     csi2_setIntr_lineNum                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_setIntr_lineNum(CSI2_DEVICE_T device_num,
                                 csi2_context_number number, uint32 lineNum);

/* ================================================================ */
/**
*   csi2_context_irq_hook_int_handler()
*  Description:-csi2_context_irq_hook_int_handler, this routine registers a handler for the interrupt specified by int_id, for the  
*                     context specified by context_num, and the devicenum is the particular instance of the csi2 device.
*  
*
*  @param CSI2_DEVICE_T devicenum,  is the csi2 peripheral instance
*  @param CSI2_CONTEXT_T context_num, is the context inside each csi2 peripheral instance.
*  @param csi2_complexio_interrup_id_t int_id, is the interrupt id witin the complex io module
*  @param Interrupt_Handle_T * handle, handle is the user parameter which has the callback and other data.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_hook_int_handler                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_context_irq_hook_int_handler(CSI2_DEVICE_T devicenum,
                                              CSI2_CONTEXT_T context_num,
                                              CSI2_CTX_INTERRUPT_ID_T int_id,
                                              Interrupt_Handle_T * handle);

/* ================================================================ */
/**
*   csi2_context_irq_unhook_int_handler()
*  Description:-csi2_context_irq_unhook_int_handler, this routine un-registers a handler for the interrupt specified by int_id, for the  
*                     context specified by context_num, and the devicenum is the particular instance of the csi2 device.
*  
*
*  @param CSI2_DEVICE_T devicenum,  is the csi2 peripheral instance
*  @param CSI2_CONTEXT_T context_num, is the context inside each csi2 peripheral instance.
*  @param csi2_complexio_interrup_id_t int_id, is the interrupt id witin the complex io module
*  @param Interrupt_Handle_T * handle, handle is the user parameter which has the callback and other data.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_unhook_int_handler                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_context_irq_unhook_int_handler(CSI2_DEVICE_T devicenum,
                                                CSI2_CONTEXT_T context_num,
                                                CSI2_CTX_INTERRUPT_ID_T int_id,
                                                Interrupt_Handle_T * handle);

/* ================================================================ */
/**
*   csi2_disable_interrupt()
*  Description:-this routine will disable a interrupt at the csi2 level, the interrupt is specified ny int_id.
*  
*
*  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
*  @param CSI2_GLOBAL_INTERRUPT_ID_T int_id, is the interrupt which needs to be enabled.
*
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_disable_interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_disable_interrupt(CSI2_DEVICE_T dev_num,
                                   CSI2_GLOBAL_INTERRUPT_ID_T int_id);

/* ================================================================ */
/**
*   
*  Description:-This routine will register a interrupt handler at csi2 interrupt management level.
*  
*
*  @param CSI2_DEVICE_T dev_num,
*  @param CSI2_GLOBAL_INTERRUPT_ID_T irq_id,
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_register_interrupt_handler                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_register_interrupt_handler(CSI2_DEVICE_T dev_num,
                                            CSI2_GLOBAL_INTERRUPT_ID_T irq_id,
                                            csi2_irq_callback_t callback_func,
                                            uint32 arg1, void *arg2);

/* ================================================================ */
/**
*   csi2_enable_interrupt()
*  Description:-this routine will enable a interrupt at the csi2 level, the interrupt is specified ny int_id.
*  
*
*  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
*  @param CSI2_GLOBAL_INTERRUPT_ID_T int_id, is the interrupt which needs to be enabled.
*
*/
/*================================================================== */

/* ===================================================================
 *  @func     csi2_enable_interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CSI2_RETURN csi2_enable_interrupt(CSI2_DEVICE_T dev_num,
                                  CSI2_GLOBAL_INTERRUPT_ID_T int_id);
								  
void csi2_wait_phy_reset();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
