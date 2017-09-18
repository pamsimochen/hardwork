/** ==================================================================
 *  @file   cslr__rot_1.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/rot/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _CSLR__ROT_1_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR__ROT_1_H_

#include <ti/psp/iss/hal/iss/simcop/common/cslr.h>

#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>

/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct {
    volatile Uint32 REVISION;
    volatile Uint32 CTRL;
    volatile Uint32 CFG;
    volatile Uint32 BLKSZ;
    volatile Uint32 SRC_START1;
    volatile Uint32 SRC_LOFST;
    volatile Uint32 DST_START1;
    volatile Uint32 DST_LOFST;
    volatile Uint32 SRC_START2;
    volatile Uint32 DST_START2;
} CSL_RotRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* REVISION */

#define CSL_ROT_REVISION_REV_MASK (0x000000FFu)
#define CSL_ROT_REVISION_REV_SHIFT (0x00000000u)
#define CSL_ROT_REVISION_REV_RESETVAL (0x00000010u)

#define CSL_ROT_REVISION_RESETVAL (0x00000010u)

/* CTRL */

#define CSL_ROT_CTRL_BUSY_MASK (0x00008000u)
#define CSL_ROT_CTRL_BUSY_SHIFT (0x0000000Fu)
#define CSL_ROT_CTRL_BUSY_RESETVAL (0x00000000u)
/*----BUSY Tokens----*/
#define CSL_ROT_CTRL_BUSY_IDLE (0x00000000u)
#define CSL_ROT_CTRL_BUSY_BUSY (0x00000001u)

#define CSL_ROT_CTRL_EN_MASK (0x00000001u)
#define CSL_ROT_CTRL_EN_SHIFT (0x00000000u)
#define CSL_ROT_CTRL_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_ROT_CTRL_EN_DISABLE (0x00000000u)
#define CSL_ROT_CTRL_EN_ENABLE (0x00000001u)

#define CSL_ROT_CTRL_RESETVAL (0x00000000u)

/* CFG */

#define CSL_ROT_CFG_NBLKS_MASK (0x0000F000u)
#define CSL_ROT_CFG_NBLKS_SHIFT (0x0000000Cu)
#define CSL_ROT_CFG_NBLKS_RESETVAL (0x00000000u)

#define CSL_ROT_CFG_AUTOGATING_MASK (0x00000200u)
#define CSL_ROT_CFG_AUTOGATING_SHIFT (0x00000009u)
#define CSL_ROT_CFG_AUTOGATING_RESETVAL (0x00000001u)
/*----AUTOGATING Tokens----*/
#define CSL_ROT_CFG_AUTOGATING_DISABLE (0x00000000u)
#define CSL_ROT_CFG_AUTOGATING_ENABLE (0x00000001u)

#define CSL_ROT_CFG_TRIG_SRC_MASK (0x00000100u)
#define CSL_ROT_CFG_TRIG_SRC_SHIFT (0x00000008u)
#define CSL_ROT_CFG_TRIG_SRC_RESETVAL (0x00000000u)
/*----TRIG_SRC Tokens----*/
#define CSL_ROT_CFG_TRIG_SRC_MMR_WRITE (0x00000000u)
#define CSL_ROT_CFG_TRIG_SRC_HW_START_SIGNAL (0x00000001u)

#define CSL_ROT_CFG_FMT_MASK (0x00000070u)
#define CSL_ROT_CFG_FMT_SHIFT (0x00000004u)
#define CSL_ROT_CFG_FMT_RESETVAL (0x00000000u)
/*----FMT Tokens----*/
#define CSL_ROT_CFG_FMT_8_BIT (0x00000000u)
#define CSL_ROT_CFG_FMT_16_BIT (0x00000001u)
#define CSL_ROT_CFG_FMT_32_BIT (0x00000002u)
#define CSL_ROT_CFG_FMT_YUV_422 (0x00000003u)
#define CSL_ROT_CFG_FMT_YUV_420 (0x00000004u)
#define CSL_ROT_CFG_FMT_NOT_ALLOWED (0x00000005u)

#define CSL_ROT_CFG_OP_MASK (0x00000007u)
#define CSL_ROT_CFG_OP_SHIFT (0x00000000u)
#define CSL_ROT_CFG_OP_RESETVAL (0x00000000u)
/*----OP Tokens----*/
#define CSL_ROT_CFG_OP_ROTATE_0DEG (0x00000000u)
#define CSL_ROT_CFG_OP_ROTATE_90DEG (0x00000001u)
#define CSL_ROT_CFG_OP_ROTATE_180DEG (0x00000002u)
#define CSL_ROT_CFG_OP_ROTATE_270DEG (0x00000003u)
#define CSL_ROT_CFG_OP_DATA_SHIFT (0x00000004u)
#define CSL_ROT_CFG_OP_HOR_CIRC_SHIFT (0x00000005u)

#define CSL_ROT_CFG_RESETVAL (0x00000200u)

/* BLKSZ */

#define CSL_ROT_BLKSZ_BLKH_MASK (0x0FFF0000u)
#define CSL_ROT_BLKSZ_BLKH_SHIFT (0x00000010u)
#define CSL_ROT_BLKSZ_BLKH_RESETVAL (0x00000000u)

#define CSL_ROT_BLKSZ_BLKW_MASK (0x00000FFFu)
#define CSL_ROT_BLKSZ_BLKW_SHIFT (0x00000000u)
#define CSL_ROT_BLKSZ_BLKW_RESETVAL (0x00000000u)

#define CSL_ROT_BLKSZ_RESETVAL (0x00000000u)

/* SRC_START1 */

#define CSL_ROT_SRC_START1_ADDR_MASK (0x00003FFFu)
#define CSL_ROT_SRC_START1_ADDR_SHIFT (0x00000000u)
#define CSL_ROT_SRC_START1_ADDR_RESETVAL (0x00000000u)

#define CSL_ROT_SRC_START1_RESETVAL (0x00000000u)

/* SRC_LOFST */

#define CSL_ROT_SRC_LOFST_LOFST_MASK (0x0000FFFFu)
#define CSL_ROT_SRC_LOFST_LOFST_SHIFT (0x00000000u)
#define CSL_ROT_SRC_LOFST_LOFST_RESETVAL (0x00000000u)

#define CSL_ROT_SRC_LOFST_RESETVAL (0x00000000u)

/* DST_START1 */

#define CSL_ROT_DST_START1_ADDR_MASK (0x00003FFFu)
#define CSL_ROT_DST_START1_ADDR_SHIFT (0x00000000u)
#define CSL_ROT_DST_START1_ADDR_RESETVAL (0x00000000u)

#define CSL_ROT_DST_START1_RESETVAL (0x00000000u)

/* DST_LOFST */

#define CSL_ROT_DST_LOFST_LOFST_MASK (0x0000FFFFu)
#define CSL_ROT_DST_LOFST_LOFST_SHIFT (0x00000000u)
#define CSL_ROT_DST_LOFST_LOFST_RESETVAL (0x00000000u)

#define CSL_ROT_DST_LOFST_RESETVAL (0x00000000u)

/* SRC_START2 */

#define CSL_ROT_SRC_START2_ADDR_MASK (0x00003FFFu)
#define CSL_ROT_SRC_START2_ADDR_SHIFT (0x00000000u)
#define CSL_ROT_SRC_START2_ADDR_RESETVAL (0x00000000u)

#define CSL_ROT_SRC_START2_RESETVAL (0x00000000u)

/* DST_START2 */

#define CSL_ROT_DST_START2_ADDR_MASK (0x00003FFFu)
#define CSL_ROT_DST_START2_ADDR_SHIFT (0x00000000u)
#define CSL_ROT_DST_START2_ADDR_RESETVAL (0x00000000u)

#define CSL_ROT_DST_START2_RESETVAL (0x00000000u)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
