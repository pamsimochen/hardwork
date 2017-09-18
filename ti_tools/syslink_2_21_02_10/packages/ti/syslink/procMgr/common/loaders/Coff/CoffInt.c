/*
 *  @file   CoffInt.c
 *
 *  @brief      Defines generic functions of COFF loader.
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
#endif /* #if defined(SYSLINK_BUILD_HLOS) */

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
#endif /* #if defined(SYSLINK_BUILD_RTOS) */

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module level headers */
#include <ti/syslink/inc/knl/Loader.h>
#include <ti/syslink/inc/knl/LoaderDefs.h>
#include <ti/syslink/inc/knl/CoffInt.h>

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*!
 *  @brief      Function to read an Int8 from file.
 *
 *  @param      obj       Pointer to the Loader instance
 *  @param      fileDesc  File descriptor
 *
 *  @sa         Coff_read16, Coff_read32
 */
Int8
Coff_read8 (Loader_Object * obj, Ptr fileDesc)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int  status = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Int8 retVal = 0 ;

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    (*(obj->fileFxnTable.read)) (obj->object,
                                 fileDesc,
                                 (Char *) &retVal,
                                 sizeof (Int8),
                                 READ_REC_SIZE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_read8",
                             status,
                             "Failed to read 8-bit value from file!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /*! @retval Value Read 8-bit value*/
    return retVal ;
}


/*!
 *  @brief      Function to read an Int16 from file.
 *
 *  @param      obj       Pointer to the Loader instance
 *  @param      fileDesc  File descriptor
 *  @param      swap      Indicates whether contents are to be swapped
 *
 *  @sa         COFF_read8, Coff_read32
 */
Int16
Coff_read16 (Loader_Object * obj, Ptr fileDesc, Bool swap)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int   status = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Int16 retVal = 0 ;

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    (*(obj->fileFxnTable.read)) (obj->object,
                                 fileDesc,
                                 (Char *) &retVal,
                                 sizeof (Int16),
                                 READ_REC_SIZE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_read16",
                             status,
                             "Failed to read 16-bit value from file!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (swap == TRUE) {
            retVal = BYTESWAP_WORD (retVal);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /*! @retval Value Read 16-bit value*/
    return retVal ;
}


/*!
 *  @brief      Function to read an Int32 from file.
 *
 *  @param      obj       Pointer to the Loader instance
 *  @param      fileDesc  File descriptor
 *  @param      swap      Indicates whether contents are to be swapped
 *
 *  @sa         COFF_read8, Coff_read16
 */
Int32
Coff_read32 (Loader_Object * obj, Ptr fileDesc, Bool swap)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int   status = LOADER_SUCCESS;
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    Int32 retVal = 0 ;

    GT_assert (curTrace, (obj      != NULL));
    GT_assert (curTrace, (fileDesc != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    status =
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
    (*(obj->fileFxnTable.read)) (obj->object,
                                 fileDesc,
                                 (Char *) &retVal,
                                 sizeof (Int32),
                                 READ_REC_SIZE);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Coff_read32",
                             status,
                             "Failed to read 32-bit value from file!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (swap == TRUE) {
            retVal = BYTESWAP_LONG (retVal);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    /*! @retval Value Read 32-bit value*/
    return retVal ;
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
