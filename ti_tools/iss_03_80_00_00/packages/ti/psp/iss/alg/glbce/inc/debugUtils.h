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


/* Certain utilities for debugging purposes */

#include <stdio.h> 
#include <math.h>
#include <stdlib.h>
#include <string.h>

void dispShortArray(Word16* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void dispByteArray(Byte* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void dispLongArray(long* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void dispLongArrayFP(long* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim, Word32 scale);
void dispShortArrayFP(Word16* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim, Word32 scale);
void writeByteArray(char* filename, Byte* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void writeShortArray(char* filename, Word16* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void writeUShortArray(char* filename, uWord16* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void writeLongArray(char* filename, Word32* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void readShortArray(char* filename, Word16* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void readLongArray(char* filename, Word32* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void writeRO(char* filename, Word16* ROy, Word16* ROx, Word32 N);
void writeSC(char* filename, uWord16* ROy, uWord16* ROx, Word32 N);
void writeMVscale(char* filename, uWord16 MVscaleY, uWord16 MVscaleX);
void writeByteArrayBinary (char* filename, Byte* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void writeShortArrayBinary(char* filename, Word16* A, Word32 nItems, Word16 init, Word16 isFirstFrame);
void writeIntArrayBinary  (char* filename, Word32* A, Word16 shift, Word32 nItems, Word16 init, Word16 isFirstFrame);
void writeIntArrayAsShortBinary(char* filename, Word32* A, Word32 nItems, Word16 init, Word16 isFirstFrame);
void writeUShortArrayBinary(char* filename, uWord16* A, Word32 nItems, Word16 init, Word16 isFirstFrame);
void dispUShortArray(uWord16* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void dispUIntArray(uWord32* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);
void writeUIntArrayBinary(char* filename, uWord32* A, Word16 shift, Word32 nItems, Word16 init, Word16 isFirstFrame);
void writeFloatArray(char* filename, float* A, Word32 nItems, Word16 init, Word16 isFirstFrame);
void dispIntArray(Word32* A, Word32 xL, Word32 xR, Word32 yU, Word32 yB, Word32 xdim, Word32 ydim);





#ifdef __cplusplus
}
#endif /* __cplusplus */
