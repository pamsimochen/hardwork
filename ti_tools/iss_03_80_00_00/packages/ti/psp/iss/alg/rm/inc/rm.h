/** ==================================================================
 *  @file   rm.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/rm/inc/                                                  
 *                                                                    
 *  @desc   This  File contains defines the data types, data structures and APIs 
 * of Resource Manager for SIMCOP \version 1.0.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/* !
 * ****************************************************************************
 * \file rm.h \brief This file defines the data types, data structures and APIs 
 * of Resource Manager for SIMCOP \version 1.0
 * **************************************************************************** */
/* =========================================================================
 * ! ! Revision History ! =================================== ! !
 * 09-Nov-2011: Venugopala Krishna: Initial version
 * ========================================================================= */

#ifndef RM_SIMCOP_H_
#define RM_SIMCOP_H_

#ifdef __cplusplus

extern "C" {
#endif

/******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <stdlib.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/iss/common/iss_types.h>
#include <ti/psp/iss/core/msp_types.h>
#include <ti/psp/vps/vps.h>

extern UInt32 Utils_getCurTimeInMsec();

#define SIMCOP_RM_MAX_TIME_OUT 1000

#define IS_DIAG_PRINTS_REQ  (0)

/******************************************************************************
 * ENUMS
 ******************************************************************************/

/*==========================================================================*/
/**
 *  @brief      RM_SIMCOP_STATUS:
 *  This enum definition contains the return types for all the
 *  Resource Manager functions. Any new return type has to be added here.
 */
/*==========================================================================*/
typedef enum _RM_SIMCOP_STATUS 
{
        RM_SIMCOP_STATUS_SUCCESS = 0,
        RM_SIMCOP_STATUS_ERROR,
        RM_SIMCOP_STATUS_ERROR_RESOURCE_BUSY
} RM_SIMCOP_STATUS;

/*==========================================================================*/
/**
 *  @brief      RM_SIMCOP_RESOURCE_NAME:
 *  Enum definition for Resource Names that are managed by the Resource
 *  Manager. These 'resource' names will be used during the RM_Init as well
 *  as during interaction between RM Clients and RM.
 */
/* ==========================================================================*/
    typedef enum _RM_SIMCOP_RESOURCE_NAME {
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_LDC,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_NSF,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_DMA,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_ROT,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_VLCDJ,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_DCT,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_HWSEQ,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_A,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_B,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_C,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_D,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_E,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_F,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_G,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_H,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_A,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_B,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_CMD_MEM_A,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_CMD_MEM_B,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_COEFF_MEM_A,
        RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_COEFF_MEM_B,
        RM_SIMCOP_RESOURCE_NAME_INVALID,
        RM_SIMCOP_RESOURCE_MAX = RM_SIMCOP_RESOURCE_NAME_INVALID
    } RM_SIMCOP_RESOURCE_NAME;

#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_LDC              (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_LDC)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_NSF              (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_NSF)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DMA              (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_DMA)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_ROT              (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_ROT)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_VLCDJ            (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_VLCDJ)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_DCT              (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_DCT)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_HWSEQ            (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_HWSEQ)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_A       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_A)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_B       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_B)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_C       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_C)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_D       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_D)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_E       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_E)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_F       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_F)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_G       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_G)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMAGEBUF_H       (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMAGEBUF_H)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMX_A            (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_A)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMX_B            (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_B)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMX_CMD_MEM_A    (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_CMD_MEM_A)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMX_CMD_MEM_B    (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_CMD_MEM_B)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMX_COEFF_MEM_A  (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_COEFF_MEM_A)
#define  RM_SIMCOP_RESOURCE_BFIELD_SIMCOP_IMX_COEFF_MEM_B  (1<<RM_SIMCOP_RESOURCE_NAME_SIMCOP_IMX_COEFF_MEM_B)

    /* ========================================================================== 
     */
/**
 *  @brief      RM_SIMCOP_ACQUIRE_MODE:
 *  This enum definition contains the type of acquire Mode.
 *  Eg; While acquiring do the client need to pend till resource is available 
 *      or immediately return with busy status
 */
    /* ========================================================================== 
     */
    typedef enum _RM_SIMCOP_ACQUIRE_MODE {
        RM_SIMCOP_ACQUIRE_WAIT = BIOS_WAIT_FOREVER,
        RM_SIMCOP_ACQUIRE_IMME = BIOS_NO_WAIT,
        RM_SIMCOP_ACQUIRE_UNDEFINED
    } RM_SIMCOP_ACQUIRE_MODE;

/******************************************************************************
 * STRUCTURE DEFINITIONS
 ******************************************************************************/

    /* ========================================================================== 
     */
/** @brief RM_SIMCOP_RESOURCE_PARAMS
 * This structure holds the resource details passed by the client to RM during
 * call to get resource handle API
 *
 * @param  nSize                     Contains the size of the structure
 * @param  eResourceName             Name of the resource (RM_RESOURCE_NAME)
 * @param  pExtResourceParams        Extended field to hold a pointer to
 *                                   resource specific information
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_RESOURCE_PARAMS {
        unsigned int nSize;
        RM_SIMCOP_RESOURCE_NAME eResourceName;
        void *pExtResourceParams;
    } RM_SIMCOP_RESOURCE_PARAMS;

    /* ========================================================================== 
     */
/** @brief _RM_SIMCOP_RESOURCE_STATUS
 * The value in this structure is populated by resource manager when granting
 * access to specific resource requested by the client
 *
 * @param  bitFieldDirtyResources    It is a bitField of Dirty status of various
 *                                   Resources 
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_RESOURCE_STATUS {
        unsigned int bitFieldDirtyResources;
    } RM_SIMCOP_RESOURCE_STATUS;

    /* ========================================================================== 
     */
/**
 * @brief RM_SIMCOP_CLIENT_PARAMS
 * This structure holds the client details passed to Resource Manager during
 * one time client registeration through RM_RegisterClient API
 *
 * @param  nSize                     Contains the size of the structure
 * @param  pClientHandle             Client Handle
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_CLIENT_PARAMS {
        unsigned int nSize;
        void *pClientHandle;
    } RM_SIMCOP_CLIENT_PARAMS;

    /* ========================================================================== 
     */
/**
 * @brief RM_SIMCOP_CLIENT
 * This structure holds the client details once the the client is registered
 *
 * @param  nSize                        Contains the size of the structure
 * @param  pClientHandle                Client Handle
 * @param  bitFieldOfResourcesAcquired  BitField telling which resources are under its possesion
 * @param  registryIndex                Contains the registryIndex of this client with RM
 * @param  timeForTimeOut               Contains the value for the timeout to be used
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_CLIENT {
        unsigned int nSize;
        void *pClientHandle;
        unsigned int bitFieldOfResourcesAcquired;
        unsigned int registryIndex;
        unsigned int timeForTimeOut;
    } RM_SIMCOP_CLIENT;

    typedef RM_SIMCOP_CLIENT *RM_SIMCOP_CLIENT_HANDLE;

    /* ========================================================================== 
     */
/**
 * @brief RM_SIMCOP_CLIENT_CONFIG
 * This structure holds client configurations for a specific resource passed to
 * RM while getting handle to any resource through RM_GetResourceHandle API
 *
 * @param  nSize        Contains the size of the structure
 * @param  bBlocking    Field indicating RM behavior.
 *                      TRUE  => RM does not return resource untill resource is 
 *                      free
 *                      FALSE => RM always returns immediately. If resource is
 *                      available then it is granted or else return error 
 *                      prompting that the resource is busy.
 * @param  pAppData     Client specific data for the specific resource
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_CLIENT_CONFIG {
        unsigned int nSize;
        unsigned char bBlocking;
        void *pAppData;
    } RM_SIMCOP_CLIENT_CONFIG;

    /* ========================================================================== 
     */
/**
 * @brief RM_SIMCOP_RESOURCE_INFO
 * This structure gives basic information on the resource for which query
 * is called
 *
 * @param  nSize                     Contains the size of the structure
 * @param  bIsAcquired               Resource is in use (i.e. is acquired)
 * @param  whoAcquired               who Acquired the resource
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_RESOURCE_INFO {
        unsigned int nSize;
        unsigned char bIsAcquired;
        RM_SIMCOP_CLIENT_HANDLE whoAcquired;
    } RM_SIMCOP_RESOURCE_INFO;

/**
 * Defines the maximum algos which is serviced by Resource Manager. 
 */
#define RM_SIMCOP_MAX_CLIENTS  (40)

    /* ========================================================================== 
     */
/**
 * @brief RM_SIMCOP_RESOURCE_MANAGER
 * This structure is the basic data structure with which RM does all the 
 * Resource Management and Arbitration.
 *
 * @param  nSize                     Contains the size of the structure
 * @param  clientRegistry            Registry of clients who got registered
 * @param  nRegisteredClients        Number of registered clients at any point 
 *                                   of time
 * @param  nTotalResourcesAllocated  Number of resources allocated at any point 
 *                                   of time.
 * @param  bIsResourceAcquired       Gives the status of all resources whether
 *                                   they are free or acquired.
 * @param  clientToWhichResourceGranted 
 *                                   If a resource got acquired, this tells to 
 *                                   which client it has been granted.
 * @param semaphoreForResource       Semaphore for each resource for arbitration.
 * @param semaphoreForRM             Semaphore for the RM to arbitrate different
 *                                   calls of RM from different clients.
 */
    /* ========================================================================== 
     */
    typedef struct _RM_SIMCOP_RESOURCE_MANAGER {
        unsigned int nSize;
        void *clientRegistry[RM_SIMCOP_MAX_CLIENTS];
        unsigned int nRegisteredClients;
        unsigned int nTotalResourcesAllocated;
        unsigned char bIsResourceAcquired[RM_SIMCOP_RESOURCE_MAX];
        void *clientToWhichResourceGranted[RM_SIMCOP_RESOURCE_MAX];
        Semaphore_Handle semaphoreForResource[RM_SIMCOP_RESOURCE_MAX];
        Semaphore_Handle semaphoreForRM;
        Semaphore_Handle semaphoreForRMA;
        Semaphore_Handle semaphoreForRMR;
    } RM_SIMCOP_RESOURCE_MANAGER;

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_Init() This function does the one time initialization operation 
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
    RM_SIMCOP_STATUS RM_SIMCOP_Init(void);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_DeInit() This function does the one time de-initialization operation 
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
    RM_SIMCOP_STATUS RM_SIMCOP_DeInit(void);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_RegisterClient() This function does the registration of the client 
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
    RM_SIMCOP_STATUS RM_SIMCOP_RegisterClient(RM_SIMCOP_CLIENT_PARAMS
                                              clientParams,
                                              RM_SIMCOP_CLIENT_HANDLE * Handle);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_DeRegisterClient() This function does the de-registration of the client 
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
    RM_SIMCOP_STATUS RM_SIMCOP_DeRegisterClient(RM_SIMCOP_CLIENT_HANDLE Handle);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_AcquireResource() This function does the acquisition of one resource 
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
 * @param status        : Status of the Resource Acquired
 *
 * @return             : Status of the Acquiring of the resource.
 *
 * @post               : Client has successfully acquired the resource
 */
    /* =========================================================================== */
    RM_SIMCOP_STATUS RM_SIMCOP_AcquireResource(RM_SIMCOP_CLIENT_HANDLE Handle,
                                               RM_SIMCOP_RESOURCE_NAME
                                               eResourceName,
                                               RM_SIMCOP_ACQUIRE_MODE
                                               acquireMode,
                                               RM_SIMCOP_RESOURCE_STATUS *
                                               status);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_ReleaseResource() This function does the de-acquisition of one resource 
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
                                               eResourceName);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_AcquireMultiResources() This function multiple resource acquisition 
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
 * @param acquireMode   : Acquire Mode
 * @param status        : Status of the Resources Acquired
 *
 * @return             : Status of the acquiring of the resources.
 *
 * @post               : Client has successfully acquired the resources
 */
    /* =========================================================================== */
    RM_SIMCOP_STATUS RM_SIMCOP_AcquireMultiResources(RM_SIMCOP_CLIENT_HANDLE
                                                     Handle,
                                                     unsigned int
                                                     bitFieldOfResourcesToBeAcquired,
                                                     RM_SIMCOP_ACQUIRE_MODE
                                                     acquireMode,
                                                     RM_SIMCOP_RESOURCE_STATUS *
                                                     status);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_ReleaseMultiResources() This function multiple resource release 
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
    RM_SIMCOP_STATUS RM_SIMCOP_ReleaseMultiResources(RM_SIMCOP_CLIENT_HANDLE
                                                     Handle,
                                                     unsigned int
                                                     bitFieldOfResourcesToBeAcquired);

    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_QueryResourceInfo() This function queries the Resource Manager of Resource Status.
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
                                                 ptResInfo);
    /* =========================================================================== */
/**
 * @fn RM_SIMCOP_RelinquishAllResources() 
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
                                                      Handle);

#ifdef __cplusplus
}
#endif
#endif                                                     /* RM_SIMCOP_H_ */
