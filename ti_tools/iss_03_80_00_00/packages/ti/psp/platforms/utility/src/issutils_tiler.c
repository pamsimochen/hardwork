/** ==================================================================
 *  @file   issutils_tiler.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/iss/common/trace.h>
#include <ti/psp/platforms/utility/issutils_tiler.h>

#define ISSUTILS_TILER_CNT_8BIT_SIZE    (80*MB)
#define ISSUTILS_TILER_CNT_16BIT_SIZE   (48*MB)
#define ISSUTILS_TILER_CNT_32BIT_SIZE   ( 0*MB)

#define ISSUTILS_TILER_MAX_HEIGHT_8BIT   (ISSUTILS_TILER_CNT_8BIT_SIZE /       \
                                          ISSUTILS_TILER_CNT_8BIT_PITCH)
#define ISSUTILS_TILER_MAX_HEIGHT_16BIT  (ISSUTILS_TILER_CNT_16BIT_SIZE /      \
                                          ISSUTILS_TILER_CNT_16BIT_PITCH)
#define ISSUTILS_TILER_MAX_HEIGHT_32BIT  (ISSUTILS_TILER_CNT_32BIT_SIZE /      \
                                          ISSUTILS_TILER_CNT_32BIT_PITCH)

#define ISSUTILS_TILER_CNT_8BIT_MAX_LINES   (8192u)
#define ISSUTILS_TILER_CNT_16BIT_MAX_LINES  (4096u)
#define ISSUTILS_TILER_CNT_32BIT_MAX_LINES  (4096u)

#define ISSUTILS_TILER_REG_BASE                       (0x4E000100u)
#define ISSUTILS_TILER_DMM_PAT_VIEW_MAP_BASE          (0x4E000460u)
#define ISSUTILS_TILER_DMM_PAT_VIEW_MAP__0            (0x4E000440u)

#define ISSUTILS_TILER_DMM_PAT_VIEW_MASK              (0x80000000u)
#define ISSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_MASK   (0x78000000u)
#define ISSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_SHIFT  (27u)
#define ISSUTILS_TILER_DMM_PAT_TWO_NIBBLE_SHIFT       (8u)

#define ISSUTILS_TILER_GET_CNT_MODE(tilerAddr) (((tilerAddr) >> 27) & 0x3)
#define ISSUTILS_TILER_PUT_CNT_MODE(tilerAddr, cntMode)                        \
                                    (((tilerAddr) | (((cntMode) & 0x3) << 27)))

#define ISSUTILS_TILER_ORI_MODE_SHIFT   (29u)
#define ISSUTILS_TILER_ORI_MODE_MASK    (0x07u << ISSUTILS_TILER_ORI_MODE_SHIFT)

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/

typedef struct {

    UInt32 curX[ISSUTILS_TILER_CNT_MAX];
    UInt32 curStartY[ISSUTILS_TILER_CNT_MAX];
    UInt32 curEndY[ISSUTILS_TILER_CNT_MAX];

    UInt32 enableLog;

} IssUtils_TilerObj;

#pragma DATA_ALIGN(gIssUtils_tilerPhysMem, 32*KB)
#pragma DATA_SECTION(gIssUtils_tilerPhysMem,".bss:tilerBuffer");
UInt8 gIssUtils_tilerPhysMem[ISSUTILS_TILER_PHYS_MEM_SIZE];

IssUtils_TilerObj gIssUtils_tilerObj;

/* ===================================================================
 *  @func     IssUtils_tilerInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerInit()
{
    volatile UInt32 *pReg =
        (volatile UInt32 *) ISSUTILS_TILER_DMM_PAT_VIEW_MAP_BASE;

    volatile UInt32 *mReg =
        (volatile UInt32 *) ISSUTILS_TILER_DMM_PAT_VIEW_MAP__0;

    UInt32 offsetVal;

    /* Only one bit will be set in this register so masking with 0x80000000 */
    *pReg = (UInt32) gIssUtils_tilerPhysMem & ISSUTILS_TILER_DMM_PAT_VIEW_MASK;

    /* Extract the offset value from the tiler address, Offset value is same
     * for 8bit tile and 16 bit tiler Store this offset vale in MAP__0
     * register */

    offsetVal = (((UInt32) gIssUtils_tilerPhysMem
                  & ISSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_MASK) >>
                 ISSUTILS_TILER_DMM_PAT_VIEW_MAP_OFFSET_SHIFT);

    *mReg |=
        ((offsetVal << ISSUTILS_TILER_DMM_PAT_TWO_NIBBLE_SHIFT) | offsetVal);

    IssUtils_tilerFreeAll();

    IssUtils_tilerDebugLogEnable(FALSE);

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_tilerDeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerDeInit()
{

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_tilerDebugLogEnable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerDebugLogEnable(UInt32 enable)
{
    gIssUtils_tilerObj.enableLog = enable;

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_tilerAddr2CpuAddr                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 IssUtils_tilerAddr2CpuAddr(UInt32 tilerAddr)
{
    UInt32 cpuAddr, cntMode;

    cntMode = ISSUTILS_TILER_GET_CNT_MODE(tilerAddr);

    cpuAddr =
        ISSUTILS_TILER_CPU_VIRT_ADDR + ISSUTILS_TILER_PHYS_MEM_SIZE * cntMode;
    cpuAddr += (tilerAddr & 0x07FFFFFF);

    if (gIssUtils_tilerObj.enableLog)
    {
        Iss_printf(" [TILER] Tiler Addr = 0x%08x, CPU Addr = 0x%08x\n",
                   tilerAddr, cpuAddr);
    }

    return cpuAddr;
}

/* ===================================================================
 *  @func     IssUtils_tilerGetMaxPitchHeight                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerGetMaxPitchHeight(UInt32 cntMode, UInt32 * maxPitch,
                                      UInt32 * maxHeight)
{
    *maxPitch = 0;
    *maxHeight = 0;

    switch (cntMode)
    {
        case ISSUTILS_TILER_CNT_8BIT:
            *maxPitch = ISSUTILS_TILER_CNT_8BIT_PITCH;
            *maxHeight = ISSUTILS_TILER_MAX_HEIGHT_8BIT;
            break;
        case ISSUTILS_TILER_CNT_16BIT:
            *maxPitch = ISSUTILS_TILER_CNT_16BIT_PITCH;
            *maxHeight = ISSUTILS_TILER_MAX_HEIGHT_16BIT;
            break;
        case ISSUTILS_TILER_CNT_32BIT:
            *maxPitch = ISSUTILS_TILER_CNT_32BIT_PITCH;
            *maxHeight = ISSUTILS_TILER_MAX_HEIGHT_32BIT;
            break;
        default:
            return -1;
    }

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_tilerGetAddr                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 IssUtils_tilerGetAddr(UInt32 cntMode, UInt32 startX, UInt32 startY)
{
    UInt32 tilerAddr, maxPitch, maxHeight, offset;

    IssUtils_tilerGetMaxPitchHeight(cntMode, &maxPitch, &maxHeight);

    offset = 0;
    if (cntMode == ISSUTILS_TILER_CNT_16BIT)
        offset = ISSUTILS_TILER_CNT_8BIT_SIZE;
    if (cntMode == ISSUTILS_TILER_CNT_32BIT)
        offset = ISSUTILS_TILER_CNT_8BIT_SIZE + ISSUTILS_TILER_CNT_16BIT_SIZE;

    tilerAddr = offset + (startY * maxPitch + startX);
    tilerAddr = ISSUTILS_TILER_PUT_CNT_MODE(tilerAddr, cntMode);

    if (gIssUtils_tilerObj.enableLog)
    {
        Iss_printf(" [TILER] Tiler Addr = 0x%08x, mode = %d, x,y = %d,%d\n",
                   tilerAddr, cntMode, startX, startY);
    }

    return tilerAddr;
}

/* ===================================================================
 *  @func     IssUtils_tilerGetOriAddr                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 IssUtils_tilerGetOriAddr(UInt32 tilerAddr,
                                UInt32 cntMode,
                                UInt32 oriFlag, UInt32 width, UInt32 height)
{
    UInt32 oriAddr;

    UInt32 hOffset, vOffset;

    UInt32 hStride, vStride;

    /* Get the base address without orientation and container modes */
    oriAddr = tilerAddr;
    oriAddr &= ~(0x1Fu << 27u);
    oriFlag &= (ISSUTILS_TILER_ORI_X_FLIP |
                ISSUTILS_TILER_ORI_Y_FLIP | ISSUTILS_TILER_ORI_XY_SWAP);

    /* Figure out horizontal stride and max lines as per container mode */
    if (ISSUTILS_TILER_CNT_8BIT == cntMode)
    {
        hStride = ISSUTILS_TILER_CNT_8BIT_PITCH;
        vStride = ISSUTILS_TILER_CNT_8BIT_MAX_LINES;
    }
    else if (ISSUTILS_TILER_CNT_16BIT == cntMode)
    {
        hStride = ISSUTILS_TILER_CNT_16BIT_PITCH;
        vStride = ISSUTILS_TILER_CNT_16BIT_MAX_LINES;
    }
    else
    {
        hStride = ISSUTILS_TILER_CNT_32BIT_PITCH;
        vStride = ISSUTILS_TILER_CNT_32BIT_MAX_LINES;
    }

    /* Calculate horizontal line offset from start of line */
    hOffset = oriAddr & (hStride - 1u);

    /* Calculate X' address */
    if (oriFlag & ISSUTILS_TILER_ORI_X_FLIP)
    {
        /* Clear line offset - Get the start of line address */
        oriAddr &= ~(hStride - 1u);
        GT_assert(GT_DEFAULT_MASK, (hStride > (hOffset + width)));

        /* Calculate new address from reverse X (X') axis */
        oriAddr += hStride - (hOffset + width);
    }

    /* Calculate Y' address */
    if (oriFlag & ISSUTILS_TILER_ORI_Y_FLIP)
    {
        /* Calculate vertical offset in terms of lines */
        vOffset = (oriAddr / hStride);
        GT_assert(GT_DEFAULT_MASK, (vStride > (vOffset + height)));

        /* Calculate new address from reverse Y (Y') axis */
        oriAddr = (vStride - (vOffset + height)) * hStride;
        oriAddr += hOffset;
    }

    /* Set the orientation modes */
    oriAddr &= ~ISSUTILS_TILER_ORI_MODE_MASK;
    oriAddr |= (oriFlag << ISSUTILS_TILER_ORI_MODE_SHIFT);

    /* Set the container mode */
    oriAddr = ISSUTILS_TILER_PUT_CNT_MODE(oriAddr, cntMode);

    return (oriAddr);
}

/* ===================================================================
 *  @func     IssUtils_tilerFreeAll                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerFreeAll()
{
    UInt32 cntMode;

    for (cntMode = 0; cntMode < ISSUTILS_TILER_CNT_MAX; cntMode++)
    {
        gIssUtils_tilerObj.curX[cntMode] = ISS_BUFFER_ALIGNMENT * 2;
        gIssUtils_tilerObj.curStartY[cntMode] = 0;
        gIssUtils_tilerObj.curEndY[cntMode] = 0;
    }

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_tilerAlloc                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 IssUtils_tilerAlloc(UInt32 cntMode, UInt32 width, UInt32 height)
{
    UInt32 tilerAddr, curX, curStartY, curEndY, maxWidth, maxHeight;

    width = IssUtils_align(width, ISS_BUFFER_ALIGNMENT * 2);
    height = IssUtils_align(height, 2);

    curX = gIssUtils_tilerObj.curX[cntMode];
    curStartY = gIssUtils_tilerObj.curStartY[cntMode];
    curEndY = gIssUtils_tilerObj.curEndY[cntMode];

    IssUtils_tilerGetMaxPitchHeight(cntMode, &maxWidth, &maxHeight);

    if ((curX + width) > maxWidth)
    {
        curX = 0;
        curStartY = curEndY;
    }

    if ((curStartY + height) > maxHeight)
    {
        return 0;
    }

    if ((curStartY + height) > curEndY)
    {
        curEndY = curStartY + height;
    }

    tilerAddr = IssUtils_tilerGetAddr(cntMode, curX, curStartY);

    curX += width;

    gIssUtils_tilerObj.curX[cntMode] = curX;
    gIssUtils_tilerObj.curStartY[cntMode] = curStartY;
    gIssUtils_tilerObj.curEndY[cntMode] = curEndY;

    return tilerAddr;
}

/* ===================================================================
 *  @func     IssUtils_tilerFrameAlloc                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerFrameAlloc(FVID2_Format * pFormat,
                               FVID2_Frame * pFrame, UInt16 numFrames)
{
    UInt32 frameId;

    /* align height to multiple of 2 */
    pFormat->height = IssUtils_align(pFormat->height, 2);

    for (frameId = 0; frameId < numFrames; frameId++)
    {
        /* init FVID2_Frame to 0's */
        memset(pFrame, 0, sizeof(*pFrame));

        /* copy channelNum to FVID2_Frame from FVID2_Format */
        pFrame->channelNum = pFormat->channelNum;

        switch (pFormat->dataFormat)
        {
            case FVID2_DF_YUV422SP_UV:

                /* Y plane */
                pFrame->addr[0][0] =
                    (Ptr) IssUtils_tilerAlloc(ISSUTILS_TILER_CNT_8BIT,
                                              pFormat->width, pFormat->height);

                /* C plane */
                pFrame->addr[0][1] =
                    (Ptr) IssUtils_tilerAlloc(ISSUTILS_TILER_CNT_16BIT,
                                              pFormat->width, pFormat->height);
                break;
            case FVID2_DF_YUV420SP_UV:

                /* Y plane */
                pFrame->addr[0][0] =
                    (Ptr) IssUtils_tilerAlloc(ISSUTILS_TILER_CNT_8BIT,
                                              pFormat->width, pFormat->height);

                /* C plane */
                pFrame->addr[0][1] =
                    (Ptr) IssUtils_tilerAlloc(ISSUTILS_TILER_CNT_16BIT,
                                              pFormat->width,
                                              pFormat->height / 2);
                break;
            default:
                GT_assert(GT_DEFAULT_MASK, 0);

        }

        pFrame++;
    }

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_tilerCopy                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssUtils_tilerCopy(UInt32 dir, UInt32 tilerAddr, UInt32 dataWidth,
                         UInt32 dataHeight, UInt8 * ddrAddr, UInt32 ddrPitch)
{
    UInt32 cntMode;

    UInt32 tilerCpuAddr, tilerPitch, tilerHeight;

    UInt32 inc;

    UInt32 dstAddrBase, dstAddr, dstPitch;

    UInt32 srcAddrBase, srcAddr, srcPitch;

    UInt32 h, w;

    cntMode = ISSUTILS_TILER_GET_CNT_MODE(tilerAddr);
    tilerCpuAddr = IssUtils_tilerAddr2CpuAddr(tilerAddr);

    IssUtils_tilerGetMaxPitchHeight(cntMode, &tilerPitch, &tilerHeight);

    inc = 1 << cntMode;

    if (dir == ISSUTILS_TILER_COPY_TO_DDR)
    {
        dstAddrBase = (UInt32) ddrAddr;
        dstPitch = ddrPitch;
        srcAddrBase = tilerCpuAddr;
        srcPitch = tilerPitch;
    }
    else
    {
        srcAddrBase = (UInt32) ddrAddr;
        srcPitch = ddrPitch;
        dstAddrBase = tilerCpuAddr;
        dstPitch = tilerPitch;
    }

    for (h = 0; h < dataHeight; h++)
    {
        dstAddr = dstAddrBase;
        srcAddr = srcAddrBase;

        switch (cntMode)
        {
            case ISSUTILS_TILER_CNT_8BIT:
                for (w = 0; w < dataWidth;
                     w += inc, dstAddr += inc, srcAddr += inc)
                    *(volatile UInt8 *) dstAddr = *(volatile UInt8 *) srcAddr;
                break;
            case ISSUTILS_TILER_CNT_16BIT:
                for (w = 0; w < dataWidth;
                     w += inc, dstAddr += inc, srcAddr += inc)
                    *(volatile UInt16 *) dstAddr = *(volatile UInt16 *) srcAddr;
                break;
            case ISSUTILS_TILER_CNT_32BIT:
                for (w = 0; w < dataWidth;
                     w += inc, dstAddr += inc, srcAddr += inc)
                    *(volatile UInt32 *) dstAddr = *(volatile UInt32 *) srcAddr;
                break;
        }

        dstAddrBase += dstPitch;
        srcAddrBase += srcPitch;
    }

    return 0;
}
