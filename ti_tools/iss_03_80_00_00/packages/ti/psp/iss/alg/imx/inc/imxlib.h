/** ===========================================================================
* @file  imxlib.h
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
//----------------------------------------------------------------------------
//           Copyright (C) 2002 Texas Instruments Incorporated.
//                           All Rights Reserved
//----------------------------------------------------------------------------
// MODULE NAME... IMXLIB - DSC25 Chip IMX Library
// FILENAME...... imxlib.h
// DATE CREATED.. 06Feb02
// PROJECT....... DSC 
//----------------------------------------------------------------------------
// HISTORY:
//   06Feb02   CHT   Created.
//
//----------------------------------------------------------------------------
// DESCRIPTION:  Provides the DSC25 iMX Information
//               
//----------------------------------------------------------------------------

#ifndef _IMXLIB_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMXLIB_H_

#include "imxbase.h"
#include "imxvideoiva.h"
#include "imx_types.h"



//----------------------------------------
// IMXLIB macro declarations
//----------------------------------------




/* D/C/O Points */
#define D_0123    0x4
#define C_0123    0x4
#define O_0123    0x4

#define D_0011    0x2
#define C_0011    0x2
#define O_0011    0x2

#define D_0000    0x1
#define C_0000    0x1
#define O_0000    0x1

#define D_0246    0x3
#define O_0246    0x3
/* IMXACCLP Controls */
#define IMXACCLP_PASS   3
#define IMXACCLP_I4     2

/* IMXACCMODE Controls */
#define IMXACCMOD_SING  0
#define IMXACCMOD_MULT  1



#define IMX_NOMACS      4

#define IMX_NUMMAX_ADDRMOD	4

//----------------------------------------
// IMXLIB typedef declarations
//----------------------------------------

//----------------------------------------
// IMXLIB variable declarations
//----------------------------------------

/* added July 27/07/2011 */
// NEW VERSION
#define IMXTYPE_UBYTE   iMXTYPE_UBYTE
#define IMXTYPE_BYTE    iMXTYPE_BYTE
#define IMXTYPE_USHORT  iMXTYPE_USHORT
#define IMXTYPE_SHORT   iMXTYPE_SHORT
#define IMXTYPE_LONG    iMXTYPE_LONG
#define IMXOTYPE_BYTE   iMXTYPE_BYTE
#define IMXOTYPE_SHORT  iMXTYPE_SHORT
#define IMXOTYPE_LONG   iMXTYPE_LONG
#define IMXTYPE_INTERLEAVED  0x8000
#define IMXOTYPE_INTERLEAVED 0x8000
#define IMXOP_MPY       iMXOP_MPY
#define IMXOP_ABDF      iMXOP_ABSDIFF
#define IMXOP_ADD       iMXOP_ADD
#define IMXOP_SUB       iMXOP_SUB
#define IMXOP_TLU       iMXOP_TLU
#define IMXOP_MIN       iMXOP_MIN
#define IMXOP_MAX       iMXOP_MAX
#define IMXOP_OR        iMXOP_OR
#define IMXOP_AND       iMXOP_AND
#define IMXOP_XOR		iMXOP_XOR
#define IMXOP_NOT		iMXOP_NOT

/* ------------------------ */
/*     Prototypes           */
/* ------------------------ */
extern void Init_updnsmpl_point_op_Params(
                                          updnsmpl_point_op_Params * UpdnsmplPointOpParams
                                          );

extern short imxenc_updnsmpl_point_op(
                                      updnsmpl_point_op_Params * UpdnsmplPointOpParams
                                      );

extern void imx_set_parameters(
  short,
  short,
  short,
  short
                               );

extern void Init_imx_encode(
                            ImxEncInputType * pImxEncParams
                            );

extern Int16 imx_encode(
                        ImxEncInputType * pIn
                        );


/* imx_enc() actually calls imx_enc2() ... */
extern short imx_enc(
  short,
  short,
  short,
  short,
  short,
  short,
  short *data_init,
  short *,
  short *,
  short data_inc1,
  short,
  short,
  short,
  short data_inc_mask1,
  short,
  short,
  short coef_inc1,
  short,
  short,
  short,
  short coef_inc_mask1,
  short,
  short,
  short out_inc1,
  short,
  short,
  short,
  short out_inc_mask1,
  short,
  short,
  short dpoints,
  short,
  short,
  short dsize,
  short csize,
  short osize,
  short acclp,
  short,
  short,
  short *);



/* Same as imx_enc() but allow to align input & coeff ptr on odd byte boundary */
/* imx_enc1() actually calls imx_enc2() */
extern short imx_enc1(
  short,
  short,
  short,
  short,
  short,
  short,
  short *data_init,
  short *,
  short *,
  short data_inc1,
  short,
  short,
  short,
  short data_inc_mask1,
  short,
  short,
  short coef_inc1,
  short,
  short,
  short,
  short coef_inc_mask1,
  short,
  short,
  short out_inc1,
  short,
  short,
  short,
  short out_inc_mask1,
  short,
  short,
  short dpoints,
  short,
  short,
  short dsize,
  short csize,
  short osize,
  short acclp,
  short,
  short,
  short *,
  short,
  short);


/* Same as imx_enc() but allow to align input, coeff & output  ptr on odd byte boundary */
extern short imx_enc2(
  short,
  short,
  short,
  short,
  short,
  short,
  short *data_init,
  short *,
  short *,
  short data_inc1,
  short,
  short,
  short,
  short data_inc_mask1,
  short,
  short,
  short coef_inc1,
  short,
  short,
  short,
  short coef_inc_mask1,
  short,
  short,
  short out_inc1,
  short,
  short,
  short,
  short out_inc_mask1,
  short,
  short,
  short dpoints,
  short,
  short,
  short dsize,
  short csize,
  short osize,
  short acclp,
  short,
  short,
  short *,
  short,
  short,
  short);

/* Same as imx_enc() but parameter rnd_onoff is used to disable rounding */
/* This function should not be used on dm270. It's here really for compatibility */
/* with dsc25. On dm270, use the global variable imxRndOnOff. imx_enc2() encodes */
/* the value of this global variable */
extern short imx_enc_round_on_off(
  short,
  short,
  short,
  short,
  short,
  short,
  short *data_init,
  short *,
  short *,
  short data_inc1,
  short,
  short,
  short,
  short data_inc_mask1,
  short,
  short,
  short coef_inc1,
  short,
  short,
  short,
  short coef_inc_mask1,
  short,
  short,
  short out_inc1,
  short,
  short,
  short,
  short out_inc_mask1,
  short,
  short,
  short dpoints,
  short,
  short,
  short dsize,
  short csize,
  short osize,
  short acclp,
  short,
  short,
  short *);


extern short imxenc_ME(
  short acclp,
  short dcsize,
  short osize,
  short dcpoints,
  short clear,
  short sel,
  short IDmode,
  short IDvalue,
  short rnd_shift,
  short rnd_onoff,
  short lpend1,
  short lpend2,
  short lpend3,
  short lpend4,
  short *data_init,
  short *coef_init,
  short *outp_init,
  short data_inc1,
  short data_inc2,
  short data_inc3,
  short data_inc4,
  short data_inc_mask1,
  short data_inc_mask2,
  short data_inc_mask3,
  short coef_inc1,
  short coef_inc2,
  short coef_inc3,
  short coef_inc4,
  short coef_inc_mask1,
  short coef_inc_mask2,
  short coef_inc_mask3,
  short outp_inc1,
  short outp_inc2,
  short outp_inc3,
  short outp_inc4,
  short outp_inc_mask1,
  short outp_inc_mask2,
  short outp_inc_mask3,
  short *cmd_p,
  short data_highbyte,
  short coef_highbyte);


  // 1 byte/word operation is assumed.
extern short imxenc_MinMax(
  short acclp,
  short dsize,
  short csize,
  short osize,
  short clear,
  short sel,
  short IDmode,
  short IDvalue,
  short rnd_shift,
  short rnd_onoff,
  short lpend1,
  short lpend2,
  short lpend3,
  short lpend4,
  short *data_init,
  short *coef_init,
  short *outp_init,
  short data_inc1,
  short data_inc2,
  short data_inc3,
  short data_inc4,
  short data_inc_mask1,
  short data_inc_mask2,
  short data_inc_mask3,
  short coef_inc1,
  short coef_inc2,
  short coef_inc3,
  short coef_inc4,
  short coef_inc_mask1,
  short coef_inc_mask2,
  short coef_inc_mask3,
  short outp_inc1,
  short outp_inc2,
  short outp_inc3,
  short outp_inc4,
  short outp_inc_mask1,
  short outp_inc_mask2,
  short outp_inc_mask3,
  short *cmd_p);




#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif   /*  _IMXLIB_H_  */
