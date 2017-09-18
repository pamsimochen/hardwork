/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
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
 *
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

/*
 *  ======== StmLibrary.h ========
 *  This is the public interface of StmLibrary.
 */



#ifndef __STMLIBRARY_H
#define __STMLIBRARY_H

#ifdef _CIO
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define STMLIB_MAJOR_VERSION	(0x1)
#define STMLIB_MINOR_VERSION    (0x6)

typedef enum
{ 
	true = 1,
	false = 0
}bool;


typedef enum{
	
	eSTM_SUCCESS = 0, 						//No error, message completed
	eSTM_PENDING = 1,						//No error, message buffered 
	eSTM_ERROR_PARM = -1,					//Error, method parameter error)
	eSTM_ERROR_STRING_FORMAT = -2,			//Error, Printf string format error
	eSTM_ERROR_HEAP_ALLOCATION = -3,		//Error , Can not allocate heap error
	eSTM_ERROR_BUF_ALLOCATION = -4,			//Error, message to large for remaining buffer space 
	eSTM_ERROR_SYSTEM =-5,					//Error, DMA system access error
	eSTM_ERROR_MISSALIGNED_ADDR = -6,		//Error, DMA miss-aligned address error
	eSTM_ERROR_SUPERVISOR = -7,				//Error, DMA supervisor transaction error 
	eSTM_ERROR_SECURE = -8,					//Error, DMA secure transaction error
	eSTM_ERROR_TRANSACTION = -9,			//Error, DMA transaction error
	eSTM_ERROR_INTDISABLED = -10,			//Error, DMA interrupt not enabled
	eSTM_ERROR_INTINVALID = -11,			//Error, DMA expected IRQ status bit not set
	eSTM_ERROR_INVALID_FUNC = -12,			//Error, Function not supported by _COMPACT build
	eSTM_ERROR_CIO_ERROR = -13				//Error, CIO error occured

}eSTM_STATUS;

typedef enum
{
	eByte = 1,
	eShort = 2,
	eWord = 4
}eSTMElementSize;


#ifdef _CIO
typedef struct _STMBufObj 
{
	char        * pFileName;					// Used in eBlockingCIO case only! CIO filename for STM destination.
	                                            //  if NULL stdout used
	bool          fileAppend;                   // Preserve contents of file and append 
	char        * pMasterId;                    // Used in eBlockingCIO case only! Pointer to client provided master name  

}STMBufObj;
#else
typedef struct _STMBufObj 
{
	unsigned long maxBufSize;					//max heap space this interface can allocate
	unsigned int  DMAChan_0;					//DMA channel ID used for messages
	unsigned int  DMAChan_1;					//DMA channel ID used for timestamp
	bool 		  usingDMAInterrupt;			//using DMA interrupt flag
	unsigned int  DMAIrq;						//If using DMA interrupts, user must specify an IRQ to use
}STMBufObj;
#endif

typedef void(*STMXport_callback)(eSTM_STATUS);

typedef struct _STMMsgObj 
{
	void * 				pMsg;					// Pointer to message
	unsigned int		elementCnt;				// number of elements
	eSTMElementSize 	elementSize;			// element size
	int 				chNum;					// STM channel id
	bool 				DMA_SingleAccessMode;	// Use Burst or Single Access modes
	STMXport_callback 	pCallBack;				// Callback function
	bool 				DMA_posted;				// message posted to DMA
	struct _STMMsgObj * pNextMsg;				// next message in the link list
	struct _STMMsgObj * pPrevMsg;				// prev message in the link list
}STMMsgObj;

//Note: the contents of the handle pointer is private to the library and is subject to change with revision
#ifdef _CIO
typedef struct _STMHandle
{
	FILE *       pLogFileHdl;
	STMBufObj    BufIOInfo;

}STMHandle;
#else
typedef struct _STMHandle
{
	unsigned long ulBaseAddress;
	unsigned long chResolution;
	bool 		  BufIOEnable;
#ifndef _COMPACT
	STMBufObj     BufIOInfo;
#endif
	int 		  BufUsed;
	STMMsgObj*    pHeadMsgObj;
}STMHandle;
#endif
// Public Functions Interfaces
STMHandle* STMXport_open(STMBufObj * pSTMBufObj);
eSTM_STATUS STMXport_printf(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, ... );
eSTM_STATUS STMXport_putMsg(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, int iMsgByteCount);
eSTM_STATUS STMXport_putBuf(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, void* pDataBuf, eSTMElementSize elementSize, int elementCount);
eSTM_STATUS STMXport_getBufInfo(STMHandle* pSTMHandle, unsigned long * msgCnt, unsigned long * curMsgBufSize);
eSTM_STATUS STMXport_flush(STMHandle* pSTMHandle);
eSTM_STATUS STMXport_close(STMHandle* pSTMHandle);
eSTM_STATUS STMXport_DMAIntService(STMHandle* pSTMHandle);
eSTM_STATUS STMXport_getVersion(STMHandle* pSTMHandle, unsigned int * pSTMLibMajorVersion, unsigned int * pSTMLibMinorVersion);
eSTM_STATUS STMXport_putWord(STMHandle* pSTMHandle, int chNum,  STMXport_callback pCallBack, long data);
eSTM_STATUS STMXport_putShort(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, short data);
eSTM_STATUS STMXport_putByte(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, char data);
eSTM_STATUS STMXport_logMsg(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, ...);
eSTM_STATUS STMXport_logMsg0(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString);
eSTM_STATUS STMXport_logMsg1(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, long parm1);
eSTM_STATUS STMXport_logMsg2(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, long parm1, long parm2);
eSTM_STATUS STMXport_printfV(STMHandle* pSTMHandle, int chNum, STMXport_callback pCallBack, const char* pMsgString, va_list arg_addr);


#pragma CODE_SECTION(STMXport_open, 		".text:STMLibrarySlow")
#pragma CODE_SECTION(STMXport_printf, 		".text:STMLibrarySlow")
#pragma CODE_SECTION(STMXport_putMsg, 		".text:STMLibrarySlow")
#pragma CODE_SECTION(STMXport_putBuf, 		".text:STMLibrarySlow")
#pragma CODE_SECTION(STMXport_putWord, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_putShort, 	".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_logMsg, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_logMsg0, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_logMsg1, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_logMsg2, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_putByte, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_getBufInfo,	".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_flush, 		".text:STMLibraryFast")
#pragma CODE_SECTION(STMXport_close, 		".text:STMLibrarySlow")
#pragma CODE_SECTION(STMXport_DMAIntService,".text:STMLibraryISR")


#ifdef __cplusplus
}
#endif

#endif /* __STMLIBARY_H */
