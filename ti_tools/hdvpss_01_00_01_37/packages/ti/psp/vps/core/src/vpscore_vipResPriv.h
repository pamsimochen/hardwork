/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPSCORE_VIP_RES_PRIV_H_
#define _VPSCORE_VIP_RES_PRIV_H_


#include <string.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>

#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Semaphore.h>

/* VPDMA channel ID generation macros */

/* VIP0 VPDMA base channel ID */
#define VCORE_VIP_VCH_VIP0_BASE   (VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC0)

/* VIP1 VPDMA base channel ID */
#define VCORE_VIP_VCH_VIP1_BASE   (VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC0)

/* VPDMA channels per VIP */
#define VCORE_VIP_VCH_VIP_NUM     \
          (VCORE_VIP_VCH_VIP1_BASE - VCORE_VIP_VCH_VIP0_BASE)

/* VPDMA VIP multi-channel channel ID */
#define VCORE_VIP_VCH_VIP_MULT( inst, port, ch)      \
            ((VpsHal_VpdmaChannel)(VCORE_VIP_VCH_VIP0_BASE + \
                (inst)*VCORE_VIP_VCH_VIP_NUM + 16*(port) + (ch)))

/* VPDMA VIP multi-channel ancillary channel ID */
#define VCORE_VIP_VCH_VIP_MULT_ANC( inst, port, ch)  \
            ((VpsHal_VpdmaChannel)(VCORE_VIP_VCH_VIP0_BASE + \
                (inst)*VCORE_VIP_VCH_VIP_NUM + 16*(port+2) + (ch)))

/* VPDMA VIP Luma channel ID */
#define VCORE_VIP_VCH_VIP_LUMA( inst, port)          \
            ((VpsHal_VpdmaChannel)(VCORE_VIP_VCH_VIP0_BASE + \
                (inst)*VCORE_VIP_VCH_VIP_NUM + 16*4 + 2*(port)))

/* VPDMA VIP Chroma channel ID */
#define VCORE_VIP_VCH_VIP_CHROMA( inst, port )       \
            ((VpsHal_VpdmaChannel)(VCORE_VIP_VCH_VIP0_BASE + \
                (inst)*VCORE_VIP_VCH_VIP_NUM + 16*4 + 2*(port) + 1))

/* VPDMA VIP RGB channel ID */
#define VCORE_VIP_VCH_VIP_RGB( inst, port )          \
            ((VpsHal_VpdmaChannel)(VCORE_VIP_VCH_VIP0_BASE + \
              (inst)*VCORE_VIP_VCH_VIP_NUM + 16*4 + 4 + (port)))

/*
  Resource allocator internal data structure
*/
typedef struct
{
    /*
     * global resource table for every VIP
     */
    Vcore_VipResEntry globalResTable[VPSHAL_VIP_INST_MAX][VCORE_VIP_RES_MAX];

    /*
     * mutex lock
     */
    Semaphore_Handle lock;

    /*
     * VIP handles
     */
    VpsHal_VipHandle vipHandle[VPSHAL_VIP_INST_MAX][VPSHAL_VIP_PORT_MAX];

    /*
     * SC handles
     */
    VpsHal_Handle scHandle[VPSHAL_VIP_INST_MAX];

    /*
     * CSC handles
     */
    VpsHal_Handle cscHandle[VPSHAL_VIP_INST_MAX];

    /*
     * Enable/disable debug log
     */
    UInt32 enableDebugLog;

} Vcore_VipResCommonObj;

extern Vcore_VipResCommonObj gVcore_vipResCommonObj;

/* functions documentated in .c file  */

Int32 Vcore_vipResLock (  );
Int32 Vcore_vipResUnlock (  );

Int32 Vcore_vipResResetObj ( Vcore_VipResObj * pObj );

Int32 Vcore_vipResAlloc ( Vcore_VipResObj * pObj, UInt16 resId );
Int32 Vcore_vipResFree ( Vcore_VipResObj * pObj, UInt16 resId );

Int32 Vcore_vipResFreeAll ( Vcore_VipResObj * pObj );

Int32 Vcore_vipResResetAll ( Vcore_VipResEntry * pResObj );
Int32 Vcore_vipResReset ( Vcore_VipResEntry * pResObj, UInt16 resId );

Int32 Vcore_vipResIsInSrcMultiChYuv422 ( Vcore_VipInSrc inSrc );
Int32 Vcore_vipResIsInSrcSingleChYuv422 ( Vcore_VipInSrc inSrc );
Int32 Vcore_vipResIsInSrcSingleChRgb ( Vcore_VipInSrc inSrc );
Int32 Vcore_vipResIsInSrcSingleChYuv444 ( Vcore_VipInSrc inSrc );
Int32 Vcore_vipResIsInSrcPortA ( Vcore_VipInSrc inSrc );

Int32 Vcore_vipResAllocMultiChPath ( Vcore_VipResObj * pObj );

Int32 Vcore_vipResAllocChrDs ( Vcore_VipResObj * pObj, Int32 chrDsId,
                               UInt32 allocYcMux );

Int32 Vcore_vipResetMux (  );

Int32 Vcore_vipRestoreMux ( Vcore_VipResObj * pObj );

Int32 Vcore_vipResVrmSetState ( UInt16 instId, UInt16 resId, UInt32 alloc );

Int32 Vcore_vipResAllocSingleChRgb888Path ( Vcore_VipResObj * pObj );

Int32 Vcore_vipResAllocSingleChRgb888PathYuv422Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     VpsHal_VpsVipCscMuxSrc
                                                     cscSrc );

Int32 Vcore_vipResAllocSingleChRgb888PathYuv420Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     VpsHal_VpsVipCscMuxSrc
                                                     cscSrc );

Int32 Vcore_vipResAllocSingleChRgb888PathRgb888Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId );

Int32 Vcore_vipResAllocSingleChRgb888PathAncOut ( Vcore_VipResObj * pObj,
                                                  UInt16 outId, UInt16 portId );

Int32 Vcore_vipResAllocSingleChYuv422Path ( Vcore_VipResObj * pObj );

Int32 Vcore_vipResAllocSingleChYuv422PathYuv422Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     UInt16 portId );

Int32 Vcore_vipResAllocSingleChYuv422PathYuv420Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId );

Int32 Vcore_vipResAllocSingleChYuv422PathRgb888Yuv444Out ( Vcore_VipResObj *
                                                           pObj, UInt16 outId );

Int32 Vcore_vipResAllocSingleChYuv422PathAncOut ( Vcore_VipResObj * pObj,
                                                  UInt16 outId, UInt16 portId );

Int32 Vcore_vipResPrintStatus ( UInt16 instId, UInt16 resId );

Int32 Vcore_vipResAllocSingleChYuv422PathYuv422spOut ( Vcore_VipResObj * pObj,
                                                     UInt16 outId );

Int32 Vcore_vipResAllocSingleChRgb888PathYuv422spOut ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     VpsHal_VpsVipCscMuxSrc
                                                     cscSrc );
#endif /*  _VPSCORE_VIP_RES_PRIV_H_  */
