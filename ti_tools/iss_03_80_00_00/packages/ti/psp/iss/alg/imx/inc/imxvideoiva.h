/** ===========================================================================
* @file  imxvideoiva.h
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

#ifdef CCS_PROJECT

#include <tistdtypes.h>

#else

//#include "../../../common/tistdtypes.h"
#include <ti/psp/iss/timmosal/inc/timm_osal_types.h>

#endif


#ifndef _IMXVIDEO_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMXVIDEO_H_



short imxenc_flip(
  short *pred_blk_ptr, 		/* starting address of input */
  short *coeff_ptr,		    /* starting address of coefficients */
  short *out_ptr , 
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


short imxenc_bilinearinterp(
    short *refblk_ptr, /* starting address of 1st input */
    short *output_format, /* output format */
    short *pred_blk_ptr, /* starting address of output */
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short total_refblk_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short inter   ,
    short y ,
    short Nh ,
    short Nv ,
    short msb ,
    short rnd_shift ,
    short rnd_type ,
    short* cmdptr
);

short imxenc_copyref(
    short *refblk_ptr, /* starting address of 1st input */
    short *coeff_ptr, /* starting address of 2nd input */
    short *pred_blk_ptr, /* starting address of output */
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short total_refblk_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short inter   ,   /* 0: separate, 1: YCbYCr, 2: CbCr */
    short y ,
    short Nh ,
    short Nv ,
    short msb ,
    short rnd_shift ,
    short rnd_type ,
    short* cmdptr);

short imxenc_hinterp(
    short *refblk_ptr, /* starting address of 1st input */
    short *output_format, /* output format */
    short *pred_blk_ptr, /* starting address of output */
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short total_refblk_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */
    short y ,
    short Nh ,
    short Nv ,
    short msb ,
    short rnd_shift ,
    short rnd_type ,
    short* cmdptr

);

short imxenc_padding(short *input,
          short *coeff,
          short *output,
          short pad_width,
          short input_width,
          short output_width,
          short pad_height,
          short input_type,
          short output_type,
          short high_low,
          short inter ,
          short *cmdptr);

short imxenc_qpel_mca(
  short *input_ptr,     /* starting address of input */
  short *coeff_ptr,   /* starting address of coefficients */
  short *out_ptr ,
  short ref_width ,
  short compute_height,
  short mode ,
  short data_inter ,
  short out_inter ,
  short input_type,
  short out_type,
  short data_msb,
  short out_msb,
  short shift,
  short shift_type ,
  short *cmdptr
);


short imxenc_qpel_mcb(
  short *input_ptr,     /* starting address of input */
  short *coeff_ptr,   /* starting address of coefficients */
  short *out_ptr ,
  short ref_width ,
  short mode ,
  short data_inter ,
  short out_inter ,
  short input_type,
  short out_type,
  short data_msb,
  short out_msb,
  short shift,
  short shift_type ,
  short *cmdptr
);

short imxenc_qpel_padding(
  short *pred_blk_ptr,    /* starting address of input */
  short *coeff_ptr,       /* starting address of coefficients */
  short ref_width ,
  short mode ,             /* Note, a block here is defined as 8 pixels horizontally and
                             8 pixels vertically */
  short data_msb ,
  short data_inter ,
  short *cmdptr
);


short imxenc_allinterp(
  short *ref_ptr,   /* starting address of reference block */
  short *zero_ptr,    /* ptr to zero value */
  short *output_ptr,  /* address where result will be */
  short block_width,  /* width (x dimension) of matching block */
  short block_height, /* height (y dimension) of matching block */
  short ref_width,    /* width of reference block */
  short ref_type,     /* signed/unsigned byte short */
  short data_inter ,
  short data_msb ,
  short target_type,  /* signed/unsigned byte short */
  short rnd_on_off,
  short *cmdptr);

short imxenc_sadmultiple(
    short *target_ptr,       /* point to target array */
    short *ref_ptr,          /* point to reference array */
    short *output_ptr,       /* point to output array */
    short block_width,       /* width of matching block */
    short block_height,      /* height of matching block */
    short target_width,      /* width of target array */
    short ref_width,         /* width of refernece array */
    short step_horz,         /* horizontal offset between matchings */
    short step_vert,         /* vertical offset between matchings */
    short nsteps_horz,       /* number of steps horizontally */
    short nsteps_vert,       /* number of steps vertically */
    short minmax_clear, /* 0: retain, 1: clear */
    short minmax, /* 0: min, 1: max  */
    short id_mode,  /* 0: block count, 1: address */
    short id_value, /* 0: ID, 1: min/max value */
    short thresh,
    short center_block,
    short skip_mask,
    short *cmdptr);


short imxenc_sadmultipleinterp(
  short *target_ptr,    /* starting address of current block */
  short *ref_ptr,     /* starting pointer of reference block */
  short *output_ptr,  /* address where result will be */
  short *zero_ptr,    /* pointer to zero value */
  short *coef_ptr ,
  short *out_ptr ,
  short block_width,  /* width (x dimension) of matching block */
  short block_height, /* height (y dimension) of matching block */
  short target_width, /* width of target array */
  short ref_type,     /* unsigned/signed byte short */
  short target_type,  /* unsigned/short byte/short */
  short target_inter ,
  short data_msb,
  short *cmdptr);


short imxenc_sadmultiplesteps(
    short *target_ptr,       /* point to target array */
    short *ref_ptr,          /* point to reference array */
    short *output_ptr,       /* point to output array */
  short *data_ptrs,        /* pointer to temp array, skip_mask, match_type_array */
  short *data_format,      /* pointer to data format flags */
    short block_width,       /* width of matching block */
    short block_height,      /* height of matching block */
    short target_width,      /* width of target array */
    short ref_width,         /* width of refernece array */
  short init_step_horz, /* initial horizontal step size */
  short init_step_vert,  /* initial vertical step size */
    short nsteps_horz,       /* number of steps horizontally */
    short nsteps_vert,       /* number of steps vertically */
  short nsteps_multipleSAD,/* Number of steps of Multiple SADs*/
    short init_minmax_clear, /* 0: retain, 1: clear */
    short minmax, /* 0: min, 1: max  */
    short minmax_value, /* 0: Addr only, 1: min/max value */
    short **thresh, 
    short center_block,
#if 1
    short table_inter, /* 	0: tables are not interleaved
    					    1: Tables are interleaved to 1 table */
#endif
    short *cmdptr);  

#ifdef _DM270
static inline
#endif
short imxenc_mbpackYUV_sep(
    short *curr_ptr,    /* starting address of input */
    short *coeff_ptr,   /* pointer to single scalar used for scaling */
    short *result_ptr_Y,
    short *result_ptr_UV,
    short input_type,
    short output_width,
    short swap_CbCr,
    short *cmd_ptr);  

short imxenc_array2blkseq(
    short *input_ptr,   /* starting address of input */
    short *coeff_ptr,   /* pointer to single scalar used for scaling */
    short *output_ptr,    /* starting address of output */
    short input_width,    /* width of the input image */
    short input_height,   /* height of the input image */
    short input_type,
    short input_msb,
    short no_blks_x,      /* handled no of blocks in x-direction */
    short no_blks_y,      /* handled no of blocks in y-direction */
    short round_shift,    /* shifting parameter */
    short *cmdptr);


short imxenc_QIQ(
  short *dct_coef,
  short *quantized_coef,
  short *temp_data,
  short *zero_ptr,
  short *DC_predictor,
  short *Quant_matrix,
  short *IQuant_matrix, 
  short *QScanpath,
  short *IQScanpath,
  short *id_scan,
  short *Quant_dc_scaler,
  short *IQuant_dc_scaler, 
  short *Quant_ac_scaler,
  short *IQuant_ac_scaler,
  short *round_dc,
  short *round_ac,
  short *Quant_delta,
  short *IQuant_delta,
  short *CBP,
  short mbintra,
  short q_lower_threshold_dc,
  short q_upper_threshold_dc,
  short q_lower_threshold_ac,
  short q_upper_threshold_ac, 
  short iq_lower_threshold_ac,
  short iq_upper_threshold_ac,  
  short mode,
  short shift,
  short no_blocks,
  short function,
  short *cmd_ptr);

short imxenc_vinterp(
    short *refblk_ptr, /* starting address of 1st input */
    short *output_format, /* output format */
    short *pred_blk_ptr, /* starting address of output */
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short total_refblk_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short inter   , /* 0: separate, 1: YCbYCr, 2: CbCr */ 
    short y ,
    short Nh ,
    short Nv , 
    short msb , 
    short rnd_shift ,
    short rnd_type ,    
    short* cmdptr 
 
);
short imxenc_padding2(short *input,
    short *coeff,
    short *output,
    short pad_width,
    short input_width,
    short output_width,
    short pad_height,
    short input_type,
    short output_type,
    short high_low, 
    short inter ,
    short h_or_v,
    short *cmdptr);

#ifdef _DM270
static inline
#endif
short imxenc_allinterpblock(
    short *refblk_ptr, /* starting address of 1st input */
    short *zero_ptr,
    short *pred_blk_ptr, /* starting address of output */
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short total_refblk_width,  /* reference buffer width */
    short ref_inter,
    short ref_msb,
    short output_type, /* short/byte */
    short out_inter,    
    short* cmdptr); 

Int16 imxenc_Update_LumInput_allinterpblock
(
    Int16 **input_ptr, /* points to starting byte offset of input */
    Int16 *cmdptr_update, /* update command */
    Int16 *cmdptr_allinterpblock  
);
    

void imxUpdate_threshold_sad(
	short threshold,
	short nsteps_multipleSAD,
	short **cmd_array);

void imxUpdate_ChromInput_copyref(
	short* input_ptr,
	short  highbyte_in,
	short  input_inter,
	short* cmd_ptr);

void imxUpdate_ChromOutput_copyref(
	short* outp_ptr,
	short  highbyte_out,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short output_type, /* short/byte */
    short Nh ,
    short Nv , 
	short* cmd_ptr
);

void iMXUpdate_LumParams_copyref
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short *cmdptr);
    
void iMXUpdate_CbCrParams_copyref
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short *cmdptr);
        
void imxUpdate_LumInput_hinterp(
	short* input_ptr,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);

void imxUpdate_LumInput_vinterp(
	short* input_ptr,
	short  ref_width,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);


void imxUpdate_ChromInput_hinterp(
	short* input_ptr,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);


void imxUpdate_ChromInput_vinterp(
	short* input_ptr,
	short  ref_width,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);

void imxUpdate_ChromOutput_hvinterp(
	short* output_ptr,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short Nh ,
    short Nv , 
    short output_type,
	short* cmd_ptr
);

void iMXUpdate_LumParams_hvinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short output_inter,
    short input_inter,
    short *cmdptr);    

void iMXUpdate_CbCrParams_hvinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh,
    short Nv,
    short *cmdptr);


void iMXUpdate_round_hvinterp
(   short rnd_shift, 
    short rnd_type,
    short lum,
    short *cmdptr);        

void imxUpdate_LumInput_bilinearinterp(
	short* input_ptr,
	short  highbyte_in,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
	short ref_width,
    short  inter,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
    short Nv, 
	short* cmd_ptr);

void imxUpdate_ChromInput_bilinearinterp(
	short* input_ptr,
    short  blk_width, /* width/columns of 1st input block */
    short  blk_height,         /* height/rows of 1st input block */
	short  ref_width,
	short  Nv,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr);
	
void imxUpdate_LumOutput_bilinearinterp(
	short* output_ptr,
	short output_highbyte,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short Nh ,
    short Nv , 
    short output_inter,
    short output_type,
	short* cmd_ptr);

void imxUpdate_ChromOutput_bilinearinterp(
	short* output_ptr,
	short output_highbyte,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short Nh ,
    short Nv , 
    short output_type,
	short* cmd_ptr);
	

void iMXUpdate_LumParams_bilinearinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short output_inter,
    short input_inter,
    short *cmdptr);


void iMXUpdate_CbCrParams_bilinearinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short *cmdptr);

void iMXUpdate_round_bilinearinterp
(   short rnd_shift, 
    short rnd_type,
    short lum,
    short Nv,
    short inter,
    short *cmdptr); 

void imxUpdate_Input_padding2(
	short *input_ptr,
	short input_highbyte,
	short inter,
    short *cmd_ptr);
    
void imxUpdate_Params_padding2(
	short pad_width,
	short input_width,
	short output_width,
	short pad_height,
	short input_type,
    short output_type,
    short inter,
	short h_or_v,
    short *cmdptr);
    
void imxUpdate_LumInput_allinterpblock
(
    short *input_ptr, /* starting address of 1st input */
    short highbyte_in,
    short ref_width,  /* reference buffer width */
    short blk_height,         /* height/rows of 1st input block */
    short *cmdptr 
);            
        				             		             
         

void imxUpdate_threshold_sad(
	short threshold,
	short nsteps_multipleSAD,
	short **cmd_array);

void imxUpdate_ChromInput_copyref(
	short* input_ptr,
	short  highbyte_in,
	short  input_inter,
	short* cmd_ptr);

void imxUpdate_ChromOutput_copyref(
	short* outp_ptr,
	short  highbyte_out,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short output_type, /* short/byte */
    short Nh ,
    short Nv , 
	short* cmd_ptr
);

void iMXUpdate_LumParams_copyref
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short *cmdptr);
    
void iMXUpdate_CbCrParams_copyref
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short *cmdptr);
        
void imxUpdate_LumInput_hinterp(
	short* input_ptr,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);

void imxUpdate_LumInput_vinterp(
	short* input_ptr,
	short  ref_width,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);


void imxUpdate_ChromInput_hinterp(
	short* input_ptr,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);


void imxUpdate_ChromInput_vinterp(
	short* input_ptr,
	short  ref_width,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr
);

void imxUpdate_ChromOutput_hvinterp(
	short* output_ptr,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short Nh ,
    short Nv , 
    short output_type,
	short* cmd_ptr
);

void iMXUpdate_LumParams_hvinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short output_inter,
    short input_inter,
    short *cmdptr);    

void iMXUpdate_CbCrParams_hvinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh,
    short Nv,
    short *cmdptr);


void iMXUpdate_round_hvinterp
(   short rnd_shift, 
    short rnd_type,
    short lum,
    short *cmdptr);        

void imxUpdate_LumInput_bilinearinterp(
	short* input_ptr,
	short  highbyte_in,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
	short ref_width,
    short  inter,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
    short Nv, 
	short* cmd_ptr);

void imxUpdate_ChromInput_bilinearinterp(
	short* input_ptr,
    short  blk_width, /* width/columns of 1st input block */
    short  blk_height,         /* height/rows of 1st input block */
	short  ref_width,
	short  Nv,
	short  highbyte_in,
    short  inter   ,  /* 0: separate, 1: YCbYCr, 2: CbCr */ 
	short* cmd_ptr);
	
void imxUpdate_LumOutput_bilinearinterp(
	short* output_ptr,
	short output_highbyte,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short Nh ,
    short Nv , 
    short output_inter,
    short output_type,
	short* cmd_ptr);

void imxUpdate_ChromOutput_bilinearinterp(
	short* output_ptr,
	short output_highbyte,
    short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short Nh ,
    short Nv , 
    short output_type,
	short* cmd_ptr);
	

void iMXUpdate_LumParams_bilinearinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short output_inter,
    short input_inter,
    short *cmdptr);


void iMXUpdate_CbCrParams_bilinearinterp
(   short blk_width, /* width/columns of 1st input block */
    short blk_height,         /* height/rows of 1st input block */
    short ref_width,  /* reference buffer width */
    short output_type, /* short/byte */
    short Nh ,
    short Nv,
    short *cmdptr);

void iMXUpdate_round_bilinearinterp
(   short rnd_shift, 
    short rnd_type,
    short lum,
    short Nv,
    short inter,
    short *cmdptr); 

void imxUpdate_Input_padding2(
	short *input_ptr,
	short input_highbyte,
	short inter,
    short *cmd_ptr);
    
void imxUpdate_Params_padding2(
	short pad_width,
	short input_width,
	short output_width,
	short pad_height,
	short input_type,
    short output_type,
    short inter,
	short h_or_v,
    short *cmdptr);
    
void imxUpdate_LumInput_allinterpblock
(
    short *input_ptr, /* starting address of 1st input */
    short highbyte_in,
    short ref_width,  /* reference buffer width */
    short blk_height,         /* height/rows of 1st input block */
    short *cmdptr 
);            

short imxenc_array_op_distribute(
    short *input_ptr,		/* starting address of input */
    short *coeff_ptr,		/* starting address of coefficients */
    short *output_ptr,		/* starting address of output */
    short input_width,		/* width of input array */
    short input_height,		/* height of input array */
    short coeff_width,		/* width of coefficient array */
    short coeff_height,		/* height of coefficient array */
    short output_width,		/* width of output array */
    short output_height,	/* height of output array */
    short compute_width,	/* computed width */
    short compute_height,	/* computed height */
    short operation,		/* operation data with coefficient */
    short num_distribute_horz,	/* number of inner arrays horizontally */
    short num_distribute_vert,	/* number of inner arrays vertically */
    short input_offset_horz,	/* horizontal offset of input inner arrays in data pts */
    short input_offset_vert,	/* vertical offset of input inner arrays in data pts */
    short output_offset_horz,	/* horizontal offset of output inner arrays in data pts */
    short output_offset_vert,	/* vertical offset of output inner arrays in data pts */
    short input_type,		/* short/byte, signed/unsigned */
    short coeff_type,		/* short/byte, signed/unsigned */
    short output_type, 		/* short/byte */
    short round_shift,		/* shifting parameter */
    short *cmdptr);

short imxenc_sadmultiplesteps_addrmod(
    short *target_ptr,       /* point to target array */
    short *ref_ptr,          /* point to reference array */
    short *output_ptr,       /* point to output array */
	short *data_ptrs,        /* pointer to temp array, skip_mask, match_type_array */
	short *data_format,      /* pointer to data format flags */
    short block_width,       /* width of matching block */
    short block_height,      /* height of matching block */
    short target_width,      /* width of target array */
    short ref_width,         /* width of refernece array */
	short init_step_horz,	/* initial horizontal step size */
	short init_step_vert,  /* initial vertical step size */
    short nsteps_horz,       /* number of steps horizontally */
    short nsteps_vert,       /* number of steps vertically */
	short nsteps_multipleSAD,/* Number of steps of Multiple SADs*/
    short init_minmax_clear, /* 0: retain, 1: clear */
    short minmax, /* 0: min, 1: max  */
    short minmax_value, /* 0: Addr only, 1: min/max value */
    short **thresh, 
    short center_block,
    short table_inter, /* 	0: tables are not interleaved
    					    1: Tables are interleaved to 1 table */
    short *cmdptr);        				             		             


short
imxenc_decision4mv1mv(
		short *SAD4mv,
		short *SAD1mv,
		short *flag_4mv,
		short *flag_notborder,
		short *zero_ptr,
		short *branch_cmd_table,		
		short *mvskip_thresh,		
		short *branch_cmd,
		short *cmd_ptr);


              
short
imxenc_decisionskip4mv(
		short *zerobias,
		short *bias_table,
		short *SADMV0,
		short *scratch_imgbuf,
		short *SAD1mv,
		short *flag_4mv,
		short *flag_notborder,
		short *zero_ptr,
		short *branch_cmd_table,
		short *threshold_4mv,		
		short *branch_cmd,
		short *cmd_ptr);



short imxenc_sadsingle(
    short *target_ptr,       /* point to target array */
    short *ref_ptr,          /* point to reference array */
    short *output_ptr,       /* point to output array */
    short block_width,       /* width of matching block */
    short block_height,      /* height of matching block */
    short target_width,      /* width of target array */
    short ref_width,         /* width of refernece array */
    short minmax_clear, /* 0: retain, 1: clear */
    short minmax, /* 0: min, 1: max  */
    short id_mode,  /* 0: block count, 1: address */
    short id_value, /* 0: ID, 1: min/max value */
    short thresh,
    short center_block,
    short skip_mask, 
    short *cmdptr);



Int16 imxenc_bitstitch(
    Int16  *input_ptr, 
    Int16  *mask_ptr,				/* 2 coefficients */
    Int16  *output_ptr, 
    Int16  width, 
    Int16  height, 
    Int16  shift,
    Int16  input_highbyte,
    Int16  coeff_highbyte,
    Int16  output_highbyte,
    Int16  *cmdptr);
    
Int16 imxenc_swap_forstitch(
    Int16  *input_ptr, 
    Int16  *zero_ptr,				/* 2 coefficients */
    Int16  *output_ptr, 
    Int16  width, 
    Int16  height, 
    Int16  shift,
    Int16  input_highbyte,
    Int16  coeff_highbyte,
    Int16  output_highbyte,
    Int16  *cmdptr);    



void
imxenc_bilinearinterp_round_control(
  RND_Control *rndCtrl, 
  int enable);

Int16 iMX_SetSingleOp(
                      Int16 *input,
                      Int16 *coeff,
                      Int16 *output,
                      Int16 input_type,
                      Int16 coeff_type,
                      Int16 output_type,
                      Int16 rnd_shift,
                      Int16 operation,
                      Uint16 *cmd_buf
                      );
                      
Int16 imxenc_setsingleop(
                      Int16 *input,
                      Int16 *coeff,
                      Int16 *output,
                      Int16 input_type,
                      Int16 coeff_type,
                      Int16 output_type,
                      Int16 rnd_shift,
                      Int16 operation,
                      Uint16 *cmd_buf
                      );


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
