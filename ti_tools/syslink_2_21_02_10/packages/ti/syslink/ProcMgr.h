/** 
 *  @file   ProcMgr.h
 *
 *  @brief      The Processor Manager on a master processor provides control
 *              functionality for a slave device.
 *
 *              The ProcMgr module provides the following services for the
 *              slave processor:
 *                - Slave processor boot-loading
 *                - Read from or write to slave processor memory
 *                - Slave processor power management
 *
 *              Internally, processor management is composed of:
 *                - Loader: There may be multiple implementations of the Loader
 *                        interface within a single Processor instance.
 *                        For example, COFF, ELF, dynamic loader, custom types
 *                        of loaders may be written and plugged in.
 *                - Power Manager: The Power Manager implementation can be a
 *                        separate module that is plugged into the Processor
 *                        module. This allows the Processor code to remain
 *                        generic, and the Power Manager may be written and
 *                        maintained either by a separate team, or by customer.
 *
 *                - Processor: The implementation of this interface provides all
 *                        other functionality for the slave processor, including
 *                        setup and initialization of the Processor module,
 *                        management of slave processor MMU (if available),
 *                        functions to write to and read from slave memory etc.
 *
 *              All processors in the system shall be identified by unique
 *              processor ID. The management of this processor ID is done by the
 *              MultiProc module.
 *
 *
 */
/* 
 *  ============================================================================
 *
 *  Copyright (c) 2008-2012, Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  
 *  *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  
 *  *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  
 *  *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *  
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  Contact information for paper mail:
 *  Texas Instruments
 *  Post Office Box 655303
 *  Dallas, Texas 75265
 *  Contact information: 
 *  http://www-k.ext.ti.com/sc/technical-support/product-information-centers.htm?
 *  DCMP=TIHomeTracking&HQS=Other+OT+home_d_contact
 *  ============================================================================
 *  
 */


#ifndef ProcMgr_H_0xf2ba
#define ProcMgr_H_0xf2ba

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @brief  Other ProcMgr clients have still setup the ProcMgr module
 */
#define ProcMgr_S_SETUP            5

/*!
 *  @brief  Other ProcMgr handles are still open in this process
 */
#define ProcMgr_S_OPENHANDLE       4

/*!
 *  @brief  The ProcMgr instance has already been created/opened in this process
 */
#define ProcMgr_S_ALREADYEXISTS    3
/*!
 *  @brief  The resource is still in use
 */
#define ProcMgr_S_BUSY             2

/*!
 *  @brief  The module has been already setup
 */
#define ProcMgr_S_ALREADYSETUP     1

/*!
 *  @brief  Operation is successful
 */
#define ProcMgr_S_SUCCESS          0

/*!
 *  @brief  Generic failure
 */
#define ProcMgr_E_FAIL             -1

/*!
 *  @brief  Argument passed to function is invalid
 */
#define ProcMgr_E_INVALIDARG       -2

/*!
 *  @brief  Operation resulted in memory failure
 */
#define ProcMgr_E_MEMORY           -3

/*!
 *  @brief  The specified entity already exists
 */
#define ProcMgr_E_ALREADYEXISTS    -4

/*!
 *  @brief  Unable to find the specified entity
 */
#define ProcMgr_E_NOTFOUND         -5

/*!
 *  @brief  Operation timed out
 */
#define ProcMgr_E_TIMEOUT          -6

/*!
 *  @brief  Module is not initialized
 */
#define ProcMgr_E_INVALIDSTATE     -7

/*!
 *  @brief  A failure occurred in an OS-specific call
 */
#define ProcMgr_E_OSFAILURE        -8

/*!
 *  @brief  Specified resource is not available
 */
#define ProcMgr_E_RESOURCE         -9

/*!
 *  @brief  Operation was interrupted. Please restart the operation
 */
#define ProcMgr_E_RESTART          -10

/*!
 *  @brief  Invalid object handle specified
 */
#define ProcMgr_E_HANDLE           -11

/*!
 *  @brief  The operation is not permitted in this process.
 */
#define ProcMgr_E_ACCESSDENIED     -12

/*!
 *  @brief  An address translation error occurred.
 */
#define ProcMgr_E_TRANSLATE        -13

/*!
 *  @brief  Could not find the specified symbol in the loaded file
 */
#define ProcMgr_E_SYMBOLNOTFOUND   -14

/*!
 *  @brief  Failed to map/unmap an address range
 */
#define ProcMgr_E_MAP              -15


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Checks if the Processor ID is valid
 */
#define IS_VALID_PROCID(id)   (id < MultiProc_MAXPROCESSORS)

/*!
 *  @brief  Defines ProcMgr object handle
 */
typedef struct ProcMgr_Object * ProcMgr_Handle;


/*!
 *  @brief  Enumerations to indicate Processor states.
 */
typedef enum {
    ProcMgr_State_Unknown     = 0u,
    /*!< Unknown Processor state (e.g. at startup or error). */
    ProcMgr_State_Powered     = 1u,
    /*!< Indicates the Processor is powered up. */
    ProcMgr_State_Reset       = 2u,
    /*!< Indicates the Processor is reset. */
    ProcMgr_State_Loaded      = 3u,
    /*!< Indicates the Processor is loaded. */
    ProcMgr_State_Running     = 4u,
    /*!< Indicates the Processor is running. */
    ProcMgr_State_Unavailable = 5u,
    /*!< Indicates the Processor is unavailable to the physical transport. */
    ProcMgr_State_EndValue    = 6u
    /*!< End delimiter indicating start of invalid values for this enum */
} ProcMgr_State ;

/*!
 *  @brief  Enumerations to indicate different types of slave boot modes.
 */
typedef enum {
    ProcMgr_BootMode_Boot           = 0u,
    /*!< ProcMgr is responsible for loading the slave and its reset control. */
    ProcMgr_BootMode_NoLoad_Pwr     = 1u,
    /*!< Unsupported */
    ProcMgr_BootMode_NoLoad_NoPwr   = 2u,
    /*!< Unsupported */
    ProcMgr_BootMode_NoBoot         = 3u,
    /*!< ProcMgr is not responsible for loading or reset control of the slave.
         The slave either self-boots, or this is done by some entity outside of
         the ProcMgr module. It only powers the 
         resources that are owned by SysLink/IPC (e.g. spinlocks, mailboxes).
         For Internal Use. */
    ProcMgr_BootMode_EndValue       = 4u
    /*!< End delimiter indicating start of invalid values for this enum */
} ProcMgr_BootMode ;

/*!
 *  @brief  Enumerations to indicate address types used for translation
 */
typedef enum {
    ProcMgr_AddrType_MasterKnlVirt = 0u,
    /*!< Kernel Virtual address on master processor */
    ProcMgr_AddrType_MasterUsrVirt = 1u,
    /*!< User Virtual address on master processor */
    ProcMgr_AddrType_MasterPhys    = 2u,
    /*!< Physical address on master processor */
    ProcMgr_AddrType_SlaveVirt     = 3u,
    /*!< Virtual address on slave processor */
    ProcMgr_AddrType_SlavePhys     = 4u,
    /*!< Physical address on slave processor */
    ProcMgr_AddrType_EndValue      = 5u
    /*!< End delimiter indicating start of invalid values for this enum */
} ProcMgr_AddrType;

/**
 *  @brief      Address Map Mask type
 */
typedef UInt32 ProcMgr_MapMask;

/**
 *  @brief      Kernel virtual address on master processor
 */
#define ProcMgr_MASTERKNLVIRT   (ProcMgr_MapMask)(1 << 0)

/**
 *  @brief      User virtual address on master processor
 */
#define ProcMgr_MASTERUSRVIRT   (ProcMgr_MapMask)(1 << 1)

/**
 *  @brief      Virtual address on slave processor
 */
#define ProcMgr_SLAVEVIRT       (ProcMgr_MapMask)(1 << 2)

/*!
 *  @brief  Configuration parameters specific to the slave ProcMgr instance.
 */
typedef struct ProcMgr_AttachParams_tag {
    ProcMgr_BootMode bootMode;
    /*!< Boot mode for the slave processor. */
    Ptr              bootParams;
    /*!< Params for dependent processors like IVHD for netra. */
} ProcMgr_AttachParams ;

/*!
 *  @brief  Configuration parameters to be provided while starting the slave
 *          processor.
 */
typedef struct ProcMgr_StartParams_tag {
    UInt32 reserved;
    /*!< Reserved for future params. */
} ProcMgr_StartParams ;


/*!
 *  @brief  This structure defines information about memory regions mapped by
 *          the ProcMgr module.
 */
typedef struct ProcMgr_AddrInfo_tag {
    UInt32          addr [ProcMgr_AddrType_EndValue];
    /*!< Addresses for each type of address space */
    UInt32          size;
    /*!< Size of the memory region in bytes */
    Bool            isCached;
    /*!< Region cached? */
    Bool            isMapped;
    /*!< tells whether the entry is mapped */
    ProcMgr_MapMask mapMask;
    /*!< map type needed for default entries in translation table */
    UInt16          refCount;
    /*!< To ensure entry is added/removed only in appropriate map/unmap
     */
} ProcMgr_AddrInfo;

/*!
 *  @brief Structure containing information of mapped memory regions
 */
typedef struct ProcMgr_MappedMemEntry {
    ProcMgr_AddrInfo info;
    /*!< Address information */
    ProcMgr_AddrType srcAddrType;
    /*!< Source address type used for mapping */
    ProcMgr_MapMask  mapMask;
    /*!< Mapping type */
    Bool             inUse;
    /*!< Entry index is in use? */
} ProcMgr_MappedMemEntry;

/*!
 *  @brief  Characteristics of the slave processor
 */
typedef struct ProcMgr_ProcInfo_tag {
    ProcMgr_BootMode       bootMode;
    /*!< Boot mode of the processor */
    UInt16                 maxMemoryRegions;
    /*!< Value of ProcMgr_Object.maxMemoryRegions */
    UInt16                 numMemEntries;
    /*!< Number of valid memory entries */
    ProcMgr_MappedMemEntry memEntries [0];
    /*!< Configuration of memory regions */
} ProcMgr_ProcInfo;


/*!
 *  @brief  Characteristics of sections in executable
 */
typedef struct ProcMgr_SectionInfo_tag {
   UInt32    physicalAddress;
   /*!< Requested/returned section's physicalAddress */
   UInt32    virtualAddress;
   /*!< Requested/returned section's vitualAddress */
   UInt16    sectId;
   /*!< Requested/returned section id */
   UInt32    size;
   /*!< Section size */
} ProcMgr_SectionInfo;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/*!
 *  @brief      Function to open a handle to an existing ProcMgr object handling
 *              the procId.
 *
 *              This function returns a handle to an existing ProcMgr instance
 *              created for this procId. It enables other entities to access
 *              and use this ProcMgr instance.
 *
 *  @param[out] handlePtr   Return Parameter: Handle to the ProcMgr instance
 *  @param[in]  procId      Processor ID represented by this ProcMgr instance
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_S_ALREADYEXISTS Object is already created/opened in this
 *                                      process
 *  @retval     ProcMgr_E_MAP           Failed to map address range to host OS
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *  @retval     ProcMgr_E_MEMORY        Memory allocation failed
 *
 *  @sa         ProcMgr_close()
 */
Int ProcMgr_open (ProcMgr_Handle * handlePtr, UInt16 procId);

/*!
 *  @brief      Function to close this handle to the ProcMgr instance.
 *
 *              This function closes the handle to the ProcMgr instance
 *              obtained through ProcMgr_open call made earlier. The handle in
 *              the passed pointer is reset on success.
 *
 *  @param[in]  handlePtr    Pointer to the ProcMgr handle
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_ACCESSDENIED  All open handles to this ProcMgr object
 *                                      are already closed
 *  @retval     ProcMgr_S_OPENHANDLE    Other threads in this process have
 *                                      already opened handles to this ProcMgr
 *                                      instance.
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_open()
 */
Int ProcMgr_close (ProcMgr_Handle * handlePtr);

/*!
 *  @brief      Function to initialize the parameters for the ProcMgr attach
 *              function.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to #ProcMgr_attach filled in by the
 *              ProcMgr module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      handle   Handle to the ProcMgr object. If specified as NULL,
 *                       the default global configuration values are returned.
 *  @param      params   Pointer to the ProcMgr attach params structure in
 *                       which the default params is to be returned.
 *
 *  @sa         ProcMgr_attach()
 */
Void ProcMgr_getAttachParams (ProcMgr_Handle         handle,
                              ProcMgr_AttachParams * params);

/*!
 *  @brief      Function to attach the client to the specified slave and also
 *              initialize the slave (if required).
 *
 *  @remarks    This function attaches to an instance of the ProcMgr module and
 *              performs any hardware initialization required to power up the
 *              slave device. This function also performs the required state
 *              transitions for this ProcMgr instance to ensure that the local
 *              object representing the slave device correctly indicates the
 *              state of the slave device. Depending on the slave boot mode
 *              being used, the slave may be powered up, in reset, or even
 *              running state.
 *              In typical scenarios, after calling ProcMgr_attach, the
 *              application should call ProcMgr_detach during shutdown to
 *              detach the client from the slave and perform any hardware
 *              finalization needed. However, there may be cases
 *              (e.g. in a slave loader application) when it is desirable to
 *              leave the hardware initialized and ready upon exiting the
 *              application. In such cases it is fine to defer the
 *              ProcMgr_detach call to a later point in time.
 *
 *  @remarks    Configuration parameters need to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then ProcMgr_getAttachParams() can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call ProcMgr_attach() with @c NULL
 *              parameters.
 *              The default parameters would get automatically used.
 *
 *  @param[in]  handle   Handle to the ProcMgr object.
 *  @param[in]  params   Optional ProcMgr attach parameters. If provided as
 *                       NULL, default configuration is used.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_MAP           Failed to map address range to host OS
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_detach()
 *  @sa         ProcMgr_getAttachParams()
 */
Int ProcMgr_attach (ProcMgr_Handle handle, ProcMgr_AttachParams * params);

/*!
 *  @brief      Function to detach the client from the specified slave and also
 *              finalze the slave (if required).
 *
 *  @remarks    This function detaches from an instance of the ProcMgr module
 *              and performs any hardware finalization required to power down
 *              the slave device. This function also performs the required state
 *              transitions for this ProcMgr instance to ensure that the local
 *              object representing the slave device correctly indicates the
 *              state of the slave device. Depending on the slave boot mode
 *              being used, the slave may be powered down, in reset, or left in
 *              its original state.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_attach
 */
Int ProcMgr_detach (ProcMgr_Handle handle);

/*!
 *  @brief      Function to load the specified slave executable on the slave
 *              Processor.
 *
 *  @remarks    This function allows usage of different types of loaders. The
 *              loader specified when creating this instance of the ProcMgr
 *              is used for loading the slave executable. Depending on the type
 *              of loader, the imagePath parameter may point to the path of the
 *              file in the host file system, or it may be NULL. Some loaders
 *              may require specific parameters to be passed.
 *
 * @remarks     This function returns a fileId, which can be used for further
 *              function calls that reference a specific file that has been
 *              loaded on the slave processor.
 *
 * @remarks     Some loaders may not support all features.  For example, the
 *              ELF loader may not support passing argc/argv to the slave.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *  @param[in]  imagePath  Full file path
 *  @param[in]  argc       Number of arguments
 *  @param[in]  argv       String array of arguments
 *  @param[in]  params     Loader specific parameters
 *  @param[in,out]  fileId     Return parameter: ID of the loaded file
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_unload()
 */
Int ProcMgr_load (ProcMgr_Handle handle,
                  String         imagePath,
                  UInt32         argc,
                  String *       argv,
                  Ptr            params,
                  UInt32 *       fileId);

/*!
 *  @brief      Function to unload the previously loaded file on the slave
 *              processor.
 *
 *  @remarks    This function unloads the file that was previously loaded on the
 *              slave processor through the ProcMgr_load() API. It frees up any
 *              resources that were allocated during ProcMgr_load for this file.
 *
 *  @remarks    The fileId received from the load function must be passed to
 *              this function.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *  @param[in]  fileId     ID of the loaded file to be unloaded
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_load()
 */
Int ProcMgr_unload (ProcMgr_Handle handle, UInt32 fileId);

/*!
 *  @brief      Function to initialize the parameters for the ProcMgr start
 *              function.
 *
 *  @remarks    This function can be called by the application to get their
 *              configuration parameter to ProcMgr_start() filled in by the
 *              ProcMgr module with the default parameters.
 *
 *  @remarks    If the user does not wish to make any change in the default
 *              parameters, this API is not required to be called.
 *
 *  @param[in]  handle   Handle to the ProcMgr object. If @c NULL,
 *                       the default global configuration values are returned.
 *  @param      params   Pointer to the ProcMgr start params structure in
 *                       which the default params is to be returned.
 *
 *  @sa         ProcMgr_start
 */
Void ProcMgr_getStartParams (ProcMgr_Handle        handle,
                             ProcMgr_StartParams * params);

/*!
 *  @brief      Start the slave processor running
 *
 *  @remarks    Function to start execution of the loaded code on the slave
 *              from the entry point specified in the slave executable loaded
 *              earlier by call to ProcMgr_load().
 *
 *  @post       After successful completion of this function, the ProcMgr
 *              instance is expected to be in the #ProcMgr_State_Running state.
 *
 *  @param[in]  handle   Handle to the ProcMgr object
 *  @param[in]  params   Optional ProcMgr start parameters. If @c NULL,
 *                       default parameters are used.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_SYMBOLNOTFOUND Entry ponit symbol not found in loaded
 *                                       file
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_stop()
 */
Int ProcMgr_start (ProcMgr_Handle handle, ProcMgr_StartParams * params);

/*!
 *  @brief      Stop the slave processor
 *
 *  @remarks    Depending on the boot mode, after successful completion of this
 *              function, the ProcMgr instance may be in the
 *              #ProcMgr_State_Reset state.
 *
 *  @param      handle   Handle to the ProcMgr object
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_start()
 */
Int ProcMgr_stop (ProcMgr_Handle handle);

/*!
 *  @brief      Function to get the current state of the slave Processor.
 *
 *  @remarks    This function gets the state of the slave processor as
 *              maintained on the master Processor state machine. It does not
 *              go to the slave processor to get its actual state at the time
 *              when this API is called.
 *
 *  @param[in]  handle   Handle to the ProcMgr object
 *
 *  @retval     Processor-state       Operation successful
 */
ProcMgr_State ProcMgr_getState (ProcMgr_Handle handle);

/*!
 *  @brief      Function to read from the slave processor's memory.
 *
 *  @remarks    This function reads from the specified address in the
 *              processor's address space and copies the required number of
 *              bytes into the specified buffer.
 *
 *  @remarks    It returns the number of bytes actually read in the numBytes
 *              parameter.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *  @param      procAddr   Address in space processor's address space of the
 *                         memory region to read from.
 *  @param[in,out]  numBytes   As an IN-parameter, it takes in the
 *                         number of bytes to be read. When the function
 *                         returns, this parameter contains the number of bytes
 *                         actually read.
 *  @param[in,out]  buffer User-provided buffer in which the slave processor's
 *                         memory contents are to be copied.
 *
 *  @pre        @c handle is a valid (non-NULL) ProcMgr handle.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_TRANSLATE     Address is not mapped
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_write
 */
Int ProcMgr_read (ProcMgr_Handle handle,
                  UInt32         procAddr,
                  UInt32 *       numBytes,
                  Ptr            buffer);

/*!
 *  @brief      Function to write into the slave processor's memory.
 *
 *              This function writes into the specified address in the
 *              processor's address space and copies the required number of
 *              bytes from the specified buffer.
 *              It returns the number of bytes actually written in the numBytes
 *              parameter.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *  @param[in]  procAddr   Address in space processor's address space of the
 *                         memory region to write into.
 *  @param[in,out]  numBytes   As an IN-parameter, it takes in the
 *                         number of bytes to be written. When the function
 *                         returns, this parameter contains the number of bytes
 *                         actually written.
 *  @param[in]  buffer     User-provided buffer from which the data is to be
 *                         written into the slave processor's memory.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_TRANSLATE     Address is not mapped
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_read()
 */
Int ProcMgr_write (ProcMgr_Handle handle,
                   UInt32         procAddr,
                   UInt32 *       numBytes,
                   Ptr            buffer);

/*!
 *  @brief      Perform device-dependent operations
 *
 *  @remarks    This function performs control operations supported by the
 *              as exposed directly by the specific implementation of the
 *              Processor interface. These commands and their specific argument
 *              types are used with this function.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *  @param[in]  cmd        Device specific processor command
 *  @param[in]  arg        Arguments specific to the type of command.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 */
Int ProcMgr_control (ProcMgr_Handle handle, Int32 cmd, Ptr arg);

/*!
 *  @brief      Translate between two types of address spaces
 *
 *  @remarks    This function translates addresses between two types of address
 *              spaces. The destination and source address types are indicated
 *              through parameters specified in this function.
 *
 *  @param[in]  handle      Handle to the ProcMgr object
 *  @param[out] dstAddr     Return parameter: Pointer to receive the translated
 *                          address.
 *  @param[in]  dstAddrType Destination address type requested
 *  @param[in]  srcAddr     Source address in the source address space
 *  @param[in]  srcAddrType Source address type
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_TRANSLATE     Failed to translate address.
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_AddrType
 */
Int ProcMgr_translateAddr (ProcMgr_Handle   handle,
                           Ptr *            dstAddr,
                           ProcMgr_AddrType dstAddrType,
                           Ptr              srcAddr,
                           ProcMgr_AddrType srcAddrType);

/*!
 *  @brief      Function to retrieve the target address of a symbol from the
 *              specified file.
 *
 *  @param[in]  handle     Handle to the ProcMgr object
 *  @param[in]  fileId     ID of the file received from the load function
 *  @param[in]  symbolName Name of the symbol
 *  @param[out] symValue   Return parameter: Symbol address
 *
 *  @retval     ProcMgr_S_SUCCESS        Operation successful
 *  @retval     ProcMgr_E_INVALIDARG     Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE   Module was not initialized
 *  @retval     ProcMgr_E_HANDLE         Invalid NULL handle specified
 *  @retval     ProcMgr_E_SYMBOLNOTFOUND Symbol not found in loaded file
 *  @retval     ProcMgr_E_OSFAILURE      Failed in an OS-specific call
 */
Int ProcMgr_getSymbolAddress (ProcMgr_Handle handle,
                              UInt32         fileId,
                              String         symbolName,
                              UInt32 *       symValue);

/*!
 *  @brief      Function to map address to specified destination type(s).
 *
 *  @remarks    This function maps the provided address of specified srcAddrType
 *              to one or more destination address types, and returns the mapped
 *              addresses and size in the same addrInfo structure.
 *
 *  @param[in]  handle      Handle to the Processor object
 *  @param[in]  mapMask     Mask of destination types of mapping to be
 *                          performed. One or more types may be ORed together.
 *  @param[in,out]  addrInfo    Structure containing map info. When this API is
 *                          called, user must provide a valid address for the
 *                          address of srcAddrType. On successful completion of
 *                          this function, this same structure shall contain
 *                          valid addresses for destination address types given
 *                          in mapType.
 *  @param[in]  srcAddrType Source address type.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_MAP           Failed to map address range to host OS
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_unmap()
 */
Int ProcMgr_map (ProcMgr_Handle     handle,
                 ProcMgr_MapMask    mapMask,
                 ProcMgr_AddrInfo * addrInfo,
                 ProcMgr_AddrType   srcAddrType);

/*!
 *  @brief      Function to unmap address from slave address space.
 *
 *  @remarks    This function unmaps the provided address(es) of specified
 *              one or more destination address types. The srcAddrType indicates
 *              the source address with which the entry for the mapping is to be
 *              identified. It must be the same address that was specified as
 *              the src address when the corresponding mapping was performed.
 *
 *  @param[in]  handle      Handle to the Processor object
 *  @param[in]  mapMask     Mask of destination types of unmapping to be
 *                          performed. One or more types may be ORed together.
 *  @param[in,out]  addrInfo    Structure containing map info. When this API is
 *                          called, user must provide a valid address for the
 *                          address of srcAddrType, as well as each of the
 *                          destination types given in the mapType mask. On
 *                          successful completion of this function, the
 *                          specified destination mappings are unmapped.
 *  @param[in]  srcAddrType Source address type.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_NOTFOUND      Info provided does not match with any
 *                                      mapped entry
 *  @retval     ProcMgr_E_MAP           Failed to unmap address range from host
 *                                      OS
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_map()
 */
Int ProcMgr_unmap (ProcMgr_Handle     handle,
                   ProcMgr_MapMask    mapMask,
                   ProcMgr_AddrInfo * addrInfo,
                   ProcMgr_AddrType   srcAddrType);

/**
 *  @brief      Get the maximum number of memory entries
 *
 *  @param[in]  handle      Handle to the ProcMgr object
 *
 *  @remarks    The kernel objects contain tables of size
 *              ProcMgr_MAX_MEMORY_REGIONS, this function retrieves that value
 *              for dynamically-sized arrays.
 *
 *  @retval     Number of max memory entries
 */
UInt32 ProcMgr_getMaxMemoryRegions(ProcMgr_Handle handle);


/**
 *  @brief      Retrieve information about the slave processor
 *
 *  @param[in]  handle      Handle to the ProcMgr object
 *  @param[out] procInfo    Pointer to the ProcInfo object to be populated.
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 */
Int ProcMgr_getProcInfo (ProcMgr_Handle     handle,
                         ProcMgr_ProcInfo * procInfo);


/**
 *  @brief      Function that returns section information given the name of
 *              section and number of bytes to read
 *
 *  @param[in]  handle      Handle to the ProcMgr object
 *  @param[in]  fileId      ID of the file received from the load function
 *  @param[in]  sectionName Name of section to be retrieved
 *  @param[out] sectionInfo Return parameter: Section information
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 */
Int ProcMgr_getSectionInfo (ProcMgr_Handle        handle,
                            UInt32                fileId,
                            String                sectionName,
                            ProcMgr_SectionInfo * sectionInfo);

/**
 *  @brief      Function that returns section data in a buffer given section id
 *              and size to be read
 *
 *  @param[in]  handle      Handle to the ProcMgr object
 *  @param[in]  fileId      ID of the file received from the load function
 *  @param[in]  sectionInfo Structure filled by ProcMgr_getSectionInfo()
 *  @param[out] buffer      Return parameter
 *
 *  @retval     ProcMgr_S_SUCCESS       Operation successful
 *  @retval     ProcMgr_E_INVALIDARG    Invalid parameter specified
 *  @retval     ProcMgr_E_INVALIDSTATE  Module was not initialized
 *  @retval     ProcMgr_E_HANDLE        Invalid NULL handle specified
 *  @retval     ProcMgr_E_OSFAILURE     Failed in an OS-specific call
 *
 *  @sa         ProcMgr_getSectionInfo()
 */
Int ProcMgr_getSectionData (ProcMgr_Handle        handle,
                            UInt32                fileId,
                            ProcMgr_SectionInfo * sectionInfo,
                            Ptr                   buffer);


/**
 *  @brief      Function that returns section data in a buffer given section id
 *              and size to be read
 *
 *  @param[in]  handle      Handle to the ProcMgr object
 *
 *  @retval     fileId                  Operation successful
 *
 *  @sa         ProcMgr_load()
 */
UInt32 ProcMgr_getLoadedFileId (ProcMgr_Handle handle);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* ProcMgr_H_0xf2ba */
