/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDeiCore.c
 *
 *  \brief VPS DEI M2M driver internal file used for core interactions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/drivers/m2m/src/vpsdrv_m2mDeiPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 vpsMdrvDeiCfgDeiCore(VpsMdrv_DeiHandleObj *hObj,
                                  VpsMdrv_DeiChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps);
static Int32 vpsMdrvDeiCfgDeiHqCore(VpsMdrv_DeiHandleObj *hObj,
                                    VpsMdrv_DeiChObj *chObj,
                                    Vcore_Handle coreHandle,
                                    const Vcore_Ops *coreOps);
static Int32 vpsMdrvDeiCfgDeiWrbkCore(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj,
                                      Vcore_Handle coreHandle,
                                      const Vcore_Ops *coreOps);
static Int32 vpsMdrvDeiCfgVipCore(VpsMdrv_DeiHandleObj *hObj,
                                  VpsMdrv_DeiChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps);
static Int32 vpsMdrvDeiAlcCfgVipCore(VpsMdrv_DeiHandleObj *hObj,
                                     Vcore_Handle coreHandle,
                                     const Vcore_Ops *coreOps);

static Int32 vpsMdrvDeiGetSocCh(VpsMdrv_DeiHandleObj *hObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vpsMdrvDeiOpenCores
 *  Opens and configures all the cores required for this handle.
 */
Int32 vpsMdrvDeiOpenCores(VpsMdrv_DeiHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              perChCfg;
    UInt32              chCnt, coreCnt;
    VpsMdrv_DeiInstObj *instObj;
    const Vcore_Ops    *coreOps;
    Vcore_Property      property;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
    {
        perChCfg = FALSE;
    }
    else
    {
        perChCfg = TRUE;
    }

    /* Reset the handles first */
    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        hObj->coreHandle[coreCnt] = NULL;
    }

    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        /* Open and configure core only if needed by that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->open));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getProperty));

            /* Open core */
            hObj->coreHandle[coreCnt] = coreOps->open(
                                            instObj->coreInstId[coreCnt],
                                            VCORE_OPMODE_MEMORY,
                                            hObj->numCh,
                                            perChCfg);
            if (NULL == hObj->coreHandle[coreCnt])
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not open core %d!\n", coreCnt);
                retVal = FVID2_EALLOC;
                break;
            }

            /* Get core property */
            retVal = coreOps->getProperty(hObj->coreHandle[coreCnt], &property);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not get property of core %d!\n", coreCnt);
                break;
            }

            /* Configure all channels of the core */
            for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
            {
                if (VCORE_TYPE_DEI == property.name)
                {
                    /* Configure DEI core */
                    retVal = vpsMdrvDeiCfgDeiCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
                else if (VCORE_TYPE_DEI_HQ == property.name)
                {
                    /* Configure DEI HQ core */
                    retVal = vpsMdrvDeiCfgDeiHqCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
                else if (VCORE_TYPE_DEI_WB == property.name)
                {
                    /* Configure DEI writeback core */
                    retVal = vpsMdrvDeiCfgDeiWrbkCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
                else if (VCORE_TYPE_VIP == property.name)
                {
                    /* Configure VIP core */
                    retVal = vpsMdrvDeiCfgVipCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
                else
                {
                    GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                        "Core type %d not supported\n", property.type);
                    retVal = FVID2_EFAIL;
                }

                /* Break if error occurs */
                if (FVID2_SOK != retVal)
                {
                    GT_2trace(VpsMdrvDeiTrace, GT_ERR,
                        "Config of core %d for channel %d failed.\n",
                        coreCnt, hObj->chObjs[chCnt]->chNum);
                    break;
                }
            }

            /* If error occurs break from outer loop as well */
            if (FVID2_SOK != retVal)
            {
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate the Resources in VIP core if VIP core is required */
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_VIP_CORE_IDX])
        {
            coreOps = instObj->coreOps[VPSMDRV_DEI_VIP_CORE_IDX];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));

            /* Allocate path in VIP based on parameters set and Set
               the channel configuration in VIP core */
            retVal = vpsMdrvDeiAlcCfgVipCore(
                        hObj,
                        hObj->coreHandle[VPSMDRV_DEI_VIP_CORE_IDX],
                        coreOps);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the VPDMA channels for which to program the sync on client */
        retVal = vpsMdrvDeiGetSocCh(hObj);
    }

    /* Close handles if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsMdrvDeiCloseCores(hObj);
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiCloseCores
 *  Closes all the cores opened for this handle.
 */
Int32 vpsMdrvDeiCloseCores(VpsMdrv_DeiHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              coreCnt;
    VpsMdrv_DeiInstObj *instObj;
    const Vcore_Ops    *coreOps;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Free up the path allocated in VIP core if vip core is used */
    if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_VIP_CORE_IDX])
    {
        coreOps = instObj->coreOps[VPSMDRV_DEI_VIP_CORE_IDX];
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
        GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->control));

        if (NULL != hObj->coreHandle[VPSMDRV_DEI_VIP_CORE_IDX])
        {
            retVal = coreOps->control(
                        hObj->coreHandle[VPSMDRV_DEI_VIP_CORE_IDX],
                        VCORE_IOCTL_VIP_FREE_PATH,
                        NULL);
        }
    }

    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        /* Close cores only if already opened for that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->close));

            if (NULL != hObj->coreHandle[coreCnt])
            {
                retVal = coreOps->close(hObj->coreHandle[coreCnt]);
                GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == retVal));
                hObj->coreHandle[coreCnt] = NULL;
            }
        }
    }

    return (retVal);
}



Int32 vpsMdrvDeiHqRdAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    const Vcore_Ops        *coreOps;
    VpsMdrv_DeiInstObj     *instObj;
    Vps_DeiHqRdWrAdvCfg    *advCfg;
    Vcore_Property          property;
    Bool                    isDeiPresent;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getProperty));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == retVal)
    {
        advCfg = (Vps_DeiHqRdWrAdvCfg *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (advCfg->chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get core property */
        retVal = coreOps->getProperty(
                     hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                     &property);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not get DEI core property\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if ((VCORE_TYPE_DEI_HQ == property.name) &&
            (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX]) &&
            (NULL != coreOps->control))
        {
            retVal = coreOps->control(
                        hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                        VCORE_IOCTL_GET_DEIHQ_CFG,
                        advCfg);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Get DEIHQ Adv IOCTL failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    return (retVal);
}



Int32 vpsMdrvDeiHqWrAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  descSet;
    const Vcore_Ops        *coreOps;
    VpsMdrv_DeiChObj       *chObj;
    VpsMdrv_DeiInstObj     *instObj;
    Vps_DeiHqRdWrAdvCfg    *advCfg;
    Vcore_Property          property;
    Bool                    isDeiPresent;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getProperty));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->programReg));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == retVal)
    {
        advCfg = (Vps_DeiHqRdWrAdvCfg *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (advCfg->chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get core property */
        retVal = coreOps->getProperty(
                     hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                     &property);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not get DEI core property\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if ((VCORE_TYPE_DEI_HQ == property.name) &&
            (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX]) &&
            (NULL != coreOps->control))
        {
            retVal = coreOps->control(
                        hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                        VCORE_IOCTL_SET_DEIHQ_CFG,
                        advCfg);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Get DEIHQ Adv IOCTL failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    if (FVID2_SOK == retVal)
    {
        chObj = hObj->chObjs[advCfg->chNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            retVal |= coreOps->programReg(
                    hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                    chObj->chNum,
                    &chObj->coreDescMem[descSet][VPSMDRV_DEI_DEI_CORE_IDX]);
        }
    }

    return (retVal);
}



Int32 vpsMdrvDeiRdAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    const Vcore_Ops        *coreOps;
    VpsMdrv_DeiInstObj     *instObj;
    Vps_DeiRdWrAdvCfg      *advCfg;
    Vcore_Property          property;
    Bool                    isDeiPresent;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getProperty));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == retVal)
    {
        advCfg = (Vps_DeiRdWrAdvCfg *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (advCfg->chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get core property */
        retVal = coreOps->getProperty(
                     hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                     &property);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not get DEI core property\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if ((VCORE_TYPE_DEI == property.name) &&
            (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX]) &&
            (NULL != coreOps->control))
        {
            retVal = coreOps->control(
                        hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                        VCORE_IOCTL_GET_DEI_CFG,
                        advCfg);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Get DEI Adv IOCTL failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    return (retVal);
}



Int32 vpsMdrvDeiWrAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  descSet;
    const Vcore_Ops        *coreOps;
    VpsMdrv_DeiChObj       *chObj;
    VpsMdrv_DeiInstObj     *instObj;
    Vps_DeiRdWrAdvCfg      *advCfg;
    Vcore_Property          property;
    Bool                    isDeiPresent;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getProperty));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->programReg));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == retVal)
    {
        advCfg = (Vps_DeiRdWrAdvCfg *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (advCfg->chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get core property */
        retVal = coreOps->getProperty(
                     hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                     &property);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not get DEI core property\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if ((VCORE_TYPE_DEI == property.name) &&
            (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX]) &&
            (NULL != coreOps->control))
        {
            retVal = coreOps->control(
                        hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                        VCORE_IOCTL_SET_DEI_CFG,
                        advCfg);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Get DEI Adv IOCTL failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    if (FVID2_SOK == retVal)
    {
        chObj = hObj->chObjs[advCfg->chNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            retVal |= coreOps->programReg(
                    hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                    chObj->chNum,
                    &chObj->coreDescMem[descSet][VPSMDRV_DEI_DEI_CORE_IDX]);
        }
    }

    return (retVal);
}



Int32 vpsMdrvDeiScRdAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  coreCnt;
    const Vcore_Ops        *coreOps = NULL;
    VpsMdrv_DeiChObj       *chObj;
    VpsMdrv_DeiInstObj     *instObj;
    Vps_ScRdWrAdvCfg       *advCfg;
    Vcore_ScCfgParams       scCfgPrms;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        advCfg = (Vps_ScRdWrAdvCfg *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (advCfg->chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the core to which the scalar ID matches */
        coreCnt = instObj->scalarCoreId[advCfg->scalarId];
        GT_assert(VpsMdrvDeiTrace, (VPSMDRV_DEI_MAX_CORE != coreCnt));
        coreOps = instObj->coreOps[coreCnt];
        /* Check if the scalar ID is valid */
        if (NULL == coreOps)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid Scalar ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (NULL != coreOps->control)
        {
            chObj = hObj->chObjs[advCfg->chNum];
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

            scCfgPrms.chNum = advCfg->chNum;
            scCfgPrms.scAdvCfg = &advCfg->scAdvCfg;
            /* Since config memory is same for both the sets, using 0th set. */
            scCfgPrms.descMem = &chObj->coreDescMem[0u][coreCnt];
            retVal = coreOps->control(
                        hObj->coreHandle[coreCnt],
                        VCORE_IOCTL_GET_SC_CFG,
                        &scCfgPrms);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Get SC Adv IOCTL failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    return (retVal);
}



Int32 vpsMdrvDeiScWrAdvCfgIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  coreCnt;
    const Vcore_Ops        *coreOps = NULL;
    VpsMdrv_DeiChObj       *chObj;
    VpsMdrv_DeiInstObj     *instObj;
    Vps_ScRdWrAdvCfg       *advCfg;
    Vcore_ScCfgParams       scCfgPrms;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        advCfg = (Vps_ScRdWrAdvCfg *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (advCfg->chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the core to which the scalar ID matches */
        coreCnt = instObj->scalarCoreId[advCfg->scalarId];
        GT_assert(VpsMdrvDeiTrace, (VPSMDRV_DEI_MAX_CORE != coreCnt));
        coreOps = instObj->coreOps[coreCnt];
        /* Check if the scalar ID is valid */
        if (NULL == coreOps)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid Scalar ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (NULL != coreOps->control)
        {
            chObj = hObj->chObjs[advCfg->chNum];
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

            scCfgPrms.chNum = advCfg->chNum;
            scCfgPrms.scAdvCfg = &advCfg->scAdvCfg;
            /* Since config memory is same for both the sets, using 0th set. */
            scCfgPrms.descMem = &chObj->coreDescMem[0u][coreCnt];
            retVal = coreOps->control(
                        hObj->coreHandle[coreCnt],
                        VCORE_IOCTL_SET_SC_CFG,
                        &scCfgPrms);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Set SC Adv IOCTL failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiCfgDeiCore
 *  Configures the DEI core according to the channel parameters.
 */
static Int32 vpsMdrvDeiCfgDeiCore(VpsMdrv_DeiHandleObj *hObj,
                                  VpsMdrv_DeiChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt, drvInstId;
    Vcore_DeiParams     deiPrms;
    Vcore_DeiCtxInfo   *deiCoreCtxInfo;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.outFmtDei));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.deiCfg));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.scCfg));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->setParams));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getDeiCtxInfo));

    /* Get the instance ID */
    drvInstId = hObj->instObj->drvInstId;

    /* Copy the DEI buffer format */
    VpsUtils_memcpy(&deiPrms.fmt, &chObj->chPrms.inFmt, sizeof(FVID2_Format));

    /* Fill other DEI core parameters */
    deiPrms.frameWidth = chObj->chPrms.inFmt.width;
    deiPrms.frameHeight = chObj->chPrms.inFmt.height;
    deiPrms.startX = 0u;
    deiPrms.startY = 0u;

#ifdef TI_816X_BUILD
    if (drvInstId == VPS_M2M_INST_AUX_DEI_SC4_VIP1)
#else
    if ((drvInstId == VPS_M2M_INST_MAIN_DEI_SC3_VIP0) ||
        (drvInstId == VPS_M2M_INST_AUX_SC4_VIP1))
#endif
    {
        deiPrms.secScanFmt = chObj->chPrms.outFmtVip->scanFormat;
    }
    else
    {
        deiPrms.secScanFmt = chObj->chPrms.outFmtDei->scanFormat;
    }
    deiPrms.memType = chObj->chPrms.inMemType;
    deiPrms.drnEnable = chObj->chPrms.drnEnable;
    for (cnt = 0u; cnt < VCORE_DEI_MAX_COMPR; cnt++)
    {
        deiPrms.comprEnable[cnt] = chObj->chPrms.comprEnable;
    }
    for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
    {
        deiPrms.dcomprEnable[cnt] = chObj->chPrms.comprEnable;
    }
    deiPrms.tarWidth = chObj->chPrms.outFmtDei->width;
    deiPrms.tarHeight = chObj->chPrms.outFmtDei->height;

    deiPrms.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;

    /* Copy the DEI parameters */
    VpsUtils_memcpy(
        &deiPrms.deiCfg,
        chObj->chPrms.deiCfg,
        sizeof(Vps_DeiConfig));

    /* Copy the DEI scalar parameters */
    VpsUtils_memcpy(&deiPrms.scCfg, chObj->chPrms.scCfg, sizeof(Vps_ScConfig));

    /* Copy the DEI crop parameters */
    VpsUtils_memcpy(&deiPrms.cropCfg,
                    chObj->chPrms.deiCropCfg,
                    sizeof(Vps_CropConfig));

    /* Set the DEI core parameter */
    retVal = coreOps->setParams(coreHandle, chObj->chNum, &deiPrms);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Set params for DEI core failed!\n");
    }

    /* Get the descriptor info of DEI core */
    deiCoreCtxInfo = &chObj->deiCoreCtxInfo;
    retVal = coreOps->getDeiCtxInfo(
                 coreHandle,
                 chObj->chNum,
                 deiCoreCtxInfo);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Could not get context info for DEI core!\n");
    }
    else
    {
        /* Assert if info more than array size */
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numPrevFld <= VCORE_DEI_MAX_PREV_FLD));
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numCurFldOut <= 1u));
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numMvIn <= VCORE_DEI_MAX_MV_IN));
        GT_assert(VpsMdrvDeiTrace, (deiCoreCtxInfo->numMvOut <= 1u));
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numMvstmIn <= (VCORE_DEI_MAX_MVSTM_IN + 1u)));
        GT_assert(VpsMdrvDeiTrace, (deiCoreCtxInfo->numMvstmOut <= 1u));
    }

    if (FVID2_SOK == retVal)
    {
        if (!((TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) ||
              (TRUE == chObj->deiCoreCtxInfo.isTnrMode)))
        {
            /* When compressor is not needed, disable it and override user
             * provided settings */
            chObj->chPrms.comprEnable = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiCfgDeiHqCore
 *  Configures the DEI HQ core according to the channel parameters.
 */
static Int32 vpsMdrvDeiCfgDeiHqCore(VpsMdrv_DeiHandleObj *hObj,
                                    VpsMdrv_DeiChObj *chObj,
                                    Vcore_Handle coreHandle,
                                    const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    Vcore_DeiParams     deiPrms;
    Vcore_DeiCtxInfo   *deiCoreCtxInfo;
#ifdef TI_816X_BUILD
    UInt32              drvInstId;
#endif

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.outFmtDei));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.deiHqCfg));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.scCfg));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->setParams));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getDeiCtxInfo));

    /* Copy the DEI buffer format */
    VpsUtils_memcpy(&deiPrms.fmt, &chObj->chPrms.inFmt, sizeof(FVID2_Format));

    /* Fill other DEI core parameters */
    deiPrms.frameWidth = chObj->chPrms.inFmt.width;
    deiPrms.frameHeight = chObj->chPrms.inFmt.height;
    deiPrms.startX = 0u;
    deiPrms.startY = 0u;

#ifdef TI_816X_BUILD
    /* Get the instance ID */
    drvInstId = hObj->instObj->drvInstId;
    if (drvInstId == VPS_M2M_INST_AUX_DEI_SC4_VIP1)
    {
        deiPrms.secScanFmt = chObj->chPrms.outFmtVip->scanFormat;
    }
    else
    {
        deiPrms.secScanFmt = chObj->chPrms.outFmtDei->scanFormat;
    }
#else
    deiPrms.secScanFmt = chObj->chPrms.outFmtDei->scanFormat;
#endif
    deiPrms.memType = chObj->chPrms.inMemType;
    deiPrms.drnEnable = chObj->chPrms.drnEnable;
    for (cnt = 0u; cnt < VCORE_DEI_MAX_COMPR; cnt++)
    {
        /* Caution: Since this code enables both the compression modules,
           It is assumed that it will not create any issue in
           deinterlacing mode */
        deiPrms.comprEnable[cnt] = chObj->chPrms.comprEnable;
    }
    for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
    {
        /* Caution: Since this code enables all the decompression modules,
           It is assumed that it will not create any issue in 4 field
           deinterlacing mode */
        deiPrms.dcomprEnable[cnt] = chObj->chPrms.comprEnable;
    }
    deiPrms.tarWidth = chObj->chPrms.outFmtDei->width;
    deiPrms.tarHeight = chObj->chPrms.outFmtDei->height;

    deiPrms.deiHqCtxMode = hObj->deiHqCtxMode;
    if (VPS_DEIHQ_CTXMODE_APP_N_1 == hObj->deiHqCtxMode)
    {
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.inFmtFldN_1));
        /* Copy the DEI N-1 field buffer format */
        VpsUtils_memcpy(
            &deiPrms.inFmtFldN_1,
            chObj->chPrms.inFmtFldN_1,
            sizeof(FVID2_Format));
    }

    /* Copy the DEI HQ parameters */
    VpsUtils_memcpy(
        &deiPrms.deiHqCfg,
        chObj->chPrms.deiHqCfg,
        sizeof(Vps_DeiHqConfig));

    /* Copy the DEI scalar parameters */
    VpsUtils_memcpy(&deiPrms.scCfg, chObj->chPrms.scCfg, sizeof(Vps_ScConfig));

    /* Copy the DEI crop parameters */
    VpsUtils_memcpy(&deiPrms.cropCfg,
                    chObj->chPrms.deiCropCfg,
                    sizeof(Vps_CropConfig));

    /* Set the DEI core parameter */
    retVal = coreOps->setParams(coreHandle, chObj->chNum, &deiPrms);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Set params for DEI core failed!\n");
    }

    /* Get the descriptor info of DEI core */
    deiCoreCtxInfo = &chObj->deiCoreCtxInfo;
    retVal = coreOps->getDeiCtxInfo(
                 coreHandle,
                 chObj->chNum,
                 deiCoreCtxInfo);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Could not get context info for DEI core!\n");
    }
    else
    {
        /* Assert if info more than array size */
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numPrevFld <= VCORE_DEI_MAX_PREV_FLD));
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numCurFldOut <= 2u));
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numMvIn <= VCORE_DEI_MAX_MV_IN));
        GT_assert(VpsMdrvDeiTrace, (deiCoreCtxInfo->numMvOut <= 1u));
        GT_assert(VpsMdrvDeiTrace,
            (deiCoreCtxInfo->numMvstmIn <= (VCORE_DEI_MAX_MVSTM_IN + 1u)));
        GT_assert(VpsMdrvDeiTrace, (deiCoreCtxInfo->numMvstmOut <= 1u));
    }

    if (FVID2_SOK == retVal)
    {
        if (!((TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) ||
              (TRUE == chObj->deiCoreCtxInfo.isTnrMode)))
        {
            /* When compressor is not needed, disable it and override user
             * provided settings */
            chObj->chPrms.comprEnable = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiCfgDeiWrbkCore
 *  Configures the DEI Wrbk core according to the channel parameters.
 */
static Int32 vpsMdrvDeiCfgDeiWrbkCore(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj,
                                      Vcore_Handle coreHandle,
                                      const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Format        dwpFmt;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.outFmtDei));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->setFormat));

    /* Copy the DEI writeback out buffer format */
    VpsUtils_memcpy(&dwpFmt.fmt, chObj->chPrms.outFmtDei, sizeof(FVID2_Format));

    /* Fill other DEI WRBK core formats */
    dwpFmt.frameWidth = dwpFmt.fmt.width;
    dwpFmt.frameHeight = dwpFmt.fmt.height;
    dwpFmt.startX = 0u;
    dwpFmt.startY = 0u;
    dwpFmt.secScanFmt = dwpFmt.fmt.scanFormat;
    dwpFmt.memType = chObj->chPrms.outMemTypeDei;

    /* Set the DEI WRBK core format */
    retVal = coreOps->setFormat(coreHandle, chObj->chNum, &dwpFmt);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Set format for DEI WRBK core failed!\n");

    }

    return (retVal);
}



/**
 *  vpsMdrvDeiCfgVipCore
 *  Configures the VIP core according to the channel parameters.
 */
static Int32 vpsMdrvDeiCfgVipCore(VpsMdrv_DeiHandleObj *hObj,
                                  VpsMdrv_DeiChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    Vcore_VipParams     vipPrms;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.outFmtVip));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->chPrms.vipScCfg));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->setParams));

    /* Copy the VIP buffer format */
    VpsUtils_memcpy(
        &vipPrms.fmt,
        chObj->chPrms.outFmtVip,
        sizeof(FVID2_Format));

    /* Fill other VIP core parameters */
    vipPrms.isScReq = hObj->isVipScReq;
    vipPrms.memType = chObj->chPrms.outMemTypeVip;
    vipPrms.srcWidth = chObj->chPrms.inFmt.width;
    if (TRUE == chObj->deiCoreCtxInfo.isDeinterlacing)
    {
        vipPrms.srcHeight = chObj->chPrms.inFmt.height * 2u;
    }
    else
    {
        vipPrms.srcHeight = chObj->chPrms.inFmt.height;
    }

    /* Copy the VIP scalar parameters */
    VpsUtils_memcpy(
        &vipPrms.scCfg,
        chObj->chPrms.vipScCfg,
        sizeof(Vps_ScConfig));

    /* Copy the DEI crop parameters */
    VpsUtils_memcpy(&vipPrms.cropCfg,
                    chObj->chPrms.vipCropCfg,
                    sizeof(Vps_CropConfig));

    /* Set the VIP core parameter */
    retVal = coreOps->setParams(coreHandle, chObj->chNum, &vipPrms);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Set params for VIP core failed!\n");
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiAlcCfgVipCore
 *  Allocate and configure VIP core
 */
static Int32 vpsMdrvDeiAlcCfgVipCore(VpsMdrv_DeiHandleObj *hObj,
                                     Vcore_Handle coreHandle,
                                     const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->control));

    /* Allocate the path in VIP based on the parameters set using SetParams */
    retVal = coreOps->control(
                coreHandle,
                VCORE_IOCTL_VIP_ALLOC_PATH,
                NULL);

    if (FVID2_SOK == retVal)
    {
        /* Configure all channels of the core */
        for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
        {
            GT_assert(VpsMdrvDeiTrace, (NULL != hObj->chObjs[chCnt]));

            retVal = coreOps->control(
                        coreHandle,
                        VCORE_IOCTL_VIP_SET_CONFIG,
                        &(hObj->chObjs[chCnt]->chNum));

            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Set Config for %d channel in VIP core failed!\n",
                    hObj->chObjs[chCnt]->chNum);
            }
        }
    }
    else
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Resource Allocation in VIP core failed!\n");
    }

    return (retVal);
}



/**
 */
static Int32 vpsMdrvDeiGetSocCh(VpsMdrv_DeiHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  coreCnt;
    UInt32                  numSocCh;
    UInt32                  chCnt;
    Vcore_DescInfo          descInfo;
    Vcore_Property          property;
    const Vcore_Ops        *coreOps;
    VpsMdrv_DeiInstObj     *instObj = NULL;

    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    numSocCh = 0u;
    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        /* Get the Descriptor Information for this core */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getDescInfo));

            /* Get the descriptor info of each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         hObj->chObjs[0u]->chNum,
                         0u,
                         &descInfo);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }

            /* Get the Property info to see if it is input ot output core */
            retVal = coreOps->getProperty(
                         hObj->coreHandle[coreCnt],
                         &property);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not get property for core %d!\n", coreCnt);
                break;
            }

            /* Get the index of the VPDMA channel, which will be used for
             * programming sync on client control
             * descriptor at the end of each channel's descriptors */
            if (VCORE_TYPE_INPUT == property.type)
            {
                hObj->sochIdx = numSocCh;
            }

            GT_assert(VpsMdrvDeiTrace,
                (descInfo.numChannels < VCORE_MAX_VPDMA_CH));
            for (chCnt = 0u; chCnt < descInfo.numChannels; chCnt++)
            {
                GT_assert(VpsMdrvDeiTrace, (numSocCh < VCORE_MAX_VPDMA_CH));
                hObj->socChNum[numSocCh] = descInfo.socChNum[chCnt];
                numSocCh++;
            }
        }
        if (FVID2_SOK != retVal)
        {
            break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        hObj->numVpdmaChannels = numSocCh;
    }

    return (retVal);
}
