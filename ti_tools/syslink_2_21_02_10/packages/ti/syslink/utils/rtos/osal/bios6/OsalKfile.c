/*
 *  @file   OsalKfile.c
 *
 *  @brief      BIOS File operation interface implementation.
 *
 *              This abstracts the file operation interface in the BIOS
 *              code. All the usual operations like open, close, read, write,
 *              seek and tell are supported.
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



/*-------------------------   Standard headers   ---------------------------  */
#include <stdio.h>
/*-------------------------   XDC headers   --------------------------------  */
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/hal/Hwi.h>

/*-------------------------   OSAL and utils   -----------------------------  */
#include <ti/syslink/inc/knl/OsalKfile.h>
#include <ti/syslink/utils/Trace.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief   Definition of the OsalKfile object used by all OsalKfile functions.
 */
typedef struct OsalKfile_Object_tag {
    UInt32        signature; /*!<  Signature of the OsalKfile_Object object.*/
    FILE *        fileDesc;  /*!< File descriptor pointer */
    Char *        fileName;  /*!< Name of the file used */
    UInt32        size;      /*!< Size of the file */
    UInt32        curPos;    /*!< Current position of file read pointer */
} OsalKfile_Object;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*
 * ======== OsalKfile_open ========
 */
Int
OsalKfile_open (String             fileName,
                Char *             fileMode,
                OsalKfile_Handle * fileHandle)
{
    Int                 status      = OSALKFILE_SUCCESS;
    FILE *              fileDesc    = NULL;
    OsalKfile_Object *  fileObject  = NULL;


    GT_3trace (curTrace, GT_ENTER, "OsalKfile_open",
               fileName, fileMode, fileHandle);

    GT_assert (curTrace, (fileName != NULL));
    GT_assert (curTrace, (fileHandle != NULL));
    GT_assert (curTrace, (fileMode != NULL) && (fileMode [0] == 'r'));

    *fileHandle = NULL;
    fileObject = Memory_alloc (NULL, sizeof (OsalKfile_Object), 0, NULL);
    if (fileObject == NULL) {
        status = OSALKFILE_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalKfile_open",
                             status,
                             "Failed to allocate memory!");
    }

    GT_assert (curTrace, (fileObject != NULL));

    fileDesc = fopen(fileName, fileMode);
    if (fileDesc == NULL) {
        status = OSALKFILE_E_FILEOPEN;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalKfile_open",
                             status,
                             "Failed to open file!");
    }

    GT_assert (curTrace, (fileDesc != NULL));

    fileObject->fileDesc = fileDesc;
    fileObject->fileName = fileName;
    fileObject->curPos = 0;

    *fileHandle = (OsalKfile_Handle) fileObject;
    fseek (fileDesc, 0, SEEK_END);
    fileObject->size = ftell (fileDesc);

    fseek (fileDesc, 0, SEEK_SET);

    GT_1trace (curTrace, GT_LEAVE, "OsalKfile_open", status);

    /*! @retval OSALKFILE_SUCCESS Operation successfully completed. */
    return (status);
}

/*
 * ======== OsalKfile_close ========
 */
Int
OsalKfile_close (OsalKfile_Handle * fileHandle)
{
    Int                 status      = OSALKFILE_SUCCESS;
    OsalKfile_Object *  fileObject  = NULL;

    GT_1trace (curTrace, GT_ENTER, "OsalKfile_close", fileHandle);
    GT_assert (curTrace, (fileHandle != NULL) && (*fileHandle != NULL));

    fileObject = (OsalKfile_Object *) *fileHandle;
    fclose (fileObject->fileDesc);

    Memory_free (NULL, fileObject, sizeof(OsalKfile_Object));
    /* Reset user's file handle pointer. */
    *fileHandle = NULL;

    GT_1trace (curTrace, GT_LEAVE, "OsalKfile_close", status);
    /*! @retval OSALKFILE_SUCCESS Operation successfully completed. */
    return (status);
}


/*
 * ======== OsalKfile_read ========
 */
Int OsalKfile_read(OsalKfile_Handle fileHandle, Char *buffer, UInt32 size,
        UInt32 count, UInt32 *numBytes)
{
    Int                 status      = OSALKFILE_SUCCESS;
    UInt32              elementsRead   = 0;
    OsalKfile_Object*   fileObject  = (OsalKfile_Object*) fileHandle;

    GT_4trace (curTrace, GT_ENTER, "OsalKfile_read",
               fileHandle, buffer, size, count);

    GT_assert (curTrace, (fileHandle != NULL));
    GT_assert (curTrace, (buffer != NULL));
    GT_assert (curTrace, (size != 0));
    GT_assert (curTrace, (count != 0));
    GT_assert (curTrace, (numBytes != NULL));

    fileObject = (OsalKfile_Object*) fileHandle;
    GT_assert (curTrace,
              ((fileObject->curPos + (size * count)) <= fileObject->size));

    elementsRead = fread(buffer, size, count, fileObject->fileDesc);

    *numBytes = elementsRead;

    if (elementsRead == 0) {
        status = OSALKFILE_E_FILEREAD;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OsalKfile_read",
                             status,
                             "Failed to read from the file.");
    }
    GT_assert (curTrace, (elementsRead != 0));

    fileObject->curPos += elementsRead * size;
    GT_assert (curTrace, (elementsRead == (UInt32) count));


    GT_1trace(curTrace, GT_LEAVE, "OsalKfile_read", status);

    return (status);
}


/*
 * ======== OsalKfile_seek ========
 */
Int
OsalKfile_seek (OsalKfile_Handle fileHandle,
                Int32            offset,
                OsalKfile_Pos    pos)
{
    Int                 status      = OSALKFILE_SUCCESS;
    FILE *       fileDesc    = NULL;
    OsalKfile_Object *  fileObject  = NULL;

    GT_3trace (curTrace, GT_ENTER, "OsalKfile_seek", fileHandle, offset, pos);

    GT_assert (curTrace, (fileHandle != NULL));
    GT_assert (curTrace, (pos < OsalKfile_Pos_EndValue));

    fileObject = (OsalKfile_Object* ) fileHandle;
    fileDesc = fileObject->fileDesc;
    switch (pos) {

        case OsalKfile_Pos_SeekSet:
            {
                if ((offset < 0) || (offset > fileObject->size)) {
                    /*! @retval OSALKFILE_E_OUTOFRANGE offset is out of range
                                             for seek from the specified pos */
                    status = OSALKFILE_E_OUTOFRANGE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OsalKfile_seek",
                                         status,
                                         "offset is out of range for"
                                         " seek from the specified pos");
                }
                else {
                     fseek (fileDesc, offset, SEEK_SET);
                     fileObject->curPos = ftell (fileDesc);
                }
            }
            break;

            case OsalKfile_Pos_SeekCur:
            {
                if (   ((fileObject->curPos + offset) > fileObject->size)
                || (((Int32)fileObject->curPos + offset) < 0)) {
                    /*! @retval OSALKFILE_E_OUTOFRANGE offset is out of range
                                             for seek from the specified pos */
                    status = OSALKFILE_E_OUTOFRANGE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OsalKfile_seek",
                                         status,
                                         "offset is out of range for"
                                         " seek from the specified pos");
                }
                else {
                     fseek (fileDesc, offset, SEEK_CUR);
                     fileObject->curPos = ftell (fileDesc);
                }
            }
            break;

            case OsalKfile_Pos_SeekEnd:
            {
                /* A negative offset indicates offset from the end of file.
                 * Check that the specified offset is not beyond
                 * the bounds of the file.
                 */
                if ((-offset < 0) || (-offset > fileObject->size)) {
                    /*! @retval OSALKFILE_E_OUTOFRANGE offset is out of range
                                             for seek from the specified pos */
                    status = OSALKFILE_E_OUTOFRANGE;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OsalKfile_seek",
                                         status,
                                         "offset is out of range for"
                                         " seek from the specified pos");
                }
                else {
                     fseek (fileDesc, offset, SEEK_END);
                     fileObject->curPos = ftell (fileDesc);
                }
            }
            break;

            default:
            {
                /*! @retval OSALKFILE_E_INVALIDARG Invalid value provided for
                                                   argument pos. */
                status = OSALKFILE_E_INVALIDARG;
                GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "OsalKfile_seek",
                                    status,
                                    "Invalid value provided for argument pos.");
            }
            break;
        }

    GT_1trace (curTrace, GT_LEAVE, "OsalKfile_seek", status);

    /*! @retval OSALKFILE_SUCCESS Operation successfully completed. */
    return status;
}


/*
 * ======== OsalKfile_tell ========
 */
UInt32
OsalKfile_tell (OsalKfile_Handle fileHandle)
{
    OsalKfile_Object *  fileObject  = NULL;
    UInt32              posValue    = 0u;
    UInt32              tempPos     = 0u;

    (Void) tempPos; /* to avoid warning */
    GT_1trace (curTrace, GT_ENTER, "OsalKfile_tell", fileHandle);
    GT_assert (curTrace, (fileHandle != NULL));

    fileObject = (OsalKfile_Object*) fileHandle;
    posValue = fileObject->curPos;
    tempPos  = ftell (fileObject->fileDesc);
    GT_assert (GT_1CLASS, (posValue == tempPos));

    /* !< @retval File-position Current file pointer position in file.*/
    return posValue;
}
