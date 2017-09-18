/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include <string.h>

#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <ti/sysbios/heaps/HeapMem.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/sysbios/knl/Clock.h>

/* See vpsutils_mem.h for function documentation  */

/* Memory pool handle */
HeapMem_Handle  gVpsUtils_heapMemFrameHandle;

Bool gVpsUtils_memClearBuf;

Int32 VpsUtils_memInit_internal(UInt32 *pMemAddr, UInt32 memSize)
{
  HeapMem_Params heapMemPrm;

  /* create memory pool heap  */

  HeapMem_Params_init(&heapMemPrm);

  heapMemPrm.buf = pMemAddr;
  heapMemPrm.size = memSize;

  gVpsUtils_heapMemFrameHandle = HeapMem_create(&heapMemPrm, NULL);

  GT_assert( GT_DEFAULT_MASK, gVpsUtils_heapMemFrameHandle!=NULL);

  gVpsUtils_memClearBuf = FALSE;

  return 0;
}

Int32 VpsUtils_memDeInit()
{
  /* delete memory pool heap  */
  HeapMem_delete(&gVpsUtils_heapMemFrameHandle);

  return 0;
}


/*
  Get buffer size based on data format

  pFormat - data format information
  *size - buffer size
  *cOffset - C plane offset for YUV420SP data
*/
Int32 VpsUtils_memFrameGetSize(FVID2_Format *pFormat,
    UInt32 *size, UInt32 *cOffset)
{
  UInt32 bufferHeight;
  Int32 status = 0;

  bufferHeight = pFormat->height;

  switch(pFormat->dataFormat)
  {
    case FVID2_DF_YUV422I_YUYV:
    case FVID2_DF_YUV422I_YVYU:
    case FVID2_DF_YUV422I_UYVY:
    case FVID2_DF_YUV422I_VYUY:
    case FVID2_DF_YUV444I:
    case FVID2_DF_RGB24_888:
    case FVID2_DF_RAW_VBI:

      /* for single plane data format's */
      *size = pFormat->pitch[0] * bufferHeight;
      break;

    case FVID2_DF_YUV422SP_UV:
    case FVID2_DF_YUV420SP_UV:

      /* for Y plane  */
      *size = pFormat->pitch[0] * bufferHeight;

      /* cOffset is at end of Y plane  */
      *cOffset = *size;

      if(pFormat->dataFormat==FVID2_DF_YUV420SP_UV)
      {
        /* C plane height is 1/2 of Y plane */
        bufferHeight = bufferHeight/2;
      }

      /* for C plane  */
      *size += pFormat->pitch[1] * bufferHeight;
      break;

    default:
      /* illegal data format  */
      status = -1;
      break;
  }

  /* align size to minimum required frame buffer alignment  */
  *size = VpsUtils_align(*size, VPS_BUFFER_ALIGNMENT);

  return status;
}

Int32 VpsUtils_memFrameAlloc(FVID2_Format *pFormat,
        FVID2_Frame *pFrame, UInt16 numFrames)
{
  UInt32 size, cOffset, frameId;
  Int32 status;
  UInt8 *pBaseAddr;

  /* init FVID2_Frame to 0's  */
  memset(pFrame, 0, sizeof(*pFrame));

  /* align height to multiple of 2  */
  pFormat->height = VpsUtils_align(pFormat->height, 2);

  /* get frame size for given pFormat */
  status = VpsUtils_memFrameGetSize(pFormat, &size, &cOffset);

  if(status==0)
  {
    /* allocate the memory for 'numFrames' */

    /* for all 'numFrames' memory is contigously allocated  */
    pBaseAddr = VpsUtils_memAlloc(size*numFrames, VPS_BUFFER_ALIGNMENT);

    if(pBaseAddr==NULL)
    {
      status = -1;  /* Error in allocation, exit with error */
    }
  }

  if(status==0)
  {
    /* init memory pointer for 'numFrames'  */
    for(frameId = 0 ; frameId < numFrames; frameId++)
    {

      /* copy channelNum to FVID2_Frame from FVID2_Format */
      pFrame->channelNum = pFormat->channelNum;
      pFrame->addr[0][0] = pBaseAddr;

      switch(pFormat->dataFormat)
      {
        case FVID2_DF_RAW_VBI:
        case FVID2_DF_YUV422I_UYVY:
        case FVID2_DF_YUV422I_VYUY:
        case FVID2_DF_YUV422I_YUYV:
        case FVID2_DF_YUV422I_YVYU:
        case FVID2_DF_YUV444I:
        case FVID2_DF_RGB24_888:
          break;
        case FVID2_DF_YUV422SP_UV:
        case FVID2_DF_YUV420SP_UV:
          /* assign pointer for C plane */
          pFrame->addr[0][1] = (UInt8*)pFrame->addr[0][0] + cOffset;
          break;
        default:
          /* illegal data format  */
          status = -1;
          break;
      }
      /* go to next frame */
      pFrame++;

      /* increment base address */
      pBaseAddr += size;
    }
  }

  GT_assert( GT_DEFAULT_MASK, status==0);

  return status;
}

Int32 VpsUtils_memFrameFree(FVID2_Format *pFormat,
        FVID2_Frame *pFrame, UInt16 numFrames)
{
  UInt32 size, cOffset;
  Int32 status;

  /* get frame size for given 'pFormat' */
  status = VpsUtils_memFrameGetSize(pFormat, &size, &cOffset);

  if(status==0)
  {
    /* free the frame buffer memory */

    /* for all 'numFrames' memory is allocated contigously during alloc,
        so first frame memory pointer points to the complete
        memory block for all frames
     */
    VpsUtils_memFree(pFrame->addr[0][0], size*numFrames);
  }

  return 0;
}

Ptr   VpsUtils_memAlloc(UInt32 size, UInt32 align)
{
  Ptr addr;

  /* allocate memory  */
  addr = HeapMem_alloc(gVpsUtils_heapMemFrameHandle, size, align, NULL);

  if(addr!=NULL && gVpsUtils_memClearBuf)
    memset(addr, 0x80, size);

  return addr;
}

Int32 VpsUtils_memFree(Ptr addr, UInt32 size)
{
  /* free previously allocated memory  */
  HeapMem_free(gVpsUtils_heapMemFrameHandle, addr, size);

  return 0;
}

Int32 VpsUtils_memClearOnAlloc(Bool enable)
{
    gVpsUtils_memClearBuf = enable;

#ifdef PLATFORM_ZEBU
    gVpsUtils_memClearBuf = FALSE;
#endif

    return 0;
}

UInt32 VpsUtils_memGetSystemHeapFreeSpace(void)
{
    Memory_Stats                stats;
    extern const IHeap_Handle   Memory_defaultHeapInstance;

    Memory_getStats(Memory_defaultHeapInstance, &stats);

    return ((UInt32) (stats.totalFreeSize));
}

void VpsUtils_memPrintSystemHeapStatus()
{

    return;
}

UInt32 VpsUtils_memGetBufferHeapFreeSpace(void)
{
    Memory_Stats                stats;

    HeapMem_getStats(gVpsUtils_heapMemFrameHandle, &stats);

    return ((UInt32) (stats.totalFreeSize));
}
