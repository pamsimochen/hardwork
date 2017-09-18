/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_vpdma.h
 *
 * \brief VPS VPDMA HAL header file
 * This file exposes the HAL APIs of the VPS VPDMA.
 *
 */

#ifndef _VPSHAL_VPDMA_H
#define _VPSHAL_VPDMA_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/fvid2.h>

#ifdef TI_816X_BUILD
#include <ti/psp/vps/hal/vpshalVpdmaTI816x.h>
#endif

#ifdef TI_814X_BUILD
#include <ti/psp/vps/hal/vpshalVpdmaTI814x.h>
#endif

#ifdef TI_8107_BUILD
#include <ti/psp/vps/hal/vpshalVpdmaTI8107.h>
#endif


#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** Descriptor Sizes */
#define VPSHAL_VPDMA_DATA_DESC_SIZE            (32u)
#define VPSHAL_VPDMA_CTRL_DESC_SIZE            (16u)
#define VPSHAL_VPDMA_CONFIG_DESC_SIZE          (16u)

/** Descriptor Alignments */
#define VPSHAL_VPDMA_DESC_BYTE_ALIGN           (16u)
#define VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN (16u)
#define VPSHAL_VPDMA_LIST_SIZE_SHIFT           (0x4u)
/**
 *  List size is always in terms of 128 bit words so it is aligned on 16
 *  bytes boundary
 */
#define VPSHAL_VPDMA_LIST_SIZE_ALIGN           (16u)
#define VPSHAL_VPDMA_LIST_ADDR_ALIGN           (16u)
#define VPSHAL_VPDMA_LINE_STRIDE_ALIGN         (16u)
#define VPSHAL_VPDMA_BUF_ADDR_ALIGN            (16u)
#define VPSHAL_VPDMA_WR_DESC_BUF_ADDR_ALIGN    (32u)

/* VPDMA Macros */
#define VPSHAL_VPDMA_MAX_LIST                  (8u)
#define VPSHAL_VPDMA_MAX_CLIENTS               (63u)
#define VPSHAL_VPDMA_MAX_SI_SOURCE             (16u)
#define VPSHAL_VPDMA_MAX_LM_FID                (3u)

#define VPSHAL_VPDMA_MIN_FREE_CHANNEL          (193u)
/* TODO: VPDMA free channel should start from 192 channel number */
#define VPSHAL_VPDMA_MAX_FREE_CHANNEL          (255u)
#define VPSHAL_VPDMA_NUM_FREE_CHANNELS      (VPSHAL_VPDMA_MAX_FREE_CHANNEL -   \
                                             VPSHAL_VPDMA_MIN_FREE_CHANNEL + 1)

/** Type of the Descriptors */
#define VPSHAL_VPDMA_PT_DATA                   (0xAu)
#define VPSHAL_VPDMA_PT_CONFIG                 (0xBu)
#define VPSHAL_VPDMA_PT_CONTROL                (0xCu)

#define VPSHAL_VPDMA_LIST_ATTR_LISTBUSYOFFSET  (16u)
#define VPSHAL_VPDMA_PAYLOADSIZEALIGN          (16u)

/** Masks for addresses for input and output buffers */
#define VPSHAL_VPDMA_DATADESC_INBUFFERMASK     (0xFFFFFFFCu)
#define VPSHAL_VPDMA_DATADESC_INBUFFEROFFSET   (2u)
#define VPSHAL_VPDMA_DATADESC_OUTBUFFERMASK    (0xFFFFFFE0u)
#define VPSHAL_VPDMA_DATADESC_OUTBUFFEROFFSET  (5u)

/* Max frame size in data descriptor */
#define VPSHAL_VPDMA_MAX_FRAME_WIDTH            (4096u)
#define VPSHAL_VPDMA_MAX_FRAME_HEIGHT           (2048u)

/* Minumum width is suggested by hardware team. This
 * is not the actual hardware restriction, but combination
 * of hardware paths like SC_H, DEI_H etc
 */
#define VPSHAL_VPDMA_MIN_FRAME_WIDTH            (24u)
#define VPSHAL_VPDMA_MIN_FRAME_HEIGHT           (8u)

#define VPSHAL_VPDMA_INBOUND_DATA_DESC          (0x0u)
#define VPSHAL_VPDMA_OUTBOUND_DATA_DESC         (0x1u)

#define VPSHAL_VPDMA_MAX_SKIP_DESC              (7u)

/** Graphics Flags Used in the graphics region datadescriptor to enable
 * specific feature */
#define VPSHAL_VPDMA_GRPX_START_REGION         (0x00000080u)
#define VPSHAL_VPDMA_GRPX_END_REGION           (0x00000100u)
#define VPSHAL_VPDMA_GRPX_SCALAR               (0x00000200u)
#define VPSHAL_VPDMA_GRPX_ANTI_FLICKER         (0x00000400u)
#define VPSHAL_VPDMA_GRPX_STENCIL              (0x00000800u)
#define VPSHAL_VPDMA_GRPX_BOUNDRY_BOX          (0x00008000u)
#define VPSHAL_VPDMA_GRPX_TRANSPARENCY         (0x00010000u)

/* Maximum client number used in reading/writing configuration through
 * VPI bus */
#define VPSHAL_VPDMA_MAX_CLIENT_NUM            (127u)


#define VPSHAL_VPDMA_LM_REG_START       (0x8000u)
#define VPSHAL_VPDMA_LM_REG_CNT         (0x100u)


//#define VPSHAL_VPDMA_ENABLE_VIRT_TO_PHY_MAP

#define VPSHAL_VPDMA_OCMC_PHYS_BASE            (0x40000000)

static inline UInt32 VpsHal_vpdmaVirtToPhy(Ptr virt)
{
    #ifdef VPSHAL_VPDMA_ENABLE_VIRT_TO_PHY_MAP
    if ( virt && (((UInt32)virt & 0xF0000000) == 0x0))
    {
        return ((UInt32)virt + VPSHAL_VPDMA_OCMC_PHYS_BASE);
    }
    #endif

    return (UInt32)(virt);
}

static inline UInt32 VpsHal_vpdmaPhyToVirt(Ptr phys)
{
    #ifdef VPSHAL_VPDMA_ENABLE_VIRT_TO_PHY_MAP
    if ( (((UInt32)phys & 0xF0000000) == VPSHAL_VPDMA_OCMC_PHYS_BASE))
    {
        return ((UInt32)phys & 0x0FFFFFFF);
    }
    #endif

    return (UInt32)(phys);
}
/**
 *  \brief Enumeration for VPDMA firmware versions
 */
typedef enum
{
    VPSHAL_VPDMA_VER_195 = 0,
    VPSHAL_VPDMA_VER_19B,
    VPSHAL_VPDMA_VER_19F,
    VPSHAL_VPDMA_VER_1A0,
    VPSHAL_VPDMA_VER_1A1,
    VPSHAL_VPDMA_VER_1A3,
    VPSHAL_VPDMA_VER_1A4,
    VPSHAL_VPDMA_VER_1A5,
    VPSHAL_VPDMA_VER_1A6,
    VPSHAL_VPDMA_VER_1AD,
    VPSHAL_VPDMA_VER_1AE,
    VPSHAL_VPDMA_VER_1B0,
    VPSHAL_VPDMA_VER_1B2,
    VPSHAL_VPDMA_VER_1B5,
    VPSHAL_VPDMA_VER_1B6,
    VPSHAL_VPDMA_VER_1B7,
    VPSHAL_VPDMA_VER_1XX_MAX = 100,
    VPSHAL_VPDMA_VER_286 = VPSHAL_VPDMA_VER_1XX_MAX + 1,
    VPSHAL_VPDMA_VER_287,
    VPSHAL_VPDMA_VER_288,
    VPSHAL_VPDMA_VER_28A,
    VPSHAL_VPDMA_VER_28C,
    VPSHAL_VPDMA_VER_28D,
    VPSHAL_VPDMA_VER_290,
    VPSHAL_VPDMA_VER_MAX

}VpsHal_VpdmaVersion;

/**
 *  \brief Enums for data types that VPDMA channel can
 *  accept. This enum can be used directly to set the data type in data
 *  descriptor
 */
typedef enum
{
    VPSHAL_VPDMA_CHANDT_RGB565 = 0,
    /**< RGB  565 */
    VPSHAL_VPDMA_CHANDT_ARGB1555 = 1,
    /**< ARGB 1555 */
    VPSHAL_VPDMA_CHANDT_ARGB4444 = 2,
    /**< ARGB 4444 */
    VPSHAL_VPDMA_CHANDT_RGBA5551 = 3,
    /**< RGBA 5551 */
    VPSHAL_VPDMA_CHANDT_RGBA4444 = 4,
    /**< RGBA 4444 */
    VPSHAL_VPDMA_CHANDT_ARGB6666 = 5,
    /**< ARGB 6666 */
    VPSHAL_VPDMA_CHANDT_RGB888 = 6,
    /**< RGB  888 */
    VPSHAL_VPDMA_CHANDT_ARGB8888 = 7,
    /**< ARGB 8888 */
    VPSHAL_VPDMA_CHANDT_RGBA6666 = 8,
    /**< RGBA 6666 */
    VPSHAL_VPDMA_CHANDT_RGBA8888 = 9,
    /**< RGBA 8888 */
    /*new for PG2.0*/
    VPSHAL_VPDMA_CHANDT_BGR565 = 0x10,
    /**< BGR 565 */
    VPSHAL_VPDMA_CHANDT_ABGR1555 = 0x11,
    /**< ABGR 1555 */
    VPSHAL_VPDMA_CHANDT_ABGR4444 = 0x12,
    /**< ABGR 4444 */
    VPSHAL_VPDMA_CHANDT_BGRA5551 = 0x13,
    /**< BGRA 1555 */
    VPSHAL_VPDMA_CHANDT_BGRA4444 = 0x14,
    /**< BGRA 4444 */
    VPSHAL_VPDMA_CHANDT_ABGR6666 = 0x15,
    /**< ABGR 6666 */
    VPSHAL_VPDMA_CHANDT_BGR888 = 0x16,
    /**< BGR 888 */
    VPSHAL_VPDMA_CHANDT_ABGR8888 = 0x17,
    /**< ABGR 8888 */
    VPSHAL_VPDMA_CHANDT_BGRA6666 = 0x18,
    /**< BGRA 6666 */
    VPSHAL_VPDMA_CHANDT_BGRA8888 = 0x19,
    /**< BGRA 8888 */
    VPSHAL_VPDMA_CHANDT_BITMAP8 = 0x20,
    /**< 8 bit clut */
    VPSHAL_VPDMA_CHANDT_BITMAP4_LOWER = 0x22,
    /**< 4 bit clut with lower address */
    VPSHAL_VPDMA_CHANDT_BITMAP4_UPPER = 0x23,
    /**< 4 bit clut with upper address */
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET0 = 0x24,
    /**< 2 bit clut with offset0 */
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET1 = 0x25,
    /**< 2 bit clut with offset1 */
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET2 = 0x26,
    /**< 2 bit clut with offset2 */
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET3 = 0x27,
    /**< 2 bit clut with offset3 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET0 = 0x28,
    /**< 1 bit clut with offset0 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET1 = 0x29,
    /**< 1 bit clut with offset1 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET2 = 0x2A,
    /**< 1 bit clut with offset2 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET3 = 0x2B,
    /**< 1 bit clut with offset3 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET4 = 0x2C,
    /**< 1 bit clut with offset4 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET5 = 0x2D,
    /**< 1 bit clut with offset5 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET6 = 0x2E,
    /**< 1 bit clut with offset6 */
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET7 = 0x2F,
    /**< 1 bit clut with offset7 */
    /*new for PG2.0*/
    VPSHAL_VPDMA_CHANDT_BITMAP8_BGRA32 = 0x30,
    /**< 8 bit clut with BGRA32 format */
    VPSHAL_VPDMA_CHANDT_BITMAP4_LOWER_BGRA32 = 0x32,
    /**< 4 bit clut lower address with BGRA32 format */
    VPSHAL_VPDMA_CHANDT_BITMAP4_UPPER_BGRA32 = 0x33,
    /**< 4 bit clut UPPER address with BGRA32 format */
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET0_BGRA32 = 0x34,
    /**< 2 bit clut with offset0 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET1_BGRA32 = 0x35,
    /**< 2 bit clut with offset1 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET2_BGRA32 = 0x36,
    /**< 2 bit clut with offset2 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET3_BGRA32 = 0x37,
    /**< 2 bit clut with offset3 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET0_BGRA32 = 0x38,
    /**< 1 bit clut with offset0 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET1_BGRA32 = 0x39,
    /**< 1 bit clut with offset1 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET2_BGRA32 = 0x3A,
    /**< 1 bit clut with offset2 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET3_BGRA32 = 0x3B,
    /**< 1 bit clut with offset3 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET4_BGRA32 = 0x3C,
    /**< 1 bit clut with offset4 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET5_BGRA32 = 0x3D,
    /**< 1 bit clut with offset5 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET6_BGRA32 = 0x3E,
    /**< 1 bit clut with offset6 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET7_BGRA32 = 0x3F,
    /**< 1 bit clut with offset7 BGRA32 format*/
    VPSHAL_VPDMA_CHANDT_Y444 = 0,
    /**< Y 4:4:4 */
    VPSHAL_VPDMA_CHANDT_Y422 = 1,
    /**< Y 4:2:2 */
    VPSHAL_VPDMA_CHANDT_Y420 = 2,
    /**< Y 4:2:0 */
    VPSHAL_VPDMA_CHANDT_YC420 = 3,
    /**< YC 4:2:0 Y on LSB, C on MSB */
    VPSHAL_VPDMA_CHANDT_C444 = 4,
    /**< C 4:4:4 */
    VPSHAL_VPDMA_CHANDT_C422 = 5,
    /**< C 4:2:2 */
    VPSHAL_VPDMA_CHANDT_C420 = 6,
    /**< C 4:2:0 */
    VPSHAL_VPDMA_CHANDT_YC422 = 7,
    /**< YC 4:2:2 Y on LSB, C on MSB i.e. YUYV */
    VPSHAL_VPDMA_CHANDT_YC444 = 8,
    /**< YC 4:4:4 Y on LSB, C on MSB */
    VPSHAL_VPDMA_CHANDT_MV = 3,
    /**< Motion Vector and MVSTM are 4-bit data. Program one less. */
    VPSHAL_VPDMA_CHANDT_STENCIL = 0,
    /**< Stencil Data is 1-bit. Program one less. */
    VPSHAL_VPDMA_CHANDT_CLUT = 7,
    /**< CLUT Table is 8-bit. Program one less. */
    VPSHAL_VPDMA_CHANDT_ANC = 8,
    /**< Ancillary Data is 8-bit. Program one less. */
    VPSHAL_VPDMA_CHANDT_YCb422 = 0x17,
    /**< YC 4:2:2 Y on LSB, C on MSB i.e. YVYU */
    VPSHAL_VPDMA_CHANDT_CY422 = 0x27,
    /**< CY 4:2:2 C on LSB, Y on MSB i.e. UYVY */
    VPSHAL_VPDMA_CHANDT_CbY422 = 0x37,
    /**< CY 4:2:2 C on LSB, Y on MSB i.e. VYUY */
    VPSHAL_VPDMA_CHANDT_INVALID = 0xFF
    /**< Invalid Data Type */
} VpsHal_VpdmaChanDT;

/**
 *  \brief This defines the configuration destinations.
 *  This enum can be used directly to set the destination field of
 *  configuration descriptor
 */
typedef enum
{
    VPSHAL_VPDMA_CONFIG_DEST_MMR = 0,
    /**< Destination is MMR client */
    VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX0,
    /**< Graphics 0 scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX1,
    /**< Graphics 1 scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX2,
    /**< Graphics 2 scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC1,
    /**< Primary path scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC2,
    /**< Aux path scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC5,
    /**< Write back 2 scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC3,
    /**< Video Input Port 0 scalar */
    VPSHAL_VPDMA_CONFIG_DEST_SC4
    /**< Video Input Port 1 scalar */
} VpsHal_VpdmaConfigDest;

/**
 *  \brief This defines the type of control descriptor.
 *  This can be directly used in descriptor to the set the control
 *  descriptor type */
typedef enum
{
    VPSHAL_VPDMA_CDT_SOC = 0,
    /**< Sync on Client */
    VPSHAL_VPDMA_CDT_SOL,
    /**< Sync on List */
    VPSHAL_VPDMA_CDT_SOR,
    /**< Sync on Register */
    VPSHAL_VPDMA_CDT_SOT,
    /**< Sync on LM Timer */
    VPSHAL_VPDMA_CDT_SOCH,
    /**< Sync on Channel */
    VPSHAL_VPDMA_CDT_INTR_CHANGE,
    /**< Client interrupt change */
    VPSHAL_VPDMA_CDT_SI,
    /**< Send Interrupt */
    VPSHAL_VPDMA_CDT_RL,
    /**< Reload List */
    VPSHAL_VPDMA_CDT_ABT_CHANNEL,
    /**< Abort Channel */
    VPSHAL_VPDMA_CDT_TOGGLE_FID
    /**< Toggle LM Fid */
} VpsHal_VpdmaCtrlDescType;

/**
 *  \brief This defines the sync events for the sync on client
 *  control descriptor
 */
typedef enum
{
    VPSHAL_VPDMA_SOC_EOF = 0,
    /**< End of Frame */
    VPSHAL_VPDMA_SOC_SOF,
    /**< Start of Frame */
    VPSHAL_VPDMA_SOC_EOL,
    /**< End of Line X */
    VPSHAL_VPDMA_SOC_SOL,
    /**< Start of Line X */
    VPSHAL_VPDMA_SOC_PIXEL,
    /**< Matching exact pixel */
    VPSHAL_VPDMA_SOC_ACTIVE,
    /**< Client is Active */
    VPSHAL_VPDMA_SOC_NOTACTIVE,
    /**< Client is not active */
    VPSHAL_VPDMA_SOC_FID_CHANGE01,
    /**< Field Change from 0 to 1 */
    VPSHAL_VPDMA_SOC_FID_CHANGE10,
    /**< Field Change from 1 to 0 */
    VPSHAL_VPDMA_SOC_EOEL
    /**< End of Every Line X */
} VpsHal_VpdmaSocEvent;

/**
 *  \brief Enum for type of the list supported by VPDMA
 */
typedef enum
{
    VPSHAL_VPDMA_LT_NORMAL = 0,
    /**< Normal List */
    VPSHAL_VPDMA_LT_SELFMODIFYING,
    /**< Self Modifying List */
    VPSHAL_VPDMA_LT_DEBUG = 7
    /**< List type for VPDMA debugging */
} VpsHal_VpdmaListType;

/**
 *  \brief Enum for type of memory
 */
typedef enum
{
    VPSHAL_VPDMA_MT_NONTILEDMEM = 0,
    /**< Non-Tiled Memory. */
    VPSHAL_VPDMA_MT_TILEDMEM
    /**< Tiled Memory. */
} VpsHal_VpdmaMemoryType;

/**
 *  \brief Enum for frame start event. This tells VPDMA
 *  which NF signal to use for a client. This has to be configured on the
 *  channel associated with client.
 */
typedef enum
{
    VPSHAL_VPDMA_FSEVENT_HDMI_FID = 0,
    /**< Change in value of hdmi_field_id */
    VPSHAL_VPDMA_FSEVENT_DVO2_FID,
    /**< Change in value of dvo2_field_id */
    VPSHAL_VPDMA_FSEVENT_HDCOMP_FID,
    /**< Change in value of hdcomp_field_id */
    VPSHAL_VPDMA_FSEVENT_SD_FID,
    /**< Change in value of sd_field_id */
    VPSHAL_VPDMA_FSEVENT_LM_FID0,
    /**< Use List Manager Internal Field - 0 */
    VPSHAL_VPDMA_FSEVENT_LM_FID1,
    /**< Use List Manager Internal Field - 1 */
    VPSHAL_VPDMA_FSEVENT_LM_FID2,
    /**< Use List Manager Internal Field - 2*/
    VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE
    /**< Start on channel active */
} VpsHal_VpdmaFSEvent;

/**
 *  Enum for number of line skip supported by VPDMA
 */
typedef enum
{
    VPSHAL_VPDMA_LS_1 = 0,
    /**< Skip 1 line */
    VPSHAL_VPDMA_LS_2
    /**< Skip 2 lines */
} VpsHal_VpdmaLineSkip;

/**
 *  VpsHal_VpdmaClient
 *  \brief Enum for the client used by the VPDMA
 */
typedef enum
{
    VPSHAL_VPDMA_CLIENT_INVALID = -1,
    /**< Invalid value, used as boundary */
    VPSHAL_VPDMA_CLIENT_DEI_HQ_1_CHROMA =   0,
    /**< DEI HQ Chroma port Field 1 */
    VPSHAL_VPDMA_CLIENT_DEI_HQ_1_LUMA,
    /**< DEI HQ Luma port Field 1 */
    VPSHAL_VPDMA_CLIENT_DEI_HQ_2_LUMA,
    /**< DEI HQ Luma port Field 2 */
    VPSHAL_VPDMA_CLIENT_DEI_HQ_2_CHROMA,
    /**< DEI HQ Chroma port Field 2 */
    VPSHAL_VPDMA_CLIENT_DEI_HQ_3_LUMA,
    /**< DEI HQ Luma port Field 3 */
    VPSHAL_VPDMA_CLIENT_DEI_HQ_3_CHROMA,
    /**< DEI HQ Chroma port Field 3 */

    VPSHAL_VPDMA_CLIENT_DEI_HQ_MV_IN    =   12,
    /**< DEI HQ MV Input port */

    VPSHAL_VPDMA_CLIENT_DEI_HQ_MV_OUT   =   15,
    /**< DEI HQ MV output port */

    VPSHAL_VPDMA_CLIENT_DEI_SC_OUT      =   17,
    /**< DEI SC output port */
    VPSHAL_VPDMA_CLIENT_PIP_WRBK,
    /**< PIP write back output port */
    VPSHAL_VPDMA_CLIENT_SC_IN_CHROMA,
    /**< SC Chroma input port */
    VPSHAL_VPDMA_CLIENT_SC_IN_LUMA,
    /**< SC Luma input port */

    VPSHAL_VPDMA_CLIENT_SC_OUT          =   29,
    /**< SC output port */
    VPSHAL_VPDMA_CLIENT_COMP_WRBK,
    /**< Compositor write back output port */
    VPSHAL_VPDMA_CLIENT_GRPX0,
    /**< Graphics instance 1 input port */
    VPSHAL_VPDMA_CLIENT_GRPX1,
    /**< Graphics instance 2 input port */
    VPSHAL_VPDMA_CLIENT_GRPX3,
    /**< Graphics instance 3 input port */
    VPSHAL_VPDMA_CLIENT_VIP0_LO_Y,
    /**< VIP instance 1 Luma Low output port */
    VPSHAL_VPDMA_CLIENT_VIP0_LO_UV,
    /**< VIP instance 1 Chroma Low output port */
    VPSHAL_VPDMA_CLIENT_VIP0_HI_Y,
    /**< VIP instance 1 Luma High output port */
    VPSHAL_VPDMA_CLIENT_VIP0_HI_UV,
    /**< VIP instance 1 Chroma High output port */
    VPSHAL_VPDMA_CLIENT_VIP1_LO_Y,
    /**< VIP instance 2 Luma Low output port */
    VPSHAL_VPDMA_CLIENT_VIP1_LO_UV,
    /**< VIP instance 2 Chroma Low output port */
    VPSHAL_VPDMA_CLIENT_VIP1_HI_Y,
    /**< VIP instance 2 Luma High output port */
    VPSHAL_VPDMA_CLIENT_VIP1_HI_UV,
    /**< VIP instance 2 Chroma High output port */
    VPSHAL_VPDMA_CLIENT_GRPX0_ST,
    /**< Graphics Instance 1 stencil input port */
    VPSHAL_VPDMA_CLIENT_GRPX1_ST,
    /**< Graphics Instance 2 stencil input port */
    VPSHAL_VPDMA_CLIENT_GRPX3_ST,
    /**< Graphics Instance 3 stencil input port */
    VPSHAL_VPDMA_CLIENT_NF_422_IN,
    /**< NF Input port */
    VPSHAL_VPDMA_CLIENT_NF_420_Y_IN,
    /**< NF YUV420 Luma Input port */
    VPSHAL_VPDMA_CLIENT_NF_420_UV_IN,
    /**< NF YUV420 Chroma Input port */
    VPSHAL_VPDMA_CLIENT_NF_420_Y_OUT,
    /**< NF YUV420 Luma Output port */
    VPSHAL_VPDMA_CLIENT_NF_420_UV_OUT,
    /**< NF YUV420 Chroma output port */

    VPSHAL_VPDMA_CLIENT_VBI_SDVENC      = 51,
    /**< SDVENC VBI output port */
    VPSHAL_VPDMA_CLIENT_VBI_CTL,
    /**< SDVENC VBI control output port */
    VPSHAL_VPDMA_CLIENT_HDMI_WRBK_OUT,
    /**< HDMI Writeback output port */
    VPSHAL_VPDMA_CLIENT_TRANS1_CHROMA,
    VPSHAL_VPDMA_CLIENT_TRANS1_LUMA,
    VPSHAL_VPDMA_CLIENT_TRANS2_CHROMA,
    VPSHAL_VPDMA_CLIENT_TRANS2_LUMA,
    VPSHAL_VPDMA_CLIENT_VIP0_ANC_A,
    /**< VIP0 Ancilary A output port */
    VPSHAL_VPDMA_CLIENT_VIP0_ANC_B,
    /**< VIP0 Ancilary B output port */
    VPSHAL_VPDMA_CLIENT_VIP1_ANC_A,
    /**< VIP1 Ancilary A output port */
    VPSHAL_VPDMA_CLIENT_VIP1_ANC_B,
    /**< VIP1 Ancilary B output port */
    VPSHAL_VPDMA_CLIENT_MAX
    /**< Enumeration Guard */
}VpsHal_VpdmaClient;

typedef enum
{
    VPSHAL_VPDMA_LM_0 = 0,
    /**< repeat lines twice each output data line gets 2 times the number
         of frame lines */
    VPSHAL_VPDMA_LM_1,
    /**< each line once with Line Buffer Disabled, so no mirroring. Each
         line gets frame lines with identical data */
    VPSHAL_VPDMA_LM_2,
    /**< Each line seen once Mirroring is enabled so the top lines get
         the top lines repeated at the top of the frame and the bottom
         lines have the bottom lines repeated. Each line of data gets
         frame lines + number of buffered lines */
    VPSHAL_VPDMA_LM_3
    /**< Each line once only on one line. Each data line gets number of
         frame lines divided by number of buffered lines */
} VpsHal_VpdmaLineMode;

/**
 *  \brief Enum for specifying transparency mask for graphics region
 */
typedef enum
{
    VPSHAL_VPDMA_GTM_NOMASKING = 0,
    /**< No Masking */
    VPSHAL_VPDMA_GTM_MASK1BIT,
    /**< Mask 0 bit */
    VPSHAL_VPDMA_GTM_MASK2BIT,
    /**< Mask [1:0] bits */
    VPSHAL_VPDMA_GTM_MASK3BIT
    /**< Mask [2:0] bits */
} VpsHal_VpdmaGrpxTransMask;

/**
 *  Enum for specifying blending type for graphics region
 */
typedef enum
{
    VPSHAL_VPDMA_GBT_NOBLENDING = 0,
    /**< No Blending */
    VPSHAL_VPDMA_GBT_GLOBALBLENDING,
    /**< Global Blending */
    VPSHAL_VPDMA_GBT_CLUTBLENDING,
    /**< Clut Blending */
    VPSHAL_VPDMA_GBT_PIXELBLENDING
    /**< Pixel Alpha based blending */
} VpsHal_VpdmaGrpxBlendType;

/**
 *  Enum for specifying configuration command type i.e. Direct or
 *  indirect
 */
typedef enum
{
    VPSHAL_VPDMA_CCT_INDIRECT = 0,
    /**< Payload is stored somewhere in memory */
    VPSHAL_VPDMA_CCT_DIRECT
    /**< Payload is contiguous with config desc */
} VpsHal_VpdmaConfigCmdType;

/**
 *  \brief Enum for specifying configuration descriptor payload type
 *  i.e. Address set or Block set
 */
typedef enum VpsHal_VpdmaConfigPayloadType_T
{
    VPSHAL_VPDMA_CPT_ADDR_DATA_SET = 0,
    /**< Payload consists of Blocks of addresses */
    VPSHAL_VPDMA_CPT_BLOCK_SET
    /**< Payload consists of simple a block of data */
} VpsHal_VpdmaConfigPayloadType;

typedef enum
{
    VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED = 0,
    /**< Unlimited line size */
    VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_352_PIXELS = 4,
    /**< Maximum 352 Pixels per line */
    VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_768_PIXELS = 5,
    /**< Maximum 768 Pixels per line */
    VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_1280_PIXELS = 6,
    /**< Maximum 1280 Pixels per line */
    VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_1920_PIXELS = 7
    /**< Maximum 1920 Pixels per line */
} VpsHal_VpdmaOutBoundMaxWidth;

typedef enum
{
    VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED = 0,
    /**< Unlimited frame size */
    VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_288_LINES = 4,
    /**< Maximum 288 lines per frame */
    VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_576_LINES = 5,
    /**< Maximum 576 lines per frame */
    VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_720_LINES = 6,
    /**< Maximum 720 lines per frame */
    VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_1080_LINES = 7
    /**< Maximum 1080 lines per frame */
} VpsHal_VpdmaOutBoundMaxHeight;

/**
 *  \brief Enum for OCP bus priority in the data descriptor
 */
typedef enum
{
    VPSHAL_VPDMA_DATADESCPRIO_0 = 0,
    /**< Highest Priority */
    VPSHAL_VPDMA_DATADESCPRIO_1,
    VPSHAL_VPDMA_DATADESCPRIO_2,
    VPSHAL_VPDMA_DATADESCPRIO_3,
    VPSHAL_VPDMA_DATADESCPRIO_4,
    VPSHAL_VPDMA_DATADESCPRIO_5,
    VPSHAL_VPDMA_DATADESCPRIO_6,
    VPSHAL_VPDMA_DATADESCPRIO_7
    /**< Lowest Priority */
} VpsHal_VpdmaDataDescPrio;

/**
 * \brief VpsHal_VpdmaLmFidCtrl
 *  Enum defines possible value for LM FID control descriptor
 */
typedef enum
{
    VPSHAL_VPDMA_LM_FID_UNCHANGED = 0,
    /**< FID remains unchanged */
    VPSHAL_VPDMA_LM_FID_TOGGLE,
    /**< FID Toggles */
    VPSHAL_VPDMA_LM_FID_CHANGE_0,
    /**< FID becomse 0 */
    VPSHAL_VPDMA_LM_FID_CHANGE_1
    /**< FID becomse 1 */
} VpsHal_VpdmaLmFidCtrl;

/**
 * \brief VpsHal_VpdmaPath
 *  Enum defining VPDMA path
 */
typedef enum
{
    VPSHAL_VPDMA_PATH_VIP0_LO_UV = 0,
    VPSHAL_VPDMA_PATH_VIP0_LO_Y,
    VPSHAL_VPDMA_PATH_VIP0_UP_UV,
    VPSHAL_VPDMA_PATH_VIP0_UP_Y,
    VPSHAL_VPDMA_PATH_VIP1_LO_UV,
    VPSHAL_VPDMA_PATH_VIP1_LO_Y,
    VPSHAL_VPDMA_PATH_VIP1_UP_UV,
    VPSHAL_VPDMA_PATH_VIP1_UP_Y,
    VPSHAL_VPDMA_PATH_VIP0_ANCA,
    VPSHAL_VPDMA_PATH_VIP0_ANCB,
    VPSHAL_VPDMA_PATH_VIP1_ANCA,
    VPSHAL_VPDMA_PATH_VIP1_ANCB,
    VPSHAL_VPDMA_PATH_SEC0,
    VPSHAL_VPDMA_PATH_SEC1,
    VPSHAL_VPDMA_PATH_PRI,
    VPSHAL_VPDMA_PATH_WB0,
    VPSHAL_VPDMA_PATH_AUX,
    VPSHAL_VPDMA_PATH_WB1,
    VPSHAL_VPDMA_PATH_BP0,
    VPSHAL_VPDMA_PATH_BP1,
    VPSHAL_VPDMA_PATH_WB2,
    VPSHAL_VPDMA_PATH_GRPX0,
    VPSHAL_VPDMA_PATH_GRPX0_STENC,
    VPSHAL_VPDMA_PATH_GRPX1,
    VPSHAL_VPDMA_PATH_GRPX1_STENC,
    VPSHAL_VPDMA_PATH_GRPX2,
    VPSHAL_VPDMA_PATH_GRPX2_STENC,
    VPSHAL_VPDMA_PATH_VBI_HD,
    VPSHAL_VPDMA_PATH_VBI_SD,
    VPSHAL_VPDMA_PATH_NF,
    VPSHAL_VPDMA_MAX_PATH
} VpsHal_VpdmaPath;



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief VPDMA InBound Data Descriptor. Used for display to submit the
 *  data descriptor to VPDMA
 */
typedef struct
{
    UInt32 lineStride:16;
    UInt32 oddSkip:3;
    UInt32 reserved1:1;
    UInt32 evenSkip:3;
    UInt32 oneD:1;
    UInt32 fieldId:1;
    UInt32 notify:1;
    UInt32 dataType:6;
    UInt32 transferHeight:16;
    /**< Maximum transfer height is 2048 */
    UInt32 transferWidth:16;
    /**< Maximum transfer width is 4096 */
    UInt32 address:32;
    UInt32 nextChannel:9;
    UInt32 priority:3;
    UInt32 reserved2:2;
    UInt32 mosaicMode:1;
    UInt32 reserved3:1;
    UInt32 channel:9;
    UInt32 direction:1;
    UInt32 memType:1;
    UInt32 descType:5;
    UInt32 frameHeight:16;
    /**< Maximum transfer height is 2048 */
    UInt32 frameWidth:16;
    /**< Maximum transfer width is 4096 */
    UInt32 verticalStart:16;
    UInt32 horizontalStart:16;
    UInt32 clientSpecific1:32;
    UInt32 clientSpecific2:32;
} VpsHal_VpdmaInDataDesc;

/**
 *  \brief VPDMA OutBound Data Descriptor. Used for capture to submit the
 *  data descriptor to VPDMA. It tells VPDMA whether to drop the data,
 *  whether to write back the data descriptor for captured buffers.
 */
typedef struct
{
    UInt32 lineStride:16;
    UInt32 oddSkip:3;
    UInt32 reserved1:1;
    UInt32 evenSkip:3;
    UInt32 oneD:1;
    UInt32 reserved2:1;
    UInt32 notify:1;
    UInt32 dataType:6;
    UInt32 reserved3:32;
    UInt32 address:32;
    UInt32 nextChannel:9;
    UInt32 priority:3;
    UInt32 reserved4:3;
    UInt32 descSkip:1;
    UInt32 channel:9;
    UInt32 direction:1;
    UInt32 memType:1;
    UInt32 descType:5;
    UInt32 useDescReg:1;
    UInt32 dropData:1;
    UInt32 writeDesc:1;
    UInt32 reserved5:2;
    UInt32 outDescAddress:27;
    UInt32 maxHeight:3;
    /**< The Maximum allowable lines per frame */
    UInt32 reserved6:1;
    UInt32 maxWidth:3;
    /**< The Maximum allowable pixels per line */
    UInt32 reserved7:25;
    UInt32 clientSpecific1:32;
    UInt32 clientSpecific2:32;
} VpsHal_VpdmaOutDataDesc;

/**
 * Non bit-field based data descriptor overlay for efficient access to
 * descriptor in memory
*/
typedef struct
{
  UInt32 dataInfo;
  UInt32 reserved;
  UInt32 startAddr;
  UInt32 channelInfo;
  UInt32 descWriteInfo;
  UInt32 maxWidthHeight;
  UInt32 clientSpecific[2];
} VpsHal_VpdmaOutDataDescOverlay;

/**
 * Non bit-field based data descriptor overlay for efficient access
 * to descriptor in memory
 */
typedef struct
{
  UInt32 dataInfo;
  UInt32 transferWidthHeight;
  UInt32 startAddr;
  UInt32 channelInfo;
  UInt32 frameWidthHeight;
  UInt32 horzVertStart;
  UInt32 clientSpecific[2];
} VpsHal_VpdmaInDataDescOverlay;

/**
 *  \brief VPDMA Data Descriptor for the region of the graphics frame. Used
 *  for graphics driver to submit region attributes along with the
 *  region buffer to the attributes.
 */
typedef struct
{
    UInt32 lineStride:16;
    UInt32 oddSkip:3;
    UInt32 reserved1:1;
    UInt32 evenSkip:3;
    UInt32 reserved2:1;
    UInt32 fieldId:1;
    UInt32 notify:1;
    UInt32 dataType:6;
    UInt32 transferHeight:16;
    UInt32 transferWidth:16;
    UInt32 address:32;
    UInt32 nextChannel:9;
    UInt32 priority:3;
    UInt32 defaultSize:4;
    UInt32 channel:9;
    UInt32 direction:1;
    UInt32 memType:1;
    UInt32 descType:5;
    UInt32 regionHeight:16;
    UInt32 regionWidth:16;
    UInt32 verticalStart:16;
    UInt32 horizontalStart:16;
    UInt32 regionPriority:4;
    UInt32 reserved3:3;
    UInt32 regionAttr:9;
    UInt32 bbAlpha:8;
    UInt32 blendAlpha:8;
    UInt32 blendType:2;
    UInt32 reserved5:3;
    UInt32 enableTransparency:1;
    UInt32 transMask:2;
    UInt32 transColor:24;
} VpsHal_VpdmaRegionDataDesc;

/**
 *  \brief VPDMA Configuration Descriptor. Used for sending configuration to
 *  the modules. This configuration can be used to setting MMR
 *  registers or scalar coefficients.
 */
typedef struct
{
    UInt32 destAddr:32;
    UInt32 dataLength:16;
    UInt32 reserved1:16;
    UInt32 payloadAddress:32;
    UInt32 payloadLength:16;
    UInt32 destination:8;
    UInt32 class:2;
    UInt32 direct:1;
    UInt32 descType:5;
} VpsHal_VpdmaConfigDesc;

/**
 *  \brief VPDMA address data sub block header in configuration overlay memory.
 */
typedef struct
{
    UInt32 nextClientAddr:32;
    UInt32 subBlockLength:16;
    UInt32 reserved1:16;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
} VpsHal_VpdmaSubBlockHeader;

/**
 *  \brief VPDMA configuration descriptor for the graphics frame. For the
 *  graphics, frame configuration is specified by configuration
 *  descriptor and region configuration is specifed by data
 *  descriptor. Scalar coefficients are also specified along with the frame
 *  configuration.
 */
typedef struct
{
    UInt32 payloadAddress:32;
    UInt32 address:24;
    UInt32 length:8;
    UInt32 payloadLength:16;
    UInt32 destination:8;
    UInt32 class:2;
    UInt32 direct:1;
    UInt32 descType:5;
} VpsHal_VpdmaGrpxFrameConfigDesc;

/**
 *  \brief Sync On Client control descriptor. Used for changing interrupt
 *  generation event and then waiting for that event to occur.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 lineCount:16;
    UInt32 pixelCount:16;
    UInt32 event:4;
    UInt32 reserved2:28;
    UInt32 ctrl:4;
    UInt32 reserved3:12;
    UInt32 channel:9;
    UInt32 reserved4:2;
    UInt32 descType:5;
} VpsHal_VpdmaSyncOnClientDesc;

/**
 *  \brief Sync On List control descriptor. Used to ensure that multiple lists
 *  have all reached a common point. Need to specify the bit-mask of
 *  all list to wait for.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
    UInt32 ctrl:4;
    UInt32 reserved4:12;
    UInt32 lists:9;
    UInt32 reserved5:2;
    UInt32 descType:5;
} VpsHal_VpdmaSyncOnListDesc;

/**
 *  \brief Sync On Register Control Descriptor. Used to wait for write to the
 *  LIST_STAT_SYNC register to the field for the list that
 *  the control descriptor is in.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
    UInt32 ctrl:4;
    UInt32 reserved4:12;
    UInt32 listNum:9;
    UInt32 reserved5:2;
    UInt32 descType:5;
} VpsHal_VpdmaSyncOnRegDesc;

/**
 *  \brief Sync on List Manager Timer Control Descriptor. Used to wait for
 *  number of cycles to elapse from the current time position.
 */
typedef struct
{
    UInt32 numCycles:16;
    UInt32 reserved1:16;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
    UInt32 ctrl:4;
    UInt32 reserved4:23;
    UInt32 descType:5;
} VpsHal_VpdmaSyncOnLmTimerDesc;

/**
 *  \brief Sync on Channel Control Descriptor. Used to wait for the specified
 *  channel to become free.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
    UInt32 ctrl:4;
    UInt32 reserved4:12;
    UInt32 channel:9;
    UInt32 reserved5:2;
    UInt32 descType:5;
} VpsHal_VpdmaSyncOnChannelDesc;

/**
 * \brief  Send Interrupt Control Descriptor. It causes the VPDMA to generate
 *  an interrupt on the list manager controlled interrupts as specified
 *  by the Source Field.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
    UInt32 ctrl:4;
    UInt32 reserved4:12;
    UInt32 source:9;
    UInt32 reserved5:2;
    UInt32 descType:5;
} VpsHal_VpdmaSendIntrDesc;

/**
 *  \brief Reload Control Descriptor. It causes descriptors after this
 *  descriptor in the original list to be dropped and a new list at
 *  the location and of the size specified in the descriptor.
 */
typedef struct
{
    UInt32 reloadAddr:32;
    UInt32 listSize:16;
    UInt32 reserved1:16;
    UInt32 reserved2:32;
    UInt32 ctrl:4;
    UInt32 reserved3:23;
    UInt32 descType:5;
} VpsHal_VpdmaReloadDesc;

/**
 *  \brief Abort Control Descriptor. Used to clear channel from issuing any
 *  more request. Any outstanding requests for that channel
 *  will complete as originally scheduled.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 reserved2:32;
    UInt32 reserved3:32;
    UInt32 ctrl:4;
    UInt32 reserved4:12;
    UInt32 channel:9;
    UInt32 reserved5:2;
    UInt32 descType:5;
} VpsHal_VpdmaAbortDesc;

/**
 *  \brief Toggle LM FID control descriptor. used if the clients set their
 *  frame source to LM FID. The read clients will start transmitting
 *  data upon the FID signal inside the LM changing value. This
 *  descriptor will cause the LM to toggle the value of the internal FID.
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 reserved2:32;
    UInt32 lmFidCtrl0:2;
    UInt32 lmFidCtrl1:2;
    UInt32 lmFidCtrl2:2;
    UInt32 reserved3:26;
    UInt32 ctrl:4;
    UInt32 reserved4:23;
    UInt32 descType:5;
} VpsHal_VpdmaToggleLmFidDesc;

/**
 * \brief Change Client Interrupt Source control descriptor.
 *        Expected to be used to configure the VPDMA to generate interrupt after
 *        specified number of lines or specified number of pixels. If required
 *        to get an interrupt after specified number of lines, update lineCount
 *        and set pixelCount to 0x0
 */
typedef struct
{
    UInt32 reserved1:32;
    UInt32 lineCount:16;
    UInt32 pixelCount:16;
    UInt32 event:4;
    UInt32 reserved2:28;
    UInt32 ctrl:4;
    UInt32 reserved3:12;
    UInt32 source:9;
    UInt32 reserved4:2;
    UInt32 descType:5;

}VpsHal_VpdmaChangeClientIntSrc;


/**
 *  struct VpsHal_VpdmaInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 *
 */
typedef struct
{
    UInt32             vpdmaBaseAddress;
    /**< Base address of the VPDMA instance */
    UInt32             vpsBaseAddress;
    /**< Base address of the VPS */
} VpsHal_VpdmaInstParams;

/**
 *  struct VpsHal_VpdmaInDescParams
 *  \brief This structure is for creating
 *  inbound data descriptor. Upper layer passes pointer of this structure to
 *  the createInBoundDataDesc function to create the data data descriptor in
 *  the given memory.
 *
 */
typedef struct
{
    VpsHal_VpdmaChannel channel;
    /**< VPDMA Channel number for which data descriptor is to be created */
    VpsHal_VpdmaChanDT dataType;
    /**< Channel Data Type */
    UInt16 transWidth;
    /**< Width of the buffer. VPDMA provides feature using which it can read
         small buffer and provide larger frame to the downstream module by
         filing up the remaining buffer with the background color. Transfer size
         refers to the input buffer size and frame size refers to the target
         buffer provided to the downstream module.*/
    UInt16 transHeight;
    /**< Height of the buffer */
    UInt16 frameWidth;
    /**< Width of frame */
    UInt16 frameHeight;
    /**< Height of the frame */
    UInt16 startX;
    /**< Horizontal start position of the transfer window in frame window */
    UInt16 startY;
    /**< Vertical start position of the transfer window in frame window */
    UInt16 lineStride;
    /**< Line stride in bytes between two lines in transfer window */
    VpsHal_VpdmaLineSkip lineSkip;
    /**< Number of lines to skip after each line in transfer window. This along
         with the lineStride is used to calculate next line address */
    VpsHal_VpdmaChannel nextChannel;
    /**< For virtual video buffer or region base graphics, this parameter is
         used to the specify channel number of the next free channel */
    UInt32 notify;
    /**< Fires notify interrupt for this list at end of data transfer */
    VpsHal_VpdmaDataDescPrio priority;
    /**< Data descriptor priority */
    VpsHal_VpdmaMemoryType  memType;
    /**< Type of memory i.e. Tiled or Non-Tiled. */
    UInt32 is1DMem;
    /**< Memory type */
} VpsHal_VpdmaInDescParams;

/**
 *  struct VpsHal_VpdmaOutDescParams
 *  \brief This structure is for creating
 *  outbound data descriptor. Upper layer passes pointer of this structure to
 *  the createOutBoundDataDesc function to create the data data descriptor in
 *  the given memory.
 *
 */
typedef struct
{
    VpsHal_VpdmaChannel channel;
    /**< VPDMA Channel number for which data descriptor is to be created */
    VpsHal_VpdmaChanDT dataType;
    /**< Channel Data Type */
    UInt16 lineStride;
    /**< Line stride in bytes between two lines in transfer window */
    VpsHal_VpdmaLineSkip lineSkip;
    /**< Number of lines to skip after each line in transfer window.This along
         with the lineStride is used to calculate next line address */
    UInt32 notify;
    /**< Fires notify interrupt for this list at end of data transfer */
    VpsHal_VpdmaDataDescPrio priority;
    /**< Data descriptor priority */
    VpsHal_VpdmaMemoryType  memType;
    /**< Type of memory i.e. Tiled or Non-Tiled. */
    VpsHal_VpdmaChannel nextChannel;
    /**< Next channel to be programmed in the outbound descriptor */
    VpsHal_VpdmaOutBoundMaxWidth maxWidth;
    /**< The maximum number of pixes per line */
    VpsHal_VpdmaOutBoundMaxHeight maxHeight;
    /**< The maximum number of lines per frame */
    UInt32 is1DMem;
} VpsHal_VpdmaOutDescParams;

/**
 *  struct VpsHal_VpdmaRegionDataDescParams
 *  \brief This structure is for creating
 *  graphics region data descriptor. Upper layer passes pointer of this
 *  structure to the createRegionDataDesc function to create the data
 *  descriptor in the given memory. Parametes starting from channel to
 *  nextChannel in the below structure are same as the inbound data descriptor.
 *
 */
typedef struct
{
    VpsHal_VpdmaChannel channel;
    VpsHal_VpdmaChanDT dataType;
    UInt16 regionWidth;
    UInt16 regionHeight;
    UInt16 startX;
    UInt16 startY;
    UInt16 lineStride;
    VpsHal_VpdmaLineSkip lineSkip;
    VpsHal_VpdmaChannel nextChannel;
    UInt8  regionPriority;
    /**< Priority of the region */
    UInt32 regionAttr;
    /**< Region Attributes used to set the graphics specific features */
    UInt8  blendAlpha;
    /**< Alpha value for the blending */
    UInt8  bbAlpha;
    /**< Boundry Box alpha Value */
    UInt32 transColor;
    /**< Transparency color */
    VpsHal_VpdmaGrpxTransMask transMask;
    /**< Transparency Mask */
    VpsHal_VpdmaGrpxBlendType blendType;
    /**< Type of the blending to be used for this region */
    UInt32 notify;
    /**< Fires notify interrupt for this list at end of data transfer */
    VpsHal_VpdmaDataDescPrio priority;
    /**< Data descriptor priority */
    VpsHal_VpdmaMemoryType  memType;
    /**< Type of memory i.e. Tiled or Non-Tiled. */
} VpsHal_VpdmaRegionDataDescParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_vpdmaInit
 *  \brief Function for initializing VPDMA HAL
 *  \param arg           For the Future use, not used currently.
 *
 *  \param initParams    Instance Specific parameters. Contains register base
 *                       address for VPDMA
 *  \param ver           Version of VPDMA that should be used.
 *  \return              Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaInit(VpsHal_VpdmaInstParams *initParams,
                     VpsHal_VpdmaVersion ver,
                     Ptr arg);


/**
 *  VpsHal_vpdmaDeInit
 *  \brief Function De Initialize VPDMA and associated HALs.
 *
 *  \param arg           For the Future use, not used currently.
 *
 *  \return              Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaDeInit(Ptr arg);


/**
 *  VpsHal_vpdmaPostList
 *  \brief Function for posting the list to the VPDMA. Once posted, VPDMA will
 *  start reading and processing the list. It is interrupt protected so
 *  can be called from the interrupt context also.
 *
 *  \param listNum          List Number
 *  \param listType         List Type i.e. Normal or Self Modifying
 *  \param listAddr         Physical address of the contiguous memory
 *                          containing list
 *  \param listSize         List size in bytes
 *  \param enableCheck      Flag to indicate whether parameter check needs to
 *                          be done or not.
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaPostList(UInt8 listNum,
                         VpsHal_VpdmaListType listType,
                         Ptr listAddr,
                         UInt32 listSize,
                         UInt32 enableCheck);

/**
 *  VpsHal_vpdmaSetBgColorRGB
 *  \brief Function to set the background color in RGB.
 *
 *  \param red              Value of Red component in the color
 *  \param green            Value of Blue component in the color
 *  \param blue             Value of Green component in the color
 *  \param alpha            Value of Alpha component in the color
 *  \returns                None
 */
Void VpsHal_vpdmaSetBgColorRGB(UInt8 red, UInt8 green, UInt8 blue, UInt8 alpha);

/**
 *  VpsHal_vpdmaSetBgColorYUV
 *  \brief Function to set the background color in RGB.
 *
 *  \param y                Value of Luma component in the color
 *  \param cb               Value of Cb component in the color
 *  \param cr               Value of Cr component in the color
 *  \return                 None
 */
Void VpsHal_vpdmaSetBgColorYUV(UInt8 y, UInt8 cb, UInt8 cr);

/**
 *  VpsHal_vpdmaSetFrameStartEvent
 *  \brief Function is used to set the frame start event for the channel/client.
 *  This function tells VPDMA to use a given source of NF to transfer data
 *  to the down stream module. VPDMA starts transfer of data at the NF
 *  signal only. If there is no NF signal, it will transfer data to the
 *  internal FIFO. When FIFO becomes full, that channel will be blocked.
 *  Upper layer should pass the channel number to set the frame
 *  start event and VPDMA HAL sets it in the client register. This VPDMA
 *  client is the one associated with the channel number. If event is to
 *  be set for multiplexed client, upper layer can pass any channel
 *  associated with that client to set frame start event
 *
 *  \param chanNum          Channel Number for which frame start event is
 *                          to be set
 *  \param fsEvent          Source of NF signal
 *  \param lineMode         NONE
 *  \param reqDelay         NONE
 *  \param ovlyPtr          NONE
 *  \param index            NONE
 *  \return                 None
 */
Void VpsHal_vpdmaSetFrameStartEvent(VpsHal_VpdmaChannel chanNum,
                                    VpsHal_VpdmaFSEvent fsEvent,
                                    VpsHal_VpdmaLineMode lineMode,
                                    UInt32 reqDelay,
                                    Ptr ovlyPtr,
                                    UInt32 index);
/**
 *  VpsHal_vpdmaStopList
 *  \brief Function to stop the self modiyfing list. Self modifying list is a
 *  free running list. It is like a circular list which runs on its own.
 *  This function is used to stop self modifying list. When stop bit is set,
 *  it completes the current transfer and stops the list.
 *
 *  \param listNum          List to be stopped
 *  \param listType         NONE
 *  \return                 None
 */
Void VpsHal_vpdmaStopList(UInt8 listNum, VpsHal_VpdmaListType listType);

/**
 *  \brief Function to load initial state machine in the VPDMA. This function
 *
 *  Should be called before calling anyother function of VPDMA. It
 *  loads the VPDMA firmware containing state machine and waits
 *  for loading to get completed.
 *
 *  \param firmwareAddr     Physical Address of the memory containing
 *                          firmware, must be 32byte aligned
 *  \param timeout          0: No timeout, else timeout loop count
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaLoadStateMachine(Ptr firmwareAddr, UInt32 timeout);

/**
 *  VpsHal_vpdmaCreateInBoundDataDesc
 *  \brief Function to create In Bound Data descriptor from the given parameters
 *  and in the given contiguous memory. Memory pointer given must contain
 *  physically contiguous memory because VPDMA works with that only.
 *
 *  \param memPtr           Pointer to physically contiguous memory into
 *                          which descriptor will be created
 *  \param descInfo         Pointer to structure containing in bound data
 *                          descriptor parameters
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateInBoundDataDesc(Ptr memPtr,
                                      VpsHal_VpdmaInDescParams *descInfo);

/**
 *  VpsHal_vpdmaSetSyncList
 *  \brief Function to register bit for the Sync on Register event.
 *
 *  \param listNum          List Number
 *  \return                 None
 */
Void VpsHal_vpdmaSetSyncList(UInt8 listNum);

/**
 *  VpsHal_vpdmaSetTransferDim
 *  \brief Function to set the transfer width and height. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param transWidth       Width of transfer
 *  \param transHeight      Height of transfer
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetTransferDim(Ptr memPtr,
                                              UInt16 transWidth,
                                              UInt16 transHeight)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->transferWidth = transWidth;
    ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->transferHeight = transHeight;
}

/**
 *  VpsHal_vpdmaCreateOutBoundDataDesc
 *  \brief Function to create Out Bound Data descriptor from the given
 *  parameters and in the given contiguous memory. Memory pointer given must
 *  contain physically contiguous memory because VPDMA works with that only.
 *
 *  \param memPtr           Pointer to physically contiguous memory into
 *                          which descriptor will be created
 *  \param descInfo         Pointer to structure containing out bound data
 *                          descriptor parameters
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateOutBoundDataDesc(Ptr memPtr,
                                       VpsHal_VpdmaOutDescParams *descInfo);

/**
 *  VpsHal_vpdmaCreateRegionDataDesc
 *  \brief This function is used to create the Region data descriptor in
 *  the given memory. Upper layer passes the parameters need to be set
 *  in second argument and this function makes data descriptor in
 *  the memory given in the first argument. Memory pointer given must
 *  contain physically contiguous memory because VPDMA works with that only.
 *
 *  \param memPtr           Pointer to physically contiguous memory into
 *                          which descriptor will be created
 *  \param descInfo         Pointer to structure containing region data
 *                          descriptor parameters
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateRegionDataDesc(Ptr memPtr,
                                    VpsHal_VpdmaRegionDataDescParams *descInfo);
/**
 *  VpsHal_vpdmaGetClientRegAdd
 *  \brief This function is used to get the base address for the client
 * registers. This address needs to pass to VPDMA to create the register
 * overlay for that client register
 *
 *  \param chanNum          Channel number for which the base address of client
 *                          reg is address.
 *  \return                 Returns the base address of the client register.
 */
Ptr VpsHal_vpdmaGetClientRegAdd(VpsHal_VpdmaChannel chanNum);

/**
 *  VpsHal_vpdmaReadConfig
 *  \brief Function to read configuration and coefficients through the VPI
 *  control bus. VPDMA provides two registers VPI_CTL_ADDRESS and
 *  VPI_CTL_DATA to to write/read the modules that are only on
 *  VPI CTL bus and not on the normal memory mapped bus. These
 *  include the GRPX modules and the coefficients for the SCs. This
 *  function reads the length number of words from the given
 *  client address for the given client. and stores them into the data
 *  pointer.
 *
 *  \param clientNum    Client Number. It must be between 0 to 127
 *  \param clientAddr   Address offset of the client. This address is
 *                      the local client address.
 *  \param length       Number of 32 bit words to be written from
 *                      the client. VPDMA writes data after collecting
 *                      128 bits of data so length must be 16 bytes/4 word
 *                      aligned.
 *  \param data         Pointer where read data will be stored. Memory
 *                      pointed by this pointer must be larde enough to
 *                      store length number of words.
 *  \return             Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaReadConfig(UInt8 clientNum,
                           UInt32 clientAddr,
                           UInt32 length,
                           UInt32 *data);

/**
 *  VpsHal_vpdmaWriteConfig
 *  \brief Function to write configuration and coefficients through the VPI
 *  control bus. VPDMA provides two registers VPI_CTL_ADDRESS and
 *  VPI_CTL_DATA to to write/read the modules that are only on
 *  VPI CTL bus and not on the normal memory mapped bus. These
 *  include the GRPX modules and the coefficients for the SCs. This
 *  function writes the length number of words from the given
 *  client address for the given client. and stores them into the data
 *  pointer
 *
 *  \param clientNum    Client Number. It must be between 0 to 127
 *  \param clientAddr   Address offset of the client. This address is
 *                      the local client address.
 *  \param length       Number of 32 bit words to be written from
 *                      the client. VPDMA writes data after collecting
 *                      128 bits of data so length must be 16 bytes/4 word
 *                      aligned.
 *  \param data         Pointer to the memory containing data to be
 *                      written.
 *  \return             Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaWriteConfig(UInt8 clientNum,
                            UInt32 clientAddr,
                            UInt32 length,
                            UInt32 *data);

/**
 *  VpsHal_vpdmaCalcRegOvlyMemSize
 *  \brief Function to calculate the size of memory required for creating
 *  VPDMA configuration address set for the given register offsets. It
 *  also returns the offsets of the virtual register in the register
 *  overlay from the starting location of the overlay in terms of
 *  words. This function will be used for the other HALs to calculate
 *  the size required to store configured through VPDMA and to get
 *  the virtual register offset so that they can modify virtual
 *  registers.
 *
 *  \param regOffset        List of Register Offsets
 *  \param numReg           Number of register offsets in first parameter
 *  \param virtRegOffset    Pointer to array in which virtual register
 *                          offsets will be returned. This offsets are
 *                          in terms of words.
 *  \return                 Size of memory in terms of bytes
 */
UInt32 VpsHal_vpdmaCalcRegOvlyMemSize(UInt32 * const *regOffset,
                                      UInt32 numReg,
                                      UInt32 *virtRegOffset);

/**
 *  VpsHal_vpdmaCreateRegOverlay
 *  Function is used to create complete configuration descriptor
 *  with the address set for the given registers offsets. It takes
 *  list of register offsets and pointer to memory, creates
 *  configuration descriptor and address set for the register set and
 *  returns the virtual addresses of the registers.
 *
 *  \param configRegList    List of Register Offsets
 *  \param numConfigReg     List of Register Offsets
 *  \param regOverlay       Pointer to array in which virtual register
 *                          offsets will be returned. This offsets are
 *                          in terms of words.
 *  \return                 Size of memory in terms of bytes
 */
Int VpsHal_vpdmaCreateRegOverlay(UInt32 * const *configRegList,
                                 UInt32 numConfigReg,
                                 Void *regOverlay);

/**
 *  VpsHal_vpdmaCreateCoeffOverlay
 *  \brief Function is used to create complete configuration descriptor for
 *  the scalar coefficient. It takes memory containing coefficients
 *  and type of scalar and returns configuration descriptor.
 *
 *  \param coeffMemory      Physical Address of the memory containing
                            coefficients
 *  \param coeffSize        Coefficient Memory Size In terms of bytes
 *  \param configDest       Scalar for which coefficients is to be
 *                          configured
 *  \param descPtr           Address of the Configuration Descriptor
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_vpdmaCreateCoeffOverlay(UInt32 *coeffMemory,
                                   UInt32 coeffSize,
                                   VpsHal_VpdmaConfigDest configDest,
                                   Void *descPtr);

/**
 *  VpsHal_vpdmaSetDescWriteAddrReg
 *  \brief Function to write descriptor write address in the register. This
 *  is the address at which VPDMA will write back descriptor for the
 *  already captured channels.
 *
 *  \param addr             Physical address where descriptor is to
 *                          be written
 *  \param numDesc          Maximum number of descriptors which could be
 *                          written to this memory.
 *  \return                 None
 */
Int32 VpsHal_vpdmaSetDescWriteAddrReg(Ptr addr, UInt16 size);

/**
 *  VpsHal_vpdmaCreateConfigDesc
 *  \brief Function to create the configuration descriptor in the given memory.
 *
 *  \param memPtr           Pointer memory in which config descriptor is
 *                          to be created
 *  \param dest             Config descriptor destination
 *  \param clss             This indicates whether payload is blocks of
 *                          data or single contiguous block.
 *  \param direct           This indicates whether payload is stored in
 *                          contiguous with the descriptor or stored in some
 *                          other memory location
 *  \param payloadSize      Payload size in bytes
 *  \param payloadAddr      Pointer to payload memory for indirect
 *                          configuration
 *  \param destAddr         Destination address offset used for block type
 *                          configuration class. Not used for address set.
 *  \param dataSize         NONE
 *
 *  \return                 None
 */
Void VpsHal_vpdmaCreateConfigDesc(Ptr memPtr,
                                  VpsHal_VpdmaConfigDest dest,
                                  VpsHal_VpdmaConfigPayloadType clss,
                                  VpsHal_VpdmaConfigCmdType direct,
                                  UInt32 payloadSize,
                                  Ptr payloadAddr,
                                  Ptr destAddr,
                                  UInt16 dataSize);

/**
 *  VpsHal_vpdmaCreateSOLCtrlDesc
 *  \brief Function to create Sync on List Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to memory for control descriptor
 *  \param lists            Array of list numbers
 *  \param numList          Number of lists in second argument
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOLCtrlDesc(Ptr memPtr, UInt8 *lists, UInt8 numList);

/**
 *  VpsHal_vpdmaCreateSICtrlDesc
 *  \brief Function to create a Send Interrupt Control Descriptor
 *  in the given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param source           Send Interrupt source possible values are 0-15
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSICtrlDesc(Ptr memPtr, UInt16 source);

/**
 *  VpsHal_vpdmaCreateChangeClientIntrCtrlDesc
 *  \brief Function to create a control descriptor to change
 *  client interrupt
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose interrupt is to be changed
 *  \param event            event to which interrupt is to be changed
 *  \param lineCount        Line Count
 *  \param pixCount         Pixel Count
 *  \return                 None
 */
Void VpsHal_vpdmaCreateChangeClientIntrCtrlDesc(Ptr memPtr,
                                                VpsHal_VpdmaChannel channelNum,
                                                VpsHal_VpdmaSocEvent event,
                                                UInt16 lineCount,
                                                UInt16 pixCount);

/**
 *  VpsHal_vpdmaCreateSOCCtrlDesc
 *  Function to create a Sync on Client control Descriptor
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose interrupt is to be changed
 *  \param event            event to which interrupt is to be changed
 *  \param lineCount        Line Count
 *  \param pixCount         Pixel Count
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOCCtrlDesc(Ptr memPtr,
                                   VpsHal_VpdmaChannel channelNum,
                                   VpsHal_VpdmaSocEvent event,
                                   UInt16 lineCount,
                                   UInt16 pixCount);

/**
 *  VpsHal_vpdmaCreateCreateLmFidCtrlDesc
 *  \brief Function to create a control descriptor to Invert List Manager
 *  FID
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param fidNum           Indicates which LM Fid to be changed
 *  \param lmFidCtrl        Indicates what is to be done with LM FID
 *  \returns                None
 */
Void VpsHal_vpdmaCreateLmFidCtrlDesc(Ptr memPtr,
                                     UInt32 fidNum,
                                     VpsHal_VpdmaLmFidCtrl lmFidCtrl);

/**
 *  VpsHal_vpdmaCreateAbortCtrlDesc
 *  \brief Function to create a control descriptor to Invert List Manager
 *  FID
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose data transfer is to be aborted
 *  \return                 None
 */
Void VpsHal_vpdmaCreateAbortCtrlDesc(Ptr memPtr,
                                     VpsHal_VpdmaChannel channelNum);

/**
 *  VpsHal_vpdmaCreateRLCtrlDesc
 *  \brief Function to create Reload Control Descriptor in the given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param rlListAddr       Address of the memory from where list is to be
 *                          reloaded
 *  \param size             Size of the reloaded list in bytes
 *  \return                 None
 */
Void VpsHal_vpdmaCreateRLCtrlDesc(Ptr memPtr,
                                  Ptr rlListAddr,
                                  UInt32 size);

/**
 *  VpsHal_vpdmaCreateSORCtrlDesc
 *  \brief Function to create Sync On Register Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param listNum          List Number for which SOR is to be created
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSORCtrlDesc(Ptr memPtr, UInt8 listNum);

/**
 *  VpsHal_vpdmaCreateSOTCtrlDesc
 *  \brief Function to create Sync On Timer Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param numCycles        Number of cycles to wait for.
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOTCtrlDesc(Ptr memPtr, UInt16 numCycles);

/**
 *  VpsHal_vpdmaCreateSOCHCtrlDesc
 *  \brief Function to create Sync On Channel Control Descriptor in the
 *  given memory
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel Number to sync on
 *  \return                 None
 */
Void VpsHal_vpdmaCreateSOCHCtrlDesc(Ptr memPtr,
                                    VpsHal_VpdmaChannel channelNum);


/**
 *  VpsHal_vpdmaCreateCCISCtrlDesc
 *  Function to create a Change client interrupt source control descriptor
 *
 *  \param memPtr           Pointer to the memory in which control
 *                          descriptor is to be created
 *  \param channelNum       Channel whose interrupt is to be changed
 *  \param event            event to which interrupt is to be changed
 *  \param lineCount        Line Count
 *  \param pixCount         Pixel Count
 *  \return                 None
 */
Void VpsHal_vpdmaCreateCCISCtrlDesc(Ptr memPtr,
                                    VpsHal_VpdmaChannel channelNum,
                                    VpsHal_VpdmaSocEvent event,
                                    UInt16 lineCount,
                                    UInt16 pixCount);
/**
 *  VpsHal_vpdmaCreateDummyDesc
 *  \brief Function to create Dummy descriptor of 4 words in the given memory
 *
 *  \param memPtr           Pointer to the memory in which dummy
 *                          descriptor is to be created
 *  \return                 None
 */
Void VpsHal_vpdmaCreateDummyDesc(Ptr memPtr);

/**
 *  VpsHal_vpdmaIsValidSize
 *  \brief Function to check whether the size is valid or not for
 *         the given input path and for the given format
 *
 *  \param vpdmaPath        VPDMA Path
 *  \param dataFormat       VPDMA Path
 *                          descriptor is to be created
 *  \return                 None
 */
UInt32 VpsHal_vpdmaIsValidSize(VpsHal_VpdmaPath vpdmaPath,
                               UInt32 dataFormat,
                               VpsHal_VpdmaMemoryType memType,
                               UInt32 width,
                               UInt32 height);

/**
 *  VpsHal_vpdmaSetAddress
 *  \brief Function to set the buffer address and fid in the data descriptor.
 *  This function can be called from the ISR to update the buffer
 *  address and fid
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param fid              Value of FID. It must be either 0 or 1.
 *  \param bufferAddr       Buffer Address to be set in the data desc.
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetAddress(Ptr memPtr, UInt8 fid, Ptr bufferAddr)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (0u == (((UInt32)bufferAddr &
            (VPSHAL_VPDMA_BUF_ADDR_ALIGN - 1u)))));
    bufferAddr = (Ptr)VpsHal_vpdmaVirtToPhy(bufferAddr);
    ((volatile VpsHal_VpdmaInDataDesc *)memPtr)->address =
                    (UInt32)bufferAddr & VPSHAL_VPDMA_DATADESC_INBUFFERMASK;
    ((volatile VpsHal_VpdmaInDataDesc *)memPtr)->fieldId = fid;
}

/**
 *  VpsHal_vpdmaGetComprTransSize
 *  \brief Function to get the compressed transfer size. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *
 *  \return                 Transfer Size
 */
static inline UInt32 VpsHal_vpdmaGetComprTransSize(Ptr memPtr)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    /* Return transfer size from the word-1 of the descriptor */
    return (((volatile UInt32 *) memPtr)[1u]);
}

/**
 *  VpsHal_vpdmaGetComprFrameSize
 *  \brief Function to get the compressed frame size. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *
 *  \return                 Frame Size
 */
static inline Int32 VpsHal_vpdmaGetComprFrameSize(Ptr memPtr)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    /* Return frame size from the word-4 of the descriptor */
    return (((volatile UInt32 *) memPtr)[4u]);
}

/**
 *  VpsHal_vpdmaSetDecomprFrameSize
 *  \brief Function to set the frame size and transfer size for the
 *  decompressor descriptor when decompression is enabled. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param frameSize        Frame size - word 4 of descriptor
 *  \param transferSize     Frame size - word 1 of descriptor
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetDecomprFrameSize(Ptr memPtr,
                                                   UInt32 frameSize,
                                                   UInt32 transferSize)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    /* Set the transfer size in word-1 and frame size in word-4 of
       the descriptor */
    ((volatile UInt32 *) memPtr)[1u] = transferSize;
    ((volatile UInt32 *) memPtr)[4u] = frameSize;
}

/**
 *  VpsHal_vpdmaClearDataDesc
 *  \brief Function to clear complete data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \return                 None
 */
static inline Void VpsHal_vpdmaClearDataDesc(Ptr memPtr, UInt8 ch)
{
    UInt32 val;

    GT_assert(VpsHalTrace, (NULL != memPtr));

    val = (ch)|(ch<<8u)|(ch<<16u)|(ch<<24u);

    ((volatile UInt32 *) memPtr)[0u] = val;
    ((volatile UInt32 *) memPtr)[1u] = val;
    ((volatile UInt32 *) memPtr)[2u] = val;
    ((volatile UInt32 *) memPtr)[3u] = val;
    ((volatile UInt32 *) memPtr)[4u] = val;
    ((volatile UInt32 *) memPtr)[5u] = val;
    ((volatile UInt32 *) memPtr)[6u] = val;
    ((volatile UInt32 *) memPtr)[7u] = val;
}

/**
 *  VpsHal_vpdmaClearCfgCtrlDesc
 *  \brief Function to clear complete Configuration or control descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \return                 None
 */
static inline Void VpsHal_vpdmaClearCfgCtrlDesc(Ptr memPtr, UInt8 ch)
{
    UInt32 val;

    GT_assert(VpsHalTrace, (NULL != memPtr));

    val = (ch)|(ch<<8u)|(ch<<16u)|(ch<<24u);

    ((volatile UInt32 *) memPtr)[0u] = val;
    ((volatile UInt32 *) memPtr)[1u] = val;
    ((volatile UInt32 *) memPtr)[2u] = val;
    ((volatile UInt32 *) memPtr)[3u] = val;
}

/**
 *  VpsHal_vpdmaSetDescWriteAddr
 *  \brief Function to set the descriptor write address into
 *  data descriptor. This is the address where VPDMA prepares the
 *  data descriptor. This function is used only for the outbound
 *  data descriptor.
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param descAddr         Address of the memory where descriptors will
 *                          be written by VPDMA
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetDescWriteAddr(Ptr memPtr, Ptr descAddr)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (0u == (((UInt32)descAddr &
            (VPSHAL_VPDMA_WR_DESC_BUF_ADDR_ALIGN - 1u)))));
    GT_assert(VpsHalTrace, (0u == ((UInt32)descAddr &
            (~VPSHAL_VPDMA_DATADESC_OUTBUFFERMASK))));

    descAddr = (Ptr)VpsHal_vpdmaVirtToPhy(descAddr);
    ((volatile VpsHal_VpdmaOutDataDesc *)memPtr)->outDescAddress =
        (UInt32)descAddr >> 5u;
}

/**
 *  VpsHal_vpdmaSetLineStride
 *  Function to set the line stride for both in/out bound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param lineStride       Line stride in bytes between two lines
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetLineStride(Ptr memPtr,
                                             UInt16 lineStride)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    /* Caution: Assuming memType bit position is same for both in and out
     * bound descriptors */
    ((volatile VpsHal_VpdmaInDataDesc *)memPtr)->lineStride = lineStride;
}

/**
 *  VpsHal_vpdmaSetMemType
 *  Function to set the memory type for both in/out bound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param memType          Type of memory i.e. Tiled or Non-Tiled
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetMemType(Ptr memPtr,
                                          VpsHal_VpdmaMemoryType memType)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    /* Caution: Assuming memType bit position is same for both in and out
     * bound descriptors */
    ((volatile VpsHal_VpdmaInDataDesc *)memPtr)->memType = memType;
}

/**
 *  VpsHal_vpdmaSetDropData
 *  Function to set the drop data flag in out bound data descriptor.
 *  When this flag is set, VPDMA will drop the captured data without
 *  storing it the memory.
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param enable           Flag to indicate whether to enable/disable
 *                          drop data be written by VPDMA
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetDropData(Ptr memPtr, UInt8 enable)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    ((volatile VpsHal_VpdmaOutDataDesc *)memPtr)->dropData = enable;
}

/**
 *  VpsHal_vpdmaSetWriteDesc
 *  Function to set the write descriptor flag in out bound data
 *  descriptor. Setting this flag tells VPDMA to write data descriptor
 *  in the memory
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param enable           Flag to indicate whether to enable/disable
 *                          Write Descriptor
 *                          be written by VPDMA
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetWriteDesc(Ptr memPtr, UInt8 enable)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    ((volatile VpsHal_VpdmaOutDataDesc *)memPtr)->writeDesc = enable;
}


/**
 *  VpsHal_vpdmaSetDescHeight
 *  Function to set the height in out bound data
 *  descriptor. This API is used to set the height in
 *  case of slice based operations.
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param enable
 *                          Pointer to Data descriptor whose height
 *                          field needs to be modified
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetDescHeight(Ptr memPtr,
                                             VpsHal_VpdmaInDescParams *descInfo)
{

    volatile VpsHal_VpdmaInDataDesc *dataDesc = NULL;
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != descInfo));


    dataDesc = (volatile VpsHal_VpdmaInDataDesc *)memPtr;

    dataDesc->frameHeight = descInfo->frameHeight;
    dataDesc->transferHeight = descInfo->transHeight;

}
/**
 *  VpsHal_vpdmaSetUseDescReg
 *  \brief Function to inform VPDMA to use register to get the descriptor
 *  write address. This bit is available in outbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing out bound
 *                          descriptor
 *  \param enable           Flag to indicate whether to enable/disable
 *                          Use register for descriptor write address
 *                          be written by VPDMA
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetUseDescReg(Ptr memPtr, UInt8 enable)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    ((volatile VpsHal_VpdmaOutDataDesc *)memPtr)->useDescReg = enable;
}

/**
 *  VpsHal_vpdmaSet1DMem
 *  \brief Function to set the 1D mem bit. This bit is available in
 *  inbound as well as in outbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in/out bound
 *                          descriptor
 *  \param is1DMem          Flag to indicate whether to enable/disable
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSet1DMem(Ptr memPtr, UInt32 is1DMem)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    /* Note: This bit is in the same location in both in/out descriptors. So
     * using inbound structure for both */
    ((volatile VpsHal_VpdmaInDataDesc*) memPtr)->oneD = is1DMem;
}

/**
 *  VpsHal_vpdmaSetFrameDim
 *  \brief Function to set the frame width and height. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param frameWidth       Width of frame
 *  \param frameHeight      Height of frame
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetFrameDim(Ptr memPtr,
                                           UInt16 frameWidth,
                                           UInt16 frameHeight)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->frameWidth = frameWidth;
    ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->frameHeight = frameHeight;
}

/**
 *  VpsHal_vpdmaGetFrameDim
 *  \brief Function to get the frame width and height. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param frameWidth       Width of frame
 *  \param frameHeight      Height of frame
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaGetFrameDim(Ptr memPtr,
                                           UInt16 *frameWidth,
                                           UInt16 *frameHeight)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != frameWidth));
    GT_assert(VpsHalTrace, (NULL != frameHeight));

    *frameWidth = ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->frameWidth;
    *frameHeight = ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->frameHeight;
}

/**
 *  VpsHal_vpdmaGetFid
 *  \brief Function to get the fid. This bit is
 *  available in inbound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param fid              FID of the field
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaGetFid(Ptr memPtr,
                                      UInt16 *fid)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != fid));

    *fid = ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->fieldId;
}

/**
 *  VpsHal_vpdmaGetChannel
 *  \brief Function to get the VPDMA Channel.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param fid              VPDMA Channel
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaGetChannel(Ptr memPtr,
                                          VpsHal_VpdmaChannel *channel)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    GT_assert(VpsHalTrace, (NULL != channel));

    *channel = (VpsHal_VpdmaChannel)
                    ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->channel;
}

/**
 *  VpsHal_vpdmaSetMosaicMode
 *  \brief Function to set the mosaic mode in the in bound data descriptor.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param isMosaicMode     Flag to indicate whether to enable/disable
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaSetMosaicMode(Ptr memPtr,
                                             UInt32 isMosaicMode)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));
    ((volatile VpsHal_VpdmaInDataDesc *) memPtr)->mosaicMode = isMosaicMode;
}

/**
 *  VpsHal_vpdmaEnableDescSkip
 *  \brief Function to enable/disable skip desc bit.
 *
 *  \param memPtr           Pointer to memory containing in bound
 *                          descriptor
 *  \param enableSkip       Flag to indicate whether to enable/disable
 *
 *  \return                 None
 */
static inline Void VpsHal_vpdmaEnableDescSkip(Ptr memPtr,
                                              UInt32 enableSkip)
{
    GT_assert(VpsHalTrace, (NULL != memPtr));

    ((volatile VpsHal_VpdmaOutDataDesc *) memPtr)->descSkip = enableSkip;
}

/**
 *  VpsHal_vpdmaPrintDesc
 *  Prints the VPDMA descriptor on console.
 */
static inline void VpsHal_vpdmaPrintDesc(const Void *descPtr, UInt32 traceMask)
{
    UInt32          wordCnt;
    const UInt32   *tempPtr;

    tempPtr = (UInt32 *) descPtr;
    for (wordCnt = 0u;
         wordCnt < (VPSHAL_VPDMA_DATA_DESC_SIZE / sizeof (UInt32));
         wordCnt++)
    {
        GT_2trace(traceMask, GT_DEBUG,
            "Word %d: 0x%0.8x", wordCnt, *tempPtr++);
    }

    return;
}

Int32 VpsHal_vpdmaSetPerfMonRegs(void);
Int32 VpsHal_vpdmaPrintPerfMonRegs(void);
Int32 VpsHal_vpdmaListWaitComplete(UInt8 listNum, UInt32 timeout);

/**
 *  VpsHal_vpdmaInitDebug
 *  \brief VPDMA Debug function initialization. This function should be called
 *  Before calling any debug functions.
 *
 *  \param baseAddr         VPDMA base address.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
UInt32 VpsHal_vpdmaInitDebug(UInt32 baseAddr);

/**
 *  VpsHal_vpdmaGetListStatus
 *  \brief VPDMA Debug function - Reads and returns the current list status.
 *
 *  VpsHal_vpdmaInitDebug should be called before calling this function.
 *
 *  \param listNum          List number for which status to be returned.
 *
 *  \return                 List status.
 */
UInt32 VpsHal_vpdmaGetListStatus(UInt32 listNum);

/**
 *  VpsHal_vpdmaReadLmReg
 *  \brief VPDMA Debug function - Reads and returns the list manager
 *  internal register.
 *
 *  VpsHal_vpdmaInitDebug should be called before calling this function.
 *
 *  \param listNum          List number to be used to perform the read.
 *  \param regOffset        Register offset to read.
 *
 *  \return                 List manager register value.
 */
UInt32 VpsHal_vpdmaReadLmReg(UInt32 listNum, UInt32 regOffset);

Int32 VpsHal_vpdmaReadChMemory(UInt32 listNum, UInt32 chNum, UInt32 *chMemory);

/**
 *  VpsHal_vpdmaPrintList
 *  \brief VPDMA Debug function - Parses and prints the VPDMA descriptors
 *  till a reload is encountered or till the list size.
 *
 *  \param listAddr   [IN]  List address to start printing from.
 *  \param listSize   [IN]  List size in VPDMA words.
 *  \param rlListAddr [OUT] Reload list address for next parsing. A value of 0
 *                          means the list ends.
 *  \param rlListSize [OUT] Reload list size in VPDMA words.
 *  \param printLevel [IN]  0 - Prints only Errors and Warnings.
 *                          1 - Prints descriptor information in addition
 *                              to level 0.
 *                          2 - Prints register overlay in addition to level 1.
 */
Void VpsHal_vpdmaPrintList(UInt32 listAddr,
                           UInt32 listSize,
                           UInt32 *rlListAddr,
                           UInt32 *rlListSize,
                           UInt32 printLevel);

/**
 *  VpsHal_vpdmaParseRegOverlay
 *  \brief VPDMA Debug function - Parses the MMR config overlay memory
 *  and extract the address-data value.
 *
 *  \param memPtr        [IN]  Overlay memory pointer.
 *  \param payloadLength [IN]  Overlay memory size in VPDMA words.
 *  \param maxRegToParse [IN]  Represents the array size of below pointers
 *                             in words.
 *  \param numRegParsed  [OUT] Number of registers parsed. This paramter could
 *                             be NULL if no return is needed.
 *  \param regAddr       [OUT] Pointer to register address array where the
 *                             parsed register address is populated. This
 *                             paramter could be NULL if no return is needed.
 *  \param regVal        [OUT] Pointer to register value array where the
 *                             parsed register value is populated. This
 *                             paramter could be NULL if no return is needed.
 *  \param traceMask     [IN]  Trace mask for printing different level of debug
 *                             messages.
 *
 *  \return     Returns VPS_SOK if parsing is successful else returns error.
 */
UInt32 VpsHal_vpdmaParseRegOverlay(const Void *memPtr,
                                   UInt32 payloadLength,
                                   UInt32 maxRegToParse,
                                   UInt32 *numRegParsed,
                                   UInt32 *regAddr,
                                   UInt32 *regVal,
                                   UInt32 traceMask);

/**
 *  VpsHal_vpdmaGetClientNo
 *  \brief Function that would return the client that a given channel belongs
 *          to. Currently, valid only for VIP channels.
 *
 *  \param chan [IN]  Valid Channel number, should one of value in the
 *                    enumeration VpsHal_VpdmaChannel.
 *
 *  \return     Returns VPSHAL_VPDMA_CLIENT_MAX in cases of error, one of values
 *              in enumeration VpsHal_VpdmaClient otherwise.
 */
VpsHal_VpdmaClient VpsHal_vpdmaGetClientNo(VpsHal_VpdmaChannel chan);

Void VpsHal_vpdmaPrintCStat(void);

/**
 *  \brief Return current descriptor write address
 *
 *  \return current descriptor write address
 */
UInt8 *VpsHal_vpdmaGetCurrentDescWriteAddr(void);


UInt32 VpsHal_vpdmaGetDeadListStatus(UInt32 listNum, UInt32 deadValue);

UInt32 VpsHal_vpdmaGetListCompleteStatus(UInt32 listNum);

Void VpsHal_vpdmaClearListCompleteStatus(UInt32 listNum);

Void VpsHal_vpdmaPrintInBoundDesc(const Void *memPtr, UInt32 traceMask);

#endif /* End of #ifndef _VPSHAL_VPDMA_H */
