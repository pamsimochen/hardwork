/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>



#define VPSUTILS_TILER_CNT_8BIT_SIZE    (80*MB)
#define VPSUTILS_TILER_CNT_16BIT_SIZE   (48*MB)
#define VPSUTILS_TILER_CNT_32BIT_SIZE   ( 0*MB)

#define VPSUTILS_TILER_MAX_HEIGHT_8BIT   (VPSUTILS_TILER_CNT_8BIT_SIZE /       \
                                          VPSUTILS_TILER_CNT_8BIT_PITCH)
#define VPSUTILS_TILER_MAX_HEIGHT_16BIT  (VPSUTILS_TILER_CNT_16BIT_SIZE /      \
                                          VPSUTILS_TILER_CNT_16BIT_PITCH)
#define VPSUTILS_TILER_MAX_HEIGHT_32BIT  (VPSUTILS_TILER_CNT_32BIT_SIZE /      \
                                          VPSUTILS_TILER_CNT_32BIT_PITCH)

#define VPSUTILS_TILER_CNT_8BIT_MAX_LINES   (8192u)
#define VPSUTILS_TILER_CNT_16BIT_MAX_LINES  (4096u)
#define VPSUTILS_TILER_CNT_32BIT_MAX_LINES  (4096u)

#define VPSUTILS_TILER_REG_BASE                       (0x4E000100u)
#define VPSUTILS_TILER_DMM_PAT_VIEW_MAP_BASE          (0x4E000460u)
#define VPSUTILS_TILER_DMM_PAT_VIEW_MAP__0            (0x4E000440u)

#define VPSUTILS_TILER_DMM_PAT_VIEW_MASK              (0x80000000u)
#define VPSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_MASK   (0x78000000u)
#define VPSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_SHIFT  (27u)
#define VPSUTILS_TILER_DMM_PAT_TWO_NIBBLE_SHIFT       (8u)

#define VPSUTILS_TILER_GET_CNT_MODE(tilerAddr) (((tilerAddr) >> 27) & 0x3)
#define VPSUTILS_TILER_PUT_CNT_MODE(tilerAddr, cntMode)                        \
                                    (((tilerAddr) | (((cntMode) & 0x3) << 27)))

#define VPSUTILS_TILER_ORI_MODE_SHIFT   (29u)
#define VPSUTILS_TILER_ORI_MODE_MASK    (0x07u << VPSUTILS_TILER_ORI_MODE_SHIFT)

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/

typedef struct {

  UInt32 curX[VPSUTILS_TILER_CNT_MAX];
  UInt32 curStartY[VPSUTILS_TILER_CNT_MAX];
  UInt32 curEndY[VPSUTILS_TILER_CNT_MAX];

  UInt32 enableLog;

} VpsUtils_TilerObj;

#pragma DATA_ALIGN(gVpsUtils_tilerPhysMem, 32*KB)
#pragma DATA_SECTION(gVpsUtils_tilerPhysMem,".bss:tilerBuffer");
UInt8 gVpsUtils_tilerPhysMem[VPSUTILS_TILER_PHYS_MEM_SIZE];

VpsUtils_TilerObj gVpsUtils_tilerObj;
static Bool gVpsTilerInitDone = FALSE;

Int32 VpsUtils_tilerInit()
{
  volatile UInt32 *pReg = (volatile UInt32 *)VPSUTILS_TILER_DMM_PAT_VIEW_MAP_BASE;

  volatile UInt32 *mReg = (volatile UInt32 *)VPSUTILS_TILER_DMM_PAT_VIEW_MAP__0;

  UInt32    offsetVal;

   /* Only one bit will be set in this register so masking with 0x80000000 */
  *pReg = (UInt32)gVpsUtils_tilerPhysMem & VPSUTILS_TILER_DMM_PAT_VIEW_MASK;


  /* Extract the offset value from the tiler address,
     Offset value is same for 8bit tile and 16 bit tiler
     Store this offset vale in MAP__0 register          */

  offsetVal  = (((UInt32)gVpsUtils_tilerPhysMem
                    & VPSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_MASK) >>
                    VPSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_SHIFT);

  *mReg |= ((offsetVal << VPSUTILS_TILER_DMM_PAT_TWO_NIBBLE_SHIFT) |offsetVal);

  gVpsTilerInitDone = TRUE;
  VpsUtils_tilerFreeAll();
  VpsUtils_tilerDebugLogEnable(FALSE);

  return 0;
}

Int32 VpsUtils_tilerDeInit()
{
  gVpsTilerInitDone = FALSE;
  return 0;
}


Int32 VpsUtils_tilerDebugLogEnable(UInt32 enable)
{
  gVpsUtils_tilerObj.enableLog = enable;

  return 0;
}

UInt32 VpsUtils_tilerAddr2CpuAddr(UInt32 tilerAddr)
{
  UInt32 cpuAddr, cntMode;

  cntMode = VPSUTILS_TILER_GET_CNT_MODE(tilerAddr);

  cpuAddr = VPSUTILS_TILER_CPU_VIRT_ADDR+VPSUTILS_TILER_PHYS_MEM_SIZE*cntMode;
  cpuAddr += (tilerAddr & 0x07FFFFFF);

  if(gVpsUtils_tilerObj.enableLog)
  {
    Vps_printf(" [TILER] Tiler Addr = 0x%08x, CPU Addr = 0x%08x\n",
      tilerAddr, cpuAddr
    );
  }

  return cpuAddr;
}

Int32 VpsUtils_tilerGetMaxPitchHeight(UInt32 cntMode, UInt32 *maxPitch, UInt32 *maxHeight)
{
  *maxPitch = 0;
  *maxHeight = 0;

  switch(cntMode)
  {
    case VPSUTILS_TILER_CNT_8BIT:
      *maxPitch = VPSUTILS_TILER_CNT_8BIT_PITCH;
      *maxHeight= VPSUTILS_TILER_MAX_HEIGHT_8BIT;
      break;
    case VPSUTILS_TILER_CNT_16BIT:
      *maxPitch = VPSUTILS_TILER_CNT_16BIT_PITCH;
      *maxHeight= VPSUTILS_TILER_MAX_HEIGHT_16BIT;
      break;
    case VPSUTILS_TILER_CNT_32BIT:
      *maxPitch = VPSUTILS_TILER_CNT_32BIT_PITCH;
      *maxHeight= VPSUTILS_TILER_MAX_HEIGHT_32BIT;
      break;
    default:
      return -1;
  }

  return 0;
}

UInt32 VpsUtils_tilerGetAddr(UInt32 cntMode, UInt32 startX, UInt32 startY)
{
  UInt32 tilerAddr, maxPitch, maxHeight, offset;

  VpsUtils_tilerGetMaxPitchHeight(cntMode, &maxPitch, &maxHeight);

  offset=0;
  if(cntMode==VPSUTILS_TILER_CNT_16BIT)
    offset=VPSUTILS_TILER_CNT_8BIT_SIZE;
  if(cntMode==VPSUTILS_TILER_CNT_32BIT)
    offset=VPSUTILS_TILER_CNT_8BIT_SIZE+VPSUTILS_TILER_CNT_16BIT_SIZE;

  tilerAddr = offset + (startY*maxPitch + startX);
  tilerAddr = VPSUTILS_TILER_PUT_CNT_MODE(tilerAddr, cntMode);

  if(gVpsUtils_tilerObj.enableLog)
  {
    Vps_printf(" [TILER] Tiler Addr = 0x%08x, mode = %d, x,y = %d,%d\n",
      tilerAddr, cntMode, startX, startY
    );
  }

  return tilerAddr;
}

UInt32 VpsUtils_tilerGetOriAddr(UInt32 tilerAddr,
                                UInt32 cntMode,
                                UInt32 oriFlag,
                                UInt32 width,
                                UInt32 height)
{
    UInt32      oriAddr;
    UInt32      hOffset, vOffset;
    UInt32      hStride, vStride;

    /* Get the base address without orientation and container modes */
    oriAddr = tilerAddr;
    oriAddr &= ~(0x1Fu << 27u);
    oriFlag &= (VPSUTILS_TILER_ORI_X_FLIP |
                VPSUTILS_TILER_ORI_Y_FLIP |
                VPSUTILS_TILER_ORI_XY_SWAP);

    /* Figure out horizontal stride and max lines as per container mode */
    if (VPSUTILS_TILER_CNT_8BIT == cntMode)
    {
        hStride = VPSUTILS_TILER_CNT_8BIT_PITCH;
        vStride = VPSUTILS_TILER_CNT_8BIT_MAX_LINES;
    }
    else if (VPSUTILS_TILER_CNT_16BIT == cntMode)
    {
        hStride = VPSUTILS_TILER_CNT_16BIT_PITCH;
        vStride = VPSUTILS_TILER_CNT_16BIT_MAX_LINES;
    }
    else
    {
        hStride = VPSUTILS_TILER_CNT_32BIT_PITCH;
        vStride = VPSUTILS_TILER_CNT_32BIT_MAX_LINES;
    }

    /* Calculate horizontal line offset from start of line */
    hOffset = oriAddr & (hStride - 1u);

    /* Calculate X' address */
    if (oriFlag & VPSUTILS_TILER_ORI_X_FLIP)
    {
        /* Clear line offset - Get the start of line address */
        oriAddr &= ~(hStride - 1u);
        GT_assert(GT_DEFAULT_MASK, (hStride > (hOffset + width)));

        /* Calculate new address from reverse X (X') axis */
        oriAddr += hStride - (hOffset + width);
    }

    /* Calculate Y' address */
    if (oriFlag & VPSUTILS_TILER_ORI_Y_FLIP)
    {
        /* Calculate vertical offset in terms of lines */
        vOffset = (oriAddr / hStride);
        GT_assert(GT_DEFAULT_MASK, (vStride > (vOffset + height)));

        /* Calculate new address from reverse Y (Y') axis */
        oriAddr = (vStride - (vOffset + height)) * hStride;
        oriAddr += hOffset;
    }

    /* Set the orientation modes */
    oriAddr &= ~VPSUTILS_TILER_ORI_MODE_MASK;
    oriAddr |= (oriFlag << VPSUTILS_TILER_ORI_MODE_SHIFT);

    /* Set the container mode */
    oriAddr = VPSUTILS_TILER_PUT_CNT_MODE(oriAddr, cntMode);

    return (oriAddr);
}

Int32 VpsUtils_tilerFreeAll()
{
  UInt32 cntMode;

  GT_assert(GT_DEFAULT_MASK, (TRUE == gVpsTilerInitDone));

  for(cntMode=0; cntMode<VPSUTILS_TILER_CNT_MAX; cntMode++)
  {
    gVpsUtils_tilerObj.curX[cntMode] = VPS_BUFFER_ALIGNMENT*2;
    gVpsUtils_tilerObj.curStartY[cntMode] = 0;
    gVpsUtils_tilerObj.curEndY[cntMode] = 0;
  }

  return 0;
}

UInt32 VpsUtils_tilerAlloc( UInt32 cntMode, UInt32 width, UInt32 height)
{
  UInt32 tilerAddr, curX, curStartY, curEndY, maxWidth, maxHeight;

  GT_assert(GT_DEFAULT_MASK, (TRUE == gVpsTilerInitDone));

  width = VpsUtils_align(width, VPS_BUFFER_ALIGNMENT*2);
  height = VpsUtils_align(height, 2);

  curX = gVpsUtils_tilerObj.curX[cntMode];
  curStartY = gVpsUtils_tilerObj.curStartY[cntMode];
  curEndY = gVpsUtils_tilerObj.curEndY[cntMode];

  VpsUtils_tilerGetMaxPitchHeight(cntMode, &maxWidth, &maxHeight);

  if( (curX + width) > maxWidth)
  {
    curX = 0;
    curStartY = curEndY;
  }

  if( (curStartY+height) > maxHeight)
  {
    return 0;
  }

  if( (curStartY+height) > curEndY)
  {
    curEndY = curStartY+height;
  }

  tilerAddr = VpsUtils_tilerGetAddr(cntMode, curX, curStartY);

  curX += width;

  gVpsUtils_tilerObj.curX[cntMode] = curX;
  gVpsUtils_tilerObj.curStartY[cntMode] = curStartY;
  gVpsUtils_tilerObj.curEndY[cntMode] = curEndY;

  return tilerAddr;
}


Int32 VpsUtils_tilerFrameAlloc(FVID2_Format *pFormat,
        FVID2_Frame *pFrame, UInt16 numFrames)
{
  UInt32 frameId;

  GT_assert(GT_DEFAULT_MASK, (TRUE == gVpsTilerInitDone));

  /* align height to multiple of 2  */
  pFormat->height = VpsUtils_align(pFormat->height, 2);

  for(frameId=0; frameId<numFrames; frameId++)
  {
    /* init FVID2_Frame to 0's  */
    memset(pFrame, 0, sizeof(*pFrame));

    /* copy channelNum to FVID2_Frame from FVID2_Format */
    pFrame->channelNum = pFormat->channelNum;

    switch(pFormat->dataFormat)
    {
      case FVID2_DF_YUV422SP_UV:

        /* Y plane  */
        pFrame->addr[0][0] = (Ptr)VpsUtils_tilerAlloc(VPSUTILS_TILER_CNT_8BIT,
                            pFormat->width, pFormat->height
                            );

        /* C plane  */
        pFrame->addr[0][1] = (Ptr)VpsUtils_tilerAlloc(VPSUTILS_TILER_CNT_16BIT,
                            pFormat->width, pFormat->height
                            );
        break;
      case FVID2_DF_YUV420SP_UV:

        /* Y plane  */
        pFrame->addr[0][0] = (Ptr)VpsUtils_tilerAlloc(VPSUTILS_TILER_CNT_8BIT,
                            pFormat->width, pFormat->height
                            );

        /* C plane  */
        pFrame->addr[0][1] = (Ptr)VpsUtils_tilerAlloc(VPSUTILS_TILER_CNT_16BIT,
                            pFormat->width, pFormat->height/2
                            );
        break;
      default:
        GT_assert( GT_DEFAULT_MASK, 0);

    }

    pFrame++;
  }

  return 0;
}


Int32 VpsUtils_tilerCopy(UInt32 dir, UInt32 tilerAddr, UInt32 dataWidth, UInt32 dataHeight,
          UInt8 *ddrAddr, UInt32 ddrPitch)
{
  UInt32 cntMode;
  UInt32 tilerCpuAddr, tilerPitch, tilerHeight;
  UInt32 inc;
  UInt32 dstAddrBase, dstAddr, dstPitch;
  UInt32 srcAddrBase, srcAddr, srcPitch;
  UInt32 h, w;

  GT_assert(GT_DEFAULT_MASK, (TRUE == gVpsTilerInitDone));

  cntMode = VPSUTILS_TILER_GET_CNT_MODE(tilerAddr);
  tilerCpuAddr = VpsUtils_tilerAddr2CpuAddr(tilerAddr);

  VpsUtils_tilerGetMaxPitchHeight(cntMode, &tilerPitch, &tilerHeight);

  inc = 1<<cntMode;

  if(dir==VPSUTILS_TILER_COPY_TO_DDR)
  {
    dstAddrBase = (UInt32)ddrAddr;
    dstPitch    = ddrPitch;
    srcAddrBase = tilerCpuAddr;
    srcPitch    = tilerPitch;
  }
  else
  {
    srcAddrBase = (UInt32)ddrAddr;
    srcPitch    = ddrPitch;
    dstAddrBase = tilerCpuAddr;
    dstPitch    = tilerPitch;
  }

  for(h=0; h<dataHeight; h++)
  {
    dstAddr = dstAddrBase;
    srcAddr = srcAddrBase;

    switch(cntMode)
    {
      case VPSUTILS_TILER_CNT_8BIT:
        for(w=0; w<dataWidth; w+=inc, dstAddr+=inc, srcAddr+=inc)
          *(volatile UInt8*)dstAddr = *(volatile UInt8*)srcAddr;
        break;
      case VPSUTILS_TILER_CNT_16BIT:
        for(w=0; w<dataWidth; w+=inc, dstAddr+=inc, srcAddr+=inc)
          *(volatile UInt16*)dstAddr = *(volatile UInt16*)srcAddr;
        break;
      case VPSUTILS_TILER_CNT_32BIT:
        for(w=0; w<dataWidth; w+=inc, dstAddr+=inc, srcAddr+=inc)
          *(volatile UInt32*)dstAddr = *(volatile UInt32*)srcAddr;
        break;
    }

    dstAddrBase += dstPitch;
    srcAddrBase += srcPitch;
  }

  return 0;
}
