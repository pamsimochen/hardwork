/*
 *  @file   Coff.c
 *
 *  @brief      Generic COFF parser
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
#endif

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
#endif

#if defined(__KERNEL__)
#include <linux/string.h>
#else
#include <string.h>
#endif

/* SysLink headers */
#include <ti/syslink/inc/knl/ProcDefs.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module level headers */
#include <ti/syslink/inc/knl/Loader.h>
#include <ti/syslink/inc/CoffLoader.h>
#include <ti/syslink/inc/knl/CoffInt.h>
#include <ti/syslink/inc/knl/Coff.h>
#include <ti/syslink/inc/knl/Coff55x.h>
#include <ti/syslink/inc/knl/Coff64x.h>
#include <ti/syslink/inc/knl/CoffM3.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Macro to find if the section is loadable. TBD
 */
#define IS_LOADABLE_SECTION(x) (!(   (((x).flags & (  SECT_NOLOAD       \
                                                    | SECT_DSECT        \
                                                    | SECT_COPY)) > 0)  \
                                  || ((x).size == 0)))


/*!<
 *  @brief  Section name where the configuration info is kept.
 */
/* TODO */
#define SYSLINK_EMBEDDEDCONFIG_SECTNAME     "SysLink_Platform_GenConfig"

/*! @brief Node containing section info */
typedef struct Coff_Sections {
    Coff_SectionHeader info;
    /*!< Section info */
    Bool               isLoaded;
    /*!< Is section loadable */
} Coff_Sections;

/*!
 *  @brief  Module state object
 *
 *          Structure defining the COFF object. This object is created
 *          on initialization of this sub component and it is required to be
 *          passed as a parameter for any subsequent function call.
 */
typedef struct Coff_Object_tag {
    Ptr                  fileDesc;
    /*!< File object for the slave base image file. */
    UInt32               startAddr;
    /*!< Entry point address for the slave base image file. */
    Processor_ProcArch   procArch;
    /*!< Architecture of the slave processor. */
    Bool                 isSwapped;
    /*!< Flag to indicate if the file data is swapped. */
    Coff_SymbolEntry *   symTab;
    /*!< Symbol table of the executable. The memory for the symbol table is
         allocated by the Loader. */
    Char *               strTab;
    /*!< String table of the executable. The memory for the string table is
         allocated by the Loader. */
    UInt32               numSymbols;
    /*!< Number of symbols in the Symbol table of the executable. */
    UInt32               optHdrSize;
    /*!< Size of the optional header. */
    UInt32               numSections;
    /*!< Number of sections in the executable. */
    UInt32               stringTableSize;
    /*!< Size of the string table. */
    Coff_Sections *      sectionInfo;
    /*!< Section table */
    Coff_Params          params;
    /*!< If the COFF loader is memory-based, contains the information about
         memory buffer. */
} Coff_Object ;

/* =============================================================================
 *  Forward declarations of internal functions
 * =============================================================================
 */
/* Repositions the file position indicator to the section header. */
static
Int
_Coff_seekToSectionHeader (Loader_Object * obj,
                           Ptr             fileDesc,
                           UInt32          sectIndex,
                           Bool            swap);

/* Checks if the fields of headers are stored as byte swapped values. */
static
Bool
_Coff_isSwapped (Loader_Object * obj, Ptr fileDesc, Processor_ProcArch procArch);

/* Checks if the file data format is valid for the given architecture. */
static
Bool
_Coff_isValidFile (Loader_Object * obj, Ptr fileDesc, Processor_ProcArch procArch);

/* Fills up the specified buffer with arguments to be sent to the slave's "main"
 * function for the given architecture.
 */
static
Int
_Coff_fillArgsBuffer (Processor_ProcArch     procArch,
                      UInt32                 argc,
                      Char **                argv,
                      UInt32                 sectSize,
                      UInt32                 loadAddr,
                      UInt32                 wordSize,
                      Processor_Endian       endian,
                      Ptr                    argsBuf);

/* Get the size of optional header in file. This function is used at many places
 * to quickly seek to the desired field in file.
 */
static
UInt32
_Coff_getOptHeaderSize (Loader_Object * obj, Ptr fileDesc, Bool swap);

/* Gets the details associated to the symbol table - i.e. number of symbols in
 * the file and the offset of symbol table in file.
 */
static
Int
_Coff_getSymTabDetails (Loader_Object * obj,
                        Ptr             fileDesc,
                        Bool            swap,
                        UInt32 *        offsetSymTab,
                        UInt32 *        numSymbols);

/* Gets the File Header information. The caller should allocate memory for file
 * header.
 */
static
Int
_Coff_getFileHeader (Loader_Object * obj, Coff_FileHeader * fileHeader);

/* Gets the COFF file's optional header. The caller should allocate memory for
 * optional header.
 */
static
Int
_Coff_getOptionalHeader (Loader_Object *    obj,
                         Coff_OptHeader *   optHeader,
                         UInt32 *           optHdrSize);

/* Gets the header information for a section. The caller should allocate memory
 * for section header.
 */
static
Int
_Coff_getSectionHeader (Loader_Object *      obj,
                        UInt32               sectId,
                        Coff_SectionHeader * sectHeader);

/* Gets the string from string table if required. This function checks if the
 * 'str' argument is a valid string, if not, it looks up the string in
 * string-table. If the str argument is a 8 character symbol name padded with
 * nulls, Coff_getString returns a reference to the symbol name being passed to
 * it in the outStr variable (i.e. in this scenario outStr = str). Else it
 * returns a pointer to the offset where the string is present in the string
 * table.
 */
static
Char *
_Coff_getString (Loader_Object * obj,
                 Char *          str);

/* Get the primary SymbolEntry for all the symbols in the coff file.
 * Memory for the symbol table is allocated in this function.
 */
static
Int
_Coff_getSymbolTable (Loader_Object *     obj,
                      UInt32              offsetSymTab,
                      Coff_SymbolEntry ** symTable,
                      UInt32 *            numSymbols);

/* Get the string table for the coff file.
 * Memory for the string table is allocated in this function.
 */
static
Char *
_Coff_getStringTable (Loader_Object *  obj,
                      UInt32           numSymbols,
                      UInt32           offsetSymTab);


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/*!
 *  @brief      Function to load the slave processor.
 *
 *  @param      handle    Handle to the Loader instance
 *  @param      imagePath Descriptor of the file to be loaded
 *  @param      argc      Number of arguments to be sent to the slave main
 *                        function
 *  @param      argv      String array for the arguments
 *  @param      params    Coff loader-specific parameters
 *  @param      fileId    Return parameter: ID of the loaded file
 *
 *  @sa         Coff_unload
 */
Int
Coff_load (Loader_Handle    handle,
           Ptr              fileDesc,
           UInt32           argc,
           String *         argv,
           Coff_Params *    params,
           UInt32 *         fileId)
{
    Int                  status          = LOADER_SUCCESS;
    UInt32               offsetSymTab    = 0;
    Bool                 isValidArch     = FALSE;
    Loader_Object *      loaderObj       = (Loader_Object *) handle;
    Bool                 loadedArgs      = FALSE;
    Bool                 argsSection     = FALSE;
    Char *               sectName        = NULL;
    Coff_FileHeader      fileHeader      = {0};
    Coff_OptHeader       optHeader       = {0};
    Int32                cmpResult       = -1;
    Char                 symbolFromStrTab [COFF_NAME_LEN + 1];
    CoffLoader_Object *  coffLoaderObj;
    Coff_Object *        obj;
    Coff_SectionHeader   sectHeader;
    UInt32               i;
    UInt32               j;
    Loader_MemSegment    memSeg;
    Loader_MemRequest    memReq;

    GT_5trace (curTrace, GT_ENTER, "Coff_load",
               handle, fileDesc, argc, argv, params);

    GT_assert (curTrace, (handle   != NULL));
    GT_assert (curTrace, (fileDesc != NULL));
    GT_assert (curTrace,
               (   ((argc == 0) && (argv == NULL))
                || ((argc != 0) && (argv != NULL)))) ;
    GT_assert (curTrace, (params != NULL)); /* COFF parser expects params */
    GT_assert (curTrace, (fileId != NULL));

    /* This is an internal function, so parameter validation is not done. */
    coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
    GT_assert (curTrace, (coffLoaderObj != NULL));

    /* Allocate memory for the Coff object. */
    obj = coffLoaderObj->coffObject = (Coff_Object *)
            (*(loaderObj->memFxnTable.alloc)) (loaderObj->object,
                                               sizeof (Coff_Object));
    /* Initialize the object. */
    memset (obj, 0, sizeof (Coff_Object));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (coffLoaderObj->coffObject == NULL) {
        /*! @retval LOADER_E_MEMORY Memory allocation failure */
        status = LOADER_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_load",
                             status,
                             "Failed to allocate memory for Coff_Object!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj->procArch    = params->procArch;
        obj->fileDesc    = fileDesc;
        memcpy (&(obj->params), params, sizeof (Coff_Params));

        isValidArch = _Coff_isValidFile (loaderObj, fileDesc, obj->procArch);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (isValidArch == FALSE) {
        /*! @retval LOADER_E_CORRUPTFILE Provided file is not in a supported
                                         format */
            status = LOADER_E_CORRUPTFILE;
            GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_load",
                             status,
                             "Provided file is not in a supported format");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            obj->isSwapped = _Coff_isSwapped (loaderObj,
                                             fileDesc,
                                             obj->procArch);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /*  ------------------------------------------------------------------------
     *  Populate the string table.
     *  ------------------------------------------------------------------------
     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = _Coff_getSymTabDetails (loaderObj,
                                         fileDesc,
                                         obj->isSwapped,
                                         &offsetSymTab,
                                         &(obj->numSymbols));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Coff_load",
                                 status,
                                 "Failed to get symbol table details!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (obj->numSymbols != 0) {
                obj->strTab = _Coff_getStringTable (loaderObj,
                                                    obj->numSymbols,
                                                    offsetSymTab);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (obj->strTab == NULL) {
                    /*! @retval COFFLOADER_E_FILEPARSE Failed to get string
                                                       table. */
                    status = COFFLOADER_E_FILEPARSE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Coff_load",
                                         status,
                                         "Failed to get string table!");
                }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /*  ------------------------------------------------------------------------
     *  Get the symbol table.
     *  ------------------------------------------------------------------------
     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (obj->numSymbols != 0) {
            status = _Coff_getSymbolTable (loaderObj,
                                           offsetSymTab,
                                           &(obj->symTab),
                                           &(obj->numSymbols));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Coff_load",
                                     status,
                                     "Failed to get symbol table!");
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = _Coff_getFileHeader (loaderObj, &fileHeader);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Coff_load",
                                 status,
                                 "Failed to get file header!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "    Coff_load: Number of sections: %d\n",
                       fileHeader.numSections);
            obj->numSections = fileHeader.numSections;
            status = _Coff_getOptionalHeader (loaderObj,
                                              &optHeader,
                                              &(obj->optHdrSize));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Coff_load",
                                     status,
                                     "Failed to get optional file header!");
            }
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        obj->sectionInfo = Memory_calloc (NULL,
                                      obj->numSections * sizeof (Coff_Sections),
                                      0,
                                      NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (obj->sectionInfo == NULL) {
            /*! @retval LOADER_E_FAIL memory allocation failed */
            status = LOADER_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_2CLASS,
                                 "Coff_load",
                                 status,
                                 "memory allocation failed");
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    for (i = 0 ; (status >= 0) && (i < fileHeader.numSections) ; i++) {
        /*  ----------------------------------------------------------------
         *  A COFF image can contain some sections that are not loadable.
         *  So check if the section is a loadable section and contains
         *  data to be written.
         *  ----------------------------------------------------------------
         */
        status = _Coff_getSectionHeader (loaderObj, i, &sectHeader);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Coff_load",
                                 status,
                                 "Failed to get section header!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            if ((argc > 0) && (loadedArgs == FALSE)) {
                for (j = 0 ; j < COFF_NAME_LEN ; j++) {
                    symbolFromStrTab [j] = sectHeader.name [j];
                }
                symbolFromStrTab [COFF_NAME_LEN] = '\0';

                sectName = _Coff_getString (loaderObj, symbolFromStrTab);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (sectName == NULL) {
                    /*! @retval COFFLOADER_E_FILEPARSE Failed to
                     *          get section name string */
                    status = COFFLOADER_E_FILEPARSE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Coff_load",
                                         status,
                                         "Failed to get section name!");
                }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (IS_LOADABLE_SECTION (sectHeader)) {
                    sectHeader.isLoadSection = TRUE;
                }
                else {
                    sectHeader.isLoadSection = FALSE;
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

        /* Insert the section info */
        if (status >= 0) {
            memcpy ((Ptr) &(obj->sectionInfo [i].info),
                         (Ptr) &sectHeader,
                         sizeof (Coff_SectionHeader));
            if (sectHeader.isLoadSection == FALSE) {
                obj->sectionInfo [i].isLoaded = FALSE;
            }
        }

        if ((status >= 0) && (sectHeader.isLoadSection == TRUE)) {
            if ((argc > 0) && (loadedArgs == FALSE)) {
                /* -----------------------------------------------------
                 * Args processing
                 * -----------------------------------------------------
                 */
                cmpResult = strcmp (sectName, ".args") ;
                if (cmpResult == 0) {
                    GT_0trace (curTrace,
                               GT_1CLASS,
                               "    Coff_load: Loading .args section\n");
                    loadedArgs  = TRUE;
                    argsSection = TRUE;
                    sectHeader.data = (*(loaderObj->memFxnTable.alloc))
                                       (loaderObj->object, sectHeader.size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (sectHeader.data == NULL) {
                        /*! @retval LOADER_E_MEMORY Memory
                                              allocation failure */
                        status = LOADER_E_MEMORY;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "Coff_load",
                                             status,
                                             "Failed to allocate memory for"
                                             " section header!");
                    }
                    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Load args into an slave memory */
                        status = _Coff_fillArgsBuffer (obj->procArch,
                                     (UInt32) argc,
                                     argv,
                                     (UInt32) sectHeader.size,
                                     (UInt32) sectHeader.virtualAddress,
                                     (UInt32) params->maduSize,
                                     params->endian,
                                     sectHeader.data);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "Coff_load",
                                            status,
                                        "_Coff_fillArgsBuffer failed!");
                        }
                    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "Coff_load",
                                             status,
                                             "Failed to fill args buffer!");
                    }
                    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        /* Fill up the memory segment information for COFF
                         * Loader.
                         */
                        memSeg.memSize     = sectHeader.size;
                        memSeg.trgAddress  = (UInt32)
                                             sectHeader.virtualAddress;
                        memSeg.objSize     = sectHeader.size;
                        memSeg.trgPage     = 0x0; /* Not required */
                        memReq.segment     = &memSeg;
                        memReq.align       = 2; /* Need 4-byte align */
                        memReq.endian      = params->endian;
                        memReq.fileDesc    = fileDesc;
                        memReq.flags       = LOADER_SEG_EXECUTABLE;
                        memReq.isLoaded    = FALSE;
                        memReq.hostAddress = sectHeader.data;

                        GT_0trace (curTrace,
                                   GT_2CLASS,
                                   "    Coff_load: Writing .args"
                                   " section to target memory\n");
                        /* Write the contents to target memory */
                        status = (*(loaderObj->trgWriteFxnTable.write))
                                               (loaderObj->object, &memReq);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                            GT_4CLASS,
                                            "Coff_load",
                                            status,
                                            "Failed to write section to"
                                            " target memory!");
                        }
                        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                            /* The mapped address is returned from the
                             * loader.
                             */
                            obj->sectionInfo [i].info.data =
                                                       (Ptr) memReq.hostAddress;
                            obj->sectionInfo [i].isLoaded = memReq.isLoaded;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

                        /* Free the section data memory */
                        (*(loaderObj->memFxnTable.free)) (loaderObj->object,
                                                          sectHeader.data,
                                                          sectHeader.size);
                        sectHeader.data = NULL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            /* Not an args section. */
            if ((status >= 0) && (argsSection == FALSE)) {
                /* Load non-args section. */
                /* -------------------------------------------------------------
                 * CoffLoader handles specifics of loader types.
                 * File-based loader:
                 *      Copy: Copy from file into local memory buffer
                 *            return it.
                 *      Write: Write to target memory and free buffer
                 * Memory-based loader:
                 *      Copy: Get offset into file buffer and return.
                 *      Write: Write to target memory.
                 * Shm loader:
                 *      Copy: Copy from file into target memory address.
                 *      Write: Do nothing.
                 * -------------------------------------------------------------
                 */
                /* Fill up the memory segment information for COFF Loader.*/
                memSeg.memSize     = sectHeader.size;
                /* Include section data offset for usage by CoffLoader. */
                memSeg.trgAddress  = (UInt32) sectHeader.virtualAddress;
                memSeg.objSize     = sectHeader.size;
                memSeg.trgPage     = 0x0; /* Not required here. */
                memReq.segment     = &memSeg;
                memReq.align       = 2; /* Need 4-byte alignment. */
                memReq.endian      = params->endian;
                memReq.offset      = (UInt32) sectHeader.fpRawData;
                memReq.fileDesc    = fileDesc;
                memReq.flags       = LOADER_SEG_EXECUTABLE;
                memReq.isLoaded    = FALSE; /* Returned by CoffLoader. */
                memReq.hostAddress = NULL; /* Returned from CoffLoader. */

                GT_1trace (curTrace,
                           GT_3CLASS,
                           "    Coff_load: Reading section [%d] from file"
                           " into host buffer\n",
                           i);
                status = (*(loaderObj->trgWriteFxnTable.copy)) (
                                                loaderObj->object, &memReq);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                               GT_4CLASS,
                               "Coff_load",
                               status,
                               "Failed to copy into host accessible memory!");
                }
                else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    if (memReq.isLoaded == FALSE) {
                        GT_1trace (curTrace,
                                   GT_3CLASS,
                                   "    Coff_load: Writing section [%d] into"
                                   " target memory\n",
                                   i);
                        /* Write the contents to target memory */
                        status = (*(loaderObj->trgWriteFxnTable.write))
                                                   (loaderObj->object, &memReq);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Coff_load",
                                         status,
                                         "Failed to write section to"
                                         " target memory!");
                        }
                        else {
                        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }

                    if (status >= 0) {
                        /* The mapped address is returned from the loader. */
                        obj->sectionInfo [i].info.data =
                                                       (Ptr) memReq.hostAddress;
                        obj->sectionInfo [i].isLoaded = memReq.isLoaded;
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* Re-initialize argsSection to FALSE. */
            argsSection = FALSE;
        }
    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Provide executable entry point to the loader and set in local
         * object.
         */
        obj->startAddr = optHeader.entry;
        status = (*(loaderObj->trgWriteFxnTable.execute)) (loaderObj->object,
                                                           optHeader.entry);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                      GT_4CLASS,
                      "Coff_load",
                      status,
                      "Failed in loader execute function!");
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        Coff_unload (handle, *fileId);
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Coff_load", status);

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
 *                    loaders can return LOADER_E_NOTIMPL.
 *
 *  @param      handle   Handle to the loader instance
 *  @param      fileDesc Descriptor of the file whose symbols are to be loaded.
 *
 *  @sa         Coff_load
 */
Int
Coff_loadSymbols (Loader_Handle handle, Ptr fileDesc)
{
    GT_2trace (curTrace, GT_ENTER, "Coff_loadSymbols", handle, fileDesc);

    (Void) handle; /* Not used. */
    (Void) fileDesc; /* Not used. */

    /* Nothing to be done here. Function is not supported for COFF parser. */
    GT_0trace (curTrace, GT_4CLASS, "Nothing to be done in this function.");

    GT_1trace (curTrace, GT_LEAVE, "Coff_loadSymbols", LOADER_E_NOTIMPL);

    /*! @retval LOADER_E_NOTIMPL This function is not implemented for this
                                 parser. */
    return LOADER_E_NOTIMPL;
}


/*!
 *  @brief      Function to unload the previously loaded file on the slave
 *              processor.
 *
 *  @param      handle   Handle to the loader instance
 *  @param      fileId   ID of the file received from the load function
 *
 *  @sa         Coff_load
 */
/*
 */
Int
Coff_unload (Loader_Handle handle, UInt32 fileId)
{
    Int                  status      = LOADER_SUCCESS;
    Loader_Object *      loaderObj   = (Loader_Object *) handle;
    CoffLoader_Object *  coffLoaderObj;
    Coff_Object *        obj;
    Int                  i;
    ProcMgr_AddrInfo     aInfo;

    GT_2trace (curTrace, GT_ENTER, "Coff_unload", handle, fileId);

    GT_assert (curTrace, (handle != NULL));

    /* This is an internal function, so parameter validation is not done. */
    coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
    GT_assert (curTrace, (coffLoaderObj != NULL));
    obj = coffLoaderObj->coffObject;

    if (obj != NULL) {
        if (obj->sectionInfo != NULL) {
            for (i = 0; i < obj->numSections; i++) {
                if (obj->sectionInfo [i].isLoaded == TRUE) {
                     /* Unmap */
                     aInfo.addr [ProcMgr_AddrType_MasterPhys] = (UInt32)
                                      obj->sectionInfo[i].info.physicalAddress;
                     aInfo.addr [ProcMgr_AddrType_MasterKnlVirt] = (UInt32)
                                      obj->sectionInfo [i].info.data;
                     aInfo.addr [ProcMgr_AddrType_SlaveVirt] = (UInt32)
                                      obj->sectionInfo [i].info.virtualAddress;
                     aInfo.size = obj->sectionInfo[i].info.size;
                     aInfo.isCached = FALSE;
                     /* TBD: For now, don't unmap from slaveVirt also. This
                      * will be done through Platform.c. When DMM is
                      * implemented, slaveVirt unmapping will also be done
                      * here.
                      */
                     (*(loaderObj->trgWriteFxnTable.unmap)) (
                             loaderObj->object,
                             (ProcMgr_MASTERKNLVIRT | ProcMgr_SLAVEVIRT),
                             &aInfo, ProcMgr_AddrType_MasterPhys);
                }
            }

            Memory_free (NULL,
                         obj->sectionInfo,
                         obj->numSections * sizeof(Coff_Sections));
            obj->sectionInfo = NULL;
        }

        /* Free the symbol table. */
        if (obj->symTab != NULL) {
            (*(loaderObj->memFxnTable.free)) (loaderObj->object,
                                              obj->symTab,
                                              (   (obj->numSymbols)
                                               *  sizeof (Coff_SymbolEntry)));
            obj->symTab = NULL;
        }

        /* Free the string table. */
        if (obj->strTab != NULL) {
            (*(loaderObj->memFxnTable.free)) (loaderObj->object,
                                              obj->strTab,
                                              obj->stringTableSize);
            obj->strTab = NULL;
        }

        /* Free memory for the Coff object. */
        (*(loaderObj->memFxnTable.free)) (loaderObj->object,
                                          obj,
                                          sizeof (Coff_Object));
        coffLoaderObj->coffObject = NULL;
    }

    GT_1trace (curTrace, GT_LEAVE, "Coff_unload", status);

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
Coff_getSymbolAddress (Loader_Handle handle,
                       UInt32        fileId,
                       String        symName,
                       UInt32 *      symValue)
{
    Int                  status    = LOADER_SUCCESS;
    Loader_Object *      loaderObj = (Loader_Object *) handle;
    CoffLoader_Object *  coffLoaderObj;
    Coff_Object *        obj;
    Int                  i;
    Char *               temp;

    GT_3trace (curTrace, GT_ENTER, "Coff_getSymbolAddress",
               handle, symName, symValue);

    coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
    GT_assert (curTrace, (coffLoaderObj != NULL));
    obj = coffLoaderObj->coffObject;

    if (obj->numSymbols > 0) {
        for (i = 0 ; i < obj->numSymbols; i++) {
            if (   (obj->symTab [i].name[0] == 0)
                && (obj->symTab [i].name[1] == 0)
                && (obj->symTab [i].name[2] == 0)
                && (obj->symTab [i].name[3] == 0)) {
                temp = _Coff_getString (loaderObj, obj->symTab [i].name);
                if (strcmp (temp, symName) == 0) {
                    *symValue = obj->symTab [i].value;
                    break;
                }
            }
            else {
                if (strcmp (obj->symTab [i].name, symName) == 0) {
                    *symValue = obj->symTab [i].value;
                    break;
                }
            }
        }
    }
    else {
        /*! @retval LOADER_E_FAIL Failed to read from file */
        status = LOADER_E_FAIL;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_getSymbolAddress",
                             status,
                             "Empty symbol table!");
    }

    GT_1trace (curTrace, GT_LEAVE, "Coff_getSymbolAddress", status);

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
 *  @sa         Coff_load
 */
Int
Coff_getEntryPt (Loader_Handle handle,
                 UInt32        fileId,
                 UInt32 *      entryPt)
{
    Int             status      = LOADER_SUCCESS ;
    Loader_Object * loaderObj   = (Loader_Object *) handle;
    Coff_Object *   obj;

    GT_3trace (curTrace, GT_ENTER, "Coff_getEntryPt", handle, fileId, entryPt);

    GT_assert (curTrace, (handle  != NULL));
    /* File ID can be ignored since Coff parser only supports one file load at a
     * time for each loader instance.
     */
    GT_assert (curTrace, (entryPt != NULL));

    /* This is an internal function, so parameter validation is not done. */

    /* Get a pointer to the Coff object. */
    obj = (Coff_Object *) ((CoffLoader_Object *) loaderObj->object)->coffObject;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (obj == NULL) {
        /*! @retval LOADER_E_INVALIDSTATE The loader is in an invalid state. */
        status = LOADER_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_getEntryPt",
                             status,
                             "The loader is in an invalid state!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Provide executable entry point to the loader. */
        *entryPt = obj->startAddr;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,
               GT_1CLASS,
               "    Coff_getEntryPt: Entry point [0x%x]",
               *entryPt);
    GT_1trace (curTrace, GT_LEAVE, "Coff_getEntryPt", status);

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
Int
Coff_getSectionInfo (Loader_Handle         handle,
                     UInt32                fileId,
                     String                sectionName,
                     ProcMgr_SectionInfo * sectionInfo)
{
    /*! @retval LOADER_E_NOTFOUND Could not find section for specified name */
    Int                 status      = LOADER_E_NOTFOUND;
    Loader_Object     * loaderObj   = (Loader_Object *) handle;
    Char *              sectName    = NULL;
    CoffLoader_Object * coffLoaderObj;
    Coff_Object       * obj;
    Int                 i;
    UInt32              j;
    Char                symbolFromStrTab [COFF_NAME_LEN + 1];

    GT_4trace (curTrace,
               GT_ENTER,
               "Coff_getSectionInfo",
               handle,
               fileId,
               sectionName,
               sectionInfo);

    GT_assert (curTrace, (handle      != NULL));
    /* Cannot check for fileId because it is loader dependent. */
    GT_assert (curTrace, (sectionName != NULL));
    /* Number of bytes to be read can be zero */
    GT_assert (curTrace, (sectionInfo != NULL));

    /* This is an internal function, so parameter validation is not done. */
    coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
    GT_assert (curTrace, (coffLoaderObj != NULL));
    obj = coffLoaderObj->coffObject;

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (obj == NULL) {
        /*! @retval LOADER_E_INVALIDSTATE The loader is in an invalid state. */
        status = LOADER_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_getSectionInfo",
                             status,
                             "The loader is in an invalid state!");
    }
    else if (obj->sectionInfo == NULL) {
        /*! @retval LOADER_E_INVALIDSTATE The loader is in an invalid state. */
        status = LOADER_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_getSectionInfo",
                             status,
                             "The loader is in an invalid state!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        for (i = 0; i < obj->numSections; i++) {
            for (j = 0 ; j < COFF_NAME_LEN ; j++) {
                symbolFromStrTab [j] = ((obj->sectionInfo) + i)->info.name [j];
            }
            symbolFromStrTab [COFF_NAME_LEN] = '\0';

            sectName = _Coff_getString (loaderObj, symbolFromStrTab);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (sectName == NULL) {
                /*! @retval COFFLOADER_E_FILEPARSE Failed to
                 *          get section name string */
                status = COFFLOADER_E_FILEPARSE;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Coff_getSectionInfo",
                                     status,
                                     "Failed to get section name!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (strcmp (sectionName, sectName) == 0) {
                    sectionInfo->physicalAddress =
                                 ((obj->sectionInfo) + i)->info.physicalAddress;
                    sectionInfo->virtualAddress =
                                 ((obj->sectionInfo) + i)->info.virtualAddress;
                    sectionInfo->size =
                                 ((obj->sectionInfo) + i)->info.size;
                    sectionInfo->sectId = i;
                    GT_4trace (curTrace,
                               GT_2CLASS,
                               "Coff_getSectionInfo\n"
                               "    sectId          [%d]\n"
                               "    physicalAddress [0x%x]\n"
                               "    virtualAddress  [0x%x]\n"
                               "    size            [0x%x]\n",
                               sectionInfo->sectId,
                               sectionInfo->physicalAddress,
                               sectionInfo->virtualAddress,
                               sectionInfo->size);
                    status = LOADER_SUCCESS;
                    break;
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "Coff_getSectionInfo", status);

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
Int
Coff_getSectionData (Loader_Handle        handle,
                     UInt32               fileId,
                     ProcMgr_SectionInfo * sectionInfo,
                     Ptr                  buffer)
{
    Int                 status      = LOADER_SUCCESS ;
    Loader_Object     * loaderObj   = (Loader_Object *) handle;
    CoffLoader_Object * coffLoaderObj;
    UInt32              numBytes;
    Coff_Object       * obj;
    UInt16              sectId;
    UInt32              size;

    GT_4trace (curTrace,
               GT_ENTER,
               "Coff_getSectionData",
               handle,
               fileId,
               sectionInfo,
               buffer);

    GT_assert (curTrace, (handle   != NULL));
    /* Cannot check for fileId because it is loader dependent. */
    GT_assert (curTrace, (sectionInfo != NULL));
    /* Number of bytes to be read and sectId can be zero */
    GT_assert (curTrace, (buffer   != NULL));

    /* This is an internal function, so parameter validation is not done. */

    sectId = sectionInfo->sectId;
    size = sectionInfo->size;

    coffLoaderObj = (CoffLoader_Object *) loaderObj->object;
    GT_assert (curTrace, (coffLoaderObj != NULL));
    obj = coffLoaderObj->coffObject;

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (obj == NULL) {
        /*! @retval LOADER_E_INVALIDSTATE The loader is in an invalid state. */
        status = LOADER_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_getSectionData",
                             status,
                             "The loader is in an invalid state!");
    }
    else if (obj->sectionInfo == NULL) {
        /*! @retval LOADER_E_INVALIDSTATE The loader is in an invalid state. */
        status = LOADER_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_getSectionData",
                             status,
                             "The loader is in an invalid state!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        /* Seek to the location of the section data.*/
        status = (*(loaderObj->fileFxnTable.seek)) (loaderObj->object,
                                obj->fileDesc,
                                ((obj->sectionInfo) + sectId)->info.fpRawData,
                                LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                      GT_4CLASS,
                      "Coff_getSectionData",
                      status,
                      "Failed in seek!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Read the section data from file. */
            numBytes = (*(loaderObj->fileFxnTable.read)) (loaderObj->object,
                                                          obj->fileDesc,
                                                          buffer,
                                                          size,
                                                          READ_REC_SIZE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (numBytes == 0u) {
                /*! @retval LOADER_E_FILE Failed to read from file */
                status = LOADER_E_FILE;
                GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "Coff_getSectionData",
                          status,
                          "Failed in file read!");
            }
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */


    GT_1trace (curTrace, GT_LEAVE, "Coff_getSectionData", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/* =============================================================================
 *  Internal functions
 * =============================================================================
 */
/*!
 *  @brief      Repositions the file position indicator to the section header.
 *
 *  @param      obj         Handle to the Loader instance
 *  @param      fileDesc    Pointer to the file object
 *  @param      sectIndex   Section Index.
 *  @param      swap        Flag to indicate that headers in this file are
 *                          swapped.
 *
 *  @sa
 */
static
Int
_Coff_seekToSectionHeader (Loader_Object * obj,
                           Ptr             fileDesc,
                           UInt32          sectIndex,
                           Bool            swap)
{
    Int    status      = LOADER_SUCCESS;
    UInt32 numSections = 0;
    Int32  optHdrSize;
    Int32  sectHdrOffset;
	(Void)numSections;
    GT_4trace (curTrace, GT_ENTER, "_Coff_seekToSectionHeader",
               obj, fileDesc, sectIndex, swap);

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

    /* This is an internal function, so parameter validation is not done. */
    optHdrSize = (((Coff_Object *)
                ((CoffLoader_Object *)(obj->object))->coffObject))->optHdrSize;
    GT_assert (curTrace, (optHdrSize != 0));
    numSections = (((Coff_Object *)
                ((CoffLoader_Object *)(obj->object))->coffObject))->numSections;
    GT_assert (curTrace, (sectIndex < numSections));
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (sectIndex >= numSections) {
        /*! @retval COFFLOADER_E_RANGE Provided sectIndex is out of range */
        status = COFFLOADER_E_RANGE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Coff_seekToSectionHeader",
                             status,
                             "Provided sectIndex is out of range!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
        /* Get the offset where the section begins */
        sectHdrOffset = (Int32)  (  SIZE_COFF_FILE_HEADER
                                  + optHdrSize
                                  + (sectIndex * SIZE_COFF_SECTION_HEADER));

        /* Seek to the location of the section header.*/
        status = (*(obj->fileFxnTable.seek)) (obj->object,
                                              fileDesc,
                                              sectHdrOffset,
                                              LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "_Coff_seekToSectionHeader",
                              status,
                              "Failed to seek to section header position!");
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)*/

#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_seekToSectionHeader", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Checks if the fields of headers are stored as byte swapped
 *              values.
 *
 *  @param      obj         Handle to the Loader instance
 *  @param      fileDesc    Pointer to the file object
 *  @param      procArch    Architecture of the processor.
 *
 *  @sa
 */
static
Bool
_Coff_isSwapped (Loader_Object * obj, Ptr fileDesc, Processor_ProcArch procArch)
{
    /*! @retval FALSE Indicates that header fields are not byte-swapped. */
    Bool isSwapped = FALSE;

    GT_3trace (curTrace, GT_ENTER, "_Coff_isSwapped",
               obj, fileDesc, procArch);

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));
    GT_assert (curTrace, (procArch < Processor_ProcArch_EndValue));

    switch (procArch) {
        case Processor_ProcArch_C55x:
        {
            isSwapped = Coff_isSwapped_55x (obj, fileDesc);
        }
        break ;

        case Processor_ProcArch_C64x:
        {
            isSwapped = Coff_isSwapped_64x (obj, fileDesc);
        }
        break ;
        case Processor_ProcArch_M3:
        {
            isSwapped = Coff_isSwapped_M3 (obj, fileDesc);
        }
        break ;

        default:
        {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Coff_isSwapped",
                                 LOADER_E_INVALIDARG,
                                 "Unsupported procArch specified");
        }
        break ;
    }

    GT_1trace (curTrace, GT_LEAVE, "_Coff_isSwapped", isSwapped);

    /*! @retval TRUE Indicates that header fields are byte-swapped. */
    return isSwapped;
}


/*!
 *  @brief  Checks if the file data format is valid for the given architecture.
 *
 *  @param      obj         Handle to the Loader instance
 *  @param      fileDesc    Pointer to the file object
 *  @param      procArch    Architecture of the processor.
 *
 *  @sa
 */
static
Bool
_Coff_isValidFile (Loader_Object * obj, Ptr fileDesc, Processor_ProcArch procArch)
{
    /*! @retval FALSE File is not a valid COFF file. */
    Bool isValidFile = FALSE;

    GT_3trace (curTrace, GT_ENTER, "_Coff_isValidFile",
               obj, fileDesc, procArch);

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));
    GT_assert (curTrace, (procArch < Processor_ProcArch_EndValue));

    switch (procArch) {
        case Processor_ProcArch_C55x:
        {
            isValidFile = Coff_isValidFile_55x (obj, fileDesc);
        }
        break ;

        case Processor_ProcArch_C64x:
        {
            isValidFile = Coff_isValidFile_64x (obj, fileDesc);
        }
        break ;
        case Processor_ProcArch_M3:
        {
            isValidFile = Coff_isValidFile_M3 (obj, fileDesc);
        }
        break;
        default:
        {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Coff_isValidFile",
                                 LOADER_E_INVALIDARG,
                                 "Unsupported procArch specified");
        }
        break ;
    }

    GT_1trace (curTrace, GT_LEAVE, "_Coff_isValidFile", isValidFile);

    /*! @retval TRUE File is a valid COFF file. */
    return isValidFile;
}


/*!
 *  @brief  Fills up the specified buffer with arguments to be sent to the
 *          slave's "main" function for the given architecture.
 *
 *  @param      procArch  Architecture of the processor.
 *  @param      argc      Number of arguments to be sent to the slave main
 *                        function
 *  @param      argv      String array for the arguments
 *  @param      sectSize  Size of the '.args' section obtained from the COFF
 *                        file.
 *  @param      loadAddr  Load address for the '.args' section.
 *  @param      wordSize  Word-size on the target processor.
 *  @param      endian    Endianism to be used while writing data.
 *  @param      argsBuf   Buffer to be filled with formatted argc and argv.
 *
 *  @sa
 */
static
Int
_Coff_fillArgsBuffer (Processor_ProcArch procArch,
                      UInt32             argc,
                      Char **            argv,
                      UInt32             sectSize,
                      UInt32             loadAddr,
                      UInt32             wordSize,
                      Processor_Endian   endian,
                      Ptr                argsBuf)
{
    Int status = LOADER_SUCCESS;

    GT_5trace (curTrace, GT_ENTER, "_Coff_fillArgsBuffer",
               procArch, argc, argv, sectSize, loadAddr);

    GT_assert (curTrace, (procArch < Processor_ProcArch_EndValue));

    /* If this function is called, it means that args are provided.*/
    GT_assert (curTrace, (argc > 0u));
    GT_assert (curTrace, (argv != NULL));
    GT_assert (curTrace, (sectSize != 0u));
    GT_assert (curTrace, (loadAddr != 0u));
    GT_assert (curTrace, (endian < Processor_Endian_EndValue));
    GT_assert (curTrace, (argsBuf != NULL));

    switch (procArch) {
        case Processor_ProcArch_C55x:
        {
            status = Coff_fillArgsBuffer_55x (procArch,
                                              argc,
                                              argv,
                                              sectSize,
                                              loadAddr,
                                              wordSize,
                                              endian,
                                              argsBuf);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_Coff_fillArgsBuffer",
                                     status,
                                     "Filling args buffer failed!");

            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break ;

        case Processor_ProcArch_C64x:
        {
            status = Coff_fillArgsBuffer_64x (procArch,
                                              argc,
                                              argv,
                                              sectSize,
                                              loadAddr,
                                              wordSize,
                                              endian,
                                              argsBuf);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_Coff_fillArgsBuffer",
                                     status,
                                     "Filling args buffer failed!");

            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break ;

        case Processor_ProcArch_M3:
        {
            status = Coff_fillArgsBuffer_M3 (procArch,
                                              argc,
                                              argv,
                                              sectSize,
                                              loadAddr,
                                              wordSize,
                                              endian,
                                              argsBuf);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_Coff_fillArgsBuffer",
                                     status,
                                     "Filling args buffer failed!");

            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break ;

        default:
        {
            /*! @retval LOADER_SUCCESS Unsupported procArch specified. */
            status = LOADER_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Coff_fillArgsBuffer",
                                 status,
                                 "Unsupported procArch specified");
        }
        break ;
    }

    GT_1trace (curTrace, GT_LEAVE, "_Coff_fillArgsBuffer", status);

    /*! @retval LOADER_SUCCESS Operation successfully completed. */
    return status;
}

/*!
 *  @brief      Get the size of optional header in file. This function is used
 *              at many places to quickly seek to the desired field in file.
 *
 *  @param      obj         Handle to the Loader instance
 *  @param      fileDesc    Pointer to the file object
 *  @param      swap        Flag to indicate that headers in this file are
 *                          swapped.
 *
 *  @sa
 */
static
UInt32
_Coff_getOptHeaderSize (Loader_Object * obj, Ptr fileDesc, Bool swap)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int    status     = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    /*! @retval 0   Failed to get Optional header size */
    UInt32 optHdrSize = 0u;

    GT_3trace (curTrace, GT_ENTER, "_Coff_getOptHeaderSize",
               obj, fileDesc, swap);

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

    /* This is an internal function, so parameter validation is not done. */
    /* Seek to the location of the optional file header.*/
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
             (*(obj->fileFxnTable.seek)) (obj->object,
                                          fileDesc,
                                          SIZE_OPT_HDR_LOC,
                                          LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "_Coff_getOptHeaderSize",
                          status,
                          "Failed to seek to optional file header position!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        optHdrSize = Coff_read16 (obj, fileDesc, swap);
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "    _Coff_getOptHeaderSize: Optional Header Size: %d\n",
                   optHdrSize);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_getOptHeaderSize", optHdrSize);

    /*! @retval Optional Header Size Operation successful */
    return optHdrSize;
}


/*!
 *  @brief      Gets the details associated to the symbol table -
 *              i.e. number of symbols in the file and the offset of symbol
 *              table in file.
 *
 *  @param      obj          Handle to the Loader instance
 *  @param      fileDesc     Pointer to the file object
 *  @param      swap         Flag to indicate that headers in this file are
 *                           swapped.
 *  @param      offsetSymTab Return parameter: Offset of symbol table.
 *  @param      numSymbols   Return parameter: Number of symbols.
 *
 *  @sa
 */
static
Int
_Coff_getSymTabDetails (Loader_Object * obj,
                        Ptr             fileDesc,
                        Bool            swap,
                        UInt32 *        offsetSymTab,
                        UInt32 *        numSymbols)
{
    Int status = LOADER_SUCCESS;

    GT_5trace (curTrace, GT_ENTER, "_Coff_getSymTabDetails",
               obj, fileDesc, swap, offsetSymTab, numSymbols);

    GT_assert (curTrace, (obj           != NULL));
    GT_assert (curTrace, (fileDesc      != NULL));
    GT_assert (curTrace, (offsetSymTab  != NULL));
    GT_assert (curTrace, (numSymbols    != NULL));

    /* This is an internal function, so parameter validation is not done. */

    /* Seek to the location of the symbol table information. */
    status = (*(obj->fileFxnTable.seek)) (obj->object,
                                          fileDesc,
                                          SYMTAB_OFFSET,
                                          LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "_Coff_getSymTabDetails",
                          status,
                          "Failed to seek to location of symbol table info!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        *offsetSymTab = (UInt32) Coff_read32 (obj, fileDesc, swap);
        *numSymbols   = (UInt32) Coff_read32 (obj, fileDesc, swap);
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "    _Coff_getSymTabDetails: Number of symbols: %d\n",
                   *numSymbols);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_getSymTabDetails", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Gets the File Header information.
 *              The caller should allocate memory for file header.
 *
 *  @param      obj          Handle to the Loader instance
 *  @param      fileHeader   Return parameter: File header information.
 *
 *  @sa
 */
static
Int
_Coff_getFileHeader (Loader_Object * obj, Coff_FileHeader * fileHeader)
{
    Int  status   = LOADER_SUCCESS;
    Bool swap     = FALSE;
    Ptr  fileDesc = NULL;

    GT_2trace (curTrace, GT_ENTER, "_Coff_getFileHeader", obj, fileHeader);

    GT_assert (curTrace, (obj != NULL));
    GT_assert (curTrace, (fileHeader != NULL));

    /* This is an internal function, so parameter validation is not done. */
    fileDesc = (((Coff_Object *)
                ((CoffLoader_Object *)(obj->object))->coffObject))->fileDesc;
    swap = (((Coff_Object *)
            ((CoffLoader_Object *)(obj->object))->coffObject))->isSwapped;
    GT_assert (curTrace, (fileDesc != NULL));

    /* Read the file header from the start of the file.*/
    status = (*(obj->fileFxnTable.seek)) (obj->object,
                                          fileDesc,
                                          0,
                                          LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Coff_getFileHeader",
                             status,
                             "Failed to seek to file header position!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        fileHeader->version           = (UInt16) Coff_read16 (obj,
                                                              fileDesc,
                                                              swap);
        fileHeader->numSections       = (UInt16) Coff_read16 (obj,
                                                              fileDesc,
                                                              swap);
        fileHeader->dateTime          = Coff_read32 (obj, fileDesc, swap);
        fileHeader->fpSymTab          = Coff_read32 (obj, fileDesc, swap);
        fileHeader->numSymTabEntries  = Coff_read32 (obj, fileDesc, swap);
        fileHeader->numBytesOptHeader = (UInt16) Coff_read16 (obj,
                                                              fileDesc,
                                                              swap);
        fileHeader->flags             = (UInt16) Coff_read16 (obj,
                                                              fileDesc,
                                                              swap);
        fileHeader->targetId          = (UInt16) Coff_read16 (obj,
                                                              fileDesc,
                                                              swap);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_getFileHeader", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Gets the COFF file's optional header.
 *              The caller should allocate memory for optional header.
 *
 *  @param      obj          Handle to the Loader instance
 *  @param      optHeader    Return parameter: Optional header information.
 *
 *  @sa
 */
static
Int
_Coff_getOptionalHeader (Loader_Object *    obj,
                         Coff_OptHeader *   optHeader,
                         UInt32 *           optHdrSize)
{
    Int   status            = LOADER_SUCCESS;
    Bool  swap              = FALSE;
    Ptr   fileDesc          = NULL;
    Int32 numBytesOptHeader = 0;

    GT_3trace (curTrace, GT_ENTER, "_Coff_getOptionalHeader",
               obj, optHeader, optHdrSize);

    GT_assert (curTrace, (obj != NULL));
    GT_assert (curTrace, (optHeader != NULL));
    GT_assert (curTrace, (optHdrSize != NULL));

    /* This is an internal function, so parameter validation is not done. */
    fileDesc = (((Coff_Object *)
                ((CoffLoader_Object *)(obj->object))->coffObject))->fileDesc;
    swap = (((Coff_Object *)
            ((CoffLoader_Object *)(obj->object))->coffObject))->isSwapped;
    GT_assert (curTrace, (fileDesc != NULL));

    numBytesOptHeader = _Coff_getOptHeaderSize (obj, fileDesc, swap);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (numBytesOptHeader == 0) {
        /*! @retval COFFLOADER_E_FILEPARSE Failed to get optional file header
                                           size */
        status = COFFLOADER_E_FILEPARSE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Coff_getOptionalHeader",
                             status,
                             "Failed to get optional file header size!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        *optHdrSize = numBytesOptHeader;
        /* Seek to the location of the Optional File Header. */
        status = (*(obj->fileFxnTable.seek)) (obj->object,
                                              fileDesc,
                                              SIZE_COFF_FILE_HEADER,
                                              LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "_Coff_getOptionalHeader",
                            status,
                            "Failed to seek to optional file header position!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            optHeader->magic          = Coff_read16 (obj, fileDesc, swap);
            optHeader->version        = Coff_read16 (obj, fileDesc, swap);
            optHeader->sizeExeCode    = Coff_read32 (obj, fileDesc, swap);
            optHeader->sizeInitData   = Coff_read32 (obj, fileDesc, swap);
            optHeader->sizeUninitData = Coff_read32 (obj, fileDesc, swap);
            optHeader->entry          = Coff_read32 (obj, fileDesc, swap);
            optHeader->addrExe        = Coff_read32 (obj, fileDesc, swap);
            optHeader->addrInitData   = Coff_read32 (obj, fileDesc, swap);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_getOptionalHeader", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Gets the header information for a section.
 *              The caller should allocate memory for section header.
 *
 *  @param      obj          Handle to the Loader instance
 *  @param      sectId       Section index
 *  @param      sectHeader   Return parameter: Section header information
 *
 *  @sa
 */
static
Int
_Coff_getSectionHeader (Loader_Object *      obj,
                        UInt32               sectId,
                        Coff_SectionHeader * sectHeader)
{
    Int   status            = LOADER_SUCCESS;
    Bool  swap              = FALSE;
    Ptr   fileDesc          = NULL;
    Ptr   addr              = NULL;
    Int32 i;

    GT_3trace (curTrace, GT_ENTER, "_Coff_getSectionHeader",
               obj, sectId, sectHeader);

    GT_assert (curTrace, (obj != NULL));
    GT_assert (curTrace, (sectHeader != NULL));

    /* This is an internal function, so parameter validation is not done. */
    fileDesc = (((Coff_Object *)
                ((CoffLoader_Object *)(obj->object))->coffObject))->fileDesc;
    swap = (((Coff_Object *)
            ((CoffLoader_Object *)(obj->object))->coffObject))->isSwapped;
    GT_assert (curTrace, (fileDesc != NULL));

    status = _Coff_seekToSectionHeader (obj, fileDesc, sectId, swap);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Coff_getSectionHeader",
                             status,
                             "Failed to seek to section header!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        for (i = 0 ; i < COFF_NAME_LEN ; i++) {
            sectHeader->name [i] = Coff_read8 (obj, fileDesc) ;
        }
        sectHeader->physicalAddress = Coff_read32 (obj, fileDesc, swap);
        if (sectHeader->physicalAddress) {
               /* Update the physicalAddress to actual one if master phys is different
                * from slavePhys
                */
               status = (*(obj->trgWriteFxnTable.translate)) (obj->object,
                                                          (Ptr *)&addr,
                                                          ProcMgr_AddrType_MasterPhys,
                                                          (Ptr)sectHeader->physicalAddress,
                                                          ProcMgr_AddrType_SlaveVirt);
               GT_assert (curTrace, (status >= 0));
               sectHeader->physicalAddress = (UInt32) addr;
        }
        sectHeader->virtualAddress  = Coff_read32 (obj, fileDesc, swap);
        sectHeader->size            = Coff_read32 (obj, fileDesc, swap);
        sectHeader->fpRawData       = Coff_read32 (obj, fileDesc, swap);
        sectHeader->fpReloc         = Coff_read32 (obj, fileDesc, swap);
        sectHeader->fpLineNum       = Coff_read32 (obj, fileDesc, swap);
        sectHeader->numReloc   = (UInt32) Coff_read32 (obj, fileDesc, swap);
        sectHeader->numLine    = (UInt32) Coff_read32 (obj, fileDesc, swap);
        sectHeader->flags      = (UInt32) Coff_read32 (obj, fileDesc, swap);
        sectHeader->reserved   = (UInt16) Coff_read16 (obj, fileDesc, swap);
        sectHeader->memPageNum = (UInt16) Coff_read16 (obj, fileDesc, swap);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_getSectionHeader", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Gets the string from string table if required. This function
 *              checks if the 'str' argument is a valid string, if not, it looks
 *              up the string in string-table.
 *              If the str argument is a 8 character symbol name padded with
 *              nulls, Coff_getString returns a reference to the symbol name
 *              being passed to it in the outStr variable (i.e. in this scenario
 *              outStr = str). Else it returns a pointer to the offset where
 *              the string is present in the string table.
 *
 *  @param      obj         Handle to the Loader instance
 *  @param      str         Pointer to the string that needs to be checked in
 *                          string table.
 *
 *  @sa
 */
static
Char *
_Coff_getString (Loader_Object * obj,
                 Char *          str)
{
    /*! @retval NULL Failed to get string */
    Char *        outStr  = NULL;
    Bool          swap    = FALSE;
    Coff_Object * coffObj = NULL;
    UInt32        strOffset;
    UInt32        offsetinStrTab;

    GT_assert (curTrace, (obj != NULL));
    GT_assert (curTrace, (str != NULL));

    /* This is an internal function, so parameter validation is not done. */

    coffObj = (Coff_Object *) ((CoffLoader_Object *)(obj->object))->coffObject;

    /*  ------------------------------------------------------------------------
     *  If the first four bytes of the 'string' are NULL - it indicates that
     *  string is present in the symbol table.
     *  ------------------------------------------------------------------------
     */
    if (*((UInt32 *) ((Ptr) str)) == 0) {
        swap = coffObj->isSwapped;

        if (swap == TRUE) {
            strOffset = BYTESWAP_LONG (*((UInt32 *) (Ptr) (str + 4)));
        }
        else {
            strOffset = *(UInt32 *) ((Ptr) (str + 4));
        }

        /*  --------------------------------------------------------------------
         *  Decrement the offset by 4 to account for the 4 bytes needed
         *  to store the size
         *  --------------------------------------------------------------------
         */
        strOffset -= 4;

        offsetinStrTab = ((UInt32)((Ptr) coffObj->strTab) + strOffset);

        /* Return the offset in the string table */
        outStr  = (Char *) (offsetinStrTab);
    }
    else {
        /* Return the 'str' argument itself */
        outStr = str;
    }

    /*! @retval Pointer to string   Operation successful*/
    return outStr;
}


/*!
 *  @brief      Get the primary SymbolEntry for all the symbols in the COFF
 *              file. Memory for the symbol table is allocated in this function.
 *
 *  @param      obj          Handle to the Loader instance
 *  @param      offsetSymTab Pointer to the string that needs to be checked in
 *                           string table.
 *  @param      symTable     Return parameter: Pointer to symbol table
 *  @param      numSymbols   Return parameter: Number of symbols in symbol table
 *
 *  @sa
 */
static
Int
_Coff_getSymbolTable (Loader_Object *     obj,
                      UInt32              offsetSymTab,
                      Coff_SymbolEntry ** symTable,
                      UInt32 *            numSymbols)
{
    Int                status           = LOADER_SUCCESS;
    Bool               swap             = FALSE;
    Ptr                fileDesc         = NULL;
    Char *             symData          = NULL;
    Char *             symDataAddress   = NULL;
    Coff_SymbolEntry * coffSymbolEntry  = NULL;
    Coff_Object *      coffObj          = NULL;
    UInt32             numBytes         = 0u;
    UInt32             stringTableStart;
    UInt32             i;
    UInt32             j;
    UInt8              temp8_1;
    UInt8              temp8_2;
    UInt8              temp8_3;
    UInt8              temp8_4;
    UInt32             temp;

    GT_4trace (curTrace, GT_ENTER, "_Coff_getSymbolTable",
               obj, offsetSymTab, symTable, numSymbols);

    GT_assert (curTrace, (obj        != NULL));
    GT_assert (curTrace, (symTable   != NULL));
    GT_assert (curTrace, (numSymbols != NULL));

    /* This is an internal function, so parameter validation is not done. */

    coffObj = (Coff_Object *) ((CoffLoader_Object *)(obj->object))->coffObject;

    *symTable = NULL ;
    swap = coffObj->isSwapped;

    GT_assert (curTrace, (coffObj->fileDesc != NULL));
    fileDesc = coffObj->fileDesc;

    /* Allocate memory for the symbol table. */
    *symTable = (*(obj->memFxnTable.alloc)) (obj->object,
                                            (   (coffObj->numSymbols)
                                             *  sizeof (Coff_SymbolEntry)));
    GT_1trace (curTrace,
               GT_2CLASS,
               "    _Coff_getSymbolTable: Allocated memory for "
               "the symbol table\n",
               *symTable);
    GT_1trace (curTrace,
               GT_2CLASS,
               "    _Coff_getSymbolTable: Seeking to the location of"
               " the symbol table\n",
               offsetSymTab);
    /* Seek to the location of the Symbol Table. */
    status = (*(obj->fileFxnTable.seek)) (obj->object,
                                          fileDesc,
                                          offsetSymTab,
                                          LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Coff_getSymbolTable",
                             status,
                             "Failed to seek to symbol table position!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate temporary memory for the symbol table. */
        symData = (*(obj->memFxnTable.alloc)) (obj->object,
                                               (  (coffObj->numSymbols)
                                                * SIZE_COFF_SYMBOL_ENTRY));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (symData == NULL) {
            /*! @retval LOADER_E_MEMORY Memory allocation failed for
                                        temporary symbol table */
            status = LOADER_E_MEMORY;
            GT_setFailureReason (curTrace,
                    GT_4CLASS,
                    "_Coff_getSymbolTable",
                    status,
                    "Memory allocation failed for temporary symbol table!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Store start address of symbol table */
            symDataAddress   = symData ;
            stringTableStart =  ((*numSymbols) * SIZE_COFF_SYMBOL_ENTRY)
                              + offsetSymTab;
            GT_1trace (curTrace,
                       GT_2CLASS,
                       "    _Coff_getSymbolTable: Reading symbol "
                       "table into memory\n",
                       symData);
            GT_1trace (curTrace,
                       GT_2CLASS,
                       "    _Coff_getSymbolTable: Start of "
                       "string table [0x%x]\n",
                       stringTableStart);
            /* Read symbol table into memory */
            numBytes = (*(obj->fileFxnTable.read)) (obj->object,
                                   fileDesc,
                                   symData,
                                   ((*numSymbols) * SIZE_COFF_SYMBOL_ENTRY),
                                   READ_REC_SIZE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (numBytes == 0u) {
                /*! @retval LOADER_E_FILE Failed to read from file */
                status = LOADER_E_FILE;
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "_Coff_getSymbolTable",
                                  status,
                                  "Failed to read symbol table from file!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                i = 0;
                while (offsetSymTab < stringTableStart) {
                    coffSymbolEntry = (Coff_SymbolEntry *) ((Ptr) symData);

                    /*  ----------------------------------------------------
                     *  File up entries of symbol table and account for
                     *  swapping
                     *  ----------------------------------------------------
                     */
                    for (j = 0 ; j < COFF_NAME_LEN ; j++) {
                        (*symTable) [i].name [j] = coffSymbolEntry->name[j];
                    }
                    GT_1trace (curTrace,
                             GT_3CLASS,
                             "    _Coff_getSymbolTable: Symbol name [%s]\n",
                             _Coff_getString (obj, (*symTable) [i].name));

                    temp8_1 = *((UInt8 *) ((UInt32) coffSymbolEntry + 8));
                    temp8_2 = *((UInt8 *) ((UInt32) coffSymbolEntry + 9));
                    temp8_3 = *((UInt8 *) ((UInt32) coffSymbolEntry + 10));
                    temp8_4 = *((UInt8 *) ((UInt32) coffSymbolEntry + 11));
                    temp = (UInt32) (    ((UInt32) temp8_4 << 24)
                                     |   ((UInt32) temp8_3 << 16)
                                     |   ((UInt32) temp8_2 << 8)
                                     |   ((UInt32) temp8_1));
                    (*symTable) [i].value     = temp;
                    (*symTable) [i].sectNum   = coffSymbolEntry->sectNum;
                    (*symTable) [i].type      = coffSymbolEntry->type;
                    (*symTable) [i].storage   = coffSymbolEntry->storage;
                    (*symTable) [i].numAuxEnt = coffSymbolEntry->numAuxEnt;
                    if (swap == TRUE) {
                        (*symTable) [i].value =
                                    BYTESWAP_LONG ((*symTable) [i].value);
                        (*symTable) [i].sectNum =
                                    BYTESWAP_WORD ((*symTable) [i].sectNum);
                        (*symTable) [i].type =
                                    BYTESWAP_WORD ((*symTable) [i].type);
                    }
                    symData      += SIZE_COFF_SYMBOL_ENTRY;
                    offsetSymTab += SIZE_COFF_SYMBOL_ENTRY;

                    if ((*symTable) [i].numAuxEnt == 1) {
                        symData      += SIZE_COFF_SYMBOL_ENTRY;
                        offsetSymTab += SIZE_COFF_SYMBOL_ENTRY;
                    }
                    i++;
                }
                *numSymbols = i;
                GT_1trace (curTrace,
                           GT_2CLASS,
                           "    _Coff_getSymbolTable: Updated numSymbols"
                           " after reading symbol table [%d]\n",
                           *numSymbols);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Free temporary memory reserved for symbol table. */
            if (symDataAddress != NULL) {
                (*(obj->memFxnTable.free)) (obj->object,
                                            symDataAddress,
                                            (   (coffObj->numSymbols)
                                             *  sizeof (Coff_SymbolEntry)));
                symDataAddress = NULL;
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }

    /* Free the symbol table if there was a failure. */
    if ((status < 0) && (symTable != NULL) && (*symTable != NULL)) {
        (*(obj->memFxnTable.free)) (obj->object,
                                    *symTable,
                                    (   (coffObj->numSymbols)
                                     *  sizeof (Coff_SymbolEntry)));
        *symTable = NULL;
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_Coff_getSymbolTable", status);

    /*! @retval LOADER_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Get the string table for the COFF file.
 *              Memory for the string table is allocated in this function.
 *
 *  @param      obj          Handle to the Loader instance
 *  @param      numSymbols   Number of symbols in symbol table
 *  @param      offsetSymTab Offset to symbol table in file
 *
 *  @sa
 */
static
Char *
_Coff_getStringTable (Loader_Object *  obj,
                      UInt32           numSymbols,
                      UInt32           offsetSymTab)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int             status   = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    /*! @retval NULL    Failure to get string table. */
    Char *          strTable = NULL;
    Coff_Object *   coffObj  = NULL;
    Bool            swap     = FALSE;
    Ptr             fileDesc = NULL;
    UInt32          numBytes = 0u;
    UInt32          sizeStringTable;
    UInt32          offsetStrTable;

    GT_3trace (curTrace, GT_ENTER, "_Coff_getStringTable",
               obj, numSymbols, offsetSymTab);

    GT_assert (curTrace, (obj != NULL));

    /* This is an internal function, so parameter validation is not done. */

    coffObj = (Coff_Object *) ((CoffLoader_Object *)(obj->object))->coffObject;
    swap = coffObj->isSwapped;

    GT_assert (curTrace, (coffObj->fileDesc != NULL));
    fileDesc = coffObj->fileDesc;

    /* Get the offset of the string table. */
    offsetStrTable = ((numSymbols) * SIZE_COFF_SYMBOL_ENTRY) + offsetSymTab;

    /* Validate position into file and set seek pointer. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
             (*(obj->fileFxnTable.seek)) (obj->object,
                                          fileDesc,
                                          offsetStrTable,
                                          LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_Coff_getSymbolTable",
                             status,
                             "Failed to seek to symbol table position!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /*  ------------------------------------------------------------
         *  Get the string table into memory. This will enable faster
         *  access to the string name if the name is in the string table
         *  ------------------------------------------------------------
         */

        /*  Get the size of the string table. The first 4 bytes of the
         *  string table indicate its size. Decrement the size of the
         *  string table by 4 to account for the 4 bytes needed to store
         *  the size
         */
        sizeStringTable  = (UInt32) Coff_read32 (obj, fileDesc, swap);
        sizeStringTable -= 4 ;
        coffObj->stringTableSize = sizeStringTable;

        /* Allocate memory for string table */
        strTable = (*(obj->memFxnTable.alloc)) (obj->object,
                                                sizeStringTable);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (strTable == NULL) {
            /*! @retval LOADER_E_MEMORY Memory allocation failed for string
                                        table */
            status = LOADER_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_Coff_getStringTable",
                                 status,
                                 "Memory allocation failed for string table!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Read string table into memory */
            numBytes = (*(obj->fileFxnTable.read)) (obj->object,
                                                    fileDesc,
                                                    strTable,
                                                    sizeStringTable,
                                                    READ_REC_SIZE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (numBytes == 0u) {
                /*! @retval LOADER_E_FILE Failed to read from file */
                status = LOADER_E_FILE;
                GT_setFailureReason (curTrace,
                                  GT_4CLASS,
                                  "_Coff_getStringTable",
                                  status,
                                  "Failed to read string table from file!");
            }

            /* Free the string table if there was a failure. */
            if ((status < 0) && (strTable != NULL)) {
                (*(obj->memFxnTable.free)) (obj->object,
                                            strTable,
                                            sizeStringTable);
                strTable = NULL;
            }
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace,
               GT_1CLASS,
               "    _Coff_getStringTable: String table [0x%x]",
               strTable);
    GT_1trace (curTrace, GT_LEAVE, "_Coff_getStringTable", strTable);

    /*! @retval String Table    Operation successful */
    return strTable;
}
