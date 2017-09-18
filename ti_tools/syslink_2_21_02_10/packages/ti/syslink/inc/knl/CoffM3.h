/** 
 *  @file   CoffM3.h
 *
 *  @brief      Defines interface for CM3 specific functions of COFF parser.
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



#ifndef CoffM3_H_0xcb32
#define CoffM3_H_0xcb32


/* Module headers */
#include "LoaderDefs.h"
#include "Coff.h"


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @brief  Magic number to identify M3 COFF file format.
 */
#define     COFF_MAGIC_M3                        0x0097u


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/* Checks if the fields of headers are stored as byte swapped values in M3
 * file format.
 */
Bool Coff_isSwapped_M3 (Loader_Object * obj, Ptr fileDesc);

/* Checks if the file data format is valid for M3 architecture. */
Bool Coff_isValidFile_M3 (Loader_Object * obj, Ptr fileDesc);

/* Fills up the specified buffer with arguments to be sent to slave's "main"
 * function for the M3 architecture.
 */
Int Coff_fillArgsBuffer_M3 ( Processor_ProcArch procArch,
                             UInt32             argc,
                             Char **            argv,
                             UInt32             sectSize,
                             UInt32             loadAddr,
                             UInt32             wordSize,
                             Processor_Endian   endian,
                             Ptr                argsBuf);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* CoffM3_H_0xcb32 */
