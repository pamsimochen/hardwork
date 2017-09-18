/** ===========================================================================
* @file  imxbase.h
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
/* BASE FILE FOR IMX: COMMON ACROSS ALL CHIPS, NO CHIP SPECIFIC INFO
   SHOULD BE INCLUDED
*/
#ifndef  _IMXBASE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define  _IMXBASE_H_

#ifdef CCS_PROJECT

#include <tistdtypes.h>
#include <stdlib.h>

#else

#include <stdlib.h>
//#include <WTSD_DucatiMMSW/alg/common/tistdtypes.h>
#include <ti/psp/iss/timmosal/inc/timm_osal_types.h>

#endif


/* IMX_ALIGN can be used to align a quantity to the # macs */
/* IMX_NOMACS must be defined in the file imxlib.h */
#define IMX_ALIGN(a)   ( ((a+IMX_NOMACS-1)/IMX_NOMACS)*IMX_NOMACS )

#define MAX_ROUND_VALS 5

// Used for Dynamic Control of Rounding for Certain APIs
typedef struct _RND_Control {
  short num_vals;
  short *vals[MAX_ROUND_VALS];
} RND_Control;

extern RND_Control *imxenc_round_control_ptr;

void imxenc_enable_round_control(
                                 RND_Control *rndCtrl
                                 );

void imxenc_set_round_control(
                              RND_Control *rndCtrl, 
                              int enable
                              );

extern Uint16 imxRndOnOff;

#define IMX_RND_ON 0
#define IMX_RND_OFF 1

#define IMX_SEQ_MODE_OFF 1
#define IMX_SEQ_MODE_ON  4


/* ------------------------ */
/*     Types                */
/* ------------------------ */

#define iMX_SHIFT_TYPE 0x100
#define iMXTYPE_INTER 0x100

#define iMXTYPE_UNSIGNED  0x10
#define iMXTYPE_ROUNDONMASK 0x100

#define iMXTYPE_LONG      0x4
#define iMXTYPE_SHORT     0x2
#define iMXTYPE_BYTE      0x1

#define iMXTYPE_ULONG     (iMXTYPE_LONG | iMXTYPE_UNSIGNED)
#define iMXTYPE_USHORT    (iMXTYPE_SHORT | iMXTYPE_UNSIGNED)
#define iMXTYPE_UBYTE     (iMXTYPE_BYTE  | iMXTYPE_UNSIGNED)
#define iMXTYPE_BYTE_INTER  (iMXTYPE_BYTE | iMXTYPE_INTER)
#define iMXTYPE_UBYTE_INTER (iMXTYPE_UBYTE | iMXTYPE_INTER)

#define iMXTYPE_SIZEMASK  0x0F
#define iMXTYPE_SIZE_SIGN_MASK  0xFF

/* ------------------------ */
/*     Operators            */
/* ------------------------ */
#define iMXOP_MPY       0x0
#define iMXOP_ABSDIFF   0x1
#define iMXOP_ADD       0x2
#define iMXOP_SUB       0x3
#define iMXOP_TLU		0x4


/* ------------------------ */
/*     Extra Operators            */
/* ------------------------ */
#define iMXOP_MIN       0x5
#define iMXOP_MAX       0x6
#define iMXOP_OR        0x7
#define iMXOP_AND        0x8005
#define iMXOP_XOR        0x8006
#define iMXOP_NOT        0x8007
// NEW VERSION

#define IMXTYPE_UBYTE   iMXTYPE_UBYTE
#define IMXTYPE_BYTE    iMXTYPE_BYTE
#define IMXTYPE_USHORT  iMXTYPE_USHORT
#define IMXTYPE_SHORT   iMXTYPE_SHORT
#define IMXTYPE_LONG    iMXTYPE_LONG
#define IMXTYPE_ULONG   iMXTYPE_USHORT

#define IMXOTYPE_BYTE   iMXTYPE_BYTE
#define IMXOTYPE_SHORT  iMXTYPE_SHORT
#define IMXOTYPE_LONG   iMXTYPE_LONG

#define IMXOP_MPY       iMXOP_MPY
#define IMXOP_ABDF      iMXOP_ABSDIFF
#define IMXOP_ADD       iMXOP_ADD
#define IMXOP_SUB       iMXOP_SUB
#define IMXOP_TLU       iMXOP_TLU
#define IMXOP_MIN       iMXOP_MIN
#define IMXOP_MAX       iMXOP_MAX
#define IMXOP_OR        iMXOP_OR
#define IMXOP_AND       iMXOP_AND
#define IMXOP_XOR        iMXOP_XOR
#define IMXOP_NOT        iMXOP_NOT

/*
 * Structure used to record last saturation parameters passed to imxenc_set_saturation().
 * Each time imxenc_set_saturation() is called, the values passed as input parameters
 * are saved in this structure
 *
*/

typedef struct IMX_SatParams{
    short sat_unsigned;
    short sat_high;   
    short sat_high_set;
    short sat_low;
    short sat_low_set;   
} IMX_SatParamsStruct;

/*----- System Functions -----*/

extern short imxenc_set_parameters(
  short a_sat_high,                    /* saturation upper bound compare value */
  short a_sat_high_set,                /* value set if >= sat_high             */
  short a_sat_low,                     /* saturation lower bound compare value */
  short a_sat_low_set,                 /* value set if < sat_low               */
  short *cmdptr);

short imxenc_set_saturation( 
  int sat_high,        /* 32bits */
  int sat_high_set,    /* 32bits */
  int sat_low,         /* 32bits */
  int sat_low_set,     /* 32bits */
  short *cmd_ptr );

short imxenc_save_sat_parameters(IMX_SatParamsStruct *imxsatparams);

short imxenc_restore_sat_parameters(IMX_SatParamsStruct *imxsatparams, short *cmd_ptr);

extern short imxenc_set_parameters_sign(
  short sat_unsigned,                  /*  1bit  */
  short sat_high,                      /* 16bits */
  short sat_high_set,                  /* 16bits */
  short sat_low,                       /* 16bits */
  short sat_low_set,                   /* 16bits */
  short *cmd_ptr);

extern short imxenc_sleep(
  short *cmdptr);

extern Int16 imxenc_jump(
  Int16 * cmdaddr,
  Int16 * cmd_p);

extern Int16 imxenc_return(
  Int16 * cmd_p);

extern short imxenc_call(
  short *cmdaddr,
  short *cmd_p);

extern short imxenc_call_dataaddr(
  short *cmdaddr,
  short *daddr,
  short *caddr,
  short *oaddr,
  short daddr_highbyte,
  short caddr_highbyte,
  short oaddr_highbyte,
  short *cmd_p);

extern short imxenc_cmdwrite(
  short *cmdaddr,
  short *startaddr,
  short number,
  short *cmd_p);

extern short imxenc_nop(
  short *cmd_p);

/*----- General Matrix/Array Functions -----*/

/* ==================================================================== */
/* Array Operations - add, subtract, multiply, absdiff, etc             */
/* ==================================================================== */

extern short imxenc_array_op(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output    */
  short input1_width,                  /* width/columns of 1st input    */
  short input1_height,                 /* height/rows of 1st input      */
  short input2_width,                  /* width/columns of 2nd input    */
  short input2_height,                 /* height/rows of 2nd input      */
  short output_width,                  /* width/columns of output       */
  short output_height,                 /* height/rows of output         */
  short compute_width,                 /* computation width             */
  short compute_height,                /* computation height            */
  short input1_type,                   /* short/byte signed/unsigned    */
  short input2_type,                   /* short/byte signed/unsigned    */
  short output_type,                   /* short/byte                    */
  short round_shift,                   /* Shifting parameter            */
  short operation,                     /* Chip Dependent                */
  short *cmdptr);

/* ==================================================================== */
/* Array Multiply and accumulate             */
/* ==================================================================== */

extern short imxenc_array_mac(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output    */
  short input1_width,                  /* width/columns of 1st input    */
  short input1_height,                 /* height/rows of 1st input      */
  short input2_width,                  /* width/columns of 2nd input    */
  short input2_height,                 /* height/rows of 2nd input      */
  short output_width,                  /* width/columns of output       */
  short output_height,                 /* height/rows of output         */
  short compute_width,                 /* computation width             */
  short compute_height,                /* computation height            */
  short input1_type,                   /* short/byte signed/unsigned    */
  short input2_type,                   /* short/byte signed/unsigned    */
  short output_type,                   /* short/byte                    */
  short round_shift,                   /* Shifting parameter            */
  short *cmdptr);

/* ==================================================================== */
/* Array-Scalar Operation, add/subtract/multiply/absdiff array and scalar */
/* ==================================================================== */
extern short imxenc_array_scalar_op_TB(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output    */
  short input1_width,                  /* width/columns of 1st input    */
  short input1_height,                 /* height/rows of 1st input      */
  short input2_width,                  /* width of 2nd input, 1 or 2    */
  short input2_height,                 /* height of 2nd input, 1 or 2   */
  short output_width,                  /* width/columns of output       */
  short output_height,                 /* height/rows of output         */
  short compute_width,                 /* computation width             */
  short compute_height,                /* computation height            */
  short input1_type,                   /* short/byte signed/unsigned    */
  short input2_type,                   /* short/byte signed/unsigned    */
  short output_type,                   /* short/byte                    */
  short round_shift,                   /* Shifting parameter            */
  short operation,                     /* Chip-dependent                */
  short *cmdptr);

extern short imxenc_array_upsample(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output    */
  short input1_width,                  /* width/columns of 1st input    */
  short input1_height,                 /* height/rows of 1st input      */
  short input2_width,                  /* width of 2nd input, 1 or 2    */
  short input2_height,                 /* height of 2nd input, 1 or 2   */
  short output_width,                  /* width/columns of output       */
  short output_height,                 /* height/rows of output         */
  short compute_width,                 /* computation width             */
  short compute_height,                /* computation height            */
  short input1_type,                   /* short/byte signed/unsigned    */
  short input2_type,                   /* short/byte signed/unsigned    */
  short output_type,                   /* short/byte                    */
  short round_shift,                   /* Shifting parameter            */
  short operation,                     /* Chip-dependent                */
  short *cmdptr);

extern Int16 imxenc_extractLuma(
  short * input1_ptr,
  short * input2_ptr,
  short * output_ptr,
  short input1_width,
  short input1_height,
  short input2_width,
  short input2_height,
  short output_width,
  short output_height,
  short compute_width,
  short compute_height,
  short input1_type,
  short input2_type,
  short output_type,
  short round_shift,
  short color_space,
  short *cmdptr
);

/* ==================================================================== */
/* Array-Scalar Operation, add/subtract/multiply/absdiff array and scalar */
/* ==================================================================== */
extern short imxenc_array_scalar_op(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output    */
  short input1_width,                  /* width/columns of 1st input    */
  short input1_height,                 /* height/rows of 1st input      */
  short input2_width,                  /* width of 2nd input, 1 or 2    */
  short input2_height,                 /* height of 2nd input, 1 or 2   */
  short output_width,                  /* width/columns of output       */
  short output_height,                 /* height/rows of output         */
  short compute_width,                 /* computation width             */
  short compute_height,                /* computation height            */
  short input1_type,                   /* short/byte signed/unsigned    */
  short input2_type,                   /* short/byte signed/unsigned    */
  short output_type,                   /* short/byte                    */
  short round_shift,                   /* Shifting parameter            */
  short operation,                     /* Chip-dependent                */
  short *cmdptr);

/* ==================================================================== */
/* Array-Scalar Operation, add/subtract/multiply/absdiff array and scalar */
/* ==================================================================== */
extern short imxenc_array_scalar_op_byte(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output */
  short input1_width,                  /* width/columns of 1st input */
  short input1_height,                 /* height/rows of 1st input */
  short input2_width,                  /* width of 2nd input, 1 or 2 */
  short input2_height,                 /* height of 2nd input, 1 or 2 */
  short output_width,                  /* width/columns of output */
  short output_height,                 /* height/rows of output */
  short compute_width,                 /* computation width */
  short compute_height,                /* computation height */
  short input1_type,                   /* short/byte signed/unsigned */
  short input2_type,                   /* short/byte signed/unsigned */
  short output_type,                   /* short/byte */
  short round_shift,                   /* Shifting parameter */
  short msb_input,                     /* input msb */
  short msb_coeff,                     /* coefficient msb */
  short msb_output,                    /* output msb */
  short operation,                     /* Chip-dependent                */
  short *cmdptr);

extern Int16 imxenc_scalar_array_op(
        Int16 * input1_ptr,                  /* starting address of 1st input */
        Int16 * input2_ptr,                  /* starting address of 2nd input */
        Int16 * output_ptr,                  /* starting address of output */
        Int16 input1_width,                  /* width/columns of 1st input, 1 or 2 */
        Int16 input1_height,                 /* height/rows of 1st input, 1 or 2 */
        Int16 input2_width,                  /* width of 2nd input */
        Int16 input2_height,                 /* height of 2nd input */
        Int16 output_width,                  /* width/columns of output */
        Int16 output_height,                 /* height/rows of output */
        Int16 compute_width,                 /* computation width */
        Int16 compute_height,                /* computation height */
        Int16 input1_type,                   /* short/byte signed/unsigned */
        Int16 input2_type,                   /* short/byte signed/unsigned */
        Int16 output_type,                   /* short/byte */
        Int16 round_shift,                   /* Shifting parameter */
        Int16 operation,                     /* (0) -> multiply
         * (1) -> absdiff
         * (2) -> add
         * (3) -> subtract
         */
        Int16 * cmdptr
);

/* ================================================ */
/*	Matrix multiplication			    */
/* ================================================ */

extern short imxenc_mat_mul(
  short *input1_ptr,                   /* starting address of 1st input   */
  short *input2_ptr,                   /* starting address of 2nd input   */
  short *output_ptr,                   /* starting address of output      */
  short input1_width,                  /* width/columns of 1st input      */
  short input1_height,                 /* height/rows of 1st input        */
  short input2_width,                  /* width/columns of 2nd input      */
  short input2_height,                 /* height/rows of 2nd input        */
  short output_width,                  /* width/columns of output         */
  short output_height,                 /* height/rows of output           */
  short mat1_width,                    /* computation width of 1st input  */
  short mat1_height,                   /* computation height of 1st input */
  short mat2_width,                    /* computation width of 2nd input  */
  short mat2_height,                   /* computation height of 2nd input */
  short input1_type,                   /* short/byte signed/unsigned      */
  short input2_type,                   /* short/byte signed/unsigned      */
  short output_type,                   /* short/byte                      */
  short round_shift,                   /* shifting parameter              */
  short *cmdptr);


extern short imxenc_rotate(
  short *input_ptr,                    /* starting address of input                              */
  short *coeff_ptr,                    /* pointer to single scalar used for scaling              */
  short *output_ptr,                   /* starting address of output                             */
  short input_width,                   /* width of the input image                               */
  short input_height,                  /* height of the input image                              */
  short output_width,                  /* width of the output image                              */
  short output_height,                 /* height of the output image                             */
  short compute_width,                 /* computed width in output image                         */
  short compute_height,                /* computed height in output image                        */
  short input_type,                    /* short/byte signed/unsigned                             */
  short coeff_type,                    /* short/byte signed/unsigned                             */
  short output_type,                   /* short/byte                                             */
  short angle,                         /* angle of rotation: only 90/180/270 degree is supported */
  short round_shift,                   /* shifting parameter                                     */
  short *cmdptr);


extern short imxenc_shift(
  short *input_ptr,                    /* starting address of input  */
  short *coeff_ptr,                    /* 2 coefficients             */
  short *output_ptr,                   /* starting address of output */
  short input_width,                   /* width of the input array   */
  short input_height,                  /* height of the input array  */
  short shift,                         /* round shift                */
  short *cmdptr);

/* ==================================================================== */
/* Array Summation, sum up element of 1-D/2-D array, with scaling       */
/* ==================================================================== */

extern short imxenc_sum(
  short *input_ptr,                    /* starting address of array input        */
  short *scaler_ptr,                   /* starting address of scaler, normally 1 */
  short *output_ptr,                   /* starting address of output             */
  short input_width,                   /* width/columns of array input           */
  short input_height,                  /* height/rows of array input             */
  short compute_width,                 /* computation width                      */
  short compute_height,                /* computation height                     */
  short input_type,                    /* short/byte signed/unsigned             */
  short scaler_type,                   /* short/byte signed/unsigned             */
  short output_type,                   /* short/byte                             */
  short round_shift,                   /* Shifting parameter                     */
  short sum_mode,                      /* chip dependent                         */
  short *cmdptr);

/* ==================================================================== */
/*    Array Summation with absolute difference      			*/
/* ==================================================================== */

extern short imxenc_sum_abs_diff(
  short *target_ptr,                   /* point to target array                     */
  short *ref_ptr,                      /* point to reference array                  */
  short *output_ptr,                   /* point to output array                     */
  short *interim_ptr,                  /* point to intermediate result array        */
  short *zero_ptr,                     /* point to a short zero word                */
  short block_width,                   /* width of matching block                   */
  short block_height,                  /* height of matching block                  */
  short target_width,                  /* width of target array                     */
  short target_height,                 /* height of target array                    */
  short ref_width,                     /* width of refernece array                  */
  short ref_height,                    /* height of refernece array                 */
  short step_horz,                     /* horizontal offset between matchings       */
  short step_vert,                     /* vertical offset between matchings         */
  short nsteps_horz,                   /* number of steps horizontally              */
  short nsteps_vert,                   /* number of steps vertically                */
  short target_type,                   /* short/byte, signed/unsigned               */
  short ref_type,                      /* short/byte, signed/unsigned               */
  short output_type,                   /* short/byte                                */
  short round_shift,                   /* number of bits to downshift before output */
  short *cmdptr);

/* =================================================== */
/*	       matrix transpose                        */
/*   transposition is not that efficient on iMX but    */
/*   is needed to implement certain algorithms         */
/* =================================================== */

extern short imxenc_transpose(
  short *input_ptr,                    /* starting address of input                 */
  short *coeff_ptr,                    /* pointer to single scalar used for scaling */
  short *output_ptr,                   /* starting address of output                */
  short input_width,                   /* width of the input image                  */
  short input_height,                  /* height of the input image                 */
  short output_width,                  /* width of the output image                 */
  short output_height,                 /* height of the output image                */
  short compute_width,                 /* computed width in output image            */
  short compute_height,                /* computed height in output image           */
  short input_type,                    /* short/byte signed/unsigned                */
  short coeff_type,                    /* short/byte signed/unsigned                */
  short output_type,                   /* short/byte                                */
  short round_shift,                   /* shifting parameter                        */
  short *cmdptr);


/*----- CFA Functions -----*/

extern short imx_cfa_setup_coeff(
  short phase,                         /* phase, depending on upper-left corner of input */
  /* phase = 0: green color, on green/blue line     */
  /* phase = 1: blue color                          */
  /* phase = 2: red color                           */
  /* phase = 3: green color, on red/green line      */
  short coeff_width,                   /* width of filter kernel                         */
  short coeff_height,                  /* height of filter kernel                        */
  short *coeff_g_ptr,                  /* ptr to start of coefficient array for green    */
  short *coeff_rb_ptr,                 /* ptr to start of coefficient array for red/blue */
  short *coeff_cfa_ptr);               /* ptr to IMX Coeff buffer for coefficient output */


extern void imx_filter_cfa_ib_setup(
  short *coeff_gr,                     /* Coefficient array for Gr Pixel     */
  short *coeff_gb,                     /* Coefficient array for Gb Pixel     */
  short *coeff_r,                      /* Coefficient array for R Pixel      */
  short *coeff_b,                      /* Coefficient array for B Pixel      */
  short *coeff,                        /* Coefficient output pointer         */
  short coeff_cols,                    /* Width of coefficient array         */
  short coeff_rows,                    /* Height of coefficient array        */
  short phase);                        /* Color phase of input buffer        */

extern void imx_filter_cfa_sb_setup_g(
  short *coeff_mat,                    /* Pointer to 2D Coefficient array    */
  short *coeff,                        /* Coefficient output pointer         */
  short coeff_width,                   /* Width of coefficient array         */
  short coeff_height,                  /* Height of coefficient array        */
  short phase);                        /* Color phase of input buffer        */

extern void imx_filter_cfa_sb_setup_rb(
  short *coeff_mat,                    /* Pointer to 2D Coefficient array    */
  short *coeff,                        /* Coefficient output pointer         */
  short coeff_width,                   /* Width of coefficient array         */
  short coeff_height,                  /* Height of coefficient array        */
  short rb_flag,                       /* Red or blue filter?   0 = R, 1 = B */
  short phase);                        /* Color phase of input buffer        */


extern short imxenc_cfa_interpolation(
  short *input_ptr,                    /* starting address of input              */
  short *coeff_ptr,                    /* starting address of coefficients g/r/b */
  short *output_r_ptr,                 /* starting address of r output           */
  short *output_g_ptr,                 /* starting address of g output           */
  short *output_b_ptr,                 /* starting address of b output           */
  short input_width,                   /* width/columns of input                 */
  short input_height,                  /* height/rows of input                   */
  short coeff_width,                   /* width/columns of coefficients          */
  short coeff_height,                  /* height/rows of coefficients            */
  short output_width,                  /* width/columns of output                */
  short output_height,                 /* height/rows of output                  */
  short compute_width,                 /* computation width                      */
  short compute_height,                /* computation height                     */
  short phase,                         /* upper-left corner of input: g/b/r/g    */
  short round_shift,                   /* shifting parameter                     */
  short *cmdptr);



/* =======================START LBCE ALGOS APIS====================================== */
/*	APIS for LBCE ALGO USE	 */
/*                              */
extern Int16 imxenc_computeColFilterMask(
    Int16  *input1_ptr,		/* starting address of 1st input */
    Int16  *input2_ptr,		/* starting address of 2nd input */
    Int16  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input2_width,		/* width/columns of 2nd input */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
	Int16 filter_size,
    Int16 input1_type,		/* short/byte signed/unsigned */
    Int16 input2_type, 		/* short/byte signed/unsigned */
    Int16 output_type,		/* short/byte */
    Int16 *cmdptr);

extern Int16 imxenc_deinterleaveRGB0(
    Int16  *input1_ptr,		/* starting address of 1st input */
    Int16  *scale_ptr,		/* starting address of 2nd input */
    Int16  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input1_height,	/* height/rows of 1st input */
    Int16 output_width,		/* width/columns of output */
    Int16 output_height,	/* height/rows of output */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
    Int16 input1_type,		/* short/byte signed/unsigned */
    Int16 scale_type, 		/* short/byte signed/unsigned */
    Int16 output_type,		/* short/byte */
    Int16 round_shift,		/* Shifting parameter */
    Int16 phase,          /* Phase = 0:  Select even points.
                             Phase = 1:  Select odd points. */
    Int16 *cmdptr);

extern Int16 imxenc_interleave_arr(
     Int16 * input1_ptr,                  /* starting address of 1st input */
     Int16 * input2_ptr,                  /* starting address of 2nd input */
     Int16 * output_ptr,                  /* starting address of output */
     Int16 input1_width,                  /* width/columns of 1st input */
     Int16 output_width,                  /* width/columns of output */
     Int16 compute_width,                 /* computation width */
     Int16 compute_height,                /* computation height */
     Int16 input1_type,                   /* short/byte signed/unsigned */
     Int16 input2_type,                   /* short/byte signed/unsigned */
     Int16 output_type,                   /* short/byte */
     Int16 * cmdptr);

extern Int16 imxenc_mulAccGwithRowFilterMask(
    Int16  *input1_ptr,		/* starting address of 1st input */
    Int16  *input2_ptr,		/* starting address of 2nd input */
    Int16  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input2_width,		/* width/columns of 2nd input */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
	Int16 filter_size,		/* Size of filter*/
    Int16 input1_type,		/* short/byte signed/unsigned */
    Int16 input2_type, 		/* short/byte signed/unsigned */
    Int16 output_type,		/* short/byte */
	Int16 shift, 
    Int16 *cmdptr);

extern Int16 imxenc_mulAccLrowWithFilterMask(
    Int16  *input1_ptr,		/* starting address of 1st input */
    Int16  *input2_ptr,		/* starting address of 2nd input */
    Int16  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input2_width,		/* width/columns of 2nd input */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
	Int16 filter_size,		/* Size of filter*/
    Int16 input1_type,		/* short/byte signed/unsigned */
    Int16 input2_type, 		/* short/byte signed/unsigned */
    Int16 output_type,		/* short/byte */
	Int16 shift, 
    Int16 *cmdptr);

extern Int16 imxenc_computeRowFilterMask(
    Int16  *input1_ptr,		/* starting address of 1st input */
    Int16  *input2_ptr,		/* starting address of 2nd input */
    Int16  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input2_width,		/* width/columns of 2nd input */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
	Int16 filter_size,
    Int16 input1_type,		/* short/byte signed/unsigned */
    Int16 input2_type, 		/* short/byte signed/unsigned */
    Int16 output_type,		/* short/byte */
    Int16 *cmdptr);


Int16 imxenc_computeSumOfFilterMask(
    Int16  *input1_ptr,		/* starting address of 1st input */
    Int16  *input2_ptr,		/* starting address of 2nd input */
    Int16  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input2_width,		/* width/columns of 2nd input */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
	Int16 filter_size,		/* Size of filter*/
    Int16 input1_type,		/* short/byte signed/unsigned */
    Int16 input2_type, 		/* short/byte signed/unsigned */
    Int16 output_type,		/* short/byte */
    Int16 *cmdptr);






/* ===================END LBCE ALGO IMX APIS========================================== */




/* ============================================================= */
/*	Filtering - 2-D FIR, 1-D row and column filtering 	 */
/*              Uses inter-band filters                  */
/*    ENCODE COMMAND SEQUENCE ONLY                       */
/*                                                       */
/*  Assume following structure for filter coefficients   */
/*     cfa_p0(0, 0)                                      */
/*     cfa_p1(0, 0)                                      */
/*     cfa_p0(0, 1)                                      */
/*     cfa_p1(0, 1)                                      */
/*     cfa_p0(0, 2)                                      */
/*       .                                               */
/*       .                                               */
/*       .                                               */
/*     cfa_p0(0, coeff_cols-1)                           */
/*     cfa_p1(0, coeff_cols-1)                           */
/*     cfa_p0(1, 0)                                      */
/*     cfa_p1(1, 0)                                      */
/*       .                                               */
/*       .                                               */
/*       .                                               */
/*     cfa_p0(coeff_rows-1, coeff_cols-1)                */
/*     cfa_p1(coeff_rows-1, coeff_cols-1)                */
/*     cfa_p2(0, 0)                                      */
/*     cfa_p3(0, 0)                                      */
/*       .                                               */
/*       .                                               */
/*       .                                               */
/*     cfa_p2(coeff_rows-1, coeff_cols-1)                */
/*     cfa_p3(coeff_rows-1, coeff_cols-1)                */
/*                                                       */
/*  Where CFA pattern is:                                */
/*     p0 p1                                             */
/*     p2 p3                                             */
/*                                                       */
/*  Call imx_filter_cfa_ib_setup() before to setup       */
/*  filter coefficients                                  */
/* ============================================================= */

extern short imxenc_filter_cfa_ib(
  short *input_ptr,                    /* starting address of input               */
  short *coeff_ptr,                    /* starting address of coefficients        */
  short *output_ptr,                   /* starting address of output              */
  short input_width,                   /* width/columns of input                  */
  short input_height,                  /* height/rows of input                    */
  short coeff_width,                   /* width/columns of output                 */
  short coeff_height,                  /* height/rows of output                   */
  short output_width,                  /* width/columns of coefficients           */
  short output_height,                 /* height/rows of coefficients             */
  short compute_width,                 /* computation dimension/number of columns */
  short compute_height,                /* computation dimension/number of rows    */
  short input_type,                    /* short/byte, signed/unsigned             */
  short coeff_type,                    /* short/byte, signed/unsigned             */
  short output_type,                   /* short/byte                              */
  short rnd_shift,                     /* shifting parameter                      */
  short *cmdptr);


extern short imxenc_ib_cfa_interpolation(
  short *input_p,                      /* Starting address of input              */
  short *coeff_p,                      /* Starting address of coefficient array  */
  short *outputr_p,                    /* Starting address of red output         */
  short *outputg_p,                    /* Starting address of green output       */
  short *outputb_p,                    /* Starting address of blue output        */
  short input_width,                   /* Width of input buffer                  */
  short input_height,                  /* Height of input buffer                 */
  short coeff_width,                   /* Width of coefficient array             */
  short coeff_height,                  /* Height of coefficient array            */
  short output_width,                  /* Width of output buffers                */
  short output_height,                 /* Height of output buffers               */
  short compute_width,                 /* Computation width                      */
  short compute_height,                /* Computation height                     */
  short input_type,                    /* Short/Byte, Signed/Unsigned            */
  short coeff_type,                    /* Short/Byte, Signed/Unsigned            */
  short output_type,                   /* Short/Byte, Signed/Unsigned            */
  short rnd_shift,                     /* Shift parameter                        */
  short *cmdptr);


extern short imxenc_sum_cfa(
  short *input_ptr,                    /* starting address of array input        */
  short *scaler_ptr,                   /* starting address of scaler, normally 1 */
  short *output_ptr,                   /* starting address of output             */
  short input_width,                   /* width/columns of array input           */
  short input_height,                  /* height/rows of array input             */
  short compute_width,                 /* computation width                      */
  short compute_height,                /* computation height                     */
  short input_type,                    /* short/byte signed/unsigned             */
  short scaler_type,                   /* short/byte signed/unsigned             */
  short output_type,                   /* short/byte                             */
  short round_shift,                   /* Shifting parameter                     */
  short *cmdptr);


/*----- Block Transform Functions -----*/

/* ================================================ */
/*	1-D Column DCT on 8x8 blocks of data        */
/*      with 8x8 coefficient matrix                 */
/* ================================================ */

extern short imxenc_dct8x8col(
  short *input_ptr,                    /* starting address of input        */
  short *coeff_ptr,                    /* starting address of coefficients */
  short *output_ptr,                   /* starting address of output       */
  short input_width,                   /* width of the input image         */
  short input_height,                  /* height of the input image        */
  short output_width,                  /* width of the output image        */
  short output_height,                 /* height of the output image       */
  short calc_Hblks,                    /* number of horizontal blocks      */
  short calc_Vblks,                    /* number of vertical blocks        */
  short input_type,                    /* short/byte signed/unsigned       */
  short coeff_type,                    /* short/byte signed/unsigned       */
  short output_type,                   /* short/byte                       */
  short round_shift,                   /* shifting parameter               */
  short *cmdptr);


/* ================================================ */
/*	1-D Row DCT on 8x8 blcoks of data           */
/*      with 8x8 coefficient matrix                 */
/* ================================================ */
extern short imxenc_dct8x8row(
  short *input_ptr,                    /* starting address of input        */
  short *coeff_ptr,                    /* starting address of coefficients */
  short *output_ptr,                   /* starting address of output       */
  short input_width,                   /* width of the input image         */
  short input_height,                  /* height of the input image        */
  short output_width,                  /* width of the output image        */
  short output_height,                 /* height of the output image       */
  short calc_Hblks,                    /* number of horizontal blocks      */
  short calc_Vblks,                    /* number of vertical blocks        */
  short input_type,                    /* short/byte signed/unsigned       */
  short coeff_type,                    /* short/byte signed/unsigned       */
  short output_type,                   /* short/byte                       */
  short round_shift,                   /* shifting parameter               */
  short *cmdptr);

/* =================================================== */
/*	1-D columnwise linear (block) Transform of data*/
/*      with MxN coefficient matrix                    */
/* --------------------------------------------------- */
/*   output = coeff * data                             */
/* =================================================== */

extern short imxenc_trafo_blk_col(
  short *input_ptr,                    /* starting address of input             */
  short *coeff_ptr,                    /* starting address of coefficients      */
  short *output_ptr,                   /* starting address of output            */
  short input_width,                   /* width of the input image              */
  short input_height,                  /* height of the input image             */
  short coeff_width,                   /* width of the transform coefficients   */
  short coeff_height,                  /* height of the transform coefficients  */
  short output_width,                  /* width of the output image             */
  short output_height,                 /* height of the output image            */
  short compute_width,                 /* computation width           */
  short calc_Vblks,                    /* number of vertical blocks             */
  short input_type,                    /* short/byte signed/unsigned            */
  short coeff_type,                    /* short/byte signed/unsigned            */
  short output_type,                   /* short/byte                            */
  short round_shift,                   /* shifting parameter                    */
  short coeff_arrang,                  /* 1 - coefficient are stored transposed */
  short *cmdptr);


/* =================================================== */
/*	1-D rowwise linear (block) Transform of data   */
/*      with MxN coefficient matrix A                  */
/*   	the horizontal dimension N must be a multiple  */
/*      of 4 => the argument coeff_with must be x4     */
/* --------------------------------------------------- */
/*   output = data * coeff'                            */
/* =================================================== */

extern short imxenc_trafo_blk_row(
  short *input_ptr,                    /* starting address of input            */
  short *coeff_ptr,                    /* starting address of coefficients     */
  short *output_ptr,                   /* starting address of output           */
  short input_width,                   /* width of the input image             */
  short input_height,                  /* height of the input image            */
  short coeff_width,                   /* width of the transform coefficients  */
  short coeff_height,                  /* height of the transform coefficients */
  short output_width,                  /* width of the output image            */
  short output_height,                 /* height of the output image           */
  short calc_Hblks,                    /* number of horizontal blocks          */
  short compute_height,                /* height of computed array             */
  short input_type,                    /* short/byte signed/unsigned           */
  short coeff_type,                    /* short/byte signed/unsigned           */
  short output_type,                   /* short/byte                           */
  short round_shift,                   /* shifting parameter                   */
  short *cmdptr);


/*----- Filtering Functions -----*/

/* ============================================================= */
/*	Filtering - 2-D FIR, 1-D row and column filtering 	 */
/* ============================================================= */

extern short imxenc_filter(
  short *input_ptr,                    /* starting address of input               */
  short *coeff_ptr,                    /* starting address of coefficients        */
  short *output_ptr,                   /* starting address of output              */
  short input_width,                   /* width/columns of input                  */
  short input_height,                  /* height/rows of input                    */
  short coeff_width,                   /* width/columns of coefficients           */
  short coeff_height,                  /* height/rows of coefficients             */
  short output_width,                  /* width/columns of output                 */
  short output_height,                 /* height/rows of output                   */
  short compute_width,                 /* computation dimension/number of columns */
  short compute_height,                /* computation dimension/number of rows    */
  short dnsmpl_horz,                   /* horizontal downsampling factor          */
  short dnsmpl_vert,                   /* verical downsampling factor             */
  short input_type,                    /* short/byte, signed/unsigned             */
  short coeff_type,                    /* short/byte, signed/unsigned             */
  short output_type,                   /* short/byte                              */
  short round_shift,                   /* shifting parameter                      */
  short *cmdptr);


/* =================================================== */
/*  function:     imx_fir_poly_setup_coeff             */
/*  description:  setup the coefficients for the       */
/*	              polyphase filter function        */
/*  input:        1D filter with TAPS coefficients     */
/* --------------------------------------------------- */
/*   PREPARATION FUNCTION                              */
/* =================================================== */

extern short imx_fir_poly_setup_coeff(
  short *src_p,                        /* address of 1D filter coefficients    */
  short *dst_p,                        /* destination address in iMXCMD memory */
  int taps,                            /* number of filter taps                */
  int smpl_nom,                        /* upsampling factor                    */
  int smpl_denom);                     /* downsampling factor                  */


/* ============================================================== */
/*	1D polyphase filtering along columns                   	  */
/* -------------------------------------------------------------- */
/* NOTE: The filter coefficients must be stored in a specific way */
/* input data, upsampling by 4     x...x...x...x...x...x...x...   */
/* filter - phase 1                1   5   9   0   0              */
/* filter - phase 2                0   4   8   0   0              */
/* filter - phase 3                0   3   7   0   0              */
/* filter - phase 4                0   2   6   0   0              */
/* filter - phase 5                0   1   5   9   0              */
/* filter - phase 12               0   0   0   2   6              */
/* for subsequent downsampling by 3, the phase count is 12        */
/* the assumed filter length is 9-tap, index by 1...9;
   0 means a 0 value */
/* CONSTRAINTS:                                   
   -) input height >= smpl_nom * (compute_height/smpl_denom -1) 
			+ smpl_denom + coeff_taps / smpl_nom
   -) compute_width must be multiple of 4 (for speed reasons)
   -) compute_height must be multiple of smpl_nom                 */
/* ============================================================== */

extern short imxenc_fir_poly_col(
  short *input_ptr,                    /* starting address of input               */
  short *coeff_ptr,                    /* starting address of coefficients        */
  short *output_ptr,                   /* starting address of output              */
  short input_width,                   /* width/columns of input                  */
  short input_height,                  /* height/rows of input                    */
  short coeff_taps,                    /* number of filter coefficients           */
  short output_width,                  /* width/columns of output                 */
  short output_height,                 /* height/rows of output                   */
  short compute_width,                 /* computation dimension/number of columns */
  short compute_height,                /* computation dimension/number of rows    */
  short smpl_nom,                      /* nominator for fractional vertical
                                        * sampling factor (upsampling)     */
  short smpl_denom,                    /* denominator for fractional vertical 
                                        * sampling factor (downsampling) */
  short input_type,                    /* short/byte, signed/unsigned             */
  short coeff_type,                    /* short/byte, signed/unsigned             */
  short output_type,                   /* short/byte                              */
  short round_shift,                   /* shifting parameter                      */
  short *cmdptr);


/*----- YCbCr Formatting Functions -----*/



/* ======================================================================== */
/* imxenc_YcbCrPack -Packing YCbCr color data stored in separate components */
/*		     into a single image with 16bit per color pixel	    */
/*                  (YcbCr 4:2:2 format)                                    */
/* ======================================================================== */
#ifdef _DM270
//static inline
#endif
short imxenc_YCbCrPack(
  short **input_ptr,                   /* starting address of input                   */
  short *coeff_ptr,                    /* pointer to single scalar used for scaling   */
  short *output_ptr,                   /* pointer to packed output image              */
  short input_width,                   /* width of the input image                    */
  short input_height,                  /* height of the input image                   */
  short output_width,                  /* width of the output image                   */
  short output_height,                 /* width of the output image                   */
  short calc_width,                    /* computed width in output image              */
  short calc_height,                   /* computed height in output image             */
  short colorspace,                    /* input YCbCr 4:4:4 - 0, 4:2:2 - 1, 4:2:0 - 2 */
  short round_shift,                   /* shifting parameter                          */
  short *cmdptr);



/*----- Table Lookup Functions -----*/
short imx_tlu_LDinterleaved(
  short *table_src[],                  /* address pointer to source location of tables  */
  short *table_base,                   /* target address for table setup in iMXCOEFFBUF */
  short nbrtbls,                       /* number of tables (2, 4)                       */
  short tblsize);                      /* size of tables (number of entries             */


extern short imxenc_tlu(
  short *data_ptr,                     /* point to input data array          */
  short *table_base,                   /* point to lookup table              */
  short *output_ptr,                   /* point to output array              */
  short input_width,                   /* width of input data array          */
  short input_height,                  /* height of input data array         */
  short table_size,                    /* number of entries in the table     */
  short no_lut_horiz,                  /* number of tables horizontally      */
  short no_lut_vert,                   /* number of tables vertically        */
  short output_width,                  /* width of output data array         */
  short output_height,                 /* height of output data array        */
  short compute_width,                 /* width of actual output             */
  short compute_height,                /* height of actual output            */
  short input_type,                    /* short/byte, signed/unsigned        */
  short table_type,                    /* short/byte, signed/unsigned        */
  short output_type,                   /* output written as short/byte       */
  short round_shift,                   /* rounding right shift before lookup */
  short *cmdptr);

extern short imxenc_tlu_cfa(
  short *data_ptr,                     /* point to input data array          */
  short *table_base,                   /* point to lookup table              */
  short *output_ptr,                   /* point to output array              */
  short input_width,                   /* width of input data array          */
  short input_height,                  /* height of input data array         */
  short table_size,                    /* number of entries in the table     */
  short cfaphase,                      /* phase of CFA Bayer pattern         */
  short output_width,                  /* width of output data array         */
  short output_height,                 /* height of output data array        */
  short compute_width,                 /* width of actual output             */
  short compute_height,                /* height of actual output            */
  short input_type,                    /* short/byte, signed/unsigned        */
  short table_type,                    /* short/byte, signed/unsigned        */
  short output_type,                   /* output written as short/byte       */
  short round_shift,                   /* rounding right shift before lookup */
  short *cmdptr);


/*----- Color Space Conversion -----*/

extern short imxenc_color_spc_conv(
  short *input_ptr,                    /* point to input data               */
  short *coeff_ptr,                    /* point to coef array               */
  short *output_ptr,                   /* point to output array             */
  short input_width,                   /* width of input                    */
  short input_height,                  /* height of input                   */
  short input_depth,                   /* number of input color planes      */
  short output_width,                  /* width of output                   */
  short output_height,                 /* height of output                  */
  short output_depth,                  /* number of output color planes     */
  short compute_width,                 /* num pixels processed horizontally */
  short compute_height,                /* num pixels processed vertically   */
  short input_step_color,              /* offset between input colors       */
  short input_step_row,                /* offset between input rows         */
  short output_step_color,             /* offset between outp colors        */
  short output_step_row,               /* offset between outp rows          */
  short input_type,                    /* short/byte signed/unsigned        */
  short coeff_type,                    /* short/byte signed/unsigned        */
  short output_type,                   /* short/byte                        */
  short round_shift,                   /* shifting parameter                */
  short *cmdptr);

/* Piece wise linear funciton */
short imxenc_pcWiseLinear(
  short *input,
  short *output,
  short input_width,
  short input_height,
  short output_width,
  short output_height,
  short compute_width,
  short compute_height,
  short nbPts,                         /* must be >1 */
  short *xpts,                         /* must be in ascending order */
  short *ypts,
  short Qshift,
  short input_type,
  short output_type,
  short *scratch1,                     /*can be in img buffer or iMX coeff,
                                        * should be output_width*output_height
                                        */
  short *scratch2,                     /* output_width*output_height+
                                        * 2*nbPts */
  short *cmdPtr);

short imxenc_sum_array_op(
  short *target_ptr,                   /* point to target array */
  short *ref_ptr,                      /* point to reference array */
  short *output_ptr,                   /* point to output array */
  short *interim_ptr,                  /* point to intermediate result array */
  short block_width,                   /* width of matching block */
  short block_height,                  /* height of matching block */
  short target_width,                  /* width of target array */
  short target_height,                 /* height of target array */
  short ref_width,                     /* width of refernece array */
  short ref_height,                    /* height of refernece array */
  short step_horz,                     /* horizontal offset between matchings */
  short step_vert,                     /* vertical offset between matchings */
  short nsteps_horz,                   /* number of steps horizontally */
  short nsteps_vert,                   /* number of steps vertically */
  short target_type,                   /* short/byte, signed/unsigned */
  short ref_type,                      /* short/byte, signed/unsigned */
  short output_type,                   /* short/byte */
  short round_shift,                   /* number of bits to downshift before output */
  short data_inter,                    /* 1 --> yes, 0 --> no                       */
  short coef_inter,
  short out_inter,
  short data_msb,
  short coef_msb,
  short out_msb,
  short operation,
  short *cmdptr);




short imxenc_array_minmax(
  short *input_ptr,                    /* point to target array */
  short *output_ptr,                   /* point to output array */
  short input_width,                   /* width of matching block */
  short input_height,                  /* height of matching block */
  short block_width,                   /* width of matching block */
  short block_height,                  /* height of matching block */
  short minmax_clear,                  /* 0: retain, 1: clear */
  short minmax,                        /* 0: min, 1: max  */
  short id_mode,                       /* 0: block count, 1: address */
  short id_value,                      /* 0: ID, 1: min/max value */
  short input_type,
  short *cmdptr);

Int16 imxenc_minmax_row(
  Int16 * input_ptr,
  Int16 * output_ptr,
  Int16 input_width,
  Int16 input_height,
  Int16 output_width,
  Int16 output_height,
  Int16 block_width,
  Int16 block_height,
  Int16 mask_width,
  Int16 minmax_clear,
  Int16 minmax,
  Int16 id_mode,
  Int16 id_value,
  Int16 input_type,
  Int16 * cmdptr );

Int16 imxenc_minmax_col(
  Int16 * input_ptr,
  Int16 * output_ptr,
  Int16 input_width,
  Int16 input_height,
  Int16 output_width,
  Int16 output_height,
  Int16 block_width,
  Int16 block_height,
  Int16 mask_width,
  Int16 minmax_clear,
  Int16 minmax,
  Int16 id_mode,
  Int16 id_value,
  Int16 input_type,
  Int16 * cmdptr );

short imxenc_array_point_op(
  short *input1_ptr,                   /* starting address of 1st input */
  short *input2_ptr,                   /* starting address of 2nd input */
  short *output_ptr,                   /* starting address of output */
  short input1_width,                  /* width/columns of 1st input */
  short input2_width,                  /* width/columns of 2nd input */
  short output_width,                  /* width/columns of output */
  short output_height,                 /* height/rows of output */
  short compute_width,                 /* computation width */
  short compute_height,                /* computation height */
  short input1_type,                   /* short/byte signed/unsigned */
  short input2_type,                   /* short/byte signed/unsigned */
  short output_type,                   /* short/byte */
  short round_shift,                   /* Shifting parameter */
  short rnd_type,
  short operation,                     /* (0) -> multiply
                                        * (1) -> absdiff
                                        * (2) -> add
                                        * (3) -> subtract 
                                        */
  short Nh,
  short Nv,
  short coef_rotate,
  short data_inter,
  short coef_inter,
  short out_inter,
  short data_msb,
  short coef_msb,
  short out_msb,
  short num_macs,
  short *cmdptr);

Int16 imxenc_Update_InputPtr(
  Int16 ** input_ptr,                  /* points to starting byte offset of input */
  Int16 * cmdptr_update,               /* update command */
  Int16 * cmdptr_API);




short imxPreUpdate_DataPtr(
  short *data_ptr,
  short highbyte_data);


void imxUpdate_InputPtr(
  short *data_ptr,
  short highbyte_data,
  short *cmd_ptr);


void imxUpdate_CoeffPtr(
  short *coeff_ptr,
  short highbyte_coeff,
  short *cmd_ptr);


void imxUpdate_OutPtr(
  short *out_ptr,
  short highbyte_out,
  short *cmd_ptr);

void imxUpdate_Params_array_scalar_op(
  short input1_width,                  /* width/columns of 1st input */
  short input1_height,                 /* height/rows of 1st input */
  short input2_width,                  /* width of 2nd input, 1 or 2 */
  short input2_height,                 /* height of 2nd input, 1 or 2 */
  short output_width,                  /* width/columns of output */
  short output_height,                 /* height/rows of output */
  short compute_width,                 /* computation width */
  short compute_height,                /* computation height */
  short input1_type,
  short output_type,
  short *cmdptr);


void imxUpdate_out_sumarrayop(
  short *outp_ptr,
  short highbyte_out,
  short output_type,
  short *cmd_ptr);

void imxUpdate_Params_y2blkseq(
  short input_width,                   /* width of the input image */
  short input_height,                  /* height of the input image */
  short no_blks_x,                     /* handled no of blocks in x-direction */
  short no_blks_y,                     /* handled no of blocks in y-direction */
  short *cmdptr);

/* ==================================================================== */
/* Single-level Gaussian pyramid										*/
/* ==================================================================== */
extern short imxenc_gaussian_pyr(
  short *input_ptr,                    /* starting address of data */
  short *filter_ptr,                   /* starting address of filter */
  short *output_ptr,                   /* starting address of output */
  short input_width,                   /* width/columns of data */
  short input_height,                  /* height/rows of data */
  short filter_width,                  /* width of filter, usually 3 */
  short filter_height,                 /* height of filter, usually 3 */
  short output_width,                  /* width/columns of output */
  short output_height,                 /* height/rows of output */
  short compute_width,                 /* computation width */
  short compute_height,                /* computation height */
  short comp_row_offset,               /* row offset between computation block and input block */
  short comp_col_offset,               /* column offset between computation block and input block */
  short out_row_offset,                /* row offset between actual start of output and output_ptr */
  short out_col_offset,                /* column offset between actual start of output and output_ptr */
  short input_type,                    /* short/byte signed/unsigned */
  short filter_type,                   /* short/byte signed/unsigned */
  short output_type,                   /* short/byte */
  short round_shift,                   /* Shifting parameter */
  short *cmdptr                        /* location where generated opcodes are stored */
  );



short imxenc_mirror(
  short *pred_blk_ptr, 		/* starting address of input */
  short *coeff_ptr,		    /* starting address of coefficients */
  short *out_ptr ,           /* starting address of output */
  short ref_width ,  
  short compute_width ,
  short compute_height ,
  short out_width ,    
  short data_type ,       /* 1 - byte ;  2 - word */
  short data_msb ,        /* valid only if data_type =1 ; 1 - MSB ; 0 - LSB */  /* if data_type =2 keep fixed at 0 */
  short data_inter ,      /* valid only if data_type =1 ; 1 - data interleaved ; 0 - not interleaved *//* if data_type =2 keep fixed at 0 */
  short out_type ,        /* 1 - byte ;  2 - word */
  short out_msb ,        /* valid only if out_type =1 ; 1 - MSB ; 0 - LSB *//* if out_type =2 keep fixed at 0 */
  short out_inter ,      /* valid only if out_type =1 ; 1 - out data interleaved ; 0 - not interleaved *//* if out_type =2 keep fixed at 0 */
  short *cmdptr
);



//-------------------------------------------
// Macro for imxenc_pcWiseLinear
//-------------------------------------------
#define  imxencWriteMem(adr, data) ((*(adr))=data)

/* =================================================== */
/* function can NOT operate in-place */
/* =================================================== */
/* ==================================================================== */
/* imxenc_YcbCrUnpack Unpacking YcbCr color data store in a single image*/
/*		     with 16bit per color picel (YcbCr 4:2:2 format)   	*/
/*		     into separate components                          	*/
/* ==================================================================== */
short imxenc_YCbCrUnpack(
  short *input_ptr,                    /* starting address of input                   */
  short *coeff_ptr,                    /* pointer to single scalar used for scaling   */
  short **output_ptr,                  /* starting address of output                  */
  short input_width,                   /* width of the input image                    */
  short input_height,                  /* height of the input image                   */
  short output_width,                  /* width of the output image                   */
  short output_height,                 /* width of the output image                   */
  short calc_width,                    /* compute width (multiple of 2                */
  short calc_height,                   /* compute height                              */
  short colorspace,                    /* YCbCr 4:4:4 - 0, 4:2:2 - 1, 4:2:0 - 2       */
  short round_shift,                   /* shifting parameter                          */
                         short *cmdptr
                         );

/* ==================================================================== */
/* imxenc_y2blkseq - Reorganize a NxM 2D matrix into a (N*M/8) matrix 	*/
/*		     (or storing data in 8x8 blocksequentially)   	*/
/*		                                                   	*/
/* ==================================================================== */
short imxenc_y2blkseq(
  short *input_ptr,                    /* starting address of input                 */
  short *coeff_ptr,                    /* pointer to single scalar used for scaling */
  short *output_ptr,                   /* starting address of output                */
  short input_width,                   /* width of the input image                  */
  short input_height,                  /* height of the input image                 */
  short no_blks_x,                     /* handled no of blocks in x-direction       */
  short no_blks_y,                     /* handled no of blocks in y-direction       */
  short round_shift,                   /* shifting parameter                        */
                      short *cmdptr
                      );

/* ====================================================================     */
/* imxenc_blkSeq2Array - Reorganize data stored in a block sequential manner */
/*		      to a big  array    	                            */
/*		                                                   	    */
/* ====================================================================     */
short imxenc_blkSeq2Array(
  short *input_ptr,                    /* starting address of input                 */
  short *coeff_ptr,                    /* pointer to single scalar used for scaling */
  short *output_ptr,                   /* starting address of output                */
  short input_blksize,                 /* input size of the square block */
  short compute_blksize,               /* compute size of the square block */
  short output_width,                  /* width of the output image                 */
  short output_height,                 /* height of the output image                */
  short no_blks_x,                     /* handled no of blocks in x-direction       */
  short no_blks_y,                     /* handled no of blocks in y-direction       */
  short round_shift,                   /* shifting parameter                        */
                          short *cmdptr
                          );


/*===========================================================================*/
/**
*@brief  Reorganize a NxM 2D matrix into a (N*M/8) matrix
*
*		     (or storing data in 8x8 blocksequentially) 
*
*@param *input_ptr,
*
*
*@param *coeff_ptr
*
*
*@param *output_ptr
*
*
*@param output_width
*
*
*@param output_height
*
*
*@param no_blks_x
*
*
*@param no_blks_y
*
*
*@param round_shift
*
*
*@param *cmdptr
*
*
*@return number of words written to cmd memory starting at cmdptr
*
*@note   None
*/
/*===========================================================================*/
#define imxenc_blkseq2y(\
	input_ptr,\
	coeff_ptr,\
	output_ptr,\
	output_width,\
        output_height,\
        no_blks_x,\
        no_blks_y,\
	round_shift,\
	cmdptr)\
	imxenc_blkSeq2Array(\
	       		input_ptr,\
		        coeff_ptr,\
		        output_ptr,\
                        8,\
                        8,\
                        output_width,\
                        output_height,\
                        no_blks_x,\
                        no_blks_y,\
                        round_shift,\
                        cmdptr)

typedef struct arraydim {
  Int16 array_width;                   /* width/columns of 1st input */
  Int16 array_type;
  Int16 array_smpl_horz;
  Int16 array_smpl_vert;
  Int16 array_msb;
} ArrayDim;

typedef struct blockdim {
  Int16 compute_width;                 /* computation width */
  Int16 compute_height;                /* computation height */
  Int16 block_width;                   /* block width */
  Int16 block_height;                  /* block height */
} BlockDim;

typedef struct updnsmpl_point_op_params {
  Int16 *input1_ptr;                   /* starting address of 1st input */
  Int16 *input2_ptr;                   /* starting address of 2nd input */
  Int16 *output_ptr;                   /* starting address of output */
  ArrayDim InputDim;
  ArrayDim CoefDim;
  ArrayDim OutDim;
  BlockDim BlkDim;
  Int16 round_shift;                   /* Shifting parameter */
  Int16 rnd_type;
  Int16 operation;                     /* (0) -> multiply
                                        * (1) -> absdiff
                                        * (2) -> add
                                        * (3) -> subtract 
                                        */
  Int16 Nh;
  Int16 Nv;
  Int16 coef_scaler;
  Int16 input_order;                   /*   0: array order
                                        * 1: sequential order
                                        */
  Int16 coef_order;                    /*   0: array order
                                        * 1: single block
                                        */

  Int16 out_order;                     /*  0: array order
                                        * 1: sequential order
                                        */
  Int16 num_macs;
  Int16 *cmdptr;
} updnsmpl_point_op_Params;



Int16 imxenc_set_parameters_mul32(
  Int16 a_sat_high, 
  Int16 a_sat_high_set, 
  Int16 a_sat_low, 
  Int16 a_sat_low_set,
  Int16 *cmd_p);

/** 
 * Perform point to point multiplication of two 32-bits array
 * and produce an array of 32 or 16 bits data.
 * If output is set to IMXOTYPE_LONG then the application must call 
 * the API imxenc_set_parameters_sign32()
 * before calling imxenc_mult32array:
 * \param input_ptr Points to first 32-bits elements array
 * \param coef_ptr Points to second 32-bits elements array
 * \param output_ptr Points to output array
 * \param input1_width width of the first array
 * \param input2_width width of the second array
 * \param output_width width of the output array
 * \param computation_width computation width
 * \param computation_height computation height
 * \param output_type can be either IMXOTYPE_SHORT or IMXOTYPE_LONG
 * \param round_shift shifting parameter
 */   
 Int16 imxenc_mult32array(
    Int32  *input_ptr,		/* starting address of 1st input */
    Int32  *coef_ptr,		/* starting address of 2nd input */
    Int32  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 input2_width,		/* width/columns of 1st input */
    Int16 output_width,		/* width/columns of output */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
    Int16 output_type,		/* short/byte */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);
 
 /** 
 * Perform point to point multiplication of one 32-bits array with one 32-bit scalar
 * and produce an array of 32 or 16 bits data.
 * If output is set to IMXOTYPE_LONG then the application must call 
 * the API imxenc_set_parameters_sign32()
 * before calling imxenc_mult32scalar:
 * \param input_ptr Points to first 32-bits elements array
 * \param coef_ptr Points to second 32-bits elements array
 * \param output_ptr Points to output array
 * \param input1_width width of the first array
 * \param output_width width of the output array
 * \param computation_width computation width
 * \param computation_height computation height
 * \param output_type can be either IMXOTYPE_SHORT or IMXOTYPE_LONG
 * \param round_shift shifting parameter
 */   
 Int16 imxenc_mult32scalar(
    Int32  *input_ptr,		/* starting address of 1st input */
    Int32  *coef_ptr,		/* starting address of 2nd input */
    Int32  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 output_width,		/* width/columns of output */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
    Int16 output_type,		/* short/byte */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);
    
/** 
 * Perform transpose and point to point multiplication of one 32-bits array with one 32-bit scalar
 * and produce an array of 32 or 16 bits data.
 * If output is set to IMXOTYPE_LONG then the application must call 
 * the API imxenc_set_parameters_sign32()
 * before calling imxenc_transpose_32bit:
 * \param input_ptr Points to first 32-bits elements array
 * \param coef_ptr Points to second 32-bits elements array
 * \param output_ptr Points to output array
 * \param input1_width width of the first array
 * \param output_width width of the output array
 * \param computation_width computation width
 * \param computation_height computation height
 * \param output_type can be either IMXOTYPE_SHORT or IMXOTYPE_LONG
 * \param round_shift shifting parameter
 */  
Int16 imxenc_transpose_32bit(
    Int32  *input_ptr,		/* starting address of 1st input */
    Int32  *coef_ptr,		/* starting address of 2nd input */
    Int32  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 output_width,		/* width/columns of output */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
    Int16 output_type,		/* short/byte */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);
    
 
 /** 
 * Perform 2-D filtering, 1-D column and row filtering on 32 bits data. Output can be either 32 or 16 bits.
 * If output is set to IMXOTYPE_LONG then the application must call 
 * the API imxenc_set_parameters_sign32()
 * before calling imxenc_transpose_32bit:
 * \param input_ptr Points to first 32-bits elements array
 * \param coef_ptr Points to second 32-bits elements array
 * \param output_ptr Points to output array
 * \param input1_width width of the first array
 * \param coef_width width of filter array
 * \param coef_height height of filter array
 * \param output_width width of the output array
 * \param computation_width computation width
 * \param computation_height computation height
 * \param output_type can be either IMXOTYPE_SHORT or IMXOTYPE_LONG
 * \param round_shift shifting parameter
 */  
 Int16 imxenc_filter_32bit(
    Int32  *input_ptr,		/* starting address of 1st input */
    Int32  *coef_ptr,		/* starting address of 2nd input */
    Int32  *output_ptr,		/* starting address of output */
    Int16 input1_width,		/* width/columns of 1st input */
    Int16 coef_width,		/* width/columns of 1st input */
    Int16 coef_height,		/* width/columns of 1st input */
    Int16 output_width,		/* width/columns of output */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
    Int16 output_type,		/* short/byte */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);

/** 
 * Perform summation of entire matrix of 16-bit elements
 * and produce two 32-bits partial sums.
 * Application can apply imxenc_sum32bTo32b() on the partial sums
 * to obtain final sum.
 * Important ! The application must call the API imxenc_set_parameters_sign32()
 * before calling imxenc_sum16bTo32b():
 * cmdlen = imxenc_set_parameters_sign32
 * (
	*	1,			// sat_unsigned	1bit: 1 is unsigned; 0 is signed
	*	0xffff,		// sat_high		16bits
	*	0xffff,		// sat_high_set	16bits
	*	0,			// sat_low,		16bits
	*	0,			// sat_low_set	16bits
	*	cmdptr 
 * );
 * \param input_ptr Points to input 16-bits elements matrix
 * \param scaler_ptr Points to 16-bits scaler coefficient, usually 1
 * \param output_ptr Points to output location where two 32-bits elements  will be written
 * \param input_width width of the input matrix
 * \param computation_width computation width
 * \param computation_height computation height
 * \param round_shift shifting parameter
 */    
Int16 imxenc_sum16bTo32b
(
    Int16  *input_ptr,		/* starting address of array input */
    Int32  *scaler_ptr,		/* starting address of scaler, normally 0x00010001 */
    Int32  *output_ptr,		/* starting address of output */
    Int16 input_width,		/* width/columns of array input */
    Int16 computation_width,	/* computation width */ 
    Int16 computation_height,	/* computation height */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr
);

/** 
 * Perform summation of entire matrix of 32-bit elements
 * and produce one 32-bits sums.
 * \param input_ptr Points to input 32-bits elements matrix
 * \param coef_ptr Points to 32-bits scaler coefficient, usually 1
 * \param output_ptr Points to output location where two 32-bits elements  will be written
 * \param input_width width of the input matrix
 * \param output_width width of the output matrix
 * \param computation_width computation width
 * \param computation_height computation height
 * \param round_shift shifting parameter
 */    
Int16 imxenc_sum32bTo32b(
    Int32  *input_ptr,		/* starting address of 1st input */
    Int32  *coef_ptr,		/* starting address of 2nd input */
    Int32  *output_ptr,		/* starting address of output */
    Int16 input_width,		/* width/columns of 1st input */
    Int16 compute_width,	/* computation width */ 
    Int16 compute_height,	/* computation height */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);

/*--- 32-bit Scalar & Array addition APIs which are useful for integral image API ---*/

/* Performs addition of byte/short input with a 32-bit scalar to produce a 32-bit output */
Int16 imxenc_array_scalar_32_bit_op(
                             Int16 * input1_ptr,                  /* starting address of 1st input */
                             Int16 * input2_ptr,                  /* starting address of 2nd input */
                             Int16 * output_ptr,                  /* starting address of output */
                             Int16 input1_width,                  /* width/columns of 1st input */
                             Int16 input1_height,                 /* height/rows of 1st input */
                             Int16 input2_width,                  /* width of 2nd input, 1 or 2 */
                             Int16 input2_height,                 /* height of 2nd input, 1 or 2 */
                             Int16 output_width,                  /* width/columns of output */
                             Int16 output_height,                 /* height/rows of output */
                             Int16 compute_width,                 /* computation width */
                             Int16 compute_height,                /* computation height */
                             Int16 input1_type,                   /* short/byte signed/unsigned */
                             Int16 input2_type,                   /* short/byte signed/unsigned */
                             Int16 output_type,                   /* short/byte */
                             Int16 round_shift,                   /* Shifting parameter */
                             Int16 operation,                     /* (0) -> multiply 
                                                                   * (1) -> absdiff
                                                                   * (2) -> add
                                                                   * (3) -> subtract
                                                                   */
                             Int16 * cmdptr);

/* Performs column-wise addition in the vertical direction using filtering mechanism with a 32-bit scalar word */
Int16 imxenc_add_32bit_vert_dir(
							Int32  *input_ptr,		/* starting address of 1st input */
							Int32  *coef_ptr,		/* starting address of 2nd input */
							Int32  *output_ptr,		/* starting address of output */
							Int16 input1_width,		/* width/columns of 1st input */
							Int16 coef_width,		/* width/columns of 1st input */
							Int16 coef_height,		/* width/columns of 1st input */
							Int16 output_width,		/* width/columns of output */
							Int16 compute_width,	/* computation width */ 
							Int16 compute_height,	/* computation height */
							Int16 output_type,		/* short/byte */
							Int16 round_shift,		/* Shifting parameter */
							Int16 *cmdptr);

/* Performs row-wise addition in the horizontal direction using filtering mechanism with a 32-bit scalar word */
Int16 imxenc_add_32bit_horz_dir(
							Int32  *input_ptr,		/* starting address of 1st input */
							Int32  *coef_ptr,		/* starting address of 2nd input */
							Int32  *output_ptr,		/* starting address of output */
							Int16 input1_width,		/* width/columns of 1st input */
							Int16 coef_width,		/* width/columns of 1st input */
							Int16 coef_height,		/* width/columns of 1st input */
							Int16 output_width,		/* width/columns of output */
							Int16 compute_width,	/* computation width */ 
							Int16 compute_height,	/* computation height */
							Int16 output_type,		/* short/byte */
							Int16 round_shift,		/* Shifting parameter */
							Int16 *cmdptr);

/*----- Temporal Noise Filtering Functions -----*/

/* =========================================================================== */
/* imxenc_tnf_color_weighted_motion_chroma -Computes color weighted motion component (mc) of CbCr components  */
/* Output, mc = w1 * mu + w2 * mv                                                                                                                         */
/* w1, w2 are color weights, mu = |CurrFrmU - PrevFrmU|, mv = |CurrFrmV - PrevFrmV|                                          */
/* (YcbCr 4:2:0 format)                                                                                                                                            */
/* =========================================================================== */


extern Int16 imxenc_tnf_color_weighted_chroma_motion(
                      Int16 * input1_ptr,
                      Int16 * input2_ptr,
                      Int16 * output_ptr,
                      Int16 input1_width,
                      Int16 input1_height,
                      Int16 input2_width,
                      Int16 input2_height,
                      Int16 output_width,
                      Int16 output_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 operation,
                      Int16 * cmdptr);

Int16 imxenc_tnf_compute_mc(
                      Int16 * input1_ptr,
                      Int16 * input2_ptr,
                      Int16 * output_ptr,
                      Int16 input1_width,
                      Int16 input1_height,
                      Int16 input2_width,
                      Int16 input2_height,
                      Int16 output_width,
                      Int16 output_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 operation,
                      Int16 * cmdptr
                      );

Int16 imxenc_tnf_motion_calculate(
                      Int16 * input1_ptr,
                      Int16 * input2_ptr,
                      Int16 * output_ptr,
                      Int16 input1_width,
                      Int16 input1_height,
                      Int16 input2_width,
                      Int16 input2_height,
                      Int16 output_width,
                      Int16 output_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 operation,
                      Int16 * cmdptr
                      );

Int16 imxenc_tnf_inner_product(
                                 short *input_ptr,
                                 short *coeff_ptr,
                                 short *output_ptr,
                                 short input_width,
                                 short input_height,
                                 short coeff_width,
                                 short coeff_height,
                                 short output_width,
                                 short output_height,
                                 short compute_width,
                                 short compute_height,
                                 short num_distribute_horz,
                                 short num_distribute_vert,
                                 short input_offset_horz,
                                 short input_offset_vert,
                                 short coeff_offset_horz,
                                 short coeff_offset_vert,
                                 short input_type,
                                 short coeff_type,
                                 short output_type,
                                 short round_shift,
                                 short nonseqmode,
                                 short skipline,
                                 short operation,
                                 short *cmdptr
                                 );

Int16 imxenc_tnf_array_skip_line_op(
                                 short *input_ptr,
                                 short *coeff_ptr,
                                 short *output_ptr,
                                 short input_width,
                                 short input_height,
                                 short coeff_width,
                                 short coeff_height,
                                 short output_width,
                                 short output_height,
                                 short compute_width,
                                 short compute_height,
                                 short input_type,
                                 short coeff_type,
                                 short output_type,
                                 short round_shift,
                                 short nonseqmode,
                                 short skipline,
                                 short operation,
                                 short *cmdptr
                                 );

/* ============================================================= */
/* Sobel X-Direction Filtering    */
/* ============================================================= */
short imxenc_sobelx(
                                short *input_ptr,    /* starting address of input */
                                short *coeff_ptr,
                                short *temp_ptr,        
                                short *output_ptr,   /* starting address of output */
                                short input_width,   /* width/columns of input */
                                short input_height,    /* height/rows of input */
                                short output_width,    /* width/columns of output */
                                short output_height, /* height/rows of output */
                                short compute_width, /* computation dimension/number of columns */
                                short compute_height,  /* computation dimension/number of rows */
                                short input_type,    /* short/byte, signed/unsigned */
                                short output_type,     /* short/byte */
                                short round_shift,   /* shifting parameter */
                                short *cmdptr);

/* ============================================================= */
/* Sobel Y-Direction Filtering    */
/* ============================================================= */
short imxenc_sobely(
    short *input_ptr,    /* starting address of input */
    short *coeff_ptr,
    short *temp_ptr,        
    short *output_ptr,   /* starting address of output */
    short input_width,   /* width/columns of input */
    short input_height,    /* height/rows of input */
    short output_width,    /* width/columns of output */
    short output_height, /* height/rows of output */
    short compute_width, /* computation dimension/number of columns */
    short compute_height,  /* computation dimension/number of rows */
    short input_type,    /* short/byte, signed/unsigned */
    short output_type,     /* short/byte */
    short round_shift,   /* shifting parameter */
    short *cmdptr);

Int16 imxenc_filterLR(
                       Int16 * input_ptr,
                      Int16 *weight_ptr,
                      Int16 *output_ptr,
                      Int16 input_width,
                      Int16 input_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input_type,
                      Int16 scalar_type,
                      Int16 round_shift,
                      Int16 * cmdptr
                      );

Int16 imxenc_array_op_bin_bytes_to_dec(
                      Int16 * input1_ptr,
                      Int16 * input2_ptr,
                      Int16 * output_ptr,
                      Int16 input1_width,
                      Int16 input1_height,
                      Int16 input2_width,
                      Int16 input2_height,
                      Int16 output_width,
                      Int16 output_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 operation,
                      Int16 *cmdptr
                      );

Int16 imxenc_tlu_given_num_points(
                 Int16 * data_ptr,                    /* point to input data array */
                 Int16 * table_base,                  /* point to lookup table */
                 Int16 * output_ptr,                  /* point to output array */
                 Int16 input_width,                   /* width of input data array */
                 Int16 input_height,                  /* height of input data array */
                 Int16 table_size,                    /* number of entries in the table */
                 Int16 no_lut_horiz,                  /* number of tables horizontally */
                 Int16 no_lut_vert,                   /* number of tables vertically */
                 Int16 data_points,					  /* number of data points */
                 Int16 coeff_points,                  /* number of coefficient points */
                 Int16 output_points,                 /* number of output points */
                 Int16 output_width,                  /* width of output data array */
                 Int16 output_height,                 /* height of output data array */
                 Int16 compute_width,                 /* width of actual output */
                 Int16 compute_height,                /* height of actual output */
                 Int16 input_type,                    /* short/byte, signed/unsigned */
                 Int16 table_type,                    /* short/byte, signed/unsigned */
                 Int16 output_type,                   /* output written as short/byte */
                 Int16 round_shift,                   /* rounding right shift before lookup */
                 Int16 * cmdptr
                 );

Int16 imxenc_array_integral_sum_horizontal_dir(
                    Int16 * input_ptr,                   /* starting address of input */
                    Int16 * coeff_ptr,                   /* starting address of coefficients */
                    Int16 * output_ptr,                  /* starting address of output */
                    Int16 input_width,                   /* width/columns of input */
                    Int16 input_height,                  /* height/rows of input */
                    Int16 coeff_width,                   /* width/columns of coefficients */
                    Int16 coeff_height,                  /* height/rows of coefficients */
                    Int16 output_width,                  /* width/columns of output */
                    Int16 output_height,                 /* height/rows of output */
                    Int16 compute_width,                 /* computation dimension/number of columns */
                    Int16 compute_height,                /* computation dimension/number of rows */
                    Int16 input_type,                    /* short/byte, signed/unsigned */
                    Int16 coeff_type,                    /* short/byte, signed/unsigned */
                    Int16 output_type,                   /* short/byte */
                    Int16 round_shift,                   /* shifting parameter */
                    Int16 operation,
                    Int16 * cmdptr
                    );

Int16 imxenc_array_scalar_column_op(
                      Int16 * input1_ptr,
                      Int16 * input2_ptr,
                      Int16 * output_ptr,
                      Int16 input1_width,
                      Int16 input1_height,
                      Int16 input2_width,
                      Int16 input2_height,
                      Int16 output_width,
                      Int16 output_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 operation,
                      Int16 * cmdptr
                      );

Int16 imxenc_scalar_arrays_op(
                      Int16 * input1_ptr,
                      Int16 * input2_ptr,
                      Int16 * output_ptr,
                      Int16 input1_width,
                      Int16 input1_height,
                      Int16 input2_width,
                      Int16 input2_height,
                      Int16 output_width,
                      Int16 output_height,
                      Int16 compute_width,
                      Int16 compute_height,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 operation,
                      Int16 * cmdptr
                      );

Int16 imxenc_call_ptr_mod_command(
	                         Int16 * call_cmd_addr,
                             Int16 * input1_ptr,                  /* starting address of 1st input */
                             Int16 * input2_ptr,                  /* starting address of 2nd input */
                             Int16 * output_ptr,                  /* starting address of output */
                             Int16 input1_type,                   /* short/byte signed/unsigned */
                             Int16 input2_type,                   /* short/byte signed/unsigned */
                             Int16 output_type,                   /* short/byte */
                             Int16 * cmdptr
                             );

Int16 imxenc_cmd_write_command(
	                         Int16 * dst_cmd_mem_addr,
                             Int16 * src_ptr_addr,
                             Int16    num_16bit_words,
                             Int16 * cmdptr
                             );

Int16 imxenc_tlu_multiOutput(
   Int16 *data_ptr,          /* point to input data array */
   Int16 *table_base,        /* point to lookup table */
   Int16 *output_ptr,        /* point to output array */
   Int16 input_width,        /* width of input data array */
   Int16 input_height,       /* height of input data array */
   Int16 table_size,         /* number of entries in the table */
   Int16 no_lut_horiz,       /* number of tables horizontally */
   Int16 no_lut_vert,        /* number of tables vertically */
   Int16 output_width,       /* width of output data array */
   Int16 output_height,      /* height of output data array */
   Int16 compute_width,      /* width of actual output */
   Int16 compute_height,     /* height of actual output */
   Int16 input_type,         /* IMXTYPE_BYTE, IMXTYPE_UBYTE, IMXTYPE_SHORT, IMXTYPE_USHORT */
   Int16 table_type,         /* IMXTYPE_BYTE, IMXTYPE_UBYTE, IMXTYPE_SHORT, IMXTYPE_USHORT */
   Int16 output_type,        /* IMXOTYPE_BYTE, IMXOTYPE_SHORT */
   Int16 round_shift,        /* rounding right shift before lookup */
   Int16 numOutputPerLUT,    /* number of output points per lookup: 1, 2, 4 */
   Int16 *cmdptr);

Int16 imxenc_fftButterFlyStage1(
    Int16 *input_ptr,		/* starting address of complex points input */
    Int16 *coef_ptr,		/* starting address of coef[1 1 1 -1] */
    Int16 *output_ptr,    /* starting address of output complext points*/
    Int16 numPoints,		/* number of complex points */
    Int16 input_type,		/* type of real and imaginary components of input */
    Int16 coef_type,		/* IMXTYPE_SHORT, IMXTYPE_BYTE */
    Int16 output_type,		/* type of real and imaginary components of output */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);

Int16 imxenc_fftTwiddleMpy(
    Int16 *input_ptr,		/* starting address of complex points input */
    Int16 *coef_ptr,		/* starting address of twiddle factors */
    Int16 *output_ptr,    /* starting address of output complext points*/
    Uint16 input_width,   /* for fft case, offset between sets in complex points */
    Uint16 coef_width,    /* for fft case, offset between set of twiddle factor */
    Uint16 output_width,  /* for fft case, offset between sets in complex points */
    Uint16 compute_width, /* for fft case, number of complex points per set to multiply with twiddle */
    Uint16 compute_height, /* for fft case, number of sets */
    Int16 input_type,		/* type of real and imaginary components of input */
    Int16 coef_type,		/* type of real and imaginary components of input of twiddle factors */
    Int16 output_type,		/* type of real and imaginary components of output */
    Int16 round_shift,		/* Shifting parameter */
    Int16 *cmdptr);

Int16 imxenc_array_mac(
                      Int16 * input1_ptr,   /* Starting address of the 1st set of arrays A, C*/
                      Int16 * input2_ptr,   /* Starting address of the 2st set of arrays B, D*/
                      Int16 * output_ptr,   /* Starting address of the output array*/
                      Int16 input1_width,   /* N1 */
                      Int16 input2_width,   /* N2 */
                      Int16 output_width,   /* output width */
                      Int16 compute_width,  /* N */
                      Int16 compute_height, /* M */
                      Int16 input1_arrayOffset,
                      Int16 input2_arrayOffset,
                      Int16 input1_type,
                      Int16 input2_type,
                      Int16 output_type,
                      Int16 round_shift,
                      Int16 num_arrays,
                      Int16 * cmdptr
                      );

Int16 imxenc_scaleInputnMul(
    Int16 *input_ptr,
    Int16 *coef_ptr,
    Int16 *output_ptr,
    Int16 *mask_ptr,
    Int16 input_width,
    Int16 coef_width,
    Int16 output_width,
    Int16 compute_width,
    Int16 compute_height,
    Int16 input_type,
    Int16 coef_type,
    Int16 output_type,
    Int16 round_shift,
    Int16 *cmdptr
    );
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif   /* _IMXBASE_H */
