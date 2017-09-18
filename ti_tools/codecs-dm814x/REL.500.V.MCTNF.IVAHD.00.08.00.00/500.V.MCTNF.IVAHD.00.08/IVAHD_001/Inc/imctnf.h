/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from a
 *  4:2:0 semi planar Raw data.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

/**
  ******************************************************************************
  *  @file     imctnf.h
  *
  *  @brief    IMCTNF Interface Header
  *
  *  @author   Shyam Jagannathan
  *
  *  @version  0.1 - Nov 30,2013 : Initial Version [Shyam]
  *
 *****************************************************************************
*/

/**
 *  @defgroup   MCTNF IMCTNF_TI (V7M)
 *  @ingroup    m3
 *
 *              The IMCTNF_TI interface enables video noise filtering
 *
 */

#ifndef _IMCTNF_H_  //--{

#define _IMCTNF_H_

#include <ti/xdais/ialg.h>
#include "ividnf1.h"

/** @ingroup    MCTNF */
/*@{*/


#ifdef __cplusplus
extern "C" {
#endif

/**
  Length of the version string. The memory to get version
  number is owned by application
*/
#define IMCTNF_VERSION_LENGTH (64)

/**
  control method commands
*/
#define IMCTNF_GETSTATUS      XDM_GETSTATUS
#define IMCTNF_SETPARAMS      XDM_SETPARAMS
#define IMCTNF_RESET          XDM_RESET
#define IMCTNF_FLUSH          XDM_FLUSH
#define IMCTNF_SETDEFAULT     XDM_SETDEFAULT
#define IMCTNF_GETBUFINFO     XDM_GETBUFINFO

typedef IVIDNF1_Cmd IMCTNF_Cmd;


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Definition of all the Enumeration define by this interafce  */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**
 *  @enum       IMCTNF_ErrorBit
 *  @brief      error informations of MCTNF implementation by TI.
 *
 *  @remarks    When an internal error occurs, the algorithm will return
 *              an error return value (e.g. EFAIL, EUNSUPPORTED)
 *
 *  @remarks    The value of each enum is the bit which is set.
 *
 *  @remarks    Bits 8-15 are defined by XDM and hence not used by algorithm
 *              implementation.
 *              rest all bits are used. XDM defined error bits are also active.
 *
 *  @remarks    The algorithm can set multiple bits to 1 based on conditions.
 *              e.g. it will set bits #XDM_FATALERROR (fatal) and
 *              #XDM_UNSUPPORTEDPARAM (unsupported params) in case
 *              of unsupported run time parameters.
 *
 */

typedef enum {
  IMCTNF_IMPROPER_HDVICP2_STATE = 16,
          /**< Bit 16 - Device is not proper state to use.
          */

  IMCTNF_UNSUPPORTED_DEVICE = 18,
          /**< Bit 18 - MCTNF run on invalid device.
          *   @remarks  This error is set when MCTNF is run on
          *    an in invalid device.
          */

  IMCTNF_IMPROPER_DATASYNC_SETTING = 19,
          /**< Bit 19 - data synch settings are not proper
          *   @remarks  This error is set when algorithm is asked to operate
          *   at sub frame level but the call back function pointer is NULL
          */

  IMCTNF_UNSUPPORTED_VIDNF1PARAMS = 20,
          /**< Bit 20 - Invalid vidnf1 parameters
          *   @remarks  This error is set when any parameter of struct
          *   IVIDNF1_Params is not in allowed range
          */

  IMCTNF_UNSUPPORTED_MOTIONSEARCHPARAMS = 22,
          /**< Bit 22 - Invalid inter coding parameters
          *   @remarks  This error is set when any parameter of struct
          *   IMCTNF_MotionSearchParams is not in allowed range
          */

  IMCTNF_UNSUPPORTED_FIELDSELECTIONTYPE = 23,
          /**< Bit 23 - Invalid Intra coding parameters
          *   @remarks  This error is set when any parameter of struct
          *   IMCTNF_IntraCodingParams is not in allowed range
          */

  IMCTNF_UNSUPPORTED_NOISEFILTERPARAMS = 26,
          /**< Bit 26 - Invalid loop filter related parameters
          *   @remarks  This error is set when any parameter of struct
          *   IMCTNF_NoiseFilterParams is not in allowed range
          */

  IMCTNF_UNSUPPORTED_MCTNFPARAMS = 29,
          /**< Bit 29 - Invalid Create time extended parameters
          *   @remarks  This error is set when any parameter of struct
          *   IMCTNF_Params is not in allowed range
          */

  IMCTNF_UNSUPPORTED_VIDNF1DYNAMICPARAMS = 30,
          /**< Bit 30 - Invalid base class dyanmic paaremeters during control
          *   @remarks  This error is set when any parameter of struct
          *   IVIDNF1_DynamicParams is not in allowed range
          */

  IMCTNF_UNSUPPORTED_MCTNFDYNAMICPARAMS = 31
          /**< Bit 31 - Invalid exteded class dyanmic paaremeters during control
          *   @remarks  This error is set when any parameter of struct
          *   IMCTNF_DynamicParams (excluding embedded structures) is not in
          *    allowed range
          */

} IMCTNF_ErrorBit ;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* Definition of all the structures define by this interafce   */
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/**<

  @struct IMCTNF_MotionSearchParams
  @brief  This structure contains all the parameters for motion search.
  @param  motionSearchPreset
          This Preset controls the USER_DEFINED vs DEFAULT mode. if User is
          not aware about following fields, it should be set as
          IMCTNF_MOTIONSEARCH_DEFAULT
  @param  searchRangeHor :regarded @ IMCTNF_DynamicParams::motionSearchParams
          Horizontal Search Range
  @param  searchRangeVer :regarded @ IMCTNF_DynamicParams::motionSearchParams
          Vertical Search Range
*/

typedef struct IMCTNF_MotionSearchParams {
  XDAS_Int8   motionSearchPreset;
  XDAS_Int16  searchRangeHor;
  XDAS_Int16  searchRangeVer;
} IMCTNF_MotionSearchParams ;

/**<
  @struct IMCTNF_NoiseFilterParams
  @brief  This structure contains all the parameters controlling the noise filter.
  @param  meEffectivenessTh
  @param  blendingFactorQ4
  @param  minBlendQ4
  @param  maxBlendQ4
  @param  meLambdaType
  @param  meLambdaFactorQ2
  @param  maxLambdaQ2
  @param  sadForMinLambda
  @param  fixWtForCurQ8
  @param  minWtForCurQ8
  @param  biasZeroMotion
  @param  staticMBThZeroMV
  @param  staticMBThNonZeroMV
  @param  blockinessRemFactor
  @param  sadForMaxStrength
  @param  mvXThForStaticLamda
  @param  mvYThForStaticLamda
*/

typedef struct IMCTNF_NoiseFilterParams {
  XDAS_Int32 meEffectivenessTh;
  XDAS_Int32 blendingFactorQ4;
  XDAS_Int32 minBlendQ4;
  XDAS_Int32 maxBlendQ4;
  XDAS_Int32 meLambdaType;
  XDAS_Int32 meLambdaFactorQ2;
  XDAS_Int32 maxLambdaQ2;
  XDAS_Int32 sadForMinLambda;
  XDAS_Int32 fixWtForCurQ8;
  XDAS_Int32 minWtForCurQ8;
  XDAS_Int32 biasZeroMotion;
  XDAS_Int32 staticMBThZeroMV;
  XDAS_Int32 staticMBThNonZeroMV;
  XDAS_Int32 blockinessRemFactor;
  XDAS_Int32 sadForMaxStrength;
  XDAS_Int32 mvXThForStaticLamda;
  XDAS_Int32 mvYThForStaticLamda;
} IMCTNF_NoiseFilterParams;

/**<
  @struct IMCTNF_NoiseFilterStats
  @brief  This structure contains all the noise filter stats.
  @param  avgZeroLumaSadSubBlk
  @param  numMBWithZeroMV
  @param  numStaticMBs
  @param  avgYCbCrMECostSubBlock
  @param  GMVx
  @param  GMVy
  @param  maxBlendPic
  @param  minBlendPic
  @param  filtStrength
  @param  mePassed
  @param  picLambdaQ2
*/
typedef struct IMCTNF_NoiseFilterStats {
  XDAS_Int32 avgZeroLumaSadSubBlk;
  XDAS_Int32 numMBWithZeroMV;
  XDAS_Int32 numStaticMBs;
  XDAS_Int32 avgYCbCrMECostSubBlock;
  XDAS_Int32 GMVx;
  XDAS_Int32 GMVy;
  XDAS_Int32 maxBlendPic;
  XDAS_Int32 minBlendPic;
  XDAS_Int32 filtStrength;
  XDAS_Int32 mePassed;
  XDAS_Int32 picLambdaQ2;
}IMCTNF_NoiseFilterStats;

/**<

  @struct IMCTNF_Params
  @brief This structure defines the Create time parameters for all
         MCTNF objects

  @param  vidnf1Params         must be followed for all video noise filters.
                               Base class create params
  @param  motionSearchParams   Controls all motion search related parameters

  @param  pConstantMemory
          This pointer points to the the memory area where constants are
          located. It has to be in DDR addressable space by vDMA. This is
          use ful to allow relocatable constants for the applications which
          doesn't use M3 as host. Actual memory controller/allocator
          is on another master processor. If this is set to NULL then
          encoder assumes that all constants are pointed by symbol
          MCTNF_TI_ConstData

  @param  debugTraceLevel
          This parameter configures the codec to dump a debug trace log

  @param  lastNFramesToLog
          This parameter configures the codec to maintain a history of last
          N frames/pictures

  @param  enableAnalyticinfo
          This parameter configures the codec to expose analytic info like
          MVs and SAD parameters

*/

typedef struct
{
  IVIDNF1_Params                vidnf1Params;
  IMCTNF_MotionSearchParams     motionSearchParams;

  XDAS_Int32                    pConstantMemory;
  XDAS_UInt32                   debugTraceLevel;
  XDAS_UInt32                   lastNFramesToLog;
  XDAS_Int8                     enableAnalyticinfo;
  XDAS_Int32                    reservedParams[3];
} IMCTNF_Params;


/**<

  @struct IMCTNF_Status
  @brief This structure informs back the status of H264 encoder and tells the
         value of each control parameter

  @param  vidnf1Status        must be followed for all video encoders.
                               Base class status

  @param  motionSearchParams    Controls all Inter coding related parameters
  @param  fieldSelectionType  Controls the type of interlaced coding, refer
                               IMCTNF_FieldSelectionType for more details
  @param  debugTraceLevel
          This parameter configures the codec to dump a debug trace log

  @param  lastNFramesToLog
          This parameter configures the codec to maintain a history of last
          N frames/pictures

  @param  enableAnalyticinfo
          This parameter configures the codec to expose analytic info like
          MVs and SAD parameters

  @param  searchCenter         seacrh Center for motion estimation
  @param  extMemoryDebugTraceAddr  This parameter reports the external
                               memory address (as seen by M3) where debug trace
                               information is being dumped
  @param  extMemoryDebugTraceSize  This parameter reports the external
                               memory buffer size (in bytes) where debug trace
                               information is being dumped
*/

typedef struct
{
  IVIDNF1_Status                 vidnf1Status;
  IMCTNF_MotionSearchParams      motionSearchParams;

  XDAS_UInt32                    debugTraceLevel;
  XDAS_UInt32                    lastNFramesToLog;
  XDAS_Int8                      enableAnalyticinfo;
  XDM_Point                      searchCenter;
  XDAS_UInt32                   *extMemoryDebugTraceAddr;
  XDAS_UInt32                    extMemoryDebugTraceSize;
} IMCTNF_Status;

/**< This structure must be the first field of all MCTNF instance objects */
typedef struct IMCTNF_Obj
{
    struct IMCTNF_Fxns *fxns;
} IMCTNF_Obj;

/**< This handle is used to reference all MCTNF instance objects */
typedef struct IMCTNF_Obj *IMCTNF_Handle;

/**<Default parameter values for MCTNF instance objects */
extern const IMCTNF_Params MCTNF_TI_PARAMS;


/**<

  @struct IMCTNF_DynamicParams
  @brief This structure defines the run time parameters for all MCTNF objects

  @param  vidnf1DynamicParams must be followed for all video encoders
  @param  motionSearchParams    Controls all iner MB coding related parameters.
                               only few are supported to be changed as
                               part @ Control call. Refer motionSearchParams to
                               find out
  @param  searchCenter         seacrh Center for motion estimation.
                               XDM_Point.x == 0xFFFF means ignore searchCenter

  @param  reservedDynParams
          Some part is kept reserved to add parameters later without changing
          the foot print of interface memory

*/

typedef struct IMCTNF_DynamicParams {
  IVIDNF1_DynamicParams       vidnf1DynamicParams;
  IMCTNF_MotionSearchParams   motionSearchParams;
  IMCTNF_NoiseFilterParams    noiseFilterParams;

  XDM_Point                    searchCenter;
  XDAS_Int32                   reservedDynParams[4];
} IMCTNF_DynamicParams;

extern const IMCTNF_DynamicParams MCTNF_TI_DYNAMICPARAMS;

/**<

  @struct IMCTNF_InArgs
  @brief  This structure defines the input arguments passt to MCTNF.

  @params vidnf1InArgs : It is instance of base class. It cntains all
          the necessary  info required run time parameters for all MCTNF
          objects

*/
typedef struct IMCTNF_InArgs {
  IVIDNF1_InArgs  vidnf1InArgs;

} IMCTNF_InArgs;


/**<

  @struct IMCTNF_OutArgs
  @brief  This structure defines the output arguments generated by MCTNF.

  @params vidnf1OutArgs : It is instance of base class. It cntains all
          the necessary info encoder should produce
  @params nfStats: This contains the frame level statistics generated
          at the end of every picture. Refer IMCTNF_NoiseFilterStats
          for more details.
*/
typedef struct IMCTNF_OutArgs {
  IVIDNF1_OutArgs  vidnf1OutArgs;
  IMCTNF_NoiseFilterStats nfStats;
} IMCTNF_OutArgs;

/**<

  @struct IMCTNF_Fxns
  @brief  This structure defines of the operations on MCTNF objects

    @params IVIDNF1_Fxns : It is instance of base class. It contains all
            function table

*/
typedef struct IMCTNF_Fxns
{
    IVIDNF1_Fxns ividnf;
} IMCTNF_Fxns;

#ifdef __cplusplus
}
#endif

/*@}*/ /* ingroup HDVICP2MCTNF */

#endif  //_IMCTNF_H_ //--}

/* ========================================================================*/
/* End of file : imctnf.h                                                  */
/*-------------------------------------------------------------------------*/
/*            Copyright (c) 2013 Texas Instruments, Incorporated.          */
/*                           All Rights Reserved.                          */
/* ========================================================================*/

