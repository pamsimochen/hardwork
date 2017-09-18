/** 
 *  @file   _CoffLoader.h
 *
 *  @brief      Internal header for loader for COFF format.
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



#ifndef _CoffLoader_H_0x1bc4
#define _CoffLoader_H_0x1bc4


/* Module headers */
#include <ti/syslink/inc/knl/LoaderDefs.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 *  See CoffLoader.h, LoaderDefs.h
 * =============================================================================
 */


/* =============================================================================
 *  Internal functions accessed through function table
 * =============================================================================
 */
/* Function to attach to the Loader. */
Int CoffLoader_attach (Loader_Handle handle, Loader_AttachParams * params);

/* Function to detach from the Loader. */
Int CoffLoader_detach (Loader_Handle handle);

/* Function to load the slave processor.
 * Returns file ID in the passed fileId parameter.
 */
Int CoffLoader_load (Loader_Handle   handle,
                     String          imagePath,
                     UInt32          argc,
                     String *        argv,
                     Ptr             params,
                     UInt32 *        fileId);

/* Function to load symbols for the specified file. This will allow the symbols
 * to be usable when linked against another object file.
 * External symbols will be made available for global symbol linkage.
 * NOTE: This function is only relevant for dynamic loader. Other loaders can
 *       return CoffLoader_E_NOTIMPL.
 */
Int CoffLoader_loadSymbols (Loader_Handle handle,
                            String        imagePath,
                            Ptr           params);

/* Function to unload the previously loaded file on the slave processor.
 * The fileId received from the load function must be passed to this
 * function.
 */
Int CoffLoader_unload (Loader_Handle handle, UInt32 fileId);

/* Function to retrieve the target address of a symbol from the specified file.
 * The fileId received from the load function must be passed to this
 * function.
 */
Int CoffLoader_getSymbolAddress (Loader_Handle   handle,
                                 UInt32          fileId,
                                 String          symName,
                                 UInt32 *        symValue);

/* Function to retrieve the entry point of the specified file.
 * The fileId received from the load function must be passed to this
 * function.
 */
Int CoffLoader_getEntryPt (Loader_Handle   handle,
                           UInt32          fileId,
                           UInt32 *        entryPt);
/* Function that returns section information given the name of section and
 * number of bytes to read
 */
Int CoffLoader_getSectionInfo (Loader_Handle         handle,
                               UInt32                fileId,
                               String                sectionName,
                               ProcMgr_SectionInfo * sectionInfo);

/* Function that returns section data in a buffer given sectionInfo data
 * returned from previous call to CoffLoader_getSectionInfo.
 */
Int CoffLoader_getSectionData (Loader_Handle         handle,
                               UInt32                fileId,
                               ProcMgr_SectionInfo * sectionInfo,
                               Ptr                   buffer);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* _CoffLoader_H_0x1bc4 */
