/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshalGrpxDefaults.h
 *
 *  \brief VPS graphic scaler HAL coefficient File
 *   This file exposes the coefficients of the Graphic Scalar HAL to
 *   the upper layers.
 *
 */

#ifndef VPS_GRPX_SC_H
#define VPS_GRPX_SC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief  frame attribute overlay size(words)
 */
#define GRPX_VDPMA_OVLY_FRAME_ATTR_SIZE                            (0x4u)

/**
*  \brief whole GRPX overlay size(words)
*/
#define GRPX_VPDMA_OVLY_ALL_ATTRS_SIZE                             (0x30u)

/**
*  \brief GRPX scaler overlay size(words)
*/

#define GRPX_VPDMA_OVLY_ALL_COEFFS_CONFIGS_SIZE                    (0x2Cu)

/**
*  \brief  Horizontal coefficient size(words)
*/
#define GRPX_VPDMA_OVLY_HOR_COEFFS_SIZE                            (0x14u)

/**
*  \brief Vertical coefficient size(words)
*/
#define GRPX_VPDMA_OVLY_VER_COEFFS_SIZE                            (0x10u)

/**
*  \brief Scaler configuration size
*/
#define GRPX_VPDMA_OVLY_SC_CONFIG_SIZE                             (0x04u)

/**
*  \brief scaler base divider
*/
#define GRPX_SC_RESOLUTION                                         (2048)

/* The followings are used to set the destAddr filed in the VPDMA
 * configuration descriptor
 */

/**
*  \brief frame attribute offset in hardware
*/
#define GRPX_FRAME_DISPLAY_ATTR_ADDR_OFFSET                        (0x01u)

/**
*  \brief scaler attribute offset in hardware
*/

#define GRPX_REGION_SC_ATTR1_ADDR_OFFSET                           (0x02u)

/**
*  \brief frame attribute offset in overlay
*/
#define GRPX_REGION_FRAME_DISPLAY_ATTR_OFFSET                      (0x0u)

/**
*  \brief scaler attribute offset in overlay
*/
#define GRPX_REGION_SC_ATTR1_OFFSET                                (0x04u)

/**
*  \brief extra output lines from hardware for scaled region
*/
#define GRPX_SCALED_REGION_EXTRA_LINES                             (0x00u)

/**
*  \brief extra output pixels from hardware for scaled region
*/

#define GRPX_SCALED_REGION_EXTRA_PIXES                             (0x00u)

/**
*  \brief  gap required by hardware for up-scaled region
*/
#define GRPX_REGION_UP_SCALED_GAP                                  (0x01u)

/**
*  \brief gap required by hardware for down-scaled region
*/
#define GRPX_REGION_DOWN_SCALED_GAP                                (0x02u)

/** \brief GRPX Scalar coefficients for  Polyphase horizontal up scalar
 *  These coefficients are used for horizontal up scale only 5taps/8phases*/
#define GRPX_SC_HORIZONTAL_UP_SCALE_COEFF                      \
{                                                               \
    /*1.x - 8.x*/                                               \
    {0x03f6, 0x03fa, 0x03fd, 0x03ff, 0x0000, 0x0000, 0x0000, 0x0000},   \
    {0x0089, 0x005d, 0x0034, 0x0015, 0x0000, 0x03f5, 0x03f1, 0x03f2},   \
    {0x008b, 0x00b7, 0x00de, 0x00f7, 0x0100, 0x00f7, 0x00de, 0x00b7},   \
    {0x03f6, 0x03f2, 0x03f1, 0x03f5, 0x0000, 0x0015, 0x0034, 0x005d},   \
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x03ff, 0x03fd, 0x03fa}    \
}                                                               \

/** \brief GRPX Scalar coefficients for  Polyphase vertical up scalar
 *  These coefficients are used for vertical up scale only 4taps/8phases*/
#define GRPX_SC_VERTICAL_UP_SCALE_COEFF                        \
{                                                               \
    /* 1.x - 8.x*/                                              \
    {0x0000, 0x03fa, 0x03f8, 0x03fa, 0x03fd, 0x03ff, 0x0000, 0x0000},   \
    {0x0100, 0x00f7, 0x00dd, 0x00b4, 0x0083, 0x0054, 0x002c, 0x0010},   \
    {0x0000, 0x000f, 0x002b, 0x0053, 0x0083, 0x00b3, 0x00dc, 0x00f6},   \
    {0x0000, 0x0000, 0x0000, 0x03ff, 0x03fd, 0x03fa, 0x03f8, 0x03fa}    \
}                                                               \


/** \brief GRPX Scalar coefficients for polyphase horizontal  anti-flicker
* This is bypass filter
*/
#define GRPX_SC_HORIZONTAL_AF_COEFF                                       \
{                                                                     \
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, \
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, \
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, \
    {0x0100, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}, \
    {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000}  \
}                                                                     \


/** \brief GRPX Scalar coefficients for polyphase vertical  anti-flicker
* this is 0.5 downscaling coeff
*/
#define GRPX_SC_VERTICAL_AF_COEFF                   \
{                                                   \
    {0x0026, 0x0018, 0x000e, 0x0007, 0x0003, 0x0001, 0x0000, 0x0000},      \
    {0x00b4, 0x00b0, 0x00a5, 0x0094, 0x007f, 0x0065, 0x004e, 0x0039},      \
    {0x0026, 0x0038, 0x004d, 0x0064, 0x007b, 0x0093, 0x00a4, 0x00af},      \
    {0x0000, 0x0000, 0x0000, 0x0001, 0x0003, 0x0007, 0x000e, 0x0018}       \
}                                                   \

/** \brief GRPX Scalar coefficients for  Polyphase horizontal down scalar
 *  These coefficients are used for vertical up scale only */
#define GRPX_SC_HORIZONTAL_DOWN_SCALE_COEFF                         \
{                                                                   \
    {                                                               \
        /* horizontal decimation [ 0.25 - 0.375) use 0.25 coeff*/                  \
        {0x0014, 0x000e, 0x0009, 0x0005, 0x0003, 0x0001, 0x0000, 0x0000},   \
        {0x006c, 0x0060, 0x0054, 0x0047, 0x003b, 0x002f, 0x0025, 0x001c},   \
        {0x006c, 0x0076, 0x007e, 0x0084, 0x0084, 0x0084, 0x007e, 0x0076},   \
        {0x0014, 0x001c, 0x0025, 0x002f, 0x003b, 0x0047, 0x0054, 0x0060},   \
        {0x0000, 0x0000, 0x0000, 0x0001, 0x0003, 0x0005, 0x0009, 0x000e}    \
    },                                                              \
    {                                                               \
        /* horizontal decimation [ 0.375 - 0.5), use 0.4 coeff*/                   \
        {0x000e, 0x0009, 0x0005, 0x0002, 0x0001, 0x0000, 0x0000, 0x0000},   \
        {0x0072, 0x0063, 0x0054, 0x0045, 0x0036, 0x002a, 0x001e, 0x0015},   \
        {0x0072, 0x007f, 0x0089, 0x008f, 0x0092, 0x008f, 0x0089, 0x007f},   \
        {0x000e, 0x0015, 0x001e, 0x002a, 0x0036, 0x0045, 0x0054, 0x0063},   \
        {0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0002, 0x0005, 0x0009}    \
    },                                                              \
    {                                                               \
        /* horizontal decimation [ 0.5 - 0.75), use 0.6 coeff*/                    \
        {0x0003, 0x0000, 0x0000, 0x03ff, 0x03ff, 0x0000, 0x0000, 0x0000},   \
        {0x007e, 0x0067, 0x0051, 0x003d, 0x002b, 0x001c, 0x0011, 0x0009},   \
        {0x007c, 0x0090, 0x009e, 0x00a8, 0x00ac, 0x00a8, 0x009e, 0x0090},   \
        {0x0003, 0x0009, 0x0011, 0x001c, 0x002b, 0x003d, 0x0051, 0x0067},   \
        {0x0000, 0x0000, 0x0000, 0x0000, 0x03ff, 0x03ff, 0x0000, 0x0000}    \
    },                                                              \
    {                                                               \
        /* horizontal decimation [ 0.75 - 1.0), use 0.75 coeff*/                    \
        {0x03fc, 0x03fc, 0x03fc, 0x03fe, 0x03ff, 0x0000, 0x0000, 0x0000},   \
        {0x0084, 0x0067, 0x004b, 0x0032, 0x001e, 0x000e, 0x0004, 0x03ff},   \
        {0x0084, 0x009e, 0x00b5, 0x00c2, 0x00c6, 0x00c2, 0x00b5, 0x009e},   \
        {0x03fc, 0x03ff, 0x0004, 0x000e, 0x001e, 0x0032, 0x004b, 0x0067},   \
        {0x0000, 0x0000, 0x0000, 0x0000, 0x03ff, 0x03fe, 0x03fc, 0x03fc}    \
    }                                                               \
}                                                                   \

 /** \brief GRPX Scalar coefficients for  Polyphase vertical down scalar
 *  These coefficients are used for vertical up scale only */
#define GRPX_SC_VERTICAL_DOWN_SCALE_COEFF                           \
{                                                                   \
    {                                                               \
        /* vertical decimation [ 0.25 - 0.375) use 0.25 coeff*/                    \
        {0x0030, 0x0022, 0x0017, 0x000e, 0x0008, 0x0004, 0x0001, 0x0000},   \
        {0x00a0, 0x009e, 0x0096, 0x0089, 0x0079, 0x0066, 0x0053, 0x0041},   \
        {0x0030, 0x0040, 0x0052, 0x0065, 0x0077, 0x0088, 0x0095, 0x009d},   \
        {0x0000, 0x0000, 0x0001, 0x0004, 0x0008, 0x000e, 0x0017, 0x0022}    \
    },                                                              \
    {                                                               \
        /* vertical decimation [ 0.375 - 0.5) use 0.4 coeff*/                     \
        {0x002b, 0x001d, 0x0012, 0x000a, 0x0005, 0x0002, 0x0000, 0x0000},   \
        {0x00aa, 0x00a7, 0x009e, 0x008f, 0x007d, 0x0066, 0x0051, 0x003d},   \
        {0x002b, 0x003c, 0x0050, 0x0065, 0x0079, 0x008e, 0x009d, 0x00a6},   \
        {0x0000, 0x0000, 0x0000, 0x0002, 0x0005, 0x000a, 0x0012, 0x001d}    \
    },                                                              \
    {                                                               \
        /* vertical decimation [ 0.5 - 0.75) use 0.6 coeff*/                      \
        {0x0020, 0x0012, 0x0009, 0x0004, 0x0001, 0x0000, 0x0000, 0x0000},      \
        {0x00c0, 0x00bc, 0x00ae, 0x0099, 0x0080, 0x0064, 0x004a, 0x0033},      \
        {0x0020, 0x0032, 0x0049, 0x0063, 0x007e, 0x0098, 0x00ad, 0x00bb},      \
        {0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0004, 0x0009, 0x0012}       \
    },                                                              \
    {                                                               \
        /* vertical decimation [ 0.75 -1.0), use 0.75 coeff*/                       \
        {0x0015, 0x0009, 0x0002, 0x0000, 0x03ff, 0x03ff, 0x0000, 0x0000},   \
        {0x00d6, 0x00cf, 0x00bd, 0x00a2, 0x0081, 0x0060, 0x0042, 0x0029},   \
        {0x0015, 0x0028, 0x0041, 0x005f, 0x0081, 0x00a1, 0x00bc, 0x00ce},   \
        {0x0000, 0x0000, 0x0000, 0x03ff, 0x03ff, 0x0000, 0x0002, 0x0009}    \
    }                                                               \
}                                                                   \



/**
 *  Grpx_ScaledSet
 *
 */
typedef enum
{
    GRPX_SC_DS_SET_0 = 0,
    /**< Coefficient for the down sampling (1/4) =< Factor < (3/8). */
    GRPX_SC_DS_SET_1,
    /**< Coefficient for the down sampling (3/8) =< Factor < (1/2). */
    GRPX_SC_DS_SET_2,
    /**< Coefficient for the down sampling (1/2) =< Factor < (3/4). */
    GRPX_SC_DS_SET_3,
    /**< Coefficient for the down sampling (3/4) =< Factor < (1). */
    GRPX_SC_AF,
    /** <Coefficient for anti-flicker case */
    GRPX_SC_US_SET
    /** < Coefficient for up sample 1.0 -> 4.0x */
}Grpx_ScScaleSet;

/**
 *  Grpx_RegionScaledType
 *  \brief region scale type
 *
 */
typedef enum
{
    GRPX_REGION_NO_SCALED = 0,
    /**< no scaled region */
    GRPX_REGION_UP_SCALED ,
    /**< up scaled region */
    GRPX_REGION_DOWN_SCALED
    /**< down scaled region */
}Grpx_RegionScaledType;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
/**
 * getScaleSet(VpsHal_GrpxHandle handle,
 *               UInt16 input, UInt16 output, UInt32 *scale_cnt)
 * Function to calcuate the scaling ratio set
 */
static inline Grpx_ScScaleSet getScaleSet(VpsHal_GrpxHandle handle,
                             UInt16 input, UInt16 output, UInt32 *scale_cnt)
{
    UInt32 scale_count, temp;
    /*input parameter checking*/
    GT_assert(VpsHalTrace, (NULL != handle));

    if (input != output)
    {
        temp = GRPX_SC_RESOLUTION * input * 100 / (output * 100 - 99);
    }
    else
    {
        *scale_cnt = GRPX_SC_RESOLUTION;
        return GRPX_SC_AF;
    }
    *scale_cnt = (UInt32)temp;
    scale_count = GRPX_SC_RESOLUTION * input / output;
    if ((scale_count <= 8196) && (scale_count > 5461))
        return GRPX_SC_DS_SET_0;
    else if ((scale_count <= 5461) && (scale_count > 4096))
        return GRPX_SC_DS_SET_1;
    else if ((scale_count <= 4096) && (scale_count > 2731))
        return GRPX_SC_DS_SET_2;
    else if ((scale_count <=2731) && (scale_count > 2048))
        return GRPX_SC_DS_SET_3;
    else if (scale_count == 2048)
        return GRPX_SC_AF;
    else
        return GRPX_SC_US_SET;

}

/**
 *  userCoeffCopy(Grpx_Handle handle, Ptr coeffMemPtr,
 *                  const UInt16*coeff, UInt32 size)
 *  Function to copy the scaler coefficient to the predefine memory
 *                     location to be used by the VPDMA
 *
 */
static inline Void userCoeffCopy(VpsHal_GrpxHandle handle,
                                 Ptr coeffMemPtr,
                                 const UInt16 *coeff,
                                 UInt32 size)
{
    UInt16                   *coeffPtr = (UInt16 *)coeffMemPtr;
    Int32                    index, phase;
    UInt32                   tap;
    /*input parameter checking*/
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != coeff));
    GT_assert(VpsHalTrace, (NULL != coeffMemPtr));

    /* copy the coefficient to the predefined memory location
     * used by the VPDMA modules
     */

    /*Get #numtap first*/
    tap = size  / VPSHAL_GRPX_SC_COEFF_PHASE;
     /*since the hardware required reversed order of the coeff:
        revere tap while keeping phase order
        Horizontal:
         Coefh0   ->  Coefh4
         Coefh1   ->  Ceofh3
         Coefh2   ->  Coefh2
         Coefh3   ->  Ceofh1
         Coefh4   ->  Coefh0

         Vertical:
         Coefh0   ->  Coefh3
         Coefh1   ->  Ceofh2
         Coefh2   ->  Coefh1
         Coefh3   ->  Ceofh0
     */
    for (index = tap; index >= 1u; index--)
    {
        for (phase = 0u; phase < VPSHAL_GRPX_SC_COEFF_PHASE; phase++)
            *coeffPtr++ = (UInt16)
                (coeff[(index - 1u) * VPSHAL_GRPX_SC_COEFF_PHASE + phase] &
                CSL_VPS_GRPX_REGION_SCALER_ATTRIBUTE_COEFH_P0_MASK);

    }

}

/* None */

#ifdef __cplusplus
extern "C"
}
#endif

#endif
