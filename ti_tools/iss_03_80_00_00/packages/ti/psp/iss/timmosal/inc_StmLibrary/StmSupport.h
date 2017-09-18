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
 *  ======== StmSupport.h ========
 *  This is a support header file for STM Library. This
 *  file is intended for internal use only.
 */

#ifndef __STMSUPPORT_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __STMSUPPORT_H

#define STM_WORD_SIZE			(4)
#if 0
#define OST_VERSION				(1)						//Restrictions: value must be between 1 and 15
#define OST_ENTITY_ID			(1)
#define OST_PROTOCOL_PRINTF		(0x80)					//STMXport_printf
#define OST_PROTOCOL_BYTESTREAM	(OST_PROTOCOL_PRINTF+1)	//STMXport_putMsg
#define OST_PROTOCOL_MSGSTREAM	(OST_PROTOCOL_PRINTF+2)	//STMXport_putBuf
#else
#define OST_VERSION				(1 << 24)						//Restrictions: value must be between 1 and 15
#define OST_ENTITY_ID			(1 << 16)
#define OST_PROTOCOL_PRINTF		(0x80 << 8)						//STMXport_printf
#define OST_PROTOCOL_BYTESTREAM	(0x81 << 8)	//STMXport_putMsg
#define OST_PROTOCOL_MSGSTREAM	(0x82 << 8)	//STMXport_putBuf
#endif
#define OST_SHORT_HEADER_LENGTH_LIMIT	(255)

#ifdef C55XP_STM
//C55p is big endiam
	#define L0			(3)
	#define L1			(2)
	#define L2			(1)
	#define L3			(0)
#else
//ARM assumption is little
	#define L0			(0)
	#define L1			(1)
	#define L2			(2)
	#define L3			(3)
#endif

#define MAX_DMA_CHANNEL (31)
#define MAX_DMA_IRQ     (3)
#define STM_MAX_CHANNEL (254)
#define OST_SMALL_MSG   (255)

typedef enum
{
	eByteAlign = 1,
	eShortAlign = 2,
	eShortAndByteAlign = 3,
	eWordAlign = 4
}eAddrAlignment;

// DMA
#define DMA_BaseAddr			((unsigned long)&DMA4_BaseAddress)
#define DMA_OCPCntl				(DMA_BaseAddr + 0x2C )
#define DMA_FIFO            	(DMA_BaseAddr + 0x78 )
#define DMA_IrqOffset           (0x4)
#define DMA_IrqStatus(irqID)	((DMA_BaseAddr  + (irqID * DMA_IrqOffset)) + 0x8)
#define DMA_IrqEnable(irqID)    ((DMA_BaseAddr  + (irqID * DMA_IrqOffset)) + 0x18)			
#define DMA_ChOffset    		(0x60)
#define DMA_ChCntl(ch) 			((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x80 )
#define DMA_ChSrcDstPrms(ch)	((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x90 )
#define DMA_ChElem(ch)			((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x94 )
#define DMA_ChFram(ch)			((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x98 )
#define DMA_ChSrcAdd(ch)		((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x9C )
#define DMA_ChDstAdd(ch)		((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0xA0 )
#define DMA_ChStatus(ch)		((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x8C )
#define DMA_ChLnkCntl(ch)		((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x84 )
#define DMA_ChIntCntl(ch)		((DMA_BaseAddr + (ch * DMA_ChOffset)) + 0x88 )

//DMA Status register bits of interest
#define DMA_ERROR_MASK 			(0x2F00)
#define DMA_FRAME_COMPLETE		(0x1<<3)
#define DMA_SYSTEM_ERROR		(0x1<<13)
#define DMA_MISSALIGND_ERROR 	(0x1<<11)
#define DMA_SUPERVISOR_ERROR	(0x1<<10)
#define DMA_SECURE_ERROR		(0x1<<9)
#define DMA_TRASACTION_ERROR	(0x1<<8)

//DMA Control bits
#define DMA_CONTROL_MASK 		(0x80)
#define DMA_ENABLE 				(0x1<<7)

#define VALIDATE_NULL_VALUE(value) if (NULL == value) return eSTM_ERROR_PARM
#define VALIDATE_CHANNEL(chn)   if ( ( chn < 0 ) || ( chn > STM_MAX_CHANNEL ) ) return eSTM_ERROR_PARM

#define RETURN_CALLBACK(pCallBack, retValue) 		if ( NULL != pCallBack ) pCallBack(retValue); return retValue
										

typedef enum
{
	eRegular = 1,
	eTiming = 2
}eAddressType;

// Internal Functions
inline unsigned long Compose_Address(unsigned long ulBaseAddr, int chNum, unsigned long size, eAddressType addrType);
void Compose_OST_MSG(const char *pInputBuf, int iInputBufSize, const char *pOSTHeader, int iHeaderSizeInByte, char *pReturnBuf);
void Build_OST_Header(long protocolID, long numberOfBytes, long *pReturnBuf, int *pBufSizeInBytes);
inline unsigned long Build_CompactOST_Header(long protocolID, long numberOfBytes );
inline Build_ExtendedOST_Header(long protocolID, long numberOfBytes, long *pReturnBuf);
eAddrAlignment AddressAlignmentChecker(unsigned long address);

//External Functions
extern void * STM_memAlloc(size_t sizeInBytes);
extern        STM_memFree(void *);
extern void * STM_memMap(unsigned int phyAddress, size_t mapSizeInBytes);
extern        STM_memUnMap(void * vAddress, size_t mapSizeInBytes);

// Msg handling routines
eSTM_STATUS STM_putBufCPU(STMHandle * pHandle, int iChNum, void * pHdrBuf, int iHdrWrdCnt, void * pMsgBuf, unsigned int iElementCnt, eSTMElementSize iElementSize);
eSTM_STATUS STM_putMsgCPU(STMHandle * pHandle, int iChNum, void * pHdrBuf, int iHdrWrdCnt, void * pMsgBuf, unsigned int iMsgByteCnt);
eSTM_STATUS STM_putDMA(STMHandle* pHandle, int iChNum, void* pMsgBuf, unsigned int iElementCnt, eSTMElementSize iElementSize, bool DMASingleAccessMode, STMXport_callback pCallBack);

//DMA Helper routines
eSTM_STATUS DMA_checkStatus(STMHandle* pHandle);
void DMA_serviceNextMsg(STMHandle* pHandle);
void DMA_postMsg(STMHandle* pHandle, STMMsgObj * msgObj);

#pragma CODE_SECTION(Compose_Address, 			".text:STMLibraryFast")
#pragma CODE_SECTION(Compose_OST_MSG, 			".text:STMLibraryFast")
#pragma CODE_SECTION(Build_OST_Header, 			".text:STMLibraryFast")
#pragma CODE_SECTION(AddressAlignmentChecker, 	".text:STMLibraryFast")
#pragma CODE_SECTION(STM_putBufCPU, 			".text:STMLibraryFast")
#pragma CODE_SECTION(STM_putMsgCPU, 			".text:STMLibraryFast")
#pragma CODE_SECTION(STM_putDMA, 				".text:STMLibraryFast")
#pragma CODE_SECTION(DMA_checkStatus, 			".text:STMLibraryFast")
#pragma CODE_SECTION(DMA_serviceNextMsg, 		".text:STMLibraryFast")
#pragma CODE_SECTION(DMA_postMsg, 				".text:STMLibraryFast")



#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __STMSUPPORT_H */
