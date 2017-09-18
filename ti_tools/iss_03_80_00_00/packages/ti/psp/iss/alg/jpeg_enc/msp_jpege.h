/** ==================================================================
 *  @file   msp_jpege.h                                                  
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
/* ==================================================================== Texas 
 * Instruments OMAP(TM) Platform Software (c) Copyright Texas Instruments,
 * Incorporated. All Rights Reserved. Use of this software is controlled by
 * the terms and conditions found in the license agreement under which this
 * software has been supplied.
 * ==================================================================== */
/* -------------------------------------------------------------------------- 
 */
/* 
 * msp_jpege.h
 * This header file defines the public interface of MSP JPEGE module.
 *
 * @path alg\jpeg_enc\
 *
 *@rev 1.0
 */
/* -------------------------------------------------------------------------- 
 */
/* ==========================================================================
 * ! ! Revision History ! =================================== ! ! 05-Feb-2009
 * Phanish HS [phanish.hs@ti.com]: Initial version ! 16-Apr-2009 Phanish:
 * Resource Manager Integration. And Simcop Interrupt manager testing !
 * ========================================================================== */

#ifndef _MSP_JPEGE_H_
#define _MSP_JPEGE_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

    /* User code goes here */
    /* ------compilation control switches
     * ---------------------------------------- */
/****************************************************************
 * INCLUDE FILES
 ****************************************************************/
    /* ----- system and platform files -------------------------- */
/*-------program files ----------------------------------------*/
#include "inc/msp.h"
    typedef int *RM_HANDLE;

    // #define __DEBUG_STEPS__

/*--------------------------- END -------------------------------*/
/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes --------------------------------*/
    // extern void JpegCoreProcessFunction (int argc, void *argv);
/*--------------------------- END -------------------------------*/

/****************************************************************
 * PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations ----------------------------------*/
/*--------macros -----------------------------------------------*/
#define NUM_OF_PORTS 2
/*--------------------------- END ------------------------------*/
/** End of Stream Buffer Flag:
  *
  * A component sets EOS when it has no more data to emit on a particular
  * output port. Thus an output port shall set EOS on the last buffer it
  * emits. A component's determination of when an output port should
  * cease sending data is implemenation specific.
  * @ingroup buf
  */

#define MSP_BUFFERFLAG_EOS 0x00000001
#define DEFAULT_BYTE_ALIGN 4
#define SIXTEEN_BYTE_ALIGN 16
#define DHT_TABLE_WIDTH 8
#define DHT_TABLE_HEIGHT 256
#define DHT_TABLE_ENTRIES (DHT_TABLE_WIDTH * DHT_TABLE_HEIGHT)  /* 4096 bytes 
                                                                 * = 2048
                                                                 * short */

#define DQT_WIDTH 8
#define DQT_HEIGHT 16
#define DQT_ENTRIES (DQT_WIDTH * DQT_HEIGHT)

#define MIN_NUM_SDMA_CHANNELS 1

#define MSP_JPEGE_MCU_HEIGHT_IN_PIXELS_YUV420   16
#define MSP_JPEGE_MCU_HEIGHT_IN_PIXELS_YUV422    8
#define MSP_JPEGE_MCU_WIDTH_IN_PIXELS           16

#define MSP_JPEGE_APP0_OVERFLOW   (0x06 << 8 | MSP_ERROR_FAIL)
#define MSP_JPEGE_APP1_OVERFLOW   (0x07 << 8 | MSP_ERROR_FAIL)
#define MSP_JPEGE_APP13_OVERFLOW  (0x08 << 8 | MSP_ERROR_FAIL)
    typedef struct JPEGParams MSP_JPEG_DRV_PARAMS;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_PORT_TYPE - Port definition for input and output ports
     * @param MSP_JPEGE_INPUT_PORT : Input port @param MSP_JPEGE_OUTPUT_PORT
     * : Output port */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_JPEGE_INPUT_PORT = 0,
        MSP_JPEGE_OUTPUT_PORT = 1
    } MSP_JPEGE_PORT_TYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_COLORFORMAT_TYPE - Color format type enumeration @param
     * MSP_JPEGE_COLORFORMAT_UNUSED : Default @param
     * MSP_JPEGE_COLORFORMAT_YUV420PLANAR : 420 Planar format @param
     * MSP_JPEGE_COLORFORMAT_YUV422INTERLEAVED : 422 interleaved @param
     * MSP_JPEGE_COLORFORMAT_MONOCHROME : Monochrome (Y Only) */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_JPEGE_COLORFORMAT_UNUSED = 0,
        MSP_JPEGE_COLORFORMAT_YUV420NV12,
        MSP_JPEGE_COLORFORMAT_YUV422INTERLEAVED,
        MSP_JPEGE_COLORFORMAT_YUV420PLANAR,
        MSP_JPEGE_COLORFORMAT_YUV444INTERLEAVED,
        MSP_JPEGE_COLORFORMAT_MONOCHROME
    } MSP_JPEGE_COLORFORMAT_TYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE - Operation mode of
     * the encoder input buffer @param MSP_JPEGE_SLICE_MODE : Slice based
     * operation (Memory src alone) @param MSP_JPEGE_FRAME_MODE : Frame based 
     * operation @param MSP_JPEGE_BURST_MODE : For Burst mode of capture (For 
     * camera src) @param MSP_JPEGE_STITCH_MODE : For stitching the encoder
     * outputs (Memory) */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_JPEGE_SLICE_MODE = 0,
        MSP_JPEGE_FRAME_MODE = 1,
        MSP_JPEGE_BURST_MODE = 2,
        MSP_JPEGE_STITCH_MODE = 3
    } MSP_JPEGE_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_COMPRESSEDBUFFER_OPERATINGMODE_TYPE - Operation mode of the
     * encoder output buffer @param MSP_JPEGE_CHUNK_MODE : Chunk based
     * operation @param MSP_JPEGE_NONCHUNK_MODE : Non-chunk based operation */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_JPEGE_CHUNK_MODE = 0,
        MSP_JPEGE_NONCHUNK_MODE = 1
    } MSP_JPEGE_COMPRESSEDBUFFER_OPERATINGMODE_TYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_HEADERTYPE - An enumerated type giving the header packaging
     * format used for the encoded bit stream @param MSP_JPEGE_NOEXTRAHEADER 
     * : No additional packaging format used @param MSP_JPEGE_JFIF : JFIF
     * interchange format @param MSP_JPEGE_EXIF : EXIF interchange format */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_JPEGE_NOEXTRAHEADER,
        MSP_JPEGE_JFIF,
        MSP_JPEGE_EXIF
    } MSP_JPEGE_HEADERTYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_USECASETYPE - An enumerated type giving the top level
     * usecases @param MSP_JPEGE_HQ : Advanced still image capture @param
     * MSP_JPEGE_HSS : High Speed still image capture @param MSP_JPEGE_OTHER
     * : Others */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_JPEGE_HQ = 0,
        MSP_JPEGE_HSS = 1,
        MSP_JPEGE_OTHER = 2
    } MSP_JPEGE_USECASETYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_INDEX_TYPE - Image operation specific configuration @param
     * MSP_JPEGE_INDEX_INTERCHANGEFORMATTYPE: Index for choosing JFIF or EXIF
     * @param MSP_JPEGE_INDEX_QFACTOR : Index for Quality factor configuration
     * @param MSP_JPEGE_INDEX_ENCODESOURCE : Index for source selection (Encode 
     * from camera or memory) @param MSP_JPEGE_INDEX_ROTATIONPARAM : Rotation
     * Param @param MSP_JPEGE_INDEX_COLORFORMAT : Index for color format
     * specification */
    /* ======================================================================= 
     */

    typedef enum {
        MSP_JPEGE_INDEX_QFACTOR = 0,
        MSP_JPEGE_INDEX_QUANTTABLE = 1,
        MSP_JPEGE_INDEX_HUFFTABLE = 2,
        MSP_JPEGE_INDEX_RESTARTITVL = 3
            // MSP_JPEGE_INDEX_SOURCE,
            // MSP_JPEGE_INDEX_ROTATIONPARAM
    } MSP_JPEGE_INDEX_TYPE;

    /* ======================================================================= 
     */
    /* MSP_JPEGE_ROTATION_TYPE - Rotation selection for JPEG encoding while
     * operating in frame mode and encoding from memory @param
     * MSP_JPEGE_ROTATION_UNUSED : No rotation @param MSP_JPEGE_ROTATION_90 : 
     * Rotate by 90 @param MSP_JPEGE_ROTATION_180 : Rotate by 180 @param
     * MSP_JPEGE_ROTATION_270 : Rotate by 270 */
    /* ======================================================================= 
     */
    typedef enum {
        MSP_JPEGE_ROTATION_UNUSED = 0,
        MSP_JPEGE_ROTATION_90,
        MSP_JPEGE_ROTATION_180,
        MSP_JPEGE_ROTATION_270
    } MSP_JPEGE_ROTATION_TYPE;

/* ======================================================================= */
/* MSP_JPEGE_CREATE_PARAM -  Create time parameter structure
 *
 * @param ulImageWidth          : Input frame width 
 * @param ulImageHeight         : Input frame height 
 * @param ulImageStrideHorizontal  : Input Stide 
 * @param ulInputSliceHeight    : Input slice height, if used or else 0
 * @param ulQualityFactor       : Quality Factor required at the output
 * @param ulBurstLength         : Number of burst frames captured
 * @param ulOutputChunkSize     : Size of the output bitstream, when operated
 *                                in Chunk based mode at the output
 * @param nMaxNumOfBuf          : Maximum number of buffers per port
 * @param eColorFormat          : Color format used
 * @param eUseCaseType          : Use-Case (HSS, HQ) type used
 * @param eUnComprsdOpMode      : Input operating mode (Slice, frame, etc)
 * @param eComprsdOpMode        : Output operation mode (chunk/non-chunk mode)
 * @param eRotationParam        : Rotation param to be specified 
 * @param bCustomQuantTab       : Is MSP_TRUE, if application provides custom
 *                                quantification or inverse quant table   
 * @param bCustomHuffTab        : Is MSP_TRUE, if application provides custom
 *                                Huffman table 
 * @param eHeaderFormat         : Specified whether EXIF or JFIF Header needs ot be encoded
 * @param bDRIEnable            : Specifies whether DRI Header needs to be encoded
 * @param nSdmaChannel          : DMA Channel to be used
 * @param bApp13MarkerEnable    : Flag to enable APP13 Marker
 */
/* ======================================================================= */
    typedef struct {
        MSP_U32 ulImageWidth;
        MSP_U32 ulImageHeight;
        MSP_U32 ulImageStrideHorizontal;
        MSP_U32 ulInputSliceHeight;
        MSP_U32 ulQualityFactor;
        MSP_U32 ulBurstLength;
        MSP_U32 ulOutputChunkSize;
        MSP_U32 nMaxNumOfBuf[NUM_OF_PORTS];
        MSP_JPEGE_COLORFORMAT_TYPE eColorFormat;
        MSP_JPEGE_USECASETYPE eUseCaseType;
        MSP_JPEGE_UNCOMPRESSEDBUFFER_OPERATINGMODE_TYPE eUnComprsdOpMode;
        MSP_JPEGE_COMPRESSEDBUFFER_OPERATINGMODE_TYPE eComprsdOpMode;
        MSP_JPEGE_ROTATION_TYPE eRotationParam;
        MSP_BOOL bCustomHuffTab;
        MSP_BOOL bCustomQuantTab;
        MSP_JPEGE_HEADERTYPE eHeaderFormat;
        MSP_BOOL bDRIEnable;
        MSP_U32 ulRestartInterval;
        MSP_U32 nSdmaChannel;
        MSP_BOOL bApp13MarkerEnable;
    } MSP_JPEGE_CREATE_PARAM;
/****************************************************************
 * PRIVATE DECLARATIONS Defined here, used only here
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes --------------------------------*/
/*--------macros ------------------------------------------------*/
/*--------------------------- END -------------------------------*/
char * getVersion_JPEGENC();
#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     /* !_MSP_JPEGE_H_ */
