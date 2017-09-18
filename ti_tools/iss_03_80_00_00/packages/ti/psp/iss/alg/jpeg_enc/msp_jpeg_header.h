/** ==================================================================
 *  @file   msp_jpeg_header.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/jpeg_enc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef EXIF_DATA_H
#define EXIF_DATA_H
#ifdef __cplusplus
extern "C" {
#endif
#include "inc/msp.h"
#include "msp_jpege.h"
#define MAX_HEADER_SIZE 65536
#define MAX_HEADER_SIZE_WITHOUT_THUMBNAIL (3*1024-2416-32-7-4)
#define MIN_HEADER_SIZE_WITHOUT_THUMBNAIL (32)
#define MAX_THUMB_SIZE     (60*1024)
#define MAX_APP13_HEADER_SIZE 65536
#define MAX_APP13_HEADER_SIZE_WITHOUT_THUMBNAIL (3*1024)
#define MAX_APP13_THUMB_SIZE     (60*1024)
#define MAX_STRING_LEN             30
#define MAX_TIME_STRING_LEN       20
#define MAX_IFD01_TAGS             32
#define MAX_IFD_EXIF_TAGS        57
#define MAX_IFD_GPS_TAGS        31
#define MAX_EXIF_TAG_LEN          12
#define MAX_GPS_IFD_LEN            0x200
#define MAX_EXIF_STRIP_OFFSET_COUNT 0x80
#define EXIF_PARAM_INVALID    (0xFFFF)

    typedef struct {
        MSP_U32 ulImageWidth;
        MSP_BOOL bValidityImageWidth;
        MSP_U32 ulImageHeight;
        MSP_BOOL bValidityImageHeight;
        MSP_U16 usBitsPerSample[3];
        MSP_BOOL bValidityBitsPerSample;
        MSP_U16 usCompression;
        MSP_BOOL bValidityCompression;
        MSP_U16 usPhotometricInterpretation;
        MSP_BOOL bValidityPhotometricInterpretation;
        MSP_U16 usOrientation;
        MSP_BOOL bValidityOrientation;
        MSP_U16 usSamplesPerPixel;
        MSP_BOOL bValiditySamplesPerPixel;
        MSP_U16 usPlanarConfiguration;
        MSP_BOOL bValidityPlanarConfiguration;
        MSP_U16 usYCbCrSubSampling[2];
        MSP_BOOL bValidityYCbCrSubSampling;
        MSP_U16 usYCbCrPositioning;
        MSP_BOOL bValidityYCbCrPositioning;
        MSP_U32 ulXResolution[2];
        MSP_BOOL bValidityXResolution;
        MSP_U32 ulYResolution[2];
        MSP_BOOL bValidityYResolution;
        MSP_U16 usResolutionUnit;
        MSP_BOOL bValidityResolutionUnit;
        MSP_U32 ulRowsPerStrip;
        MSP_BOOL bValidityRowsPerStrip;
        MSP_U8 *pDataAddrOffset;
        MSP_U32 ulDataSize;
        MSP_BOOL bValidityDataSize;
        MSP_U16 usTransferFunction[3 * 256];
        MSP_BOOL bValidityTransferFunction;
        MSP_U32 ulWhitePoint[4];                           // 2x2
        MSP_BOOL bValidityWhitePoint;
        MSP_U32 ulPrimaryChromaticities[12];               // 2 x 6
        MSP_BOOL bValidityPrimaryChromaticities;
        MSP_U32 ulYCbCrCoefficients[6];                    // 2 x 6
        MSP_BOOL bValidityYCbCrCoefficients;
        MSP_U32 ulReferenceBlackWhite[12];                 // 2 x 6
        MSP_BOOL bValidityReferenceBlackWhite;

        MSP_S8 *pDateTimeBuff;
        MSP_U32 ulDateTimeBuffSizeBytes;
        MSP_BOOL bValidityDateTime;
        MSP_S8 *pImageDescriptionBuff;
        MSP_U32 ulImageDescriptionBuffSizeBytes;
        MSP_BOOL bValidityImageDescription;
        MSP_S8 *pMakeBuff;
        MSP_U32 ulMakeBuffSizeBytes;
        MSP_BOOL bValidityMake;
        MSP_S8 *pModelBuff;
        MSP_U32 ulModelBuffSizeBytes;
        MSP_BOOL bValidityModel;
        MSP_S8 *pSoftwareBuff;
        MSP_U32 ulSoftwareBuffSizeBytes;
        MSP_BOOL bValiditySoftware;
        MSP_S8 *pArtistBuff;
        MSP_U32 ulArtistBuffSizeBytes;
        MSP_BOOL bValidityArtist;
        MSP_S8 *pCopyrightBuff;
        MSP_U32 ulCopyrightBuffSizeBytes;
        MSP_BOOL bValidityCopyright;

        MSP_U8 *ulExifIFDPointer;                          // Are output
                                                           // parameters in
                                                           // Case
                                                           // Application
                                                           // wants to Access 
                                                           // the EXIF tags
                                                           // directly
        MSP_U8 *ulGPSIFDPointer;                           // Are output
                                                           // parameters in
                                                           // Case
                                                           // Application
                                                           // wants to write
                                                           // the GPS tags
                                                           // with this
                                                           // offset
                                                           // information

        MSP_S8 cExifVersion[4];
        MSP_BOOL bValidityExifVersion;
        MSP_S8 cFlashpixVersion[4];
        MSP_BOOL bValidityFlashpixVersion;
        MSP_U16 usColorSpace;
        MSP_BOOL bValidityColorSpace;
        MSP_S8 cComponentsConfiguration[4];
        MSP_BOOL bValidityComponentsConfiguration;
        MSP_U32 ulCompressedBitsPerPixel[2];
        MSP_BOOL bValidityCompressedBitsPerPixel;
        MSP_U32 ulPixelXDimension;
        MSP_BOOL bValidityPixelXDimension;
        MSP_U32 ulPixelYDimension;
        MSP_BOOL bValidityPixelYDimension;
        MSP_S8 *pMakerNoteBuff;
        MSP_U32 ulMakerNoteBuffSizeBytes;
        MSP_BOOL bValidityMakerNote;
        MSP_S8 *pUserCommentBuff;
        MSP_U32 ulUserCommentBuffSizeBytes;
        MSP_BOOL bValidityUserComment;

        MSP_S8 cRelatedSoundFile[13];
        MSP_BOOL bValidityRelatedSoundFile;

        MSP_S8 *pDateTimeOriginalBuff;
        MSP_U32 ulDateTimeOriginalBuffSizeBytes;
        MSP_BOOL bValidityDateTimeOriginal;
        MSP_S8 *pDateTimeDigitizedBuff;
        MSP_U32 ulDateTimeDigitizedBuffSizeBytes;
        MSP_BOOL bValidityDateTimeDigitized;
        MSP_S8 *pSubSecTimeBuff;
        MSP_U32 ulSubSecTimeBuffSizeBytes;
        MSP_BOOL bValiditySubSecTime;
        MSP_S8 *pSubSecTimeOriginalBuff;
        MSP_U32 ulSubSecTimeOriginalBuffSizeBytes;
        MSP_BOOL bValiditySubSecTimeOriginal;
        MSP_S8 *pSubSecTimeDigitizedBuff;
        MSP_U32 ulSubSecTimeDigitizedBuffSizeBytes;
        MSP_BOOL bValiditySubSecTimeDigitized;

        MSP_U32 ulExposureTime[2];
        MSP_BOOL bValidityExposureTime;
        MSP_U32 ulFNumber[2];
        MSP_BOOL bValidityFNumber;
        MSP_U16 usExposureProgram;
        MSP_BOOL bValidityExposureProgram;

        MSP_S8 *pSpectralSensitivityBuff;
        MSP_U32 ulSpectralSensitivityBuffSizeBytes;
        MSP_BOOL bValiditySpectralSensitivity;

        MSP_U16 usISOCount;
        MSP_BOOL bValidityISOCount;
        MSP_U16 *pISOSpeedRatings;
        MSP_BOOL bValidityISOSpeedRatings;

        MSP_S8 *pOECFBuff;
        MSP_U32 ulOECFBuffSizeBytes;
        MSP_BOOL bValidityOECF;

        MSP_S32 slShutterSpeedValue[2];
        MSP_BOOL bValidityShutterSpeedValue;
        MSP_U32 ulApertureValue[2];
        MSP_BOOL bValidityApertureValue;
        MSP_S32 slBrightnessValue[2];
        MSP_BOOL bValidityBrightnessValue;
        MSP_S32 slExposureBiasValue[2];
        MSP_BOOL bValidityExposureBiasValue;
        MSP_U32 ulMaxApertureValue[2];
        MSP_BOOL bValidityMaxApertureValue;
        MSP_U32 ulSubjectDistance[2];
        MSP_BOOL bValiditySubjectDistance;
        MSP_U16 usMeteringMode;
        MSP_BOOL bValidityMeteringMode;
        MSP_U16 usLightSource;
        MSP_BOOL bValidityLightSource;
        MSP_U16 usFlash;
        MSP_BOOL bValidityFlash;
        MSP_U32 ulFocalLength[2];
        MSP_BOOL bValidityFocalLength;
        MSP_U16 usSubjectArea[4];
        MSP_BOOL bValiditySubjectArea;
        MSP_U32 ulFlashEnergy[2];
        MSP_BOOL bValidityFlashEnergy;

        MSP_S8 *pSpatialFrequencyResponseBuff;
        MSP_U32 ulSpatialFrequencyResponseBuffSizeBytes;
        MSP_BOOL bValiditySpatialFrequencyResponse;

        MSP_U32 ulFocalPlaneXResolution[2];
        MSP_BOOL bValidityFocalPlaneXResolution;
        MSP_U32 ulFocalPlaneYResolution[2];
        MSP_BOOL bValidityFocalPlaneYResolution;
        MSP_U16 usFocalPlaneResolutionUnit;
        MSP_BOOL bValidityFocalPlaneResolutionUnit;
        MSP_U16 usSubjectLocation[2];
        MSP_BOOL bValiditySubjectLocation;
        MSP_U32 ulExposureIndex[2];
        MSP_BOOL bValidityExposureIndex;
        MSP_U16 usSensingMethod;
        MSP_BOOL bValiditySensingMethod;
        MSP_S8 cFileSource;
        MSP_BOOL bValidityFileSource;
        MSP_S8 cSceneType;
        MSP_BOOL bValiditySceneType;

        MSP_S8 *pCFAPatternBuff;
        MSP_U32 ulCFAPatternBuffSizeBytes;
        MSP_BOOL bValidityCFAPattern;

        MSP_U16 usCustomRendered;
        MSP_BOOL bValidityCustomRendered;
        MSP_U16 usExposureMode;
        MSP_BOOL bValidityExposureMode;
        MSP_U16 usWhiteBalance;
        MSP_BOOL bValidityWhiteBalance;
        MSP_U32 ulDigitalZoomRatio[2];
        MSP_BOOL bValidityDigitalZoomRatio;
        MSP_U16 usFocalLengthIn35mmFilm;
        MSP_BOOL bValidityFocalLengthIn35mmFilm;
        MSP_U16 usSceneCaptureType;
        MSP_BOOL bValiditySceneCaptureType;
        MSP_U16 usGainControl;
        MSP_BOOL bValidityGainControl;
        MSP_U16 usContrast;
        MSP_BOOL bValidityContrast;
        MSP_U16 usSaturation;
        MSP_BOOL bValiditySaturation;
        MSP_U16 usSharpness;
        MSP_BOOL bValiditySharpness;
        MSP_S8 *pDeviceSettingDescriptionBuff;
        MSP_U32 ulDeviceSettingDescriptionBuffSizeBytes;
        MSP_BOOL bValidityDeviceSettingDescription;

        MSP_U16 usSubjectDistanceRange;
        MSP_BOOL bValiditySubjectDistanceRange;
        MSP_S8 cImageUniqueID[33];
        MSP_BOOL bValidityImageUniqueID;

        MSP_U8 *pPrivateNextIFDPointer;                    // Should not be
                                                           // used by the
                                                           // application;
                                                           // used internally
        MSP_U8 *pPrivateThumbnailSize;
        MSP_U8 *pPrivateTiffHeaderPointer;

        MSP_U8 ucGpsVersionId[4];
        MSP_BOOL bValidityGpsVersionId;
        MSP_S8 cGpslatitudeRef[2];
        MSP_BOOL bValidityGpslatitudeRef;
        MSP_U32 ulGpsLatitude[6];
        MSP_BOOL bValidityGpsLatitude;
        MSP_S8 cGpsLongitudeRef[2];
        MSP_BOOL bValidityGpsLongitudeRef;
        MSP_U32 ulGpsLongitude[6];
        MSP_BOOL bValidityGpsLongitude;
        MSP_U8 ucGpsAltitudeRef;
        MSP_BOOL bValidityGpsAltitudeRef;
        MSP_U32 ulGpsAltitude[2];
        MSP_BOOL bValidityGpsAltitude;
        MSP_U32 ulGpsTimeStamp[6];
        MSP_BOOL bValidityGpsTimeStamp;
        MSP_S8 *pGpsSatellitesBuff;
        MSP_U32 ulGpsSatellitesBuffSizeBytes;
        MSP_BOOL bValidityGpsSatellites;

        MSP_S8 cGpsStatus[2];
        MSP_BOOL bValidityGpsStatus;
        MSP_S8 cGpsMeasureMode[2];
        MSP_BOOL bValidityGpsMeasureMode;
        MSP_U32 ulGpsDop[2];
        MSP_BOOL bValidityGpsDop;
        MSP_S8 cGpsSpeedRef[2];
        MSP_BOOL bValidityGpsSpeedRef;
        MSP_U32 ulGpsSpeed[2];
        MSP_BOOL bValidityGpsSpeed;
        MSP_S8 cGpsTrackRef[2];
        MSP_BOOL bValidityGpsTrackRef;
        MSP_U32 ulGpsTrack[2];
        MSP_BOOL bValidityGpsTrack;
        MSP_S8 cGpsImgDirectionRef[2];
        MSP_BOOL bValidityGpsImgDirectionRef;
        MSP_U32 ulGpsImgDirection[2];
        MSP_BOOL bValidityGpsImgDirection;

        MSP_S8 *pGpsMapDatumBuff;
        MSP_U32 ulGpsMapDatumBuffSizeBytes;
        MSP_BOOL bValidityGpsMapDatum;

        MSP_S8 cGpsDestLatitudeRef[2];
        MSP_BOOL bValidityGpsDestLatitudeRef;
        MSP_U32 ulGpsDestLatitude[6];
        MSP_BOOL bValidityGpsDestLatitude;
        MSP_S8 cGpsDestLongitudeRef[2];
        MSP_BOOL bValidityGpsDestLongitudeRef;
        MSP_U32 ulGpsDestLongitude[6];
        MSP_BOOL bValidityGpsDestLongitude;
        MSP_S8 cGpsDestBearingRef[2];
        MSP_BOOL bValidityGpsDestBearingRef;
        MSP_U32 ulGpsDestBearing[2];
        MSP_BOOL bValidityGpsDestBearing;
        MSP_S8 cGpsDestDistanceRef[2];
        MSP_BOOL bValidityGpsDestDistanceRef;
        MSP_U32 ulGpsDestDistance[2];
        MSP_BOOL bValidityGpsDestDistance;

        MSP_S8 *pGpsProcessingMethodBuff;
        MSP_U32 ulGpsProcessingMethodBuffSizeBytes;
        MSP_BOOL bValidityGpsProcessingMethod;
        MSP_S8 *pGpsAreaInformationBuff;
        MSP_U32 ulGpsAreaInformationBuffSizeBytes;
        MSP_BOOL bValidityGpsAreaInformation;

        MSP_S8 cGpsDateStamp[11];
        MSP_BOOL bValidityGpsDateStamp;
        MSP_U16 usGpsDifferential;
        MSP_BOOL bValidityGpsDifferential;
    } TI_EXIF_INFO_SUPPORTED;

    /* Brief APP13 Marker Parameter structure */

    typedef struct {
        MSP_S8 *pBuf;                                      // APP13 data
                                                           // buffer
        MSP_U32 ulBufLen;                                  // App13 data
                                                           // buffer length

        MSP_BOOL bThumbnailPresent;                        // < TRUE:
                                                           // Thumbnail image 
                                                           // information is
                                                           // valid, \n
                                                           // FALSE:
                                                           // Thumbnail image 
                                                           // information is
                                                           // not used
        MSP_U8 *pThumbnail;                                // APP13 thumbnail 
                                                           // buffer
        MSP_U16 usThumbnailSize;                           // APP13 thumbnail 
                                                           // size
        MSP_U16 usApp13Size;                               // Total APP13
                                                           // marker size.

    } TI_APP13_PARAMS_SUPPORTED;

/**
    \brief EXIF Parameter structure

    - This structure stores all EXIF related information for both main image and thumbnail image.
    - During EXIF creation this structure is passed as input to DSCAPI_EXIF_MakeHeader() API
    - During EXIF parsing this structure is filled by DSCAPI_EXIF_ParseHeader() API

    For more info on EXIF file format refer <A HREF="http://www.exif.org/specifications.html"> EXIF 2.2 specification </A>
*/
    typedef struct {
        TI_EXIF_INFO_SUPPORTED *pMainImage;                // /< Main Image
                                                           // information
        TI_EXIF_INFO_SUPPORTED *pThumbImage;               // /< Thumbnail
                                                           // image
                                                           // information
        MSP_BOOL bThumbnailPresent;                        // /< TRUE:
                                                           // Thumbnail image 
                                                           // information is
                                                           // valid, \n
                                                           // FALSE:
                                                           // Thumbnail image 
                                                           // information is
                                                           // not used
        MSP_U32 ulExifSize;
        MSP_U32 ulOffsetToStartOfEncodedStream;
    } TI_EXIF_PARAMS_SUPPORTED;

    typedef enum {
        JFIF_THUMBNAIL_JPEG,
        JFIF_THUMBNAIL_1BYTEPERPIXEL,
        JFIF_THUMBNAIL_RGB
    } JFIF_THUMBNAIL_FORMAT;

    typedef struct {
        MSP_BOOL bThumbnailPresent;                        // /< TRUE:
                                                           // Thumbnail image 
                                                           // information is
                                                           // valid, \n
                                                           // FALSE:
                                                           // Thumbnail image 
                                                           // information is
                                                           // not used
        JFIF_THUMBNAIL_FORMAT eThumbnailFormat;

        MSP_U8 ucThumbWidth;                               /* Applicable only 
                                                            * if the
                                                            * thumbnail
                                                            * format is NOT
                                                            * JPEG */
        MSP_U8 ucThumbHeight;                              /* Applicable only 
                                                            * if the
                                                            * thumbnail
                                                            * format is NOT
                                                            * JPEG */
        MSP_U8 *pThumbnail;
        MSP_U16 usThumbnailSize;
        MSP_U16 usJFIFSize;
        MSP_U32 ulOffsetToStartOfEncodedStream;
    } TI_JFIF_PARAMS_SUPPORTED;

    typedef struct {
        const unsigned int JPEGENC_DCHUFFY[12];
        const unsigned int JPEGENC_ACHUFFY[176];
        const unsigned int JPEGENC_DCHUFFUV[12];
        const unsigned int JPEGENC_ACHUFFUV[176];
        /* DHT Marker */
        const unsigned char lum_dc_codelens[16];
        unsigned int lum_dc_ncodes;
        const unsigned char lum_dc_symbols[12];
        unsigned int lum_dc_nsymbols;
        const unsigned char lum_ac_codelens[16];
        unsigned int lum_ac_ncodes;
        const unsigned char lum_ac_symbols[162];
        unsigned int lum_ac_nsymbols;
        const unsigned char chm_dc_codelens[16];
        unsigned int chm_dc_ncodes;
        const unsigned char chm_dc_symbols[12];
        unsigned int chm_dc_nsymbols;
        const unsigned char chm_ac_codelens[16];
        unsigned int chm_ac_ncodes;
        const unsigned char chm_ac_symbols[162];
        unsigned int chm_ac_nsymbols;
    } MSP_CUSTOM_HUFF_TABLE;

    MSP_ERROR_TYPE TI_WriteJpegHeader(MSP_U8 * pBuffer,
                                      MSP_JPEGE_CREATE_PARAM * pMspParams,
                                      void *pInterchangeFmt, void *pApp13Marker,
                                      MSP_U8 * pQuantTab,
                                      MSP_CUSTOM_HUFF_TABLE * pHuffTab);
    MSP_ERROR_TYPE TI_WriteJpegFooter(MSP_U8 * pBuffer);
    MSP_ERROR_TYPE TI_EXIF_SetDefaultParams(TI_EXIF_INFO_SUPPORTED * pExif,
                                            MSP_U8 ucIFDNum);
    MSP_U32 TI_GetEncodeByteOffset(MSP_JPEGE_HEADERTYPE eHeaderFormat,
                                   MSP_BOOL bThumbnailPresent,
                                   MSP_U32 ulThumbnailSize,
                                   MSP_BOOL bApp13MarkerEnable,
                                   MSP_BOOL bApp13ThumbnailPresent);
    MSP_U32 TI_GetMinBitStreamSizeInBytes(MSP_U32 ulImageWidth,
                                          MSP_U32 ulImageHeight,
                                          MSP_JPEGE_COLORFORMAT_TYPE
                                          eColorFormat);

#ifdef __cplusplus
}
#endif
#endif
