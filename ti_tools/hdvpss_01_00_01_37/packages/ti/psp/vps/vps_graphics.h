/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \defgroup VPS_DRV_FVID2_GRAPHICS Graphics API
 *
 * @{
*/

/**
 *  \file vps_graphics.h
 *
 *  \brief Graphics API
 *   This modules define API to display or store graphics data via graphics ports
 *   in HD-VPSS.
 */

#ifndef _VPS_GRAPHICS_H
#define _VPS_GRAPHICS_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of GRPX driver instance.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_DISP_GRPX_MAX_INST     (3u)

/*
 *  Macros for different driver instance numbers to be passed as instance ID
 *  at the time of driver create.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
/** \brief Grpx0  driver instance number. */
#define VPS_DISP_INST_GRPX0        (0u)
/** \brief Grpx1  driver instance number. */
#define VPS_DISP_INST_GRPX1        (1u)
/** \brief Grpx2  driver instance number. */
#define VPS_DISP_INST_GRPX2        (2u)

/** \brief number of GRPX scaling coefficient phases*/
#define VPS_GRPX_COEFF_PHASE       (0x8u)
/** \brief number of GRPX horizontal scaling coefficient taps */
#define VPS_GRPX_HOR_COEFF_TAP     (0x5u)
/** \brief number of GRPX vertical scaling coefficient tap number*/
#define VPS_GRPX_VER_COEFF_TAP     (0x4u)
/** \brief number of GRPX horizontal scaling 16bit coefficients */
#define VPS_GRPX_HOR_COEFF_SIZE    (VPS_GRPX_COEFF_PHASE \
                                         * VPS_GRPX_HOR_COEFF_TAP)
/** \brief nubmer of GRPX vertical scaling 16bit coefficients*/
#define VPS_GRPX_VER_COEFF_SIZE    (VPS_GRPX_COEFF_PHASE \
                                         * VPS_GRPX_VER_COEFF_TAP)

/**\brief maximum number of regions supported in one frame  */
#define VPS_GRPX_MAX_REGIONS_PER_FRAME       (12u)

/**
 *  enum Vps_GprxDriverMode
 *  \brief This enum is used to define the operation modes, which graphics
 *   display driver supports.
 *  frame buffer mode: When operating under this mode, it does not support
 *  queuing mechansim. Although app can still call queue function to queue
 *  buffer, but driver only maintain the buffers queued by the last queue
 *  function. Dequeue function is not valid under this mode. It is similar to
 *  the Linux Fbdev Frame buffer mode.
 *
 *  streaming mode: When operating under this mode, it supports
 *  queuing mechansim, app is able to queue/dequeue multiple buffers
 *  by calling queue/dequeu function multiple times. It is similiar to Linux
 *  V4L2 streaming mode.
 */
typedef enum
{
    VPS_GRPX_STREAMING_MODE = 0,
    /**< Driver operates as streaming mode (streaming V4L2 mode).*/
    VPS_GRPX_FRAME_BUFFER_MODE,
    /**< Driver operates as frame buffer mode (non-streaming FBDEV mode). */
    VPS_GRPX_MODE_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */
}Vps_GrpxDriverMode;

/**
 *  enum Vps_GrpxBlend
 *  \brief This enum is used to define various GRPX display blending type for
 *  regions. GRPX support 3 blending types: Global blending, palette blending
 *  and pixel blending.
 */
typedef enum
{
    VPS_GRPX_BLEND_NO = 0,
    /**< no blending, region ALPHA 0xFF was applied to make region opaque*/
    VPS_GRPX_BLEND_REGION_GLOBAL,
    /**< global region blending, user_defined alpha value will be applied to the
         complete region */
    VPS_GRPX_BLEND_COLOR,
    /**< color (palette) blending, alpha value in the CLUT is applied
         Input should BIT MAP
         CLUT LSB should be RGBA format */
    VPS_GRPX_BLEND_PIXEL,
    /**< pixel(embedding alpha)blending, embedded alpha value is applied */
    VPS_GRPX_BLEND_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */

} Vps_GrpxBlendType;

/**
 *  enum Vps_GrpxTrans
 *  \brief This enum is used to define the GRPX transparency masking type for
 *  regions. number of bits will be masked out from user_defined color key
 *  comparison.
 *  Number bits of RGB component data is compared to the transparency data
 *  which is decided by the configuration data.  For example, if 3BIT_MASK is
 *  select, then MSB 5 bits of RGB data is compared
 *  to the transparency data.
 */
typedef enum
{
    VPS_GRPX_TRANS_NO_MASK = 0,
    /**< No mask, 8 bits compared */
    VPS_GRPX_TRANS_1BIT_MASK,
    /**< 1bit mask, MSB 7Bits compared */
    VPS_GRPX_TRANS_2BIT_MASK,
    /**< 2bit mask, MSB 6Bits compared */
    VPS_GRPX_TRANS_3BIT_MASK,
    /**< 3bit maks, MSB 5bits compared */
    VPS_GRPX_TRANS_MASK_MAX
    /**< Should be the last value of this enumeration.
         Will be used by driver for validating the input parameters. */

}Vps_GrpxTransMaskType;

/* ========================================================================= */
/*                         Structure Declarations                             */
/* ========================================================================= */


/**
 *  struct Vps_GrpxScParams
 *  \brief This structure is used to define the configuration for graphic
 *  pholyphase scaler by the application. Those configuration include the
 *  scaler factor(0.25x-4x), phase(0-7) and fineoffset of the scaler
 */
typedef struct
{
    UInt32                   inWidth;
    /**< input region width */
    UInt32                   inHeight;
    /**< input region height */
    UInt32                   outWidth;
    /**< output region width */
    UInt32                   outHeight;
    /**< output region height */
    UInt32                   horFineOffset;
    /**< horizontal fine offset , please set to zero*/
    UInt32                   verFineOffset;
    /**< vertical fine offset, please set to zero */
    Ptr                      scCoeff;
    /**< coefficient memory location(write only). If scCoeff is
         set to NULL, GRPX driver load the preloaded coefficients
         based on the input/output ratio, if scScoeff is not NULL,
         it must be defined by Vps_GrpxScCoeff to load app defined
         coefficients. When calling IOCTL IOCTL_VPS_GET_GRPX_PARAMS,
         scCoeff is not been used by the driver*/
}Vps_GrpxScParams;

/**
 *  struct Vps_GrpxScCoeff
 *  \brief This structure is used to define the coefficient for graphics
 *  pholyphase scaler by the application. The coefficient is in the order
 *  of tap0(phase 0-7), ... tapN(phase 0 - 7). The valid values for coefficient
    are [-256, 255], since it is 10bits register, bit9 should be set to 1 if
    the coefficient is negative value.
    the MSB[15:10] will be discarded by the driver.
 */
typedef struct
{
    UInt16                   hsCoeff[VPS_GRPX_HOR_COEFF_SIZE];
    /**< horizontal coefficient */
    UInt16                   vsCoeff[VPS_GRPX_VER_COEFF_SIZE];
    /**< vertical coefficients */
}Vps_GrpxScCoeff;

/**
 *  struct Vps_GrpxRegionParams
 * \brief This structure is used to define graphics region attributes.
 */
typedef struct
{
    UInt32                   regionWidth;
    /**< width of current region */
    UInt32                   regionHeight;
    /**<  height of current region  */
    UInt32                   regionPosX;
    /**< start x position of region in the current frame */
    UInt32                   regionPosY;
    /**< start y position of region in the current frame*/
    UInt32                   dispPriority;
    /**< display priority of region in the current frame, this priority will
       be used to switch order between two overlapping regions from
       two GRPX pipelines. */
    UInt32                   firstRegion;
    /**< first region or not, indicate this is the first region in the frame
       app must set this accordingly*/
    UInt32                   lastRegion;
    /**< last region or not, indicate this is the last region in the frame,
       app must set this accordingly.
       For single region per frame use case, app must set both firstRegion
       and lastRegion to TRUE. */
    UInt32                   scEnable;
    /**< Enables/Disables the scaling feature for graphics progressvie input
       data, scaling is invalid if the input graphics data is interlaced. When
       both this flag is set to TRUE and  input region size is equal to output
       region size, scalar performs anti-flicker filtering instead of scaling.
       */
    UInt32                   stencilingEnable;
    /**< Enables/Disables GRPX Stenciling feature for regions.
        When this feature is enabled, app need provide a region mask table,
        where each bit in the represent the one pixel in the region, and the
        GRPX forces the alpha value of the pixel to ZERO in order to
        "mask"-off the pixel if the bit associated with pixel is one.*/
    UInt32                   bbEnable;
    /**< Eanbles/Disables GRPX Boundbox feature for regions.
        When this feature is enabled, the GRPX supports overwriting alpha
        values of pixels that make up a 1-pixel wide boundary box of a
        region with a user defined alpha value so that the flickering around
        the edges can be minimized. */
    UInt32                   bbAlpha;
    /**< bound box alpha value  valid only if bbEnable is set to 1*/
    UInt32                   blendAlpha;
    /**< blending alpha value only valid if blendType is set to global region*/
    UInt32                   blendType
    /**< GRPX blending type for regions
         For valid values see #Vps_GrpxBlendType */;
    UInt32                   transEnable;
    /**< Enables/Disables GRPX transparency mask feature for regions*/
    UInt32                   transType;
    /**< transparency type. For valid values see #Vps_GrpxTransMaskType*/
    UInt32                   transColorRgb24;
    /**< user defined RGB color data to perform transparency masking,
      it need be in RGB888 format */
}Vps_GrpxRegionParams;


/**
 *  struct Vps_GrpxRtParams
 *  \brief This structure is used to define graphics configuration of each
 *  region. This structure is also used to store the runtime region
 *  paramters. App should pass structure pointer with perFrameCfg as
 *  RunTime GRPX parameters in the  FVID2_frame structure.
 */
typedef struct
{
    UInt32                  regId;
    /**< The ID of region in the current frame, this ID must be start from 0.
     For single region use case, it should be always 0
     For the multiple-region use case, it should in the range of 0
     to (numRegions -1)*/
    UInt32                  format;
    /**< Data format. For valid values see #FVID2_DataFormat. */
    UInt32                  pitch[FVID2_MAX_PLANES];
    /**< memory pitch*/
    Vps_GrpxRegionParams    regParams;
    /**< GRPX region attributes*/
    UInt32                  rotation;
    /**< rotation type, only valid if GRPX data is stored in the tiler memory.
         For valid values see #Vps_MemRotationType. */
    Vps_GrpxScParams       *scParams;
    /**< Parameters for the GRPX Scalar, this field is reserved for future usage,
        please set it to NULL*/
    Ptr                     stenPtr;
    /**< stenciling data Ptr */
    UInt32                  stenPitch;
    /**<stenciling stride(pitch of the stenciling data buffer) */
}Vps_GrpxRtParams;

/**
 *  struct Vps_GrpxParamsList
 *  \brief This structure is used to define the parameters for all regions,
 *  which will be passed when application call IOCTL_VPS_SET_GRPX_PARAMS
 *  /IOCTL_VPS_GET_GRPX_PARAMS IOCTL to set/get the parameter
 *  for the graphcis driver...
 */
typedef struct
{
    UInt32                   numRegions;
    /**< number of regions to display per frame*/
    Vps_GrpxRtParams         *gParams;
    /**< array of the region configurations, size of the array is
       determined by the numRegions, this can not be set to NULL */
    Vps_GrpxScParams         *scParams;
    /**< scaling parameters of the GRPX, this can be NULL only if
       no region in the frame has scaling feature enable*/
    Ptr                      clutPtr;
    /**< pointer to memory containing CLUT data, this is not array,
        all regions will share the same CLUT.*/
}Vps_GrpxParamsList;

/**
 *  struct Vps_GrpxRtList
 *  \brief This structure is used to define the runtime parameter
 *  shared by all regions in the given frame. This struture should
 *  be passed throught perListCfg in the FVID2_frameList or
 *  FVID2_processList
 */
 typedef struct
{
    Ptr                      clutPtr;
    /**< pointer to memory containing CLUT data, this is not array,
        all regions share the same CLUT.*/
    Vps_GrpxScParams         *scParams;
    /**< scalling parameters used by the GRPX, this is not array
        pointer, this can be NULL if no change on scaling parameters */
} Vps_GrpxRtList;


/**
 *  struct Vps_GrpxCreateParams
 *  \brief Graphics driver create parameter structure to be passed to the
 *  driver at the time of graphics driver create call.
 */
typedef struct
{
    UInt32                    memType;
    /**< VPDMA Memory type. For valid values see #Vps_VpdmaMemoryType. */
    UInt32                    drvMode;
    /**< graphics driver operation mode.
        For valid values see #Vps_GrpxDriverMode */
    UInt32                  periodicCallbackEnable;
    /**< TRUE: User callback passed during FVID2 create is called periodically.
         For progressive display, this interval is equal to VSYNC interval.
         For interlaced display, this interval is equal to twice the VSYNC
         interval as frames (two fields) are queued to the driver.
         FALSE: User callback passed during FVID2 create is called only
         if one or more frames (requests) are available in the driver output
         queue for the application to dequeue. */

} Vps_GrpxCreateParams;

/**
 *  struct Vps_GrpxCreateStatus
 *  \brief Grpx Driver Status for the display Grpx open call. This status
 *   returns the GRPX driver information back to caller.
 */
typedef struct
{
    Int32                     retVal;
    /**< Reture value of the create call.*/
    UInt32                    dispWidth;
    /**< Width of the display at the VENC in pixels to which the grpx driver
         path is connected. */
    UInt32                    dispHeight;
    /**< Height of the display at the VENC in linesto which the grpx driver
         path is connected. */
    UInt32                    minBufNum;
    /**< Minimum number of buffers to start GRPX display */
    UInt32                    maxReq;
    /**< Maximum number of request per instance that caller can sumbit
         without calling dequeue*/
    UInt32                    maxRegions;
    /**< Maximum region GRPX driver support for the display */
} Vps_GrpxCreateStatus;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* End of  #ifndef _VPS_GRAPHICS_H */

/* @} */

