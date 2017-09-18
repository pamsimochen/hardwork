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
********************************************************************************
 * @file mctnf_ti_rman_config.h
 *
 * @brief This file contains RMAN API functions
 *
 *        This file contains functions to emulate actual RMAN in system
 *
 * @author: Pramod Swami (pramods@ti.com)
 *
 * @version 0.0 (Jan 2008) : Base version created
 *                           [Pramod]
 * @version 0.1 (Jan 2013) : Removed requesting of 2D reference buffers.
 *                           [Shyam Jagannathan]
********************************************************************************
*/
#ifndef MCTNF_TI_RMAN_CONFIG_H__
#define MCTNF_TI_RMAN_CONFIG_H__

#include "mctnf_ti_config.h"
#include "mctnf_ti_test.h"

/**
 *  Macro defining total possible number of IVAHD resources
*/
#define NUM_IVAHD_RESOURCES 5

/**
 *  Enable Data sinc mode
 *  Note that Data sync mode is not supported in singl processor(IVAHD)mode
 *  So enable accordingly.
*/
#ifdef HOST_SEPERATE
#define BITSTREAM_SUBFRAME_SYNC
#endif
/**
 *  First data unit size Non multiple of 4K
*/
#define INITIAL_DATA_UNIT       0x00000258


/**
 *  Macro specifieng the output unit size when data sync feature is enabled
*/
#define DATA_SYNCH_UNIT         0x00000800

/**
  * Gap between two data synch units
*/
#define DATA_SYNCH_UNIT_GAP     0x00
/**
 *  Macro specifieng Total number of output blocks when data sync is enabled
*/
#define NUM_BLOCKS (8)
/**
 *  Macro specifieng Maximum number of blocks can be given for each data request
*/
#define MAX_NUM_INPUT_BLOCKS (15)

/**
 *  Macro specifieng Tiled memory base address
*/
#define TILERBASEADDRESS ((Void*)0x60000000)

extern XDAS_Int32 gInitialDataUnit;
extern XDAS_Int32 gDataSynchUnit;

/**
 *  Arry of algorithm handle
*/
extern IALG_Handle g_handle[NUM_IVAHD_RESOURCES];

/*----------------------------------------------------------------------------*/
/* Flag to check whetehr Aquire call made or not                              */
/* NOTE: need to check with Uday why it is needed                             */
/*----------------------------------------------------------------------------*/
extern Uint32 isAcquireCallMade;

/*----------------------------------------------------------------------------*/
/*  Global variables which will hold IVAHD Config, SL2 PRCM base address      */
/*----------------------------------------------------------------------------*/
extern Uint32 MEM_BASE;
extern Uint32 REG_BASE;
extern Uint32 RST_CNTL_BASE;

extern IRES_HDVICP2_Handle gHDVICP2Handle ;
/*This enum should be removed after making actula calls to rman*/

/**
 *******************************************************************************
 *  @enum       _IRESResourcesHost
 *  @brief      Resource information for MCTNF.
 *
 *  @remarks    allocates appropriate number for each resource
 *
 *  @remarks    XXX_RESOURCE_MAX will specify max number of resources of
 *              perticular type
 *
 *  @remarks    Contains HDVICP and 1D memory resources
 *
 *  @remarks    Provides total numer of resource of perticular type
 *******************************************************************************
*/

typedef enum _IRESResourcesHost {
  IRES_HDVICP_RESOURCE_START = 0,
  IRES_HDVICP_RESOURCE_IVAHD_0 = IRES_HDVICP_RESOURCE_START,
  IRES_HDVICP_RESOURCE_MAX,

  IRES_TILED_MEMORY_1D_RESOURCE_START = IRES_HDVICP_RESOURCE_MAX,
  IRES_TILED_MEMORY_1D_RESOURCE_COL_MBINFO = IRES_TILED_MEMORY_1D_RESOURCE_START,
  IRES_TILED_MEMORY_1D_RESOURCE_PERSISTENT_MEM,
  IRES_TILED_MEMORY_1D_RESOURCE_DEBUGTRACE_MEM,
  IRES_TILED_MEMORY_1D_RESOURCE_MAX,

  IRES_HDVICP_RES = IRES_HDVICP_RESOURCE_MAX - IRES_HDVICP_RESOURCE_START,
  IRES_TILED_MEMORY_1D_RES = IRES_TILED_MEMORY_1D_RESOURCE_MAX - IRES_TILED_MEMORY_1D_RESOURCE_START,
  IRES_TOTAL_RESOURCES = IRES_TILED_MEMORY_1D_RESOURCE_MAX
} IRESResourcesHost;


/*----------------------------------------------------------------------------*/
/* Interrupt vector table type basically it contains pointers to the functions*/
/*----------------------------------------------------------------------------*/
typedef void (*ISR_VECTOR_TABLE)();


/**
********************************************************************************
 *  @fn     Init_IVAHDAddrSpace
 *  @brief  Function to get the IVAHD Config, SL2, PRCM base address based
 *          on IVAHD ID
 *
 *  @param[in]  None
 *
 *  @return     None
********************************************************************************
*/
void Init_IVAHDAddrSpace();

/**
********************************************************************************
 *  @fn     ISR_Function
 *  @brief  ISR function for Mail box interrupts
 *
 *          ISR function for all the IVAHD Mail box interrupts which intern
 *          calls callback api function
 *
 *  @param[in]  None
 *
 *  @return     None
********************************************************************************
*/
void ISR_Function();

/**
********************************************************************************
 *  @fn     HDVICP_Acquire
 *  @brief  Function to acquire HDVICP resource
 *
 *
 *  @param[in] handle : pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @param[in] yieldCtxt : pointer to the context switch handle
 *
 *  @param[out] status : Pointer to the HDVICP status structure in which
 *                       status of the aquired resource is returned
 *  @param[in/out] modeId : The mode in which this particular HDVICP2 was
 *                          used last time, and the mode in which it will be
 *                          used this time
 *  @param[in] lateAcquireArg : This is the channel id which algorithm returns
 *                              while acquiring the HDVICP2
 *  @return     None
********************************************************************************
*/
XDAS_Void HDVICP_Acquire(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle,
          IRES_YieldContext *yieldCtxt, XDAS_UInt32 *status,
          Uint32* modeId, Int lateAcquireArg );

/**
********************************************************************************
 *  @fn     HDVICP_reacquireIfOwner
 *  @brief  Function to acquire HDVICP resource
 *
 *
 *  @param[in] handle : pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @param[in] yieldCtxt : pointer to the context switch handle
 *
 *  @param[out] status : Pointer to the HDVICP status structure in which
 *                       status of the aquired resource is returned
 *  @param[in/out] modeId : The mode in which this particular HDVICP2 was
 *                          used last time, and the mode in which it will be
 *                          used this time
 *  @return     None
********************************************************************************
*/
XDAS_UInt32 HDVICP_reacquireIfOwner(IALG_Handle handle,
                                                IRES_HDVICP2_Handle iresHandle);
/**
********************************************************************************
 *  @fn     HDVICP_Configure
 *  @brief  Function to configure HDVICP resource
 *
 *
 *  @param[in] handle : Pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @param[in] IRES_HDVICP2_CallbackFxn : Pointer to the call back function,
 *                                        This will be tied to ISR function
 *
 *  @param[out] cbArgs : Argument to be passed to the callback function
 *
 *  @return     None
********************************************************************************
*/
void HDVICP_Configure(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle,
               void (*IRES_HDVICP2_CallbackFxn)(IALG_Handle handle, void *cbArgs),
         void *cbArgs);

/**
********************************************************************************
 *  @fn     HDVICP_Release
 *  @brief  Function to release HDVICP resource
 *
 *  @param[in] handle : Pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @return     None
********************************************************************************
*/
void HDVICP_Release(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle);

/**
********************************************************************************
 *  @fn     HDVICP_Wait
 *  @brief  Function to wait for Interrupt from IVAHD mail box
 *
 *  @param[in] handle : Pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @param[in] yieldCtxt : Pointer to the context switch handle
 *
 *  @return     None
********************************************************************************
*/
XDAS_UInt32 HDVICP_Wait(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle,
                                                  IRES_YieldContext * yieldCtxt);

/**
********************************************************************************
 *  @fn     HDVICP_Done
 *  @brief  Function to send HDVICP resouce finished processing
 *          This function will be called by the inerrrupt handler function when
 *          it detects end of frame processing
 *
 *  @param[in] handle : Pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @return     None
********************************************************************************
*/
void HDVICP_Done(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle);

/**
********************************************************************************
 *  @fn     HDVICP_Reset
 *  @brief  Function to reset HDVICP resouce before using it
 *          This function will be called by algorithm when needed
 *
 *  @param[in] handle : Pointer to algorithm handle
 *
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @return     0: Success, non-zero : Failure
 *
 *  @return     None
********************************************************************************
*/
XDAS_UInt32 HDVICP_Reset(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle);

/**
********************************************************************************
 *  @fn     allignMalloc
 *  @brief  Memory allocate function with alignment
 *
 *  @param[in] alignment : alignment value
 *
 *  @param[in] size : size of the buffer to be allocated in bytes
 *
 *  @return     Pointer to the allocated buffer
 *              Error in case allocation fails
********************************************************************************
*/
void *allignMalloc(size_t alignment, size_t size);

/**
********************************************************************************
 *  @fn     allignFree
 *  @brief  Funtion to free the memory allocated with "allignMalloc" fucntion
 *
 *  @param[in] ptr : pointer to the buffer free
 *
 *  @return     None
********************************************************************************
*/
void allignFree(void *ptr);

/**
********************************************************************************
 *  @fn     RMAN_AssignResources
 *  @brief  Funtion to assigne the HDVICP and Memory resources, This is dummy
 *          implimentation to emulate exact RMAN module API
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @return     IRES_Status : status of the resource assigment
 *
********************************************************************************
*/
IRES_Status RMAN_AssignResources(IALG_Handle handle, XDAS_Int8 lumaTilerSpace,
                                 XDAS_Int8 chromaTilerSpace);

/**
********************************************************************************
 *  @fn     RMAN_activateAllResources
 *  @brief  Funtion to activate all the resources
 *          This fucntion intern calls algorithm call back function to activate
 *          all the resources
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @return     Status of the activation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_activateAllResources (IALG_Handle handle);

/**
********************************************************************************
 *  @fn     RMAN_deactivateAllResources
 *  @brief  Funtion to deactivate all the resources
 *          This fucntion intern calls algorithm call back function to
 *          deactivate all the resources
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @return     Status of the deactivation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_deactivateAllResources (IALG_Handle handle);

/**
********************************************************************************
 *  @fn     RMAN_activateHDVICP2Resource
 *  @brief  Funtion to activate HDVICP2 resource
 *          This fucntion intern calls algorithm call back function to activate
 *          HDVICP2 resource
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @param[in] resourceHandle : Pointer to the HDVICP2 handle
 *
 *  @return     Status of the activation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_activateHDVICP2Resource (IALG_Handle handle,
  IRES_Handle resourceHandle);

/**
********************************************************************************
 *  @fn     RMAN_deactivateHDVICP2Resource
 *  @brief  Funtion to deactivate HDVICP2 resource
 *          This fucntion intern calls algorithm call back function to
 *          deactivate HDVICP2 resource
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @param[in] resourceHandle : Pointer to the HDVICP2 handle
 *
 *  @return     Status of the deactivation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_deactivateHDVICP2Resource (IALG_Handle handle,
  IRES_Handle resourceHandle);

/**
********************************************************************************
 *  @fn     RMAN_FreeResources
 *  @brief  Funtion to free the resources allocated
 *          This fucntion basically frees all the memory allocated for memory
 *          resources
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @return     None
 *
********************************************************************************
*/
void RMAN_FreeResources(IALG_Handle handle);

/**
********************************************************************************
 *  @fn     MCTNFTest_DataSyncGetBufFxn
 *  @brief  Funtion to initialize the data sync module
 *
 *  @param[in] dataSyncHandle : pointer to the datasync handle
 *
 *  @param[in] dataSyncDesc : pointer to the datasync discriptors
 *
 *  @return     0 : when no error
 *             -1 : in case of errors
 *
********************************************************************************
*/
XDAS_Int32 MCTNFTest_DataSyncGetBufFxn(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/**
********************************************************************************
 *  @fn     MCTNFTest_DataSyncGetDataFxn1
 *  @brief  Funtion to Give fill buffer pointer details to the input YUV buufer
 *
 *  @param[in] dataSyncHandle : pointer to the datasync handle
 *
 *  @param[in] dataSyncDesc : pointer to the datasync discriptors
 *
 *  @return     0 : when no error
 *             -1 : in case of errors
 *
********************************************************************************
*/
XDAS_Int32 MCTNFTest_DataSyncGetDataFxn1(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/**
********************************************************************************
 *  @fn     MCTNFTest_DataSyncGetDataFxn2
 *  @brief  Funtion to Give fill buffer pointer details to the input YUV buufer
 *
 *  @param[in] dataSyncHandle : pointer to the datasync handle
 *
 *  @param[in] dataSyncDesc : pointer to the datasync discriptors
 *
 *  @return     0 : when no error
 *             -1 : in case of errors
 *
********************************************************************************
*/
XDAS_Int32 MCTNFTest_DataSyncGetDataFxn2(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/**
********************************************************************************
 *  @fn     MEMUTILS_getPhysicalAddr
 *  @brief  Funtion to get physical address of DDR memory
 *
 *  @param[in] addr : pointer to the address to get corresponding physical mem
 *
 *  @return     Pointer to the physical memory
 *
********************************************************************************
*/
Void * MEMUTILS_getPhysicalAddr(Void * addr);

#endif /* MCTNF_TI_RMAN_CONFIG_H__ */

