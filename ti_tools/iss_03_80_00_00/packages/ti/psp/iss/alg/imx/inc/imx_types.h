/** ===========================================================================
* @file  imx_types.h
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
#ifndef _IMX_TYPES_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMX_TYPES_H_

#ifdef CCS_PROJECT

#include <tistdtypes.h>

#else

//#include <WTSD_DucatiMMSW/alg/common/tistdtypes.h>
#include <ti/psp/iss/timmosal/inc/timm_osal_types.h>

#endif




typedef struct imxencparams {
  Int16 acc_mode;
  Int16 op;
  Int16 lpend1;
  Int16 lpend2;
  Int16 lpend3;
  Int16 lpend4;
  Int16 *data_ptr;
  Int16 data_highbyte;
  Int16 *coef_ptr;
  Int16 coef_highbyte;
  Int16 *outp_ptr;
  Int16 outp_highbyte;
  Int16 data_Addrupdate[8];
  Int16 coef_Addrupdate[8];
  Int16 outp_Addrupdate[8];
  Int16 dpoints;
  Int16 cpoints;
  Int16 opoints;
  Int16 dsize;
  Int16 csize;
  Int16 osize;
  Int16 acclp;
  Int16 rnd_shift;
  Int16 rnd_onoff;
  Int16 clear;
  Int16 sel;
  Int16 IDmode;
  Int16 IDvalue;
  Int16 cmd_type;                      /* 0: MinMax 1: MotionEstimation 2: Regular iMX */
  Int16 *cmd_p;
} ImxEncParams;

typedef struct addrupdates {
  Int16 AddrMod;
  Int16 AddrMask;
} AddrUpdates;


typedef struct addrptrs {
  Int16 *Addr;
  Int16 Addr_highbyte;
} AddrPtrs;


typedef struct imxencinputtype {
  Int16 acc_mode;
  Uint16 op;                           /* wmlai Apr 2005 */
  Int16 lpend1;
  Int16 lpend2;
  Int16 lpend3;
  Int16 lpend4;
  AddrPtrs data_ptr;
  AddrPtrs coef_ptr;
  AddrPtrs outp_ptr;
  AddrUpdates data_Addrupdate[4];
  AddrUpdates coef_Addrupdate[4];
  AddrUpdates outp_Addrupdate[4];
  Int16 dpoints;
  Int16 cpoints;
  Int16 opoints;
  Int16 dsize;
  Int16 csize;
  Int16 osize;
  Int16 acclp;
  Int16 rnd_shift;
  Int16 rnd_onoff;
  Int16 clear;
  Int16 sel;
  Int16 IDmode;
  Int16 IDvalue;
  Int16 cmd_type;                      /* 0: MinMax 1: MotionEstimation 2: Regular iMX */
  Int16 *cmd_p;
} ImxEncInputType;

typedef union SetMaskParamOpcode {
 
 struct {
   Uint32 numMskMod : 3;
   Uint32 mPoints   : 2;
   Uint32 mSize     : 1;
   Uint32 inpDsel   : 2;
   Uint32 mskPrmtr  : 1;
   Uint32 mskOp     : 2;
   Uint32 resv0     : 4;
   Uint32 resv1     : 1;
   Uint32 dummy     :16;
 } bitf;
 
 Int16 val;
} SetMaskParamOpcode;

#define SET_MASK_PARAM_OPCODE_DEFAULT 0x9800
#define SET_MASK_PARAM_MSIZE_SHORT 0
#define SET_MASK_PARAM_MSIZE_BYTE 1
#define SET_MASK_PARAM_INPDSEL_DATA 0
#define SET_MASK_PARAM_INPDSEL_COEF 1
#define SET_MASK_PARAM_INPDSEL_DATACOEF 2
#define SET_MASK_PARAM_MSKOP_OUTPUTMSK 0
#define SET_MASK_PARAM_MSKOP_INPUTSHIFT 1
#define SET_MASK_PARAM_MSKOP_INPUTFORMAT 2
#define IMX_CMD_MINMAX 0
#define IMX_CMD_ME 1
#define IMX_CMD_COMPUTE 2
#define IMX_CMD_MEDIAN  3


typedef union MptrInit {
 
 struct {
     Uint32 mPtrInit  : 14;
     Uint32 resv0     : 1;
     Uint32 bufSel    : 1;
     Uint32 dummy     :16;
 } bitf;
 
 Int16 val;
 
 } MptrInit;
 
typedef union SetMaskAddrMod {
 
 struct {
   Int32 mptrInc : 13;
   Int32 mptrMsk : 3;
   Int32 dummy     :16;
 } bitf;
 
 Int16 val;
 
 } SetMaskAddrMod;
 
typedef struct SetMaskParamCmd{
 SetMaskParamOpcode  setMaskParamOpcode;
 MptrInit            mPtrInit;
 SetMaskAddrMod      setMaskAddrMod[4];
} SetMaskParamCmd;

#define SET_MASK_PARAM_CMD_DEFAULT {0, 0, 0, 0, 0, 0}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
