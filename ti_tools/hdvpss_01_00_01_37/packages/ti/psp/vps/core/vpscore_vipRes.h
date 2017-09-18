/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPSCORE_VIP_RES_H_
#define _VPSCORE_VIP_RES_H_

/*
  this file defines APIs for VIP resource and path allocator

  Functions in this file allow a driver to allocate paths in the VIP
  depending on the input and output(s) combo that it needs.

  The implementation internally takes of finding a free path and
  allocating it to the driver.

  Mutual exlcusion in API calling is taken care internally

  Allocation can be either in exclusive mode or in shared mode,
  Complete path or portion of the path can be shared across drivers in
  shared mode.

  In shared mode driver needs to make sure that it setups all muxes that
  it needs before using the path.
*/

#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/vpshal_vip.h>
#include <ti/psp/vps/hal/vpshal_sc.h>
#include <ti/psp/vps/fvid2.h>

/*
  define this to enable debug print logs from VIP resource allocator
*/
//#define VCORE_VIP_RES_DEBUG

/*
  Maximum outputs that are possible for a path
*/
#define VCORE_VIP_MAX_OUT_PARAMS        (4)

/*
  Max VPDMA channel associated with a output
*/
#define VCORE_VIP_VPDMA_CH_PER_OUT_MAX  (2)

/*
  VIP resource ID's
*/
typedef enum
{
    /*
     * Port A input
     */
    VCORE_VIP_RES_PARSER_PORT_A,

    /*
     * Port A ancillary output
     */
    VCORE_VIP_RES_PARSER_PORT_A_ANC,

    /*
     * Port B input
     */
    VCORE_VIP_RES_PARSER_PORT_B,

    /*
     * Port B ancillary output
     */
    VCORE_VIP_RES_PARSER_PORT_B_ANC,

    /*
     * Secondary input
     */
    VCORE_VIP_RES_SEC_422,

    /*
     * COMP input
     */
    VCORE_VIP_RES_COMP_RGB,

    /*
     * CSC block
     */
    VCORE_VIP_RES_CSC,

    /*
     * SC block
     */
    VCORE_VIP_RES_SC,

    /*
     * Chroma DS 0 block
     */
    VCORE_VIP_RES_CHR_DS_0,

    /*
     * Chroma DS 1 block
     */
    VCORE_VIP_RES_CHR_DS_1,

    /*
     * Y up output
     */
    VCORE_VIP_RES_Y_UP,

    /*
     * UV up output
     */
    VCORE_VIP_RES_UV_UP,

    /*
     * Y low output
     */
    VCORE_VIP_RES_Y_LOW,

    /*
     * UV low output
     */
    VCORE_VIP_RES_UV_LOW,

    /*
     * Max resource ID
     */
    VCORE_VIP_RES_MAX
} Vcore_VipRes;

/*
  Resource allocation mode
*/
typedef enum
{
    /*
     * Exclusive allocation, one driver will
     * have exclusive access to this resource
     */
    VCORE_VIP_RES_ALLOC_EXCLUSIVE = 0,

    /*
     * Shared allocation, driver can share this resource
     */
    VCORE_VIP_RES_ALLOC_SHARED
} Vcore_VipResAllocMode;

/* Input source */
typedef enum
{
    /*
     * YUV422 Port A 8-bit input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT = 0,

    /*
     * YUV422 Port A 16-bit input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT,

    /*
     * YUV422 Port B 8-bit input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT,

    /*
     * YUV422 Secondary 16-bit input
     */
    VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT,

    /*
     * YUV422 Port A 8-bit multi-channel input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT_MULTI_CH,

    /*
     * YUV422 Port A 16-bit multi-channel input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT_MULTI_CH,

    /*
     * YUV422 Port B 8-bit multi-channel input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT_MULTI_CH,

    /*
     * RGB888 Port A 24-bit input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT,

    /*
     * RGB888 COMP 24-bit input
     */
    VCORE_VIP_IN_SRC_COMP_RGB888_24_BIT,

    /*
     * YUV444 Port A 24-bit input
     */
    VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV444_24_BIT,

    /*
     * Invalid input ID
     */
    VCORE_VIP_IN_SRC_INVALID = -1
} Vcore_VipInSrc;

/* Output formats */
typedef enum
{
    /*
     * RGB888 24-bit
     */
    VCORE_VIP_OUT_FORMAT_RGB888,

    /*
     * YUV444 24-bit
     */
    VCORE_VIP_OUT_FORMAT_YUV444,

    /*
     * YUV422 16-bit
     */
    VCORE_VIP_OUT_FORMAT_YUV422,

    /*
     * YUV420 semi-planar
     */
    VCORE_VIP_OUT_FORMAT_YUV420,

    /*
     * 16/8-bit Ancillary data
     */
    VCORE_VIP_OUT_FORMAT_ANC,

    /* YUV422 semi-planar */
    VCORE_VIP_OUT_FORMAT_YUV422SP_UV,

    /*
     * Max output format ID
     */
    VCORE_VIP_OUT_FORMAT_MAX
} Vcore_VipOutFormat;

/*
  Output type parameters
*/
typedef struct
{
    /*
     * Output format
     */
    Vcore_VipOutFormat format;

    /*
     * TRUE: scaler is applied on the output,
     * FALSE: scaler is not applied on the output
     */
    UInt32 scEnable;

} Vcore_VipOutParams;

/*
  Resource allocation parameters supplied by driver
*/
typedef struct
{

    /*
     * Driver name, driver needs to make sure the
     * string data is global or static allocation
     */
    char *drvName;

    /*
     * Path allocation mode, exclusive or shared
     */
    Vcore_VipResAllocMode openMode;

    /*
     * Driver ID used for this path
     * Ignored for exclusive mode,
     *
     * For shared mode, a resource allocation is successfull only
     * in the below cases
     *
     * 1. Resource is not already allocated
     * 2. Resource is allocated but in shared mode
     * AND also to the same Driver ID as the current requesting
     * driver ID
     */
    UInt32 drvId;

    /*
     * VIP instance from which to allocate
     */
    Int16 vipInstId;

    /*
     * Input source type
     */
    Vcore_VipInSrc inSrc;

    /*
     * VPDMA channel offset. Used to add an offset to start channel mapping.
     * This will be used when the decoder start channel ID is other than 0.
     * For example some decoder's channel 0 CHID starts from 4 instead of 0.
     * Valid only in multi-channel mode.
     */
    UInt32 muxModeStartChId;

    /*
     * Number of outputs that are needed in the path
     */
    UInt16 numOutParams;

    /*
     * output format information
     */
    Vcore_VipOutParams outParams[VCORE_VIP_MAX_OUT_PARAMS];

} Vcore_VipResAllocParams;

/*
  Mux settings that need to applied by driver

  Calculated by alloc API but not applied.
  Driver needs to apply at appropiate point.

  Utility API Vcore_vipSetMux() can used for applying the muxes

  If a value is -1 for the mux values below then it means
  that this mux should not be touched for this path.

  It may be in use by some other path
*/
typedef struct
{
    /*
     * CSC source mux setting
     */
    Int16 cscSrc;

    /*
     * SC source mux setting
     */
    Int16 scSrc;

    /*
     * Chroma DS 0/1 source mux setting
     */
    Int16 chrDsSrc[2];

    /*
     * RGB source mux setting
     */
    Int16 rgbSrc;

    /*
     * Chroma DS 0/1 bypass ON/OFF
     */
    Int16 chrDsBypass[2];

    /*
     * RGB HI out ON/OFF
     */
    Int16 rgbOutHiSel;

    /*
     * RGB LO out ON/OFF
     */
    Int16 rgbOutLoSel;

    /*
     * Multi-channel out ON/OFF
     */
    Int16 multiChSel;

} Vcore_VipMuxInfo;

/*
  Handles associated with this path

  Alloc API assigns valid handles to resources used in this path.

  If a handle is NULL then that means this resource
  should NOT be used in this path
*/
typedef struct
{
    /*
     * VIP Parser handle
     */
    VpsHal_VipHandle vipHalHandle;

    /*
     * SC handle
     */
    VpsHal_Handle scHalHandle;

    /*
     * CSC handle
     */
    VpsHal_Handle cscHalHandle;

} Vcore_VipResInfo;

/*
  VPDMA info associated with output in this path

  Alloc APIs calculates the VPDMA channels that are requried
  Driver can used this when making output descriptors for this path
*/
typedef struct
{
    /*
     * TRUE: Multi-channel mode, vpdmaChId[] has the base CH0 VPDMA channel ID
     * FALSE: Non-multi channel mode, vpdmaChId[] has the required VPDMA channel ID
     */
    UInt32 isMultiCh;

    /*
     * Number of VPDMA channels associated with this output
     */
    UInt16 vpdmaChNum;

    /*
     * VPDMA channel ID
     */
    VpsHal_VpdmaChannel vpdmaChId[VCORE_VIP_VPDMA_CH_PER_OUT_MAX];

    /*
     * VPDMA data type associated with this channel ID
     */
    VpsHal_VpdmaChanDT vpdmaChDataType[VCORE_VIP_VPDMA_CH_PER_OUT_MAX];

} Vcore_VipVpdmaInfo;

/*
  Resource information
*/
typedef struct
{
    /*
     * Driver that has allocated this resource
     */
    char *drvName;

    /*
     * resource ID
     */
    UInt16 resId;

    /*
     * Is resource allocated
     */
    UInt32 isAlloc;

    /*
     * Is resource in shared or exclusive mode
     */
    UInt32 isShared;

    /*
     * Number of drivers sharing this resource
     */
    Int16 allocCnt;

    /*
     * Driver ID associated with this resource
     */
    UInt32 drvId;

} Vcore_VipResEntry;

/*
  Resource Path Object

  For a given path,
  - has original allocate API params
  - Has information about all allocated resources
  - Has HAL handles associated with this path
  - Has Mux settings associated with this path
  - Has VPDMA channel info associated with outputs in this path
  - Has auto-calculated CSC settings that need to applied for this path
*/
typedef struct
{
    /*
     * Original allocate API parameters
     */
    Vcore_VipResAllocParams resParams;

    /*
     * resource allocation table
     */
    Vcore_VipResEntry resTable[VCORE_VIP_RES_MAX];

    /*
     * HAL handles
     */
    Vcore_VipResInfo resInfo;

    /*
     * Mux settings
     */
    Vcore_VipMuxInfo muxInfo;

    /*
     * VPDMA info for all associated outputs
     */
    Vcore_VipVpdmaInfo vpdmaInfo[VCORE_VIP_MAX_OUT_PARAMS];

    /*
     * auto-calculated CSC config
     */
    Vps_CscConfig cscConfig;

} Vcore_VipResObj;

/* functions  */

/* Init resource allocator

  Called as part of FVID2_init()
*/
Int32 Vcore_vipResInit (  );

/* De-init resource allocator

  Called as part of FVID2_deInit()
*/
Int32 Vcore_vipResDeInit (  );

/* Allocate a path

  pObj [OUT] Resource path object initialized by the API
  pParams [IN] Path parameters

  FVID2_SOK on sucessful path allocation, else failure
*/
Int32 Vcore_vipResAllocPath ( Vcore_VipResObj * pObj,
                              Vcore_VipResAllocParams * pParams );

/* Free a path

  For exclusive mode paths,
  it also resets the mux to original state for muxes that fall in this path

  pObj  [IN] Path to free
*/
Int32 Vcore_vipResFreePath ( Vcore_VipResObj * pObj );

/*
  Create config Overlay for all the multiplexers used in this path

  pObj [IN] Path object
  configOvlyPtr [IN] Overlay Pointer in which overlay is created

  Only applies those muxes which fall in the allocated path
  Will not touch other muxes
*/
Int32 Vcore_vipCreateConfigOvly(Vcore_VipResObj *pObj, Ptr configOvlyPtr);

/*
  Get config overlay size for all the multiplexers used in this path

  pObj [IN] Path object

  Only applies those muxes which fall in the allocated path
  Will not touch other muxes
*/
UInt32 Vcore_vipGetConfigOvlySize(Vcore_VipResObj *pObj);


/*
  Apply Mux settings for this path

  pObj [IN] Path object
  configOvlyPtr [IN] Overlay Pointer in which virtual registers are set
                     set to NULL to set value directly to the register

  Only applies those muxes which fall in the allocated path
  Will not touch other muxes
*/
Int32 Vcore_vipSetMux(Vcore_VipResObj *pObj, Ptr configOvlyPtr);

/*
  Set VIP parser params for allocated parser

  pObj       [IN]  Path object
  vipConfig  [IN] VIP instance config
  portConfig [IN] VIP port config
*/
Int32 Vcore_vipSetParserParams ( Vcore_VipResObj * pObj,
                                 VpsHal_VipConfig * vipConfig,
                                 VpsHal_VipPortConfig * portConfig );

/*
  Apply CSC parameters

  Parameters are applied only if CSC is in the allocated path

  pObj        [IN] Path object
  cscConfig   [IN] CSC settings, if NULL, then apply auto-calculated settings
*/
Int32 Vcore_vipSetCscParams ( Vcore_VipResObj * pObj,
                              Vps_CscConfig * cscConfig );

/*
  Apply scaler parameters

  Parameters are applied only if SC is in the allocated path

  pObj        [IN] Path object
  scConfig    [IN] scaler config
*/
Int32 Vcore_vipSetScParams ( Vcore_VipResObj * pObj,
                             VpsHal_ScConfig * scConfig );

/*
  Create coeff in coeff memory

  Coeffs created only if SC is in the allocated path
  This function is called from the Capture driver for setting the scalar
  coefficients into provided coefficient memory.

  pObj        [IN] Path object
  scCoeff     [IN] scaler coeff params
  scCoeffMem  [IN] scaler coeff memory
*/
Int32 Vcore_vipSetScCoeff ( Vcore_VipResObj * pObj,
                            Vps_ScCoeffParams * scCoeff, Ptr scCoeffMem );

/**
 *  Returns information about the scaling factor configuration for a
 *  given channel. Also updates the registr overlay based on the calculated
 *  scaling factor.
 *
 *  pObj           [IN] Pointer to VIP core resource object
 *  scConfig       [IN] Pointer to scalar HAL configuration
 *  scFactorConfig [IN] Pointer to the scaling factor configuration structure
 *                      in which the information is to be returned.
 *                      This parameter should be non-NULL.
 */
Int32 Vcore_vipGetAndUpdScFactorConfig(Vcore_VipResObj * pObj,
                                       VpsHal_ScConfig * scConfig,
                                       VpsHal_ScFactorConfig *scFactorConfig);

/*
  Return true if scaler is allocated in the path, else false
*/
UInt32 Vcore_vipIsScAlloc ( Vcore_VipResObj * pObj );

/*
  Enable/Disable VIP port

  Port that is associated with this path is enabled or disabled

  pObj  [IN] Path object
  enable  [IN] TRUE: enable, FALSE: disable
*/
Int32 Vcore_vipEnablePort ( Vcore_VipResObj * pObj, UInt32 enable );

/*
  Reset/Remove out of reset VIP port

  Port that is associated with this path is put into software reset or pull out
  out of software reset

  pObj  [IN] Path object
  enable  [IN] TRUE: enable, FALSE: disable
*/
Int32 Vcore_vipResetPort ( Vcore_VipResObj * pObj, UInt32 enable );

/*
  Reset/Remove out of reset CSC, if being used

  CSC that is associated with this path is put into software reset or pulled out
  of software reset

  pObj  [IN] Path object
  vipInst [IN] Instance of VIP
  enable  [IN] TRUE: enable, FALSE: disable
*/
Int32 Vcore_vipResetCsc ( Vcore_VipResObj * pObj, VpsHal_VpsVipInst vipInst,
                          UInt32 enable );

/*
  Reset/Remove out of reset SC, if being used

  SC that is associated with this path is put into software reset or pulled out
  of software reset

  pObj  [IN] Path object
  vipInst [IN] Instance of VIP
  enable  [IN] TRUE: enable, FALSE: disable
*/
Int32 Vcore_vipResetSc  ( Vcore_VipResObj * pObj, VpsHal_VpsVipInst vipInst,
                          UInt32 enable );

/*
  Reset/Remove out of reset CHR DS, if being used

  CHR DS that is associated with this path is put into software reset or
  pulled out of software reset

  pObj  [IN] Path object
  vipInst [IN] Instance of VIP
  enable  [IN] TRUE: enable, FALSE: disable
*/
Int32 Vcore_vipResetChrds ( Vcore_VipResObj * pObj, VpsHal_VpsVipInst vipInst,
                            UInt32 enable );

/*
  Prints all resource allocation status to stdout
*/
Int32 Vcore_vipResPrintAllStatus (  );

/*
  Enable/Disable debug log during path alloc and free
*/
int Vcore_vipResDebugLogEnable ( int enable );

/*
 *  Function to decide weather swapping of resource
 *  allocation will help
 */
Bool Vcore_vipResIsSwapReq(Vcore_VipResAllocParams * resParams);

Int32 Vcore_vipRestoreMux ( Vcore_VipResObj * pObj );

UInt32 Vcore_vipGetPortCompleteStatus(Vcore_VipResObj *pObj);
Void Vcore_vipClearPortCompleteStatus(Vcore_VipResObj *pObj);

Int32 Vcore_vipResetModules(Vcore_VipResObj *pObj, UInt32 enable);

Int32 Vcore_vipSetVipActCropCfg(Vcore_VipResObj *pObj,
                                const VpsHal_VipCropConfig *vipHalCropCfg);
Int32 Vcore_vipGetVipActCropCfg(Vcore_VipResObj *pObj,
                                VpsHal_VipCropConfig *vipHalCropCfg);

#endif /*  _VPSCORE_VIP_RES_H_  */
