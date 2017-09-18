/*=======================================================================
 *
 *            Texas Instruments Internal Reference Software
 *
 *                 Systems and Applications R&D Center
 *                    Video and Image Processing Lab
 *                           Imaging Branch         
 *
 *         Copyright (c) 2010 Texas Instruments, Incorporated.
 *                        All Rights Reserved.
 *      
 *
 *          FOR TI INTERNAL USE ONLY. NOT TO BE REDISTRIBUTED.
 *
 *							TI CONFIDENTIAL
 *
 *======================================================================*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#include "dataTypeDefs.h"
#include "glbceStruct.h"

//void loadFromExternalMemory(Byte* extMem, Byte* intMem, Word16 extFrmPels, Word16 intFrmPels, Word16 extVoffset, Word16 extHoffset, Word16 blkLines, Word16 blkPels);
//void writeToExternalMemory(Byte* extMem, Byte* intMem, Word16 extFrmPels, Word16 intFrmPels, Word16 extVoffset, Word16 extHoffset, Word16 blkLines, Word16 blkPels);
void unPackBayer(uWord16* inpBlk, uWord16* outBlk, Word16 blkSizeV, Word16 blkSizeH, Word16 index, Word16 numBitsPerPixel);
void packBayer(Word16* inpBlk1, Word16* inpBlk2, uWord16* outBlk, Word16 blkSizeV, Word16 blkSizeH, Word16 numBitsPerPixel);
void init2zero(Word16 *imgBuf, Word16  imgWidth, Word16  imgHeight);
void blockCopy(Word16 *inpBlk, Word16 *outBlk, Word16  inpWidth, Word16  blkWidth, Word16  blkHeight);
void unPackYCbCr422image(Byte* inpImg, Byte* Ybuffer, Byte* CbBuffer, Byte* CrBuffer, Word16 imgSizeV, Word16 imgSizeH, Word16 inpImgSizeV, Word16 inpImgSizeH);
void downsampleImage(Word16* inpImg, Word16* outImg, Word16 ratio, Word16 imgSizeV, Word16 imgSizeH);
void upsampleImage(Word16* inpImg, Word16* outImg, Word16 ratio, Word16 imgSizeV, Word16 imgSizeH);
void packYCbCr422image(Byte* inpImg, Byte* Ybuffer, Byte* CbBuffer, Byte* CrBuffer, Word16 imgSizeV, Word16 imgSizeH, Word16 inpImgSizeV, Word16 inpImgSizeH);
void mapCoeffs(Word16* imgBuf, Word16 imgWidth, Word16 imgHeight, Word16 threshold, Word16  vertShift, Word16 horzShift, Word16 *adapThresh, Word16 adapThrSizeV, Word16 adapThrSizeH, Word16 nlev, float slope);
void createToneCurve(Word16 numPoints, Word16* xPoints, Word16* yPoints, Byte* toneMap);
void calculateToneCurveAtOnlyTwoPoints(Word16 numPoints, Word16* xPoints, Word16* yPoints, Byte* toneMap, Word16 point1, Word16 point2);
#ifdef __cplusplus
}
#endif /* __cplusplus */
