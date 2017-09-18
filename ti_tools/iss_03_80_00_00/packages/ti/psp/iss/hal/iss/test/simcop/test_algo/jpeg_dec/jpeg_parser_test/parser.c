/** ==================================================================
 *  @file   parser.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/test/simcop/test_algo/jpeg_dec/jpeg_parser_test/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "parser.h"

Uint16 hmem[4096];

Uint16 qmem[512];

#define QMEM_SIZE      256                                 /* 512 bytes = 256 
                                                            * short */
#define HMEM_SIZE 4096

#define EOB_VAL 0x00
#define ZRL_VAL 0xF0

#define BREAK_LONG_LINES 0

/* ===================================================================
 *  @func     write_hex_short                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void write_hex_short(char *filename, Uint16 * image, int width, int height)
{
    int x, y;

    FILE *ff;

    /* open a file for writing */
    ff = fopen(filename, "w");

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            fprintf(ff, "%04X ", image[y * width + x]);
            if (BREAK_LONG_LINES && (x % 16 == 15))
                fprintf(ff, "\n");
        }
        if (BREAK_LONG_LINES && width > 16)
            fprintf(ff, "\n");
        fprintf(ff, "\n");
    }

    fclose(ff);
}

/* ======================================================================= */
/**
*  @fn BigEndianRead32Bits() gets a 32-bit value into a pointer variable
*
*  @param                 None
*
*  @pre                   None
*
*  @post                  None
*
*  @return                None
*/
/* ======================================================================= */
/* ===================================================================
 *  @func     BigEndianRead32Bits                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 BigEndianRead32Bits(Uint32 * pVal, Uint8 * ptr)
{

    *pVal = (Uint32) ((*(ptr + 3)));

    *pVal |= (Uint32) ((*(ptr + 2)) << 8);

    *pVal |= (Uint32) ((*(ptr + 1)) << 24);

    *pVal |= (Uint32) ((*(ptr + 1)) << 16);

}

/* ======================================================================= */
/**
*  @fn BigEndianRead16Bits() gets a 16-bit value into a pointer variable
*
*  @param                 None
*
*  @pre                   None
*
*  @post                  None
*
*  @return                None
*/
/* ======================================================================= */
/* ===================================================================
 *  @func     BigEndianRead16Bits                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 BigEndianRead16Bits(Uint16 * pVal, Uint8 * ptr)
{
    *pVal = *ptr;
    *pVal <<= 8;
    *pVal |= *(ptr + 1);
}

/* ===================================================================
 *  @func     InterpretDQT                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Jpeg_Dec_status InterpretDQT(Uint8 * pBuf, Uint16 * qmat_len,
                             DecTblParams * DecTblParam)
{

    Jpeg_Dec_status status = JPEG_DEC_SOK;

    if (*qmat_len > 66)
    {
        if (pBuf[0] == 0)
            memcpy(&DecTblParam->qmat[0], &pBuf[1], 64);
        if (pBuf[65] == 1)
            memcpy(&DecTblParam->qmat[64], &pBuf[66], 64);
    }

    else
    {
        if (pBuf[0] == 0)
            memcpy(&DecTblParam->qmat[0], &pBuf[1], 64);
        else
            memcpy(&DecTblParam->qmat[64], &pBuf[1], 64);

    }

  EXIT:
    return status;

}

/* ======================================================================= */
/**
*  @fn TI_GetThumbnailInformation() takes two input pointers parses the EXIF
*  part of the bitstream and extracts the EXIF information into the
*  OMX_EXIF_INFO_SUPPORTED data pointer.
*  Returns JPEGOCP_SUCCESS or Error Type.
*
*  @param pBuffer         A pointer to a EXIF Buffer
*  @param pNextIFDOffset  A pointer to start of Thumbnail tags
*  @param pThumbnail      A pointer thumbnail data structure
*                         OMX_EXIF_THUMBNAIL_INFO
*  @param ulLittleEndian  If this value is 1, it suggests the tags are in
*                         little endian format and if 0 it is big endian
*
*  @pre                 This is executed only when bitstream has a 'FF E1'
*                       tag somewhere after 'FF D8'
*
*  @post                None
*
*  @return              JPEGOCP_SUCCESS or Error Type
*/
/* ======================================================================= */
/* ===================================================================
 *  @func     TI_GetThumbnailInformation                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Jpeg_Dec_status TI_GetThumbnailInformation(Uint8 * pBuffer,
                                           Uint32 * pNextIFDOffset,
                                           EXIF_THUMBNAIL_INFO * pThumbnail,
                                           Uint32 ulLittleEndian)
{
    Uint32 TagValOffset, j = 0, TagCount, NextOffset;

    Uint16 nTagValues, Tag, TagType;

    EXIF_THUMBNAIL_INFO *pThumb;

    Jpeg_Dec_status status = JPEG_DEC_SOK;

    memset(pThumbnail, 0, sizeof(EXIF_THUMBNAIL_INFO));
    NextOffset = *pNextIFDOffset;
    pThumb = pThumbnail;

    do
    {
        pBuffer += NextOffset;
        if (ulLittleEndian)
        {
            nTagValues = (Uint16) ((*pBuffer) + (*(pBuffer + 1) << 8));
        }
        else
        {
            BigEndianRead16Bits(&nTagValues, pBuffer);
        }
        pBuffer += sizeof(Uint16);

        for (j = 0; j < nTagValues; j++)
        {
            if (ulLittleEndian)
            {
                Tag = (Uint16) ((*pBuffer) + (*(pBuffer + 1) << 8));
                pBuffer += sizeof(Uint16);

                TagType = (Uint16) ((*pBuffer) + (*(pBuffer + 1) << 8));
                pBuffer += sizeof(Uint16);

                TagCount = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                     (*(pBuffer + 2) << 16) +
                                     (*(pBuffer + 3) << 24));
                pBuffer += sizeof(Uint32);

                TagValOffset = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                         (*(pBuffer + 2) << 16) +
                                         (*(pBuffer + 3) << 24));
                pBuffer += sizeof(Uint32);
            }
            else
            {

                BigEndianRead16Bits(&Tag, pBuffer);
                pBuffer += sizeof(Uint16);

                BigEndianRead16Bits(&TagType, pBuffer);
                pBuffer += sizeof(Uint16);

                BigEndianRead32Bits(&TagCount, pBuffer);
                pBuffer += sizeof(Uint32);

                BigEndianRead32Bits(&TagValOffset, pBuffer);
                pBuffer += sizeof(Uint32);
            }

            switch (Tag)
            {
                case 0x0100:                              /* Thumb
                                                            * Imagewidth */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->ImageWidth = TagValOffset;
                    break;
                case 0x0101:                              /* Thumb
                                                            * ImageHeight */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->ImageHeight = TagValOffset;
                    break;
                case 0x0102:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->BitsPerSampleOffset = TagValOffset;
                    break;
                case 0x0103:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->Compression = TagValOffset;
                    break;
                case 0x0106:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->PhotometricInterpretation = TagValOffset;
                    break;
                case 0x0111:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->StripOffset = TagValOffset;
                    break;
                case 0x0115:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->SamplesPerPixel = TagValOffset;
                    break;
                case 0x0116:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->RowsPerStrip = TagValOffset;
                    break;
                case 0x0117:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->StripByteCount = TagValOffset;
                    break;
                case 0x011C:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->PlanarConfiguration = TagValOffset;
                    break;
                case 0x0201:                              /* Thumbnail
                                                            * JPEGimage
                                                            * offset */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->JPEGInterchangeFormat = TagValOffset;
                    break;

                case 0x0202:                              /* Thumbnail JPEG
                                                            * Image Length */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->JPEGInterchangeFormatLength = TagValOffset;
                    break;
                case 0x0212:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->YCbCrSubSamplingOffset = TagValOffset;
                    break;
                case 0x0213:

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pThumb->YCbCrPositioning = TagValOffset;
                    break;
                default:
                    break;
            }
        }
        if (ulLittleEndian)
        {
            NextOffset = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                   (*(pBuffer + 2) << 16) +
                                   (*(pBuffer + 3) << 24));
        }
        else
        {
            BigEndianRead32Bits(&NextOffset, pBuffer);
        }
        pBuffer += sizeof(Uint32);

    } while (NextOffset != NULL);

    status = JPEG_DEC_SOK;

  EXIT:
    return status;
};

/* ===================================================================
 *  @func     InterpretEXIF                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Jpeg_Dec_status InterpretEXIF(Uint8 * pExifBuffer, Uint16 * exif_len,
                              DecTblParams * DecTblParam)
{
    EXIF_INFO_SUPPORTED *pExif_temp, *pExif;

    Uint8 *pBuffer, *pTagHeader, *pValOffset, *pTiff;

    Uint32 TagValOffset, j = 0, TagCount, SkipFactor, NextIFDOffset;

    Uint32 NextOffset, LastValOffset;

    Uint16 nTagValues, Tag, TagType;

    EXIF_THUMBNAIL_INFO *pThumbnail;

    Jpeg_Dec_status status = JPEG_DEC_SOK;

    Uint32 ulLittleEndian = 0;

    Uint32 BYTES_REQUIREMENT[11] = {
        0, 1, 1, 2, 4, 8, 0, 1, 0, 4, 8
    };

    pExif = &DecTblParam->exif_info;
    pThumbnail = &DecTblParam->exif_info.pThumbnailInfo;
    memset(pExif, 0, sizeof(EXIF_THUMBNAIL_INFO));
    pExif_temp = pExif;
    pBuffer = pExifBuffer;
    if (!(*(pBuffer + 4) == 'E' || *(pBuffer + 4) == 'e') &&
        !(*(pBuffer + 5) == 'X' || *(pBuffer + 5) == 'x') &&
        !(*(pBuffer + 6) == 'I' || *(pBuffer + 6) == 'i') &&
        !(*(pBuffer + 7) == 'F' || *(pBuffer + 7) == 'f'))
    {
        printf("Exif Format no correct. Returning..\n");
        return JPEG_DEC_FAILED;
    }

    pTagHeader = pBuffer + 0xA;
    pTiff = pTagHeader;
    if ((*pTagHeader == 0x49) && (*(pTagHeader + 1) == 0x49))
    {
        ulLittleEndian = 1;
    }

    pBuffer += 0xE;

    if (ulLittleEndian == 1)
    {
        SkipFactor = (Uint32) (*pBuffer);
    }
    else
    {
        BigEndianRead32Bits(&SkipFactor, pBuffer);
    }

    pBuffer = pTagHeader + SkipFactor;                     /* 0th IFD starts
                                                            * at this
                                                            * location */

    LastValOffset = 0;
    NextIFDOffset = 0;
    NextOffset = 0;
    TagValOffset = 0;

    do
    {
        pBuffer += NextOffset;
        NextOffset = 0;

        if (ulLittleEndian == 1)
        {
            nTagValues = (Uint16) ((*pBuffer) + (*(pBuffer + 1) << 8));
        }
        else
        {
            BigEndianRead16Bits(&nTagValues, pBuffer);
        }
        pBuffer += sizeof(Uint16);

        for (j = 0; j < nTagValues; j++)
        {
            if (ulLittleEndian == 1)
            {
                Tag = (Uint16) ((*pBuffer) + (*(pBuffer + 1) << 8));
                pBuffer += sizeof(Uint16);

                TagType = (Uint16) ((*pBuffer) + (*(pBuffer + 1) << 8));
                pBuffer += sizeof(Uint16);

                TagCount = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                     (*(pBuffer + 2) << 16) +
                                     (*(pBuffer + 3) << 24));
                pBuffer += sizeof(Uint32);

                TagValOffset = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                         (*(pBuffer + 2) << 16) +
                                         (*(pBuffer + 3) << 24));
                pBuffer += sizeof(Uint32);

            }
            else
            {
                BigEndianRead16Bits(&Tag, pBuffer);
                pBuffer += sizeof(Uint16);

                BigEndianRead16Bits(&TagType, pBuffer);
                pBuffer += sizeof(Uint16);

                BigEndianRead32Bits(&TagCount, pBuffer);
                pBuffer += sizeof(Uint32);

                BigEndianRead32Bits(&TagValOffset, pBuffer);
                pBuffer += sizeof(Uint32);
            }

            switch (Tag)
            {
                case 0x013B:                              /* Artist */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pArtist = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pArtist, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0x0112:                              /* Orientation */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pExif_temp->Orientation = (Uint16) TagValOffset;
                    break;
                case 0x0131:                              /* Software */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pSoftware = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pSoftware, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0x8298:                              /* Copyright */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pCopyright = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pCopyright, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0x9003:                              /* Create Time */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pCreationDateTime = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pCreationDateTime, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0x0132:                              /* Last Change
                                                            * Date Time */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pLastChangeDateTime = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pLastChangeDateTime, pValOffset,
                           TagCount);

                    pValOffset = 0;
                    break;
                case 0x010E:                              /* ImageDescription */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pImageDescription = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pImageDescription, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0x010F:                              /* Make */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);
                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pMake = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pMake, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0x0110:                              /* Model */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_ASCII),
                                 JPEG_DEC_FAILED);

                    pValOffset = pTagHeader + TagValOffset;
                    pExif_temp->pModel = (char *) malloc(TagCount);
                    memcpy(pExif_temp->pModel, pValOffset, TagCount);

                    pValOffset = 0;
                    break;
                case 0xA002:                              /* imagewidth */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pExif_temp->ulImageWidth = TagValOffset;
                    break;
                case 0xA003:                              /* ImageHeight */

                    JPEG_EXIT_IF((!(TagType == (Uint16) TAG_TYPE_LONG ||
                                    TagType == (Uint16) TAG_TYPE_SHORT)),
                                 JPEG_DEC_FAILED);
                    pExif_temp->ulImageHeight = TagValOffset;
                    break;
                case 0x8769:                              /* Next Offset of
                                                            * the 0th IFD */

                    JPEG_EXIT_IF((TagType != (Uint16) TAG_TYPE_LONG),
                                 JPEG_DEC_FAILED);
                    NextOffset = TagValOffset;
                    break;
                default:

                    break;
            }
            if ((TagType == TAG_TYPE_ASCII) || (TagType == TAG_TYPE_RATIONAL)
                || (TagType == TAG_TYPE_SRATIONAL))
            {
                LastValOffset = TagValOffset + (TagCount *
                                                BYTES_REQUIREMENT[(Uint32)
                                                                  TagType]);
            }
        }
        if (NextIFDOffset == 0)
        {
            if (ulLittleEndian == 1)
            {
                NextIFDOffset = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                          (*(pBuffer + 2) << 16) +
                                          (*(pBuffer + 3) << 24));
            }
            else
            {
                BigEndianRead32Bits(&NextIFDOffset, pBuffer);
            }
        }

        if (NextOffset == 0)
        {
            if (ulLittleEndian == 1)
            {
                NextOffset = (Uint32) ((*pBuffer) + (*(pBuffer + 1) << 8) +
                                       (*(pBuffer + 2) << 16) +
                                       (*(pBuffer + 3) << 24));

            }
            else
            {
                BigEndianRead32Bits(&NextOffset, pBuffer);
            }
            pBuffer += sizeof(Uint32);
        }

        if ((NextOffset > (pExif->ulExifSize - 10)) ||
            (NextIFDOffset > (pExif->ulExifSize - 10)))
        {
            NextIFDOffset = 0;
            NextOffset = 0;
        }

        pBuffer = pTagHeader;                              /* Reset */

    } while (NextOffset != NULL);

    if (NextIFDOffset != NULL)
    {
        status = TI_GetThumbnailInformation(pTagHeader, &NextIFDOffset,
                                            pThumbnail, ulLittleEndian);
        JPEG_EXIT_IF((status != JPEG_DEC_SOK), status);
        if (pThumbnail->JPEGInterchangeFormatLength != 0)
        {
            pThumbnail->tmbBuf =
                malloc(pThumbnail->JPEGInterchangeFormatLength);
            pTiff += pThumbnail->JPEGInterchangeFormat;
            memcpy(pThumbnail->tmbBuf, pTiff,
                   pThumbnail->JPEGInterchangeFormatLength);

        }

    }
    status = JPEG_DEC_SOK;

  EXIT:
    return status;

}

/* ===================================================================
 *  @func     InterpretSOF                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Jpeg_Dec_status InterpretSOF(Uint8 * pBuf, Uint16 * sof_len,
                             DecTblParams * DecTblParam)
{

    Jpeg_Dec_status status = JPEG_DEC_SOK;

    // Bit precision is not 8
    // JPEG_EXIT_IF(pBuf[0]!=8,JPEG_DEC_NOTSUPPORTED);
    // JPEG_EXIT_IF(pBuf[5]!=3,JPEG_DEC_NOTSUPPORTED);

    DecTblParam->height = (pBuf[1] * 256) + pBuf[2];
    DecTblParam->width = (pBuf[3] * 256) + pBuf[4];

    DecTblParam->h1v1 = pBuf[7];
    DecTblParam->h2v2 = pBuf[10];
    DecTblParam->h3v3 = pBuf[13];

  EXIT:

    return status;

}

/* ===================================================================
 *  @func     InterpretDHT                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Jpeg_Dec_status InterpretDHT(Uint8 * pBuf, Uint16 * dhtlen,
                             DecTblParams * DecTblParam)
{

    Jpeg_Dec_status status = JPEG_DEC_SOK;

    Uint16 dht_len, dht_ptr;

    Uint8 dht_tc, dht_th, size, dht_tot_size;

    Uint16 dht_idx, last_k, code;

    Uint8 *dht_data;

    // temp arrays .
    Uint8 bits[16];

    Uint8 huffsize[200];

    Uint16 huffcode[200];

    Uint16 huffcode_ladj[200];

    Uint8 log_num_entries_code[200];

    Uint8 ctl_grp[200];

    Uint8 max_size_grp[17];

    Uint8 min_code_idx_grp[17];

    Uint8 rshift_grp[17];

    Uint16 total_entries_grp[17];

    Uint16 dcd_tbl[500];                                   /* worst case is a 
                                                            * bit over
                                                            * 2*16*11 */
    Uint16 temp_ctl_tbl[17];

    Uint16 grp_start;

    Uint16 dcd_entry;

    Int16 dcd_idx, k, i, j;

    dht_len = *dhtlen;
    dht_ptr = 0;
    dht_data = pBuf;

    while (dht_ptr < dht_len)
    {
        /* which Huffman table */

        dht_tc = (dht_data[dht_ptr] >> 4) & 0xF;
        dht_th = dht_data[dht_ptr] & 0xF;
        dht_idx = (dht_th & 1) * 2 + (dht_tc & 1);
        dht_ptr++;

        /* expand BITS into HUFFSIZE */

        dht_tot_size = 0;

        printf("Huffman bits for table %d: ", dht_idx);

        for (k = 0, i = 0; i < 16; i++)
        {
            bits[i] = dht_data[dht_ptr + i];
            dht_tot_size += bits[i];

            for (j = 0; j < bits[i]; j++)
                huffsize[k++] = i + 1;

            printf("%d ", bits[i]);
        }
        printf("\n");

        dht_ptr += 16;
        huffsize[k] = 0;
        last_k = k;

        /* build HUFFCODE from HUFFSIZE */
        k = 0;
        code = 0;
        size = bits[0];

        while (huffsize[k] != 0)
        {
            while (huffsize[k] == size)
                huffcode[k++] = code++;

            if (huffsize[k] != 0)
            {
                do
                {
                    code = 2 * code;
                    size++;
                }
                while (huffsize[k] != size);
            }
        }

        /* left-shift HUFFCODE */

        for (i = 0; i < last_k; i++)
            huffcode_ladj[i] = ~huffcode[i] << (16 - huffsize[i]);

        /* categorize into LM0 groups, 0xxxx = 16, 10xxx = 15, etc */

        for (i = 0; i < last_k; i++)
        {
            if (huffcode_ladj[i] == 0)
                ctl_grp[i] = 0;
            else if ((huffcode_ladj[i] & 0xFFFE) == 0)
                ctl_grp[i] = 1;
            else if ((huffcode_ladj[i] & 0xFFFC) == 0)
                ctl_grp[i] = 2;
            else if ((huffcode_ladj[i] & 0xFFF8) == 0)
                ctl_grp[i] = 3;
            else if ((huffcode_ladj[i] & 0xFFF0) == 0)
                ctl_grp[i] = 4;
            else if ((huffcode_ladj[i] & 0xFFE0) == 0)
                ctl_grp[i] = 5;
            else if ((huffcode_ladj[i] & 0xFFC0) == 0)
                ctl_grp[i] = 6;
            else if ((huffcode_ladj[i] & 0xFF80) == 0)
                ctl_grp[i] = 7;
            else if ((huffcode_ladj[i] & 0xFF00) == 0)
                ctl_grp[i] = 8;
            else if ((huffcode_ladj[i] & 0xFE00) == 0)
                ctl_grp[i] = 9;
            else if ((huffcode_ladj[i] & 0xFC00) == 0)
                ctl_grp[i] = 10;
            else if ((huffcode_ladj[i] & 0xF800) == 0)
                ctl_grp[i] = 11;
            else if ((huffcode_ladj[i] & 0xF000) == 0)
                ctl_grp[i] = 12;
            else if ((huffcode_ladj[i] & 0xE000) == 0)
                ctl_grp[i] = 13;
            else if ((huffcode_ladj[i] & 0xC000) == 0)
                ctl_grp[i] = 14;
            else if ((huffcode_ladj[i] & 0x8000) == 0)
                ctl_grp[i] = 15;
            else
                ctl_grp[i] = 16;
        }

        /* default longest code within each group */
        max_size_grp[0] = 16;

        for (i = 1; i < 17; i++)
            max_size_grp[i] = 17 - i;

        /* longest code within each group = last one in the group */
        /* this sets left-shift for group */
        for (i = 0; i < last_k; i++)
            max_size_grp[ctl_grp[i]] = huffsize[i];

        /* num entries for a code = 2^(max_size_grp - huffsize) */
        for (i = 0; i < last_k; i++)
            log_num_entries_code[i] = max_size_grp[ctl_grp[i]] - huffsize[i];

        /* total num entries for a group = 2^(max_size_grp - 17-grp) */
        total_entries_grp[0] = 1 << (max_size_grp[0] - 16);

        for (i = 1; i < 17; i++)
            total_entries_grp[i] = 1 << (max_size_grp[i] - 17 + i);

        /* min code index for each group */
        for (i = last_k - 1; i >= 0; i--)
            min_code_idx_grp[ctl_grp[i]] = i;

        /* shift = 16 - max_size_grp */
        if (dht_tc)
            for (i = 0; i < 17; i++)
                rshift_grp[i] = 16 - max_size_grp[i];
        else
            for (i = 0; i < 17; i++)
                rshift_grp[i] = 12 - max_size_grp[i];

        /* fill control and decode tables */

        /* last entry = first coded group, first fill non-coded groups */

        k = ctl_grp[last_k - 1];
        dcd_idx = 0;
        dcd_tbl[dcd_idx++] = 0;                            /* 0 = no valid
                                                            * code = error */

        for (i = 0; i < k; i++)
            temp_ctl_tbl[i] = (16 << 11) | 0;              /* rshift 16-bit,
                                                            * offset 0
                                                            * (error) */

        /* for each group, fill from min_code, and fill in error for
         * un-utilized code (only possible in the first coded group) */

        do                                                 /* fill backward,
                                                            * as dcd_tbl is
                                                            * indexed with
                                                            * inverted bits
                                                            * data */
        {
            /* dcd_tbl indexing adds total_entries_grp, so need to adjust
             * offset down */
            temp_ctl_tbl[k] =
                (rshift_grp[k] << 11) | (dcd_idx - total_entries_grp[k]);

            grp_start = dcd_idx;
            dcd_idx = dcd_idx + total_entries_grp[k] - 1;

            for (j = min_code_idx_grp[k]; ctl_grp[j] == k; j++)
            {
                code = dht_data[dht_ptr + j];
                dcd_entry = (huffsize[j] << 11) | (code & 0x0F << 4)
                    | (code & 0xF0 >> 4);

                if (dht_tc && code == EOB_VAL)
                    dcd_entry |= 0x100;
                else if (dht_tc && code == ZRL_VAL)
                    dcd_entry |= 0x200;

                for (i = 0; i < (1 << log_num_entries_code[j]); i++)
                    dcd_tbl[dcd_idx--] = dcd_entry;
            }

            for (; dcd_idx >= grp_start; dcd_idx--)
                dcd_tbl[dcd_idx] = 0;

            dcd_idx = grp_start + total_entries_grp[k];
        }
        while (++k <= 16);

        if (dht_idx == 0)
        {
            DecTblParam->dc_y_dcd_size = dcd_idx;
            // DecTblParam->dc_y_dcd_tbl= malloc(dcd_idx * sizeof(short));
            memcpy(DecTblParam->dc_y_dcd_tbl, dcd_tbl, dcd_idx * sizeof(short));
            memcpy(&DecTblParam->ctl_tbl[0], &temp_ctl_tbl[4],
                   13 * sizeof(short));
        }
        else if (dht_idx == 1)
        {
            DecTblParam->ac_y_dcd_size = dcd_idx;
            // DecTblParam->ac_y_dcd_tbl = malloc(dcd_idx * sizeof(short));
            memcpy(DecTblParam->ac_y_dcd_tbl, dcd_tbl, dcd_idx * sizeof(short));
            memcpy(&DecTblParam->ctl_tbl[13], &temp_ctl_tbl[0],
                   17 * sizeof(short));
        }
        else if (dht_idx == 2)
        {
            DecTblParam->dc_uv_dcd_size = dcd_idx;
            // DecTblParam->dc_uv_dcd_tbl = malloc(dcd_idx * sizeof(short));
            memcpy(DecTblParam->dc_uv_dcd_tbl, dcd_tbl,
                   dcd_idx * sizeof(short));
            memcpy(&DecTblParam->ctl_tbl[30], &temp_ctl_tbl[4],
                   13 * sizeof(short));
        }
        else
        {
            DecTblParam->ac_uv_dcd_size = dcd_idx;
            // DecTblParam->ac_uv_dcd_tbl = malloc(dcd_idx * sizeof(short));
            memcpy(DecTblParam->ac_uv_dcd_tbl, dcd_tbl,
                   dcd_idx * sizeof(short));
            memcpy(&DecTblParam->ctl_tbl[43], &temp_ctl_tbl[0],
                   17 * sizeof(short));
        }

        dht_ptr += dht_tot_size;

    }

  EXIT:
    return status;

}

/* ===================================================================
 *  @func     SIMCOPJPEG_parser                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Jpeg_Dec_status SIMCOPJPEG_parser(unsigned char *inp,
                                  DecTblParams * DecTblParam, Uint32 * nInpsize,
                                  Bool parse_first)
{
    Uint8 mcu_found = 0, parse_err = 0;

    Uint16 uBytesParsed = 0, len;

    Uint8 marker[2];

    Uint8 *temp_buffer, *pTemp;

    Uint16 skipfactor;

    Jpeg_Dec_status status = JPEG_DEC_SOK;

    Uint8 *pExifBuffer;

    // allocate a temporay buffer
    temp_buffer = malloc(256);
    if (parse_first == TRUE)
    {
        DecTblParam->sof_marker = FALSE;
        DecTblParam->sos_marker = FALSE;

    }
    while (!mcu_found && uBytesParsed != *nInpsize && !parse_err)
    {

        marker[0] = *inp++;
        marker[1] = *inp++;

        uBytesParsed += 2;
        if (marker[0] != 0xFF)                             // SOI marker
            parse_err = 1;
        else
        {
            switch (marker[1])
            {

                case MARKER_SOI:                          /* just skip */
                    break;
                case MARKER_APP0:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    pTemp = inp;

                    // int32 InterpretJFIF(inp, jfifdata); Yet to be
                    // implemented;

                    inp = pTemp + len;                     // skip the bytes
                    uBytesParsed += (len + 2);
                    break;
                case MARKER_APP1:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    pTemp = inp;
                    pExifBuffer = malloc(len);
                    inp -= 4;
                    memcpy(pExifBuffer, inp, len);
                    JPEG_EXIT_IF((InterpretEXIF(pExifBuffer, &len, DecTblParam))
                                 != JPEG_DEC_SOK, JPEG_DEC_FAILED);
                    free(pExifBuffer);
                    inp = pTemp + len;                     // skip the bytes
                    uBytesParsed += (len + 2);

                    break;

                case MARKER_DQT:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    JPEG_EXIT_IF(len > *nInpsize, JPEG_DEC_FAILED);
                    pTemp = inp;
                    JPEG_EXIT_IF((InterpretDQT(inp, &len, DecTblParam)) !=
                                 JPEG_DEC_SOK, JPEG_DEC_FAILED);
                    inp = pTemp + len;                     // skip the bytes
                    uBytesParsed += (len + 2);
                    break;

                case MARKER_SOF:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    JPEG_EXIT_IF(len > *nInpsize, JPEG_DEC_FAILED);
                    JPEG_EXIT_IF(DecTblParam->sof_marker == TRUE, JPEG_DEC_FAILED); // no 
                                                                                    // two 
                                                                                    // SOF 
                                                                                    // marlers 
                                                                                    // supported
                    pTemp = inp;
                    // InterpretSOF(inp,&len,DecTblParam);
                    JPEG_EXIT_IF((InterpretSOF(inp, &len, DecTblParam)) !=
                                 JPEG_DEC_SOK, JPEG_DEC_FAILED);
                    inp = pTemp + len;                     // skip the bytes
                    uBytesParsed += (len + 2);
                    DecTblParam->sof_marker == TRUE;
                    break;

                case MARKER_SOS:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    JPEG_EXIT_IF(len > *nInpsize, JPEG_DEC_FAILED);
                    JPEG_EXIT_IF(DecTblParam->sof_marker == TRUE, JPEG_DEC_FAILED)  // No 
                                                                                    // SOS 
                                                                                    // unless 
                                                                                    // SOF 
                                                                                    // is 
                                                                                    // encountered
                        memcpy(DecTblParam->scan_header, inp, len); // copy
                                                                    // the
                                                                    // scan
                                                                    // header
                    inp += len;
                    uBytesParsed += (len + 2);
                    DecTblParam->sos_marker == TRUE;
                    mcu_found = 1;
                    break;
                case MARKER_DHT:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    JPEG_EXIT_IF(len > *nInpsize, JPEG_DEC_FAILED);
                    pTemp = inp;
                    JPEG_EXIT_IF((InterpretDHT(inp, &len, DecTblParam)) !=
                                 JPEG_DEC_SOK, JPEG_DEC_FAILED);
                    if (parse_first == FALSE)
                        mcu_found = 1;
                    inp = pTemp + len;                     // skip the bytes
                    uBytesParsed += (len + 2);
                    break;
                case MARKER_DRI:
                default:
                    len = ((*inp) * 256) + *(inp + 1) - 2;
                    inp += 2;
                    JPEG_EXIT_IF(len > *nInpsize, JPEG_DEC_FAILED);
                    inp += len;                            // skip the bytes
                    uBytesParsed += (len + 2);
                    break;

            }

        }

    }                                                      /* end of while
                                                            * (!mcu_found &&
                                                            * !feof(fp) &&
                                                            * !parse_err) */

    if (parse_err)
    {
      EXIT:
        printf("Parse_header: parse error, marker = %02x %02x\n",
               marker[0], marker[1]);
        exit(-1);
    }

    if (DecTblParam->h1v1 == 0x22 && DecTblParam->h2v2 == 0x11 &&
        DecTblParam->h3v3 == 0x11)
        DecTblParam->yuv_mode = 420;
    else if (DecTblParam->h1v1 == 0x21 && DecTblParam->h2v2 == 0x11 &&
             DecTblParam->h3v3 == 0x11)
        DecTblParam->yuv_mode = 422;
    else if (DecTblParam->h1v1 == 0x11 && DecTblParam->h2v2 == 0x11 &&
             DecTblParam->h3v3 == 0x11)
        DecTblParam->yuv_mode = 444;
    else
    {
        printf("Parse_header: yuv mode error, h1v1=%d, h2v2=%d, h3v3=%d\n",
               DecTblParam->h1v1, DecTblParam->h2v2, DecTblParam->h3v3);
        exit(-1);
    }

    printf("Parse_header: yuv_mode=%d, iw=%d, ih=%d\n", DecTblParam->yuv_mode,
           DecTblParam->width, DecTblParam->height);

    printf("Huffman decode table size = %d + %d + %d + %d = %d\n",
           DecTblParam->dc_y_dcd_size, DecTblParam->ac_y_dcd_size,
           DecTblParam->dc_uv_dcd_size, DecTblParam->ac_uv_dcd_size,
           DecTblParam->dc_y_dcd_size + DecTblParam->ac_y_dcd_size
           + DecTblParam->dc_uv_dcd_size + DecTblParam->ac_uv_dcd_size);
}

/* ===================================================================
 *  @func     main                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int main()
{

    DecTblParams *DecTblConfig;

    FILE *finp, *foup;

    char inpf[30];

    long bitstream_start;

    Uint32 bssize, i, j, k;

    Uint8 *Inpbuf;

    DecTblConfig = (DecTblParams *) malloc(sizeof(DecTblParams));
    DecTblConfig->ctl_tbl = malloc(60 * sizeof(short));
    DecTblConfig->dc_y_dcd_tbl = malloc(14 * sizeof(short));
    DecTblConfig->ac_y_dcd_tbl = malloc(182 * sizeof(short));
    DecTblConfig->dc_uv_dcd_tbl = malloc(14 * sizeof(short));
    DecTblConfig->ac_uv_dcd_tbl = malloc(182 * sizeof(short));
    finp = fopen("176X144.nv12_qCustom_ROT-0_HSS_Exif_out.jpg", "rb");
    if (finp == NULL)
    {
        printf("error opening file");
        exit(-1);
    }

    fseek(finp, 0, SEEK_END);
    bssize = ftell(finp);                                  // -bitstream_start;
    fseek(finp, 0, SEEK_SET);
    // bssize =5354;
    Inpbuf = malloc(bssize);
    fread(Inpbuf, sizeof(char), bssize, finp);
    fclose(finp);

    SIMCOPJPEG_parser(Inpbuf, DecTblConfig, &bssize, TRUE);

    for (i = 0; i < 128; i++)
        qmem[i] = DecTblConfig->qmat[i];

    for (i = 0; i < HMEM_SIZE; i++)
        hmem[i] = i & 0xFF;

    // memset(hmem,0,HMEM_SIZE);
    /* put control tables in settings */
    k = 0;
    for (i = 0; i < 60; i++)
        hmem[k++] = DecTblConfig->ctl_tbl[i];

    /* put decode tables in hmem */
    k = DCDTBL0OFFST;
    for (i = 0; i < DecTblConfig->dc_y_dcd_size; i++)
        hmem[k++] = DecTblConfig->dc_y_dcd_tbl[i];

    k = DCDTBL1OFFST;
    for (i = 0; i < DecTblConfig->ac_y_dcd_size; i++)
        hmem[k++] = DecTblConfig->ac_y_dcd_tbl[i];

    k = DCDTBL2OFFST;
    for (i = 0; i < DecTblConfig->dc_uv_dcd_size; i++)
        hmem[k++] = DecTblConfig->dc_uv_dcd_tbl[i];

    k = DCDTBL3OFFST;
    for (i = 0; i < DecTblConfig->ac_uv_dcd_size; i++)
        hmem[k++] = DecTblConfig->ac_uv_dcd_tbl[i];

    for (i = 0; i < 1592; i++)
        hmem[k++] = 0;

    write_hex_short("dbg_qmem.hex", qmem, 8, QMEM_SIZE / 8);
    write_hex_short("dbg_hmem.hex", hmem, 8, HMEM_SIZE / 8);

    return 0;

}
