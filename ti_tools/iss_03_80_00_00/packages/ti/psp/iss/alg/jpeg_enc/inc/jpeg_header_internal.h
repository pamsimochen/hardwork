/** ==================================================================
 *  @file   jpeg_header_internal.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/jpeg_enc/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef __HDR_INTERNAL_H__
#define __HDR_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MIN_JFIF_HEADER_LEN 16
#define MIN_JFXX_HEADER_LEN (MIN_JFIF_HEADER_LEN + 8)
#define MAX_JPEG_HEADER_LEN_WITH_ROTATION (0x251)
#define MAX_JPEG_HEADER_LEN_WITHOUT_ROTATION 0x24B
#define EXIF_HEADER_LEN_SOI         20
#define JFIF_HEADER_LEN_SOI         20
#define JFXX_HEADER_LEN_SOI         (MIN_JFXX_HEADER_LEN +2)
#define EXIF_TIFFHEADER_LEN     8
#define EXIF_TAIL_NULL_LEN       4
#define EXIF_NEXT_IFD_OFFSET_LEN        4
#define THUMBNAIL_JPEG_COMPRESSED 6

    typedef enum {
        TI_EXIF_TAG_TYPE_BYTE = 0x1,
        TI_EXIF_TAG_TYPE_ASCII = 0x2,
        TI_EXIF_TAG_TYPE_SHORT = 0x3,
        TI_EXIF_TAG_TYPE_LONG = 0x4,
        TI_EXIF_TAG_TYPE_RATIONAL = 0x5,
        TI_EXIF_TAG_TYPE_UNDEFINED = 0x7,
        TI_EXIF_TAG_TYPE_SLONG = 0x9,
        TI_EXIF_TAG_TYPE_SRATIONAL = 0xA
    } TI_EXIF_TAG_TYPE;

    typedef enum {
        TI_EXIF_IFD_0 = 0,
        TI_EXIF_IFD_1 = 1,
        TI_EXIF_IFD_EXIF = 2,
        TI_EXIF_IFD_GPS = 3,
        TI_EXIF_IFD_INTEROPERABILTY = 4
    } TI_EXIF_IFD_TYPE;

    typedef enum {
        /* TIFF Rev 6.0 Attribute Information for EXIF IFD_0 and IFD_1 */

        /* Tags related to Image Data Structure */
        TI_EXIF_TAG_IMAGE_WIDTH = 0x0100,
        TI_EXIF_TAG_IMAGE_LENGTH = 0x0101,
        TI_EXIF_TAG_BITS_PER_SAMPLE = 0x0102,
        TI_EXIF_TAG_COMPRESSION = 0x0103,
        TI_EXIF_TAG_PHOTOMETRIC_INTERPRETATION = 0x0106,
        TI_EXIF_TAG_ORIENTATION = 0x0112,
        TI_EXIF_TAG_SAMPLES_PER_PIXEL = 0x0115,
        TI_EXIF_TAG_PLANAR_CONFIGURATION = 0x011c,
        TI_EXIF_TAG_YCBCR_SUB_SAMPLING = 0x0212,
        TI_EXIF_TAG_YCBCR_POSITIONING = 0x0213,
        TI_EXIF_TAG_X_RESOLUTION = 0x011a,
        TI_EXIF_TAG_Y_RESOLUTION = 0x011b,
        TI_EXIF_TAG_RESOLUTION_UNIT = 0x0128,

        /* Tags relating to recording offset */
        TI_EXIF_TAG_STRIP_OFFSETS = 0x0111,
        TI_EXIF_TAG_ROWS_PER_STRIP = 0x0116,
        TI_EXIF_TAG_STRIP_BYTE_COUNTS = 0x0117,
        TI_EXIF_TAG_JPEG_INTERCHANGE_FORMAT = 0x0201,
        TI_EXIF_TAG_JPEG_INTERCHANGE_FORMAT_LENGTH = 0x0202,

        /* Tags relating to image data characteristics */
        TI_EXIF_TAG_TRANSFER_FUNCTION = 0x012d,
        TI_EXIF_TAG_WHITE_POINT = 0x013e,
        TI_EXIF_TAG_PRIMARY_CHROMATICITIES = 0x013f,
        TI_EXIF_TAG_YCBCR_COEFFICIENTS = 0x0211,
        TI_EXIF_TAG_REFERENCE_BLACK_WHITE = 0x0214,

        /* Other tags */
        TI_EXIF_TAG_DATE_TIME = 0x0132,
        TI_EXIF_TAG_IMAGE_DESCRIPTION = 0x010e,
        TI_EXIF_TAG_MAKE = 0x010f,
        TI_EXIF_TAG_MODEL = 0x0110,
        TI_EXIF_TAG_SOFTWARE = 0x0131,
        TI_EXIF_TAG_ARTIST = 0x013b,
        TI_EXIF_TAG_COPYRIGHT = 0x8298,

        /* EXIF and GPS IFD Pointers */
        TI_EXIF_TAG_EXIF_IFD_POINTER = 0x8769,
        TI_EXIF_TAG_GPS_INFO_IFD_POINTER = 0x8825,

        /* EXIF_IFD Tags */
        /* A. Tags Relating to Version */
        TI_EXIF_TAG_EXIF_VERSION = 0x9000,
        TI_EXIF_TAG_FLASH_PIX_VERSION = 0xa000,

        /* B. Tag Relating to Image Data Characteristics */
        TI_EXIF_TAG_COLOR_SPACE = 0xa001,

        /* C. Tags Relating to Image Configuration */
        TI_EXIF_TAG_COMPONENTS_CONFIGURATION = 0x9101,
        TI_EXIF_TAG_COMPRESSED_BITS_PER_PIXEL = 0x9102,
        TI_EXIF_TAG_PIXEL_X_DIMENSION = 0xa002,
        TI_EXIF_TAG_PIXEL_Y_DIMENSION = 0xa003,

        /* D. Tags Relating to User Information */
        TI_EXIF_TAG_MAKER_NOTE = 0x927c,
        TI_EXIF_TAG_USER_COMMENT = 0x9286,

        /* E. Tag Relating to Related File Information */
        TI_EXIF_TAG_RELATED_SOUND_FILE = 0xa004,

        /* F. Tags Relating to Date and Time */
        TI_EXIF_TAG_DATE_TIME_ORIGINAL = 0x9003,
        TI_EXIF_TAG_DATE_TIME_DIGITIZED = 0x9004,
        TI_EXIF_TAG_SUB_SEC_TIME = 0x9290,
        TI_EXIF_TAG_SUB_SEC_TIME_ORIGINAL = 0x9291,
        TI_EXIF_TAG_SUB_SEC_TIME_DIGITIZED = 0x9292,

        /* G. Tags Relating to Picture-Taking Conditions */
        TI_EXIF_TAG_EXPOSURE_TIME = 0x829a,
        TI_EXIF_TAG_FNUMBER = 0x829d,
        TI_EXIF_TAG_EXPOSURE_PROGRAM = 0x8822,
        TI_EXIF_TAG_SPECTRAL_SENSITIVITY = 0x8824,
        TI_EXIF_TAG_ISO_SPEED_RATINGS = 0x8827,
        TI_EXIF_TAG_OECF = 0x8828,
        TI_EXIF_TAG_SHUTTER_SPEED_VALUE = 0x9201,
        TI_EXIF_TAG_APERTURE_VALUE = 0x9202,
        TI_EXIF_TAG_BRIGHTNESS_VALUE = 0x9203,
        TI_EXIF_TAG_EXPOSURE_BIAS_VALUE = 0x9204,
        TI_EXIF_TAG_MAX_APERTURE_VALUE = 0x9205,
        TI_EXIF_TAG_SUBJECT_DISTANCE = 0x9206,
        TI_EXIF_TAG_METERING_MODE = 0x9207,
        TI_EXIF_TAG_LIGHT_SOURCE = 0x9208,
        TI_EXIF_TAG_FLASH = 0x9209,
        TI_EXIF_TAG_FOCAL_LENGTH = 0x920a,
        TI_EXIF_TAG_SUBJECT_AREA = 0x9214,
        TI_EXIF_TAG_INTEROPERABILITY_IFD_POINTER = 0xa005,
        TI_EXIF_TAG_FLASH_ENERGY = 0xa20b,
        TI_EXIF_TAG_SPATIAL_FREQUENCY_RESPONSE = 0xa20c,
        TI_EXIF_TAG_FOCAL_PLANE_X_RESOLUTION = 0xa20e,
        TI_EXIF_TAG_FOCAL_PLANE_Y_RESOLUTION = 0xa20f,
        TI_EXIF_TAG_FOCAL_PLANE_RESOLUTION_UNIT = 0xa210,
        TI_EXIF_TAG_SUBJECT_LOCATION = 0xa214,
        TI_EXIF_TAG_EXPOSURE_INDEX = 0xa215,
        TI_EXIF_TAG_SENSING_METHOD = 0xa217,
        TI_EXIF_TAG_FILE_SOURCE = 0xa300,
        TI_EXIF_TAG_SCENE_TYPE = 0xa301,
        TI_EXIF_TAG_CFA_PATTERN = 0xa302,
        TI_EXIF_TAG_CUSTOM_RENDERED = 0xa401,
        TI_EXIF_TAG_EXPOSURE_MODE = 0xa402,
        TI_EXIF_TAG_WHITE_BALANCE = 0xa403,
        TI_EXIF_TAG_DIGITAL_ZOOM_RATIO = 0xa404,
        TI_EXIF_TAG_FOCAL_LENGTH_IN_35MM_FILM = 0xa405,
        TI_EXIF_TAG_SCENE_CAPTURE_TYPE = 0xa406,
        TI_EXIF_TAG_GAIN_CONTROL = 0xa407,
        TI_EXIF_TAG_CONTRAST = 0xa408,
        TI_EXIF_TAG_SATURATION = 0xa409,
        TI_EXIF_TAG_SHARPNESS = 0xa40a,
        TI_EXIF_TAG_DEVICE_SETTING_DESCRIPTION = 0xa40b,
        TI_EXIF_TAG_SUBJECT_DISTANCE_RANGE = 0xa40c,

        /* H. Other Tags */
        TI_EXIF_TAG_IMAGE_UNIQUE_ID = 0xa420,

        TI_EXIF_TAG_INTEROPERABILITY_INDEX = 0x0001,
        TI_EXIF_TAG_INTEROPERABILITY_VERSION = 0x0002,
        /* 
         * TI_EXIF_TAG_NEW_SUBFILE_TYPE = 0x00fe, TI_EXIF_TAG_FILL_ORDER =
         * 0x010a, TI_EXIF_TAG_DOCUMENT_NAME = 0x010d,
         * TI_EXIF_TAG_TRANSFER_RANGE = 0x0156, TI_EXIF_TAG_SUB_IFDS =
         * 0x014a, TI_EXIF_TAG_JPEG_PROC = 0x0200, TI_EXIF_TAG_XML_PACKET =
         * 0x02bc, TI_EXIF_TAG_RELATED_IMAGE_FILE_FORMAT = 0x1000,
         * TI_EXIF_TAG_RELATED_IMAGE_WIDTH = 0x1001,
         * TI_EXIF_TAG_RELATED_IMAGE_LENGTH = 0x1002,
         * TI_EXIF_TAG_CFA_REPEAT_PATTERN_DIM = 0x828d,
         * TI_EXIF_TAG_CFA_PATTERN = 0x828e, TI_EXIF_TAG_BATTERY_LEVEL =
         * 0x828f, TI_EXIF_TAG_IPTC_NAA = 0x83bb, TI_EXIF_TAG_IMAGE_RESOURCES 
         * = 0x8649, TI_EXIF_TAG_INTER_COLOR_PROFILE = 0x8773,
         * TI_EXIF_TAG_TIME_ZONE_OFFSET = 0x882a,
         * TI_EXIF_TAG_TIFF_EP_STANDARD_ID = 0x9216, TI_EXIF_TAG_XP_TITLE =
         * 0x9c9b, TI_EXIF_TAG_XP_COMMENT = 0x9c9c, TI_EXIF_TAG_XP_AUTHOR =
         * 0x9c9d, TI_EXIF_TAG_XP_KEYWORDS = 0x9c9e, TI_EXIF_TAG_XP_SUBJECT = 
         * 0x9c9f, TI_EXIF_TAG_GAMMA = 0xa500,
         * TI_EXIF_TAG_PRINT_IMAGE_MATCHING = 0xc4a5 */

        TI_EXIF_TAG_GPS_VERSIONID = 0x0000,
        TI_EXIF_TAG_GPS_LATITUDEREF = 0x0001,
        TI_EXIF_TAG_GPS_LATITUDE = 0x0002,
        TI_EXIF_TAG_GPS_LONGITUDEREF = 0x0003,
        TI_EXIF_TAG_GPS_LONGITUDE = 0x0004,
        TI_EXIF_TAG_GPS_ALTITUDEREF = 0x0005,
        TI_EXIF_TAG_GPS_ALTITUDE = 0x0006,
        TI_EXIF_TAG_GPS_TIMESTAMP = 0x0007,
        TI_EXIF_TAG_GPS_SATELLITES = 0x0008,
        TI_EXIF_TAG_GPS_STATUS = 0x0009,
        TI_EXIF_TAG_GPS_MEASUREMODE = 0x000A,
        TI_EXIF_TAG_GPS_DOP = 0x000B,
        TI_EXIF_TAG_GPS_SPEEDREF = 0x000C,
        TI_EXIF_TAG_GPS_SPEED = 0x000D,
        TI_EXIF_TAG_GPS_TRACKREF = 0x000E,
        TI_EXIF_TAG_GPS_TRACK = 0x000F,
        TI_EXIF_TAG_GPS_IMGDIRECTIONREF = 0x0010,
        TI_EXIF_TAG_GPS_IMGDIRECTION = 0x0011,
        TI_EXIF_TAG_GPS_MAPDATUM = 0x0012,
        TI_EXIF_TAG_GPS_DESTLATITUDEREF = 0x0013,
        TI_EXIF_TAG_GPS_DESTLATITUDE = 0x0014,
        TI_EXIF_TAG_GPS_DESTLONGITUDEREF = 0x0015,
        TI_EXIF_TAG_GPS_DESTLONGITUDE = 0x0016,
        TI_EXIF_TAG_GPS_DESTBEARINGREF = 0x0017,
        TI_EXIF_TAG_GPS_DESTBEARING = 0x0018,
        TI_EXIF_TAG_GPS_DESTDISTANCEREF = 0x0019,
        TI_EXIF_TAG_GPS_DESTDISTANCE = 0x001A,
        TI_EXIF_TAG_GPS_PROCESSINGMODE = 0x001B,
        TI_EXIF_TAG_GPS_AREAINFORMATION = 0x001C,
        TI_EXIF_TAG_GPS_DATESTAMP = 0x001D,
        TI_EXIF_TAG_GPS_DIFFERENTIAL = 0x001E
    } TI_EXIF_TAGS_SUPPORTED;

#ifdef __cplusplus
}
#endif
#endif                                                     /* __HDR_INTERNAL_H__ 
                                                            */
