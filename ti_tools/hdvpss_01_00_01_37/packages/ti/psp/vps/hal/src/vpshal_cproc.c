/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_cproc.c
 *
 * \brief CPROC  HAL file.
 * This file implements the HAL APIs of the VPS CPROC.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/cslr/cslr_vps_cproc.h>
#include <ti/psp/vps/hal/vpshal_cproc.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* the Field MaKe macro */
#define TEST_CPROC_FCC00_CCAM_BR_EN     (1)
#define TEST_CPROC_FCC00_CCAM_BYPASS    (0)                     //bypass
#define TEST_CPROC_FCC00_CCAM_YOFST     (1)

#define TEST_CPROC_FCC01_CCAM_BR_ADJ    (000u)

#if 0   // BT.709
#define TEST_CPROC_FCC01_CCAM_GY        (4767u)
#define TEST_CPROC_FCC02_CCAM_GUB       (8662u)
#define TEST_CPROC_FCC02_CCAM_GUG       (872u)
#define TEST_CPROC_FCC03_CCAM_GVG       (2187u)
#define TEST_CPROC_FCC03_CCAM_GVR       (7344u)
#define TEST_CPROC_FCC04_CCAM_M00       (4746u)
#define TEST_CPROC_FCC04_CCAM_M01       (9443u)
#define TEST_CPROC_FCC05_CCAM_M02       (633u)
#define TEST_CPROC_FCC05_CCAM_M10       (2401u)
#define TEST_CPROC_FCC06_CCAM_M11       (11244u)
#define TEST_CPROC_FCC06_CCAM_M12       (1213u)
#define TEST_CPROC_FCC07_CCAM_M20       (257u)
#define TEST_CPROC_FCC07_CCAM_M21       (1622u)
#define TEST_CPROC_FCC08_CCAM_M22       (13044u)
#define TEST_CPROC_FCC09_CCAM_S         (3389u)
#define TEST_CPROC_FCC09_CCAM_T         (3360u)
#define TEST_CPROC_FCC10_CCAM_A         (4325u)
#define TEST_CPROC_FCC10_CCAM_CZ        (5445u)
#define TEST_CPROC_ICC00_ICCAM_TEST1    (0u)
#define TEST_CPROC_ICC01_ICCAM_A        (3878u)
#define TEST_CPROC_ICC01_ICCAM_CZ       (3080u)
#define TEST_CPROC_ICC02_ICCAM_S        (4949u)
#define TEST_CPROC_ICC02_ICCAM_X0       (26888u)
#define TEST_CPROC_ICC03_ICCAM_M00      (22657u)
#define TEST_CPROC_ICC03_ICCAM_M01      (46410u)
#define TEST_CPROC_ICC04_ICCAM_M02      (679u)
#define TEST_CPROC_ICC04_ICCAM_M10      (60679u)
#define TEST_CPROC_ICC05_ICCAM_M11      (9669u)
#define TEST_CPROC_ICC05_ICCAM_M12      (64872u)
#define TEST_CPROC_ICC06_ICCAM_M20      (156u)
#define TEST_CPROC_ICC06_ICCAM_M21      (64712u)
#define TEST_CPROC_ICC07_ICCAM_M22      (4805u)
#else   // BT.601
#define TEST_CPROC_FCC01_CCAM_GY        (0x129Fu)
#define TEST_CPROC_FCC02_CCAM_GUB       (0x2049u)
#define TEST_CPROC_FCC02_CCAM_GUG       (0x0641u)
#define TEST_CPROC_FCC03_CCAM_GVG       (0x0D02u)
#define TEST_CPROC_FCC03_CCAM_GVR       (0x1989u)
#define TEST_CPROC_FCC04_CCAM_M00       (0x1940u)
#define TEST_CPROC_FCC04_CCAM_M01       (0x1D75u)
#define TEST_CPROC_FCC05_CCAM_M02       (0x0348u)
#define TEST_CPROC_FCC05_CCAM_M10       (0x0C2Du)
#define TEST_CPROC_FCC06_CCAM_M11       (0x27D9u)
#define TEST_CPROC_FCC06_CCAM_M12       (0x05F9u)
#define TEST_CPROC_FCC07_CCAM_M20       (0x007Bu)
#define TEST_CPROC_FCC07_CCAM_M21       (0x042Cu)
#define TEST_CPROC_FCC08_CCAM_M22       (0x3569u)
#define TEST_CPROC_FCC09_CCAM_S         (0x0C0Eu)
#define TEST_CPROC_FCC09_CCAM_T         (0x0D21u)
#define TEST_CPROC_FCC10_CCAM_A         (0x1179u)
#define TEST_CPROC_FCC10_CCAM_CZ        (0x1546u)
#define TEST_CPROC_ICC00_ICCAM_TEST1    (0u)
#define TEST_CPROC_ICC01_ICCAM_A        (0x0B1Bu)
#define TEST_CPROC_ICC01_ICCAM_CZ       (0x0C08u)
#define TEST_CPROC_ICC02_ICCAM_S        (0x153Bu)
#define TEST_CPROC_ICC02_ICCAM_X0       (0x6908u)
#define TEST_CPROC_ICC03_ICCAM_M00      (0x7C45u)
#define TEST_CPROC_ICC03_ICCAM_M01      (0xA3D9u)
#define TEST_CPROC_ICC04_ICCAM_M02      (0x02ACu)
#define TEST_CPROC_ICC04_ICCAM_M10      (0xD9BFu)
#define TEST_CPROC_ICC05_ICCAM_M11      (0x4F96u)
#define TEST_CPROC_ICC05_ICCAM_M12      (0xF972u)
#define TEST_CPROC_ICC06_ICCAM_M20      (0x01DEu)
#define TEST_CPROC_ICC06_ICCAM_M21      (0xFA9Cu)
#define TEST_CPROC_ICC07_ICCAM_M22      (0x2641u)
#endif

#define TEST_CPROC_CPRCT00_LVA_LCA_FEN              (0x00000000u)
#define TEST_CPROC_CPRCT00_LVA_LHA_FEN              (0x00000000u)
#define TEST_CPROC_CPRCT00_LVA_LSA_FEN              (0x00000001u)

#define TEST_CPROC_CPRCT05_LVA_LSA_SMAX_P           (0x3FFu)
#define TEST_CPROC_CPRCT05_LVA_LSA_SMIN_P           (0x000u)

#define TEST_CPROC_CPRCT03_LVA_LSA_SAT_GAIN_ENABLE  (1)
#define TEST_CPROC_CPRCT03_LVA_LSA_SAT_GAIN         (0x40)

/* each value is 12-bits.. unsigned.. with 10 integer bits.. and 2 frac bits */
#define TEST_CPROC_X_POS_EXT_REG_0      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_1      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_2      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_3      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_4      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_5      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_6      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_7      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_8      (0x000u)
#define TEST_CPROC_X_POS_EXT_REG_9      (0x000u)

/* each value is 12-bits.. signed.. with 10 integer bits.. and 2 frac bits */
#define TEST_CPROC_A_EXT_REG_0          (0x000u)
#define TEST_CPROC_A_EXT_REG_1          (0x000u)
#define TEST_CPROC_A_EXT_REG_2          (0x000u)
#define TEST_CPROC_A_EXT_REG_3          (0x000u)
#define TEST_CPROC_A_EXT_REG_4          (0x000u)
#define TEST_CPROC_A_EXT_REG_5          (0x000u)
#define TEST_CPROC_A_EXT_REG_6          (0x000u)
#define TEST_CPROC_A_EXT_REG_7          (0x000u)
#define TEST_CPROC_A_EXT_REG_8          (0x000u)

/* each value is 22-bits.. signed.. with 6 integer bits.. and 16frac bits */
#define TEST_CPROC_B_EXT_REG_0          (0x000000u)
#define TEST_CPROC_B_EXT_REG_1          (0x000000u)
#define TEST_CPROC_B_EXT_REG_2          (0x000000u)
#define TEST_CPROC_B_EXT_REG_3          (0x000000u)
#define TEST_CPROC_B_EXT_REG_4          (0x000000u)
#define TEST_CPROC_B_EXT_REG_5          (0x000000u)
#define TEST_CPROC_B_EXT_REG_6          (0x000000u)
#define TEST_CPROC_B_EXT_REG_7          (0x000000u)
#define TEST_CPROC_B_EXT_REG_8          (0x000000u)


#define VPSHAL_CPROC_MAKE_X_POS_EXT_REG(a, b)                       \
        ((( (a) << CSL_VPS_CPROC_CPRCT11_X_POS_EXT_REG_0_SHIFT) &    \
            CSL_VPS_CPROC_CPRCT11_X_POS_EXT_REG_0_MASK ) |          \
        (( (b) << CSL_VPS_CPROC_CPRCT11_X_POS_EXT_REG_1_SHIFT) &    \
            CSL_VPS_CPROC_CPRCT11_X_POS_EXT_REG_1_MASK ))

#define VPSHAL_CPROC_MAKE_A_EXT_REG(a, b)                           \
        ((( (a) << CSL_VPS_CPROC_CPRCT16_A_EXT_REG_0_SHIFT) &        \
            CSL_VPS_CPROC_CPRCT16_A_EXT_REG_0_MASK ) |              \
        (( (b) << CSL_VPS_CPROC_CPRCT16_A_EXT_REG_1_SHIFT) &        \
            CSL_VPS_CPROC_CPRCT16_A_EXT_REG_1_MASK ))

#define VPSHAL_CPROC_MAKE_B_EXT_REG(a)                              \
        (( (a) << CSL_VPS_CPROC_CPRCT24_B_EXT_REG_3_SHIFT) &        \
            CSL_VPS_CPROC_CPRCT24_B_EXT_REG_3_MASK )


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */



/**
 *  struct Comp_Info
 *  \brief Structure containing per  instance information for each CPROC.
 *
 */
typedef struct
{
    UInt32             instId;
    /**< CPROC instance ID */
    UInt32             openCnt;
    /**< Counter to keep track of number of open calls for an instance */
    CSL_Vps_cprocRegs *regOvly;
    /**< Pointer to the DEI register overlay */
    Vps_CprocConfig    cprocCfg;
} Cproc_Obj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void halCprocSetExpertConfig(Cproc_Obj *cprocObj);


/* ========================================================================== */
/*                          Global Variables                                  */
/* ========================================================================== */

/**
 *  CprocObjects
 *  \brief Module variable to store information about each CPROC
 *  instance.
 *  Note: If the number of CPROC instance increase, then this variable should be
 *  initialized with the added CPROC instance information.
 */
static Cproc_Obj CprocObjects[VPSHAL_CPROC_MAX_INST];

static Vps_CprocCiecamCfg CprocCiecamCfg[VPS_CPROC_CIECAM_MODE_NONE] = {
    {
        {
            {0x1940u, 0x1D75u, 0x0348u},
            {0x0C2Du, 0x27D9u, 0x05F9u},
            {0x007Bu, 0x042Cu, 0x3569u}
        },
        0x0C0Eu, 0x0D21u, 0x1179u, 0x1546u, /* S, T, A, CZ */
        0x129Fu, 0x2049u, 0x0641u, 0x0D02u, 0x1989u /* GY, GUB, GUG, GVG, GVR */
    },
    {
        {
            {4746u, 9443u, 633u},
            {2401u, 11244u, 1213u},
            {257u, 1622u, 13044u}
        },
        3389u, 3360u, 4325u, 5445u, /* S, T, A, CZ */
        4767u, 8662u, 872u, 2187u, 7344u /* GY, GUB, GUG, GVG, GVR */
    }
};

static Vps_CprocICiecamCfg ICprocCiecamCfg[VPS_CPROC_CIECAM_MODE_NONE] = {
    {
        {
            {0x7C45u, 0xA3D9u, 0x02ACu},
            {0xD9BFu, 0x4F96u, 0xF972u},
            {0x01DEu, 0xFA9Cu, 0x2641u}
        },
        0x0B1Bu, 0x0C08u, 0x153Bu, 0x6908u /* A, CZ, S, X0 */
    },
    {
        {
            {22657u, 46410u, 679u},
            {60679u, 9669u, 64872u},
            {156u, 64712u, 4805u}
        },
        3878u, 3080u, 4949u, 26888u /* A, CZ, S, X0 */
    }
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/**
 *  VpsHal_cprocInit
 *  \brief CPROC HAL init function.
 *
 *  Initializes CPROC objects, gets the register overlay offsets for CPROC
 *  registers.
 *  This function should be called before calling any of CPROC HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of CPROC HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_cprocInit(UInt32 numInstances,
                     VpsHal_CprocInstParams *cprocInstParams,
                     Ptr arg)
{
    Int                 instCnt;
    Cproc_Obj          *cprocObj;
    Int                 ret     = 0;

    GT_assert( GT_DEFAULT_MASK, NULL != cprocInstParams);
    GT_assert( GT_DEFAULT_MASK, numInstances <= VPSHAL_CPROC_MAX_INST);

    VpsUtils_memset(CprocObjects, 0, sizeof(CprocObjects));
    for (instCnt = 0; instCnt < numInstances; instCnt++)
    {
        GT_assert( GT_DEFAULT_MASK, cprocInstParams[instCnt].instId < VPSHAL_CPROC_MAX_INST);

        cprocObj = &CprocObjects[instCnt];

        cprocObj->instId = cprocInstParams[instCnt].instId;
        cprocObj->regOvly =
                (CSL_Vps_cprocRegs *)cprocInstParams[instCnt].baseAddress;

        /* By default setting the Ciecam mode to BT601 */
        cprocObj->cprocCfg.ciecamMode = VPS_CPROC_CIECAM_MODE_BT601;
        VpsUtils_memcpy(&cprocObj->cprocCfg.ciecamCfg,
                        &CprocCiecamCfg[cprocObj->cprocCfg.ciecamMode],
                        sizeof(Vps_CprocCiecamCfg));
        VpsUtils_memcpy(&cprocObj->cprocCfg.iciecamCfg,
                        &ICprocCiecamCfg[cprocObj->cprocCfg.ciecamMode],
                        sizeof(Vps_CprocICiecamCfg));
        halCprocSetExpertConfig(cprocObj);
    }

    return (ret);
}



/**
 *  VpsHal_cprocDeInit
 *  \brief CPROC HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_cprocDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_cprocOpen
 *  \brief Returns the handle to the requested CPROC instance.
 *
 *  This function should be called prior to calling any of the CPROC HAL
 *  configuration APIs to get the instance handle.
 *
 *  Cproc_init should be called prior to this.
 *
 *  \param cprocInst Requested CPROC instance.
 *
 *  \return          Returns CPROC instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_cprocOpen(UInt32 cprocInst)
{
    Int         cnt;
    UInt32      cookie;
    VpsHal_Handle handle;


    handle = NULL;

    for (cnt = 0; cnt < VPSHAL_CPROC_MAX_INST; cnt++)
    {
        /* Return the matching instance handle */
        if (cprocInst == CprocObjects[cnt].instId)
        {
            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Check whether some one has already opened this instance */
            if (0 == CprocObjects[cnt].openCnt)
            {
                handle = (VpsHal_Handle) &CprocObjects[cnt];
                CprocObjects[cnt].openCnt++;
            }

            /* Enable global interrupts */
            Hwi_restore(cookie);

            break;
        }
    }
    return (handle);
}




/**
 *  VpsHal_cprocClose
 *  \brief Closes the CPROC HAL instance
 *
 *  Cproc_init and Comp_open should be called prior to this.
 *
 *  \param handle Valid handle returned by Cproc_open function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_cprocClose(VpsHal_Handle handle)
{
    Int         ret     = -1;
    UInt32      cookie;
    Cproc_Obj   *cprocObj;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    cprocObj = (Cproc_Obj *) handle;

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Decrement the open count */
    if (cprocObj->openCnt > 0)
    {
        cprocObj->openCnt--;
        ret = 0;
    }

    /* Enable global interrupts */
    Hwi_restore(cookie);

    return (ret);
}



Int VpsHal_cprocSetFrameSize(VpsHal_Handle handle,
                             UInt32 width,
                             UInt32 height,
                             Ptr ptr)
{
    CSL_Vps_cprocRegs      *regOvly = NULL;

    /* Check for NULL pointer */
    GT_assert(GT_DEFAULT_MASK, NULL != handle);

    regOvly = ((Cproc_Obj *) handle)->regOvly;

    regOvly->CPRCT00 =
        ((TEST_CPROC_CPRCT00_LVA_LCA_FEN <<
            CSL_VPS_CPROC_CPRCT00_LVA_LCA_FEN_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_LCA_FEN_MASK) |
        ((TEST_CPROC_CPRCT00_LVA_LHA_FEN <<
            CSL_VPS_CPROC_CPRCT00_LVA_LHA_FEN_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_LHA_FEN_MASK) |
        ((TEST_CPROC_CPRCT00_LVA_LSA_FEN <<
            CSL_VPS_CPROC_CPRCT00_LVA_LSA_FEN_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_LSA_FEN_MASK) |
        ((height <<
            CSL_VPS_CPROC_CPRCT00_LVA_NO_LINES_PER_FRAME_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_NO_LINES_PER_FRAME_MASK) |
        ((width <<
            CSL_VPS_CPROC_CPRCT00_LVA_NO_PIXELS_PER_LINE_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_NO_PIXELS_PER_LINE_MASK);

    return (VPS_SOK);
}



Int32 VpsHal_cprocSetConfig(VpsHal_Handle handle,
                            Vps_CprocConfig *cprocCfg,
                            Ptr ptr)
{
    Cproc_Obj              *cprocObj = NULL;
    CSL_Vps_cprocRegs      *regOvly = NULL;
    Vps_CprocCiecamCfg     *ciecamCfg;
    Vps_CprocICiecamCfg    *iciecamCfg;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    cprocObj = (Cproc_Obj *) handle;
    regOvly = cprocObj->regOvly;

    GT_assert( GT_DEFAULT_MASK, (cprocCfg->ciecamMode <= VPS_CPROC_CIECAM_MODE_NONE));

    if (VPS_CPROC_CIECAM_MODE_NONE == cprocCfg->ciecamMode)
    {
        ciecamCfg = &cprocCfg->ciecamCfg;
        iciecamCfg = &cprocCfg->iciecamCfg;
    }
    else
    {
        ciecamCfg = &CprocCiecamCfg[cprocCfg->ciecamMode];
        iciecamCfg = &ICprocCiecamCfg[cprocCfg->ciecamMode];
    }

    /* Set the Ciecam Coefficients */
    regOvly->FCC01 |=
        ((ciecamCfg->GY <<
            CSL_VPS_CPROC_FCC01_CCAM_GY_SHIFT) &
            CSL_VPS_CPROC_FCC01_CCAM_GY_MASK);
    regOvly->FCC02 =
        ((ciecamCfg->GUB <<
            CSL_VPS_CPROC_FCC02_CCAM_GUB_SHIFT) &
            CSL_VPS_CPROC_FCC02_CCAM_GUB_MASK) |
        ((ciecamCfg->GUG <<
            CSL_VPS_CPROC_FCC02_CCAM_GUG_SHIFT) &
            CSL_VPS_CPROC_FCC02_CCAM_GUG_MASK);
    regOvly->FCC03 =
        ((ciecamCfg->GVG <<
            CSL_VPS_CPROC_FCC03_CCAM_GVG_SHIFT) &
            CSL_VPS_CPROC_FCC03_CCAM_GVG_MASK) |
        ((ciecamCfg->GVR <<
            CSL_VPS_CPROC_FCC03_CCAM_GVR_SHIFT) &
            CSL_VPS_CPROC_FCC03_CCAM_GVR_MASK);


    regOvly->FCC04 =
        ((ciecamCfg->multCoeff[0][0] <<
            CSL_VPS_CPROC_FCC04_CCAM_M00_SHIFT) &
            CSL_VPS_CPROC_FCC04_CCAM_M00_MASK) |
        ((ciecamCfg->multCoeff[0][1] <<
            CSL_VPS_CPROC_FCC04_CCAM_M01_SHIFT) &
            CSL_VPS_CPROC_FCC04_CCAM_M01_MASK);
    regOvly->FCC05 =
        ((ciecamCfg->multCoeff[0][2] <<
            CSL_VPS_CPROC_FCC05_CCAM_M02_SHIFT) &
            CSL_VPS_CPROC_FCC05_CCAM_M02_MASK) |
        ((ciecamCfg->multCoeff[1][0] <<
            CSL_VPS_CPROC_FCC05_CCAM_M10_SHIFT) &
            CSL_VPS_CPROC_FCC05_CCAM_M10_MASK);
    regOvly->FCC06 =
        ((ciecamCfg->multCoeff[1][1] <<
            CSL_VPS_CPROC_FCC06_CCAM_M11_SHIFT) &
            CSL_VPS_CPROC_FCC06_CCAM_M11_MASK) |
        ((ciecamCfg->multCoeff[1][2] <<
            CSL_VPS_CPROC_FCC06_CCAM_M12_SHIFT) &
            CSL_VPS_CPROC_FCC06_CCAM_M12_MASK);
    regOvly->FCC07 =
        ((ciecamCfg->multCoeff[2][0] <<
            CSL_VPS_CPROC_FCC07_CCAM_M20_SHIFT) &
            CSL_VPS_CPROC_FCC07_CCAM_M20_MASK) |
        ((ciecamCfg->multCoeff[2][1] <<
            CSL_VPS_CPROC_FCC07_CCAM_M21_SHIFT) &
            CSL_VPS_CPROC_FCC07_CCAM_M21_MASK);
    regOvly->FCC08 =
        (ciecamCfg->multCoeff[2][2] <<
            CSL_VPS_CPROC_FCC08_CCAM_M22_SHIFT) &
            CSL_VPS_CPROC_FCC08_CCAM_M22_MASK;

    regOvly->FCC09 =
        ((ciecamCfg->S <<
            CSL_VPS_CPROC_FCC09_CCAM_S_SHIFT) &
            CSL_VPS_CPROC_FCC09_CCAM_S_MASK) |
        ((ciecamCfg->T <<
            CSL_VPS_CPROC_FCC09_CCAM_T_SHIFT) &
            CSL_VPS_CPROC_FCC09_CCAM_T_MASK);
    regOvly->FCC10 =
        ((ciecamCfg->A <<
            CSL_VPS_CPROC_FCC10_CCAM_A_SHIFT) &
            CSL_VPS_CPROC_FCC10_CCAM_A_MASK) |
        ((ciecamCfg->CZ <<
            CSL_VPS_CPROC_FCC10_CCAM_CZ_SHIFT) &
            CSL_VPS_CPROC_FCC10_CCAM_CZ_MASK);

    /* Set the Inverse Ciecam Coefficients */
    regOvly->ICC01 =
        ((iciecamCfg->A <<
            CSL_VPS_CPROC_ICC01_ICCAM_A_SHIFT) &
            CSL_VPS_CPROC_ICC01_ICCAM_A_MASK) |
        ((iciecamCfg->CZ <<
            CSL_VPS_CPROC_ICC01_ICCAM_CZ_SHIFT) &
            CSL_VPS_CPROC_ICC01_ICCAM_CZ_MASK);
    regOvly->ICC02 =
        ((iciecamCfg->S <<
            CSL_VPS_CPROC_ICC02_ICCAM_S_SHIFT) &
            CSL_VPS_CPROC_ICC02_ICCAM_S_MASK) |
        ((iciecamCfg->X0 <<
            CSL_VPS_CPROC_ICC02_ICCAM_X0_SHIFT) &
            CSL_VPS_CPROC_ICC02_ICCAM_X0_MASK);

    regOvly->ICC03 =
        ((iciecamCfg->multCoeff[0][0] <<
            CSL_VPS_CPROC_ICC03_ICCAM_M00_SHIFT) &
            CSL_VPS_CPROC_ICC03_ICCAM_M00_MASK) |
        ((iciecamCfg->multCoeff[0][1] <<
            CSL_VPS_CPROC_ICC03_ICCAM_M01_SHIFT) &
            CSL_VPS_CPROC_ICC03_ICCAM_M01_MASK);
    regOvly->ICC04 =
        ((iciecamCfg->multCoeff[0][2] <<
            CSL_VPS_CPROC_ICC04_ICCAM_M02_SHIFT) &
            CSL_VPS_CPROC_ICC04_ICCAM_M02_MASK) |
        ((iciecamCfg->multCoeff[1][0] <<
            CSL_VPS_CPROC_ICC04_ICCAM_M10_SHIFT) &
            CSL_VPS_CPROC_ICC04_ICCAM_M10_MASK);
    regOvly->ICC05 =
        ((iciecamCfg->multCoeff[1][1] <<
            CSL_VPS_CPROC_ICC05_ICCAM_M11_SHIFT) &
            CSL_VPS_CPROC_ICC05_ICCAM_M11_MASK) |
        ((iciecamCfg->multCoeff[1][2] <<
            CSL_VPS_CPROC_ICC05_ICCAM_M12_SHIFT) &
            CSL_VPS_CPROC_ICC05_ICCAM_M12_MASK);
    regOvly->ICC06 =
        ((iciecamCfg->multCoeff[2][0] <<
            CSL_VPS_CPROC_ICC06_ICCAM_M20_SHIFT) &
            CSL_VPS_CPROC_ICC06_ICCAM_M20_MASK) |
        ((iciecamCfg->multCoeff[2][1] <<
            CSL_VPS_CPROC_ICC06_ICCAM_M21_SHIFT) &
            CSL_VPS_CPROC_ICC06_ICCAM_M21_MASK);
    regOvly->ICC07 =
        ((iciecamCfg->multCoeff[2][2] <<
            CSL_VPS_CPROC_ICC07_ICCAM_M22_SHIFT) &
            CSL_VPS_CPROC_ICC07_ICCAM_M22_MASK);

    VpsUtils_memcpy(&cprocObj->cprocCfg.ciecamCfg,
                    ciecamCfg,
                    sizeof(cprocObj->cprocCfg.ciecamCfg));
    VpsUtils_memcpy(&cprocObj->cprocCfg.iciecamCfg,
                    iciecamCfg,
                    sizeof(cprocObj->cprocCfg.iciecamCfg));
    cprocObj->cprocCfg.ciecamMode = cprocCfg->ciecamMode;

    return (VPS_SOK);
}



Int32 VpsHal_cprocGetConfig(VpsHal_Handle handle,
                            Vps_CprocConfig *cprocCfg)
{
    Cproc_Obj              *cprocObj = NULL;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != handle);

    cprocObj = (Cproc_Obj *) handle;

    VpsUtils_memcpy(cprocCfg,
                    &cprocObj->cprocCfg,
                    sizeof(cprocObj->cprocCfg));

    return (VPS_SOK);
}



static Void halCprocSetExpertConfig(Cproc_Obj *cprocObj)
{
    CSL_Vps_cprocRegs      *regOvly = NULL;

    regOvly = cprocObj->regOvly;

    regOvly->FCC00 =
        ((TEST_CPROC_FCC00_CCAM_BR_EN <<
            CSL_VPS_CPROC_FCC00_CCAM_BR_EN_SHIFT) &
            CSL_VPS_CPROC_FCC00_CCAM_BR_EN_MASK) |
        ((TEST_CPROC_FCC00_CCAM_BYPASS <<
            CSL_VPS_CPROC_FCC00_CCAM_BYPASS_SHIFT) &
            CSL_VPS_CPROC_FCC00_CCAM_BYPASS_MASK) |
        ((TEST_CPROC_FCC00_CCAM_YOFST <<
            CSL_VPS_CPROC_FCC00_CCAM_YOFST_SHIFT) &
            CSL_VPS_CPROC_FCC00_CCAM_YOFST_MASK);

    regOvly->FCC01 =
        ((TEST_CPROC_FCC01_CCAM_BR_ADJ <<
            CSL_VPS_CPROC_FCC01_CCAM_BR_ADJ_SHIFT) &
            CSL_VPS_CPROC_FCC01_CCAM_BR_ADJ_MASK) |
        ((TEST_CPROC_FCC01_CCAM_GY <<
            CSL_VPS_CPROC_FCC01_CCAM_GY_SHIFT) &
            CSL_VPS_CPROC_FCC01_CCAM_GY_MASK);

    regOvly->FCC02 =
        ((TEST_CPROC_FCC02_CCAM_GUB <<
            CSL_VPS_CPROC_FCC02_CCAM_GUB_SHIFT) &
            CSL_VPS_CPROC_FCC02_CCAM_GUB_MASK) |
        ((TEST_CPROC_FCC02_CCAM_GUG <<
            CSL_VPS_CPROC_FCC02_CCAM_GUG_SHIFT) &
            CSL_VPS_CPROC_FCC02_CCAM_GUG_MASK);

    regOvly->FCC03 =
        ((TEST_CPROC_FCC03_CCAM_GVG <<
            CSL_VPS_CPROC_FCC03_CCAM_GVG_SHIFT) &
            CSL_VPS_CPROC_FCC03_CCAM_GVG_MASK) |
        ((TEST_CPROC_FCC03_CCAM_GVR <<
            CSL_VPS_CPROC_FCC03_CCAM_GVR_SHIFT) &
            CSL_VPS_CPROC_FCC03_CCAM_GVR_MASK);

    regOvly->FCC04 =
        ((TEST_CPROC_FCC04_CCAM_M00 <<
            CSL_VPS_CPROC_FCC04_CCAM_M00_SHIFT) &
            CSL_VPS_CPROC_FCC04_CCAM_M00_MASK) |
        ((TEST_CPROC_FCC04_CCAM_M01 <<
            CSL_VPS_CPROC_FCC04_CCAM_M01_SHIFT) &
            CSL_VPS_CPROC_FCC04_CCAM_M01_MASK);

    regOvly->FCC05 =
        ((TEST_CPROC_FCC05_CCAM_M02 <<
            CSL_VPS_CPROC_FCC05_CCAM_M02_SHIFT) &
            CSL_VPS_CPROC_FCC05_CCAM_M02_MASK) |
        ((TEST_CPROC_FCC05_CCAM_M10 <<
            CSL_VPS_CPROC_FCC05_CCAM_M10_SHIFT) &
            CSL_VPS_CPROC_FCC05_CCAM_M10_MASK);

    regOvly->FCC06 =
        ((TEST_CPROC_FCC06_CCAM_M11 <<
            CSL_VPS_CPROC_FCC06_CCAM_M11_SHIFT) &
            CSL_VPS_CPROC_FCC06_CCAM_M11_MASK) |
        ((TEST_CPROC_FCC06_CCAM_M12 <<
            CSL_VPS_CPROC_FCC06_CCAM_M12_SHIFT) &
            CSL_VPS_CPROC_FCC06_CCAM_M12_MASK);

    regOvly->FCC07 =
        ((TEST_CPROC_FCC07_CCAM_M20 <<
            CSL_VPS_CPROC_FCC07_CCAM_M20_SHIFT) &
            CSL_VPS_CPROC_FCC07_CCAM_M20_MASK) |
        ((TEST_CPROC_FCC07_CCAM_M21 <<
            CSL_VPS_CPROC_FCC07_CCAM_M21_SHIFT) &
            CSL_VPS_CPROC_FCC07_CCAM_M21_MASK);

    regOvly->FCC08 =
        (TEST_CPROC_FCC08_CCAM_M22 <<
            CSL_VPS_CPROC_FCC08_CCAM_M22_SHIFT) &
            CSL_VPS_CPROC_FCC08_CCAM_M22_MASK;

    regOvly->FCC09 =
        ((TEST_CPROC_FCC09_CCAM_S <<
            CSL_VPS_CPROC_FCC09_CCAM_S_SHIFT) &
            CSL_VPS_CPROC_FCC09_CCAM_S_MASK) |
        ((TEST_CPROC_FCC09_CCAM_T <<
            CSL_VPS_CPROC_FCC09_CCAM_T_SHIFT) &
            CSL_VPS_CPROC_FCC09_CCAM_T_MASK);

    regOvly->FCC10 =
        ((TEST_CPROC_FCC10_CCAM_A <<
            CSL_VPS_CPROC_FCC10_CCAM_A_SHIFT) &
            CSL_VPS_CPROC_FCC10_CCAM_A_MASK) |
        ((TEST_CPROC_FCC10_CCAM_CZ <<
            CSL_VPS_CPROC_FCC10_CCAM_CZ_SHIFT) &
            CSL_VPS_CPROC_FCC10_CCAM_CZ_MASK);

    regOvly->ICC00 =
        (TEST_CPROC_ICC00_ICCAM_TEST1 <<
            CSL_VPS_CPROC_ICC00_ICCAM_TEST1_SHIFT) &
            CSL_VPS_CPROC_ICC00_ICCAM_TEST1_MASK;

    regOvly->ICC01 =
        ((TEST_CPROC_ICC01_ICCAM_A <<
            CSL_VPS_CPROC_ICC01_ICCAM_A_SHIFT) &
            CSL_VPS_CPROC_ICC01_ICCAM_A_MASK) |
        ((TEST_CPROC_ICC01_ICCAM_CZ <<
            CSL_VPS_CPROC_ICC01_ICCAM_CZ_SHIFT) &
            CSL_VPS_CPROC_ICC01_ICCAM_CZ_MASK);

    regOvly->ICC02 =
        ((TEST_CPROC_ICC02_ICCAM_S <<
            CSL_VPS_CPROC_ICC02_ICCAM_S_SHIFT) &
            CSL_VPS_CPROC_ICC02_ICCAM_S_MASK) |
        ((TEST_CPROC_ICC02_ICCAM_X0 <<
            CSL_VPS_CPROC_ICC02_ICCAM_X0_SHIFT) &
            CSL_VPS_CPROC_ICC02_ICCAM_X0_MASK);

    regOvly->ICC03 =
        ((TEST_CPROC_ICC03_ICCAM_M00 <<
            CSL_VPS_CPROC_ICC03_ICCAM_M00_SHIFT) &
            CSL_VPS_CPROC_ICC03_ICCAM_M00_MASK) |
        ((TEST_CPROC_ICC03_ICCAM_M01 <<
            CSL_VPS_CPROC_ICC03_ICCAM_M01_SHIFT) &
            CSL_VPS_CPROC_ICC03_ICCAM_M01_MASK);

    regOvly->ICC04 =
        ((TEST_CPROC_ICC04_ICCAM_M02 <<
            CSL_VPS_CPROC_ICC04_ICCAM_M02_SHIFT) &
            CSL_VPS_CPROC_ICC04_ICCAM_M02_MASK) |
        ((TEST_CPROC_ICC04_ICCAM_M10 <<
            CSL_VPS_CPROC_ICC04_ICCAM_M10_SHIFT) &
            CSL_VPS_CPROC_ICC04_ICCAM_M10_MASK);

    regOvly->ICC05 =
        ((TEST_CPROC_ICC05_ICCAM_M11 <<
            CSL_VPS_CPROC_ICC05_ICCAM_M11_SHIFT) &
            CSL_VPS_CPROC_ICC05_ICCAM_M11_MASK) |
        ((TEST_CPROC_ICC05_ICCAM_M12 <<
            CSL_VPS_CPROC_ICC05_ICCAM_M12_SHIFT) &
            CSL_VPS_CPROC_ICC05_ICCAM_M12_MASK);

    regOvly->ICC06 =
        ((TEST_CPROC_ICC06_ICCAM_M20 <<
            CSL_VPS_CPROC_ICC06_ICCAM_M20_SHIFT) &
            CSL_VPS_CPROC_ICC06_ICCAM_M20_MASK) |
        ((TEST_CPROC_ICC06_ICCAM_M21 <<
            CSL_VPS_CPROC_ICC06_ICCAM_M21_SHIFT) &
            CSL_VPS_CPROC_ICC06_ICCAM_M21_MASK);

    regOvly->ICC07 =
        ((TEST_CPROC_ICC07_ICCAM_M22 <<
            CSL_VPS_CPROC_ICC07_ICCAM_M22_SHIFT) &
            CSL_VPS_CPROC_ICC07_ICCAM_M22_MASK);

    regOvly->CPRCT00 &= ~(CSL_VPS_CPROC_CPRCT00_LVA_LCA_FEN_MASK |
                          CSL_VPS_CPROC_CPRCT00_LVA_LHA_FEN_MASK |
                          CSL_VPS_CPROC_CPRCT00_LVA_LSA_FEN_MASK);
    regOvly->CPRCT00 |=
        ((TEST_CPROC_CPRCT00_LVA_LCA_FEN <<
            CSL_VPS_CPROC_CPRCT00_LVA_LCA_FEN_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_LCA_FEN_MASK) |
        ((TEST_CPROC_CPRCT00_LVA_LHA_FEN <<
            CSL_VPS_CPROC_CPRCT00_LVA_LHA_FEN_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_LHA_FEN_MASK) |
        ((TEST_CPROC_CPRCT00_LVA_LSA_FEN <<
            CSL_VPS_CPROC_CPRCT00_LVA_LSA_FEN_SHIFT) &
            CSL_VPS_CPROC_CPRCT00_LVA_LSA_FEN_MASK);

    regOvly->CPRCT05 =
        ((TEST_CPROC_CPRCT05_LVA_LSA_SMAX_P <<
          CSL_VPS_CPROC_CPRCT05_LVA_LSA_SMAX_P_SHIFT  ) &
         CSL_VPS_CPROC_CPRCT05_LVA_LSA_SMAX_P_MASK   ) |
        ((TEST_CPROC_CPRCT05_LVA_LSA_SMIN_P <<
          CSL_VPS_CPROC_CPRCT05_LVA_LSA_SMIN_P_SHIFT  ) &
         CSL_VPS_CPROC_CPRCT05_LVA_LSA_SMIN_P_MASK   );

    regOvly->CPRCT03 =
        ((TEST_CPROC_CPRCT03_LVA_LSA_SAT_GAIN_ENABLE <<
            CSL_VPS_CPROC_CPRCT03_LVA_LSA_IMAGE_SAT_GAIN_EN_SHIFT ) &
        CSL_VPS_CPROC_CPRCT03_LVA_LSA_IMAGE_SAT_GAIN_EN_MASK ) |
        ((TEST_CPROC_CPRCT03_LVA_LSA_SAT_GAIN <<
            CSL_VPS_CPROC_CPRCT03_LVA_IMAGE_SAT_GAIN_SHIFT ) &
        CSL_VPS_CPROC_CPRCT03_LVA_IMAGE_SAT_GAIN_MASK );

    regOvly->CPRCT11 =
        VPSHAL_CPROC_MAKE_X_POS_EXT_REG(
            TEST_CPROC_X_POS_EXT_REG_0,
            TEST_CPROC_X_POS_EXT_REG_1
            );

    regOvly->CPRCT12 =
        VPSHAL_CPROC_MAKE_X_POS_EXT_REG(
            TEST_CPROC_X_POS_EXT_REG_2,
            TEST_CPROC_X_POS_EXT_REG_3
            );

    regOvly->CPRCT13 =
        VPSHAL_CPROC_MAKE_X_POS_EXT_REG(
            TEST_CPROC_X_POS_EXT_REG_4,
            TEST_CPROC_X_POS_EXT_REG_5
            );

    regOvly->CPRCT14 =
        VPSHAL_CPROC_MAKE_X_POS_EXT_REG(
            TEST_CPROC_X_POS_EXT_REG_6,
            TEST_CPROC_X_POS_EXT_REG_7
            );

    regOvly->CPRCT15 =
        VPSHAL_CPROC_MAKE_X_POS_EXT_REG(
            TEST_CPROC_X_POS_EXT_REG_8,
            TEST_CPROC_X_POS_EXT_REG_9
            );

    regOvly->CPRCT16 =
        VPSHAL_CPROC_MAKE_A_EXT_REG(
            TEST_CPROC_A_EXT_REG_0,
            TEST_CPROC_A_EXT_REG_1
            );

    regOvly->CPRCT17 =
        VPSHAL_CPROC_MAKE_A_EXT_REG(
            TEST_CPROC_A_EXT_REG_2,
            TEST_CPROC_A_EXT_REG_3
            );

    regOvly->CPRCT18 =
        VPSHAL_CPROC_MAKE_A_EXT_REG(
            TEST_CPROC_A_EXT_REG_4,
            TEST_CPROC_A_EXT_REG_5
            );

    regOvly->CPRCT19 =
        VPSHAL_CPROC_MAKE_A_EXT_REG(
            TEST_CPROC_A_EXT_REG_6,
            TEST_CPROC_A_EXT_REG_7
            );

    regOvly->CPRCT20 =
        VPSHAL_CPROC_MAKE_A_EXT_REG(
            TEST_CPROC_A_EXT_REG_8,
            0
            );

    regOvly->CPRCT21 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_0
            );

    regOvly->CPRCT22 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_1
            );

    regOvly->CPRCT23 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_2
            );

    regOvly->CPRCT24 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_3
            );

    regOvly->CPRCT25 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_4
            );

    regOvly->CPRCT26 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_5
            );

    regOvly->CPRCT27 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_6
            );

    regOvly->CPRCT28 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_7
            );

    regOvly->CPRCT29 =
        VPSHAL_CPROC_MAKE_B_EXT_REG(
            TEST_CPROC_B_EXT_REG_8
            );
}
