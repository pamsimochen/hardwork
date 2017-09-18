/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_VPS_COMMON
 * \addtogroup VPS_DRV_FVID2_VPS_COMMON_VIP_PARSER HD-VPSS - VIP Parser Config API
 *
 * @{
 */

/**
 *  \file vps_cfgVipParser.h
 *
 *  \brief HD-VPSS - VIP Parser Config API
 */

#ifndef _VPS_CFG_VIP_PARSER_H
#define _VPS_CFG_VIP_PARSER_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */


/** \brief Use this when a particular value for VIP parser config is dont care */
#define VPS_VIP_VALUE_DONT_CARE     (-1)

/**
 *  \brief Enum for Control Channel Selection
 *  It describes channels numbers from extract control code and Vertical
 *  Ancillary Data
 */
typedef enum
{
     VPS_VIP_CTRL_CHAN_SEL_7_0 = 0,
     /**< Use data[7:0] to extract control codes and Vertical Ancillary Data */

     VPS_VIP_CTRL_CHAN_SEL_15_8,
     /**< Use data[15:8] to extract control codes and Vertical Ancillary Data */

     VPS_VIP_CTRL_CHAN_SEL_23_16,
     /**< Use data[23:16] to extract control codes and Vertical Ancillary Data
      */

     VPS_VIP_CTRL_CHAN_DONT_CARE = -1
     /**< Value is dont care */

} Vps_VipCtrlChanSel;

/**
 *  \brief It is used only for Discrete Sync
 */
typedef enum
{
    VPS_VIP_LINE_CAPTURE_STYLE_HSYNC = 0,
    /**< Use HSYNC style linecapture */

    VPS_VIP_LINE_CAPTURE_STYLE_ACTVID,
    /**< Use ACTVID style line capture */

    VPS_VIP_LINE_CAPTURE_STYLE_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipLineCaptureStyle;

/**
 *  \brief It is only used for Discrete Sync
 */
typedef enum
{
    VPS_VIP_FID_DETECT_MODE_PIN = 0,
    /**< Take FID from pin */

    VPS_VIP_FID_DETECT_MODE_VSYNC,
    /**< FID is determined by VSYNC skew */

    VPS_VIP_FID_DETECT_MODE_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipFidDetectMode;

/**
 *  \brief VIP Polarity
 */
typedef enum
{
    VPS_VIP_POLARITY_LOW = 0,
    /**< low Polarity */

    VPS_VIP_POLARITY_HIGH,
    /**< high Polarity */

    VPS_VIP_POLARITY_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipPolarity;

/**
 *  \brief Pixel clock edge polarity
 */
typedef enum
{
    VPS_VIP_PIX_CLK_EDGE_POL_RISING = 0,
    /**< Rising Edge is active PIXCLK edge */

    VPS_VIP_PIX_CLK_EDGE_POL_FALLING,
    /**< Falling Edge is active PIXCLK edge */

    VPS_VIP_PIX_CLK_EDGE_POL_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipPixClkEdgePol;

/**
 *  \brief VIP Field ID
 */
typedef enum
{
    VPS_VIP_FID_EVEN = 0,
    /**< Even field id */

    VPS_VIP_FID_ODD,
    /**< Odd field id */

    VPS_VIP_FID_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipFid;

/**
 *  \brief It gives the imformation in 8b interface mode from where to extract
 *  Vertical Ancillary data
 */
typedef enum
{
    VPS_VIP_ANC_CH_SEL_8B_LUMA_SIDE = 0,
    /**< Extract 8b Mode Vertical Ancillary Data from Luma Sites */

    VPS_VIP_ANC_CH_SEL_8B_CHROMA_SIDE,
    /**< Extract 8b Mode Vertical Ancillary Data from Chroma Sites */

    VPS_VIP_ANC_CH_SEL_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipAncChSel8b;

/**
 *  \brief In embedded sync for 2x/4x mux mode there are two way to extract
 *  soruce number, one is from least significant nibble of the XV/fvh codeword
 *  and other is least significant nibble of a horizontal blanking pixel value
 */
typedef enum
{
    VPS_VIP_SRC_NUM_POS_LS_NIBBLE_OF_CODEWORD = 0,
    /**< srcnum is in the least significant nibble of the XV/fvh codeword */

    VPS_VIP_SRC_NUM_POS_LS_NIBBLE_OF_HOR_BLNK_PIX,
    /**< srcnum is in the least significant nibble of a horizontal blanking
     * pixelvalue
     */

    VPS_VIP_SRC_NUM_POS_DONT_CARE = -1
    /**< Value is dont care */

} Vps_VipSrcNumPos;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Configuration parameter for VIP Instance
 */
typedef struct
{
     UInt32 clipActive;
     /**< FALSE: Do not clip active Data
      *   TRUE : Clip active Data
      */

     UInt32 clipBlank;
     /**< FALSE: Do not clip blanking Data
      *   TRUE : Clip blanking Data
      */

} Vps_VipConfig;

/**
 *  \brief Configuration parameter specific to Discrete Sync mode
 */
typedef struct
{
     UInt32 fidSkewPostCnt;
     /**< Post count value when using vsync skew in FID determination */

     UInt32 fidSkewPreCnt;
     /**< Pre count value when using vsync skew in FID determination */

     UInt32 lineCaptureStyle;
     /**< For valid values see #Vps_VipLineCaptureStyle */

     UInt32 fidDetectMode;
     /**< For valid values see #Vps_VipFidDetectMode */

     UInt32 actvidPol;
     /**< For valid values see #Vps_VipPolarity */

     UInt32 vsyncPol;
     /**< For valid values see #Vps_VipPolarity */

     UInt32 hsyncPol;
     /**< For valid values see #Vps_VipPolarity */

} Vps_VipDiscreteConfig;

/**
 *  \brief Configuration parameter specific to Embedded Sync only
 */
typedef struct
{
     UInt32 errCorrEnable;
     /**< TRUE: Error Correction enable, FALSE: disabled */

     UInt32 srcNumPos;
     /**< For valid values see #Vps_VipSrcNumPos */

     UInt32 isMaxChan3Bits;
     /**< FALSE: Use all 5 bits of the field for 32 total sources,
      *   TRUE : Use only bits 2:0 of this field to support
      *          the TVP5158's 8 sources, with the two upper
      *          most bits reserved
      */
} Vps_VipEmbConfig;

/**
 *  \brief Configuration parameter for a VIP Port
 */
typedef struct
{
     UInt32 ctrlChanSel;
     /**< For valid values see #Vps_VipCtrlChanSel */

     UInt32 ancChSel8b;
     /**< For valid values see #Vps_VipAncChSel8b */

     UInt32 pixClkEdgePol;
     /**< For valid values see #Vps_VipPixClkEdgePol */

     UInt32 invertFidPol;
     /**< FALSE: Keep FID as found, TRUE: Invert Value of FID */

     Vps_VipEmbConfig       embConfig;
     /**< Configuration parameter specific to Embedded Sync mode */

     Vps_VipDiscreteConfig  disConfig;
     /**< Configuration parameter specific to Discrete Sync mode */

} Vps_VipPortConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */



#endif

/* @} */

