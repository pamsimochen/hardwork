/** ==================================================================
 *  @file   issutils_grpx.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/******************************************************************************
 **+-------------------------------------------------------------------------+**
 **|                            ****                                         |**
 **|                            ****                                         |**
 **|                            ******o***                                   |**
 **|                      ********_///_****                                  |**
 **|                      ***** /_//_/ ****                                  |**
 **|                       ** ** (__/ ****                                   |**
 **|                           *********                                     |**
 **|                            ****                                         |**
 **|                            ***                                          |**
 **|                                                                         |**
 **|         Copyright (c) 2008-2009 Texas Instruments Incorporated          |**
 **|                        ALL RIGHTS RESERVED                              |**
 **|                                                                         |**
 **| Permission is hereby granted to licensees of Texas Instruments          |**
 **| Incorporated (TI) products to use this computer program for the sole    |**
 **| purpose of implementing a licensee product based on TI products.        |**
 **| No other rights to reproduce, use, or disseminate this computer         |**
 **| program, whether in part or in whole, are granted.                      |**
 **|                                                                         |**
 **| TI makes no representation or warranties with respect to the            |**
 **| performance of this computer program, and specifically disclaims        |**
 **| any responsibility for any damages, special or consequential,           |**
 **| connected with the use of this program.                                 |**
 **|                                                                         |**
 **+-------------------------------------------------------------------------+**
 ******************************************************************************/

/**  \file      issutils_grpx.c
 *
 *   \brief     Code to generate color bar patter based on the format and dimension.
 *
 *   (C) Copyright 2010, Texas Instruments, Inc
 *
 *   \author    PSP
 *
 *
 */

/* --------------------------------------------------------------------------- 
 * Include Files
 * --------------------------------------------------------------------------- */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/psp/iss/common/trace.h>
#include <ti/psp/vps/fvid2.h>
/* --------------------------------------------------------------------------- 
 * Global variables
 * --------------------------------------------------------------------------- */
/* used for ARGB444 or RGBA444,where alpha = 0; */
static UInt16 rgb444[2][8] = {
    {
     (0x0F << 8) | (0x0F << 4) | (0x0F),
     (0x00 << 8) | (0x00 << 4) | (0x00),
     (0x0F << 8) | (0x00 << 4) | (0x00),
     (0x00 << 8) | (0x0F << 4) | (0x00),
     (0x00 << 8) | (0x00 << 4) | (0x0F),
     (0x0F << 8) | (0x0F << 4) | (0x00),
     (0x0F << 8) | (0x00 << 4) | (0x0F),
     (0x00 << 8) | (0x0F << 4) | (0x0F),
     },
    {
     (0x00 << 8) | (0x0F << 4) | (0x0F),
     (0x0F << 8) | (0x00 << 4) | (0x0F),
     (0x0F << 8) | (0x0F << 4) | (0x00),
     (0x00 << 8) | (0x00 << 4) | (0x0F),
     (0x00 << 8) | (0x0F << 4) | (0x00),
     (0x0F << 8) | (0x00 << 4) | (0x00),
     (0x00 << 8) | (0x00 << 4) | (0x00),
     (0x0F << 8) | (0x0F << 4) | (0x0F),
     }
};

/* usedfor ARGB1555 or RGBA5551, where alpha = 0 */
static UInt32 rgb555[2][8] = {
    {
     (0x1F << 10) | (0x1F << 5) | (0x1F),
     (0x00 << 10) | (0x00 << 5) | (0x00),
     (0x1F << 10) | (0x00 << 5) | (0x00),
     (0x00 << 10) | (0x1F << 5) | (0x00),
     (0x00 << 10) | (0x00 << 5) | (0x1F),
     (0x1F << 10) | (0x1F << 5) | (0x00),
     (0x1F << 10) | (0x00 << 5) | (0x1F),
     (0x00 << 10) | (0x1F << 5) | (0x1F),
     },
    {
     (0x00 << 10) | (0x1F << 5) | (0x1F),
     (0x1F << 10) | (0x00 << 5) | (0x1F),
     (0x1F << 10) | (0x1F << 5) | (0x00),
     (0x00 << 10) | (0x00 << 5) | (0x1F),
     (0x00 << 10) | (0x1F << 5) | (0x00),
     (0x1F << 10) | (0x00 << 5) | (0x00),
     (0x00 << 10) | (0x00 << 5) | (0x00),
     (0x1F << 10) | (0x1F << 5) | (0x1F),

     }
};

/* used for ARGB6666 or RGBA6666, where alpha = 0 */
static UInt32 rgb666[2][8] = {
    {
     (0x3F << 12) | (0x3F << 6) | (0x3F),
     (0x00 << 12) | (0x00 << 6) | (0x00),
     (0x3F << 12) | (0x00 << 6) | (0x00),
     (0x00 << 12) | (0x3F << 6) | (0x00),
     (0x00 << 12) | (0x00 << 6) | (0x3F),
     (0x3F << 12) | (0x3F << 6) | (0x00),
     (0x3F << 12) | (0x00 << 6) | (0x3F),
     (0x00 << 12) | (0x3F << 6) | (0x3F),
     },
    {
     (0x00 << 12) | (0x3F << 6) | (0x3F),
     (0x3F << 12) | (0x00 << 6) | (0x3F),
     (0x3F << 12) | (0x3F << 6) | (0x00),
     (0x00 << 12) | (0x00 << 6) | (0x3F),
     (0x00 << 12) | (0x3F << 6) | (0x00),
     (0x3F << 12) | (0x00 << 6) | (0x00),
     (0x00 << 12) | (0x00 << 6) | (0x00),
     (0x3F << 12) | (0x3F << 6) | (0x3f),

     }
};

/* used for RGB565 format */
static UInt16 rgb565[2][8] = {
    {
     (0x1F << 11) | (0x3F << 5) | (0x1F),
     (0x00 << 11) | (0x00 << 5) | (0x00),
     (0x1F << 11) | (0x00 << 5) | (0x00),
     (0x00 << 11) | (0x3F << 5) | (0x00),
     (0x00 << 11) | (0x00 << 5) | (0x1F),
     (0x1F << 11) | (0x3F << 5) | (0x00),
     (0x1F << 11) | (0x00 << 5) | (0x1F),
     (0x00 << 11) | (0x3F << 5) | (0x1F),
     },
    {
     (0x00 << 11) | (0x3F << 5) | (0x1F),
     (0x1F << 11) | (0x00 << 5) | (0x1F),
     (0x1F << 11) | (0x3F << 5) | (0x00),
     (0x00 << 11) | (0x00 << 5) | (0x1F),
     (0x00 << 11) | (0x3F << 5) | (0x00),
     (0x1F << 11) | (0x00 << 5) | (0x00),
     (0x00 << 11) | (0x00 << 5) | (0x00),
     (0x1F << 11) | (0x3F << 5) | (0x1F),
     }
};

/* used for ARGB8888/RGBA8888/RGB888 format, where alpha = 0 */
static UInt32 rgb888[2][8] = {
    {
     (0xFF << 16) | (0xFF << 8) | (0xFF),
     (0x00 << 16) | (0x00 << 8) | (0x00),
     (0xFF << 16) | (0x00 << 8) | (0x00),
     (0x00 << 16) | (0xFF << 8) | (0x00),
     (0x00 << 16) | (0x00 << 8) | (0xFF),
     (0xFF << 16) | (0xFF << 8) | (0x00),
     (0xFF << 16) | (0x00 << 8) | (0xFF),
     (0x00 << 16) | (0xFF << 8) | (0xFF),
     },
    {
     (0x00 << 16) | (0xFF << 8) | (0xFF),
     (0xFF << 16) | (0x00 << 8) | (0xFF),
     (0xFF << 16) | (0xFF << 8) | (0x00),
     (0x00 << 16) | (0x00 << 8) | (0xFF),
     (0x00 << 16) | (0xFF << 8) | (0x00),
     (0xFF << 16) | (0x00 << 8) | (0x00),
     (0x00 << 16) | (0x00 << 8) | (0x00),
     (0xFF << 16) | (0xFF << 8) | (0xFF),
     },
};

/* --------------------------------------------------------------------------- 
 * Function implementation
 * --------------------------------------------------------------------------- */
/*******************************************************************************
 ** Function name:     IssUtils_getPitch
 **
 ** Description:       Get the ptich based on the width and bpp.
 ******************************************************************************/

/**
 * \brief IssUtils_getPitch
 *
 *  Function to calculate the pitch
 */

/* ===================================================================
 *  @func     IssUtils_getPitch                                               
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
UInt32 IssUtils_getPitch(UInt32 width, UInt32 bpp)
{
    UInt32 pitch;

    pitch = (width * bpp >> 3);
    if (pitch & 0xF)
        pitch += 16 - (pitch & 0xF);

    return pitch;
}

/**
 * \brief IssUtils_grpxGenPattern
 *
 *  Function to create color bar based on the format and dimension
 */

/* ===================================================================
 *  @func     IssUtils_grpxGenPattern                                               
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
void IssUtils_grpxGenPattern(UInt8 * addr,
                             UInt32 df,
                             UInt32 width,
                             UInt32 height, UInt8 reversed, UInt8 alpha)
{
    int i, j, k;

    UInt32 pitch;

    switch (df)
    {
        case FVID2_DF_ARGB32_8888:
        {
            UInt32 *start = (UInt32 *) addr;

            pitch = IssUtils_getPitch(width, 32);

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 4); k++)
                    {
                        start[k] = rgb888[reversed][i] | (alpha << 24);
                    }
                    start += (pitch / 4);
                }
            }
            break;
        }
        case FVID2_DF_RGBA32_8888:
        {
            UInt32 *start = (UInt32 *) addr;

            pitch = IssUtils_getPitch(width, 32);

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 4); k++)
                    {
                        /* make the Alpha to zero at the lsb */
                        start[k] = (rgb888[reversed][i] << 8) | alpha;
                    }
                    start += (pitch / 4);
                }
            }
            break;
        }
        case FVID2_DF_RGB24_888:
        {
            UInt8 *start = (UInt8 *) addr;

            int l;

            pitch = IssUtils_getPitch(width, 24);

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < width; k++)
                    {
                        for (l = 0; l < 3; l++)
                        {
                            start[3 * k + l] =
                                (rgb888[reversed][i] >> (8 * l)) & 0xFF;
                        }

                    }
                    start += pitch;
                }
            }
            break;
        }

        case FVID2_DF_RGB16_565:
        {
            UInt16 *start = (UInt16 *) addr;

            pitch = IssUtils_getPitch(width, 16);
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 2); k++)
                    {
                        start[k] = rgb565[reversed][i];
                    }
                    start += (pitch / 2);
                }
            }
            break;
        }
        case FVID2_DF_ARGB16_1555:
        {
            UInt16 *start = (UInt16 *) addr;

            pitch = IssUtils_getPitch(width, 16);
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 2); k++)
                    {
                        start[k] = rgb555[reversed][i] | ((alpha & 1) << 15);
                    }
                    start += (pitch / 2);
                }
            }

            break;
        }
        case FVID2_DF_RGBA16_5551:
        {
            UInt16 *start = (UInt16 *) addr;

            pitch = IssUtils_getPitch(width, 16);
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 2); k++)
                    {
                        start[k] = (rgb555[reversed][i] << 1) | (alpha & 1);
                    }
                    start += (pitch / 2);
                }
            }

            break;
        }

        case FVID2_DF_ARGB16_4444:
        {
            UInt16 *start = (UInt16 *) addr;

            pitch = IssUtils_getPitch(width, 16);

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 2); k++)
                    {
                        start[k] = rgb444[reversed][i] | ((alpha & 0xF) << 12);
                    }
                    start += (pitch / 2);
                }
            }
            break;
        }
        case FVID2_DF_RGBA16_4444:
        {
            UInt16 *start = (UInt16 *) addr;

            pitch = IssUtils_getPitch(width, 16);

            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < (pitch / 2); k++)
                    {
                        start[k] = (rgb444[reversed][i] << 4) | (alpha & 0xF);
                    }
                    start += (pitch / 2);
                }
            }

            break;
        }

        case FVID2_DF_ARGB24_6666:
        {
            UInt8 *start = (UInt8 *) addr;

            int l;

            UInt32 temp;

            pitch = IssUtils_getPitch(width, 24);
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < width; k++)
                    {
                        temp = rgb666[reversed][i] | ((alpha & 0x3F) << 18);
                        for (l = 0; l < 3; l++)
                        {

                            start[3 * k + l] = ((temp >> (8 * l)) & 0xFF);
                        }

                    }
                    start += pitch;
                }
            }
            break;
        }
        case FVID2_DF_RGBA24_6666:
        {
            UInt8 *start = (UInt8 *) addr;

            int l;

            UInt32 temp;

            pitch = IssUtils_getPitch(width, 24);
            for (i = 0; i < 8; i++)
            {
                for (j = 0; j < (height / 8); j++)
                {
                    for (k = 0; k < width; k++)
                    {
                        temp = (rgb666[reversed][i] << 6) | (alpha & 0x3F);
                        for (l = 0; l < 3; l++)
                        {
                            start[k * 3 + l] = (temp >> (8 * l)) & 0xFF;
                        }

                    }
                    start += pitch;
                }
            }
            break;
        }

    }
}
