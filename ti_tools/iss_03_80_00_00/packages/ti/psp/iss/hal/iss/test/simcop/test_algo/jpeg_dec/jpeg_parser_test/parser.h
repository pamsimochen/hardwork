/** ==================================================================
 *  @file   parser.h                                                  
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
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef int Bool;

#define TRUE		((Bool) 1)
#define FALSE		((Bool) 0)

#define DCDTBL0OFFST 60
#define DCDTBL1OFFST 74
#define DCDTBL2OFFST 256
#define DCDTBL3OFFST 270

typedef int Int;

typedef unsigned int Uns;                                  /* deprecated type 
                                                            */
typedef char Char;

typedef char *String;

typedef void *Ptr;

/* unsigned quantities */
typedef unsigned int Uint32;

typedef unsigned short Uint16;

typedef unsigned char Uint8;

typedef Uint8 Bits2;

typedef Uint8 Bits3;

typedef Uint8 Bits4;

typedef Uint8 Bits5;

typedef Uint8 Bits6;

typedef Uint8 Bits7;

typedef Uint16 Bits9;

typedef Uint16 Bits10;

typedef Uint16 Bits11;

typedef Uint16 Bits12;

typedef Uint16 Bits13;

typedef Uint16 Bits14;

typedef Uint16 Bits15;

/* signed quantities */
typedef int Int32;

typedef short Int16;

typedef char Int8;

typedef struct EXIF_THUMBNAIL_INFO {
    Uint32 ImageWidth;
    Uint32 ImageHeight;
    Uint32 StripOffset;
    Uint32 RowsPerStrip;
    Uint32 StripByteCount;
    Uint32 PlanarConfiguration;
    Uint32 YCbCrSubSamplingOffset;
    Uint32 YCbCrPositioning;
    Uint32 SamplesPerPixel;
    Uint32 PhotometricInterpretation;
    Uint32 Compression;
    Uint32 BitsPerSampleOffset;
    Uint32 JPEGInterchangeFormat;
    Uint32 JPEGInterchangeFormatLength;
    Uint32 thumbsize;
    Uint8 *tmbBuf;
    // UINT32 JPEGImageOffset;

    Uint32 nFields;                                        /* total number of 
                                                            * fields in the
                                                            * EXIF Thumbnail
                                                            * that are to be
                                                            * supported */

} EXIF_THUMBNAIL_INFO;

typedef struct EXIF_INFO_SUPPORTED {
    Uint16 Orientation;
    Uint8 *pCreationDateTime;
    Uint8 *pLastChangeDateTime;
    Uint8 *pImageDescription;
    Uint8 *pMake;
    Uint8 *pModel;
    Uint8 *pSoftware;
    Uint8 *pArtist;
    Uint8 *pCopyright;
    Uint32 ulImageWidth;
    Uint32 ulImageHeight;
    Uint32 ulExifSize;
    EXIF_THUMBNAIL_INFO pThumbnailInfo;
    Uint32 nFields;                                        /* total number of 
                                                            * fields in the
                                                            * EXIF format
                                                            * that are to be
                                                            * supported */

} EXIF_INFO_SUPPORTED;

typedef enum {
    TAG_TYPE_BYTE = 0x1,
    TAG_TYPE_ASCII = 0x2,
    TAG_TYPE_SHORT = 0x3,
    TAG_TYPE_LONG = 0x4,
    TAG_TYPE_RATIONAL = 0x5,
    TAG_TYPE_UNDEFINED = 0x7,
    TAG_TYPE_SLONG = 0x9,
    TAG_TYPE_SRATIONAL = 0xA
} EXIF_TAG_TYPE;

typedef struct {
    Uint16 width;
    Uint16 height;
    Uint8 nComponents;
    unsigned char qmat[128];
    unsigned short *ctl_tbl;
    unsigned short dc_y_dcd_size;
    unsigned short ac_y_dcd_size;
    unsigned short dc_uv_dcd_size;
    unsigned short ac_uv_dcd_size;
    unsigned short *dc_y_dcd_tbl;
    unsigned short *ac_y_dcd_tbl;
    unsigned short *dc_uv_dcd_tbl;
    unsigned short *ac_uv_dcd_tbl;
    unsigned char scan_header[20];
    Bool sos_marker;
    Bool sof_marker;
    Bool dht_marker1;
    Bool dht_marker2;
    Uint32 uInputframesize;
    Uint16 h1v1;
    Uint16 h2v2;
    Uint16 h3v3;
    Uint16 yuv_mode;
    EXIF_INFO_SUPPORTED exif_info;

} DecTblParams;

#define MARKER_MARKER 0xff
#define MARKER_SOI    0xd8
#define MARKER_EOI    0xd9
#define MARKER_SOS    0xda
#define MARKER_DQT    0xdb
#define MARKER_DNL    0xdc
#define MARKER_DRI    0xdd
#define MARKER_DHP    0xde
#define MARKER_EXP    0xdf
#define MARKER_DHT    0xc4
#define MARKER_SOF    0xc0
#define MARKER_RSC    0xd0
#define MARKER_APP0    0xe0

#define MARKER_JPG    0xf0
#define MARKER_APP1  0xe1

#define HEADER2_OFST_HEIGHT  5
#define HEADER2_OFST_H1V1   11
#define HEADER2_OFST_H2V2   14
#define HEADER2_OFST_H3V3   17

#define JPEG_DEC_SOK                 (1)                   /* Success */
#define JPEG_DEC_FAILED           (-1)                     /* Generic failure 
                                                            */
#define JPEG_DEC_NOTSUPPORTED (-2)                         /* Format (YUV,
                                                            * Height, Width
                                                            * or QF not
                                                            * Supported */
#define JPEG_DEC_INVPARAMS        (-3)
#define JPEG_DEC_INITFAILED        (-4)
#define JPEG_DEC_NOMEMORY        (-5)
#define JPEG_DEC_INVALIDHANDLE  (-6)
#define JPEG_DEC_DEINITFAILED    (-7)

typedef Int16 Jpeg_Dec_status;

#define JPEG_EXIT_IF(_Cond,_ErrorCode) { \
    if ((_Cond)) { \
        status = _ErrorCode; \
        printf ("Error :: %s : %s : %d :: Exiting because : %s\n", \
                __FILE__, __FUNCTION__, __LINE__, #_Cond); \
        goto EXIT; \
    } \
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
