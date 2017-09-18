/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file csl_nsf.h
 *
 * This File contains data-structure and function declarations for csl_nsf2.c 
 *
 * @path  $(DUCATIVOB)\drivers\csl\iss\simcop\nsf\
 *
 * @rev  00.01
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 27-Jan - 2009 Sanish Mahdik: Modified the data-types and added few more 
 *! 09-Dec -2008 Sherin Sasidharan:  Created the file.  
 *! 
 *!
 *!  mf: Revisions appear in reverse chronological order;
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifndef _CSL_NF2_H_
#define _CSL_NF2_H_

#ifdef __cplusplus
extern "C" {
#endif

   /****************************************************************
    * INCLUDE FILES
    ****************************************************************/
    /* ----- system and platform files -------------------------- */
   /*-------program files ----------------------------------------*/
#include "../common/simcop.h"
#include "../common/csl_types.h"
#include "../common/csl_error.h"
#include "cslr__nsf.h"

   /****************************************************************
    * EXTERNAL REFERENCES NOTE : only use if not found in header file
    ****************************************************************/
   /*--------data declarations -----------------------------------*/
   /*--------function prototypes --------------------------------*/
   /*--------------------------- END -------------------------------*/
   /****************************************************************
    * PUBLIC DECLARATIONS Defined here, used elsewhere
    ****************************************************************/
   /*--------data-structure declarations ----------------------------------*/

    /* ======================================================================= 
     */
    /* ! \struct CSL_Nf2Obj CSL_Nf2Obj - this is the object structure which
     * holds the NSF instance information @param openMode :This is the mode
     * which the CSL instance is opened @param uid :This is a unique
     * identifier to the instance of NSF being referred to by this object
     * @param xio : This is the variable that contains the current state of a 
     * resource being shared by current instance of NSF with other
     * peripherals @param regs :This is a pointer to the registers of the
     * instance of NSF referred to by this object @param perNum : This is the 
     * instance of NSF being referred to by this object */
    /* ======================================================================= 
     */

    typedef struct {
        CSL_OpenMode openMode;
        CSL_Uid uid;
        CSL_Xio xio;
        CSL_NsfRegsOvly regs;
        CSL_NsfNum perNum;
    } CSL_Nf2Obj;

    typedef CSL_Nf2Obj *CSL_Nf2Handle;

    typedef CSL_HwEnableType CSL_Nf2HwEnableType;

    /* ======================================================================= 
     */
    /* ! \enum CSL_Nf2ConfigType CSL_Nf2ConfigType enumeration for
     * configuration of NSF parameters @param CSL_SIMCOP_NSF_CFG_START
     * :START the NSF processing @param CSL_SIMCOP_NSF_CFG_INT_EN :Enable
     * interrupts @param CSL_SIMCOP_NSF_CFG_OUT_EN :Output enable @param
     * CSL_SIMCOP_NSF_CFG_MODE :YUV422/YUV420 mode @param
     * CSL_SIMCOP_NSF_CFG_TRIG_SRC :Triggered by MMR/HWSEQ @param
     * CSL_SIMCOP_NSF_CFG_C_AS_INTRLV_Y :Treat UV data as 2 interleaved
     * planes @param CSL_SIMCOP_NSF_CFG_RESERVED_6 :RESERVED @param
     * CSL_SIMCOP_NSF_CFG_SOFT_THR_EN_LUMA :enable soft threshold for LUMA
     * @param CSL_SIMCOP_NSF_CFG_SOFT_THR_EN_CHROMA :enable soft threshold
     * for CHROMA @param CSL_SIMCOP_NSF_CFG_CBCR_EN :enable filtering for
     * chroma @param CSL_SIMCOP_NSF_CFG_Y_EN :enable filtering for luma
     * @param CSL_SIMCOP_NSF_CFG_LBKEEP :reset contents of WMEM @param
     * CSL_SIMCOP_NSF_CFG_EE_EN :enable edge enhancement @param
     * CSL_SIMCOP_NSF_CFG_SHD_EN :enable shading gain correction @param
     * CSL_SIMCOP_NSF_CFG_DESAT_EN :enable desaturation @param
     * CSL_SIMCOP_NSF_CFG_BUSY :BUSY/Idle */
   /*===================================================================*/
    typedef enum {
        CSL_SIMCOP_NSF_CFG_START,
        CSL_SIMCOP_NSF_CFG_INT_EN,
        CSL_SIMCOP_NSF_CFG_OUT_EN,
        CSL_SIMCOP_NSF_CFG_MODE,
        CSL_SIMCOP_NSF_CFG_TRIG_SRC,
        CSL_SIMCOP_NSF_CFG_C_AS_INTRLV_Y,
        CSL_SIMCOP_NSF_CFG_RESERVED_6,
        CSL_SIMCOP_NSF_CFG_SOFT_THR_EN_LUMA,
        CSL_SIMCOP_NSF_CFG_SOFT_THR_EN_CHROMA,
        CSL_SIMCOP_NSF_CFG_CBCR_EN,
        CSL_SIMCOP_NSF_CFG_Y_EN,
        CSL_SIMCOP_NSF_CFG_LBKEEP,
        CSL_SIMCOP_NSF_CFG_EE_EN,
        CSL_SIMCOP_NSF_CFG_SHD_EN,
        CSL_SIMCOP_NSF_CFG_DESAT_EN,
        CSL_SIMCOP_NSF_CFG_BUSY
    } CSL_Nf2ConfigType;

    /* ======================================================================= 
     */
    /* ! \enum CSL_Nf2AddressType CSL_Nf2AddressType enumeration for
     * configuration of NSF address parameters @param CSL_SIMCOP_NSF_BLKW
     * block width @param CSL_SIMCOP_NSF_BLKH block height @param
     * CSL_SIMCOP_NSF_IOMST IOMEM start address @param CSL_SIMCOP_NSF_IOMOFST 
     * IOMEM offset @param CSL_SIMCOP_NSF_WMST WMEM start address @param
     * CSL_SIMCOP_NSF_WMOFST WMEM offset @param CSL_SIMCOP_NSF_IOMSTC IOMEM
     * start address for UV data */
    /* ======================================================================= 
     */
    typedef enum {
        CSL_SIMCOP_NSF_BLKW,
        CSL_SIMCOP_NSF_BLKH,
        CSL_SIMCOP_NSF_IOMST,
        CSL_SIMCOP_NSF_IOMOFST,
        CSL_SIMCOP_NSF_WMST,
        CSL_SIMCOP_NSF_WMOFST,
        CSL_SIMCOP_NSF_IOMSTC
    } CSL_Nf2AddressType;

    /* ======================================================================= 
     */
    /* ! \struct CSL_Nf2AddrConfig CSL_Nf2AddressType structure for
     * configuration of NSF address parameters @param nsf_blkw block width
     * @param nsf_blkh block height @param nsf_iomst IOMEM start address
     * @param nsf_iomofst IOMEM offset @param nsf_wmst WMEM start address
     * @param nsf_wmofst WMEM offset @param nsf_iomstcz IOMEM start address
     * for UV data */
    /* ======================================================================= 
     */
    typedef struct {
        Uint16 nsf_blkw;
        Uint16 nsf_blkh;
        Uint16 nsf_iomst;
        Uint16 nsf_iomofst;
        Uint16 nsf_wmst;
        Uint16 nsf_wmofst;
        Uint16 nsf_iomstcz;
    } CSL_Nf2AddrConfig;

    /* ======================================================================= 
     */
    /* ! \enum CSL_Nf2ThrType CSL_Nf2ThrType enumeration for selecting color
     * and level: offset/slope @param CSL_SIMCOP_NSF_THR00 color 0 level 1
     * offset, U10Q2 @param CSL_SIMCOP_NSF_THR01 color 0 level 1 slope, S9Q10
     * @param CSL_SIMCOP_NSF_THR02 color 0 level 2 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR03 color 0 level 2 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR04 color 0 level 3 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR05 color 0 level 3 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR10 color 1 level 1 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR11 color 1 level 1 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR12 color 1 level 2 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR13 color 1 level 2 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR14 color 1 level 3 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR15 color 1 level 3 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR20 color 2 level 1 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR21 color 2 level 1 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR22 color 2 level 2 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR23 color 2 level 2 slope, S9Q10 @param
     * CSL_SIMCOP_NSF_THR24 color 2 level 3 offset, U10Q2 @param
     * CSL_SIMCOP_NSF_THR25 color 2 level 3 slope, S9Q10 */
    /* ======================================================================= 
     */
    typedef enum {
        CSL_SIMCOP_NSF_THR00,
        CSL_SIMCOP_NSF_THR01,
        CSL_SIMCOP_NSF_THR02,
        CSL_SIMCOP_NSF_THR03,
        CSL_SIMCOP_NSF_THR04,
        CSL_SIMCOP_NSF_THR05,

        CSL_SIMCOP_NSF_THR10,
        CSL_SIMCOP_NSF_THR11,
        CSL_SIMCOP_NSF_THR12,
        CSL_SIMCOP_NSF_THR13,
        CSL_SIMCOP_NSF_THR14,
        CSL_SIMCOP_NSF_THR15,

        CSL_SIMCOP_NSF_THR20,
        CSL_SIMCOP_NSF_THR21,
        CSL_SIMCOP_NSF_THR22,
        CSL_SIMCOP_NSF_THR23,
        CSL_SIMCOP_NSF_THR24,
        CSL_SIMCOP_NSF_THR25
    } CSL_Nf2ThrType;

    /* ======================================================================= 
     */
    /* ! \struct CSL_Nf2ThrConfig CSL_Nf2ThrConfig structure for configuring
     * color and level: offset/slope @param nsf_thr00 color 0 level 1
     * offset, U10Q2 @param nsf_thr01 color 0 level 1 slope, S9Q10 @param
     * nsf_thr02 color 0 level 2 offset, U10Q2 @param nsf_thr03 color 0 level 
     * 2 slope, S9Q10 @param nsf_thr04 color 0 level 3 offset, U10Q2 @param
     * nsf_thr05 color 0 level 3 slope, S9Q10 @param nsf_thr10 color 1 level 
     * 1 offset, U10Q2 @param nsf_thr11 color 1 level 1 slope, S9Q10 @param
     * nsf_thr12 color 1 level 2 offset, U10Q2 @param nsf_thr13 color 1 level 
     * 2 slope, S9Q10 @param nsf_thr14 color 1 level 3 offset, U10Q2 @param
     * nsf_thr15 color 1 level 3 slope, S9Q10 @param nsf_thr20 color 2 level 
     * 1 offset, U10Q2 @param nsf_thr21 color 2 level 1 slope, S9Q10 @param
     * nsf_thr22 color 2 level 2 offset, U10Q2 @param nsf_thr23 color 2 level 
     * 2 slope, S9Q10 @param nsf_thr24 color 2 level 3 offset, U10Q2 @param
     * nsf_thr25 color 2 level 3 slope, S9Q10 */
    /* ======================================================================= 
     */
    typedef struct {
        Int16 nsf_thr00;
        Int16 nsf_thr01;
        Int16 nsf_thr02;
        Int16 nsf_thr03;
        Int16 nsf_thr04;
        Int16 nsf_thr05;

        Int16 nsf_thr10;
        Int16 nsf_thr11;
        Int16 nsf_thr12;
        Int16 nsf_thr13;
        Int16 nsf_thr14;
        Int16 nsf_thr15;

        Int16 nsf_thr20;
        Int16 nsf_thr21;
        Int16 nsf_thr22;
        Int16 nsf_thr23;
        Int16 nsf_thr24;
        Int16 nsf_thr25;
    } CSL_Nf2ThrConfig;

    /* ======================================================================= 
     */
    /* ! \enum CSL_Nf2ShdType CSL_Nf2ShdType enumeration for selecting
     * shading gain parameters @param CSL_SIMCOP_NSF_SHD_XST shading gain
     * horizontal start count @param CSL_SIMCOP_NSF_SHD_YST shading gain
     * vertical start count @param CSL_SIMCOP_NSF_SHD_HA1 shading gain HA1
     * coefficient @param CSL_SIMCOP_NSF_SHD_HA2 shading gain HA2 coefficient
     * @param CSL_SIMCOP_NSF_SHD_VA1 shading gain VA1 coefficient @param
     * CSL_SIMCOP_NSF_SHD_VA2 shading gain VA2 coefficient @param
     * CSL_SIMCOP_NSF_SHD_HS_HSC shading gain HSC coefficient @param
     * CSL_SIMCOP_NSF_SHD_HS2 shading gain HS2 coefficient @param
     * CSL_SIMCOP_NSF_SHD_HS1 shading gain HS1 shift count @param
     * CSL_SIMCOP_NSF_SHD_S0 shading gain S0 shift count @param
     * CSL_SIMCOP_NSF_SHD_VS_VCS shading gain VCS coefficient @param
     * CSL_SIMCOP_NSF_SHD_VS_VS2 shading gain VS2 coefficient @param
     * CSL_SIMCOP_NSF_SHD_VS_VS1 shading gain VS1 coefficient @param
     * CSL_SIMCOP_NSF_SHD_GADJ shading gain gain adjustment, unsigned @param
     * CSL_SIMCOP_NSF_SHD_OADJ Shading gain offset adjustment, S13Q5 @param
     * CSL_SIMCOP_NSF_MAXG shading gain maximum gain, U9Q5 */
    /* ======================================================================= 
     */
    typedef enum {
        CSL_SIMCOP_NSF_SHD_XST,
        CSL_SIMCOP_NSF_SHD_YST,
        CSL_SIMCOP_NSF_SHD_HA1,
        CSL_SIMCOP_NSF_SHD_HA2,
        CSL_SIMCOP_NSF_SHD_VA1,
        CSL_SIMCOP_NSF_SHD_VA2,
        CSL_SIMCOP_NSF_SHD_HS_HSC,
        CSL_SIMCOP_NSF_SHD_HS_HS2,
        CSL_SIMCOP_NSF_SHD_HS_HS1,
        CSL_SIMCOP_NSF_SHD_HS_S0,
        CSL_SIMCOP_NSF_SHD_VS_VCS,
        CSL_SIMCOP_NSF_SHD_VS_VS2,
        CSL_SIMCOP_NSF_SHD_VS_VS1,
        CSL_SIMCOP_NSF_SHD_GADJ,
        CSL_SIMCOP_NSF_SHD_OADJ,
        CSL_SIMCOP_NSF_MAXG
    } CSL_Nf2ShdType;

    /* ======================================================================= 
     */
    /* ! \struct CSL_Nf2ShdConfig CSL_Nf2ShdConfig structure for configuring
     * shading gain parameters @param nsf_shd_xst shading gain horizontal
     * start count @param nsf_shd_yst shading gain vertical start count
     * @param nsf_shd_ha1 shading gain HA1 coefficient @param nsf_shd_ha2
     * shading gain HA2 coefficient @param nsf_shd_va1 shading gain VA1
     * coefficient @param nsf_shd_va2 shading gain VA2 coefficient @param
     * nsf_shd_hs_hsc shading gain HSC coefficient @param nsf_shd_hs_hs2
     * shading gain HS2 coefficient @param nsf_shd_hs_hs1 shading gain HS1
     * shift count @param nsf_shd_hs_s0 shading gain S0 shift count @param
     * nsf_shd_vs_vcs shading gain VCS coefficient @param nsf_shd_vs_vs2
     * shading gain VS2 coefficient @param nsf_shd_vs_vs1 shading gain VS1
     * coefficient @param nsf_shd_vs_vs1 shading gain gain adjustment,
     * unsigned @param nsf_shd_oadj Shading gain offset adjustment, S13Q5
     * @param nsf_shd_maxg shading gain maximum gain, U9Q5 */
    /* ======================================================================= 
     */
    typedef struct {
        Uint16 nsf_shd_xst;
        Uint16 nsf_shd_yst;
        Uint16 nsf_shd_ha1;
        Uint16 nsf_shd_ha2;
        Uint16 nsf_shd_va1;
        Uint16 nsf_shd_va2;
        Uint16 nsf_shd_hs_hsc;
        Uint16 nsf_shd_hs_hs2;
        Uint16 nsf_shd_hs_hs1;
        Uint16 nsf_shd_hs_s0;
        Uint16 nsf_shd_vs_vcs;
        Uint16 nsf_shd_vs_vs2;
        Uint16 nsf_shd_vs_vs1;
        Uint16 nsf_shd_gadj;
        Uint16 nsf_shd_oadj;
        Uint16 nsf_shd_maxg;
    } CSL_Nf2ShdConfig;

    /* ======================================================================= 
     */
    /* ! \enum CSL_Nf2EeLxType CSL_Nf2EeLxType enumeration for selecting
     * edge enhancement parameters @param CSL_SIMCOP_NSF_SFT_SLOPE
     * soft-threshold slope select {0, 1,2,4}/8 @param
     * CSL_SIMCOP_NSF_EE_L1_SLOPE edge enhancement slope, select {1, 2, ...,
     * 8}/2 @param CSL_SIMCOP_NSF_EE_L1_THR1 edge enhancement threshold 1,
     * U10Q2 @param CSL_SIMCOP_NSF_EE_L1_THR2 edge enhancement threshold 2,
     * U10Q2 @param CSL_SIMCOP_NSF_EE_L1_OFST2 edge enhancement offset 2,
     * U10Q2, ofst2 = thr1 + slope * (thr2 - thr1) @param
     * CSL_SIMCOP_NSF_EE_L2_SLOPE edge enhancement slope, select {1, 2, ...,
     * 8}/2 @param CSL_SIMCOP_NSF_EE_L2_THR1 edge enhancement threshold 1,
     * U10Q2 @param CSL_SIMCOP_NSF_EE_L2_THR2 edge enhancement threshold 2,
     * U10Q2 @param CSL_SIMCOP_NSF_EE_L2_OFST2 edge enhancement offset 2,
     * U10Q2, ofst2 = thr1 + slope * (thr2 - thr1) @param
     * CSL_SIMCOP_NSF_EE_L3_SLOPE edge enhancement slope, select {1, 2, ...,
     * 8}/2 @param CSL_SIMCOP_NSF_EE_L3_THR1 edge enhancement threshold 1,
     * U10Q2 @param CSL_SIMCOP_NSF_EE_L3_THR2 edge enhancement threshold 2,
     * U10Q2 @param CSL_SIMCOP_NSF_EE_L3_OFST2 edge enhancement offset 2,
     * U10Q2, ofst2 = thr1 + slope * (thr2 - thr1) */
    /* ======================================================================= 
     */
    typedef enum {
        CSL_SIMCOP_NSF_SFT_SLOPE,

        CSL_SIMCOP_NSF_EE_L1_SLOPE,
        CSL_SIMCOP_NSF_EE_L1_THR1,
        CSL_SIMCOP_NSF_EE_L1_THR2,
        CSL_SIMCOP_NSF_EE_L1_OFST2,

        CSL_SIMCOP_NSF_EE_L2_SLOPE,
        CSL_SIMCOP_NSF_EE_L2_THR1,
        CSL_SIMCOP_NSF_EE_L2_THR2,
        CSL_SIMCOP_NSF_EE_L2_OFST2,

        CSL_SIMCOP_NSF_EE_L3_SLOPE,
        CSL_SIMCOP_NSF_EE_L3_THR1,
        CSL_SIMCOP_NSF_EE_L3_THR2,
        CSL_SIMCOP_NSF_EE_L3_OFST2
    } CSL_Nf2EeLxType;

    /* ======================================================================= 
     */
    /* ! \struct CSL_Nf2EeLxConfig CSL_Nf2EeLxConfig structure for
     * configuring edge enhancement parameters @param nsf_sft_slope
     * soft-threshold slope select {0, 1,2,4}/8 @param nsf_ee_l1_slope edge
     * enhancement slope, select {1, 2, ..., 8}/2 @param nsf_ee_l1_thr1 edge
     * enhancement threshold 1, U10Q2 @param nsf_ee_l1_thr2 edge enhancement
     * threshold 2, U10Q2 @param nsf_ee_l1_ofst2 edge enhancement offset 2,
     * U10Q2, ofst2 = thr1 + slope * (thr2 - thr1) @param nsf_ee_l2_slope
     * edge enhancement slope, select {1, 2, ..., 8}/2 @param nsf_ee_l2_thr1
     * edge enhancement threshold 1, U10Q2 @param nsf_ee_l2_thr2 edge
     * enhancement threshold 2, U10Q2 @param nsf_ee_l2_ofst2 edge enhancement 
     * offset 2, U10Q2, ofst2 = thr1 + slope * (thr2 - thr1) @param
     * nsf_ee_l3_slope edge enhancement slope, select {1, 2, ..., 8}/2 @param 
     * nsf_ee_l3_thr1 edge enhancement threshold 1, U10Q2 @param
     * nsf_ee_l3_thr2 edge enhancement threshold 2, U10Q2 @param
     * nsf_ee_l3_ofst2 edge enhancement offset 2, U10Q2, ofst2 = thr1 + slope 
     * * (thr2 - thr1) */
    /* ======================================================================= 
     */
    typedef struct {
        /* Uint16 nsf_sft_slope; */

        Uint16 nsf_ee_l1_slope;
        Uint16 nsf_ee_l1_thr1;
        Uint16 nsf_ee_l1_thr2;
        Uint16 nsf_ee_l1_ofst2;

        Uint16 nsf_ee_l2_slope;
        Uint16 nsf_ee_l2_thr1;
        Uint16 nsf_ee_l2_thr2;
        Uint16 nsf_ee_l2_ofst2;

        Uint16 nsf_ee_l3_slope;
        Uint16 nsf_ee_l3_thr1;
        Uint16 nsf_ee_l3_thr2;
        Uint16 nsf_ee_l3_ofst2;
    } CSL_Nf2EeLxConfig;

    /* ======================================================================= 
     */
    /* ! \enum CSL_Nf2DsType CSL_Nf2DsType enumeration for selecting
     * desaturation parameters @param CSL_SIMCOP_NSF_DS_THR desaturation
     * threshold 1/2, U6Q0 @param CSL_SIMCOP_NSF_DS_SLOPE1 Desaturation slope 
     * 1, U10Q10 @param CSL_SIMCOP_NSF_DS_SLOPE2 Desaturation slope 2, U10Q10 */
    /* ======================================================================= 
     */
    typedef enum {
        CSL_SIMCOP_NSF_DS_THR2,
        CSL_SIMCOP_NSF_DS_THR1,
        CSL_SIMCOP_NSF_DS_SLOPE1,
        CSL_SIMCOP_NSF_DS_SLOPE2
    } CSL_Nf2DsType;

    /* ======================================================================= 
     */
    /* ! \struct CSL_Nf2DsConfig CSL_Nf2DsConfig structure for configuring
     * desaturation parameters @param CSL_SIMCOP_NSF_DS_THR desaturation
     * threshold 1/2, U6Q0 @param CSL_SIMCOP_NSF_DS_SLOPE1 Desaturation slope 
     * 1, U10Q10 @param CSL_SIMCOP_NSF_DS_SLOPE2 Desaturation slope 2, U10Q10 */
    /* ======================================================================= 
     */
    typedef struct {
        Uint16 nsf_ds_thr2;
        Uint16 nsf_ds_thr1;
        Uint16 nsf_ds_slope1;
        Uint16 nsf_ds_slope2;
    } CSL_Nf2DsConfig;

    typedef CSL_Nf2ConfigType CSL_Nf2HwQueryType;

    /* ======================================================================= 
     */
    /* ! \enum CSL_NSF_SOFTSLOPE Smoothening parameter enumeration for NSF
     * @param NSF_SOFTSLOPE_0 :Soft-threshold slope =0 @param NSF_SOFTSLOPE_1 
     * :Soft-threshold slope =1/8 @param NSF_SOFTSLOPE_2 :Soft-threshold
     * slope =1/4 @param NSF_SOFTSLOPE_3 :Soft-threshold slope =1/2 */
    /* ======================================================================= 
     */
    typedef enum {
        NSF_SOFTSLOPE_0 = 0,
        NSF_SOFTSLOPE_1 = 1,
        NSF_SOFTSLOPE_2 = 2,
        NSF_SOFTSLOPE_3 = 3
    } CSL_NSF_SOFTSLOPE;

    /* ========================================================================== 
     */
    /* ! struct CSL_Nf2HwSetupCtrl CSL_Nf2HwSetupCtrl - NSF Hardware Setup
     * structure
     * 
     * * @param START Start NSF processing @param INT_EN Enable interrupts
     * @param OUT_EN Output enable @param MODE YUV422/YUV420 mode @param
     * TRIG_SRC Triggered by MMR/HWSEQ @param C_AS_INTRLV_Y Treat UV data as 
     * 2 interleaved planes @param SOFT_THR_EN_LUMA enable soft threshold
     * for LUMA @param SOFT_THR_EN_CHROMA enable soft threshold for CHROMA
     * @param CBCR_EN enable filtering for chroma @param Y_EN enable
     * filtering for luma @param LBKEEP reset contents of WMEM @param EE_EN 
     * enable edge enhancement @param SHD_EN enable shading gain correction
     * @param DESAT_EN enable desaturation @param address_config address
     * configuration parameters @param threshold_config threshold
     * configuration parameters @param shading_config shading correction
     * configuration parameters @param edge_config adge enhancement
     * configuration parameters @param desaturation_config desaturation
     * configuration parameters */
    /* ========================================================================== 
     */
    typedef struct {

        CSL_Nf2HwEnableType START;
        CSL_Nf2HwEnableType INT_EN;
        CSL_Nf2HwEnableType OUT_EN;
        CSL_Nf2HwEnableType MODE;
        CSL_Nf2HwEnableType TRIG_SRC;
        CSL_Nf2HwEnableType C_AS_INTRLV_Y;
        CSL_Nf2HwEnableType SOFT_THR_EN_LUMA;
        CSL_Nf2HwEnableType SOFT_THR_EN_CHROMA;
        CSL_Nf2HwEnableType CBCR_EN;
        CSL_Nf2HwEnableType Y_EN;
        CSL_Nf2HwEnableType LBKEEP;
        CSL_Nf2HwEnableType EE_EN;
        CSL_Nf2HwEnableType SHD_EN;
        CSL_Nf2HwEnableType DESAT_EN;

        CSL_NSF_SOFTSLOPE soft_slope_value;
        CSL_Nf2AddrConfig *address_config;
        CSL_Nf2ThrConfig *threshold_config;
        CSL_Nf2ShdConfig *shading_config;
        CSL_Nf2EeLxConfig *edge_config;
        CSL_Nf2DsConfig *desaturation_config;

    } CSL_Nf2HwSetupCtrl;

   /*--------function prototypes --------------------------------*/

    /* ========================================================================== 
     */
    /* ! CSL_nsf2Init() initializes the NSF2 with the RESET values @param
     * hndl Handle to the NSF2 object @return status CSL_SOK Succesfull open 
     * Other value = Open failed and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status CSL_nf2Init(CSL_Nf2Handle hndl);

    /* ========================================================================== 
     */
    /* ! CSL_nsf2Open() The open call sets up the data structures for the
     * particular instance of the NSF. It gets a handle to the NSF2 module
     * of SIMCOP @param hNsf2Obj Handle to the NSF2 object @param nsf2Num
     * Instance of NSF device= @param openMode Open mode (Shared/Exclusive)
     * @return status CSL_SOK Succesfull open Other value = Open failed and
     * the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status CSL_nf2Open(CSL_Nf2Obj * hNsf2Obj, CSL_NsfNum nsf2Num,
                           CSL_OpenMode openMode);

    /* ========================================================================== 
     */
    /* ! CSL_nsf2HwSetup() Sets the H/w for NSF2 module at one shot with the 
     * sttings provided by the application @param hndl Handle to the NSF2
     * object @param setup Setup structure containing values for register
     * settings @return status CSL_SOK Succesfull Close Other value = Close
     * failed and the error code is returned. */
    /* ========================================================================== 
     */
    CSL_Status CSL_nf2HwSetup(CSL_Nf2Handle hndl, CSL_Nf2HwSetupCtrl * setup);

    /* ========================================================================== 
     */
    /* ! CSL_nsf2Close() The close call deallocates resources for NSF for
     * the particular instance of the NSF for which the handle is passed
     * @param hndl Handle to the NSF object @return status CSL_SOK
     * Succesfull Close Other value = Close failed and the error code is
     * returned. */
    /* ========================================================================== 
     */
    CSL_Status CSL_nf2Close(CSL_Nf2Handle hndl);

#ifdef __cplusplus
}
#endif
#endif
