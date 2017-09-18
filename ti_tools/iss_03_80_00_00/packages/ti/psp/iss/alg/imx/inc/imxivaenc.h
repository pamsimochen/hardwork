/** ===========================================================================
* @file  imxivaenc.h
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
/*------------------------------------------------------------------------------
* MODULE NAME... IMX for IVA
* FILENAME...... imxivaenc.h
* DATE CREATED.. 2003-06-11
* PROJECT....... IVA Chip Support Library
* COMPONENT..... 
* PREREQUISITS.. 
*------------------------------------------------------------------------------
* HISTORY:
*   CREATED:    2003-06-11
*   AUTHOR:
*
* 2003-07-17  For portability, it changed Data-types that defined by 'datatype.h'.
* 2003-08-06  add....IMX_start
*
*------------------------------------------------------------------------------
* DESCRIPTION:  (interface file for the IMX module on IVA)
*               
*
*
*
\******************************************************************************/
#ifndef _IMXIVAENC_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMXIVAENC_H

#ifdef CCS_PROJECT

#include <tistdtypes.h>
#include <imx_types.h>

#else
#include <ti/psp/iss/timmosal/inc/timm_osal_types.h>
#include <ti/psp/iss/alg/imx/inc/imx_types.h>

#endif

#define IMX_RND_ON 0
#define IMX_RND_OFF 1

#define NO_OF_MACS    4

/* ------------------------ */
/*    Offsets into iMX      */
/* ------------------------ */
#define   IMXENC_OPCODE2OFST    1
#define   IMXENC_LPEND12OFST    2
#define   IMXENC_LPEND34OFST    3
#define   IMXENC_INOFST       4
#define   IMXENC_COEFFOFST    5
#define   IMXENC_OUTOFST      6
#define   IMXENC_ADDRINCOFST    7
#define 	SUMARROP_CMD2OFST		15
/* pfernandez */
#define 	COPYREF_CMD2OFST		14       //13
#define 	INTERP_CMDOFST			18       //17
/* ~pfernandez */
#define 	BI_INTERP_CMDOFST				17
#define		ARRAYPOINTOP_SCALAR_CMDOFST		14

/* ------------------------ */
/*     Commands             */
/* ------------------------ */
#define IMXCMD_SLEEP        0x8000
#define IMXCMD_SETUP_PARAM  0xC000
#define CMDMEMWR        0xD800
#define NOPCMD270       0xF800
#define CMDMEMMSK       0x00FF
#define CMDME       0xE800
#define CMDSETME      0xF000
#define SMEADDMASK      0x01FF
#define CMDMEMWR      0xD800
#define NOPCMD270     0xF800
#define CMDMEMMSK     0x00FF
#define CMDMINMAX       0xE000
#define PLAINCALL			0xC800
#define PCADDRMASK          0x07FF
#define JUMPCALL            0x8800
#define IMXCMD_RETURN       0xb000    
#define CALLWITHAM          0xD000
#define IMXCMD_SETUP_PARAM_MUL32  0xC002

#define IMXACCLP_I4    2
#define IMXACCLP_I3    1
#define IMXACCLP_I2    0
#define IMXACCLP_PASS  3

#define IMXINC_ALWAYS		0
#define IMXINC_I4 			1
#define IMXINC_I4I3 		3
#define IMXINC_I4I3I2 		7

// Read pattern
#define IMX_DPTS_0123     4       /* dpoints settings */
#define IMX_DPTS_0011     2
#define IMX_DPTS_0000     1
#define IMX_DPTS_0022     3
#define IMX_DPTS_0246_BYT 3

#define IMX_CPTS_0123     4       /* cpoints settings */
#define IMX_CPTS_0101     2
#define IMX_CPTS_0000     1
#define IMX_CPTS_0202     3
#define IMX_CPTS_0246_BYT 3

#define IMX_OPTS_0123     4       /* opoints settings */
#define IMX_OPTS_02       2
#define IMX_OPTS_0        1
#define IMX_OPTS_0_2      3
#define IMX_OPTS_0_1_2_3_BYT 3


/* ------------------------ */
/*     Types                */
/* ------------------------ */
#define iMXTYPE_SIZEMASK  0x0F
#define iMXTYPE_UNSIGNED  0x10
#define iMXTYPE_BYTE      0x1
#define iMXTYPE_SHORT     0x2
#define iMXTYPE_USHORT    (iMXTYPE_SHORT | iMXTYPE_UNSIGNED)
#define iMXTYPE_UBYTE     (iMXTYPE_BYTE  | iMXTYPE_UNSIGNED)
// NEW VERSION
#define IMXTYPE_BYTE    iMXTYPE_BYTE
#define IMXTYPE_SHORT   iMXTYPE_SHORT

#define imx_getSizeFromInputType(a) (a & iMXTYPE_SIZEMASK)
#define imx_getSizeFromOutputType(a) (a & iMXTYPE_SIZEMASK)

/* ------------------------ */
/*     Operators            */
/* ------------------------ */
void Init_imx_encode(ImxEncInputType *pImxEncParams);

Int16 imx_encode(ImxEncInputType *pInParams);

Int16 Compute_InitEncodeAddr(Uint32 InitAddr, Int16 high_byte);

#define iMXOP_MPY       0x0
#define iMXOP_ABSDIFF   0x1
#define iMXOP_ADD       0x2
#define iMXOP_SUB       0x3
/* ------------------------ */
/*     Extra Operators            */
/* ------------------------ */
#define iMXOP_MIN       0x5
#define iMXOP_MAX       0x6
#define iMXOP_OR        0x7

Int16 imx_enc(
  Int16 acc_mode,                      /* 0: single acc, 1: multiple, indexed by i4 */
  Int16 op,                            /* 0: mpy, 1: absdiff, 2: add, 3: sub, 4: lookup */
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 * data_init,
  Int16 * coef_init,
  Int16 * outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 dpoints,
  Int16 cpoints,
  Int16 opoints,
  Int16 dsize,
  Int16 csize,
  Int16 osize,
  Int16 acclp,
  Int16 rnd_shift,
  Int16 coeff_unsigned,
  Int16 * cmd_p);


Int16 imx_enc1(
  Int16 acc_mode,                      /* 0: single acc, 1: multiple, indexed by i4 */
  Int16 op,                            /* 0: mpy, 1: absdiff, 2: add, 3: sub, 4: lookup */
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 * data_init,
  Int16 * coef_init,
  Int16 * outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 dpoints,
  Int16 cpoints,
  Int16 opoints,
  Int16 dsize,
  Int16 csize,
  Int16 osize,
  Int16 acclp,
  Int16 rnd_shift,
  Int16 coeff_unsigned,
  Int16 * cmd_p,
  Int16 highbyte_data,
  Int16 highbyte_coeff);


Int16 imx_enc_round_off(
  Int16 acc_mode,                      /* 0: single acc, 1: multiple, indexed by i4 */
  Int16 op,                            /* 0: mpy, 1: absdiff, 2: add, 3: sub, 4: lookup */
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 * data_init,
  Int16 * coef_init,
  Int16 * outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 dpoints,
  Int16 cpoints,
  Int16 opoints,
  Int16 dsize,
  Int16 csize,
  Int16 osize,
  Int16 acclp,
  Int16 rnd_shift,
  Int16 rnd_onoff,                     /* 1: rnd off, 0: rnd on */
  Int16 * cmd_p);


Int16 imx_enc2(
  Int16 acc_mode,                      /* 0: single acc, 1: multiple, indexed by i4 */
  Int16 op,                            /* 0: mpy, 1: absdiff, 2: add, 3: sub, 4: lookup */
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 * data_init,
  Int16 * coef_init,
  Int16 * outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 dpoints,
  Int16 cpoints,
  Int16 opoints,
  Int16 dsize,
  Int16 csize,
  Int16 osize,
  Int16 acclp,
  Int16 rnd_shift,
  Int16 coeff_unsigned,
  Int16 * cmd_p,
  Int16 highbyte_data,
  Int16 highbyte_coeff,
  Int16 highbyte_out);

Int16 imx_enc_call_ptr_mod_command(
               Int16 *called_cmd_addr,                      /* 0: single acc, 1: multiple, indexed by i4 */
               Int16 * data_init,
               Int16 * coef_init,
               Int16 * outp_init,
               Int16 dsize,
               Int16 csize,
               Int16 osize,
               Int16 *cmd_p,
               Int16 highbyte_data,
               Int16 highbyte_coeff,
               Int16 highbyte_out);

Int16 imx_enc_cmd_write_command(
	           Int16 * dst_cmdmem_addr,
               Int16 * srcptr_addr,
               Int16    numof16bit_words,
               Int16 * cmd_p,
               Int16 highbyte_data,
               Int16 highbyte_coeff,
               Int16 highbyte_out);

void imx_set_parameters(
  Int16 a_sat_high,
  Int16 a_sat_high_set,
  Int16 a_sat_low,
  Int16 a_sat_low_set);


Int16 imxenc_set_parameters(
  Int16 a_sat_high,
  Int16 a_sat_high_set,
  Int16 a_sat_low,
  Int16 a_sat_low_set,
  Int16 * cmd_p);

Int16 imxenc_set_parameters_sign32(
  Int16 sat_unsigned,    /*  1bit  */
  Int16 sat_high,        /* 16bits */
  Int16 sat_high_set,    /* 16bits */
  Int16 sat_low,         /* 16bits */
  Int16 sat_low_set,     /* 16bits */
  Int16 *cmd_ptr );

Int16 imxenc_set_parameters_sign(
  Int16 sat_unsigned,               /*  1bit  */
  Int16 sat_high,                      /* 16bits */
  Int16 sat_high_set,               /* 16bits */
  Int16 sat_low,                       /* 16bits */
  Int16 sat_low_set,                /* 16bits */
  Int16 *cmd_ptr );

Int16 imxenc_sleep(
  Int16 * cmd_p);
Int16 imxenc_nop(
  Int16 * cmd_p);

Int16 imxenc_ME2(
  Int16 acclp,
  Int16 dcsize,
  Int16 osize,
  Int16 dcpoints,
  Int16 clear,
  Int16 sel,
  Int16 IDmode,
  Int16 IDvalue,
  Int16 rnd_shift,
  Int16 rnd_onoff,
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 * data_init,
  Int16 * coef_init,
  Int16 * outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 * cmd_p,
  Int16 data_highbyte,
  Int16 coef_highbyte);


Int16 imxenc_setME(
  Int16 sel,
  Int16 param,
  Int16 * cmd_p);
Int16 imxenc_cmdwrite(
  Int16 * cmdaddr,
  Int16 * startaddr,
  Int16 number,
  Int16 * cmd_p);


Int16 imxenc_MinMax(
  // 1 byte/word operation is assumed.
  Int16 acclp,
  Int16 dsize,
  Int16 csize,
  Int16 osize,
  Int16 clear,
  Int16 sel,
  Int16 IDmode,
  Int16 IDvalue,
  Int16 rnd_shift,
  Int16 rnd_onoff,
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 * data_init,
  Int16 * coef_init,
  Int16 * outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 * cmd_p);

Int16 determine_coef_uns(
  Int16 input1_type,
  Int16 input2_type,
  Int16 * input1_ptr,
  Int16 * input2_ptr);

Int16 imxenc_init_acc( 
  Int16 exponent, 
  Int16 *cmd_p);


Int16 imx_mul32_enc2(
  Int16 lpend1,
  Int16 lpend2,
  Int16 lpend3,
  Int16 lpend4,
  Int16 *data_init,
  Int16 *coef_init,
  Int16 *outp_init,
  Int16 data_inc1,
  Int16 data_inc2,
  Int16 data_inc3,
  Int16 data_inc4,
  Int16 data_inc_mask1,
  Int16 data_inc_mask2,
  Int16 data_inc_mask3,
  Int16 coef_inc1,
  Int16 coef_inc2,
  Int16 coef_inc3,
  Int16 coef_inc4,
  Int16 coef_inc_mask1,
  Int16 coef_inc_mask2,
  Int16 coef_inc_mask3,
  Int16 outp_inc1,
  Int16 outp_inc2,
  Int16 outp_inc3,
  Int16 outp_inc4,
  Int16 outp_inc_mask1,
  Int16 outp_inc_mask2,
  Int16 outp_inc_mask3,
  Int16 osize,
  Int16 acclp,
  Int16 rnd_shift,
  Int16 coeff_unsigned,
  Int16 *cmd_p,
  Int16 highbyte_data,
  Int16 highbyte_coeff,
  Int16 highbyte_out
  );

Int16 imxenc_init_acc( 
  Int16 exponent, 
  Int16 *cmd_p);

Int16 imx_encSetMaskParams(SetMaskParamCmd* setMaskParamCmd, Int16 *cmd_p);

void imxgenInc(Int16 *lpend, Int16 *step, Int16 numLoops, Int16 *inc);

void Init_imx_encode(ImxEncInputType *pImxEncParams);
Int16 imx_encode(ImxEncInputType *pInParams);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
