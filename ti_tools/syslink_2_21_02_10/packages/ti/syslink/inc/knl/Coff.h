/*
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
 */

/**
 *  @file   ti/syslink/inc/knl/Coff.h
 *
 *  @brief      Generic COFF parser header file
 */


#ifndef Coff_H_0xcb32
#define Coff_H_0xcb32


/* Module headers */
#include "LoaderDefs.h"
#include "ProcDefs.h"


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Location in COFF file where swap information is kept.
 */
#define     SWAP_LOCATION                     20u

/*!
 *  @brief  Identifier for dummy section.
 */
#define     SECT_DSECT                        0x0001u

/*!
 *  @brief  Identifier for a no_load section.
 */
#define     SECT_NOLOAD                       0x0002u

/*!
 *  @brief  Identifier for a BSS section.
 */
#define     SECT_BSS                          0x0080u

/*!
 *  @brief  Identifier for a COPY section.
 */
#define     SECT_COPY                         0x0010u

/*!
 *  @brief  Offset in file header where number of sections is present.
 */
#define     COFF_VERSION                       0x00C2u

/*!
 *  @brief  Location in file header for number of bytes in optional header.
 */
#define     SIZE_OPT_HDR_LOC                   16u

/*!
 *  @brief  Length of name.
 */
#define     COFF_NAME_LEN                      8u

/*!
 *  @brief  Offset in file header where symbol table details are present.
 */
#define     SYMTAB_OFFSET                      8u

/*!
 *  @brief  Offset in file header where number of sections is present.
 */
#define     NUM_SECT_OFFSET                    2u

/*!
 *  @brief  Size of file header, symbolEntry and sectionHeader structure in
 *          COFF file format.
 */
#define     SIZE_COFF_FILE_HEADER             22u
#define     SIZE_COFF_SYMBOL_ENTRY            18u
#define     SIZE_COFF_SECTION_HEADER          48u


/*!
 *  @brief   Enumerates the different types of COFF loaders supported
 */
typedef enum {
    Coff_LoaderType_File       = 0u,
    /* !< File based COFF loader */
    Coff_LoaderType_Shm        = 1u,
    /* !< COFF loader optimized for shared memory devices */
    Coff_LoaderType_Mem        = 2u,
    /* !< COFF loader that works off a memory buffer instead of file accesses */
    Coff_LoaderType_EndValue   = 3u
    /*!< End delimiter indicating start of invalid values for this enum */
} Coff_LoaderType;

/*!
 *  @brief  File header for a COFF file.
 */
typedef struct Coff_FileHeader_tag {
    UInt16  version;
    /*!< Version ID. indicates the version of the COFF file structure. */
    UInt16  numSections;
    /*!< Number of section headers */
    Int32   dateTime;
    /*!< Time and date stamp. indicates when the file was created. */
    Int32   fpSymTab;
    /*!< Symbol table's starting location in file. */
    Int32   numSymTabEntries;
    /*!< Number of entries in the symbol table. */
    UInt16  numBytesOptHeader;
    /*!< Number of bytes in the optional header. This field is
         either 0 or 28. If it is 0, there is no optional file header. */
    UInt16  flags;
    /*!< Flags (see the File Header Flags table). */
    UInt16  targetId;
    /*!< Target ID. magic number indicates the file can be executed
         in a particular system. This field is checked for validating
         the support of supplied file. */
} Coff_FileHeader;

/*!
 *  @brief  Optional header for coff file format.
 */
typedef struct Coff_OptHeader_tag {
    Int16 magic;
    /*!< Optional file header magic number */
    Int16 version;
    /*!< Version stamp. */
    Int32 sizeExeCode;
    /*!< Size (in bytes) of executable code. */
    Int32 sizeInitData;
    /*!< Size (in bytes) of initialized data. */
    Int32 sizeUninitData;
    /*!< Size (in bytes) of uninitialized data. */
    Int32 entry;
    /*!< Entry point. */
    Int32 addrExe;
    /*!< Beginning address of executable code. */
    Int32 addrInitData;
    /*!< Beginning address of initialized data. */
} Coff_OptHeader;

/*!
 *  @brief  Section header for COFF file format.
 */
typedef struct Coff_SectionHeader_tag {
    Char   name [COFF_NAME_LEN];
    /*!< This field contains one of the following:
         1) An 8-character section name, padded with nulls, or
         2) A pointer into the string table if the section name is longer than
         8 characters.
         In the latter case the first four bytes of the field are 0. */
    Int32   physicalAddress;
    /*!< Section's physical address. */
    Int32   virtualAddress;
    /*!< Section's virtual address. */
    Int32   size;
    /*!< Section's size in bytes. */
    Int32   fpRawData;
    /*!< File pointer to raw data. */
    Int32   fpReloc;
    /*!< File pointer to relocation entries. */
    Int32   fpLineNum;
    /*!< File pointer to line-number entries. */
    UInt32  numReloc;
    /*!< Number of relocation entries. */
    UInt32  numLine;
    /*!< Number of line-number entries. */
    UInt32  flags;
    /*!< Flags (see the Section Header Flags table) */
    UInt16  reserved;
    /*!< Reserved. */
    UInt16  memPageNum;
    /*!< Memory page number. */
    Bool    isLoadSection;
    /*!< Flag to indicate that the section is loadable. */
    Char * data;
    /*!< Buffer to hold data. */
} Coff_SectionHeader;

/*!
 *  @brief  Defines the structure for a symbol table entry.
 */
typedef struct Coff_SymbolEntry_tag {
    Char   name [COFF_NAME_LEN];
    /*!< This field contains one of the following:
         1) An 8-character symbol name, padded with nulls, or
         2) A pointer into the string table if the symbol name is longer than
         8 characters.
         In the latter case the first four bytes of the field are 0. */
    Int32   value;
    /*!< Symbol value; storage class dependent. */
    Int16   sectNum;
    /*!< Section number of the symbol. */
    UInt16  type;
    /*!< Basic and derived type specification. */
    Char   storage;
    /*!< Storage class of the symbol. */
    Char   numAuxEnt;
    /*!< Number of auxiliary entries (always 0 or 1). If this is '1' then this
         structure is followed by the Auxilliary entry structure (which is of
         the same size as this structure). */
} Coff_SymbolEntry;

/*!
 *  @brief  Defines parameters structure for Coff module.
 *
 *          A pointer to this structure is passed during the load API into
 *          as the params.
 */
typedef struct Coff_Params_tag {
    Coff_LoaderType      loaderType;
    /*!< Type of loader */
    Processor_Endian     endian;
    /*!< Endianism of the slave */
    UInt32               maduSize;
    /*!< Minimum Addressable Data Unit of the slave processor */
    UInt32               fileAddr;
    /*!< Host physical address from where the COFF file is to be read. */
    UInt32               size;
    /*!< Size of .out file. */
    Processor_ProcArch   procArch;
    /*!< Processor architecture */
} Coff_Params ;


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/* Loads the specified executable on the slave processor and returns a file ID
 * with which the file can be identified.
 */
Int Coff_load (Loader_Handle handle,
               Ptr           fileDesc,
               UInt32        argc,
               String *      argv,
               Coff_Params * params,
               UInt32 *      fileId);

/* Function to load symbols for the specified file. This will allow the symbols
 * to be usable when linked against another object file.
 * External symbols will be made available for global symbol linkage.
 * Not implemented for Coff Parser.
 */
Int Coff_loadSymbols (Loader_Handle handle, Ptr fileDesc);

/* Function to unload the previously loaded file on the slave processor. */
Int Coff_unload (Loader_Handle handle, UInt32 fileId);

/* Function to retrieve the target address of a symbol from the specified
 * file.
 */
Int Coff_getSymbolAddress (Loader_Handle handle,
                           UInt32        fileId,
                           String        symName,
                           UInt32 *      symValue);

/* Function to retrieve the entry point of the specified file. */
Int Coff_getEntryPt (Loader_Handle handle,
                     UInt32        fileId,
                     UInt32 *      entryPt);
/* Function that returns section information given the name of section and
 * number of bytes to read
 */
Int Coff_getSectionInfo (Loader_Handle         handle,
                         UInt32                fileId,
                         String                sectionName,
                         ProcMgr_SectionInfo * sectionInfo);

/* Function that returns section data in a buffer given section info retrieved
 * via previous call to Coff_getSectionInfo.
 */
Int Coff_getSectionData (Loader_Handle         handle,
                         UInt32                fileId,
                         ProcMgr_SectionInfo * sectionInfo,
                         Ptr                   buffer);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* Coff_H_0xcb32 */
