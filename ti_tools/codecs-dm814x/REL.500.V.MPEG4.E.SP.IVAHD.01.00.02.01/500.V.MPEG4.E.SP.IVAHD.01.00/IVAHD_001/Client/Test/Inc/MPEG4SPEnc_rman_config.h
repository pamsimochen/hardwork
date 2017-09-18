/*
********************************************************************************
 * HDVICP2.0 Based MPEG4 SP Encoder
 *
 * "HDVICP2.0 Based MPEG4 SP Encoder" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw
 *  video into a simple profile bit-stream. Based on ISO/IEC 14496-2."
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/
/**
********************************************************************************
 * @file MPEG4SPEnc_rman_config.h
 *
 * @brief This file contains RMAN API functions
 *
 *        This file contains functions to emulate actual RMAN in system
 *
 * @author: Pramod Swami (pramods@ti.com)
 *
 * @version 0.0 (Jan 2008) : Base version created
 *                           [Pramod]
 * @version 0.1 (Nov 2009) : Review and more commenting along with cleanup
 *                           [Venugopala Krishna]
********************************************************************************
*/
#ifndef MPEG4SPENC_RMAN_CONFIG_H__
#define MPEG4SPENC_RMAN_CONFIG_H__

#include "mpeg4enc_ti_config.h"
/**
 *  Macro defining total possible number of IVAHD resources
*/
#define NUM_IVAHD_RESOURCES 5

/**
 *  Enable Data sinc mode
*/
#define BITSTREAM_SUBFRAME_SYNC

/**
 *  First data unit size Non multiple of 4K
*/
#define INITIAL_DATA_UNIT       0x00000100


/**
 *  Macro specifieng the output unit size when data sync feature is enabled
*/
#define DATA_SYNCH_UNIT         0x00001000
/**
 *  Macro specifieng Total number of output blocks when data sync is enabled
*/
#define NUM_BLOCKS (1)

/**
 *  Macro specifieng Tiled memory base address
*/
#define TILERBASEADDRESS ((Void*)0x60000000)


#ifdef HOST_M3 
/**
 *  HOST_M3 is defined if Host is is ARM M3 on any SoC
*/
#ifdef A9_HOST_FILE_IO

#define MEM_BASE_IVAHD0      0xBB000000
#define REG_BASE_IVAHD0      0xBA000000
#define RST_CNTL_BASE_IVAHD0 0xAA306F10

/**
 *  For OMAP4 SoC COnfiguration
*/
#elif HOSTCORTEXM3_OMAP4

#define MEM_BASE_IVAHD0  0x5B000000
#define REG_BASE_IVAHD0  0x5A000000
#define RST_CNTL_BASE_IVAHD0 0x4A306F10

#elif NETRA_SIM/* HOSTCORTEXM3_OMAP4 */

#define MEM_BASE_IVAHD0  0x59000000
#define REG_BASE_IVAHD0  0x58000000
#define RST_CNTL_BASE_IVAHD0 0x48180C10

#define MEM_BASE_IVAHD1  0x5B000000
#define REG_BASE_IVAHD1  0x5A000000
#define RST_CNTL_BASE_IVAHD1 0x48180D10

#define MEM_BASE_IVAHD2  0x54000000
#define REG_BASE_IVAHD2  0x53000000
#define RST_CNTL_BASE_IVAHD2 0x48180E10


#endif /*A9_HOST_FILE_IO */

#else /* HOST_M3*/

/* Incase of FPGA */
#define MEM_BASE_IVAHD0  0x90000000
#define REG_BASE_IVAHD0  0x50080000
#define RST_CNTL_BASE_IVAHD0 0x30082000

#endif /* HOST_M3 */

/*----------------------------------------------------------------------------*/
/*  Global variables which will hold IVAHD Config, SL2 PRCM base address      */
/*----------------------------------------------------------------------------*/
extern Uint32 MEM_BASE;
extern Uint32 REG_BASE;
extern Uint32 RST_CNTL_BASE;

/*This enum should be removed after making actula calls to rman*/

/**
 *******************************************************************************
 *  @enum       _IRESResourcesHost
 *  @brief      Resource information for the H264 encoder.
 *
 *  @remarks    allocates appropriate number for each resource
 *
 *  @remarks    XXX_RESOURCE_MAX will specify max number of resources of
 *              perticular type
 *
 *  @remarks    Contains HDVICP and 1D, 2D memory resources
 *
 *  @remarks    Provides total numer of resource of perticular type
 *******************************************************************************
*/

/*This enum should be removed after making actula calls to rman*/
typedef enum _IRESResources_Host
{
  IRES_HDVICP_RESOURCE_IVAHD_0 = 0,
  IRES_TILED_MEMORY_1D_RESOURCE_START = 1,
  IRES_TILED_MEMORY_1D_RESOURCE_MVINFO = 1,
  IRES_TILED_MEMORY_1D_RESOURCE_PERSISTENT_MEM = 2,
  IRES_TILED_MEMORY_1D_RESOURCE_MBINFO = 3,
  IRES_TILED_MEMORY_1D_RESOURCE_RESIDUALINFO = 4,
  IRES_TILED_MEMORY_1D_RESOURCE_END = 4,
  IRES_TILED_MEMORY_2D_RESOURCE_START = 5,
  IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_LUMA_0 = 5,
  IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_CHROMA_0 = 6,
  IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_LUMA_1 = 7,
  IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_CHROMA_1 = 8,
  IRES_TILED_MEMORY_2D_RESOURCE_END = 8,

  DEBUGTRACESTRUCT = 9,
    /**<
    * For debug trace parameter storage
    */ 

  IRES_TOTAL_RESOURCES


}IRESResources_Host;

#if defined(HOST_M3) || defined(HOST_GAIA) || defined(OMAP4_SDC)
/*----------------------------------------------------------------------------*/
/* Interrupt vector table type basically it contains pointers to the functions*/
/*----------------------------------------------------------------------------*/
typedef void (*ISR_VECTOR_TABLE)();
#endif


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
 *  @func     ISR_Function
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
 *  @func     HDVICP_Acquire
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
 *  @param[in] lateAcquireArg : This is the channel id which codec returns
 *                              while acquiring the HDVICP2
 *  @return     None
********************************************************************************
*/
XDAS_Void HDVICP_Acquire(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle,
          IRES_YieldContext *yieldCtxt, XDAS_UInt32 *status,
          Uint32* modeId, Int lateAcquireArg );

/**
********************************************************************************
 *  @func     HDVICP_reacquireIfOwner
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
 *  @func     HDVICP_Configure
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
XDAS_Void HDVICP_Configure(IALG_Handle handle,
                      IRES_HDVICP2_Handle iresHandle,
                      void (*IRES_HDVICP2_CallbackFxn)
                      (IALG_Handle handle, void *cbArgs),
                      void *cbArgs);

/**
********************************************************************************
 *  @func     HDVICP_Release
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
 *  @func     HDVICP_Wait
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
XDAS_UInt32 HDVICP_Wait(IALG_Handle handle,
                 IRES_HDVICP2_Handle iresHandle,
                 IRES_YieldContext * yieldCtxt);

/**
********************************************************************************
 *  @func     HDVICP_Done
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
XDAS_Void HDVICP_Done(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle);

/**
********************************************************************************
 *  @func     HDVICP_Reset
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
 *  @func     allignMalloc
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
 *  @func     allignFree
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
 *  @func     RMAN_activateAllResources
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
 *  @func     RMAN_deactivateAllResources
 *  @brief  Funtion to deactivate all the resources
 *          This fucntion intern calls algorithm call back function to
 *          deactivate all the resources
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *
 *  @return     Status of the activation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_deactivateAllResources (IALG_Handle handle);

/**
********************************************************************************
 *  @func     RMAN_FreeResources
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
void RMAN_FreeResources(IALG_Handle handle, XDAS_Int8 lumaTilerSpace, 
                                 XDAS_Int8 chromaTilerSpace);

/**
********************************************************************************
 *  @func     H264ETest_DataSyncGetBufFxn
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
XDAS_Int32 H264ETest_DataSyncGetBufFxn(XDM_DataSyncHandle dataSyncHandle,
        XDM_DataSyncDesc *dataSyncDesc);

/**
********************************************************************************
 *  @func     MEMUTILS_getPhysicalAddr
 *  @brief  Funtion to get physical address of DDR memory
 *
 *  @param[in] addr : pointer to the address to get corresponding physical mem
 *
 *  @return     Pointer to the physical memory
 *
********************************************************************************
*/
Void * MEMUTILS_getPhysicalAddr(Void * addr);

#endif /* MPEG4SPENC_RMAN_CONFIG_H__ */

