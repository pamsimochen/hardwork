/* ==============================================================================
 * * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008, Texas
 * Instruments Incorporated.  All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement under
 * which this software has been supplied. *
 * =========================================================================== */
/**
* @file csl_ldc.h
*
* This File contains declarations for csl_ldc.c
* This entire description will appear as one 
* paragraph in the generated documentation.
*
* @path  $(CSLPATH)\inc\LDC\
*
* @rev  00.01
*/
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 19-Aug -2008 Padmanabha V Reddy:  Created the file.  
 *! 
 *!
 *! 24-Dec-2000 mf: Revisions appear in reverse chronological order;
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef CSL_LDC_H
#define CSL_LDC_H

#ifdef __cplusplus

extern "C" {
#endif
/****************************************************************
*  INCLUDE FILES                                                 
****************************************************************/
/*-------program files ----------------------------------------*/
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
    // #include <ti/psp/iss/hal/iss/simcop/ldc/inc/_csl_ldc.h>
    /****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/

    /* ========================================================================== 
     */
/**
* CSL_LdcObj - Ldc Object structure
* @param  openMode  This is the mode which the CSL instance is opened
* @param  uid  This is a unique identifier to the instance of LDC being referred to by this object
* @param  xio  This is the variable that contains the current state of a resource being shared by current instance of 
                       LDC with other peripherals
* @param  regs  This is a pointer to the registers of the instance of LDC referred to by this object
* @param  lut  This is a pointer to the LDC LUT of the instance of LDC referred to by this object
* @param  perNum  This is the instance of LDC being referred to by this object
*/
    /* ========================================================================== 
     */
    typedef struct {
        CSL_OpenMode openMode;
        CSL_Uid uid;
        CSL_Xio xio;
        CSL_LdcRegsOvly regs;
        CSL_LdcLutOvly lut;
        CSL_LdcNum perNum;
    } CSL_LdcObj;

    /* ========================================================================== 
     */
/**
* CSL_LdcHandle - Pointer to CSL_LdcObj
*/
    /* ========================================================================== 
     */
    typedef CSL_LdcObj *CSL_LdcHandle;

    /* 
     * Enums for Field value Tokens
     */

    /* ========================================================================== 
     */
/**
*CSL_LdcModeType enumeration for selecting mode of operation
*
* @param CSL_LDC_MODE_YCBCR420LD          YCbCr 420 LDC mode
* @param CSL_LDC_MODE_BAYERCA                Bayer CA Mode
* @param CSL_LDC_MODE_YCBCR422LD          YCbCr 422 LDC mode
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_LDC_MODE_YCBCR420LD = CSL_LDC_PCR_MODE_YCBCR420LD,
        CSL_LDC_MODE_BAYERCA = CSL_LDC_PCR_MODE_BAYERCA,
        CSL_LDC_MODE_YCBCR422LD = CSL_LDC_PCR_MODE_YCBCR422LD
    } CSL_LdcModeType;

    /* ========================================================================== 
     */
/** CSL_LdcBayerModeType enumeration for selecting bayer data format applicable only in CA mode only
*
* @param CSL_LDC_BMODE_ALAW         10-bit data a-law compressed to 8-bit
* @param CSL_LDC_BMODE_PKD8BIT       8-bit packed(1 pixel in a byte) 
* @param CSL_LDC_BMODE_PKD12BIT     12-bit packed(2 pixels in 3 bytes) 
* @param CSL_LDC_BMODE_UNPKD12BIT 12-bit unpacked(1 pixel in 2 bytes) 
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_LDC_BMODE_ALAW = CSL_LDC_PCR_BMODE_ALAW,
        CSL_LDC_BMODE_PKD8BIT = CSL_LDC_PCR_BMODE_PKD8BIT,
        CSL_LDC_BMODE_PKD12BIT = CSL_LDC_PCR_BMODE_PKD12BIT,
        CSL_LDC_BMODE_UNPKD12BIT = CSL_LDC_PCR_BMODE_UPKD12BIT
    } CSL_LdcBayerModeType;

    /* ========================================================================== 
     */
/** CSL_LdcYInterpolateMethodType enumeration for interpolation method for Y data
*
* @param CSL_LDC_YINTERPOLATION_BICUBIC     Bicubic Interpolation
* @param CSL_LDC_YINTERPOLATION_BILINEAR    Bilinear Interpolation
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_LDC_YINTERPOLATION_BICUBIC = CSL_LDC_CONFIG_YINT_TYP_BICUBIC,
        CSL_LDC_YINTERPOLATION_BILINEAR = CSL_LDC_CONFIG_YINT_TYP_BILINEAR
    } CSL_LdcYInterpolateMethodType;

    /* ========================================================================== 
     */
/** CSL_LdcInitColorForLDBackMapType enumeration for Initial color for LD back mapping (Bayer mode only)
*
* @param CSL_LDC_INITCOLOR_RED         Red
* @param CSL_LDC_INITCOLOR_GREENRED    GreenRed
* @param CSL_LDC_INITCOLOR_GREENBLUE   GreenBlue
* @param CSL_LDC_INITCOLOR_BLUE        Blue
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_LDC_INITCOLOR_RED = CSL_LDC_CONFIG_INTC_R,
        CSL_LDC_INITCOLOR_GREENRED = CSL_LDC_CONFIG_INTC_GR,
        CSL_LDC_INITCOLOR_GREENBLUE = CSL_LDC_CONFIG_INTC_GB,
        CSL_LDC_INITCOLOR_BLUE = CSL_LDC_CONFIG_INTC_B
    } CSL_LdcInitColorForLDBackMapType;

    typedef CSL_HwAutoGatingType CSL_LdcAutoGatingType;
    typedef CSL_HwEnableType CSL_LdcHwEnableType;
    typedef CSL_HwTrgSrcType CSL_LdcTrgSrcType;
    typedef CSL_HwIntEnableType CSL_LdcIntEnableType;
    typedef CSL_HwStatusType CSL_LdcHwStatusType;

    /* ========================================================================== 
     */
/** Ldc Hardware Control and Query Structures
* @param data_format enumeration of CSL_LdcModeType
   @param bayer_format  enumeration of  CSL_LdcBayerModeType
   @param frame_sizeh window width, in pixels, multiple of obh 
   @param frame_sizew  compute window height, in pixels, multiple of obw
   @param ld_obh output block height, in pixels, for block processing 
   @param ld_obw output block height, in pixels, for block processing
** ========================================================================== */
    typedef struct {
        CSL_LdcModeType data_format;                       /* 0: YCBCR 420,
                                                            * 1: YCBCR 422,
                                                            * 2: Bayer */
        CSL_LdcBayerModeType bayer_format;                 /* 0: Unpacked
                                                            * 12-bit
                                                            * (normal), 1:
                                                            * Packed 12-bit,
                                                            * 2: Packed
                                                            * 8-bit, 3: ALaw
                                                            * 8-bit */
        Bits14 frame_sizeh;                                /* compute window
                                                            * width, in
                                                            * pixels,
                                                            * multiple of obh 
                                                            */
        Bits14 frame_sizew;                                /* compute window
                                                            * height, in
                                                            * pixels,
                                                            * multiple of obw 
                                                            */
        Uint8 ld_obh;                                      /* output block
                                                            * height, in
                                                            * pixels, for
                                                            * block
                                                            * processing */
        Uint8 ld_obw;                                      /* output block
                                                            * height, in
                                                            * pixels, for
                                                            * block
                                                            * processing */
    } CSL_LdcFrameSizeDataFormat;

    /* 
     * Ldc Hardware Control and Query Enums
     */
    /* ========================================================================== 
     */
/** CSL_SimcopHwCtrlCmdType  describes the possible commands issued to write values to Ldc registers.
*  
* This enumeration type is used by CSL_LdcHwControl API
*/
    /* ========================================================================== 
     */
    typedef enum {
        CSL_LDC_CMD_ENABLE,
        CSL_LDC_CMD_ENABLEMAP,
        CSL_LDC_CMD_SETREADBASE,
        CSL_LDC_CMD_SETREADOFFSET,
        CSL_LDC_CMD_SET_DATAFORMAT_FRAMESIZE_OUTPUTBLOCKSIZE,
        CSL_LDC_CMD_SETINITY,
        CSL_LDC_CMD_SETINITX,
        CSL_LDC_CMD_SETWRITEBASE,
        CSL_LDC_CMD_SETWRITEOFFSET,
        CSL_LDC_CMD_SETREADBASE420C,
        CSL_LDC_CMD_SETWRITEBASE420C,
        CSL_LDC_CMD_SETTHRESHOLD,
        CSL_LDC_CMD_SETCONSTANTOOUTPUTADDRESSMODE,
        CSL_LDC_CMD_SETYINTERPOLATIONTYPE,
        CSL_LDC_CMD_SETRIGHTSHIFTBITS,
        CSL_LDC_CMD_SETINITIALCOLOR,
        CSL_LDC_CMD_SETLENSCENTERY,
        CSL_LDC_CMD_SETLENSCENTERX,
        CSL_LDC_CMD_SETKVL,
        CSL_LDC_CMD_SETKVU,
        CSL_LDC_CMD_SETKHR,
        CSL_LDC_CMD_SETKHL,
        CSL_LDC_CMD_SETPIXPAD,
        CSL_LDC_CMD_SETLUTADDRESS,
        CSL_LDC_CMD_SETLUTWRITEDATA,
        CSL_LDC_CMD_SETAFFINEA,
        CSL_LDC_CMD_SETAFFINEB,
        CSL_LDC_CMD_SETAFFINEC,
        CSL_LDC_CMD_SETAFFINED,
        CSL_LDC_CMD_SETAFFINEE,
        CSL_LDC_CMD_SETAFFINEF,
        CSL_LDC_CMD_LUT
    } CSL_LdcHwCtrlCmdType;

    /* ========================================================================== 
     */
/** CSL_LdcHwQueryType  describes the possible commands issued to read values from Simcop registers.
* This enumeration type is used by CSL_LdcGetHwStatus API
*/
    /* ========================================================================== 
     */

    typedef enum {
        CSL_LDC_QUERY_PERIPHERAL_ID,
        CSL_LDC_QUERY_CLASS_ID,
        CSL_LDC_QUERY_REV_ID,
        CSL_LDC_QUERY_ENABLE,
        CSL_LDC_QUERY_ENABLEMAP,
        CSL_LDC_QUERY_BUSY,
        CSL_LDC_QUERY_MODE,
        CSL_LDC_QUERY_BAYERMODE,
        CSL_LDC_QUERY_READBASE,
        CSL_LDC_QUERY_READOFFSET,
        CSL_LDC_QUERY_FRAMEHEIGHT,
        CSL_LDC_QUERY_FRAMEWIDTH,
        CSL_LDC_QUERY_INITY,
        CSL_LDC_QUERY_INITX,
        CSL_LDC_QUERY_WRITEBASE,
        CSL_LDC_QUERY_WRITEOFFSET,
        CSL_LDC_QUERY_THRESHOLD,
        CSL_LDC_QUERY_CONSTANTOOUTPUTADDRESSMODE,
        CSL_LDC_QUERY_READBASE420C,
        CSL_LDC_QUERY_WRITEBASE420C,
        CSL_LDC_QUERY_YINTERPOLATIONTYPE,
        CSL_LDC_QUERY_RIGHTSHIFTBITS,
        CSL_LDC_QUERY_INITIALCOLOR,
        CSL_LDC_QUERY_LENSCENTERY,
        CSL_LDC_QUERY_LENSCENTERX,
        CSL_LDC_QUERY_KVL,
        CSL_LDC_QUERY_KVU,
        CSL_LDC_QUERY_KHR,
        CSL_LDC_QUERY_KHL,
        CSL_LDC_QUERY_PIXPAD,
        CSL_LDC_QUERY_OBH,
        CSL_LDC_QUERY_OBW,
        CSL_LDC_QUERY_LUTADDRESS,
        CSL_LDC_QUERY_LUTREADDATA,
        CSL_LDC_QUERY_AFFINEA,
        CSL_LDC_QUERY_AFFINEB,
        CSL_LDC_QUERY_AFFINEC,
        CSL_LDC_QUERY_AFFINED,
        CSL_LDC_QUERY_AFFINEE,
        CSL_LDC_QUERY_AFFINEF,
        CSL_LDC_QUERY_LUT
    } CSL_LdcHwQueryType;

/*========================================================================== */
/** 
* CSL_LdcHwSetupCtrl - LDC Hardware Setup structure
*/
/*========================================================================== */
    typedef struct {
#ifndef TARGET
        Int16 ih;                                          /* source
                                                            * (distorted)
                                                            * image height,
                                                            * in pixels */
        /* NOT in register definition but needed for C model */
#endif
        CSL_LdcHwEnableType en;                            /* LD enable */
        CSL_LdcHwEnableType ldmapen;                       /* LD back mapping 
                                                            * enable */
        CSL_LdcModeType data_format;                       /* 0: YCBCR 420,
                                                            * 1: YCBCR 422,
                                                            * 2: Bayer */
        CSL_LdcBayerModeType bayer_format;                 /* 0: Unpacked
                                                            * 12-bit
                                                            * (normal), 1:
                                                            * Packed 12-bit,
                                                            * 2: Packed
                                                            * 8-bit, 3: ALaw
                                                            * 8-bit */

        Uint32 frame_rdbase;                               /* source
                                                            * (distorted)
                                                            * image base, in
                                                            * bytes */
        /* included for completeness but not used in C model */
        Uint16 frame_rdoffset;                             /* source image
                                                            * line offset, in 
                                                            * bytes */
        Bits14 frame_sizeh;                                /* compute window
                                                            * width, in
                                                            * pixels,
                                                            * multiple of obh 
                                                            */
        Bits14 frame_sizew;                                /* compute window
                                                            * height, in
                                                            * pixels,
                                                            * multiple of obw 
                                                            */
        Bits14 ld_inity;                                   /* compute window
                                                            * starting x, in
                                                            * pixels, */
        Bits14 ld_initx;                                   /* compute window
                                                            * starting y, in
                                                            * pixels, */
        Uint32 frame_wrbase;                               /* destination
                                                            * (corrected)
                                                            * window starting 
                                                            * addr, in bytes */
        /* included for completeness but not used in C model */
        Uint16 frame_wroffset;                             /* destination
                                                            * image line
                                                            * offset, in
                                                            * bytes */
        /* included for completeness but not used in C model */
        Uint32 frame_rdbase420c;
        Uint32 frame_wrbase420c;
        Bits14 ld_rth;                                     /* R_threshold in
                                                            * back mapping */
        Bool ld_const_md;                                  /* Constant output 
                                                            * address mode */
        CSL_LdcYInterpolateMethodType ld_yint_typ;         /* Interpolation
                                                            * method for Y
                                                            * data. */
        CSL_LdcInitColorForLDBackMapType ld_initc;         /* Starting phase
                                                            * for Bayer mode
                                                            * processing. */
        Bits4 ld_t;                                        /* T, right shift
                                                            * number of bits
                                                            * in back mapping 
                                                            */
        Bits14 ld_v0;                                      /* center Y
                                                            * coordinate, in
                                                            * back mapping */
        Bits14 ld_h0;                                      /* center X
                                                            * coordinate, in
                                                            * back mapping */
        Uint16 ld_kvL;                                     /* vertical lower
                                                            * scaling factor, 
                                                            * in back mapping 
                                                            */
        Uint16 ld_kvU;                                     /* vertical upper
                                                            * scaling factor, 
                                                            * in back mapping 
                                                            */
        Uint16 ld_khR;                                     /* horizontal left 
                                                            * scaling factor, 
                                                            * in back mapping 
                                                            */
        Uint16 ld_khL;                                     /* horizontal
                                                            * right scaling
                                                            * factor, in back 
                                                            * mapping */
        Bits4 ld_pixelpad;                                 /* pixel padding
                                                            * to determine
                                                            * input block for 
                                                            * block
                                                            * processing */
        Uint8 ld_obh;                                      /* output block
                                                            * height, in
                                                            * pixels, for
                                                            * block
                                                            * processing */
        Uint8 ld_obw;                                      /* output block
                                                            * height, in
                                                            * pixels, for
                                                            * block
                                                            * processing */
        Bits14 affine_a;
        Bits14 affine_b;
        Uint16 affine_c;
        Bits14 affine_d;
        Bits14 affine_e;
        Uint16 affine_f;
        Uint16 ld_lut[256];                                /* magnification
                                                            * factor table,
                                                            * 256 entries */
    } CSL_LdcHwSetupCtrl;

/*****************************************************/
/*--------function prototypes ---------------------------------*/

    CSL_Status CSL_ldcInit(CSL_LdcHandle hndl);
    CSL_Status CSL_ldcOpen(CSL_LdcObj * hLdcObj, CSL_LdcNum ldcNum,
                           CSL_OpenMode openMode);
    CSL_Status CSL_ldcHwSetup(CSL_LdcHandle hndl, CSL_LdcHwSetupCtrl * setup);
    CSL_Status CSL_ldcHwControl(CSL_LdcHandle hndl, CSL_LdcHwCtrlCmdType * cmd,
                                void *data);
    CSL_Status CSL_ldcGetHwStatus(CSL_LdcHandle hndl,
                                  CSL_LdcHwQueryType * query, void *data);
    CSL_Status CSL_ldcClose(CSL_LdcHandle hndl);

/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#ifdef __cplusplus
}
#endif
#endif                                                     /* CSL_LDC_H */
