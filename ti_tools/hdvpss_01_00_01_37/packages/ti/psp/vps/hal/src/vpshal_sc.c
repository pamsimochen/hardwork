/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_sc.c
 *
 * \brief Scalar HAL file
 * This file implements HAL for Scalar
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/vps_cfgSc.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_sc.h>
#include <ti/psp/cslr/cslr_vps_sc_h.h>
#include <ti/psp/vps/hal/src/vpshalScDefaults.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/**
 * \brief Minumim values for height and width for Scalar module
 */
#define SC_MIN_REQ_WIDTH            (34u)
#define SC_MIN_REQ_HEIGHT           (8u)

/**
 *  \brief Maximum Values for Luma and Chroma Threshold
 */
#define SC_DELTA_LUMA_THR_MAX       (8u)
#define SC_DELTA_CHROMA_THR_MAX     (8u)

/**
 *  \brief Macros to check whether given value is within range. c is checked
 *  within range from a to b
 */
#define SC_RANGECHECK(a, b, c)      ((a <= c) && (c <= b))

#define SC_HORZ_MULTIPLIER          (1024u)
#define SC_LINEAR_ACC_INC_SHIFT     (24u)
#define SC_NONLINEAR_ACC_INC_SHIFT  (24u)
#define SC_ROW_ACC_INC_SHIFT        (16u)
#define SC_MAX_OUTPUT_WIDTH         (2014u)
#define SC_HORZ_SCALE_SHIFT         (10u)
#define SC_VERT_RAV_INIT_MAX        (1024u)

/**
 *  \brief Maximum Values supported for sizes
 */
#define SC_MAX_TARGET_HEIGHT        ((CSL_VPS_SC_H_CFG_SC4_CFG_TAR_H_MASK >>  \
                                        CSL_VPS_SC_H_CFG_SC4_CFG_TAR_H_SHIFT) \
                                        + 1u)
#define SC_MAX_SOURCE_WIDTH         ((CSL_VPS_SC_H_CFG_SC24_CFG_ORG_W_MASK >> \
                                        CSL_VPS_SC_H_CFG_SC24_CFG_ORG_W_SHIFT)\
                                        + 1u)
#define SC_MAX_SOURCE_HEIGHT        ((CSL_VPS_SC_H_CFG_SC24_CFG_ORG_H_MASK >> \
                                        CSL_VPS_SC_H_CFG_SC24_CFG_ORG_H_SHIFT)\
                                        + 1u)
#define SC_MAX_CROP_WIDTH           ((CSL_VPS_SC_H_CFG_SC5_CFG_SRC_W_MASK >>  \
                                        CSL_VPS_SC_H_CFG_SC5_CFG_SRC_W_SHIFT) \
                                        + 1u)
#define SC_MAX_CROP_HEIGHT          ((CSL_VPS_SC_H_CFG_SC5_CFG_SRC_H_MASK >>  \
                                        CSL_VPS_SC_H_CFG_SC5_CFG_SRC_H_SHIFT) \
                                        + 1u)

/**
 *  \brief Masks and shifts used to extract upper 3 bits of lengths for 35
 *  division
 */
#define SC_HORZ_HIGH_VALUE_MASK     (0x00000700u)
#define SC_HORZ_HIGH_VALUE_SHIFT    (8u)

/**
 *  \brief Minimum Horizontal Ratio supported is 1/16x
 */
#define SC_ISVALID_HORZ_RATIO(a, b) ((a >> 4u) <= b)

#define SC_BILIEAR_NUM_TAPS             (7u)
#define SC_HORZ_NUM_TAPS                (7u)
#define SC_VERT_NUM_TAPS                (5u)
#define SC_COEFF_SCALE_SHIFT            (4u)
#define SC_COEFF_MASK                   (0x1FFF)
#define SC_SINGLE_PHASE_NUM_COEFF       (0x8u)

#define SC_COEFF_SET_SIZE               (VPSHAL_SC_NUM_PHASES * VPSHAL_SC_COEFF_ALIGN)

#define SC_USE_RAJITHA_METHOD

#define regw(reg, val)      ((*(volatile UInt32 *)(reg)) = (val))
#define regr(reg)           (*(volatile UInt32 *)(reg))

/**
 *  enum Sc_ConfigOvlyReg
 *  \brief Enums for the configuration overlay register offsets - used as array
 *  indices to register offset array in ScInfo object. These enums defines the
 *  registers that will be used to form the configuration register overlay for
 *  DEIH module. This in turn is used by client drivers to configure registers
 *  using VPDMA during context-switching (software channels with different
 *  configurations).
 *
 *  Note: If in future it is decided to remove or add some register, this enum
 *  should be modified accordingly. And make sure that the values of these
 *  enums are in ascending order and it starts from 0.
 */
typedef enum
{
    SC_CONFIG_OVLY_CFG_SC1_IDX = 0,
    SC_CONFIG_OVLY_CFG_SC2_IDX,
    SC_CONFIG_OVLY_CFG_SC3_IDX,
    SC_CONFIG_OVLY_CFG_SC4_IDX,
    SC_CONFIG_OVLY_CFG_SC5_IDX,
    SC_CONFIG_OVLY_CFG_SC6_IDX,
    SC_CONFIG_OVLY_CFG_SC8_IDX,
    SC_CONFIG_OVLY_CFG_SC9_IDX,
    SC_CONFIG_OVLY_CFG_SC10_IDX,
    SC_CONFIG_OVLY_CFG_SC11_IDX,
    SC_CONFIG_OVLY_CFG_SC12_IDX,
    SC_CONFIG_OVLY_CFG_SC13_IDX,
    SC_CONFIG_OVLY_CFG_SC18_IDX,
    SC_CONFIG_OVLY_CFG_SC24_IDX,
    SC_CONFIG_OVLY_CFG_SC25_IDX,
    SC_CONFIG_OVLY_CFG_SC0_IDX,
    SC_CONFIG_OVLY_NUM_IDX
    /**< This must be the last enum */
} Sc_ConfigOvlyReg;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Sc_ScalarInfo
 *  \brief There are five instances of scalar in VPS. This structure is
 *  used to store instance specific information.
 *
 */
typedef struct
{
    UInt32                  instId;
    /**< Instance ID. */
    CSL_VpsSchRegsOvly      regs;
    /**< Pointer to the Register overlay structure */
    VpsHal_VpdmaConfigDest  coeffConfigDest;
    /**< VPDMA configuration register destination for the Scalar instance. */
    UInt32                  isHqScalar;
    /**< Type of scalar - high quality or normal quality. */
    UInt32                 *userScCoeff;
    /**< User coefficients (if provided) */
    UInt32                  openCnt;
    /**< Keeps track of number of open for a Csc instance */
    UInt32                  virtRegOffset[(UInt32)SC_CONFIG_OVLY_NUM_IDX];
    /**< Array of indexes into the Overlay memory where virtual registers are
         to be configured */
    UInt32                 *regOffset[(UInt32)SC_CONFIG_OVLY_NUM_IDX];
    /**< Array of physical address of the register configured through
         VPDMA register overlay. */
    UInt32                  regOvlySize;
    /**< Virtual register overlay size in terms of bytes. Used to allocate
         memory for virtual register configured through VPDMA */
} Sc_ScalarInfo;

/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

static inline Int scIsValidScalar(VpsHal_Handle handle);
static Int scSetPeakingParams(Sc_ScalarInfo *scInfo,
                              Vps_ScPeakingConfig *peakParams);
static Int scSetEdgeDetectParams(Sc_ScalarInfo *scInfo,
                                 Vps_ScEdgeDetectConfig *edParams);
static inline Void scCalVertRavParams(const VpsHal_ScConfig *config,
                                      UInt32 *factor,
                                      UInt32 *ravInit);
static inline Void scCalVertPolyphaseParams(const VpsHal_ScConfig *config,
                                            UInt32 *rowAccInc,
                                            UInt32 *offsetA,
                                            UInt32 *offsetB);
static inline UInt32 scCalHorzLinearAccInc(const VpsHal_ScConfig *config,
                                           UInt32 *shift);
static inline UInt32 scCalHorzColAccOffset(const VpsHal_ScConfig *config);
static inline Void scCalHorzNonLinearParams(const VpsHal_ScConfig *config,
                                            UInt32 shift,
                                            Int32 *initValue,
                                            Int32 *incValue);
static inline UInt32 scMakeScCfg24Reg(const VpsHal_ScConfig *config);
static inline UInt32 scMakeScCfg4Reg(const VpsHal_ScConfig *config);
static inline UInt32 scMakeScCfg5Reg(const VpsHal_ScConfig *config);
static inline UInt32 scMakeScCfg25Reg(const VpsHal_ScConfig *config);
static inline UInt32 scMakeScCfg8Reg(const VpsHal_ScConfig *config);
static Int scCheckParams(VpsHal_Handle handle, const VpsHal_ScConfig *config);
static Void scSetScaleRatioParams(Sc_ScalarInfo *scInfo,
                                  const VpsHal_ScConfig *parmas);
static UInt32 scLongDivide(UInt32 a, UInt32 b, UInt32 c);
static Void scGetStdHorzCoeffOvly(VpsHal_Handle handle,
                                  const VpsHal_ScCoeffConfig *config,
                                  Ptr *horzCoeffMemPtr);
static Void scGetStdVertCoeffOvly(VpsHal_Handle hande,
                                  const VpsHal_ScCoeffConfig *config,
                                  Ptr *vertCoeffMemPtr,
                                  Ptr *vertBilinearCoeffMemPtr);
static Int scGetPeakingParams(Sc_ScalarInfo *scInfo,
                              Vps_ScPeakingConfig *pParams);
static Int scGetEdgeDetectParams(Sc_ScalarInfo *scInfo,
                                 Vps_ScEdgeDetectConfig *edParams);

/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */

Sc_ScalarInfo ScInfo[VPSHAL_SC_MAX_INST];

/**
 *  \brief Scaling Coefficient Memory Pointers
 */
static UInt8 *gScVertBilinearCoeffMemPtr = NULL;
static UInt8 *gScVertUpScaleCoeffMemPtr = NULL;
static UInt8 *gScVertOneToOneScaleCoeffMemPtr = NULL;
static UInt8 *gScVertDownScaleCoeffMemPtr = NULL;
static UInt8 *gScHorzUpScaleCoeffMemPtr = NULL;
static UInt8 *gScHorzDownScaleCoeffMemPtr = NULL;
static UInt8 *gScHorzOneToOneScaleCoeffMemPtr = NULL;
static Vps_ScCoeff *gUserScCoeff = NULL;

/**
 *  \brief Scaling Coefficient Arrays used for initialization
 */
static const UInt16 ScVertBilinearCoeff[2][32][8] =
                            SC_VERTICAL_BILINEAR_COEFF;
static const UInt16 ScVertUpScaleCoeff[2][32][8] =
                            SC_VERTICAL_POLYPHASE_UP_SCALE_COEFF;
static const UInt16 ScVertOneToOneScaleCoeff[2][32][8] =
                            SC_VERTICAL_POLYPHASE_1_1_SCALE_COEFF;
static const UInt16 ScVertDownScaleCoeff[13][2][32][8] =
                            SC_VERTICAL_POLYPHASE_DOWN_SCALE_COEFF;
static const UInt16 ScHorzUpScaleCoeff[2][32][8] =
                            SC_HORIZONTAL_POLYPHASE_UP_SCALE_COEFF;
static const UInt16 ScHorzDownScaleCoeff[13][2][32][8] =
                            SC_HORIZONTAL_POLYPHASE_DOWN_SCALE_COEFF;
static const UInt16 ScHorzOneToOneScaleCoeff[2][32][8] =
                            SC_HORIZONTAL_POLYPHASE_1_1_SCALE_COEFF;

/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */

/**
 *  VpsHal_scInit
 *  \brief Function to initialize scalar driver. It initializes global
 *  structure scInfo, sets the peaking and edge detect parameters
 *  in the registers and gets the VPDMA register overlay size and
 *  virtual register offsets in the overlay from the VPDMA. Since
 *  VPDMA register overlay size and virtual register offsets are
 *  non-changing and same for all scalars, these are set at initial
 *  time.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              For the Future use, not used currently.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_scInit(UInt32 numInstances,
                  const VpsHal_ScInstParams *initParams,
                  Ptr arg)
{
    Int32                       retVal = VPS_SOK;
    UInt32                        cnt, hSize, vSize, bSize;
    Vps_ScPeakingConfig        peak = SC_EXPERT_VAL_PEAKING;
    Vps_ScEdgeDetectConfig     edParams = SC_EXPERT_VAL_EDGEDETECT;
    CSL_VpsSchRegsOvly            scRegs = NULL;
    Sc_ScalarInfo                *scInfo = NULL;
    UInt8                        *tempPtr, *descMem;

    /* Check for errors */
    GT_assert( VpsHalTrace, (numInstances <= VPSHAL_SC_MAX_INST));
    GT_assert( VpsHalTrace, (initParams != NULL));

    /* Set the default Values for non-changing parameters */
    for (cnt = 0u; cnt < numInstances; cnt++)
    {
        GT_assert( VpsHalTrace, (initParams[cnt].instId < VPSHAL_SC_MAX_INST));

        scInfo = &(ScInfo[cnt]);
        scInfo->openCnt = 0u;
        scInfo->instId = initParams[cnt].instId;
        scInfo->regs = (CSL_VpsSchRegsOvly )initParams[cnt].baseAddress;
        scInfo->coeffConfigDest = initParams[cnt].coeffConfigDest;
        scInfo->isHqScalar = initParams[cnt].isHqScalar;
        scInfo->userScCoeff = NULL; /* NULL till provided by user. */
        scSetPeakingParams(scInfo, &peak);
        if (TRUE == scInfo->isHqScalar)
        {
            scSetEdgeDetectParams(scInfo, &edParams);
        }

        /* Calculate VPDMA register overlay size and register offset in the
         * VPDMA
         * register overlay. Since register organization for all scalar are
         * same,
         * only one array of register offset is sufficient. */
        /* Since the register offset are same for all scalar instances,
         * using HQ
         * scalar to get physical offset. */
        scRegs = scInfo->regs;
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC0_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC0);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC1_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC1);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC2_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC2);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC3_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC3);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC4_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC4);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC5_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC5);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC6_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC6);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC8_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC8);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC9_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC9);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC10_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC10);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC11_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC11);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC12_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC12);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC13_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC13);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC18_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC18);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC24_IDX] =
                                       (UInt32 *) &(scRegs->CFG_SC24);
        scInfo->regOffset[SC_CONFIG_OVLY_CFG_SC25_IDX] =
                                                (UInt32 *) &(scRegs->CFG_SC25);

        scInfo->regOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                scInfo->regOffset,
                                SC_CONFIG_OVLY_NUM_IDX,
                                scInfo->virtRegOffset);
    }

    /* Allocate coefficient memory */
    gScVertBilinearCoeffMemPtr = VpsUtils_allocDescMem(
                                     sizeof (ScVertBilinearCoeff),
                                     VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gScVertUpScaleCoeffMemPtr = VpsUtils_allocDescMem(
                                    sizeof (ScVertUpScaleCoeff),
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gScVertOneToOneScaleCoeffMemPtr = VpsUtils_allocDescMem(
                                    sizeof (ScVertOneToOneScaleCoeff),
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gScVertDownScaleCoeffMemPtr = VpsUtils_allocDescMem(
                                      sizeof (ScVertDownScaleCoeff),
                                      VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gScHorzUpScaleCoeffMemPtr = VpsUtils_allocDescMem(
                                    sizeof (ScHorzUpScaleCoeff),
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gScHorzDownScaleCoeffMemPtr = VpsUtils_allocDescMem(
                                      sizeof (ScHorzDownScaleCoeff),
                                      VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gScHorzOneToOneScaleCoeffMemPtr = VpsUtils_allocDescMem(
                                    sizeof (ScHorzOneToOneScaleCoeff),
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gUserScCoeff = VpsUtils_allocDescMem(
                       (sizeof (Vps_ScCoeff)),
                       VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    if ((NULL == gScVertBilinearCoeffMemPtr) ||
        (NULL == gScVertUpScaleCoeffMemPtr) ||
        (NULL == gScVertOneToOneScaleCoeffMemPtr) ||
        (NULL == gScVertDownScaleCoeffMemPtr) ||
        (NULL == gScHorzUpScaleCoeffMemPtr) ||
        (NULL == gScHorzDownScaleCoeffMemPtr) ||
        (NULL == gScHorzOneToOneScaleCoeffMemPtr) ||
        (NULL == gUserScCoeff))
    {
        retVal = VPS_EALLOC;
        VpsHal_scDeInit(NULL);
    }
    else
    {
        /* Init the coeff memories */
        VpsUtils_memcpy(
            gScVertBilinearCoeffMemPtr,
            ScVertBilinearCoeff,
            sizeof (ScVertBilinearCoeff));
        VpsUtils_memcpy(
            gScVertUpScaleCoeffMemPtr,
            ScVertUpScaleCoeff,
            sizeof (ScVertUpScaleCoeff));
        VpsUtils_memcpy(
            gScVertOneToOneScaleCoeffMemPtr,
            ScVertOneToOneScaleCoeff,
            sizeof (ScVertOneToOneScaleCoeff));
        VpsUtils_memcpy(
            gScVertDownScaleCoeffMemPtr,
            ScVertDownScaleCoeff,
            sizeof (ScVertDownScaleCoeff));
        VpsUtils_memcpy(
            gScHorzUpScaleCoeffMemPtr,
            ScHorzUpScaleCoeff,
            sizeof (ScHorzUpScaleCoeff));
        VpsUtils_memcpy(
            gScHorzDownScaleCoeffMemPtr,
            ScHorzDownScaleCoeff,
            sizeof (ScHorzDownScaleCoeff));
        VpsUtils_memcpy(
            gScHorzOneToOneScaleCoeffMemPtr,
            ScHorzOneToOneScaleCoeff,
            sizeof (ScHorzOneToOneScaleCoeff));
    }

    for (cnt = 0u; cnt < numInstances; cnt++)
    {
        GT_assert( VpsHalTrace, (initParams[cnt].instId < VPSHAL_SC_MAX_INST));

        scInfo = &(ScInfo[cnt]);

        /* Load Default Coefficients */
        descMem = VpsUtils_allocDescMem(
                      VPSHAL_VPDMA_CONFIG_DESC_SIZE * 2,
                      VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == descMem)
        {
            GT_0trace(VpsHalTrace, GT_ERR, "Desc memory alloc failed!!\n");
            retVal = VPS_EALLOC;
        }

        VpsHal_scGetCoeffOvlySize(scInfo, &hSize, &vSize, &bSize);
        tempPtr = descMem;
        /* Create the configuration descriptor for
           Coefficients Configuration */
        VpsHal_vpdmaCreateConfigDesc(
            tempPtr,
            scInfo->coeffConfigDest,
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            hSize,
            (gScHorzUpScaleCoeffMemPtr),
            NULL,
            0);

        tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

        VpsHal_vpdmaCreateConfigDesc(
            tempPtr,
            scInfo->coeffConfigDest,
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            vSize,
            (gScVertUpScaleCoeffMemPtr),
            (Ptr) (hSize),
            hSize);

        VpsHal_vpdmaPostList(0, VPSHAL_VPDMA_LT_NORMAL, descMem, VPSHAL_VPDMA_CONFIG_DESC_SIZE*2, 1);

        VpsHal_vpdmaListWaitComplete(
                0,
                10);

        VpsUtils_freeDescMem(
            descMem,
            VPSHAL_VPDMA_CONFIG_DESC_SIZE * 2);
    }
    return (retVal);
}



/**
 *  VpsHal_scDeInit
 *  \brief Function to de-initialize scalar driver.
 *
 *  \param arg             For the Future use, not used currently.
 *
 *  \return                Returns 0 on success else returns error value
 */
Int VpsHal_scDeInit(Ptr arg)
{
    UInt32 cnt;

    for (cnt = 0u; cnt < VPSHAL_SC_MAX_INST; cnt++)
    {
        ScInfo[cnt].regs = 0u;
    }

    if (NULL != gScVertBilinearCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScVertBilinearCoeffMemPtr,
            sizeof (ScVertBilinearCoeff));
        gScVertBilinearCoeffMemPtr = NULL;
    }

    if (NULL != gScVertUpScaleCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScVertUpScaleCoeffMemPtr,
            sizeof (ScVertUpScaleCoeff));
        gScVertUpScaleCoeffMemPtr = NULL;
    }

    if (NULL != gScVertOneToOneScaleCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScVertOneToOneScaleCoeffMemPtr,
            sizeof (ScVertOneToOneScaleCoeff));
        gScVertOneToOneScaleCoeffMemPtr = NULL;
    }

    if (NULL != gScVertDownScaleCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScVertDownScaleCoeffMemPtr,
            sizeof (ScVertDownScaleCoeff));
        gScVertDownScaleCoeffMemPtr = NULL;
    }

    if (NULL != gScHorzUpScaleCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScHorzUpScaleCoeffMemPtr,
            sizeof (ScHorzUpScaleCoeff));
        gScHorzUpScaleCoeffMemPtr = NULL;
    }

    if (NULL != gScHorzDownScaleCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScHorzDownScaleCoeffMemPtr,
            sizeof (ScHorzDownScaleCoeff));
        gScHorzDownScaleCoeffMemPtr = NULL;
    }

    if (NULL != gScHorzOneToOneScaleCoeffMemPtr)
    {
        VpsUtils_freeDescMem(
            gScHorzOneToOneScaleCoeffMemPtr,
            sizeof (ScHorzOneToOneScaleCoeff));
        gScHorzOneToOneScaleCoeffMemPtr = NULL;
    }

    if (NULL != gUserScCoeff)
    {
        VpsUtils_freeDescMem(gUserScCoeff, sizeof (Vps_ScCoeff));
        gUserScCoeff = NULL;
    }

    return (0);
}


/**
 *  VpsHal_scOpen
 *  \brief Function to request the handle to the scalar. This function must
 *  be called before using any scalar. Handle returned from this
 *  function must be used in all other functions to set/get parameters
 *  correctly.
 *
 *  \param scalar                Instance of the scalar to be opened
 *
 *  \return return               Handle of scalar on success. NULL on error
 */
VpsHal_Handle VpsHal_scOpen(UInt32 scalar)
{
    UInt32 cnt, cookie;
    VpsHal_Handle handle = NULL;

    for (cnt = 0u; cnt < VPSHAL_SC_MAX_INST; cnt++)
    {
        if (scalar == ScInfo[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            handle = &(ScInfo[cnt]);

            /* Check whether some one has already opened this instance */
            if (0 == ScInfo[cnt].openCnt)
            {
                ScInfo[cnt].openCnt += 1u;
            }

            /* Enable global Interrupts */
            Hwi_restore(cookie);

            break;
        }
    }

    return (handle);
}



/**
 *  VpsHal_scClose
 *  \brief Function to close/release the scalar handle. Currently it does
 *  nothing other than decrementing reference count.
 *
 *  \param handle           Scalar Handle
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scClose(VpsHal_Handle handle)
{
    Int ret = -1;
    UInt32 cookie;
    Sc_ScalarInfo *scInfo = (Sc_ScalarInfo *)handle;

    GT_assert( VpsHalTrace, (0 == scIsValidScalar(handle)));
    GT_assert( VpsHalTrace, (NULL != scInfo));

    /* Disable Hardware Interrupts */
    cookie = Hwi_disable();
    if(scInfo->openCnt > 0u)
    {
        scInfo->openCnt -= 1u;
        ret = 0;
    }
    /* Enable Hardware Interrupts */
    Hwi_restore(cookie);

    return (ret);
}



/**
 *  VpsHal_scSetConfig
 *  \brief This function is used to set the scalar parameters in appropriate
 *  scalar register or in the virtual registers created in the register
 *  overlay whose address is passed as the last argument. If last
 *  argument is null, it will set the parameters in registers otherwise
 *  it will set the parameters in the VPDMA register overlay.
 *
 *  \param handle           Scalar Handle
 *  \param config           pointers to structure containing scalar parameters
 *  \param ovlyPtr          Pointer to memory containing VPDMA register
 *                          overlay for scalar. It can be set to memory
 *                          pointer to program register in overlay or set
 *                          to null to program directly in hardware register
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scSetConfig(VpsHal_Handle handle,
                       const VpsHal_ScConfig *config,
                       Ptr ovlyPtr)
{
    UInt32 value, confFactor;
    Int ret = 0;
    Sc_ScalarInfo *scInfo = NULL;
    UInt32 srcInnerWidth, tarInnerWidth;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != config));

    scInfo = (Sc_ScalarInfo *)handle;

    /* Get the Value from SC0 Register */
    value = scInfo->regs->CFG_SC0;
    if(FALSE == config->bypass)
    {
        /* Check for the valid value of parameters */
        GT_assert( VpsHalTrace, (0 == scCheckParams(handle, config)));

        value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_SC_BYPASS_MASK);
        /* Set the input and output frame format in the scalar config0
         * register */
        if (VPS_SF_INTERLACED == config->inFrameMode)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_I_MASK;
        }
        else /* Progressive input frame */
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_I_MASK);
        }
        if (VPS_SF_INTERLACED == config->outFrameMode)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_O_MASK;
        }
        else /* Progressive output frame */
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_O_MASK);
        }

        /* If cropping size is not same as the source frame size, enable
         * trimming */
        if ((config->srcWidth != config->cropWidth) ||
            (config->srcHeight != config->cropHeight))
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_TRIM_MASK;
        }
        else /* Since crop and source size are same, disable trimming */
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_TRIM_MASK);
        }

        /* Invert FID if it is set */
        if (VPS_FIDPOL_INVERT == config->fidPol)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_INVT_FID_MASK;
        }
        else /* Field ID will be used without inversion */
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_INVT_FID_MASK);
        }

        /* Enable Self Generate FID feature if it is enabled */
        if (VPS_SC_SELF_GEN_FID_ENABLE == config->selfGenFid)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_SELFGEN_FID_MASK;
        }
        else /* Disable Self Generate FID */
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_SELFGEN_FID_MASK);
        }

        /* Set the horizontal algorithm */
        value &= (~(CSL_VPS_SC_H_CFG_SC0_CFG_AUTO_HS_MASK |
                     CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK |
                     CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK |
                     CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK));

        if (VPS_SC_HST_AUTO == config->hsType)
        {
            /* All scaling ratio are calculated based on the inner size of the
             * source and the targer image */
            if (TRUE == config->nonLinear)
            {
                /* For nonlinear scaling, source inner width is height and target
                 * inner is calculated based on strip size */
                srcInnerWidth = config->cropHeight;
                tarInnerWidth = config->tarWidth - (config->stripSize << 1u);
            }
            else /* Linear horizontal Scaling */
            {
                /* For linear scaling, inner size is same width */
                srcInnerWidth = config->cropWidth;
                tarInnerWidth = config->tarWidth;
            }

            /* Use polyphase filter for upscaling and down scaling from 1x to 1/4x.
             * Use 2x decimation from 1/4x to 1/8x and rest using polyphase.
             * Use 4x decimation from 1/8x to 1/16x and rest using polyphase.
             */
            if (tarInnerWidth >= srcInnerWidth)
            {
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
            }
            else
            {
                /* Down Scaling between 1/4 and 1 */
                if (tarInnerWidth >= (srcInnerWidth >> 2u))
                {
                    /* Down Scaling without Decimation */
                    value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
                    value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
                }
                /* Down Scaling between 1/8 and 1/4 */
                else if (tarInnerWidth >= (srcInnerWidth >> 3u))
                {
                    /* 2x decimation */
                    value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK;
                    value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
                }
                /* Down Scaling between 1/16 and 1/4 */
                else if (tarInnerWidth >= (srcInnerWidth >> 4u))
                {
                    /* 4x decimation */
                    value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK;
                    value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
                }
                else
                {
                    /* Illegal Scaling factor. i.e. factor < 1/16 */
                    ret = -1;
                }
            }
        }
        else
        {
            switch(config->hsType)
            {
                case VPS_SC_HST_DCM_2X:
                    value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK;
                    break;
                case VPS_SC_HST_DCM_4X:
                    value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK;
                    break;
                case VPS_SC_HST_POLYPHASE:
                    break;
            }
        }

        /* Enable nonlinear Algorighm */
        if (TRUE == config->nonLinear)
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_LINEAR_MASK);
        }
        else
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_LINEAR_MASK;
        }

        /* Set the Vertical Scaling Algorithm */
        if (VPS_SC_VST_RAV == config->vsType)
        {
            /* Enable Running Average Filter in CFG0 */
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK;
        }
        else
        {
            value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK);
        }

        /* Set the Bilinear Interpolation Type */
        value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_SIN2_VER_INTP_MASK);
        if (VPS_SC_BINTP_MODIFIED == config->biLinIntpType)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_SIN2_VER_INTP_MASK;
        }

        /* Set the Hq Bypass mode */
        value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK);
        if (TRUE == config->hPolyBypass)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK;
        }

        /* Enable Peaking filter */
        value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_MASK);
        if (TRUE == config->enablePeaking)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_MASK;
        }

        /* Enable Edge Detection */
        value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_EV_MASK);
        if (TRUE == config->enableEdgeDetect)
        {
            value |= CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_EV_MASK;
        }


        /* Set the default confidence factor */
        confFactor = (config->defConfFactor <<
                        CSL_VPS_SC_H_CFG_SC18_CFG_CONF_DEFAULT_SHIFT) &
                        CSL_VPS_SC_H_CFG_SC18_CFG_CONF_DEFAULT_MASK;

        /* If the overlay pointer is null, config need to be set in
         * the register otherwise config need to be set in the register overlay.
         * Register overlay is used for both memory to memory transfer and
         * runtime configuration. Since number of registers are same for
         * mem-to-mem driver as well runtime configuration, there is no need
         * to keep two separate sets of register overlay offsets. */
        if (NULL == ovlyPtr)
        {
            /* Set the Size Parameters */
            scInfo->regs->CFG_SC24 = scMakeScCfg24Reg(config);
            scInfo->regs->CFG_SC25 = scMakeScCfg25Reg(config);
            scInfo->regs->CFG_SC4 = scMakeScCfg4Reg(config);
            scInfo->regs->CFG_SC5 = scMakeScCfg5Reg(config);
            scInfo->regs->CFG_SC18 |= confFactor;
            /* Set scalar ratio specific parameters */
            scSetScaleRatioParams(scInfo, config);
        }
        else /* Set the configuration in the VPDMA overlay virtual registers */
        {
            /* Set the configuration in the VPDMA register overlay */
            regw(((UInt32 *) ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC18_IDX]),
                confFactor);
            VpsHal_scSetRunTimeConfig(handle, config, ovlyPtr);
        }
    }
    else
    {
        value |= CSL_VPS_SC_H_CFG_SC0_CFG_SC_BYPASS_MASK;
    }

    /* Set the Configuration in SC_CFG0 register */
    if (NULL == ovlyPtr)
    {
        scInfo->regs->CFG_SC0 = value;
    }
    else /* Set the configuration in the VPDMA overlay virtual registers */
    {
        /* Set the configuration in the VPDMA register overlay */
        regw(((UInt32 *) ovlyPtr +
            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]), value);
    }
    return (ret);
}


/**
 *  VpsHal_scSetAdvConfig
 *  \brief This function is used to set the advanced scalar parameters.
 *  This function should be called by core supporting advanced read
 *  and write ioctl.  It does not set all the parameters.
 *
 *  \param handle           Scalar Handle
 *  \param config           pointers to structure containing scalar parameters
 *  \param ovlyPtr          Pointer to memory containing VPDMA register
 *                          overlay for scalar. It can be set to memory
 *                          pointer to program register in overlay or set
 *                          to null to program directly in hardware register
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scSetAdvConfig(VpsHal_Handle handle,
                          const VpsHal_ScConfig *config,
                          Ptr ovlyPtr)
{
    UInt32          value, confFactor;
    Int             ret = 0;
    Sc_ScalarInfo   *scInfo = NULL;
    UInt32          *scRegsOffset;
    UInt32          ravFactor, regRavFactor;
    UInt32          accInit, accIncr, offsetA, offsetB;
    UInt32          srcInnerWidth, tarInnerWidth;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != config));

    scInfo = (Sc_ScalarInfo *)handle;
    scRegsOffset = scInfo->virtRegOffset;

    if (NULL == ovlyPtr)
    {
        /* Get the Value from SC0 Register */
        value = scInfo->regs->CFG_SC0;
    }
    else
    {
        value = regr((UInt32 *) ovlyPtr +
            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]);
    }

    /* Check for the valid value of parameters */
    GT_assert( VpsHalTrace, (0 == scCheckParams(handle, config)));


    /* Set the horizontal algorithm */
    value &= (~(CSL_VPS_SC_H_CFG_SC0_CFG_AUTO_HS_MASK |
                 CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK |
                 CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK |
                 CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK));

    if (VPS_SC_HST_AUTO == config->hsType)
    {
        /* All scaling ratio are calculated based on the inner size of the
         * source and the targer image */
        if (TRUE == config->nonLinear)
        {
            /* For nonlinear scaling, source inner width is height and target
             * inner is calculated based on strip size */
            srcInnerWidth = config->cropHeight;
            tarInnerWidth = config->tarWidth - (config->stripSize << 1u);
        }
        else /* Linear horizontal Scaling */
        {
            /* For linear scaling, inner size is same width */
            srcInnerWidth = config->cropWidth;
            tarInnerWidth = config->tarWidth;
        }

        /* Use polyphase filter for upscaling and down scaling from 1x to 1/4x.
         * Use 2x decimation from 1/4x to 1/8x and rest using polyphase.
         * Use 4x decimation from 1/8x to 1/16x and rest using polyphase.
         */
        if (tarInnerWidth >= srcInnerWidth)
        {
            value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
            value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
        }
        else
        {
            /* Down Scaling between 1/4 and 1 */
            if (tarInnerWidth >= (srcInnerWidth >> 2u))
            {
                /* Down Scaling without Decimation */
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
            }
            /* Down Scaling between 1/8 and 1/4 */
            else if (tarInnerWidth >= (srcInnerWidth >> 3u))
            {
                /* 2x decimation */
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK;
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
            }
            /* Down Scaling between 1/16 and 1/4 */
            else if (tarInnerWidth >= (srcInnerWidth >> 4u))
            {
                /* 4x decimation */
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK;
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
            }
            else
            {
                /* Illegal Scaling factor. i.e. factor < 1/16 */
                ret = -1;
            }
        }
    }
    else
    {
        switch(config->hsType)
        {
            case VPS_SC_HST_DCM_2X:
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK;
                break;
            case VPS_SC_HST_DCM_4X:
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK;
                break;
            case VPS_SC_HST_POLYPHASE:
                break;
        }
    }

    /* Set the Vertical Scaling Algorithm */
    if (VPS_SC_VST_RAV == config->vsType)
    {
        /* Enable Running Average Filter in CFG0 */
        value |= CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK;
    }
    else
    {
        value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK);
    }

    /* Set the Bilinear Interpolation Type */
    value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_SIN2_VER_INTP_MASK);
    if (VPS_SC_BINTP_MODIFIED == config->biLinIntpType)
    {
        value |= CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_SIN2_VER_INTP_MASK;
    }

    /* Set the Hq Bypass mode */
    value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK);
    if (TRUE == config->hPolyBypass)
    {
        value |= CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK;
    }

    /* Enable Peaking filter */
    value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_MASK);
    if (TRUE == config->enablePeaking)
    {
        value |= CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_MASK;
    }

    /* Enable Edge Detection */
    value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_EV_MASK);
    if (TRUE == config->enableEdgeDetect)
    {
        value |= CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_EV_MASK;
    }


    /* Set the default confidence factor */
    confFactor = (config->defConfFactor <<
                    CSL_VPS_SC_H_CFG_SC18_CFG_CONF_DEFAULT_SHIFT) &
                    CSL_VPS_SC_H_CFG_SC18_CFG_CONF_DEFAULT_MASK;

    /* If the overlay pointer is null, config need to be set in
     * the register otherwise config need to be set in the register overlay.
     * Register overlay is used for both memory to memory transfer and
     * runtime configuration. Since number of registers are same for
     * mem-to-mem driver as well runtime configuration, there is no need
     * to keep two separate sets of register overlay offsets. */
    if (NULL == ovlyPtr)
    {
        /* Set the Size Parameters */
        scInfo->regs->CFG_SC24 = scMakeScCfg24Reg(config);
        scInfo->regs->CFG_SC25 = scMakeScCfg25Reg(config);
        scInfo->regs->CFG_SC4 = scMakeScCfg4Reg(config);
        scInfo->regs->CFG_SC5 = scMakeScCfg5Reg(config);
        scInfo->regs->CFG_SC18 |= confFactor;
        /* Set scalar ratio specific parameters */
        scSetScaleRatioParams(scInfo, config);
    }
    else /* Set the configuration in the VPDMA overlay virtual registers */
    {
        /* Set the configuration in the VPDMA register overlay */
        regw(((UInt32 *) ovlyPtr +
            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC18_IDX]),
            confFactor);
        if (VPS_SC_VST_POLYPHASE == config->vsType)
        {
            /* Set the polyphase filter parameters for vertical scalar */
            scCalVertPolyphaseParams(config, &accIncr, &offsetA, &offsetB);
            regw(((UInt32 *)
                    ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]),
                    accIncr);
            regw((
                (UInt32 *)ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]),
                offsetA);
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]),
                offsetB);

            /* Init RAV related registers with default */
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]),
                scInfo->regs->CFG_SC6);
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]),
                scInfo->regs->CFG_SC13);
        }
        else /* Running Average Vertical Scaling is selected */
        {
            /* Set the running average filter parameters for vertical scalar */
            scCalVertRavParams(config, &ravFactor, &accInit);
            regRavFactor = regr((UInt32 *) ovlyPtr +
                    scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC13_IDX])
                    & ~(CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK);
            regRavFactor |= (ravFactor <<
                             CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT);
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]),
                regRavFactor);
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]),
                accInit);

            /* Init polyphase related registers with default */
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]),
                scInfo->regs->CFG_SC1);
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]),
                scInfo->regs->CFG_SC2);
            regw(((UInt32 *)
                ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]),
                scInfo->regs->CFG_SC3);
        }
    }
    /* Set the Configuration in SC_CFG0 register */
    if (NULL == ovlyPtr)
    {
        scInfo->regs->CFG_SC0 = value;
    }
    else /* Set the configuration in the VPDMA overlay virtual registers */
    {
        /* Set the configuration in the VPDMA register overlay */
        regw(((UInt32 *) ovlyPtr +
            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]), value);
    }
    if (NULL != config->peakingCfg)
    {
        scSetPeakingParams(scInfo, config->peakingCfg);
    }
    if (NULL != config->edgeDetectCfg)
    {
        if (TRUE == scInfo->isHqScalar)
        {
            scSetEdgeDetectParams(scInfo, config->edgeDetectCfg);
        }
    }
    return (ret);
}



/**
 *  VpsHal_scGetConfig
 *  \brief This function is used to get the scalar parameters from the
 *  appropriate scalar register.
 *
 *  \param handle           Scalar Handle
 *  \param config           pointers to structure in which scalar parameters
 *                          will be returned
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scGetConfig(VpsHal_Handle handle,
                       VpsHal_ScConfig *config,
                       Ptr ovlyPtr)
{
    UInt32 regValue = 0, value;
    Sc_ScalarInfo *scInfo = NULL;
    Int ret = 0;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (0 == scIsValidScalar(handle)));

    scInfo = (Sc_ScalarInfo *)handle;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC0;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]);
    }

    if (0 != ((regValue & CSL_VPS_SC_H_CFG_SC0_CFG_SC_BYPASS_MASK) >>
                        CSL_VPS_SC_H_CFG_SC0_CFG_SC_BYPASS_SHIFT))
    {
        config->bypass = TRUE;
    }
    else /* Scalar in bypass mode */
    {
        config->bypass = FALSE;
    }
    /* Get the input and output frame format i.e. interlaced or progressive */
    if (0 != ((regValue & CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_O_MASK) >>
                            CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_O_SHIFT))
    {
        config->outFrameMode = VPS_SF_INTERLACED;
    }
    else /* Output frame mode is in progressive */
    {
        config->outFrameMode = VPS_SF_PROGRESSIVE;
    }
    if (0 != ((regValue & CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_I_MASK) >>
                            CSL_VPS_SC_H_CFG_SC0_CFG_INTERLACE_I_SHIFT))
    {
        config->inFrameMode = VPS_SF_INTERLACED;
    }
    else /* Input frame mode is in progressive */
    {
        config->inFrameMode = VPS_SF_PROGRESSIVE;
    }
    if (0 != ((regValue & CSL_VPS_SC_H_CFG_SC0_CFG_LINEAR_MASK) >>
                            CSL_VPS_SC_H_CFG_SC0_CFG_LINEAR_SHIFT))
    {
        config->nonLinear = FALSE;
    }
    else /* Non Linear Scaling */
    {
        config->nonLinear = TRUE;
    }
    if(0 != ((regValue & CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK) >>
              CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_SHIFT))
    {
        config->vsType = VPS_SC_VST_RAV;
    }
    else
    {
        config->vsType = VPS_SC_VST_POLYPHASE;
    }
    config->fidPol = (Vps_FidPol)((regValue &
                            CSL_VPS_SC_H_CFG_SC0_CFG_INVT_FID_MASK) >>
                            CSL_VPS_SC_H_CFG_SC0_CFG_INVT_FID_SHIFT);
    config->selfGenFid = (Vps_ScSelfGenFid)((regValue &
                            CSL_VPS_SC_H_CFG_SC0_CFG_SELFGEN_FID_MASK) >>
                            CSL_VPS_SC_H_CFG_SC0_CFG_SELFGEN_FID_SHIFT);
    config->biLinIntpType = (UInt32)( (regValue &
                        CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_SIN2_VER_INTP_MASK) >>
                        CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_SIN2_VER_INTP_SHIFT);

    config->enableEdgeDetect = (UInt32) (regValue &
                        CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_EV_MASK) >>
                        CSL_VPS_SC_H_CFG_SC0_CFG_ENABLE_EV_SHIFT;

    config->enablePeaking = (UInt32) (regValue &
                        CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_MASK) >>
                        CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_SHIFT;

    config->hPolyBypass = (UInt32) (regValue &
                        CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_MASK) >>
                        CSL_VPS_SC_H_CFG_SC0_CFG_HP_BYPASS_SHIFT;

    value = (regValue & (CSL_VPS_SC_H_CFG_SC0_CFG_AUTO_HS_MASK |
                       CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK |
                       CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK )) >>
                       CSL_VPS_SC_H_CFG_SC0_CFG_AUTO_HS_SHIFT;
    if (0 == value)
    {
        config->hsType = VPS_SC_HST_POLYPHASE;
    }
    else if (0 != (value >> CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_SHIFT))
    {
        config->hsType = VPS_SC_HST_DCM_4X;
    }
    else if (0 != (value >> CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_SHIFT))
    {
        config->hsType = VPS_SC_HST_DCM_2X;
    }
    else /* Horizontal Scalar is automatically selected */
    {
        config->hsType = VPS_SC_HST_AUTO;
    }

    if (NULL  == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC8;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC8_IDX]);
    }
    config->stripSize = (regValue &
                                CSL_VPS_SC_H_CFG_SC8_CFG_NLIN_LEFT_MASK) >>
                                CSL_VPS_SC_H_CFG_SC8_CFG_NLIN_LEFT_SHIFT;

    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC5;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC5_IDX]);
    }
    config->cropWidth = (regValue & CSL_VPS_SC_H_CFG_SC5_CFG_SRC_W_MASK) >>
                                CSL_VPS_SC_H_CFG_SC5_CFG_SRC_W_SHIFT;
    config->cropHeight = (regValue & CSL_VPS_SC_H_CFG_SC5_CFG_SRC_H_MASK) >>
                                CSL_VPS_SC_H_CFG_SC5_CFG_SRC_H_SHIFT;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC4;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC4_IDX]);
    }
    config->tarWidth = (regValue & CSL_VPS_SC_H_CFG_SC4_CFG_TAR_W_MASK) >>
                                CSL_VPS_SC_H_CFG_SC4_CFG_TAR_W_SHIFT;
    config->tarHeight = (regValue & CSL_VPS_SC_H_CFG_SC4_CFG_TAR_H_MASK) >>
                                CSL_VPS_SC_H_CFG_SC4_CFG_TAR_H_SHIFT;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC24;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC24_IDX]);
    }
    config->srcWidth = (regValue & CSL_VPS_SC_H_CFG_SC24_CFG_ORG_W_MASK) >>
                                CSL_VPS_SC_H_CFG_SC24_CFG_ORG_W_SHIFT;
    config->srcHeight = (regValue & CSL_VPS_SC_H_CFG_SC24_CFG_ORG_H_MASK) >>
                                CSL_VPS_SC_H_CFG_SC24_CFG_ORG_H_SHIFT;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC25;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC25_IDX]);
    }
    config->cropStartX = (regValue & CSL_VPS_SC_H_CFG_SC25_CFG_OFF_W_MASK) >>
                                CSL_VPS_SC_H_CFG_SC25_CFG_OFF_W_SHIFT;
    config->cropStartY = (regValue & CSL_VPS_SC_H_CFG_SC25_CFG_OFF_H_MASK) >>
                                CSL_VPS_SC_H_CFG_SC25_CFG_OFF_H_SHIFT;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC18;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC18_IDX]);
    }
    config->defConfFactor = (regValue &
                                CSL_VPS_SC_H_CFG_SC18_CFG_CONF_DEFAULT_MASK) >>
                                CSL_VPS_SC_H_CFG_SC18_CFG_CONF_DEFAULT_SHIFT;
    /* if overlay is provided then rowAccInc, rowAccOffset, rowAccOffsetB;
       ravScFactor, ravRowAccInit, ravRowAccInitB will be read from the
        overlay pointer else it will be read from register. */
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC1;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]);
    }
    config->rowAccInc = regValue;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC2;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]);
    }
    config->rowAccOffset = regValue;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC3;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]);
    }
    config->rowAccOffsetB = regValue;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC6;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]);
    }
    config->ravRowAccInit = (regValue &
                             CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_MASK) >>
                                CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_SHIFT;
    config->ravRowAccInit = (regValue &
                          CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_B_MASK) >>
                          CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_B_SHIFT;
    if (NULL == ovlyPtr)
    {
        regValue = scInfo->regs->CFG_SC13;
    }
    else
    {
        regValue = regr((UInt32 *)ovlyPtr +
                scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]);
    }
    config->ravScFactor = (regValue &
                            CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK) >>
                            CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT;
    if (NULL != config->peakingCfg)
    {
        scGetPeakingParams(scInfo, config->peakingCfg);
    }
    if (NULL != config->edgeDetectCfg)
    {
        if (TRUE == scInfo->isHqScalar)
        {
            scGetEdgeDetectParams(scInfo, config->edgeDetectCfg);
        }
    }

    return (ret);
}



/**
 *  Void VpsHal_scSetRunTimeConfig
 *  \brief Function is used to set the scaling ratio and position specific
 *  parameters in the VPDMA register overlay. It will be called at the
 *  run time when scaling ratio or position is changed at the run
 *  time while display is on. This function will also be used for
 *  memory to memory driver to set the scaling parameters in register
 *  overlay. This function will only change scaling ratio specific
 *  parameters. It will not change scaling algorithm or parameters
 *  configurable in CFG_SC0 register.
 *
 *  \param handle           Scalar Handle
 *  \param memPtr           VPDMA register overlay pointer
 *  \param config           Pointer to structure containing scalar parameters.
 *
 *  \return                 None
 */
Void VpsHal_scSetRunTimeConfig(VpsHal_Handle handle,
                               const VpsHal_ScConfig *config,
                               Ptr memPtr)
{
    UInt32 *ovlyPtr = (UInt32 *) memPtr, *scRegsOffset;
    UInt32 shift;
    UInt32 ravFactor, regRavFactor;
    UInt32 accInit, accIncr, offsetA, offsetB;
    Int32 nonLinearInit, nonLinearAcc;
    Sc_ScalarInfo *scInfo = NULL;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != memPtr));
    GT_assert( VpsHalTrace, (NULL != config));

    /* Get the pointer to register offsets array */
    scInfo = (Sc_ScalarInfo *)handle;
    scRegsOffset = scInfo->virtRegOffset;
    GT_assert( VpsHalTrace, (NULL != scRegsOffset));

    /* Set the Size Parameters */
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC24_IDX]),
                                                    scMakeScCfg24Reg(config));
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC25_IDX]),
                                                    scMakeScCfg25Reg(config));
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC4_IDX]),
                                                    scMakeScCfg4Reg(config));
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC5_IDX]),
                                                    scMakeScCfg5Reg(config));

    /* TODO: How to calculate value of Col Acc Offset */
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC12_IDX]), scCalHorzColAccOffset(config));

    if (VPS_SC_VST_POLYPHASE == config->vsType)
    {
        /* Set the polyphase filter parameters for vertical scalar */
        scCalVertPolyphaseParams(config, &accIncr, &offsetA, &offsetB);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]), accIncr);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]), offsetA);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]), offsetB);

        /* Init RAV related registers with default */
        regw(((UInt32 *)
            ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]),
            scInfo->regs->CFG_SC6);
        regw(((UInt32 *)
            ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]),
            scInfo->regs->CFG_SC13);
    }
    else /* Running Average Vertical Scaling is selected */
    {
        /* Set the running average filter parameters for vertical scalar */
        scCalVertRavParams(config, &ravFactor, &accInit);
        regRavFactor = scInfo->regs->CFG_SC13 &
                       ~(CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK);
        regRavFactor |= (ravFactor <<
                         CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]),
             regRavFactor);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]), accInit);

        /* Init polyphase related registers with default */
        regw(((UInt32 *)
            ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]),
            scInfo->regs->CFG_SC1);
        regw(((UInt32 *)
            ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]),
            scInfo->regs->CFG_SC2);
        regw(((UInt32 *)
            ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]),
            scInfo->regs->CFG_SC3);
    }

    /* Set the linear horizontal scaling parameters */
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC9_IDX]),
                                scCalHorzLinearAccInc(config, &shift));
    /* Set the size of left and right strip for nonlinear scaling*/
    regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC8_IDX]),
                                        scMakeScCfg8Reg(config));
    if (TRUE == config->nonLinear)
    {
        /* Set the nonlinear horizontal scaling parameters */
        scCalHorzNonLinearParams(
            config,
            shift,
            &nonLinearInit,
            &nonLinearAcc);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC10_IDX]),
                                                        nonLinearInit);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC11_IDX]),
                                                        nonLinearAcc);
    }
    else
    {
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC10_IDX]),
                                                        0);
        regw((ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC11_IDX]),
                                                        0);
    }
}



/**
 *  Void VpsHal_scGetRegOvlySize
 *  \brief Function to get the VPDMA register overlay size required to
 *  store scalar registers. This overlay is mainly used for stoing
 *  register for the runtime configuration as well as for the memory
 *  to memory driver where configuration changes for each channel.
 *
 *  \param handle           Scalar Handle
 *
 *  \return                 Register Overlay size
 */
UInt32 VpsHal_scGetConfigOvlySize(VpsHal_Handle handle)
{
    GT_assert( VpsHalTrace, (0 == scIsValidScalar(handle)));

    return (((Sc_ScalarInfo *)handle)->regOvlySize);
}



/**
 *  Void VpsHal_scGetCoeffOvlySize
 *  \brief This function is used to get the memory size required to
 *  store coefficients. It returns size in terms of bytes. Upper
 *  layer can allocate the memory and pass this memory with the
 *  coefficients to fill the the memory with coefficients.
 *
 *  \param handle            Scalar Handle
 *  \param horzCoeffOvlySize Location to return size of horizontal scaling
 *                           coefficients
 *  \param vertCoeffOvlySize Location to return size of vertical scaling
 *                           coefficients
 *  \param vertBilinearCoeffOvlySize Location to return size of vertical
 *                                   bilinear scaling coefficients. If not HQ
 *                                   scalar, value returned shall be 0.
 *
 *  \return                 None
 */
Void VpsHal_scGetCoeffOvlySize(VpsHal_Handle handle,
                               UInt32 * horzCoeffOvlySize,
                               UInt32 * vertCoeffOvlySize,
                               UInt32 * vertBilinearCoeffOvlySize)
{
    Sc_ScalarInfo *scInfo = handle;

    GT_assert( VpsHalTrace, (NULL != scInfo));
    GT_assert( VpsHalTrace, (NULL != horzCoeffOvlySize));
    GT_assert( VpsHalTrace, (NULL != vertCoeffOvlySize));
    GT_assert( VpsHalTrace, (NULL != vertBilinearCoeffOvlySize));

    *horzCoeffOvlySize = VPSHAL_SC_HORZ_COEFF_OVLY_SIZE;
    *vertCoeffOvlySize = VPSHAL_SC_VERT_COEFF_OVLY_SIZE;
    if (TRUE == scInfo->isHqScalar)
    {
        *vertBilinearCoeffOvlySize = VPSHAL_SC_BILINEAR_COEFF_OVLY_SIZE;
    }
    else
    {
        *vertBilinearCoeffOvlySize = 0u;
    }
}



/**
 *  VpsHal_scCreateCoeffOvly
 *  \brief Function is used to fill up the coefficient overlay with the
 *  standard coefficients provided by the IP team. The coefficients
 *  are selected based on the scaling ratio. Upper layer should
 *  allocate coefficient memory required to store these coefficients.
 *  They can get the size of the coefficient memory by calling
 *  Sc_getCoeffOvlySize function.
 */
Int VpsHal_scCreateCoeffOvly(VpsHal_Handle handle,
                             const VpsHal_ScCoeffConfig *config,
                             Ptr * horzCoeffMemPtr,
                             Ptr * vertCoeffMemPtr,
                             Ptr * vertBilinearCoeffMemPtr)
{
    Int ret = 0;
    Sc_ScalarInfo *scInfo = (Sc_ScalarInfo *)handle;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (NULL != horzCoeffMemPtr));
    GT_assert( VpsHalTrace, (NULL != vertCoeffMemPtr));
    GT_assert( VpsHalTrace, (0 == scIsValidScalar(handle)));

    /* Check if user has provided coeffs */
    if (NULL == scInfo->userScCoeff)
    {
        /* Get a pointer to coefficient memory for Horizontal scalar */
        scGetStdHorzCoeffOvly(handle, config, horzCoeffMemPtr);

        /* Get pointers to coefficient memories for Vertical scalar */
        scGetStdVertCoeffOvly(handle,
                              config,
                              vertCoeffMemPtr,
                              vertBilinearCoeffMemPtr);
    }
    else /* Use the user-provided coeffs instead. */
    {
        ret = VpsHal_scCreateUserCoeffOvly(handle,
                                           NULL,
                                           horzCoeffMemPtr,
                                           vertCoeffMemPtr,
                                           vertBilinearCoeffMemPtr);
    }

    return (ret);
}



/**
 *  VpsHal_scCreateUserCoeffOvly
 *
 *  \brief  If the application has its own set of coefficients and does not
 *          want to use driver provided coefficient, this function is used. It
 *          copies the user provided coefficients locally so that they can be
 *          used instead of the defaults during scaling.
 *          Coefficient Memory must be allocated by using size provided
 *          by scalar.
 *  \param  handle          Scalar Handle
 *  \param  coeff           Structure containing all user-provided scalar
 *                          coefficients. If provided as NULL, this function
 *                          has been called to get, rather than create the
 *                          coefficients.
 *  \param  horzCoeffMemPtr Location to return a pointer to horizontal scaling
 *                          coefficients
 *  \param  vertCoeffMemPtr Location to return a pointer to vertical scaling
 *                          coefficients
 *  \param  vertBilinearCoeffMemPtr Location to return a pointer to vertical
 *                          bilinear scaling coefficients (only if applicable,
 *                          i.e. for HQ scalar. Can be NULL otherwise).
 *
 *  \return 0               on Success
 *          -1              on Error
 */
Int VpsHal_scCreateUserCoeffOvly(VpsHal_Handle handle,
                                 const Vps_ScCoeff *coeff,
                                 Ptr * horzCoeffMemPtr,
                                 Ptr * vertCoeffMemPtr,
                                 Ptr * vertBilinearCoeffMemPtr)
{
    Int ret = 0;
    Sc_ScalarInfo *scInfo = (Sc_ScalarInfo *)handle;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != horzCoeffMemPtr));
    GT_assert(VpsHalTrace, (NULL != vertCoeffMemPtr));

    /* If coeff is not NULL, then new coeffs are to be set. */
    if (NULL != coeff)
    {
        /* It is assumed that user has provided all the coefficients. */
        VpsUtils_memcpy(gUserScCoeff, coeff, sizeof (Vps_ScCoeff));
        scInfo->userScCoeff = (UInt32 *) gUserScCoeff;
    }

    /* Return pointers to the horizontal, vertical and bilinear coeffs. The
     * pointer to horizontal luma coeffs has the chroma coeffs after it for each
     * set of coefficients.
     */
    *horzCoeffMemPtr = &(gUserScCoeff->hsLuma);
    *vertCoeffMemPtr = &(gUserScCoeff->vsLuma);

    if (TRUE == (((Sc_ScalarInfo *)handle)->isHqScalar))
    {
        *vertBilinearCoeffMemPtr = &(gUserScCoeff->vsBilinearLuma);
    }
    else
    {
        /* Return NULL for non-HQ Scalar */
        *vertBilinearCoeffMemPtr = NULL;
    }

    return (ret);
}


/**
 *  Void VpsHal_scCreateConfigOvly
 *  \brief This function is used to create the complete VPDMA register overlay
 *  for scalar registers. It just create the overlay but does not
 *  initialize the virtual registers with the configuration.
 *  Configuration can be done by calling Sc_setParams function.
 *
 *  \param handle           Scalar Handle
 *  \param configOvlyPtr    Pointer to memory where VPDMA overlay will be
 *                          stored
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scCreateConfigOvly(VpsHal_Handle handle,
                              Ptr configOvlyPtr)
{
    Int ret = -1;

    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (NULL != configOvlyPtr));

    /* Create the register overlay */
    ret = VpsHal_vpdmaCreateRegOverlay(
              ((Sc_ScalarInfo *)handle)->regOffset,
              SC_CONFIG_OVLY_NUM_IDX,
              configOvlyPtr);
    return (ret);
}


/**
 *  \brief This function is used to get the information that
 *  identifies the scaling factor for the provided configuration.
 *  It also sets the register related to vsType changes.
 */
Int VpsHal_scGetScFactorConfig(VpsHal_Handle handle,
                               VpsHal_ScConfig *config,
                               VpsHal_ScFactorConfig *scFactorConfig,
                               Ptr ovlyPtr)
{
    Int ret = 0;
    Sc_ScalarInfo   *scInfo = NULL;
    UInt32 value;
    UInt32 srcInnerWidth, tarInnerWidth;
    UInt32 *scRegsOffset;
    UInt32 ravFactor, regRavFactor;
    UInt32 accInit, accIncr, offsetA, offsetB;
    Int32 coeffId_h, coeffId_v, numerator, denominator;

    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));
    GT_assert(VpsHalTrace, (NULL != scFactorConfig));

    scInfo = (Sc_ScalarInfo *)handle;
    scRegsOffset = scInfo->virtRegOffset;

    /* -------------------------------------------------------------------------
     * Calculate horizontal scaling configurations.
     * -------------------------------------------------------------------------
     */
    if (TRUE == config->bypass)
    {
        /* Set everything to invalid in case scalar is in bypass, so that the
         * driver knows to not do any scalar coeff configuration for this
         * channel. hScalingSet and vScalingSet don't matter in this case.
         */
        scFactorConfig->hsType = VPS_SC_HST_MAX;
        scFactorConfig->hScalingSet = VPS_SC_US_SET;
        scFactorConfig->vsType = VPS_SC_VST_MAX;
        scFactorConfig->vScalingSet = VPS_SC_US_SET;
    }
    else
    {
        /* All scaling ratio are calculated based on the inner size of the
         * source and the targer image */
        if (TRUE == config->nonLinear)
        {
            /* For nonlinear scaling, source inner width is height and target
             * inner is calculated based on strip size */
            srcInnerWidth = config->cropHeight;
            tarInnerWidth = config->tarWidth - (config->stripSize << 1u);
        }
        else /* Linear horizontal Scaling */
        {
            /* For linear scaling, inner size is same width */
            srcInnerWidth = config->cropWidth;
            tarInnerWidth = config->tarWidth;
        }

        /* Use upscaling coefficients when target width is more than source
         * width.
         * For downscaling where the downscaling factor is exactly 1/2 or 1/4,
         * polyphase filter does not get used at all, and only decimators are
         * used. For scaling factor 1, neither polyphase filter nor decimators
         * are used.
         */
        if (tarInnerWidth > srcInnerWidth)
        {
            scFactorConfig->hsType = VPS_SC_HST_POLYPHASE;
            scFactorConfig->hScalingSet = VPS_SC_US_SET;
        }
        else if (tarInnerWidth == srcInnerWidth)
        {
            scFactorConfig->hsType = VPS_SC_HST_POLYPHASE;
            scFactorConfig->hScalingSet = VPS_SC_SET_1_1;
        }
        else
        {
            /* Down Scaling between 1/4 and 1 */
            if (tarInnerWidth >= (srcInnerWidth >> 2u))
            {
                /* Down Scaling without Decimation */
                scFactorConfig->hsType = VPS_SC_HST_POLYPHASE;
                /* Polyphase filter used for scaling factor between 1/2 and 1 */
                srcInnerWidth = srcInnerWidth;
                numerator = denominator = 16;
                for(coeffId_h=VPS_SC_US_SET;coeffId_h>=VPS_SC_DS_SET_3_16; coeffId_h--)
                {
                    if( srcInnerWidth*numerator <= tarInnerWidth*denominator )
                        break;
                    numerator--;
                }
                scFactorConfig->hScalingSet = coeffId_h;

            }
            /* Down Scaling between 1/8 and 1/4 */
            else if (tarInnerWidth >= (srcInnerWidth >> 3u))
            {
                /* 2x decimation */
                scFactorConfig->hsType = VPS_SC_HST_DCM_2X;
                srcInnerWidth = srcInnerWidth >> 1;
                numerator = denominator = 16;
                for(coeffId_h=VPS_SC_US_SET;coeffId_h>=VPS_SC_DS_SET_3_16; coeffId_h--)
                {
                    if( srcInnerWidth*numerator <= tarInnerWidth*denominator )
                        break;
                    numerator--;
                }
                scFactorConfig->hScalingSet = coeffId_h;
            }
            /* Down Scaling between 1/16 and 1/4 */
            else if (tarInnerWidth >= (srcInnerWidth >> 4u))
            {
                /* 4x decimation */
                scFactorConfig->hsType = VPS_SC_HST_DCM_4X;
                /* Polyphase filter used for scaling factor between 1/2 and 1 */

                srcInnerWidth = srcInnerWidth >> 2;

                numerator = denominator = 16;
                for(coeffId_h=VPS_SC_US_SET;coeffId_h>=VPS_SC_DS_SET_3_16; coeffId_h--)
                {
                    if( srcInnerWidth*numerator <= tarInnerWidth*denominator )
                        break;
                    numerator--;
                }
                scFactorConfig->hScalingSet = coeffId_h;
            }
            else
            {
                /* Illegal Scaling factor. i.e. factor < 1/16 */
                ret = -1;
            }
            config->hsType = scFactorConfig->hsType;
            if (NULL == ovlyPtr)
            {
                value = scInfo->regs->CFG_SC0;
            }
            else
            {
                /* Get the Value from SC0 Register */
                value = regr((UInt32 *) ovlyPtr +
                        scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]);
            }
            /* Set auto HS to False always */
            value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_AUTO_HS_MASK);
            if (VPS_SC_HST_DCM_2X == config->hsType)
            {
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK;
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
            }
            else if (VPS_SC_HST_DCM_4X == config->hsType)
            {
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK;
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
            }
            else if (VPS_SC_HST_POLYPHASE == config->hsType)
            {
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_4X_MASK);
                value &= ~(CSL_VPS_SC_H_CFG_SC0_CFG_DCM_2X_MASK);
            }
        }

        if (0 == ret)
        {
            /* -----------------------------------------------------------------
             * Calculate vertical scaling configurations.
             * -----------------------------------------------------------------
             */
            /* Check if upscaling */
            if (config->tarHeight > config->cropHeight)
            {
                /* Always use polyphase for upscaling. */
                scFactorConfig->vsType = VPS_SC_VST_POLYPHASE;
                scFactorConfig->vScalingSet = VPS_SC_US_SET;
            }
            else if (config->tarHeight == config->cropHeight)
            {
                 /* 1-1 Scaling */
                scFactorConfig->vsType = VPS_SC_VST_POLYPHASE;
                scFactorConfig->vScalingSet = VPS_SC_SET_1_1;
            }
            else /* Down Scaling */
            {
                /* Determine the type of vertical scaling filter to be used. Use
                 * RAV filter for downscaling more than (1/4)x and polyphase for
                 * scaling factor lower than that.
                 */
                if (config->tarHeight >= (config->cropHeight >> 2u))
                {
                    scFactorConfig->vsType = VPS_SC_VST_POLYPHASE;
                    numerator = denominator = 16;
                    for(coeffId_v=VPS_SC_US_SET;coeffId_v>=VPS_SC_DS_SET_3_16; coeffId_v--)
                    {
                        if( config->cropHeight*numerator <= config->tarHeight*denominator )
                            break;
                        numerator--;
                    }
                    scFactorConfig->vScalingSet = coeffId_v;
                }
                else
                {
                    /* Should use RAV for factor < (1/4)x. */
                    scFactorConfig->vsType = VPS_SC_VST_RAV;
                    scFactorConfig->vScalingSet = VPS_SC_US_SET;
                }
            }

            /* Update vsType in config. hsType should not be modified. It should
             * remain auto.
             */
            config->vsType = scFactorConfig->vsType;

            /* If the overlay pointer is null, config need to be set in
             * the register otherwise config need to be set in the register
             * overlay.
             * Register overlay is used for both memory to memory transfer and
             * runtime configuration. Since number of registers are same for
             * mem-to-mem driver as well runtime configuration, there is no need
             * to keep two separate sets of register overlay offsets. */
            if (NULL == ovlyPtr)
            {
                value = scInfo->regs->CFG_SC0;
            }
            else
            {
                /* Get the Value from SC0 Register */
                value = regr((UInt32 *) ovlyPtr +
                        scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]);
            }

            if (VPS_SC_VST_POLYPHASE == scFactorConfig->vsType)
            {
                /* Disable Running Average Filter in CFG0 */
                value &= (~CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK);

                /* Set the polyphase filter parameters for vertical scalar */
                scCalVertPolyphaseParams(config, &accIncr, &offsetA, &offsetB);
                if (NULL == ovlyPtr)
                {
                    scInfo->regs->CFG_SC1 = accIncr;
                    scInfo->regs->CFG_SC2 = offsetA;
                    scInfo->regs->CFG_SC3 = offsetB;
                }
                else
                {
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]),
                        accIncr);
                    regw((
                      (UInt32 *) ovlyPtr +
                        scRegsOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]),
                      offsetA);
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]),
                        offsetB);

                    /* Init RAV related registers with default */
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]),
                        scInfo->regs->CFG_SC6);
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]),
                        scInfo->regs->CFG_SC13);
                }
            }
            else
            {
                /* Enable Running Average Filter in CFG0 */
                value |= CSL_VPS_SC_H_CFG_SC0_CFG_USE_RAV_MASK;

                /* Set the running average filter parameters for vertical
                 * scalar
                 */
                scCalVertRavParams(config, &ravFactor, &accInit);
                if (NULL == ovlyPtr)
                {
                    regRavFactor = scInfo->regs->CFG_SC13 &
                        ~(CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK);
                    regRavFactor |= (ravFactor <<
                                 CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT);
                    scInfo->regs->CFG_SC13 = regRavFactor;
                    scInfo->regs->CFG_SC6 = accInit;
                }
                else
                {
                    regRavFactor = regr((UInt32 *) ovlyPtr +
                            scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC13_IDX])
                            & ~(CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK);
                    regRavFactor |= (ravFactor <<
                                 CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT);
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC13_IDX]),
                        regRavFactor);
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC6_IDX]),
                        accInit);

                    /* Init polyphase related registers with default */
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC1_IDX]),
                        scInfo->regs->CFG_SC1);
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC2_IDX]),
                        scInfo->regs->CFG_SC2);
                    regw(((UInt32 *)
                        ovlyPtr + scRegsOffset[SC_CONFIG_OVLY_CFG_SC3_IDX]),
                        scInfo->regs->CFG_SC3);
                }
            }

            if (NULL == ovlyPtr)
            {
                scInfo->regs->CFG_SC0 = value;
            }
            else
            {
                /* Set the configuration in VPDMA overlay virtual registers */
                regw(((UInt32 *) ovlyPtr +
                    scInfo->virtRegOffset[SC_CONFIG_OVLY_CFG_SC0_IDX]), value);
            }
        }
    }

    return (ret);
}



/**
 *  VpsHal_scGetVpdmaConfigDest
 *  \brief Returns the VPDMA configuration destination to program coefficients
 *  for this scalar instance.
 *
 *  \param handle           Scalar Handle
 *
 *  \return                 VPDMA configuration destination
 */
VpsHal_VpdmaConfigDest VpsHal_scGetVpdmaConfigDest(VpsHal_Handle handle)
{
    GT_assert( VpsHalTrace, (NULL != handle));

    return (((Sc_ScalarInfo *)handle)->coeffConfigDest);
}



/**
 *  \brief Function to check for the correct value of the scalar handle.
 */
static inline Int scIsValidScalar(VpsHal_Handle handle)
{
    Int found = -1;
    UInt32 cnt;
    Sc_ScalarInfo *scTempInfo = (Sc_ScalarInfo *)handle;
    Sc_ScalarInfo *scTemp = NULL;

    GT_assert( VpsHalTrace, (NULL != scTempInfo));

    for (cnt = 0u; cnt < VPSHAL_SC_MAX_INST; cnt++)
    {
        scTemp = &(ScInfo[cnt]);
        if (scTempInfo == scTemp)
        {
            found = 0;
            break;
        }
    }
    return (found);
}



/**
 *  static Int scSetPeakingParams(Sc_ScalarInfo *scInfo,
 *                                        Vps_ScPeakingConfig *pParams)
 *
 *  \brief  Function to the peaking block parameters in the scalar register.
 *          Expert values for this block is set at the time of init. If
 *          required, expert user can change parameters for this block by
 *          modifying macros given for the peaking block.
 *          scInfo      must be correct pointer
 *          pParams     must be correct pointer
 *
 *  \param  scInfo      Pointer to scalar information structure
 *  \param  pParams     Pointer to structure containing peaking parameters
 *
 *  \return  0          on Success
 *          -1          on Error
 */
static Int scSetPeakingParams(Sc_ScalarInfo *scInfo,
                              Vps_ScPeakingConfig *pParams)
{
    Int ret = 0;
    UInt32 value;

    GT_assert( VpsHalTrace, (NULL != scInfo));
    GT_assert( VpsHalTrace, (NULL != pParams));
    GT_assert( VpsHalTrace, (pParams->nlLowThreshold < pParams->nlHighThreshold));

    /* Set the High Pass Filter Coefficients in the register */
    value = (pParams->hpfCoeff[0] <<
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF0_SHIFT) &
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF0_MASK;
    value |= (pParams->hpfCoeff[1] <<
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF1_SHIFT) &
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF1_MASK;
    value |= (pParams->hpfCoeff[2] <<
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF2_SHIFT) &
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF2_MASK;
    value |= (pParams->hpfCoeff[3] <<
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF3_SHIFT) &
                CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF3_MASK;
    scInfo->regs->CFG_SC19 = value;

    /* Set the Clipping limit and decimal point for coefficient in
       register */
    value = (pParams->hpfCoeff[4] <<
                CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF4_SHIFT) &
                CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF4_MASK;
    value |= (pParams->hpfCoeff[5] <<
                CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF5_SHIFT) &
                CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF5_MASK;
    value |= (pParams->hpfNormShift <<
                CSL_VPS_SC_H_CFG_SC20_CFG_HPF_NORM_SHIFT_SHIFT) &
                CSL_VPS_SC_H_CFG_SC20_CFG_HPF_NORM_SHIFT_MASK;
    value |= (pParams->nlClipLimit <<
                CSL_VPS_SC_H_CFG_SC20_CFG_NL_LIMIT_SHIFT) &
                CSL_VPS_SC_H_CFG_SC20_CFG_NL_LIMIT_MASK;
    scInfo->regs->CFG_SC20 = value;

    /* Set the Low threshold and low slope gain in the register */
    value = (pParams->nlLowThreshold <<
                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_THR_MASK;
    value |= (pParams->nlLowSlopeGain <<
                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_SLOPE_SHIFT) &
                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_SLOPE_MASK;
    scInfo->regs->CFG_SC21 = value;

    /* Set the High Threshold and slope shift in the register */
    value = (pParams->nlHighThreshold <<
                CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_THR_MASK;
    value |= (pParams->nlHighSlopeShift <<
                CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_SLOPE_SHIFT_SHIFT) &
                CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_SLOPE_SHIFT_MASK;
    scInfo->regs->CFG_SC22 = value;

    /* Enable Peaking block in the register */
    scInfo->regs->CFG_SC0 |= CSL_VPS_SC_H_CFG_SC0_CFG_Y_PK_EN_MASK;
    return (ret);
}


/**
 *  static Int scGetPeakingParams(Sc_ScalarInfo *scInfo,
 *                                        Vps_ScPeakingConfig *pParams)
 *
 *  \brief  Function to the peaking block parameters in the scalar register.
 *          Expert values for this block is set at the time of init. If
 *          required, expert user can change parameters for this block by
 *          modifying macros given for the peaking block.
 *          scInfo      must be correct pointer
 *          pParams     must be correct pointer
 *
 *  \param  scInfo      Pointer to scalar information structure
 *  \param  pParams     Pointer to peaking parameters structure
 *
 *  \return  0          on Success
 *          -1          on Error
 */
static Int scGetPeakingParams(Sc_ScalarInfo *scInfo,
                              Vps_ScPeakingConfig *pParams)
{
    Int ret = 0;

    GT_assert( VpsHalTrace, (NULL != scInfo));
    GT_assert( VpsHalTrace, (NULL != pParams));

    /* Set the High Pass Filter Coefficients in the register */
    pParams->hpfCoeff[0]  = (scInfo->regs->CFG_SC19 &
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF0_MASK) >>
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF0_SHIFT;
    pParams->hpfCoeff[1] = (scInfo->regs->CFG_SC19 &
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF1_MASK) >>
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF1_SHIFT;
    pParams->hpfCoeff[2] = (scInfo->regs->CFG_SC19 &
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF2_MASK) >>
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF2_SHIFT;
    pParams->hpfCoeff[3] = (scInfo->regs->CFG_SC19 &
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF3_MASK) >>
                            CSL_VPS_SC_H_CFG_SC19_CFG_HPF_COEF3_SHIFT;

    pParams->hpfCoeff[4] = (scInfo->regs->CFG_SC20 &
                            CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF4_MASK) >>
                            CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF4_SHIFT;
    pParams->hpfCoeff[5] = (scInfo->regs->CFG_SC20 &
                            CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF5_MASK) >>
                            CSL_VPS_SC_H_CFG_SC20_CFG_HPF_COEF5_SHIFT;
    pParams->hpfNormShift = (scInfo->regs->CFG_SC20 &
                            CSL_VPS_SC_H_CFG_SC20_CFG_HPF_NORM_SHIFT_MASK) >>
                            CSL_VPS_SC_H_CFG_SC20_CFG_HPF_NORM_SHIFT_SHIFT;
    pParams->nlClipLimit = (scInfo->regs->CFG_SC20 &
                            CSL_VPS_SC_H_CFG_SC20_CFG_NL_LIMIT_MASK) >>
                            CSL_VPS_SC_H_CFG_SC20_CFG_NL_LIMIT_SHIFT;

    pParams->nlLowThreshold = (scInfo->regs->CFG_SC21 &
                                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_THR_MASK) >>
                                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_THR_SHIFT;
    pParams->nlLowSlopeGain = (scInfo->regs->CFG_SC21 &
                                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_SLOPE_MASK) >>
                                CSL_VPS_SC_H_CFG_SC21_CFG_NL_LO_SLOPE_SHIFT;

    pParams->nlHighThreshold = (scInfo->regs->CFG_SC22 &
                                CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_THR_MASK) >>
                                CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_THR_SHIFT;
    pParams->nlHighSlopeShift = (scInfo->regs->CFG_SC22 &
                        CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_SLOPE_SHIFT_MASK) >>
                        CSL_VPS_SC_H_CFG_SC22_CFG_NL_HI_SLOPE_SHIFT_SHIFT;

    return (ret);
}


/**
 *  static Int scSetEdgeDetectParams(Sc_ScalarInfo *scInfo,
                                        Vps_ScEdgeDetectConfig *edParams)
 *
 *  \brief  Function to the Edge Detect block parameters in the register.
 *          Expert values for this block are set at the time of init. If
 *          required, expert user can change parameters for this block by
 *          modifying macros given for the this block.
 *          scInfo      must be correct pointer
 *          edParams    must be correct pointer
 *  \param  scInfo      Pointer to scalar information structure
 *  \param  edParams    Pointer to structure containing Edge Detect
 *                      parameters
 *
 *  \return 0           on Success
 *          -1          on Error
 *
 *
 *
 *
 *    None.
 */
static Int scSetEdgeDetectParams(Sc_ScalarInfo *scInfo,
                                 Vps_ScEdgeDetectConfig *edParams)
{
    Int ret = 0;
    UInt32 value;

    GT_assert( VpsHalTrace, (NULL != scInfo));
    GT_assert( VpsHalTrace, (NULL != edParams));
    GT_assert( VpsHalTrace,
                (SC_DELTA_LUMA_THR_MAX > edParams->deltaLumaThreshold));
    GT_assert( VpsHalTrace,
                (SC_DELTA_CHROMA_THR_MAX > edParams->deltaChromaThreshold));

    value = (edParams->gradientThreshold <<
                CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_MASK;
    value |= (edParams->gradientRange <<
                CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_RANGE_SHIFT) &
                CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_RANGE_MASK;
    value |= (edParams->minGyThreshold <<
                CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_MASK;
    value |= (edParams->minGyRange <<
                CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_RANGE_SHIFT) &
                CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_RANGE_MASK;
    scInfo->regs->CFG_SC23 = value;

    value = scInfo->regs->CFG_SC13;
    value |= (edParams->chromaIntpThreshold <<
                CSL_VPS_SC_H_CFG_SC13_CFG_CHROMA_INTP_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC13_CFG_CHROMA_INTP_THR_MASK;
    value |= (edParams->deltaChromaThreshold <<
                CSL_VPS_SC_H_CFG_SC13_CFG_DELTA_CHROMA_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC13_CFG_DELTA_CHROMA_THR_MASK;
    scInfo->regs->CFG_SC13 = value;

    value = (edParams->evThreshold <<
                CSL_VPS_SC_H_CFG_SC17_CFG_EV_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC17_CFG_EV_THR_MASK;
    value |= (edParams->deltaLumaThreshold <<
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_LUMA_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_LUMA_THR_MASK;
    value |= (edParams->deltaEvThreshold <<
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_EV_THR_SHIFT) &
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_EV_THR_MASK;
    scInfo->regs->CFG_SC17 = value;

    return (ret);
}

/**
 *  static Int scGetEdgeDetectParams(Sc_ScalarInfo *scInfo,
                                        Vps_ScEdgeDetectConfig *edParams)
 *
 *  \brief  Function to the Edge Detect block parameters in the register.
 *          Expert values for this block are set at the time of init. If
 *          required, expert user can change parameters for this block by
 *          modifying macros given for the this block.
 *          scInfo      must be correct pointer
 *          edParams    must be correct pointer
 *  \param  scInfo      Pointer to scalar information structure
 *  \param  edParams    Pointer to structure containing Edge Detect
 *                      parameters
 *
 *  \return 0           on Success
 *          -1          on Error
 *
 *
 *
 *
 *    None.
 */
static Int scGetEdgeDetectParams(Sc_ScalarInfo *scInfo,
                                 Vps_ScEdgeDetectConfig *edParams)
{
    Int ret = 0;

    GT_assert( VpsHalTrace, (NULL != scInfo));
    GT_assert( VpsHalTrace, (NULL != edParams));


    edParams->gradientThreshold = (scInfo->regs->CFG_SC23 &
                                CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_MASK) >>
                                CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_SHIFT;
    edParams->gradientRange = (scInfo->regs->CFG_SC23 &
                        CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_RANGE_MASK) >>
                        CSL_VPS_SC_H_CFG_SC23_CFG_GRADIENT_THR_RANGE_SHIFT;
    edParams->minGyThreshold = (scInfo->regs->CFG_SC23 &
                            CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_MASK) >>
                            CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_SHIFT;
    edParams->minGyRange = (scInfo->regs->CFG_SC23 &
                            CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_RANGE_MASK) >>
                            CSL_VPS_SC_H_CFG_SC23_CFG_MIN_GY_THR_RANGE_SHIFT;

    edParams->chromaIntpThreshold = (scInfo->regs->CFG_SC13 &
                    CSL_VPS_SC_H_CFG_SC13_CFG_CHROMA_INTP_THR_MASK) >>
                    CSL_VPS_SC_H_CFG_SC13_CFG_CHROMA_INTP_THR_SHIFT;
    edParams->deltaChromaThreshold = (scInfo->regs->CFG_SC13 &
                    CSL_VPS_SC_H_CFG_SC13_CFG_DELTA_CHROMA_THR_MASK) >>
                    CSL_VPS_SC_H_CFG_SC13_CFG_DELTA_CHROMA_THR_SHIFT;

    edParams->evThreshold = (scInfo->regs->CFG_SC17 &
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_EV_THR_MASK) >>
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_EV_THR_SHIFT;
    edParams->deltaLumaThreshold = (scInfo->regs->CFG_SC17 &
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_LUMA_THR_MASK) >>
                CSL_VPS_SC_H_CFG_SC17_CFG_DELTA_LUMA_THR_SHIFT;
    edParams->deltaEvThreshold = (scInfo->regs->CFG_SC17 &
                CSL_VPS_SC_H_CFG_SC17_CFG_EV_THR_MASK) >>
                CSL_VPS_SC_H_CFG_SC17_CFG_EV_THR_SHIFT;
    return (ret);
}



/**
 *  static inline Void scCalVertRavParams(const VpsHal_ScConfig *config,
 *                                                UInt32 *factor,
 *                                                UInt32 *ravInit)
 *
 *  \brief  Function to calculate parameters for running average scalar. It
 *          calculates scaling factor and row accumulator initial value and
 *          returns them to the caller. The value returned from this function
 *          can be directly set into register.
 *
 *  \param  config      pointers to structure containing source and
 *                      target size
 *  \param  factor      Vertical Scaling factor
 *  \param  ravInit     Row Accumulator initial value
 *
 *  \return None
 */
static inline Void scCalVertRavParams(const VpsHal_ScConfig *config,
                                      UInt32 *factor,
                                      UInt32 *ravInit)
{
    Int initA, initB;

    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (NULL != factor));
    GT_assert( VpsHalTrace, (NULL != ravInit));

    if (config->phInfoMode == VPS_SC_SET_PHASE_INFO_FROM_APP)
    {
        *factor = config->ravScFactor;
         initA  = config->ravRowAccInit;
         initB  = config->ravRowAccInitB;
    }
    else
    {
        *factor = (UInt16)((config->tarHeight << SC_HORZ_SCALE_SHIFT) /
                            config->cropHeight);
        *factor = (*factor & CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK) <<
                     CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT;
        /* Calculate row_acc_init a and b value and set them */
        /* Currently Rajitha's method is used to calculate these values. But
         * this might be changed to Peter's method */
#ifdef SC_USE_RAJITHA_METHOD
        initA =  *factor + ((1u + *factor) >> 1u);
        if (initA >= SC_VERT_RAV_INIT_MAX)
        {
            initA -= SC_VERT_RAV_INIT_MAX;
        }
        initB = initA + (1u + (initA >> 1u)) - (SC_VERT_RAV_INIT_MAX >> 1u);
        if (initB < 0)
        {
            initB += initA;
            initA += initA;
        }
#else   /* Use Peter's method */
        float scale, delta, int_part, fract_part;
        delta = (1.0 / scale - 1.0) / 2.0;
        int_part = floor(delta);
        frac_part = delta - int_part;

        initA = Int (1024 * (scale + (1.0 - scale) / 2.0) + 0.5);
        initB = Int (1024 * (scale + (1.0 - 2.0 * frac_part) *
                        (1.0 - (1.0 + 2.0 * int_part) * scale) / 2.0) + 0.5);
#endif
    }
    *ravInit = (initA << CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_SHIFT) &
                    CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_MASK;
    *ravInit |= (initB << CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_B_SHIFT) &
                    CSL_VPS_SC_H_CFG_SC6_CFG_ROW_ACC_INIT_RAV_B_MASK;
}



/**
 *  static inline Void scCalVertPolyphaseParams(
 *                          const VpsHal_ScConfig *config,
 *                          UInt32 *rowAccInc,
 *                          UInt32 *offsetA,
 *                          UInt32 *offsetB)
 *
 *  \brief  Function to calculate parameters for polyphase vertical scalar. It
 *          calculates Row accumulator increment and offset value and returns
 *          them to the caller. The value returned from this function can be
 *          directly set into register
 *
 *  \param  config      pointers to structure containing source and
 *                      target size
 *  \param  rowAccInc   Row Accumulator Increment Value
 *  \param  offsetA     Vertical Offset for top field or frame
 *  \param  offsetB     Vertical Offset for bottom field
 *
 *  \return None
 */
static inline Void scCalVertPolyphaseParams(const VpsHal_ScConfig *config,
                                            UInt32 *rowAccInc,
                                            UInt32 *offsetA,
                                            UInt32 *offsetB)
{
    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (NULL != rowAccInc));
    GT_assert( VpsHalTrace, (NULL != offsetA));
    GT_assert( VpsHalTrace, (NULL != offsetB));

    if (config->phInfoMode == VPS_SC_SET_PHASE_INFO_FROM_APP)
    {
        *rowAccInc = config->rowAccInc;
        *offsetA   = config->rowAccOffset;
        *offsetB   = config->rowAccOffsetB;
    }
    else
    {
        *offsetA = 0u; /* TODO: How to calculate offsetA */
        if (VPS_SF_INTERLACED == config->inFrameMode)
        {
            if (VPS_SF_INTERLACED == config->outFrameMode)
            {
                    *rowAccInc = (((config->cropHeight << 2u) - 1u) <<
                                   SC_ROW_ACC_INC_SHIFT) /
                                   ((config->tarHeight << 2u) - 1u);
                    *offsetB = (((config->cropHeight - 1u) <<
                                    (SC_ROW_ACC_INC_SHIFT - 1u)) /
                                    (config->tarHeight - 1u)) - 1u;
            }
            else /* Progressive output frame */
            {
                    *rowAccInc = (((config->cropHeight << 2u) - 1u) <<
                                   SC_ROW_ACC_INC_SHIFT) /
                                   ((config->tarHeight - 1u) << 2u);
                    *offsetB = (UInt32)((Int)(1u << (SC_ROW_ACC_INC_SHIFT - 1u)) *
                                                (-1));
            }
        }
        else /* Progressive input frame */
        {
            if (VPS_SF_INTERLACED == config->outFrameMode)
            {
                    *rowAccInc = ((config->cropHeight - 1u) <<
                                  (SC_ROW_ACC_INC_SHIFT + 1u)) /
                                  ((config->tarHeight << 2u) - 1u);
                    *offsetB = ((config->cropHeight - 1u) <<
                                  SC_ROW_ACC_INC_SHIFT) /
                                  (config->tarHeight - 1u);
            }
            else /* Progressive output frame */
            {
                    *rowAccInc = ((config->cropHeight - 1u) <<
                                  SC_ROW_ACC_INC_SHIFT) /
                                  (config->tarHeight - 1u);
                    *offsetB = 0u;
            }
        }
        *offsetB += *offsetA;
    }
}



/**
 *  static inline UInt32 scCalHorzLinearAccInc(
 *                                     const Sc_ScalarInfo *scInfo,
 *                                     UInt32 *shift)
 *
 *  \brief  Function to calculate linear accumulator increment value and also
 *          shift required to calculate linear as well non-linear parameters.
 *
 *  \param  config      pointers to structure containing source and
 *                      target size
 *  \param  shift       Returns the shift by which width and internal
 *                      width should be right shifted
 *
 *  \return             Linear Accumulator increment value
 */
static inline UInt32 scCalHorzLinearAccInc(const VpsHal_ScConfig *config,
                                           UInt32 *shift)
{
    UInt32 linearAccInc = 0, hiValue, loValue, srcInnerWidth, tarInnerWidth;

    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (NULL != shift));

    if (VPS_SC_HST_AUTO == config->hsType)
    {
        if (config->tarWidth > config->cropWidth)
        {
            *shift = 0u;
        }
        else if (config->tarWidth < (config->cropWidth >> 3u))
        {
            *shift = 2u;
        }
        else if (config->tarWidth < (config->cropWidth >> 2u))
        {
            *shift = 1u;
        }
        else /* No Decimation */
        {
            /* Since Decimation will not be used, there is no need to shift */
            *shift = 0u;
        }
    }
    else
    {
        if (config->tarWidth > config->cropWidth)
        {
            *shift = 0u;
        }
        else if (config->tarWidth <= (config->cropWidth >> 2u))
        {
            *shift = 2u;
        }
        else if (config->tarWidth <= (config->cropWidth >> 1u))
        {
            *shift = 1u;
        }
        else /* No Decimation */
        {
            /* Since Decimation will not be used, there is no need to shift */
            *shift = 0u;
        }
    }

    if (TRUE == config->nonLinear)
    {
        srcInnerWidth = config->cropHeight;
        tarInnerWidth = config->tarWidth - (config->stripSize << 1u);
    }
    else /* Linear Horizontal Scaling */
    {
        srcInnerWidth = config->cropWidth;
        tarInnerWidth = config->tarWidth;
    }

    if (config->phInfoMode == VPS_SC_SET_PHASE_INFO_FROM_APP)
    {
        linearAccInc = config->linAccIncr;
    }
    else
    {
        /* Note: Minimum target inner width is 8 such that upper 3 bits required to
         * be programmed is always 0. */
        hiValue = (((srcInnerWidth >> *shift) - 1u) &
                        SC_HORZ_HIGH_VALUE_MASK) >> SC_HORZ_HIGH_VALUE_SHIFT;
        loValue = (((srcInnerWidth >> *shift) - 1u) &
                        (~SC_HORZ_HIGH_VALUE_MASK)) << SC_LINEAR_ACC_INC_SHIFT;
        linearAccInc = scLongDivide(hiValue, loValue, (tarInnerWidth - 1u));
    }
    return (linearAccInc);
}



static inline UInt32 scCalHorzColAccOffset(const VpsHal_ScConfig *config)
{
    UInt32 colAccOffset;


    if (config->phInfoMode == VPS_SC_SET_PHASE_INFO_FROM_APP)
    {
        colAccOffset = config->colAccOffset;
    }
    else
    {
        colAccOffset = 0;
    }

    return ((colAccOffset << CSL_VPS_SC_H_CFG_SC12_CFG_COL_ACC_OFFSET_SHIFT) &
                CSL_VPS_SC_H_CFG_SC12_CFG_COL_ACC_OFFSET_MASK);
}



/**
 *  static inline Void scCalHorzNonLinearParams(
 *                          const VpsHal_ScConfig *config,
 *                          UInt32 shift,
 *                          Int32 *initValue,
 *                          Int32 *incValue)
 *
 *  \brief  Function to calculate non-linear accumulator initial and
 *          increment value. It uses shift returned from scCalHorzLinearAccInc
 *          function and calculates parameters.
 *          scCalHorzLinearAccInc function must be called before calling
 *          this function in order to get value of shift                *
 *
 *  \param  config      pointers to structure containing source and
 *                      target size
 *  \param  shift       As per the equation, width and inner width are
 *                      right shifted by this value
 *  \param  initValue   Row Accumulator initial value
 *  \param  incValue    Row Accumulator increment value
 *
 *  \return None
 */
static inline Void scCalHorzNonLinearParams(const VpsHal_ScConfig *config,
                                            UInt32 shift,
                                            Int32 *initValue,
                                            Int32 *incValue)
{
    UInt32 srcLeftStrip;
    Int d, value1, valueK;
    UInt32 hiValue, loValue;

    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (NULL != initValue));
    GT_assert( VpsHalTrace, (NULL != incValue));

    /* Only nonlinear configuration will call this function so source inner
     * width will always be source height and there is no need to check wether
     * it is linear or not. */
    srcLeftStrip = ((config->cropWidth >> shift) -
                        (config->cropHeight >> shift)) >> 1u;
    if (config->tarWidth >= config->cropWidth)
    {
        d = 0;
    }
    else /* Down Scaling */
    {
        d = (Int)((config->tarWidth - 1u) >> 1u);
    }
    if (0 != config->stripSize)
    {
        /* if Minimum value for stripSize is 3 pixels, then accumulator
         * initial and increment upper value in 35 bit can be assumed to
         * zero */
        value1 = (Int)((Int)(config->stripSize) *
                                (Int)(config->stripSize - (d << 1u)));
        hiValue = (srcLeftStrip & SC_HORZ_HIGH_VALUE_MASK) >>
                        SC_HORZ_HIGH_VALUE_SHIFT;
        loValue = (srcLeftStrip & (~SC_HORZ_HIGH_VALUE_MASK)) <<
                        SC_NONLINEAR_ACC_INC_SHIFT;
        valueK = scLongDivide(hiValue, loValue, value1);
    }
    else /* Non Zero Strip Size */
    {
        valueK = 0;
    }
    *incValue = valueK << 1u;
    *initValue = valueK - (2u * valueK * d);
}



/**
 *  static inline UInt32 scMakeScCfg24Reg(const VpsHal_ScConfig *config)
 *
 *  \brief  Inline Function to calculate and get the Source image size in
 *          32 bit value. Value returned from this function can be directly
 *          set into the register.
 *
 *  \param  config      pointers to structure containing source
 *
 *  \return             source image size in 32 bit value
 */
static inline UInt32 scMakeScCfg24Reg(const VpsHal_ScConfig *config)
{
    UInt32 size;

    GT_assert( VpsHalTrace, (NULL != config));

    size = (config->srcWidth << CSL_VPS_SC_H_CFG_SC24_CFG_ORG_W_SHIFT) &
                CSL_VPS_SC_H_CFG_SC24_CFG_ORG_W_MASK;
    size |= (config->srcHeight << CSL_VPS_SC_H_CFG_SC24_CFG_ORG_H_SHIFT) &
                CSL_VPS_SC_H_CFG_SC24_CFG_ORG_H_MASK;
    return (size);
}



/**
 *  static inline UInt32 scMakeScCfg4Reg(const VpsHal_ScConfig *config)
 *
 *  \brief  Inline Function to calculate and get Target image size in 32 bit
 *          value. Value returned from this function can be directly set into
 *          the register.
 *
 *  \param  config      pointers to structure containing source
 *
 *  \return             target image size in 32 bit value
 */
static inline UInt32 scMakeScCfg4Reg(const VpsHal_ScConfig *config)
{
    UInt32 size;
    GT_assert( VpsHalTrace, (NULL != config));
    size = (config->tarWidth << CSL_VPS_SC_H_CFG_SC4_CFG_TAR_W_SHIFT) &
                CSL_VPS_SC_H_CFG_SC4_CFG_TAR_W_MASK;
    size |= (config->tarHeight << CSL_VPS_SC_H_CFG_SC4_CFG_TAR_H_SHIFT) &
              CSL_VPS_SC_H_CFG_SC4_CFG_TAR_H_MASK;
    return (size);
}



/**
 *  static inline UInt32 scMakeScCfg5Reg(const VpsHal_ScConfig *config)
 *
 *  \brief  Inline Function to calculate and get cropped image size in 32 bit
 *          value. Value returned from this function can be directly set into
 *          the register.
 *
 *  \param  config     pointers to structure containing source
 *
 *  \return            cropped image size in 32 bit value
 */
static inline UInt32 scMakeScCfg5Reg(const VpsHal_ScConfig *config)
{
    UInt32 size;
    GT_assert( VpsHalTrace, (NULL != config));
    size = (config->cropWidth << CSL_VPS_SC_H_CFG_SC5_CFG_SRC_W_SHIFT) &
                CSL_VPS_SC_H_CFG_SC5_CFG_SRC_W_MASK;
    size |= (config->cropHeight << CSL_VPS_SC_H_CFG_SC5_CFG_SRC_H_SHIFT) &
                CSL_VPS_SC_H_CFG_SC5_CFG_SRC_H_MASK;
    return (size);
}



/**
 *  static inline UInt32 scMakeScCfg25Reg(const VpsHal_ScConfig *config)
 *
 *  \brief  Inline Function to calculate and get offset from where to crop
 *          image in 32 bit value. Value returned from this function can be
 *          directly set into the register.
 *
 *  \param  config     pointers to structure containing source
 *
 *  \return            offset in 32 bit value
 */
static inline UInt32 scMakeScCfg25Reg(const VpsHal_ScConfig *config)
{
    UInt32 size;
    GT_assert( VpsHalTrace, (NULL != config));
    size = (config->cropStartX << CSL_VPS_SC_H_CFG_SC25_CFG_OFF_W_SHIFT) &
                CSL_VPS_SC_H_CFG_SC25_CFG_OFF_W_MASK;
    size |= (config->cropStartY << CSL_VPS_SC_H_CFG_SC25_CFG_OFF_H_SHIFT) &
                CSL_VPS_SC_H_CFG_SC25_CFG_OFF_H_MASK;
    return (size);
}



/**
 *  static inline UInt32 scMakeScCfg8Reg(const VpsHal_ScConfig *config)
 *
 *  \brief  Inline Function to calculate and get left and right strip size in
 *          32 bit value. Value returned from this function can be directly
 *          set into the register.
 *
 *  \param  config   pointers to structure containing source
 *
 *  \return          strip size in 32 bit value
 */
static inline UInt32 scMakeScCfg8Reg(const VpsHal_ScConfig *config)
{
    UInt32 size, innerWidth;

    GT_assert( VpsHalTrace, (NULL != config));
    /* Typically strip should be calculated using following equation.
     * left = (config->tarWidth - config->tarHeight) >> 2;
     * right = left + config->tarHeight - 1;
     * But scalar supports non-linear scaling which has inner width different
     * from height, strip size must be specified by the user. */
    size = (config->stripSize << CSL_VPS_SC_H_CFG_SC8_CFG_NLIN_LEFT_SHIFT) &
                    CSL_VPS_SC_H_CFG_SC8_CFG_NLIN_LEFT_MASK;
    innerWidth = config->tarWidth - (config->stripSize << 1u);
    size |= ((config->stripSize + innerWidth - 1u) <<
                    CSL_VPS_SC_H_CFG_SC8_CFG_NLIN_RIGHT_SHIFT) &
                    CSL_VPS_SC_H_CFG_SC8_CFG_NLIN_RIGHT_MASK;
    return (size);
}



/**
 *  static Int scCheckParams(VpsHal_Handle handle,
 *                                   const VpsHal_ScConfig *config)
 *
 *  \brief  This function checks for the valid parameters value. It checks
 *          correct value of scalar handle and other parameters
 *
 *  \param  handle      Scalar Handle
 *  \param  config      pointers to structure containing scalar parameters
 *
 *  \return 0           on success
 *          -1          on error
 */
static Int scCheckParams(VpsHal_Handle handle,
                         const VpsHal_ScConfig *config)
{
    Int ret = 0;

    GT_assert( VpsHalTrace, (NULL != config));
    GT_assert( VpsHalTrace, (NULL != handle));
    GT_assert( VpsHalTrace, (0 == scIsValidScalar(handle)));

    /* If crop and source width are same, offsets must be zero */
    if ((config->cropWidth == config->srcWidth) &&
       (config->cropHeight == config->srcHeight))
    {
        if ((0u != config->cropStartX) || (0u != config->cropStartY))
        {
            GT_0trace(VpsHalTrace, GT_ERR, "Wrong Value of CropStartX\n");
            ret = -1;
        }
    }
    /* Maximum Output width supported is 2014 pixels */
    if (SC_MAX_OUTPUT_WIDTH < config->tarWidth)
    {
        GT_0trace(VpsHalTrace, GT_ERR, "Target width is more than supported\n");
        ret = -1;
    }

    /* Check for the maximum values of target size, source size and
     * crop size */
    if ((config->tarHeight > SC_MAX_TARGET_HEIGHT) ||
       (config->srcWidth > SC_MAX_SOURCE_WIDTH) ||
       (config->srcHeight > SC_MAX_SOURCE_HEIGHT) ||
       ((config->cropWidth + config->cropStartX) > config->srcWidth) ||
       ((config->cropHeight + config->cropStartY) > config->srcHeight))
    {
        GT_0trace(VpsHalTrace, GT_ERR, "Wrong Value\n");
        ret = -1;
    }
    /* Check to see if scaling ratio is > 0.5, then 2x or 4x decimation
     * should not be used */
    if ((VPS_SC_HST_DCM_2X == config->hsType ||
        VPS_SC_HST_DCM_2X == config->hsType) &&
        (1024 *  config->tarWidth > 512 * config->cropWidth))
    {
        GT_0trace(VpsHalTrace, GT_ERR, "Wrong scaling ratio for decimation\n");
        ret = -1;
    }
    /* Minimum horizontal down scaling ratio possible is 1/16x */
    if (!SC_ISVALID_HORZ_RATIO(config->cropWidth, config->tarWidth))
    {
        GT_0trace(VpsHalTrace, GT_ERR, "Wrong Horz scaling ratio\n");
        ret = -1;
    }
    if (config->cropWidth <  SC_MIN_REQ_WIDTH)
    {
        GT_1trace(VpsHalTrace,
                  GT_ERR,
                  "Width less than minumum required width of %d\n",
                    SC_MIN_REQ_WIDTH);
        ret = -1;
    }
    if (config->cropHeight < SC_MIN_REQ_HEIGHT)
    {
        GT_1trace(VpsHalTrace,
                  GT_ERR,
                  "Height less than minumum required height of %d\n",
                  SC_MIN_REQ_HEIGHT);
        ret = -1;
    }
    return (ret);
}



/**
 *  static Void scSetScaleRatioParams(Sc_ScalarInfo *scInfo,
 *                                            const VpsHal_ScConfig *config)
 *
 *  \brief  This scaling ratio specific parameters accumulator values. It first
 *          enables specific scaling algorithm and then sets the parameters
 *          required for the scaling algorithm.
 *
 *  \param  scInfo     Pointer to structure containing instance specific
 *                     information
 *  \param  config     pointers to structure containing scalar parameters
 *
 *  \return            None
 */
static Void scSetScaleRatioParams(Sc_ScalarInfo *scInfo,
                                  const VpsHal_ScConfig *config)
{
    UInt32 shift;
    UInt32 ravFactor;
    UInt32 accInit, accIncr, offsetA, offsetB;
    Int32 nonLinearInit, nonLinearAcc;

    GT_assert( VpsHalTrace, (NULL != scInfo));
    GT_assert( VpsHalTrace, (NULL != config));

    /* Linear parameters must be calculated first to get the value of shift */
    scInfo->regs->CFG_SC9 = scCalHorzLinearAccInc(config, &shift);
    scInfo->regs->CFG_SC12 = scCalHorzColAccOffset(config);
    scInfo->regs->CFG_SC4 &= (~CSL_VPS_SC_H_CFG_SC4_CFG_LIN_ACC_INC_U_MASK);
    /* Set the size of left and right strip for nonlinear scaling*/
    scInfo->regs->CFG_SC8 = scMakeScCfg8Reg(config);

    if (TRUE == config->nonLinear)
    {
        scCalHorzNonLinearParams(
            config,
            shift,
            &nonLinearInit,
            &nonLinearAcc);
        scInfo->regs->CFG_SC10 = nonLinearInit;
        scInfo->regs->CFG_SC11 = nonLinearAcc;
        scInfo->regs->CFG_SC4 &=
            (~CSL_VPS_SC_H_CFG_SC4_CFG_NLIN_ACC_INIT_U_MASK);
        scInfo->regs->CFG_SC5 &=
            (~CSL_VPS_SC_H_CFG_SC5_CFG_NLIN_ACC_INC_U_MASK);
    }

    if (VPS_SC_VST_RAV == config->vsType)
    {
        scCalVertRavParams(config, &ravFactor, &accInit);
        scInfo->regs->CFG_SC13 &= ~CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_MASK;
        scInfo->regs->CFG_SC13 |= (ravFactor <<
                                 CSL_VPS_SC_H_CFG_SC13_CFG_SC_FACTOR_RAV_SHIFT);
        scInfo->regs->CFG_SC6 = accInit;
    }
    else /* Polyphase Vertical Scaler */
    {
        scCalVertPolyphaseParams(config, &accIncr, &offsetA, &offsetB);
        scInfo->regs->CFG_SC1 = accIncr;
        scInfo->regs->CFG_SC2 = offsetA;
        scInfo->regs->CFG_SC3 = offsetB;
    }
}

/**
 *  scGetStdHorzCoeffOvly
 *
 *  \brief  Function to return the horizontal scaling standard coefficients
 *          as per the scaling ratio.
 *          Register Overlay Memory must be allocated by using size provided
 *          by scalar.
 *  \param  handle          Scalar Handle
 *  \param  horzCoeffMemPtr Location to return a pointer to horizontal scaling
 *                          coefficients
 *  \param  config          Scalar Parameters using which it calculates the
 *                          scaling ratio
 *
 *  \return 0               on Success
 *          -1              on Error
 */
static Void scGetStdHorzCoeffOvly(VpsHal_Handle handle,
                                  const VpsHal_ScCoeffConfig *config,
                                  Ptr *horzCoeffMemPtr)
{
    if (config->hScalingSet == VPS_SC_US_SET)
    {
        *horzCoeffMemPtr = gScHorzUpScaleCoeffMemPtr;
    }
    else if (config->hScalingSet == VPS_SC_SET_1_1)
    {
        *horzCoeffMemPtr = gScHorzOneToOneScaleCoeffMemPtr;
    }
    else
    {
        *horzCoeffMemPtr = gScHorzDownScaleCoeffMemPtr +
            (config->hScalingSet * VPSHAL_SC_HORZ_COEFF_OVLY_SIZE);
    }
}

/**
 *  scGetStdVertCoeffOvly
 *
 *  \brief  Function to return the vertical scaling standard coefficients as per
 *          the scaling ratio.
 *          Register Overlay Memory must be allocated by using size provided
 *          by scalar.
 *  \param  handle          Scalar Handle
 *  \param  vertCoeffMemPtr Location to return a pointer to vertical scaling
 *                          coefficients
 *  \param  vertBilinearCoeffMemPtr Location to return a pointer to vertical
 *                          bilinear scaling coefficients (only if applicable,
 *                          i.e. for HQ scalar. Can be NULL otherwise).
 *  \param  config          Scalar Parameters using which it calculates the
 *                          scaling ratio
 *
 *  \return 0               on Success
 *          -1              on Error
 */
static Void scGetStdVertCoeffOvly(VpsHal_Handle handle,
                                  const VpsHal_ScCoeffConfig *config,
                                  Ptr *vertCoeffMemPtr,
                                  Ptr *vertBilinearCoeffMemPtr)
{
    GT_assert(VpsHalTrace, (NULL != handle));
    GT_assert(VpsHalTrace, (NULL != config));
    GT_assert(VpsHalTrace, (NULL != vertCoeffMemPtr));

    if (config->vScalingSet == VPS_SC_US_SET)
    {
        *vertCoeffMemPtr = gScVertUpScaleCoeffMemPtr;
    }
    else if (config->vScalingSet == VPS_SC_SET_1_1)
    {
        *vertCoeffMemPtr = gScVertOneToOneScaleCoeffMemPtr;
    }
    else
    {
        *vertCoeffMemPtr = gScVertDownScaleCoeffMemPtr +
            (config->vScalingSet * VPSHAL_SC_VERT_COEFF_OVLY_SIZE);
    }

    if (TRUE == (((Sc_ScalarInfo *)handle)->isHqScalar))
    {
        *vertBilinearCoeffMemPtr = gScVertBilinearCoeffMemPtr;
    }
    else
    {
        /* Return NULL for non-HQ Scalar */
        *vertBilinearCoeffMemPtr = NULL;
    }
}

/**
 *  \brief Function is used to divide 64 bit value by 32 bit value. It is used
 *  in calculating linear as well as non-linear horizontal scaling parameters
 */
static UInt32 scLongDivide(UInt32 hiValue, UInt32 loValue, UInt32 divider)
{
    Int cnt;
    UInt32 temp;
    /* Optimization is required */
    for (cnt = 1; cnt <= 32; cnt++)
    {
        temp = (int)hiValue >> 31;
        hiValue = (hiValue << 1) | (loValue >> 31);
        loValue = loValue << 1;
        if ((hiValue | temp) >= divider)
        {
            hiValue = hiValue - divider;
            loValue = loValue + 1;
        }
    }
    return loValue;
}
