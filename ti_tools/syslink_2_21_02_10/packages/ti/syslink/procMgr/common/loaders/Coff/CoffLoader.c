/*
 *  @file   CoffLoader.c
 *
 *  @brief      File-based COFF loader implementation
 *
 *              This COFF loader opens, parses and loads the COFF file that is
 *              present in the master file system, onto the slave processor.
 *
 *              @Example
 *              @code
 *              CoffLoader_Handle     loaderHandle = NULL;
 *              CoffLoader_Config     loaderConfig;
 *              CoffLoader_Params     loaderParams;
 *
 *              CoffLoader_getConfig (&loaderConfig);
 *              status = CoffLoader_setup (&loaderConfig);
 *
 *              CoffLoader_Params_init (NULL, &loaderParams);
 *              loaderHandle = CoffLoader_create (procId, &loaderParams);
 *
 *              CoffLoader_delete (&loaderHandle);
 *              CoffLoader_destroy ();
 *              @endcode
 *
 *
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



#if defined(SYSLINK_BUILD_HLOS)
#include <ti/syslink/Std.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#endif

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/IGateProvider.h>
#include <ti/sysbios/gates/GateMutex.h>
#endif

#if defined(__KERNEL__)
#include <linux/string.h>
#else
#include <string.h>
#endif

/* OSAL & Utils headers */
#include <ti/syslink/inc/knl/OsalKfile.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>


/* Module level headers */
#include <ti/syslink/inc/knl/LoaderDefs.h>
#include <ti/syslink/inc/knl/Loader.h>
#include <ti/syslink/inc/CoffLoader.h>
#include <ti/syslink/inc/knl/_CoffLoader.h>
#include <ti/syslink/inc/knl/Coff.h>
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/_ProcMgr.h>
#include <ti/syslink/inc/knl/Processor.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/ipc/MultiProc.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define LOADER_MAKE_MAGICSTAMP(x) ((LOADER_MODULEID << 12u) | (x))

/*!
 *  @brief  CoffLoader Module state object
 */
typedef struct CoffLoader_ModuleObject_tag {
    UInt32                  refCount;
    /*!< Reference count */
    UInt32                  configSize;
    /*!< Size of configuration structure */
    CoffLoader_Config       cfg;
    /*!< CoffLoader configuration structure */
    CoffLoader_Config       defCfg;
    /*!< Default module configuration */
    Bool                    isSetup;
    /*!< Indicates whether the CoffLoader module is setup. */
    CoffLoader_Handle       loaderHandles [MultiProc_MAXPROCESSORS];
    /*!< Loader handle array. */
    IGateProvider_Handle    gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    Ptr                     osalDrvHandle;
    /*!< OsalDriver handle for CoffLoader */
} CoffLoader_ModuleObject;

/*!
 *  @brief  Internal CoffLoader instance object.
 */
typedef struct _CoffLoader_Object_tag {
    CoffLoader_Params       params;
    /*!< Instance parameters (configuration values) */
    String                  fileName;
    /*!< Name of the file currently loaded. */
    Processor_Handle        procHandle;
    /*!< Handle to the Processor instance. */
    ProcMgr_Handle          pmHandle;
    /*!< Handle to the ProcMgr instance. */
    Processor_ProcArch      procArch;
    /*!< Processor architecture */
    Ptr                     fileDesc;
    /*!< File object for the slave base image file. */
} _CoffLoader_Object;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
/*!
 *  @var    CoffLoader_state
 *
 *  @brief  CoffLoader state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
CoffLoader_ModuleObject CoffLoader_state =
{
    .isSetup = FALSE,
    .configSize = sizeof (CoffLoader_Config),
    .gateHandle = NULL,
    .osalDrvHandle = NULL
};


/* =============================================================================
 * Internal implementations of client Provided Functions
 * =============================================================================
 */

/* -----------------------------------------------------------------------------
 *  File I/O Interface
 *
 *  The client side of the loader must provide basic file I/O capabilities so
 *  that the core loader has random access into any file that it is asked to
 *  load.
 * -----------------------------------------------------------------------------
 */

/*!
 *  @brief  Function to seek to a position in the file based on the values for
 *          offset and pos.
 *
 *  @param  clientHandle    Handle to the client loader object.
 *  @param  fileDesc        File descriptor
 *  @param  offset          Offset (in bytes) from specified positioning command
 *  @param  pos             Positioning command for the file
 *
 *  @sa     CoffLoaderFile_tell
 */
inline
Int
CoffLoaderFile_seek (Ptr            clientHandle,
                     Ptr            fileDesc,
                     Int32          offset,
                     LoaderFile_Pos pos)
{
    GT_4trace (curTrace, GT_ENTER, "CoffLoaderFile_seek",
               clientHandle, fileDesc, offset, pos);

    (Void) clientHandle; /* Not used. */
    GT_assert (curTrace, (fileDesc != NULL));
    GT_assert (curTrace, (pos < LoaderFile_Pos_EndValue));

    GT_0trace (curTrace, GT_LEAVE, "CoffLoaderFile_seek");

    /*! @retval LOADER_SUCCESS Operation successfully completed. */
    return (OsalKfile_seek (fileDesc, offset, (OsalKfile_Pos)pos));
}

/*!
 *  @brief  Function to return the current file position in the file.
 *
 *          Returns the position in the file.
 *
 *  @param  clientHandle    Handle to the client loader object.
 *  @param  fileDesc        File descriptor
 *
 *  @sa     CoffLoaderMem_seek
 */
inline
UInt32
CoffLoaderFile_tell (Ptr clientHandle, Ptr fileDesc)
{
    GT_2trace (curTrace, GT_ENTER, "CoffLoaderFile_tell",
               clientHandle, fileDesc);

    (Void) clientHandle; /* Not used. */
    GT_assert (curTrace, (fileDesc != NULL));

    GT_0trace (curTrace, GT_LEAVE, "CoffLoaderFile_tell");

    /*! @retval Value: Current position in the file. */
    return (OsalKfile_tell (fileDesc));
}

/*!
 *  @brief  Function to read contents of file into specified buffer.
 *
 *          Returns number of bytes actually read.
 *
 *  @param  clientHandle    Handle to the client loader object.
 *  @param  fileDesc        File descriptor
 *  @param  buffer          Buffer to read into
 *  @param  size            Size of each record to be read
 *  @param  count           Number of records to be read
 *
 *  @sa     CoffLoaderMem_seek
 */
inline UInt32 CoffLoaderFile_read(Ptr clientHandle, Ptr fileDesc, Char *buffer,
        UInt32 size, UInt32 count)
{
    UInt32 bytesRead;
    Int status;
    UInt32 retval;

    GT_5trace (curTrace, GT_ENTER, "CoffLoaderFile_read",
               clientHandle, fileDesc, buffer, size, count);

    (Void) clientHandle; /* Not used. */
    GT_assert (curTrace, (fileDesc != NULL));
    GT_assert (curTrace, (buffer != NULL));
    GT_assert (curTrace, (size != 0));
    GT_assert (curTrace, (count != 0));

    status = OsalKfile_read(fileDesc, buffer, size, count, &bytesRead);

    retval = (status == OSALKFILE_SUCCESS) ? bytesRead : 0;

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderFile_read: 0x%x", retval);

    return (retval);
}


/* -----------------------------------------------------------------------------
 *  Host Memory management Interface
 *
 *  Allocate and free host memory as needed for the loader's internal data
 *  structures. If the loader resides on the target architecture, then this
 *  memory is allocated from a target memory heap that must be managed
 *  separately from memory that is allocated for a loaded file.
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Function to allocate specified bytes of memory.
 *          Returns a pointer to the memory buffer.
 *
 *  @param  clientHandle    Handle to the client loader object.
 *  @param  size            Size (in bytes) of buffer to be allocated.
 *
 *  @sa     CoffLoaderMem_free
 */
inline
Ptr
CoffLoaderMem_alloc (Ptr clientHandle, UInt32 size)
{
    GT_2trace (curTrace, GT_ENTER, "CoffLoaderMem_alloc", clientHandle, size);

    (Void) clientHandle; /* Not used. */
    GT_assert (curTrace, (size != 0));

    GT_0trace (curTrace, GT_LEAVE, "CoffLoaderMem_alloc");

    /*! @retval Pointer: Pointer to allocated buffer. */
    return (Memory_alloc (NULL, size, 0, NULL));
}


/*!
 *  @brief  Function pointer type for the function to free the specified buffer.
 */
/*!
 *  @brief  Function to free specified buffer.
 *
 *  @param  clientHandle       Handle to the client loader object.
 *  @param  buffer             Pointer to buffer to be freed.
 *  @param  size               Size (in bytes) of buffer to be freed.
 *
 *  @sa     CoffLoaderMem_alloc
 */
inline
Void
CoffLoaderMem_free (Ptr clientHandle, Ptr buffer, UInt32 size)
{
    GT_3trace (curTrace, GT_ENTER, "CoffLoaderMem_free",
               clientHandle, buffer, size);

    (Void) clientHandle; /* Not used. */
    GT_assert (curTrace, (buffer != NULL));
    GT_assert (curTrace, (size   != 0));

    Memory_free (NULL, buffer, size);

    GT_0trace (curTrace, GT_LEAVE, "CoffLoaderMem_free");
}


/* -----------------------------------------------------------------------------
 *  Target Memory Allocator Interface
 *
 *  The client side of the loader must create and maintain an infrastructure to
 *  manage target memory. The client must keep track of what target memory is
 *  associated with each object segment, allocating target memory for newly
 *  loaded objects and release target memory that is associated with objects
 *  that are being unloaded from the target architecture.
 *
 *  These functions are used by the core loader to interface into the client
 *  side's target memory allocator infrastructure.
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Function to allocate specified bytes of target memory.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  memReq       Memory request for this allocation.
 *
 *  @sa     CoffLoaderTrgMem_free
 */
inline
Int
CoffLoaderTrgMem_alloc (Ptr                  clientHandle,
                        Loader_MemRequest *  memReq)
{
    Int status = LOADER_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "CoffLoaderTrgMem_alloc",
               clientHandle, memReq);

    (Void) clientHandle; /* Not used. */

    /* Nothing to be done here. Target memory addresses are fixed since this
     * loader only supports static executables.
     */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgMem_alloc", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;

}


/*!
 *  @brief  Function to to free the specified target memory buffer.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  memReq       Memory request for this free.
 *
 *  @sa     CoffLoaderTrgMem_alloc
 */
inline
Int
CoffLoaderTrgMem_free (Ptr                 clientHandle,
                       Loader_MemRequest * memReq)
{
    Int status = LOADER_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "CoffLoaderTrgMem_free",
               clientHandle, memReq);

    (Void) clientHandle; /* Not used. */

    /* Nothing to be done here. Target memory addresses are fixed since this
     * loader only supports static executables.
     */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgMem_free", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;

}

/* -----------------------------------------------------------------------------
 *  Target Memory Access / Write Services
 *
 *  To complete the loading of an object segment, the segment may need to be
 *  relocated before it is actually written to target memory in the space that
 *  was allocated for it.
 *
 *  The client side of the loader provides the functions in this section to help
 *  complete the process of loading an object segment.
 *
 *  These functions help to make the core loader truly independent of
 *  whether it is running on the host or target architecture and how the
 *  client provides for reading/writing from/to target memory.
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Function to copy data into the provided segment and making it
 *          available to generic COFF parser.
 *          Memory for buffer is allocated in this function.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  memReq       Memory request for this copy
 *
 *  @sa     CoffLoaderTrgWrite_write
 */
inline
Int
CoffLoaderTrgWrite_copy (Ptr clientHandle, Loader_MemRequest * memReq)
{
    Int                  status     = LOADER_SUCCESS;
    CoffLoader_Object *  coffLoader = (CoffLoader_Object *) clientHandle;
    Char *               sectData;
    UInt32               numBytes;
    _CoffLoader_Object * coffLoaderObject;
    ProcMgr_AddrInfo     aInfo;
    UInt32               dstAddr;

    GT_2trace (curTrace, GT_ENTER, "CoffLoaderTrgWrite_copy",
               clientHandle, memReq);

    GT_assert (curTrace, (clientHandle != NULL));
    GT_assert (curTrace, (memReq       != NULL));

    coffLoaderObject = (_CoffLoader_Object *) coffLoader->coffLoaderObject;
    GT_assert (curTrace, (coffLoaderObject != NULL));

    numBytes = memReq->segment->objSize;
    /* numBytes may be 0 if a section is of size 0. */
    if (numBytes != 0) {
        /* TBD: Slave Virt addr is same as slave phys */
        status = Processor_translateAddr (coffLoaderObject->procHandle,
                                          &dstAddr,
                                          memReq->segment->trgAddress);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "CoffLoaderTrgWrite_copy",
                                 status,
                                 "Processor_translateAddr failed!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            aInfo.addr [ProcMgr_AddrType_MasterPhys] = dstAddr;
            aInfo.addr [ProcMgr_AddrType_SlaveVirt]  =
                                                    memReq->segment->trgAddress;
            aInfo.size = numBytes;
            aInfo.isCached = FALSE;
            /* Lock is held by ProcMgr_load API thus it is safe to call this API
             */
            status = _ProcMgr_map (coffLoaderObject->pmHandle,
                    (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT),
                    &aInfo, ProcMgr_AddrType_MasterPhys);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "CoffLoaderTrgWrite_copy",
                                     status,
                                     "ProcMgr_map failed!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                sectData = (Ptr) aInfo.addr [ProcMgr_AddrType_MasterKnlVirt];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
       }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (status >= 0) {
            if (memReq->offset != 0) {
                /* Seek to the location of the section data. */
                status = CoffLoaderFile_seek (clientHandle,
                                              memReq->fileDesc,
                                              memReq->offset,
                                              LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "CoffLoaderTrgWrite_copy",
                                     status,
                                     "Failed to seek to location of section data!");
                }
                else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Read from file if the loader is file-based. */
                    numBytes = CoffLoaderFile_read (clientHandle,
                                                    memReq->fileDesc,
                                                    sectData,
                                                    memReq->segment->objSize,
                                                    sizeof (UInt8));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (numBytes == 0u) {
                        /*! @retval LOADER_E_FILE Failed to read from file */
                        status = LOADER_E_FILE;
                        GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "CoffLoaderTrgWrite_copy",
                                         status,
                                         "Failed to read section data from file!");
                    }
                    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Return section data address. */
                        memReq->hostAddress = sectData;
                        memReq->isLoaded = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else { /* TBD */
                /* Return section data address. */
                memReq->hostAddress = sectData;
                memReq->isLoaded = TRUE;
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgWrite_copy", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief  Function to write data to the target memory. For file based loader,
 *          this function also frees the section data host buffer after copying
 *          the data into target memory.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  memReq       Memory request for this write
 *
 *  @sa     CoffLoaderTrgWrite_copy, ProcMgr_write
 */
inline
Int
CoffLoaderTrgWrite_write (Ptr clientHandle, Loader_MemRequest * memReq)
{
    Int                  status        = LOADER_SUCCESS;
    CoffLoader_Object *  coffLoader    = (CoffLoader_Object *) clientHandle;
    UInt32               numBytes      = 0;
    _CoffLoader_Object * coffLoaderObject;
    ProcMgr_AddrInfo     aInfo;
    UInt32               dstAddr;

    GT_2trace (curTrace, GT_ENTER, "CoffLoaderTrgWrite_write",
               clientHandle, memReq);

    GT_assert (curTrace, (clientHandle != NULL));
    GT_assert (curTrace, (memReq       != NULL));

    coffLoaderObject = (_CoffLoader_Object *) coffLoader->coffLoaderObject;
    GT_assert (curTrace, (coffLoaderObject != NULL));

    /* Only load the section if it is not already loaded. */
    if (memReq->isLoaded == FALSE) {
        /* numBytes may be 0 if a section is of size 0. */
        numBytes = memReq->segment->objSize;
        if (numBytes != 0) {
            /* First try to translate the address, if it is
             * premapped
             */
            /* TBD: Slave Virt addr is same as slave phys */
            status = Processor_translateAddr (coffLoaderObject->procHandle,
                                              &dstAddr,
                                              memReq->segment->trgAddress);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "CoffLoaderTrgWrite_copy",
                                     status,
                                     "Processor_translateAddr failed!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                aInfo.addr [ProcMgr_AddrType_MasterPhys] = dstAddr;
                aInfo.addr [ProcMgr_AddrType_SlaveVirt]  =
                                                    memReq->segment->trgAddress;
                aInfo.size = numBytes;
                aInfo.isCached = FALSE;
                /* Lock is held by ProcMgr_load API thus it is safe to call this
                 * API
                 */
                status = _ProcMgr_map(coffLoaderObject->pmHandle,
                        (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT),
                        &aInfo, ProcMgr_AddrType_MasterPhys);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "CoffLoaderTrgWrite_copy",
                                         status,
                                         "ProcMgr_map failed!");
                }
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

            /* Write to target memory. */
            status = ProcMgr_write (coffLoaderObject->pmHandle,
                                    memReq->segment->trgAddress,
                                    &numBytes,
                                    memReq->hostAddress);

            /* Set memReq->hostAddress to the mapped address so that it can be
             * unmapped later.
             */
            memReq->hostAddress = (Ptr)
                                    aInfo.addr [ProcMgr_AddrType_MasterKnlVirt];

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "CoffLoaderTrgWrite_write",
                                     status,
                                     "Failed to write to target memory!");
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgWrite_write", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief  Function to start execution of the target from the given execAddr.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  execAddr     Target address from which to execute it.
 *
 *  @sa
 */
inline
Int
CoffLoaderTrgWrite_execute (Ptr clientHandle, UInt32 execAddr)
{
    Int status = LOADER_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "CoffLoaderTrgWrite_execute",
               clientHandle, execAddr);

    (Void) clientHandle; /* Not used. */

    /* Nothing to be done here. Execution shall happen on request from user. */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgWrite_execute", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief  Function to map a slave address into host address space.
 *
 *  @param      clientHandle Handle to the client loader object.
 *  @param      mapType     Type of mapping to be performed.
 *  @param      addrInfo    Structure containing map info.
 *  @param      srcAddrType Source address type.
 *
 *  @sa     CoffLoaderTrgWrite_unmap
 */
inline
Int
CoffLoaderTrgWrite_map (Ptr                clientHandle,
                        ProcMgr_MapMask    mapType,
                        ProcMgr_AddrInfo * aInfo,
                        ProcMgr_AddrType   srcAddrType)
{
    Int                  status        = LOADER_SUCCESS;
    CoffLoader_Object *  coffLoader    = (CoffLoader_Object *) clientHandle;
    _CoffLoader_Object * coffLoaderObject;

    GT_4trace (curTrace, GT_ENTER, "CoffLoaderTrgWrite_map",
               clientHandle, mapType, aInfo, srcAddrType);

    GT_assert (curTrace, (clientHandle != NULL));
    GT_assert (curTrace, (aInfo != NULL));
    GT_assert (curTrace, (srcAddrType < ProcMgr_AddrType_EndValue));

    coffLoaderObject = (_CoffLoader_Object *) coffLoader->coffLoaderObject;
    GT_assert (curTrace, (coffLoaderObject != NULL));

    /* Write to target memory. */
    status = _ProcMgr_map (coffLoaderObject->pmHandle,
                           mapType,
                           aInfo,
                           srcAddrType);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoaderTrgWrite_map",
                             status,
                             "ProcMgr_map failed!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgWrite_map", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief  Function to unmap a slave address from host address space.
 *
 *  @param      clientHandle Handle to the client loader object.
 *  @param      mapType     Type of mapping to be performed.
 *  @param      addrInfo    Structure containing map info.
 *  @param      srcAddrType Source address type.
 *
 *  @sa     CoffLoaderTrgWrite_unmap
 */
inline
Int
CoffLoaderTrgWrite_unmap (Ptr                clientHandle,
                          ProcMgr_MapMask    mapType,
                          ProcMgr_AddrInfo * aInfo,
                          ProcMgr_AddrType   srcAddrType)
{
    Int                  status        = LOADER_SUCCESS;
    CoffLoader_Object *  coffLoader    = (CoffLoader_Object *) clientHandle;
    _CoffLoader_Object * coffLoaderObject;

    GT_4trace (curTrace, GT_ENTER, "CoffLoaderTrgWrite_unmap",
               clientHandle, mapType, aInfo, srcAddrType);

    GT_assert (curTrace, (clientHandle != NULL));
    GT_assert (curTrace, (aInfo != NULL));
    GT_assert (curTrace, (srcAddrType < ProcMgr_AddrType_EndValue));

    coffLoaderObject = (_CoffLoader_Object *) coffLoader->coffLoaderObject;
    GT_assert (curTrace, (coffLoaderObject != NULL));

    /* Write to target memory. */
    status = _ProcMgr_unmap (coffLoaderObject->pmHandle,
                             mapType,
                             aInfo,
                             srcAddrType);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoaderTrgWrite_unmap",
                             status,
                             "ProcMgr_unmap failed!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgWrite_unmap", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief  Function to map a slave address into host address space.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  mapInfo      mapping info.
 *  @param  mapType      Type of mapping.
 *
 *  @sa     CoffLoaderTrgWrite_unmap
 */
inline
Int
CoffLoaderTrgWrite_translate (Ptr              clientHandle,
                              Ptr *            dstAddr,
                              ProcMgr_AddrType dstAddrType,
                              Ptr              srcAddr,
                              ProcMgr_AddrType srcAddrType)
{
    Int                  status        = LOADER_SUCCESS;
    CoffLoader_Object *  coffLoader    = (CoffLoader_Object *) clientHandle;
    _CoffLoader_Object * coffLoaderObject;

    GT_5trace (curTrace, GT_ENTER, "CoffLoaderTrgWrite_translate",
               clientHandle, dstAddr, dstAddrType, srcAddr, srcAddrType);

    GT_assert (curTrace, (clientHandle != NULL));

    coffLoaderObject = (_CoffLoader_Object *) coffLoader->coffLoaderObject;
    GT_assert (curTrace, (coffLoaderObject != NULL));

    /* Write to target memory. */
    status = ProcMgr_translateAddr (coffLoaderObject->pmHandle,
                                    dstAddr,
                                    dstAddrType,
                                    srcAddr,
                                    srcAddrType);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoaderTrgWrite_translate",
                             status,
                             "ProcMgr_translateAddr failed!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderTrgWrite_translate", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/* -----------------------------------------------------------------------------
 *  Loading and Unloading of Dependent Files
 *  NOTE: Only applicable for dynamic loader. For other loaders, these functions
 *        do not need to be implemented and can simply return LOADER_E_NOTIMPL.
 *
 *  The dynamic loader core loader must coordinate loading and unloading
 *  dependent object files with the client side of the dynamic loader.
 *  This allows the client to keep its bookkeeping information up to date
 *  with what is currently loaded on the target architecture.
 *
 *  For instance, the client may need to interact with a file system or
 *  registry.  The client may also need to update debug information in
 *  synch with the loading and unloading of shared objects.
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Function to find and open a dependent file identified by the dllName
 *          parameter, then, if necessary, initiate a load call to actually load
 *          the shared object onto the target. A successful load will return a
 *          file ID that the client can associate with the newly loaded file.
 *          NOTE: Only applicable for dynamic loader. For other loaders, these
 *                functions do not need to be implemented and can simply return
 *                LOADER_E_NOTIMPL.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  dllName      Path to the DLL to be loaded.
 *
 *  @sa     CoffLoaderDll_loadDependent
 */
inline
Int
CoffLoaderDll_loadDependent (Ptr clientHandle, String dllName)
{
    GT_2trace (curTrace, GT_ENTER, "CoffLoaderDll_loadDependent",
               clientHandle, dllName);

    (Void) clientHandle; /* Not used. */
    (Void) dllName; /* Not used. */

    /* Nothing to be done here. Function is not supported for static loader. */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderDll_loadDependent",
               LOADER_E_NOTIMPL);

    /*! @retval LOADER_E_NOTIMPL This function is not implemented for this
                                 loader. */
    return LOADER_E_NOTIMPL;
}


/*!
 *  @brief  Function to unload a dependent file identified by the fileId
 *          parameter. Then, client must initiate an unload call to actually
 *          unload the shared object to free the target memory occupied by the
 *          object file.
 *          The fileId received from the load function must be passed to this
 *          function.
 *          NOTE: Only applicable for dynamic loader. For other loaders, these
 *                functions do not need to be implemented and can simply return
 *                LOADER_E_NOTIMPL.
 *
 *  @param  clientHandle Handle to the client loader object.
 *  @param  fileId       ID of the dependent file to be unloaded.
 *
 *  @sa     CoffLoaderDll_loadDependent
 */
inline
Int
CoffLoaderDll_unloadDependent (Ptr clientHandle, UInt32 fileId)
{
    GT_2trace (curTrace, GT_ENTER, "CoffLoaderDll_unloadDependent",
               clientHandle, fileId);

    (Void) clientHandle; /* Not used. */
    (Void) fileId; /* Not used. */

    /* Nothing to be done here. Function is not supported for static loader. */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "CoffLoaderDll_unloadDependent",
               LOADER_E_NOTIMPL);

    /*! @retval LOADER_E_NOTIMPL This function is not implemented for this
                                 loader. */
    return LOADER_E_NOTIMPL;
}


/* =============================================================================
 * APIs directly called by applications
 * =============================================================================
 */
/*!
 *  @brief      Function to get the default configuration for the CoffLoader
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to CoffLoader_setup filled in by the
 *              CoffLoader module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      cfg        Pointer to the CoffLoader module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         CoffLoader_setup
 */
Void
CoffLoader_getConfig (CoffLoader_Config * cfg)
{
    GT_1trace (curTrace, GT_ENTER, "CoffLoader_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_getConfig",
                             LOADER_E_INVALIDARG,
                             "Argument of type (CoffLoader_Config *) passed "
                             "is null!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        memcpy (cfg,
                     &CoffLoader_state.defCfg,
                     sizeof (CoffLoader_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_0trace (curTrace, GT_LEAVE, "CoffLoader_getConfig");
}


/*!
 *  @brief      Function to setup the CoffLoader module.
 *
 *              This function sets up the CoffLoader module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then CoffLoader_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call CoffLoader_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional CoffLoader module configuration. If provided as
 *                    NULL, default configuration is used.
 *
 *  @sa         CoffLoader_destroy
 *              GateMutex_create
 */
Int
CoffLoader_setup (CoffLoader_Config * cfg)
{
    Int               status = LOADER_SUCCESS;
    CoffLoader_Config tmpCfg;
    IArg              key;
    Error_Block       eb;

    GT_1trace (curTrace, GT_ENTER, "CoffLoader_setup", cfg);
    Error_init(&eb);
    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    key = Gate_enterSystem();
    if(  (CoffLoader_state.refCount & LOADER_MAKE_MAGICSTAMP(0))
       != LOADER_MAKE_MAGICSTAMP(0))
    {
        CoffLoader_state.refCount = LOADER_MAKE_MAGICSTAMP(0);
	}
    Gate_leaveSystem(key);

    key = Gate_enterSystem();
    CoffLoader_state.refCount++;

    if ( CoffLoader_state.refCount != LOADER_MAKE_MAGICSTAMP(1u))
    {
	    Gate_leaveSystem(key);
        status = LOADER_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "CoffLoader Module already initialized!");
    }
    else {
        Gate_leaveSystem(key);
        if (cfg == NULL) {
            CoffLoader_getConfig (&tmpCfg);
            cfg = &tmpCfg;
        }

        /* Create a default gate handle for local module protection. */
        CoffLoader_state.gateHandle = (IGateProvider_Handle)
                               GateMutex_create ((GateMutex_Params*)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (CoffLoader_state.gateHandle == NULL) {
            /*! @retval LOADER_E_FAIL Failed to create GateMutex! */
            status = LOADER_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "CoffLoader_setup",
                                 status,
                                 "Failed to create GateMutex!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the user provided values into the state object. */
            memcpy (&CoffLoader_state.cfg,
                         cfg,
                         sizeof (CoffLoader_Config));

            /* Initialize the name to handles mapping array. */
            memset (&CoffLoader_state.loaderHandles,
                        0,
                        (sizeof (CoffLoader_Handle) * MultiProc_MAXPROCESSORS));

            CoffLoader_state.isSetup = TRUE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    }

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_setup", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to destroy the CoffLoader module.
 *
 *              Once this function is called, other CoffLoader module APIs, except
 *              for the CoffLoader_getConfig API cannot be called anymore.
 *
 *  @sa         CoffLoader_setup
 *              GateMutex_delete
 */
Int
CoffLoader_destroy (Void)
{
    Int    status = LOADER_SUCCESS;
    UInt16 i;
    IArg              key;

    GT_0trace (curTrace, GT_ENTER, "CoffLoader_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    key = Gate_enterSystem();

    if (CoffLoader_state.refCount < LOADER_MAKE_MAGICSTAMP(1))
    {
	    Gate_leaveSystem(key);
        /*! @retval LOADER_E_INVALIDSTATE Module was not initialized */
        status = LOADER_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
	    Gate_leaveSystem(key);
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem();
        CoffLoader_state.refCount--;
        if ( CoffLoader_state.refCount == LOADER_MAKE_MAGICSTAMP(0))
        {
	        Gate_leaveSystem(key);
            /* Check if any CoffLoader instances have not been deleted so far.
             * If not, delete them.
             */
            for (i = 0 ; i < MultiProc_MAXPROCESSORS ; i++) {
                GT_assert (curTrace, (CoffLoader_state.loaderHandles [i] == NULL));
                if (CoffLoader_state.loaderHandles [i] != NULL) {
                    CoffLoader_delete (&(CoffLoader_state.loaderHandles [i]));
                }
            }

            if (CoffLoader_state.gateHandle != NULL) {
                GateMutex_delete ((GateMutex_Handle *)
                                        &(CoffLoader_state.gateHandle));
            }

            CoffLoader_state.isSetup = FALSE;
        }
        else {
       	    Gate_leaveSystem(key);
	    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_destroy", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to initialize the parameters for this loader instance.
 *
 *  @param      handle  Handle to the loader instance. If provided as NULL, the
 *                      default parameters are returned, otherwise parameters
 *                      as configured for the loader instance are returned.
 *  @param      params  Configuration parameters.
 *
 *  @sa         CoffLoader_create
 */
Void
CoffLoader_Params_init (CoffLoader_Handle handle, CoffLoader_Params * params)
{
    CoffLoader_Object *  coffLoader    = (CoffLoader_Object *) handle;
    _CoffLoader_Object * coffLoaderObject;

    GT_2trace (curTrace, GT_ENTER, "CoffLoader_Params_init", handle, params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (params == NULL) {
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "CoffLoader_Params_init",
                          LOADER_E_INVALIDARG,
                          "Argument of type (CoffLoader_Params *) passed "
                          "is null!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        if (handle == NULL) {
            params->reserved = 0;
        }
        else {
            coffLoaderObject = (_CoffLoader_Object *)
                                        coffLoader->coffLoaderObject;
            GT_assert (curTrace, (coffLoaderObject != NULL));

            /* Return updated COFF loader instance specific parameters. */
            memcpy (params,
                         &(coffLoaderObject->params),
                         sizeof (CoffLoader_Params));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_0trace (curTrace, GT_LEAVE, "CoffLoader_Params_init");
}

/*!
 *  @brief      Function to create an instance of this loader.
 *
 *  @param      procId  Processor ID for which this loader instance is required.
 *  @param      params  Configuration parameters.
 *
 *  @sa         CoffLoader_delete
 */
CoffLoader_Handle
CoffLoader_create (      UInt16              procId,
                   const CoffLoader_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                 status  = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Loader_Object *     handle  = NULL;
    IArg                key;

    GT_2trace (curTrace, GT_ENTER, "CoffLoader_create", procId, params);

    GT_assert (curTrace, IS_VALID_PROCID (procId));
    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (!IS_VALID_PROCID (procId)) {
        /*! @retval NULL Function failed */
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_create",
                             status,
                             "Invalid procId specified");
    }
    else if (params == NULL) {
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_create",
                             status,
                             "params passed is null!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        /* Enter critical section protection. */
        key = IGateProvider_enter (CoffLoader_state.gateHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        /* Check if the loader already exists for specified procId. */
        if (CoffLoader_state.loaderHandles [procId] != NULL) {
            status = LOADER_E_ALREADYEXIST;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_create",
                             status,
                             "Loader already exists for specified procId!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Allocate memory for the handle */
            handle = (Loader_Object *) Memory_calloc (NULL,
                                                      sizeof (Loader_Object),
                                                      0,
                                                      NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (handle == NULL) {
                status = LOADER_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "CoffLoader_create",
                                     status,
                                     "Memory allocation failed for handle!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Populate the handle fields */
                /* Loader functions. */
                handle->loaderFxnTable.attach = &CoffLoader_attach;
                handle->loaderFxnTable.detach = &CoffLoader_detach;
                handle->loaderFxnTable.load = &CoffLoader_load;
                handle->loaderFxnTable.loadSymbols =
                                                   &CoffLoader_loadSymbols;
                handle->loaderFxnTable.unload = &CoffLoader_unload;
                handle->loaderFxnTable.getSymbolAddress =
                                               &CoffLoader_getSymbolAddress;
                handle->loaderFxnTable.getEntryPt = &CoffLoader_getEntryPt;
                handle->loaderFxnTable.getSectionInfo =
                                                     &CoffLoader_getSectionInfo;
                handle->loaderFxnTable.getSectionData =
                                                     &CoffLoader_getSectionData;
                /* File I/O Interface */
                handle->fileFxnTable.seek = &CoffLoaderFile_seek;
                handle->fileFxnTable.tell = &CoffLoaderFile_tell;
                handle->fileFxnTable.read = &CoffLoaderFile_read;

                /* Host Memory management Interface */
                handle->memFxnTable.alloc = &CoffLoaderMem_alloc;
                handle->memFxnTable.free  = &CoffLoaderMem_free;

                /* Target Memory Allocator Interface */
                handle->trgMemFxnTable.alloc = &CoffLoaderTrgMem_alloc;
                handle->trgMemFxnTable.free  = &CoffLoaderTrgMem_free;

                /* Target Memory Access / Write Services */
                handle->trgWriteFxnTable.copy    = &CoffLoaderTrgWrite_copy;
                handle->trgWriteFxnTable.write   = &CoffLoaderTrgWrite_write;
                handle->trgWriteFxnTable.execute = &CoffLoaderTrgWrite_execute;
                handle->trgWriteFxnTable.map     = &CoffLoaderTrgWrite_map;
                handle->trgWriteFxnTable.unmap   = &CoffLoaderTrgWrite_unmap;
                handle->trgWriteFxnTable.translate =
                                                  &CoffLoaderTrgWrite_translate;
                /* Loading and Unloading of Dependent Files */
                handle->dllFxnTable.load   = &CoffLoaderDll_loadDependent;
                handle->dllFxnTable.unload = &CoffLoaderDll_unloadDependent;

                /* Allocate memory for the CoffLoader object */
                handle->object = (CoffLoader_Object *)
                                  Memory_calloc (NULL,
                                                 sizeof (CoffLoader_Object),
                                                 0,
                                                 NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (handle->object == NULL) {
                    status = LOADER_E_MEMORY;
                    GT_setFailureReason (curTrace,
                         GT_4CLASS,
                         "CoffLoader_create",
                         status,
                         "Memory allocation failed for CoffLoader object!");
                }
                else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Allocate memory for the internal CoffLoader object */
                    ((CoffLoader_Object *) (handle->object))->coffLoaderObject =
                           (_CoffLoader_Object *)
                                      Memory_calloc (NULL,
                                                    sizeof (_CoffLoader_Object),
                                                    0,
                                                    NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (((CoffLoader_Object *)
                            (handle->object))->coffLoaderObject == NULL) {
                        status = LOADER_E_MEMORY;
                        GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "CoffLoader_create",
                                    status,
                                    "Memory allocation failed for internal"
                                    " CoffLoader object!");
                    }
                    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        handle->procId = procId;
                        CoffLoader_state.loaderHandles [procId] =
                                                    (CoffLoader_Handle) handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
            }
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Leave critical section protection. */
        IGateProvider_leave (CoffLoader_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)*/

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        if (NULL != handle) {
            if (NULL != handle->object) {
                Memory_free (NULL, handle->object, sizeof (CoffLoader_Object));
            }
            Memory_free (NULL, handle, sizeof (Loader_Object));
        }
        /*! @retval NULL Function failed */
        handle = NULL;
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_create", handle);

    /*! @retval Valid-Handle Operation successful */
    return (CoffLoader_Handle) handle;
}


/*!
 *  @brief      Function to delete an instance of this loader.
 *
 *              The user provided pointer to the handle is reset after
 *              successful completion of this function.
 *
 *  @param      handlePtr   Pointer to handle to the loader instance
 *
 *  @sa         CoffLoader_create
 */
Int
CoffLoader_delete (CoffLoader_Handle * handlePtr)
{
    Int                  status           = LOADER_SUCCESS;
    CoffLoader_Object *  object           = NULL;
    _CoffLoader_Object * coffLoaderObject = NULL;
    Loader_Object *      handle;
    IArg                 key;
    (Void)object;
    GT_1trace (curTrace, GT_ENTER, "CoffLoader_delete", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handlePtr == NULL) {
        /*! @retval LOADER_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_delete",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval LOADER_E_HANDLE Invalid NULL *handlePtr specified */
        status = LOADER_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_delete",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        handle = (Loader_Object *) (*handlePtr);
        /* Enter critical section protection. */
        key = IGateProvider_enter (CoffLoader_state.gateHandle);

        GT_assert (curTrace, (IS_VALID_PROCID (handle->procId)));
        /* Reset handle in Loader handle array. */
        CoffLoader_state.loaderHandles [handle->procId] = NULL;

        object = (CoffLoader_Object *) handle->object;

        /* Free memory used for the CoffLoader object. */
        if (handle->object != NULL) {
            coffLoaderObject = ((CoffLoader_Object *)
                                (handle->object))->coffLoaderObject;
            /* Free memory used for the internal CoffLoader object. */
            if (coffLoaderObject != NULL) {
                Memory_free (NULL,
                             coffLoaderObject,
                             sizeof (_CoffLoader_Object));
            }

            Memory_free (NULL,
                         handle->object,
                         sizeof (CoffLoader_Object));
            handle->object = NULL;
        }

        /* Free memory used for the Loader object. */
        Memory_free (NULL, handle, sizeof (Loader_Object));
        *handlePtr = NULL;

        /* Leave critical section protection. */
        IGateProvider_leave (CoffLoader_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_0trace (curTrace, GT_LEAVE, "CoffLoader_delete");

    /*! @retval LOADER_SUCCESS Operation successful */
    return (status);
}


/*!
 *  @brief      Function to open a handle to an instance of this loader. This
 *              function is called when access to the loader is required from
 *              a different process.
 *
 *  @param      procId  Processor ID addressed by this CoffLoader instance.
 *  @param      handle  Return parameter: Handle to the loader instance
 *
 *  @sa         CoffLoader_close
 */
Int
CoffLoader_open (CoffLoader_Handle * handlePtr, UInt16 procId)
{
    Int status = LOADER_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "CoffLoader_open", handlePtr, procId);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, IS_VALID_PROCID (procId));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handlePtr == NULL) {
        /*! @retval LOADER_E_HANDLE Invalid MULL handlePtr specified */
        status = LOADER_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_open",
                             status,
                             "Invalid NULL handlePtr specified");
    }
    else if (!IS_VALID_PROCID (procId)) {
        /*! @retval LOADER_E_INVALIDARG Invalid procId specified */
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_open",
                             status,
                             "Invalid procId specified");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        /* Initialize return parameter handle. */
        *handlePtr = NULL;

        /* Check if the Loader exists and return the handle if found. */
        if (CoffLoader_state.loaderHandles [procId] == NULL) {
            /*! @retval LOADER_E_NOTFOUND Specified instance not found */
            status = LOADER_E_NOTFOUND;
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "CoffLoader_open",
                              status,
                              "Specified CoffLoader instance does not exist!");
        }
        else {
            *handlePtr = CoffLoader_state.loaderHandles [procId];
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_open", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to close a handle to an instance of this loader.
 *
 *  @param      handlePtr   Pointer to handle to the loader instance
 *
 *  @sa         CoffLoader_open
 */
Int
CoffLoader_close (CoffLoader_Handle * handlePtr)
{
    Int status = LOADER_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "CoffLoader_close", handlePtr);

    GT_assert (curTrace, (handlePtr != NULL));
    GT_assert (curTrace, ((handlePtr != NULL) && (*handlePtr != NULL)));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handlePtr == NULL) {
        /*! @retval LOADER_E_INVALIDARG Invalid NULL handlePtr pointer
                                         specified*/
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_close",
                             status,
                             "Invalid NULL handlePtr pointer specified");
    }
    else if (*handlePtr == NULL) {
        /*! @retval LOADER_E_HANDLE Invalid NULL *handlePtr specified */
        status = LOADER_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_close",
                             status,
                             "Invalid NULL *handlePtr specified");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        /* Nothing to be done for close. */
        *handlePtr = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_close", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/* =============================================================================
 * APIs called by Loader module (part of function table interface)
 * =============================================================================
 */
/*!
 *  @brief      Function to attach to the Loader.
 *
 *  @param      handle  Handle to the loader instance
 *  @param      params  Attach params
 *
 *  @sa         CoffLoader_detach
 */
Int
CoffLoader_attach (Loader_Handle handle, Loader_AttachParams * params)
{
    Int                 status = LOADER_SUCCESS ;
    CoffLoader_Object * coffLoaderObj;

    GT_2trace (curTrace, GT_ENTER, "CoffLoader_attach", handle, params);

    GT_assert (curTrace, (handle != NULL));
    GT_assert (curTrace, (params != NULL));

    /* Set the procHandle. */
    coffLoaderObj = (CoffLoader_Object *) (((Loader_Object *) handle)->object);
    ((_CoffLoader_Object *) (coffLoaderObj->coffLoaderObject))->procHandle =
                                                            params->procHandle;
    ((_CoffLoader_Object *) (coffLoaderObj->coffLoaderObject))->pmHandle =
                                                               params->pmHandle;

    /* Set the procArch. */
    ((_CoffLoader_Object *) (coffLoaderObj->coffLoaderObject))->procArch =
                                                            params->procArch;

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_attach",status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to detach from the Loader.
 *
 *  @param      handle  Handle to the loader instance
 *
 *  @sa         CoffLoader_attach
 */
Int
CoffLoader_detach (Loader_Handle handle)
{
    Int status = LOADER_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "CoffLoader_detach", handle);

    /* Nothing to be done for this. */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_detach",status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to load the slave processor.
 *
 *  @param      handle    Handle to the loader instance
 *  @param      imagePath Full file path of the executable to be loaded
 *  @param      argc      Number of arguments to be sent to the slave main
 *                        function
 *  @param      argv      String array for the arguments
 *  @param      params    Loader specific parameters (if any)
 *  @param      fileId    Return parameter: ID of the loaded file
 *
 *  @sa         CoffLoader_unload
 */
Int
CoffLoader_load (Loader_Handle       handle,
                 String              imagePath,
                 UInt32              argc,
                 String *            argv,
                 Ptr                 params,
                 UInt32 *            fileId)
{
    Int                  status    = LOADER_SUCCESS ;
    OsalKfile_Handle     fileDesc  = NULL;
    Char *               mode      = "r";
    Loader_Object *      loaderObj = (Loader_Object *) handle;
    Coff_Params          coffParams;
    CoffLoader_Object *  coffLoaderObj;
    _CoffLoader_Object * _coffLoaderObj;

    GT_5trace (curTrace, GT_ENTER, "CoffLoader_load",
               handle, imagePath, argc, argv, params);

    /* params are not applicable for file based CoffLoader. */
    GT_assert (curTrace, (handle    != NULL));
    GT_assert (curTrace, (imagePath != NULL));
    GT_assert (curTrace,
               (   ((argc == 0) && (argv == NULL))
                || ((argc != 0) && (argv != NULL)))) ;
    GT_assert (curTrace, (fileId   != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        /*! @retval LOADER_E_HANDLE NULL provided for argument handle. */
        status = LOADER_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_load",
                             status,
                             "NULL provided for argument handle");
    }
    else if (imagePath == NULL) {
        /*! @retval LOADER_E_INVALIDARG NULL provided for argument imagePath. */
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_load",
                             status,
                             "NULL provided for argument imagePath");
    }
    else if (   ((argc == 0) && (argv != NULL))
             || ((argc != 0) && (argv == NULL))) {
        /*! @retval LOADER_E_INVALIDARG Invalid values provided for argc/argv.*/
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_load",
                             status,
                             "Invalid values provided for argc/argv");
    }
    else if (fileId == NULL) {
        /*! @retval LOADER_E_INVALIDARG NULL provided for argument fileId. */
        status = LOADER_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_load",
                             status,
                             "NULL provided for argument fileId");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        status = OsalKfile_open (imagePath, mode, &fileDesc);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "CoffLoader_load",
                                 status,
                                 "Failed to open file!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            GT_assert (curTrace, (fileDesc != NULL));
            coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
            GT_assert (curTrace, (coffLoaderObj != NULL));
            _coffLoaderObj = coffLoaderObj->coffLoaderObject;
            GT_assert (curTrace, (_coffLoaderObj != NULL));
            _coffLoaderObj->fileDesc = fileDesc;

            coffParams.loaderType = Coff_LoaderType_File;
            coffParams.endian     = Processor_Endian_Default;
            coffParams.maduSize   = 1; /* TBD: Based on ProcArch? */
            /* Not applicable for file based loader*/
            coffParams.fileAddr   = 0u;
             /* Not applicable for file based loader*/
            coffParams.size       = 0u;
            coffParams.procArch   = _coffLoaderObj->procArch;
            status = Coff_load (handle,
                                fileDesc,
                                argc,
                                argv,
                                &coffParams,
                                fileId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "CoffLoader_load",
                                     status,
                                     "Failed to load COFF file!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Set the state of the Processor to loaded. */
                Processor_setState (_coffLoaderObj->procHandle,
                                    ProcMgr_State_Loaded);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_load", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to load symbols for the specified file. This will allow
 *              the symbols to be usable when linked against another object
 *              file.
 *              External symbols will be made available for global symbol
 *              linkage.
 *              NOTE: This function is only relevant for dynamic loader. Other
 *                    loaders can return CoffLOADER_E_NOTIMPL.
 *
 *  @param      handle    Handle to the loader instance
 *  @param      imagePath Full file path of the executable to be loaded
 *  @param      params    Loader specific parameters (if any)
 *
 *  @sa         CoffLoader_load
 */
Int
CoffLoader_loadSymbols (Loader_Handle     handle,
                        String            imagePath,
                        Ptr               params)
{
    GT_3trace (curTrace, GT_ENTER, "CoffLoader_loadSymbols",
               handle, imagePath, params);

    (Void) handle; /* Not used. */
    (Void) imagePath; /* Not used. */
    (Void) params; /* Not used. */

    /* Nothing to be done here. Function is not supported for COFF parser. */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_loadSymbols", LOADER_E_NOTIMPL);

    /*! @retval LOADER_E_NOTIMPL This function is not implemented for this
                                 loader. */
    return LOADER_E_NOTIMPL;
}

/*!
 *  @brief      Function to unload the previously loaded file on the slave
 *              processor.
 *
 *  @param      handle   Handle to the loader instance
 *  @param      fileId   ID of the file received from the load function
 *
 *  @sa         CoffLoader_load
 */
Int
CoffLoader_unload (Loader_Handle handle, UInt32 fileId)
{
    Int                  status    = LOADER_SUCCESS;
    Int                  tmpStatus = LOADER_SUCCESS;
    Loader_Object *      loaderObj = (Loader_Object *) handle;
    CoffLoader_Object *  coffLoaderObj;
    _CoffLoader_Object * _coffLoaderObj;

    GT_2trace (curTrace, GT_ENTER, "CoffLoader_unload", handle, fileId);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (handle == NULL) {
        /*! @retval LOADER_E_HANDLE NULL provided for argument handle. */
        status = LOADER_E_HANDLE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_load",
                             status,
                             "NULL provided for argument handle");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
        GT_assert (curTrace, (coffLoaderObj != NULL));
        _coffLoaderObj = coffLoaderObj->coffLoaderObject;
        GT_assert (curTrace, (_coffLoaderObj != NULL));

        status = Coff_unload (handle, fileId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "CoffLoader_unload",
                                 status,
                                 "Failed to unload COFF file!");
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

        tmpStatus = OsalKfile_close ((OsalKfile_Handle *)
                                                 &(_coffLoaderObj->fileDesc));
        if ((tmpStatus < 0) && (status >= 0)) {
            status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "CoffLoader_unload",
                                 status,
                                 "Failed to close the file!");
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_unload", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to retrieve the target address of a symbol from the
 *              specified file.
 *
 *  @param      handle   Handle to the loader instance
 *  @param      fileId   ID of the file received from the load function
 *  @param      symName  Name of the symbol
 *  @param      symValue Return parameter: Symbol address
 *
 *  @sa
 */
Int
CoffLoader_getSymbolAddress (Loader_Handle       handle,
                             UInt32              fileId,
                             String              symName,
                             UInt32 *            symValue)
{
    Int status = LOADER_SUCCESS;

    GT_4trace (curTrace, GT_ENTER, "CoffLoader_getSymbolAddress",
               handle, fileId, symName, symValue);

    GT_assert (curTrace, (handle      != NULL));
    /* Cannot check for fileId because it is loader dependent. */
    /* Number of bytes to be read and sectId can be zero */
    GT_assert (curTrace, (symValue != NULL));

    status = Coff_getSymbolAddress (handle,
                                    fileId,
                                    symName,
                                    symValue);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_getSymbolAddress",
                             status,
                             "Failed to get COFF file symbol information!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_getSymbolAddress", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}

/*!
 *  @brief      Function to retrieve the entry point of the specified file.
 *
 *  @param      handle   Handle to the loader instance
 *  @param      fileId   ID of the file received from the load function
 *  @param      entryPt  Return parameter: Entry point of the executable
 *
 *  @sa         CoffLoader_load
 */
Int
CoffLoader_getEntryPt (Loader_Handle     handle,
                       UInt32            fileId,
                       UInt32 *          entryPt)
{
    Int status = LOADER_SUCCESS;

    GT_3trace (curTrace, GT_ENTER, "CoffLoader_getEntryPt",
               handle, fileId, entryPt);

    GT_assert (curTrace, (handle  != NULL));
    GT_assert (curTrace, (entryPt != NULL));

    status = Coff_getEntryPt (handle, fileId, entryPt);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_getEntryPt",
                             status,
                             "Failed to get COFF file entry point!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_2CLASS, "COFF File entry point: [0x%x]", *entryPt);
    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_getEntryPt", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}

/*!
 *  @brief      Function that returns section information given the name of
 *              section and number of bytes to read
 *
 *  @param      handle      Handle to the ProcMgr object
 *  @param      fileId      ID of the file received from the load function
 *  @param      sectionName Name of section to be retrieved
 *  @param      size        Number of bytes to be reaad from section's data
 *  @param      sectionInfo Return parameter
 *
 *  @sa
 */
Int CoffLoader_getSectionInfo (Loader_Handle         handle,
                               UInt32                fileId,
                               String                sectionName,
                               ProcMgr_SectionInfo * sectionInfo)
{
    Int status = LOADER_SUCCESS;

    GT_4trace (curTrace,
               GT_ENTER,
               "CoffLoader_getSectionInfo",
               handle,
               fileId,
               sectionName,
               sectionInfo);

    GT_assert (curTrace, (handle      != NULL));
    /* Cannot check for fileId because it is loader dependent. */
    GT_assert (curTrace, (sectionName != NULL));
    /* Number of bytes to be read can be zero */
    GT_assert (curTrace, (sectionInfo != NULL));

    status = Coff_getSectionInfo (handle,
                                  fileId,
                                  sectionName,
                                  sectionInfo);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_getSectionInfo",
                             status,
                             "Failed to get COFF file section information!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_getSectionInfo", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}

/*  @brief      Function that returns section data in a buffer given section id
 *              and size to be read
 *
 *  @param      handle      Handle to the ProcMgr object
 *  @param      fileId      ID of the file received from the load function
 *  @param      sectionInfo Section info to be read.
 *  @param      buffer      Return parameter
 *
 *  @sa
 */
Int CoffLoader_getSectionData (Loader_Handle        handle,
                               UInt32               fileId,
                               ProcMgr_SectionInfo * sectionInfo,
                               Ptr                  buffer)
{
    Int status = LOADER_SUCCESS;

    GT_4trace (curTrace,
               GT_ENTER,
               "CoffLoader_getSectionData",
               handle,
               fileId,
               sectionInfo,
               buffer);

    GT_assert (curTrace, (handle      != NULL));
    /* Cannot check for fileId because it is loader dependent. */
    GT_assert (curTrace, (sectionInfo != NULL));
    /* Number of bytes to be read and sectId can be zero */
    GT_assert (curTrace, (buffer != NULL));

    status = Coff_getSectionData (handle,
                                  fileId,
                                  sectionInfo,
                                  buffer);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "CoffLoader_getSectionData",
                             status,
                             "Failed to get COFF file section information!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "CoffLoader_getSectionData", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}
