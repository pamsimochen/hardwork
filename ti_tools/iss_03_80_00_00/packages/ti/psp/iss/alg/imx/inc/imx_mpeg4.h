/** ===========================================================================
* @file  imx_mpeg4.h
*
* @brief .
*
*
* <b><i>Target(s):</i></b> DM290, DM299
*
* =============================================================================
*
* Copyright (c) Texas Instruments Inc 2006
*
* Use of this software is controlled by the terms and conditions found
* in the license agreement under which this software has been supplied
*
* ===========================================================================
*
*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include"imxbase.h"

/**************************************************
 * Sreenik: Prototypes for imx_mpeg4.c
 *
 **************************************************/
short imxenc_mbPackSpecial(
  short *curr_ptr,
  short *coeff_ptr,
  short *result_ptr_Y,
  short *result_ptr_UV,
  short input_type,
  short output_width,
  short swap_CbCr,
  short *cmd_ptr);

short imxenc_mbUnpackRowSpecial(
  short *curr_ptr,
  short *coeff_ptr,
  short *result_ptr_Y,
  short *result_ptr_UV,
  short *cmd_ptr);

short imxenc_mbUnpackSpecial(
  short *curr_ptr,
  short *coeff_ptr,
  short *result_ptr_Y,
  short *result_ptr_UV,
  short *cmd_ptr);

short imxenc_fullMV(
  short *full_ptr,
  short *half_ptr,
  short *SADMV0,
  short *bias_table,
  short *zero_ptr,
  short *coeff_full_ptr,
  short *coeff_half_ptr,
  short *coef_n32,
  short *coef_1,
  short *out_ptr,
  short *cmd_ptr);

short imxenc_updateMcomp(
  short *lum_ptr,
  short *mv_ptr,
  short *coef_0,
  short *and_mask,
  short *mcUpdate,
  short *mc_array,
  short *mvTemp,
  short *out_ptr,
  short *mCompCmd,
  short bordermv_0,
  short *cmd_ptr);


short imxenc_hpel_interpf(
  short *refblk_ptr,                   /* starting address of reference block input */
  short *best_ref_ptr,                 /* starting address of 18x18 best
                                        * reference block array space */
  short *zero_ptr,                     /* USHORT zero pointer (needed for copy operations) */
  short *interp_output_ptr,            /* starting address of 103x16
                                        * interp output array space */
  short best_ref_offset,               /* DSP address offset of best_ref match
                                        * in the refblk */
  short high_byte,                     /* odd byte offset for best_ref_offset */
  short blk_width,                     /* Width of the block to interp */
  short blk_height,                    /* Height of the block to interp */
  short *cmdptr);
void
imxenc_hpel_interpf_round_control(
  RND_Control *rndCtrl, 
  int enable);
#ifdef __cplusplus
}
#endif /* __cplusplus */
