/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshalCscDefaults.h
 *
 * \brief VPS CSC HAL default configuration file
 * This file contains default configuration i.e. expert values for
 * Color Space Convertor.
 *
 */

#ifndef _VPSHALCSCDEFAULTS_H
#define _VPSHALCSCDEFAULTS_H

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
 *  \brief Expert value for the anchor and interpolated pixel
 */
#define VPSHAL_CSC_DEFAULT_EXPERT_VAL                                          \
{                                                                              \
    /* VPS_CSC_MODE_SDTV_VIDEO_R2Y */                                          \
    {{{0x0132, 0x0259, 0x0075}, {0x1F50, 0x1EA5, 0x020B},                      \
      {0x020B, 0x1EA4, 0x1FAB}}, {0x0000, 0x0200, 0x0200}},                    \
                                                                               \
    /* VPS_CSC_MODE_SDTV_VIDEO_Y2R */                                          \
    {{{0x0400, 0x0000, 0x057D}, {0x0400, 0x1EA7, 0x1D35},                      \
      {0x0400, 0x06EF, 0x1FFE}}, {0x0D40, 0x0210, 0x0C88}},                    \
                                                                               \
    /* VPS_CSC_MODE_SDTV_GRAPHICS_R2Y */                      \
    {{{0x0107, 0x0204, 0x0064}, {0x1F68, 0x1ED6, 0x01C2},                      \
      {0x01C2, 0x1E87, 0x1FB7}}, {0x0040, 0x0200, 0x0200}},                    \
                                                                               \
    /* VPS_CSC_MODE_SDTV_GRAPHICS_Y2R */                                       \
    {{{0x04A8, 0x1FFE, 0x0662}, {0x04A8, 0x1E6F, 0x1CBF},                      \
      {0x04A8, 0x0812, 0x1FFF}}, {0x0C84, 0x0220, 0x0BAC}},                    \
                                                                               \
    /* VPS_CSC_MODE_HDTV_VIDEO_R2Y */                         \
    {{{0x00DA, 0x02DC, 0x004A}, {0x1F88, 0x1E6C, 0x020C},                      \
      {0x020C, 0x1E24, 0x1FD0}}, {0x0000, 0x0200, 0x0200}},                    \
                                                                               \
    /* VPS_CSC_MODE_HDTV_VIDEO_Y2R */                                          \
    {{{0x0400, 0x0000, 0x0629}, {0x0400, 0x1F45, 0x1E2B},                      \
     {0x0400, 0x0742, 0x0000}}, {0x0CEC, 0x0148, 0x0C60}},                     \
                                                                               \
    /* VPS_CSC_MODE_HDTV_GRAPHICS_R2Y */                     \
    {{{0x00BB, 0x0275, 0x003F}, {0x1F99, 0x1EA5, 0x01C2},                      \
      {0x01C2, 0x1E67, 0x1FD7}}, {0x0040, 0x0200, 0x0200}},                    \
                                                                               \
    /* VPS_CSC_MODE_HDTV_GRAPHICS_Y2R   */                                     \
    {{{0x04A8, 0x0000, 0x072C}, {0x04A8, 0x1F26, 0x1DDE},                      \
      {0x04A8, 0x0873, 0x0000}}, {0x0C20, 0x0134, 0x0B7C}}                     \
}

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None*/

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHALCSCDEFAULTS_H */
