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


#ifndef _TEST_PLATFORM_INCLUDED_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _TEST_PLATFORM_INCLUDED_

#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <io.h>
#include "dataTypeDefs.h"
#include "bmp_util.h"

//#define NO_PRINTF      //disables all printf statements

//Maximum image size
#define MAX_FRAME_SIZE_V                 4000           
#define MAX_FRAME_SIZE_H                 5000           


//Test platform structure. Contains all parameters for the test platform.
typedef struct {
    char	gainTableFileName[500];
	char    inputFileName[500];
	char    outputFileName[500];
	imgFormatType    inputImageFormat;
	imgFormatType    outputImageFormat;
	FILE*            inputFile;
	Word32           inputFilePosition;
	FILE*            outputFile;
	Byte*            bmpMemory;
	Byte             *red,*green,*blue;
	pixComponentType upperLeftPixInBayer;
	Word16           is16bits;
	endianType       endian;  
	gainFileFormatType gainFileFormat;

	Byte*    inputImageBuffer;
	Byte*    outputImageBuffer;
	Word16*  boxcarMemory;
	Word16*  bayerMemory;
	Word16*  bayerImage;
	float*   GainMap;
	Word32*  histogramMemory;
	uWord16* gainTable;
	Word16   numLSCbits;
	Word16   numLSCfractionBits;
	float    maxGain;
	Word16   glbceStatus;
// ISP SIMULATOR FORMAT FOR GLBCE / GBCE

	char	BayerInputFileName[500];
	char	boxcarFileName[500];
	char	HistogramInputFileName[500];
	char	GammaInputFileName[500];
	char	BayerOutputFileName[500];
	char	LocalToneInputFileName[500];
	char	GlobalToneFileName[500];
	Byte	downsampleRatio;
	Word16	imgHorzSize;
	Word16	imgVertSize;
	Byte	numBits;
	Word16	WB_RGain;
	Word16	WB_GrGain;
	Word16	WB_GbGain;
	Word16	WB_BGain;
	Word16	WB_ROffset;
	Word16	WB_GrOffset;
	Word16	WB_GbOffset;
	Word16	WB_BOffset;
	char	CaptureParamsFile[500];
	Word16	GBEStrength;
	Word16	GCEStrength;
	Word16	RRCoef;
	Word16	RGCoef;
	Word16	RBCoef;
	Word16	GRCoef;
	Word16	GGCoef;
	Word16	GBCoef;
	Word16	BRCoef;
	Word16	BGCoef;
	Word16	BBCoef;
	Word16	ROffset;
	Word16	GOffset;
	Word16	BOffset;
	Word16	RRCoef1;
	Word16	RGCoef1;
	Word16	RBCoef1;
	Word16	GRCoef1;
	Word16	GGCoef1;
	Word16	GBCoef1;
	Word16	BRCoef1;
	Word16	BGCoef1;
	Word16	BBCoef1;
	Word16	ROffset1;
	Word16	GOffset1;
	Word16	BOffset1;
	char	DDDLUT_TABLE_R[500];
	char	DDDLUT_TABLE_G[500];
	char	DDDLUT_TABLE_B[500];
	Word16	GAIN_RY;
	Word16	GAIN_GY;
	Word16	GAIN_BY;
	Word16	GAIN_RCB;
	Word16	GAIN_GCB;
	Word16	GAIN_BCB;
	Word16	GAIN_RCR;
	Word16	GAIN_GCR;
	Word16	GAIN_BCR;
	Word16	OFFSET_Y;
	Word16	OFFSET_CB;
	Word16	OFFSET_CR;
	Byte	SensorType;
	Word16	LBEStrength;
	Word16	LCEStrength;

} TPstruct;


void initializeTestPlatform(TPstruct* tp);
void loadInputImageFromFile(TPstruct *tp);
void saveOutputImageToFile(TPstruct *tp);
void closeTestPlatform(TPstruct* tp);
void simulateBoxcar(Word16* inpImg, Word16* boxcarOutput, Word16 downsampleRatio, Word16 imgSizeV, Word16 imgSizeH, pixComponentType upperLeftPixInBayer);
void loadBoxcarOutputFromFile(TPstruct* tp);
void simulateLSC(TPstruct* tp);
void downsampleY(Byte* inpImg, Word16* outImg, Word16 downsampleRatio, Word16 imgSizeV, Word16 imgSizeH);
void saveGainTableToFile(TPstruct* tp);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif





