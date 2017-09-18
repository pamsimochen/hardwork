/** ==================================================================
 *  @file   rm.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/rm/src/                                                  
 *                                                                    
 *  @desc   This  File contains  implementation of APIs of Resource
 * Manager for SIMCOP \version 1.0.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/* !
 * ****************************************************************************
 * \file rm.c \brief This file has the implementation of APIs of Resource
 * Manager for SIMCOP \version 1.0
 * **************************************************************************** */
/* =========================================================================
 * ! ! Revision History ! =================================== ! !
 * 09-Nov-2011: Venugopala Krishna: Initial version
 * ========================================================================= */

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include "rm.h"

/******************************************************************************
 * GLOBALS
 ******************************************************************************/
RM_SIMCOP_RESOURCE_MANAGER gSimcop_RM;

/* =========================================================================== */
/**
 * @func RM_SIMCOP_Init() This function does the one time initialization operation 
 *                      of the Resource Manager.
 *
 * @details: Does the initialization of the global structure of RM_SIMCOP_RESOURCE_MANAGER
 *           1) Reset all the entries of clientRegistry
 *           2) Reset all the entries of bIsResourceAcquired
 *           3) Reset all the entries of clientToWhichResourceGranted
 *           4) Reset nRegisteredClients
 *           5) Create all the semaphores of semaphoreForResource with initial value of ONE. (All these are BINARY SEMAPHORES)
 *           6) Create semaphoreForRM with initial value of ONE. (All these are BINARY SEMAPHORES)
 *
 * @param void         : 
 *
 * @return             : Status of the SIMCOP RM initialization.
 *
 * @post               : SIMCOP_RM component is successfully instanstiated
 */
/* =========================================================================== */
RM_SIMCOP_STATUS RM_SIMCOP_Init(void)
{
    Semaphore_Params params;

    unsigned int i;

    Semaphore_Params_init(&params);
    memset(&gSimcop_RM, 0, sizeof(RM_SIMCOP_RESOURCE_MANAGER));

    gSimcop_RM.semaphoreForRM = Semaphore_create(1, &params, NULL);
    gSimcop_RM.semaphoreForRMA = Semaphore_create(1, &params, NULL);
    gSimcop_RM.semaphoreForRMR = Semaphore_create(1, &params, NULL);
    for (i = RM_SIMCOP_RESOURCE_NAME_SIMCOP_LDC; i < RM_SIMCOP_RESOURCE_MAX;
         i++)
        gSimcop_RM.semaphoreForResource[i] = Semaphore_create(1, &params, NULL);

    if (IS_DIAG_PRINTS_REQ)
        Vps_rprintf("%d : RM_SIMCOP::: RM_SIMCOP_Init called. \n",
                    Utils_getCurTimeInMsec());

    return RM_SIMCOP_STATUS_SUCCESS;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_DeInit() This function does the one time de-initialization operation 
 *                      of the Resource Manager.
 *
 * @details: Does the De-initialization of the global structure of RM_SIMCOP_RESOURCE_MANAGER
 *           1) Report the ERROR if nRegisteredClients OR nTotalResourcesAllocated is Non-ZERO
 *           2) Else 
 *           3) Reset all the entries of clientRegistry
 *           4) Reset all the entries of bIsResourceAcquired
 *           5) Reset all the entries of clientToWhichResourceGranted
 *           6) Reset nRegisteredClients
 *           7) Delete all the semaphores of semaphoreForResource
 *           8) Delete semaphoreForRM
 *
 * @param void         : 
 *
 * @return             : Status of the SIMCOP RM De-initialization.
 *
 * @post               : SIMCOP_RM component is successfully instanstiated
 */
/* =========================================================================== */
RM_SIMCOP_STATUS RM_SIMCOP_DeInit(void)
{
    unsigned int i;

    if (IS_DIAG_PRINTS_REQ)
        Vps_rprintf("%d : RM_SIMCOP::: RM_SIMCOP_DeInit called \n",
                    Utils_getCurTimeInMsec());
    if (gSimcop_RM.nRegisteredClients || gSimcop_RM.nTotalResourcesAllocated)
        return RM_SIMCOP_STATUS_ERROR;

    for (i = RM_SIMCOP_RESOURCE_NAME_SIMCOP_LDC; i < RM_SIMCOP_RESOURCE_MAX;
         i++)
        Semaphore_delete(&gSimcop_RM.semaphoreForResource[i]);

    Semaphore_delete(&gSimcop_RM.semaphoreForRM);
    Semaphore_delete(&gSimcop_RM.semaphoreForRMA);
    Semaphore_delete(&gSimcop_RM.semaphoreForRMR);
    memset(&gSimcop_RM, 0, sizeof(RM_SIMCOP_RESOURCE_MANAGER));

    return RM_SIMCOP_STATUS_SUCCESS;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_RegisterClient() This function does the registration of the client 
 *                      with the Resource Manager.
 *
 * @details: Does the registraion of the client with Resource Manager
 *           Pend the semaphoreForRM
 *           1) Looks for a free entry in registry, if registry is full return ERR. 
 *           2) Else Allocate the Memory for the RM_SIMCOP_CLIENT_HANDLE
 *           3) Increment the nRegisteredClients;
 *           4) Assign the registryIndex with the free entry index in registry.
 *           5) Assign the pClientHandle reading the pClientHandle of RM_SIMCOP_CLIENT_PARAMS
 *           6) Reset the bitFieldOfResourcesAcquired 
 *           7) Write the Handle into the registry index by registryIndex.
 *           Post the semaphoreForRM
 *
 * @param clientParams : Provides the client parameters for registration
 *
 * @param Handle       : Client RM Handle whose memory allocation and registraion to be done.
 *
 * @return             : Status of the Registration of client.
 *
 * @post               : Client is successfully registered with SIMCOP RM
 */
/* =========================================================================== */
RM_SIMCOP_STATUS RM_SIMCOP_RegisterClient(RM_SIMCOP_CLIENT_PARAMS clientParams,
                                          RM_SIMCOP_CLIENT_HANDLE * Handle)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    unsigned int i = 0;

    Semaphore_pend(gSimcop_RM.semaphoreForRM, BIOS_WAIT_FOREVER);
    if (gSimcop_RM.nRegisteredClients >= RM_SIMCOP_MAX_CLIENTS)
    {
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }
    // Find the free client registry entry
    for (i = 0; i < RM_SIMCOP_MAX_CLIENTS; i++)
    {
        if (gSimcop_RM.clientRegistry[i] == NULL)
            break;

    }
    if (i == RM_SIMCOP_MAX_CLIENTS)
    {
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }

    *Handle = (RM_SIMCOP_CLIENT_HANDLE) malloc(sizeof(RM_SIMCOP_CLIENT));
    if ((*Handle) == NULL)
     {
         status =  RM_SIMCOP_STATUS_ERROR;
         goto EXIT;
     }

    (*Handle)->registryIndex = i;
    (*Handle)->bitFieldOfResourcesAcquired = 0;
    (*Handle)->pClientHandle = clientParams.pClientHandle;
    (*Handle)->timeForTimeOut = SIMCOP_RM_MAX_TIME_OUT;

    gSimcop_RM.clientRegistry[i] = *Handle;
    gSimcop_RM.nRegisteredClients++;
    if (IS_DIAG_PRINTS_REQ)
        Vps_rprintf("%d : RM_SIMCOP::: RM_SIMCOP_Register Client Handle %x \n",
                    Utils_getCurTimeInMsec(), *Handle);

  EXIT:
    Semaphore_post(gSimcop_RM.semaphoreForRM);
    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_DeRegisterClient() This function does the de-registration of the client 
 *                      with the Resource Manager.
 *
 * @details: Does the de-registraion of the client with Resource Manager
 *           Pend the semaphoreForRM
 *           1) Read the bitFieldOfResourcesAcquired, if non-ZERO, return ERR 
 *           2) Else Read the registryIndex and clear the location in registry.
 *           3) Decrement the nRegisteredClients;
 *           4) De-allcate Memory for the Handle. 
 *           Post the semaphoreForRM
 *
 *
 * @param Handle       : Client RM Handle whose memory de-allocation and de-registraion to be done.
 *
 * @return             : Status of the De-Registration of client.
 *
 * @post               : Client is successfully de-registered with SIMCOP RM
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_DeRegisterClient(RM_SIMCOP_CLIENT_HANDLE Handle)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    Semaphore_pend(gSimcop_RM.semaphoreForRM, BIOS_WAIT_FOREVER);
    if (Handle->bitFieldOfResourcesAcquired)
    {
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }
    if (IS_DIAG_PRINTS_REQ)
        Vps_rprintf
            ("%d : RM_SIMCOP::: RM_SIMCOP_DeRegister Client Handle %x \n",
             Utils_getCurTimeInMsec(), Handle);

    gSimcop_RM.clientRegistry[Handle->registryIndex] = 0;
    gSimcop_RM.nRegisteredClients--;
    free(Handle);

  EXIT:
    Semaphore_post(gSimcop_RM.semaphoreForRM);
    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_AcquireResource() This function does the acquisition of one resource 
 *         for the client from the Resource Manager.
 *
 * @details: Does the acuisition of resource from Resource Manager
 *           Pend the semaphoreForRM
 *           1) Read the bIsResourceAcquired[eResourceName] from RM, if it is ZERO, goto 3) 
 *           2) if(acquireMode == BIOS_WAIT_FOREVER) Sem pend the semaphoreForResource[eResourceName], else return RM_SIMCOP_STATUS_ERROR_RESOURCE_BUSY
 *           3) Assign the Handle to clientToWhichResourceGranted[eResourceName]
 *           4) Increment the nTotalResourcesAllocated
 *           5) Set the bIsResourceAcquired[eResourceName]
 *           6) In the Handle set the appropriate bit bitFieldOfResourcesAcquired.
 *           7) Sem pend the semaphoreForResource[eResourceName].
 *           Post the semaphoreForRM
 *
 *
 * @param Handle        : Client RM Handle which is trying to acquire the Resource.
 * @param eResourceName : Name of the Resource trying to be acquired.
 * @param acquireMode   : Acquire Mode
 *
 * @return             : Status of the Acquiring of the resource.
 *
 * @post               : Client has successfully acquired the resource
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_AcquireResource(RM_SIMCOP_CLIENT_HANDLE Handle,
                                           RM_SIMCOP_RESOURCE_NAME
                                           eResourceName,
                                           RM_SIMCOP_ACQUIRE_MODE acquireMode,
                                           RM_SIMCOP_RESOURCE_STATUS *
                                           resStatus)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    Bool semPendStatus;

    Bool isDirty = 0;

    if (eResourceName >= RM_SIMCOP_RESOURCE_MAX)
    {
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }
    // if(IS_DIAG_PRINTS_REQ) Vps_rprintf("%d : RM_SIMCOP:::
    // RM_SIMCOP_acquireResource entered for Handle %x and Resource
    // %d\n",Utils_getCurTimeInMsec(), Handle, eResourceName);

    semPendStatus =
        Semaphore_pend(gSimcop_RM.semaphoreForResource[eResourceName],
                       acquireMode);
    if (semPendStatus == FALSE)
    {
        if (IS_DIAG_PRINTS_REQ)
            Vps_rprintf
                ("%d : RM_SIMCOP::: RM_SIMCOP_acquireResource not successful for Handle %x  and Resource %d\n",
                 Utils_getCurTimeInMsec(), Handle, eResourceName);
        status = RM_SIMCOP_STATUS_ERROR_RESOURCE_BUSY;
        goto EXIT;
    }
    // if(IS_DIAG_PRINTS_REQ) Vps_rprintf("%d : RM_SIMCOP:::
    // RM_SIMCOP_acquireResource successful for Handle %x and Resource
    // %d\n",Utils_getCurTimeInMsec(), Handle, eResourceName);
    gSimcop_RM.nTotalResourcesAllocated++;
    gSimcop_RM.bIsResourceAcquired[eResourceName] = 1;
    if (gSimcop_RM.clientToWhichResourceGranted[eResourceName] != Handle)
        isDirty = 1;
    gSimcop_RM.clientToWhichResourceGranted[eResourceName] = Handle;

    Handle->bitFieldOfResourcesAcquired |= (1 << eResourceName);

    resStatus->bitFieldDirtyResources |= (isDirty << eResourceName);

  EXIT:
    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_ReleaseResource() This function does the de-acquisition of one resource 
 *         for the client from the Resource Manager.
 *
 * @details: Does the de-acquisition of resource from Resource Manager
 *           Pend the semaphoreForRM
 *           1) Read the bIsResourceAcquired[eResourceName] from RM, if it is ZERO, return SUCCESS. 
 *           2) Reset the entry of clientToWhichResourceGranted[eResourceName]
 *           4) Decrement the nTotalResourcesAllocated
 *           5) Reset the bIsResourceAcquired[eResourceName]
 *           6) In the Handle reset the appropriate bit bitFieldOfResourcesAcquired.
 *           7) Post the semaphoreForResource[eResourceName].
 *           Post the semaphoreForRM
 *
 *
 * @param Handle        : Client RM Handle which is trying to release the Resource.
 * @param eResourceName : Name of the Resource trying to be releases.
 *
 * @return             : Status of the releasing of the resource.
 *
 * @post               : Client has successfully released the resource
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_ReleaseResource(RM_SIMCOP_CLIENT_HANDLE Handle,
                                           RM_SIMCOP_RESOURCE_NAME
                                           eResourceName)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    if (eResourceName >= RM_SIMCOP_RESOURCE_MAX)
    {
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }
    // if(IS_DIAG_PRINTS_REQ) Vps_rprintf("%d : RM_SIMCOP:::
    // RM_SIMCOP_ReleaseResource entered for Handle %x and Resource
    // %d\n",Utils_getCurTimeInMsec(), Handle, eResourceName);

    if ((gSimcop_RM.bIsResourceAcquired[eResourceName] == 0) ||
        (gSimcop_RM.clientToWhichResourceGranted[eResourceName] != Handle))
    {
        if (IS_DIAG_PRINTS_REQ)
            Vps_rprintf
                ("%d : RM_SIMCOP::: RM_SIMCOP_ReleaseResource un successful for Handle %x  and Resource %d\n",
                 Utils_getCurTimeInMsec(), Handle, eResourceName);
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }
    // if(IS_DIAG_PRINTS_REQ) Vps_rprintf("%d : RM_SIMCOP:::
    // RM_SIMCOP_ReleaseResource successful for Handle %x and Resource
    // %d\n",Utils_getCurTimeInMsec(), Handle, eResourceName);
    gSimcop_RM.nTotalResourcesAllocated--;
    gSimcop_RM.bIsResourceAcquired[eResourceName] = 0;
    // gSimcop_RM.clientToWhichResourceGranted[eResourceName] = NULL;

    Handle->bitFieldOfResourcesAcquired ^= (1 << eResourceName);
    Semaphore_post(gSimcop_RM.semaphoreForResource[eResourceName]);

  EXIT:

    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_AcquireMultiResources() This function multiple resource acquisition 
 *         for the client from the Resource Manager.
 *
 * @details: Does the acquisition of multiple resources from Resource Manager
 *           Pend the semaphoreForRM
 *           i = 0;
 *           while(i < 32)
 *           {
 *           if(bitFieldOfResourcesToBeAcquired & (1<<i))
 *           {
 *           eResourceName = i;
 *           1) Read the bIsResourceAcquired[eResourceName] from RM, if it is ZERO, goto 3) 
 *           2) if(acquireMode == BIOS_WAIT_FOREVER) Sem pend the semaphoreForResource[eResourceName], else return RM_SIMCOP_STATUS_ERROR_RESOURCE_BUSY
 *           3) Assign the Handle to clientToWhichResourceGranted[eResourceName]
 *           4) Increment the nTotalResourcesAllocated
 *           5) Set the bIsResourceAcquired[eResourceName]
 *           6) In the Handle set the appropriate bit bitFieldOfResourcesAcquired.
 *           7) Sem pend the semaphoreForResource[eResourceName].
 *           }
 *           i++;
 *           }
 *           Post the semaphoreForRM
 *
 *
 * @param Handle        : Client RM Handle which is trying to acquire multiple Resources.
 * @param bitFieldOfResourcesToBeAcquired : BitField of the Resources to be Acquired.
 * @param acquireMode  : Acquire Mode
 *
 * @return             : Status of the acquiring of the resources.
 *
 * @post               : Client has successfully acquired the resources
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_AcquireMultiResources(RM_SIMCOP_CLIENT_HANDLE Handle,
                                                 unsigned int
                                                 bitFieldOfResourcesToBeAcquired,
                                                 RM_SIMCOP_ACQUIRE_MODE
                                                 acquireMode,
                                                 RM_SIMCOP_RESOURCE_STATUS *
                                                 resStatus)
{

    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    unsigned int resource = 0;

    unsigned int unResource = 0;

    // Semaphore_pend(gSimcop_RM.semaphoreForRMA, BIOS_WAIT_FOREVER);
    resStatus->bitFieldDirtyResources = 0;
    if (IS_DIAG_PRINTS_REQ)
        Vps_rprintf
            ("%d : RM_SIMCOP::: RM_SIMCOP_MultiAcquireResource entered for Handle %x\n",
             Utils_getCurTimeInMsec(), Handle);

    for (resource = 0; resource < RM_SIMCOP_RESOURCE_MAX; resource++)
    {
        if (bitFieldOfResourcesToBeAcquired & (1 << resource))
            status =
                RM_SIMCOP_AcquireResource(Handle,
                                          (RM_SIMCOP_RESOURCE_NAME) resource,
                                          acquireMode, resStatus);

        if (status != RM_SIMCOP_STATUS_SUCCESS)
        {
            for (unResource = 0; unResource < resource; unResource++)
                RM_SIMCOP_ReleaseResource(Handle,
                                          (RM_SIMCOP_RESOURCE_NAME) unResource);

            break;
        }

    }
    // Semaphore_post(gSimcop_RM.semaphoreForRMA);
    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_ReleaseMultiResources() This function multiple resource release 
 *         for the client from the Resource Manager.
 *
 * @details: Does the release of multiple resources from Resource Manager
 *           Pend the semaphoreForRM
 *           i = 0;
 *           while(i < 32)
 *           {
 *           if(bitFieldOfResourcesToBeAcquired & (1<<i))
 *           {
 *           eResourceName = i;
 *           1) Read the bIsResourceAcquired[eResourceName] from RM, if it is ZERO,goto next. 
 *           2) Reset the entry of clientToWhichResourceGranted[eResourceName]
 *           4) Decrement the nTotalResourcesAllocated
 *           5) Reset the bIsResourceAcquired[eResourceName]
 *           6) In the Handle reset the appropriate bit bitFieldOfResourcesAcquired.
 *           7) Post the semaphoreForResource[eResourceName].
 *           }
 *           i++;
 *           }
 *           Post the semaphoreForRM
 *
 *
 * @param Handle        : Client RM Handle which is trying to release multiple Resources.
 * @param bitFieldOfResourcesToBeAcquired : BitField of the Resources to be Released.
 *
 * @return             : Status of the releasing of the resources.
 *
 * @post               : Client has successfully released the resources
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_ReleaseMultiResources(RM_SIMCOP_CLIENT_HANDLE Handle,
                                                 unsigned int
                                                 bitFieldOfResourcesToBeAcquired)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    unsigned int resource = 0;

    // Semaphore_pend(gSimcop_RM.semaphoreForRMR, BIOS_WAIT_FOREVER);
    if (IS_DIAG_PRINTS_REQ)
        Vps_rprintf
            ("%d : RM_SIMCOP::: RM_SIMCOP_MultiReleaseResource entered for Handle %x  \n",
             Utils_getCurTimeInMsec(), Handle);

    for (resource = 0; resource < RM_SIMCOP_RESOURCE_MAX; resource++)
    {
        if (bitFieldOfResourcesToBeAcquired & (1 << resource))
            status |=
                RM_SIMCOP_ReleaseResource(Handle,
                                          (RM_SIMCOP_RESOURCE_NAME) resource);
    }

    // Semaphore_post(gSimcop_RM.semaphoreForRMR);
    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_QueryResourceInfo() This function queries the Resource Manager of Resource Status.
 *
 * @details: 
 *           Fill the ptResInfo based on the status of the Resource eResourceName
 *
 *
 * @param eResourceName : Resource whose status is queried.
 * @param ptResInfo     : Resource Info.
 *
 * @return              : Status of the querying.
 *
 * @post                : Client has successfully queried the status of resources
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_QueryResourceInfo(RM_SIMCOP_RESOURCE_NAME
                                             eResourceName,
                                             RM_SIMCOP_RESOURCE_INFO *
                                             ptResInfo)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    if (eResourceName >= RM_SIMCOP_RESOURCE_MAX)
    {
        status = RM_SIMCOP_STATUS_ERROR;
        goto EXIT;
    }

    ptResInfo->bIsAcquired = gSimcop_RM.bIsResourceAcquired[eResourceName];
    ptResInfo->whoAcquired =
        gSimcop_RM.clientToWhichResourceGranted[eResourceName];
  EXIT:
    return status;
}

/* =========================================================================== */
/**
 * @func RM_SIMCOP_RelinquishAllResources() 
 *  This function relinquishes all the resources hold by the RM handle at any point of time.
 *
 * @details: 
 *           relinquishes all the resources hold by the RM handle at any point of time.
 *
 * @param Handle        : Client RM Handle which is trying to release multiple Resources.
 *
 * @return              : Status of the relinquishing.
 *
 * @post                : Client has successfully relinquished all the resources in its hold
 */
/* =========================================================================== */

RM_SIMCOP_STATUS RM_SIMCOP_RelinquishAllResources(RM_SIMCOP_CLIENT_HANDLE
                                                  Handle)
{
    RM_SIMCOP_STATUS status = RM_SIMCOP_STATUS_SUCCESS;

    unsigned int resource = 0;

    for (resource = 0; resource < RM_SIMCOP_RESOURCE_MAX; resource++)
    {
        if (Handle->bitFieldOfResourcesAcquired & (1 << resource))
            status |=
                RM_SIMCOP_ReleaseResource(Handle,
                                          (RM_SIMCOP_RESOURCE_NAME) resource);
    }

    return status;
}
