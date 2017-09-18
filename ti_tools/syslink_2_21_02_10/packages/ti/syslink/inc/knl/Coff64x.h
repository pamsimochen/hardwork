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
 *  @file   ti/syslink/inc/knl/Coff64x.h
 *
 *  @brief      Defines interface for C64x specific functions of COFF parser.
 */

#ifndef Coff64x_H_0xcb32
#define Coff64x_H_0xcb32


/* Module headers */
#include "LoaderDefs.h"
#include "Coff.h"


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @brief  Magic number to identify 64x COFF file format.
 */
#define     COFF_MAGIC_64x                        0x0099u


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/* Checks if the fields of headers are stored as byte swapped values in 64x
 * file format.
 */
Bool Coff_isSwapped_64x (Loader_Object * obj, Ptr fileDesc);

/* Checks if the file data format is valid for 64x architecture. */
Bool Coff_isValidFile_64x (Loader_Object * obj, Ptr fileDesc);

/* Fills up the specified buffer with arguments to be sent to slave's "main"
 * function for the 64x architecture.
 */
Int Coff_fillArgsBuffer_64x (Processor_ProcArch procArch,
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

#endif /* Coff64x_H_0xcb32 */
