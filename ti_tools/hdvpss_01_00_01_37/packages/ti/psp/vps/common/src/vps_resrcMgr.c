/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 * \file vps_resrcMgr.c
 *
 * \brief VPS Resource Manager Source file
 * This file exposes the APIs of the VPS Resource Manager.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VRM_SEM_TIMEOUT                     (BIOS_WAIT_FOREVER)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
   UInt32            resource[VRM_RESOURCE_MAX];
   UInt32            vpdmaList[VPSHAL_VPDMA_MAX_LIST];
   UInt32            vpdmaFreeChanFlag[VPSHAL_VPDMA_NUM_FREE_CHANNELS];
   UInt32            vpdmaFreeLmFidFlag[VPSHAL_VPDMA_MAX_LM_FID];
   UInt32            vpdmaFreeSiFlag[VPSHAL_VPDMA_MAX_SI_SOURCE];
   UInt32            cntFreeSi;
   UInt32            cntFreeChannels;
   Semaphore_Handle  semHandle;
} Vrm_Info;

/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

/* Node */

/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

Vrm_Info VrmInfo;

/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */

/**
 *  \fn     Int Vrm_init(Ptr arg)
 *
 *  \brief  Function to initialize Resource manager.
 *
 *  \param  arg         Currently not used. It should be NULL.
 *
 *  \return 0           in case of success
 *          -1          in case of failure
 *
 *  \enter  None
 *
 *  \leave  None
 */
Int Vrm_init(Ptr arg)
{
    Int                 ret = 0;
    UInt32              cnt = 0;
    Semaphore_Params    semParams;

    /* Initialize all the resource to be Free */
    for (cnt = 0u; cnt < VRM_RESOURCE_MAX; cnt ++)
    {
        VrmInfo.resource[cnt] = FALSE;
    }

    /* Initialize All the VPDMA Lists to be free */
    for (cnt = 0u; cnt < VPSHAL_VPDMA_MAX_LIST; cnt ++)
    {
        VrmInfo.vpdmaList[cnt] = FALSE;
    }

    /* Initialize All the VPDMA free channels to be free */
    for (cnt = 0u; cnt < VPSHAL_VPDMA_NUM_FREE_CHANNELS; cnt ++)
    {
        VrmInfo.vpdmaFreeChanFlag[cnt] = FALSE;
    }
    VrmInfo.cntFreeChannels = VPSHAL_VPDMA_NUM_FREE_CHANNELS;

    /* Initialize All the VPDMA LM FID to be free */
    for (cnt = 0u; cnt < VPSHAL_VPDMA_MAX_LM_FID; cnt ++)
    {
        VrmInfo.vpdmaFreeLmFidFlag[cnt] = FALSE;
    }

    /* Initialize All the VPDMA free channels to be free */
    for (cnt = 0u; cnt < VPSHAL_VPDMA_MAX_SI_SOURCE; cnt ++)
    {
        VrmInfo.vpdmaFreeSiFlag[cnt] = FALSE;
    }
    VrmInfo.cntFreeSi = VPSHAL_VPDMA_MAX_SI_SOURCE;

    /* Get the semaphore handle */
    Semaphore_Params_init(&semParams);
    VrmInfo.semHandle = Semaphore_create(1u, &semParams, NULL);
    if (NULL == VrmInfo.semHandle)
    {
        ret = -1;
    }

    return (ret);
}



/**
 *  \fn     Int Vrm_deInit(Ptr arg)
 *
 *  \brief  Function to de-initialize the resource manager.
 *
 *  \param  arg         Currently not used. It should be NULL.
 *
 *  \return 0           in case of success
 *          1           in case failure
 *
 *  \enter  None
 *
 *  \leave  None
 */
Int Vrm_deInit(Ptr arg)
{
    UInt32 cnt = 0;

    /* check to see no-resourece is allocated */
    for (cnt = 0; cnt < VRM_RESOURCE_MAX; cnt ++)
    {
        if (TRUE == VrmInfo.resource[cnt])
        {
            GT_1trace(VrmTrace, GT_ERR,
                " WARNING: HDVPSS: VRM DEINIT: %d resource is in use!!\n", cnt);
        }
    }

    Semaphore_delete(&(VrmInfo.semHandle));

    return (0);
}



/**
 *  \fn     Int32 Vrm_AllocResource(Vrm_Resource resource)
 *
 *  \brief  Function to get and allocate the give resource. It takes resource
 *          in the enum and returns the handle to the resource if resource is
 *          available.
 *
 *  \param  resource    resource to be allocated
 *
 *  \return 0           if resource is allocated or -1
 *
 *  \enter  Vrm_init must be called before calling this function
 *
 *  \leave  Makes resource allocated
 */
Int32 Vrm_allocResource(Vrm_Resource resource)
{
    Int32 retVal = VPS_EALLOC;

    /* Get the semaphore */
    Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

    if ((resource > VRM_RESOURCE_INVALID) && (resource < VRM_RESOURCE_MAX))
    {
        /* If resource is free, return handle */
        if (VrmInfo.resource[(UInt32)resource] == FALSE)
        {
            /* Mark resource as allocated */
            VrmInfo.resource[(UInt32)resource] = TRUE;
            retVal = VPS_SOK;
        }
    }

    /* Release Semaphore */
    Semaphore_post(VrmInfo.semHandle);

    return (retVal);
}




/**
 *  \fn     Int Vrm_releaseResource(Vrm_Resource resource)
 *
 *  \brief  Function to free the already allocated resource. It takes handle
 *          to the resource and frees the resource. This handle is returned
 *          from the Vrm_GetResource function.
 *
 *  \param resource     Resource to be Freed
 *
 *  \return 0           Resource is freed successfully
 *          -1          In case of error
 *
 *  \enter  Vrm_init must be called before calling this function
 *
 *  \leave  Makes resource free
 */
Int32 Vrm_releaseResource(Vrm_Resource resource)
{
    Int32   retVal = VPS_EFAIL;

    /* Get the semaphore */
    Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

    if ((resource > VRM_RESOURCE_INVALID) && (resource < VRM_RESOURCE_MAX))
    {
        if (TRUE == VrmInfo.resource[(UInt32) resource])
        {
            VrmInfo.resource[(UInt32) resource] = FALSE;
            retVal = VPS_SOK;
        }
    }

    /* Release Semaphore */
    Semaphore_post(VrmInfo.semHandle);

    return (retVal);
}



/**
 *  \brief  Function to get the free VPDMA Lists. This function returns
 *          the free list from the 8 lists supported by the VPDMA. If any
 *          one of them is free, it returns number of that list and marks it
 *          as allocated. If the lists are not available, it returns error.
 *
 *  \param  listNum     List number returned by the resource manager
 *
 *  \return 0           List is available and allocated to the caller
 *          -1          No free list available
 *
 *  \enter  Vrm_init must be called before calling this function
 *
 *  \leave  Makes resource free
 */
Int32 Vrm_allocList(UInt8 *listNum, Vrm_ListType listType)
{
    Int     retVal = -1;
    UInt8   listCnt, startListNum, allocListNum;

    startListNum = 0;

    /* the below #if 1 peice of code allows developer to give a preference
        for list allocation based on the requesting driver type.

       to make all list allocation equal, i.e no preference,
        make the below code to #if 0
    */
    #if 1
    if(listType==VRM_LIST_TYPE_CAPTURE)
        startListNum = 0;
    else
    if(listType==VRM_LIST_TYPE_DISPLAY)
        startListNum = 1;
    else
    if(listType==VRM_LIST_TYPE_M2M)
        startListNum = 4;
    #endif

    if (NULL != listNum)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        /* If resource is free, return handle */
        for (listCnt = 0; listCnt < VPSHAL_VPDMA_MAX_LIST; listCnt ++)
        {
            allocListNum = (startListNum + listCnt)%VPSHAL_VPDMA_MAX_LIST;

            if (FALSE == VrmInfo.vpdmaList[allocListNum])
            {
                break;
            }
        }

        if (listCnt < VPSHAL_VPDMA_MAX_LIST)
        {
            /* return allocated list number */
            *listNum = allocListNum;
            /* Mark the list as allocated */
            VrmInfo.vpdmaList[allocListNum] = TRUE;
            retVal = 0;
        }
        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    #if 0
    {
        char *listTypeName[]={ "CAPTURE", "DISPLAY", "M2M" };

        if(retVal==0)
            Vps_printf(" VRM: ALLOC LIST: %s: ListNum = %d \n", listTypeName[listType], *listNum);
        else
            Vps_printf(" VRM: ALLOC LIST: %s: NO Free List \n", listTypeName[listType]);
    }
    #endif

    return (retVal);
}



/**
 *  Int32 Vrm_releaseeList(UInt8 listNum)
 *
 *  \brief  Function to free already allocated VDPMA list
 *
 *  \param  listNum     List number to be freed
 *
 *  \return 0           Resource is freed successfully
 *          -1          In case of error
 *
 *  \enter  Vrm_init must be called before calling this function
 *
 *  \leave  Makes resource free
 */
Int32 Vrm_releaseList(UInt8 listNum)
{
    Int    retVal = -1;

    if (listNum < VPSHAL_VPDMA_MAX_LIST)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        if (TRUE == VrmInfo.vpdmaList[listNum])
        {
            /* Mark the list as free */
            VrmInfo.vpdmaList[listNum] = FALSE;
            retVal = 0;
        }
        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}



/**
 *  Int32 Vrm_allocFreeChannel(UInt32 *channels, UInt32 numChannels)
 *
 *  \brief  Function to get the free VPDMA channel. This function returns
 *          a free channel supported by the VPDMA. If any free channel is
 *          not allocated, it returns number of that channel and marks it
 *          as allocated. If the free channel is not available, it
 *          returns error.
 *
 *  \param  channelNum  channel number returned by the resource manager
 *
 *  \return 0           channel is available and allocated to the caller
 *          -1          No free channel available
 *
 *  \enter  Vrm_init must be called before calling this function
 *
 *  \leave  Makes resource free
 */
Int32 Vrm_allocFreeChannel(UInt32 *channels, UInt32 numChannels)
{
    Int     retVal = -1;
    UInt32  chanCnt, cnt;

    if (NULL != channels)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        /* Check to see if requested number of free send interrupt sources
         * are available */
        if (numChannels <= VrmInfo.cntFreeChannels)
        {
            for (cnt = 0u; cnt < numChannels; cnt ++)
            {
                /* If resource is free, return handle */
                for (chanCnt = 0u; chanCnt < VPSHAL_VPDMA_NUM_FREE_CHANNELS;
                        chanCnt ++)
                {
                    if (FALSE == VrmInfo.vpdmaFreeChanFlag[chanCnt])
                    {
                        break;
                    }
                }

                if (chanCnt < VPSHAL_VPDMA_NUM_FREE_CHANNELS)
                {
                    /* return allocated list number */
                    channels[cnt] = chanCnt + VPSHAL_VPDMA_MIN_FREE_CHANNEL;
                    /* Mark the list as allocated */
                    VrmInfo.vpdmaFreeChanFlag[chanCnt] = TRUE;
                    /* Decrement Free send interrupt count */
                    VrmInfo.cntFreeChannels --;
                }
            }
            retVal = 0;
        }
        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}



/**
 *  Int32 Vrm_releaseFreeChannel(UInt32 *channels, UInt32 numChannels)
 *
 *  \brief  Function to free already allocated VDPMA free channel
 *
 *  \param  channelNum  channel number to be freed
 *
 *  \return 0           Resource is freed successfully
 *          -1          In case of error
 *
 *  \enter  Vrm_init must be called before calling this function
 *
 *  \leave  Makes resource free
 */
Int32 Vrm_releaseFreeChannel(UInt32 *channels, UInt32 numChannels)
{
    Int     retVal = -1;
    UInt32  freeChan, cnt;

    if (NULL != channels)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        retVal = 0;
        for (cnt = 0u; (cnt < numChannels) && (0 == retVal); cnt ++)
        {
            freeChan = channels[cnt];

            /* Send interrupt source number should be correct */
            if ((freeChan > VPSHAL_VPDMA_MAX_FREE_CHANNEL) ||
                (freeChan < VPSHAL_VPDMA_MIN_FREE_CHANNEL))
            {
                retVal = -1;
                break;
            }

            freeChan -= VPSHAL_VPDMA_MIN_FREE_CHANNEL;
            if (TRUE == VrmInfo.vpdmaFreeChanFlag[freeChan])
            {
                /* Mark the list as free */
                VrmInfo.vpdmaFreeChanFlag[freeChan] = FALSE;
                /* Increment count of free send interrupt source */
                VrmInfo.cntFreeChannels ++;
                retVal = 0;
            }
            else /* Trying to free send interrupt which is already free */
            {
                retVal = -1;
            }
        }

        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}



/**
 *  Vrm_allocLmFid
 *  \brief Function to get the free LM FID. It returns success if it
 *  is able to find free LM FID and marks it as allocated.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param lmFidNum      Free LM FID
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_allocLmFid(UInt32 *lmFidNum)
{
    Int     retVal = -1;
    UInt8   lmFidCnt;

    if (NULL != lmFidNum)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        /* If resource is free, return handle */
        for (lmFidCnt = 0u; lmFidCnt < VPSHAL_VPDMA_MAX_LM_FID; lmFidCnt ++)
        {
            if (FALSE == VrmInfo.vpdmaFreeLmFidFlag[lmFidCnt])
            {
                break;
            }
        }

        if (lmFidCnt < VPSHAL_VPDMA_MAX_LM_FID)
        {
            /* return allocated list number */
            *lmFidNum = lmFidCnt;
            /* Mark the list as allocated */
            VrmInfo.vpdmaFreeLmFidFlag[lmFidCnt] = TRUE;
            retVal = 0;
        }
        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}



/**
 *  Vrm_releaseLmFid
 *  \brief Function to release VPDMA LM FID and mark it as free.
 *
 *  Vrm_init and Vrm_allocList function should be called before
 *  calling this function.
 *
 *  \param lmFidNum      LM FID Number
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseLmFid(UInt32 lmFidNum)
{
    Int    retVal = -1;

    if (lmFidNum < VPSHAL_VPDMA_MAX_LM_FID)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        if (TRUE == VrmInfo.vpdmaFreeLmFidFlag[lmFidNum])
        {
            /* Mark the list as free */
            VrmInfo.vpdmaFreeLmFidFlag[lmFidNum] = FALSE;
            retVal = 0;
        }
        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}



/**
 *  Vrm_allocLmFid
 *  \brief Function to get the free LM FID. It returns success if it
 *  is able to find free LM FID and marks it as allocated.
 *
 *  Vrm_init function should be called before calling this function.
 *
 *  \param lmFidNum      Free LM FID
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_allocSendIntr(UInt32 *si, UInt32 numSendIntr)
{
    Int     retVal = -1;
    UInt32  siCnt, cnt;

    if (NULL != si)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        /* Check to see if requested number of free send interrupt sources
         * are available */
        if (numSendIntr <= VrmInfo.cntFreeSi)
        {
            for (cnt = 0u; cnt < numSendIntr; cnt ++)
            {
                /* If resource is free, return handle */
                for (siCnt = 0u; siCnt < VPSHAL_VPDMA_MAX_SI_SOURCE; siCnt ++)
                {
                    if (FALSE == VrmInfo.vpdmaFreeSiFlag[siCnt])
                    {
                        break;
                    }
                }

                if (siCnt < VPSHAL_VPDMA_MAX_SI_SOURCE)
                {
                    /* return allocated list number */
                    si[cnt] = siCnt;
                    /* Mark the list as allocated */
                    VrmInfo.vpdmaFreeSiFlag[siCnt] = TRUE;
                    /* Decrement Free send interrupt count */
                    VrmInfo.cntFreeSi --;
                }
            }
            retVal = 0;
        }
        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}



/**
 *  Vrm_releaseLmFid
 *  \brief Function to release VPDMA LM FID and mark it as free.
 *
 *  Vrm_init and Vrm_allocList function should be called before
 *  calling this function.
 *
 *  \param lmFidNum      LM FID Number
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int32 Vrm_releaseSendIntr(UInt32 *si, UInt32 numSendIntr)
{
    Int     retVal = -1;
    UInt32  siSrc, cnt;

    if (NULL != si)
    {
        /* Get the semaphore */
        Semaphore_pend(VrmInfo.semHandle, (UInt32)VRM_SEM_TIMEOUT);

        retVal = 0;
        for (cnt = 0u; (cnt < numSendIntr) && (0 == retVal); cnt ++)
        {
            siSrc = si[cnt];

            /* Send interrupt source number should be correct */
            if (siSrc >= VPSHAL_VPDMA_MAX_SI_SOURCE)
            {
                retVal = -1;
                break;
            }

            if (TRUE == VrmInfo.vpdmaFreeSiFlag[siSrc])
            {
                /* Mark the list as free */
                VrmInfo.vpdmaFreeSiFlag[siSrc] = FALSE;
                /* Increment count of free send interrupt source */
                VrmInfo.cntFreeSi ++;
                retVal = 0;
            }
            else /* Trying to free send interrupt which is already free */
            {
                retVal = -1;
            }
        }

        /* Release Semaphore */
        Semaphore_post(VrmInfo.semHandle);
    }

    return (retVal);
}

