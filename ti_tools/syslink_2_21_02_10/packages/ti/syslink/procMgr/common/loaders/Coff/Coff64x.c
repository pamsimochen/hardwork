/*
 *  @file   Coff64x.c
 *
 *  @brief      Defines C64x specific functions of COFF loader.
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
/* Standard headers */
#include <ti/syslink/Std.h>

#if defined (__KERNEL__)
#include <linux/string.h>
#else
/* Standard headers */
#include <string.h>
#endif /* #if defined (__KERNEL__) */
#endif /* #if defined(SYSLINK_BUILD_HLOS) */

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
/* Standard headers */
#include <string.h>
#endif /* #if defined(SYSLINK_BUILD_RTOS) */
/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module level headers */
#include <ti/syslink/inc/knl/Coff64x.h>
#include <ti/syslink/inc/knl/Coff.h>
#include <ti/syslink/inc/knl/CoffInt.h>
#include <ti/syslink/inc/CoffLoader.h>
#include <ti/syslink/inc/knl/Loader.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief      Size of the arg pointer
 */
#define  ARG_PTR_SIZE 4u


/* =============================================================================
 *  Functions
 * =============================================================================
 */
/*!
 *  @brief      Function to check if the file data format is valid for 64x
 *              architecture.
 *
 *  @param      obj       Handle to the Loader instance
 *  @param      fileDesc  Descriptor of the file to be loaded
 *
 *  @sa         Coff_isValidFile
 */
Bool
Coff_isValidFile_64x (Loader_Object * obj, Ptr fileDesc)
{
    /*! @retval FALSE File is not a valid C64x COFF file. */
    Bool    isValidFile = FALSE;
    Bool    isValidId   = FALSE;
    Bool    isValidVer  = FALSE;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int     status      = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
    Int16   swapVal;
    UInt16  version;

    GT_2trace (curTrace, GT_ENTER, "Coff_isValidFile_64x", obj, fileDesc);

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

    /* This is an internal function, so parameter validation is not done. */

    /* Seek to the start of the file.*/
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
    (*(obj->fileFxnTable.seek)) (obj->object,
                                 fileDesc,
                                 0,
                                 LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_isValidFile_64x",
                             status,
                             "Failed to seek to start of file!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        version = (UInt16) Coff_read16 (obj, fileDesc, FALSE);

        if (version != COFF_VERSION) {
            if (BYTESWAP_WORD (version) != COFF_VERSION) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                status = LOADER_E_FILE ;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Coff_isValidFile_64x",
                                     status,
                                     "Failed to read version from file!");
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

            }
            else {
                isValidVer = TRUE;
            }
        }
        else {
            isValidVer = TRUE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        /* Seek to the swap location. */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        (*(obj->fileFxnTable.seek)) (obj->object,
                                     fileDesc,
                                     SWAP_LOCATION,
                                     LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Coff_isValidFile_64x",
                                 status,
                                 "Failed to seek to swap location!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
            swapVal = Coff_read16 (obj, fileDesc, FALSE);
            if (swapVal != COFF_MAGIC_64x) {
                if (BYTESWAP_WORD (swapVal) != COFF_MAGIC_64x) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    status = LOADER_E_FILE ;
                    GT_setFailureReason (curTrace,
                                        GT_4CLASS,
                                        "Coff_isValidFile_64x",
                                        status,
                                        "Swap value read is corrupt!");
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
                }
                else {
                    isValidId = TRUE ;
                }
            }
            else {
                isValidId = TRUE ;
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        isValidFile = ((TRUE == isValidId) && (TRUE == isValidVer)) ;
        GT_1trace (curTrace, GT_4CLASS, "is Valid File: %d\n", isValidFile);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

    GT_1trace (curTrace, GT_LEAVE, "Coff_isValidFile_64x", isValidFile);

    /*! @retval TRUE File is a valid C64x COFF file. */
    return isValidFile ;
}


/*!
 *  @brief      Function to check if the file data is swapped for 64x
 *              architecture.
 *
 *  @param      obj       Handle to the Loader instance
 *  @param      fileDesc  Descriptor of the file to be loaded
 *
 *  @sa         Coff_isSwapped
 */
Bool
Coff_isSwapped_64x (Loader_Object * obj, Ptr fileDesc)
{
    /*! @retval FALSE File headers are not swapped. */
    Bool    isSwapped = FALSE;
    Int16   swapVal;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int     status      = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

    GT_2trace (curTrace, GT_ENTER, "Coff_isSwapped_64x", obj, fileDesc);

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

    /* This is an internal function, so parameter validation is not done. */

    /* Seek to the start of the file.*/
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
    (*(obj->fileFxnTable.seek)) (obj->object,
                                 fileDesc,
                                 SWAP_LOCATION,
                                 LoaderFile_Pos_SeekSet);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_isSwapped_64x",
                             status,
                             "Failed to seek to swap location!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        swapVal = Coff_read16 (obj, fileDesc, FALSE);

        if (swapVal != COFF_MAGIC_64x) {
            if (BYTESWAP_WORD (swapVal) != COFF_MAGIC_64x) {
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                status = LOADER_E_FILE ;
                GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "Coff_isSwapped_64x",
                                    status,
                                    "Swap value read is corrupt!");
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
            }
            else {
                isSwapped = TRUE;
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status >= 0) {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */
        GT_1trace (curTrace, GT_4CLASS, "Swapped: %d\n", isSwapped);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE)  */

    GT_1trace (curTrace, GT_LEAVE, "Coff_isSwapped_64x", isSwapped);

    /*! @retval TRUE File headers are swapped. */
    return isSwapped;
}


/*!
 *  @brief      Function to allocate and fill up a buffer with arguments to be
 *              sent to slave processor's "main" function for the 64x
 *              architecture.
 *
 *              The .args buffer for Dsp/Bios 5.xx has the form:
 *                  argc,
 *              +-- argv,
 *              |   envp
 *              +-->argv[0], -------+
 *                  argv[1],        |
 *                  ...,            |
 *                  argv[n],        |
 *                  argv 0 string,<-+
 *                  argv 1 string,
 *                  ...
 *                  argv n string,
 *
 *              The envp pointer needs to be populated for the arguments to be
 *              passed correctly to the DSP executable's 'main'.
 *              However, currently this is not required.
 *
 *              The .args buffer for Dsp/Bios 6.xx has the form:
 *              argc,
 *              argv[0], -------+
 *              argv[1],        |
 *              ...,            |
 *              argv[n],        |
 *              argv 0 string,<-+
 *              argv 1 string,
 *              ...
 *              argv n string,
 *
 *              New arguments do not support the environment pointer.
 *
 *  @param      obj       Handle to the Loader instance
 *  @param      fileDesc  Descriptor of the file to be loaded
 *
 *  @sa         Coff_isSwapped
 */
Int
Coff_fillArgsBuffer_64x (Processor_ProcArch procArch,
                         UInt32             argc,
                         Char **            argv,
                         UInt32             sectSize,
                         UInt32             loadAddr,
                         UInt32             wordSize,
                         Processor_Endian   endian,
                         Ptr                argsBuf)
{
    Int     status    = LOADER_SUCCESS;
    Int32 * bufPtr    = NULL;
    Char *  srcPtr    = NULL;
    Char *  dstPtr    = NULL;
    Int32 * argvPtr   = NULL;
    UInt32  totalReqd;
    UInt32  length;
    UInt32  i;

    GT_5trace (curTrace, GT_ENTER, "Coff_fillArgsBuffer_64x",
               procArch, argc, argv, sectSize, loadAddr);

    GT_assert (curTrace, (procArch < Processor_ProcArch_EndValue));

    /* If this function is called, it means that args are provided.*/
    GT_assert (curTrace, (argc > 0u));
    GT_assert (curTrace, (argv != NULL));
    GT_assert (curTrace, (sectSize != 0u));
    GT_assert (curTrace, (loadAddr != 0u));
    GT_assert (curTrace, (endian < Processor_Endian_EndValue));
    GT_assert (curTrace, (argsBuf != NULL));

    /*  ------------------------------------------------------------------------
     *  Compute total target buffer size required (in bytes). Memory requirement
     *  is calculated as:
     *      1 units for argc.
     *      argc units for storing the argv pointers.
     *  variables are not being passed, so it is set as NULL.
     *  ------------------------------------------------------------------------
     */
    /* For Coff_ProcArch_C64x */
    totalReqd = (UInt32) ((1 + argc ) * ARG_PTR_SIZE);

    for (i = 0 ; (i < argc) ; i++) {
        GT_assert (curTrace, (argv [i] != NULL));

        length = strlen (argv [i]);
        totalReqd += (length + 1);
        /* Extend to the next wordsize */
        totalReqd =  ((totalReqd + (ARG_PTR_SIZE - 1)) / ARG_PTR_SIZE)
                   * ARG_PTR_SIZE;
    }

    if (totalReqd > sectSize) {
        status = COFFLOADER_E_SIZE ;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_fillArgsBuffer_64x",
                             status,
                             "Insufficient size for args section!");
    }
    else {
        bufPtr = (Int32 *) argsBuf;

        /*  --------------------------------------------------------------------
         *  Initialize the argument buffer with 0.
         *  --------------------------------------------------------------------
         */
        srcPtr = (Char *) argsBuf;
        for (i = 0 ; i < sectSize ; i++) {
            *srcPtr++ = 0;
        }

        /*  --------------------------------------------------------------------
         *  Fill the buffer according to the required format.
         *  --------------------------------------------------------------------
         */
        *bufPtr++ = (Int32) argc;

        argvPtr = bufPtr; /* Pointer to argv[0] */
        bufPtr  += argc;  /* Space for argv pointers */

        for ( ; argc > 0 ; argc--) {
            *argvPtr++ = (Int32) (  (  ((Char *) ((Ptr) bufPtr))
                                     - ((Char *) ((Ptr) argsBuf)))
                                  + loadAddr);

            srcPtr = *argv++;
            dstPtr = (Char *) ((Ptr) bufPtr);
            while ((*srcPtr) != '\0') {
                *dstPtr = (Char)(*srcPtr);
                dstPtr++;
                srcPtr++;
            }
            *dstPtr++ = (Char) '\0';
            bufPtr    = (Int32 *) ((Ptr) dstPtr);
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Coff_fillArgsBuffer_64x", status);

    /*! @retval LOADER_SUCCESS Operation successfully completed. */
    return status ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
