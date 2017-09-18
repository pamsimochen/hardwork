/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file _csl_nsf.c
 *
 * This File contains CSL Level 0 functions for NSF Module
* 
 *
  * @path  $(DUCATIVOB)\drivers\csl\iss\simcop\nsf\src\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 23-Jan-2009   Sanish Mahadik    : Added the basic register updation functionality
 *! 05-Dec -2008 Sherin Sasidharan:  Created the file.  
 *! 
 *!
 *! Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************
 *  INCLUDE FILES                                                 
 ****************************************************************/
/*-------program files ----------------------------------------*/

#include "../cslr__nsf.h"
#include "../inc/_csl_nsf.h"
#include "../../common/csl_types.h"
#include "../../common/csl_resid.h"

    /* ========================================================================== 
     */
    /* @fn _CSL_nsf2RegisterReset() will set the registers of NSF to their
     * RESET values @see _csl_nsf.h */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsf2RegisterReset(CSL_Nf2Handle hndl) {

        CSL_Status status = CSL_SOK;
        CSL_NsfRegsOvly nsfRegs = hndl->regs;

         nsfRegs->CFG = CSL_NF2_CFG_RESETVAL;
         nsfRegs->BLKW = CSL_NF2_BLKW_RESETVAL;
         nsfRegs->BLKH = CSL_NF2_BLKH_RESETVAL;;
         nsfRegs->IOMST = CSL_NF2_IOMST_RESETVAL;
         nsfRegs->IOMOFST = CSL_NF2_IOMOFST_RESETVAL;
         nsfRegs->WMST = CSL_NF2_WMST_RESETVAL;
         nsfRegs->WMOFST = CSL_NF2_WMOFST_RESETVAL;
         nsfRegs->THR00 = CSL_NF2_THR00_RESETVAL;
         nsfRegs->THR01 = CSL_NF2_THR01_RESETVAL;
         nsfRegs->THR02 = CSL_NF2_THR02_RESETVAL;
         nsfRegs->THR03 = CSL_NF2_THR03_RESETVAL;
         nsfRegs->THR04 = CSL_NF2_THR04_RESETVAL;
         nsfRegs->THR05 = CSL_NF2_THR05_RESETVAL;
         nsfRegs->THR10 = CSL_NF2_THR10_RESETVAL;
         nsfRegs->THR11 = CSL_NF2_THR11_RESETVAL;
         nsfRegs->THR12 = CSL_NF2_THR12_RESETVAL;
         nsfRegs->THR13 = CSL_NF2_THR13_RESETVAL;
         nsfRegs->THR14 = CSL_NF2_THR14_RESETVAL;
         nsfRegs->THR15 = CSL_NF2_THR15_RESETVAL;
         nsfRegs->THR20 = CSL_NF2_THR20_RESETVAL;
         nsfRegs->THR21 = CSL_NF2_THR21_RESETVAL;
         nsfRegs->THR22 = CSL_NF2_THR22_RESETVAL;
         nsfRegs->THR23 = CSL_NF2_THR23_RESETVAL;
         nsfRegs->THR24 = CSL_NF2_THR24_RESETVAL;
         nsfRegs->THR25 = CSL_NF2_THR25_RESETVAL;
         nsfRegs->SHD_XST = CSL_NF2_SHD_XST_RESETVAL;
         nsfRegs->SHD_YST = CSL_NF2_SHD_YST_RESETVAL;
         nsfRegs->SHD_HA1 = CSL_NF2_SHD_HA1_RESETVAL;
         nsfRegs->SHD_HA2 = CSL_NF2_SHD_HA2_RESETVAL;
         nsfRegs->SHD_VA1 = CSL_NF2_SHD_VA1_RESETVAL;
         nsfRegs->SHD_VA2 = CSL_NF2_SHD_VA2_RESETVAL;
         nsfRegs->SHD_HS = CSL_NF2_SHD_HS_RESETVAL;
         nsfRegs->SHD_VS = CSL_NF2_SHD_VS_RESETVAL;
         nsfRegs->SHD_GADJ = CSL_NF2_SHD_GADJ_RESETVAL;
         nsfRegs->SHD_OADJ = CSL_NF2_SHD_OADJ_RESETVAL;
         nsfRegs->MAXG = CSL_NF2_MAXG_RESETVAL;
         nsfRegs->SFT_SLOPE = CSL_NF2_SFT_SLOPE_RESETVAL;
         nsfRegs->EE_L1_SLOPE = CSL_NF2_EE_L1_SLOPE_RESETVAL;
         nsfRegs->EE_L1_THR1 = CSL_NF2_EE_L1_THR1_RESETVAL;
         nsfRegs->EE_L1_THR2 = CSL_NF2_EE_L1_THR2_RESETVAL;
         nsfRegs->EE_L1_OFST2 = CSL_NF2_EE_L1_OFST2_RESETVAL;
         nsfRegs->EE_L2_SLOPE = CSL_NF2_EE_L2_SLOPE_RESETVAL;
         nsfRegs->EE_L2_THR1 = CSL_NF2_EE_L2_THR1_RESETVAL;
         nsfRegs->EE_L2_THR2 = CSL_NF2_EE_L2_THR1_RESETVAL;
         nsfRegs->EE_L2_OFST2 = CSL_NF2_EE_L2_OFST2_RESETVAL;
         nsfRegs->EE_L3_SLOPE = CSL_NF2_EE_L3_SLOPE_RESETVAL;
         nsfRegs->EE_L3_THR1 = CSL_NF2_EE_L3_THR1_RESETVAL;
         nsfRegs->EE_L3_THR2 = CSL_NF2_EE_L3_THR2_RESETVAL;
         nsfRegs->EE_L3_OFST2 = CSL_NF2_EE_L3_OFST2_RESETVAL;
         nsfRegs->IOMSTC = CSL_NF2_IOMST_RESETVAL;
         nsfRegs->DS_THR = CSL_NF2_DS_THR_RESETVAL;
         nsfRegs->DS_SLOPE1 = CSL_NF2_DS_SLOPE1_RESETVAL;
         nsfRegs->DS_SLOPE2 = CSL_NF2_DS_SLOPE2_RESETVAL;

         return status;
    }
    /* ========================================================================== 
     *//* @fn _CSL_nsf2GetAttrs() will return the attributes of the NSF instance 
     * @see _csl_nsf.h *//* ========================================================================== 
     */ CSL_Status _CSL_nsf2GetAttrs(CSL_NsfNum nsfNum, CSL_Nf2Handle hNfs)
    {

        CSL_Status status = CSL_SOK;

        /* get the NSF base address into the handle */

        switch (nsfNum)
        {
            case CSL_NSF_0:
                /* get the NSF unique identifier & resource */
                /* allocation mask into the handle */
                hNfs->xio = CSL_NSF_0_XIO;
                hNfs->uid = CSL_NSF_0_UID;
                hNfs->regs = (CSL_NsfRegsOvly) CSL_NSF_2_REGS;
                hNfs->perNum = nsfNum;
                break;

            default:
                status = CSL_ESYS_OVFL;
        }

        return status;

    }

    /* ========================================================================== 
     */
    /* @fn _CSL_nsfAddrConfig() will configure the address fields of the NSF
     * module in one shot (bulk configuration) @see _csl_nsf.h */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfAddrConfig(CSL_Nf2Handle hndl, CSL_Nf2AddrConfig * data) {
        CSL_Status status = CSL_SOK;

        hndl->regs->BLKW = CSL_FMK(NF2_BLKW_BLKW, data->nsf_blkw);
        hndl->regs->BLKH = CSL_FMK(NF2_BLKH_BLKH, data->nsf_blkh);
        hndl->regs->IOMST = CSL_FMK(NF2_IOMST_ADDR, data->nsf_iomst);
        hndl->regs->IOMOFST = CSL_FMK(NF2_IOMOFST_ADDR, data->nsf_iomofst);
        hndl->regs->WMST = CSL_FMK(NF2_WMST_ADDR, data->nsf_wmst);
        hndl->regs->WMOFST = CSL_FMK(NF2_WMOFST_ADDR, data->nsf_wmofst);
        hndl->regs->IOMSTC = CSL_FMK(NF2_IOMSTC_ADDR, data->nsf_iomstcz);
        return status;
    }

    /* ========================================================================== 
     */
    /* @fn _CSL_nsfThrConfig() will configure the threshold parameters of the 
     * NSF module (bulk configuration) @see _csl_nsf.h */
    /* ========================================================================== 
     */
    CSL_Status _CSL_nsfThrConfig(CSL_Nf2Handle hndl, CSL_Nf2ThrConfig * data) {
        CSL_Status status = CSL_SOK;

        hndl->regs->THR00 = CSL_FMK(NF2_THR00_OFSET_01, data->nsf_thr00);
        hndl->regs->THR01 = CSL_FMK(NF2_THR01_SLOPE_01, data->nsf_thr01);
        hndl->regs->THR02 = CSL_FMK(NF2_THR02_OFSET_02, data->nsf_thr02);
        hndl->regs->THR03 = CSL_FMK(NF2_THR03_SLOPE_02, data->nsf_thr03);
        hndl->regs->THR04 = CSL_FMK(NF2_THR04_OFSET_03, data->nsf_thr04);
        hndl->regs->THR05 = CSL_FMK(NF2_THR05_SLOPE_03, data->nsf_thr05);

        hndl->regs->THR10 = CSL_FMK(NF2_THR10_OFSET_11, data->nsf_thr10);
        hndl->regs->THR11 = CSL_FMK(NF2_THR11_SLOPE_11, data->nsf_thr11);
        hndl->regs->THR12 = CSL_FMK(NF2_THR12_OFSET_12, data->nsf_thr12);
        hndl->regs->THR13 = CSL_FMK(NF2_THR13_SLOPE_12, data->nsf_thr13);
        hndl->regs->THR14 = CSL_FMK(NF2_THR14_OFSET_13, data->nsf_thr14);
        hndl->regs->THR15 = CSL_FMK(NF2_THR15_SLOPE_13, data->nsf_thr15);

        hndl->regs->THR20 = CSL_FMK(NF2_THR20_OFSET_21, data->nsf_thr20);
        hndl->regs->THR21 = CSL_FMK(NF2_THR21_SLOPE_21, data->nsf_thr21);
        hndl->regs->THR22 = CSL_FMK(NF2_THR22_OFSET_22, data->nsf_thr22);
        hndl->regs->THR23 = CSL_FMK(NF2_THR23_SLOPE_22, data->nsf_thr23);
        hndl->regs->THR24 = CSL_FMK(NF2_THR24_OFSET_23, data->nsf_thr24);
        hndl->regs->THR25 = CSL_FMK(NF2_THR25_SLOPE_23, data->nsf_thr25);

        return status;
    }

    /* ========================================================================== 
     */
    /* @fn _CSL_nsfShdConfig() will configure the shading correction
     * parameters of the NSF module (bulk configuration) @see _csl_nsf.h */
    /* ========================================================================== 
     */

    CSL_Status _CSL_nsfShdConfig(CSL_Nf2Handle hndl, CSL_Nf2ShdConfig * data) {
        CSL_Status status = CSL_SOK;

        hndl->regs->SHD_XST = CSL_FMK(NF2_SHD_XST_HCOUNT, data->nsf_shd_xst);
        hndl->regs->SHD_YST = CSL_FMK(NF2_SHD_YST_VCOUNT, data->nsf_shd_xst);
        hndl->regs->SHD_HA1 = CSL_FMK(NF2_SHD_HA1_HA1, data->nsf_shd_ha1);
        hndl->regs->SHD_HA2 = CSL_FMK(NF2_SHD_HA2_HA2, data->nsf_shd_ha2);
        hndl->regs->SHD_VA1 = CSL_FMK(NF2_SHD_VA1_VA1, data->nsf_shd_va1);
        hndl->regs->SHD_VA2 = CSL_FMK(NF2_SHD_VA2_VA2, data->nsf_shd_va2);

        CSL_FINS(hndl->regs->SHD_HS, NF2_SHD_HS_HCS, data->nsf_shd_hs_hsc);
        CSL_FINS(hndl->regs->SHD_HS, NF2_SHD_HS_HS2, data->nsf_shd_hs_hs2);
        CSL_FINS(hndl->regs->SHD_HS, NF2_SHD_HS_HS1, data->nsf_shd_hs_hs1);
        CSL_FINS(hndl->regs->SHD_HS, NF2_SHD_HS_S0, data->nsf_shd_hs_s0);

        CSL_FINS(hndl->regs->SHD_VS, NF2_SHD_VS_VCS, data->nsf_shd_vs_vcs);
        CSL_FINS(hndl->regs->SHD_VS, NF2_SHD_VS_VS2, data->nsf_shd_vs_vs2);
        CSL_FINS(hndl->regs->SHD_VS, NF2_SHD_VS_VS1, data->nsf_shd_vs_vs1);

        hndl->regs->SHD_GADJ = CSL_FMK(NF2_SHD_GADJ_GADJ, data->nsf_shd_gadj);
        hndl->regs->SHD_OADJ = CSL_FMK(NF2_SHD_OADJ_OABJ, data->nsf_shd_oadj);
        hndl->regs->MAXG = CSL_FMK(NF2_MAXG_MAXG, data->nsf_shd_maxg);

        return status;
    }

    /* ========================================================================== 
     */
    /* @fn _CSL_nsfEeLxConfig() will configure the edge enhancement
     * parameters of the NSF module (bulk configuration) @see _csl_nsf.h */
    /* ========================================================================== 
     */

    CSL_Status _CSL_nsfEeLxConfig(CSL_Nf2Handle hndl, CSL_Nf2EeLxConfig * data) {
        CSL_Status status = CSL_SOK;

        // hndl->regs->SFT_SLOPE = CSL_FMK(NF2_SFT_SLOPE_SLOPE,
        // data->nsf_sft_slope);

        hndl->regs->EE_L1_SLOPE =
            CSL_FMK(NF2_EE_L1_SLOPE_SLOPE, data->nsf_ee_l1_slope);
        hndl->regs->EE_L1_THR1 =
            CSL_FMK(NF2_EE_L1_THR1_THR, data->nsf_ee_l1_thr1);
        hndl->regs->EE_L1_THR2 =
            CSL_FMK(NF2_EE_L1_THR2_THR, data->nsf_ee_l1_thr2);
        hndl->regs->EE_L1_OFST2 =
            CSL_FMK(NF2_EE_L1_OFST2_OFST, data->nsf_ee_l1_ofst2);

        hndl->regs->EE_L2_SLOPE =
            CSL_FMK(NF2_EE_L2_SLOPE_SLOPE, data->nsf_ee_l2_slope);
        hndl->regs->EE_L2_THR1 =
            CSL_FMK(NF2_EE_L2_THR1_THR, data->nsf_ee_l2_thr1);
        hndl->regs->EE_L2_THR2 =
            CSL_FMK(NF2_EE_L2_THR2_THR, data->nsf_ee_l2_thr2);
        hndl->regs->EE_L2_OFST2 =
            CSL_FMK(NF2_EE_L2_OFST2_OFST, data->nsf_ee_l2_ofst2);

        hndl->regs->EE_L3_SLOPE =
            CSL_FMK(NF2_EE_L3_SLOPE_SLOPE, data->nsf_ee_l3_slope);
        hndl->regs->EE_L3_THR1 =
            CSL_FMK(NF2_EE_L3_THR1_THR, data->nsf_ee_l3_thr1);
        hndl->regs->EE_L3_THR2 =
            CSL_FMK(NF2_EE_L3_THR2_THR, data->nsf_ee_l3_thr2);
        hndl->regs->EE_L3_OFST2 =
            CSL_FMK(NF2_EE_L3_OFST2_OFST, data->nsf_ee_l3_ofst2);

        return status;
    }

    /* ========================================================================== 
     */
    /* @fn _CSL_nsfDsConfig() will configure the desaturation parameters of
     * the NSF module (bulk configuration) @see _csl_nsf.h */
    /* ========================================================================== 
     */

    CSL_Status _CSL_nsfDsConfig(CSL_Nf2Handle hndl, CSL_Nf2DsConfig * data) {
        CSL_Status status = CSL_SOK;

        CSL_NsfRegsOvly nsf2Regs = hndl->regs;

        CSL_FINS(nsf2Regs->DS_THR, NF2_DS_THR_THR2, data->nsf_ds_thr2);
        CSL_FINS(nsf2Regs->DS_THR, NF2_DS_THR_THR1, data->nsf_ds_thr1);
        nsf2Regs->DS_SLOPE1 = CSL_FMK(NF2_DS_SLOPE1_SLOPE, data->nsf_ds_slope1);
        nsf2Regs->DS_SLOPE2 = CSL_FMK(NF2_DS_SLOPE2_SLOPE, data->nsf_ds_slope2);

        return status;
    }

#ifdef __cplusplus
}
#endif
