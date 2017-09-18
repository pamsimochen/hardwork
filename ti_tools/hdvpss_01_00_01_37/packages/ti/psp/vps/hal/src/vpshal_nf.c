/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/hal/vpshal_nf.h>
#include <ti/sysbios/hal/Hwi.h>

typedef struct {

  UInt16 instId;
  CSL_VpsNfRegsOvly regs;
  UInt32 openCnt;

} VpsHal_NfObj;


static VpsHal_NfObj  gVpsHal_nfObj[VPSHAL_NF_MAX_INST];

Int32 VpsHal_nfInit(UInt32 numInstances,
                        const VpsHal_NfInstParams *initParams,
                        Ptr arg)
{
  UInt32 i;

  GT_assert( GT_DEFAULT_MASK, numInstances <= VPSHAL_NF_MAX_INST );
  GT_assert( GT_DEFAULT_MASK, initParams != NULL);

  for(i=0; i<numInstances; i++) {
    gVpsHal_nfObj[i].instId = initParams[i].instId;
    gVpsHal_nfObj[i].regs = (CSL_VpsNfRegsOvly)initParams[i].baseAddress;
    gVpsHal_nfObj[i].openCnt = 0;
  }

  return 0;
}

Int32 VpsHal_nfDeInit(Ptr arg)
{
  return 0;
}

VpsHal_Handle VpsHal_nfOpen(UInt32 nfInst)
{
  VpsHal_NfObj *pObj;
  UInt32 oldIntState;

  GT_assert( GT_DEFAULT_MASK, nfInst < VPSHAL_NF_MAX_INST );

  pObj = &gVpsHal_nfObj[nfInst];

  oldIntState = Hwi_disable();

  pObj->openCnt++;

  Hwi_restore(oldIntState);

  return pObj;
}

Int32 VpsHal_nfClose(VpsHal_Handle handle)
{
  VpsHal_NfObj *pObj=(VpsHal_NfObj *)handle;
  UInt32 oldIntState;

  GT_assert( GT_DEFAULT_MASK, pObj!=NULL);

  oldIntState = Hwi_disable();

  pObj->openCnt--;

  Hwi_restore(oldIntState);

  return 0;
}

Int32 VpsHal_nfReset(VpsHal_Handle handle)
{
  VpsHal_NfConfig config;
  UInt16 frameNoise[3], i;

  VpsHal_nfGetDefaultConfig(&config);

  VpsHal_nfSetConfig(handle, &config, NULL);

  frameNoise[0]=frameNoise[1]=frameNoise[2]=0;

  for(i=0; i<VPSHAL_NF_MAX_VIDEO_SRC_IDX; i++)
  {
    Nf_writeSavedFrameNoise(handle, i, frameNoise, NULL);
  }

  return 0;
}

Int32 VpsHal_nfGetDefaultConfig(VpsHal_NfConfig *config)
{
  config->enable                  = FALSE;
  config->refFrameConfig          = VPS_NSF_REF_FRAME_ENABLED;
  config->bypassSpatial           = FALSE;
  config->bypassTemporal          = FALSE;
  config->videoSrcIndex           = 0;
  config->initFrameNoise          = FALSE;
  config->updateFrameNoise        = FALSE;
  config->width                   = 0;
  config->height                  = 0;
  config->spatialStrengthLow[0]   = 0x8;
  config->spatialStrengthLow[1]   = 0x10;
  config->spatialStrengthLow[2]   = 0x10;
  config->spatialStrengthHigh[0]  = 0x8;
  config->spatialStrengthHigh[1]  = 0x10;
  config->spatialStrengthHigh[2]  = 0x10;
  config->temporalStrength        = 0xD;
  config->temporalTriggerNoise    = 0x6;
  config->noiseIirCoeff           = 0xD;
  config->maxNoise                = 0xA;
  config->pureBlackThres          = 0xF;
  config->pureWhiteThres          = 0xF;
  config->frameNoise[0]           = 0;
  config->frameNoise[1]           = 0;
  config->frameNoise[2]           = 0;

  return 0;
}

Int32 Nf_readSavedFrameNoise(VpsHal_Handle handle, UInt16 srcIdx, UInt16 frameNoise[3])
{
  VpsHal_NfObj *pObj=(VpsHal_NfObj *)handle;
  UInt32 oldIntState;

  GT_assert( GT_DEFAULT_MASK, pObj!=NULL);
  GT_assert( GT_DEFAULT_MASK, srcIdx < VPSHAL_NF_MAX_VIDEO_SRC_IDX );

  oldIntState = Hwi_disable();

  pObj->regs->NF_REG7 = (srcIdx << CSL_VPS_NF_NF_REG7_FRAME_NOISE_READ_INDEX_SHIFT ) & CSL_VPS_NF_NF_REG7_FRAME_NOISE_READ_INDEX_MASK;

  frameNoise[0] = (pObj->regs->NF_REG8 & CSL_VPS_NF_NF_REG8_FRAME_NOISE_Y_MASK) >> CSL_VPS_NF_NF_REG8_FRAME_NOISE_Y_SHIFT;
  frameNoise[1] = (pObj->regs->NF_REG9 & CSL_VPS_NF_NF_REG9_FRAME_NOISE_U_MASK) >> CSL_VPS_NF_NF_REG9_FRAME_NOISE_U_SHIFT;
  frameNoise[2] = (pObj->regs->NF_REG9 & CSL_VPS_NF_NF_REG9_FRAME_NOISE_V_MASK) >> CSL_VPS_NF_NF_REG9_FRAME_NOISE_V_SHIFT;

  Hwi_restore(oldIntState);

  return 0;

}

Int32 Nf_writeSavedFrameNoise(VpsHal_Handle handle, UInt16 srcIdx, UInt16 frameNoise[3], VpsHal_NfConfigOverlay *configOverlay)
{
  VpsHal_NfObj *pObj=(VpsHal_NfObj *)handle;
  UInt32 oldIntState, value;

  GT_assert( GT_DEFAULT_MASK, pObj!=NULL);
  GT_assert( GT_DEFAULT_MASK, srcIdx < VPSHAL_NF_MAX_VIDEO_SRC_IDX );

  if(configOverlay==NULL)
    oldIntState = Hwi_disable();

  value = (srcIdx << CSL_VPS_NF_NF_REG7_FRAME_NOISE_READ_INDEX_SHIFT ) & CSL_VPS_NF_NF_REG7_FRAME_NOISE_READ_INDEX_MASK;

  if(configOverlay)
    configOverlay->NF_REG7 = value;
  else
    pObj->regs->NF_REG7 = value;

  value = ( ((UInt32)frameNoise[0] << CSL_VPS_NF_NF_REG8_FRAME_NOISE_Y_SHIFT ) & CSL_VPS_NF_NF_REG8_FRAME_NOISE_Y_MASK );

  if(configOverlay)
    configOverlay->NF_REG8 = value;
  else
    pObj->regs->NF_REG8 = value;

  value = ( ((UInt32)frameNoise[1] << CSL_VPS_NF_NF_REG9_FRAME_NOISE_U_SHIFT ) & CSL_VPS_NF_NF_REG9_FRAME_NOISE_U_MASK )
         |( ((UInt32)frameNoise[2] << CSL_VPS_NF_NF_REG9_FRAME_NOISE_V_SHIFT ) & CSL_VPS_NF_NF_REG9_FRAME_NOISE_V_MASK )
         ;

  if(configOverlay)
    configOverlay->NF_REG9 = value;
  else
    pObj->regs->NF_REG9 = value;


  if(configOverlay==NULL)
    Hwi_restore(oldIntState);

  return 0;
}

Int32 VpsHal_nfSetWidthHeight(VpsHal_Handle handle, UInt16 width, UInt16 height, VpsHal_NfConfigOverlay *configOverlay)
{
  VpsHal_NfObj *pObj=(VpsHal_NfObj *)handle;
  UInt32 value;

  GT_assert( GT_DEFAULT_MASK, pObj!=NULL);

  value =   ( ( (UInt32)(width  & ~0x1) << CSL_VPS_NF_NF_REG1_WIDTH_SHIFT ) & CSL_VPS_NF_NF_REG1_WIDTH_MASK  )
          | ( ( (UInt32)(height & ~0x1) << CSL_VPS_NF_NF_REG1_HEIGHT_SHIFT) & CSL_VPS_NF_NF_REG1_HEIGHT_MASK )
          ;

  if(configOverlay)
    configOverlay->NF_REG1 = value;
  else
    pObj->regs->NF_REG1 = value;

  return 0;
}

Int32 VpsHal_nfSetConfig(VpsHal_Handle handle, VpsHal_NfConfig *config, VpsHal_NfConfigOverlay *configOverlay)
{
  VpsHal_NfObj *pObj=(VpsHal_NfObj *)handle;
  UInt32 value, bypass, oldIntState;

  GT_assert( GT_DEFAULT_MASK, pObj!=NULL);
  GT_assert( GT_DEFAULT_MASK, config!=NULL);

  if(configOverlay==NULL)
    oldIntState = Hwi_disable();

  VpsHal_nfSetWidthHeight(handle, config->width, config->height, configOverlay);

  value =  ( ((UInt32)config->spatialStrengthLow[0]  << CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_LOW_SHIFT ) & CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_LOW_MASK  )
          |( ((UInt32)config->spatialStrengthHigh[0] << CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_HIGH_SHIFT) & CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_HIGH_MASK )
          ;

  if(configOverlay)
    configOverlay->NF_REG2 = value;
  else
    pObj->regs->NF_REG2 = value;

  value =  ( ((UInt32)config->spatialStrengthLow[1]  << CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_LOW_SHIFT ) & CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_LOW_MASK  )
          |( ((UInt32)config->spatialStrengthHigh[1] << CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_HIGH_SHIFT) & CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_HIGH_MASK )
          |( ((UInt32)config->spatialStrengthLow[2]  << CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_LOW_SHIFT ) & CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_LOW_MASK  )
          |( ((UInt32)config->spatialStrengthHigh[2] << CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_HIGH_SHIFT) & CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_HIGH_MASK )
          ;

  if(configOverlay)
    configOverlay->NF_REG3 = value;
  else
    pObj->regs->NF_REG3 = value;

  value =  ( ((UInt32)config->temporalStrength     << CSL_VPS_NF_NF_REG4_TEMPORAL_STRENGTH_SHIFT            ) & CSL_VPS_NF_NF_REG4_TEMPORAL_STRENGTH_MASK             )
          |( ((UInt32)config->temporalTriggerNoise << CSL_VPS_NF_NF_REG4_TEMPORAL_FILTER_TRIGGER_NOISE_SHIFT) & CSL_VPS_NF_NF_REG4_TEMPORAL_FILTER_TRIGGER_NOISE_MASK )
          ;

  if(configOverlay)
    configOverlay->NF_REG4 = value;
  else
    pObj->regs->NF_REG4 = value;

  value =  ( ((UInt32)config->noiseIirCoeff << CSL_VPS_NF_NF_REG5_NOISE_IIR_COEFFICIENT_SHIFT ) & CSL_VPS_NF_NF_REG5_NOISE_IIR_COEFFICIENT_MASK)
          |( ((UInt32)config->maxNoise      << CSL_VPS_NF_NF_REG5_MAX_NOISE_SHIFT             ) & CSL_VPS_NF_NF_REG5_MAX_NOISE_MASK)
          ;

  if(configOverlay)
    configOverlay->NF_REG5 = value;
  else
    pObj->regs->NF_REG5 = value;

  value =  ( ((UInt32)config->pureBlackThres << CSL_VPS_NF_NF_REG6_PURE_BLACK_THRESHOLD_SHIFT ) & CSL_VPS_NF_NF_REG6_PURE_BLACK_THRESHOLD_MASK)
          |( ((UInt32)config->pureWhiteThres << CSL_VPS_NF_NF_REG6_PURE_WHITE_THRESHOLD_SHIFT ) & CSL_VPS_NF_NF_REG6_PURE_WHITE_THRESHOLD_MASK)
          ;

  if(configOverlay)
    configOverlay->NF_REG6 = value;
  else
    pObj->regs->NF_REG6 = value;

  Nf_writeSavedFrameNoise(handle, config->videoSrcIndex, config->frameNoise, configOverlay);

  bypass = 0;

  value = ( ((UInt32)config->enable           << CSL_VPS_NF_NF_REG0_NF_EN_SHIFT                ) & CSL_VPS_NF_NF_REG0_NF_EN_MASK )
        | ( ((UInt32)config->refFrameConfig   << CSL_VPS_NF_NF_REG0_NF_REF_CFG_SHIFT           ) & CSL_VPS_NF_NF_REG0_NF_REF_CFG_MASK )
        | ( ((UInt32)config->videoSrcIndex    << CSL_VPS_NF_NF_REG0_NF_VIDEO_INDEX_SHIFT       ) & CSL_VPS_NF_NF_REG0_NF_VIDEO_INDEX_MASK )
        | ( ((UInt32)bypass                   << CSL_VPS_NF_NF_REG0_NF_BYPASS_CFG_SHIFT        ) & CSL_VPS_NF_NF_REG0_NF_BYPASS_CFG_MASK )
        | ( ((UInt32)config->initFrameNoise   << CSL_VPS_NF_NF_REG0_NF_LOAD_FRAME_NOISE_SHIFT  ) & CSL_VPS_NF_NF_REG0_NF_LOAD_FRAME_NOISE_MASK)
        | ( ((UInt32)config->updateFrameNoise << CSL_VPS_NF_NF_REG0_NF_UPDATE_FRAME_NOISE_SHIFT) & CSL_VPS_NF_NF_REG0_NF_UPDATE_FRAME_NOISE_MASK)
        ;

  if(configOverlay)
    configOverlay->NF_REG0 = value;
  else
    pObj->regs->NF_REG0 = value;

  if(configOverlay) {
    extern UInt32  gVpsHal_vpsBaseAddress;
    UInt32 nfRegOffset = ((UInt32)pObj->regs - gVpsHal_vpsBaseAddress);

    configOverlay->NF_ADDR0     = nfRegOffset + 4; // addr offset of NF_REG1
    configOverlay->NF_NUM_REGS0 = 9;
    configOverlay->NF_ADDR1     = nfRegOffset; // addr offset of NF_REG0
    configOverlay->NF_NUM_REGS1 = 1;
  }

  if(configOverlay==NULL)
    Hwi_restore(oldIntState);

  return 0;
}


