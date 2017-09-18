#ifdef TI_814X_BUILD
#define VPS_HAL_CVBS_GAIN
#endif

/* Set at default value */
static VpsHal_SdVenc2xUpSampCoeff defaultLuma2xUpSampCoeffs  =
    { 0, 0, 0, 0, 6, -20, 78 };

/* Set at default value */
static VpsHal_SdVencLpfCoeff    defaultLumaLpfCoeffs =
    { 0, 0, 0, 0, 32, 64 };

/* Set at default value */
static VpsHal_SdVencLpfCoeff    defaultChromaLpfCoeffs =
    {0, 0, 0, 12, 32, 40 };

/* Full-range RGB [0…255] to YUV 10:4 Picture Sync Ratio with 7.5% setup (NTSC) */
static VpsHal_SdVencCscCoeff    defaultCvbsCscCoeffs_NTSC =
#ifdef VPS_HAL_CVBS_GAIN
    {
        {{ 310, 60, 158 }, { -126, 191, -65 }, { -225, -44, 269 }},
        { 0, 0, 0 },
        { 162, 0 , 0 }
    };
#else
    {
        {{ 285, 55, 145 }, { -140, 211, -71 }, { -250, -48, 298 }},
        { 0, 0, 0 },
        { 157, 0 , 0 }
    };
#endif
static VpsHal_SdVencCscCoeff    defaultComponentCscCoeffs_NTSC =
    {
        {{ 285, 55, 145 }, { -140, 211, -71 }, { -250, -48, 298 }},
        { 0, 0 , 0 },
        { 157, 0 , 0 }
    };
/* Full-range RGB [0…255] to YUV 7:3 Picture Sync Ratio with zero setup (PAL) */
static VpsHal_SdVencCscCoeff    defaultCvbsCscCoeffs_PAL =
#ifdef VPS_HAL_CVBS_GAIN
    {
        {{ 327, 64, 167 }, { -129, 194, -67 }, { -230, -45, 275 }},
        { 0, 0, 0 },
        { 0, 0 , 0 }
    };
#else
    {
        {{ 351, 68, 179 }, { -173, 261, -88 }, { -308, -60, 368 }},
        { 0, 0, 0 },
        { 0, 0 , 0 }
    };
#endif
static VpsHal_SdVencCscCoeff    defaultComponentCscCoeffs_PAL =
    {
        {{ 351, 68, 179 }, { -173, 261, -88 }, { -308, -60, 368 }},
        { 0, 0, 0 },
        { 0, 0 , 0 }
    };

static VpsHal_SdVencDac2xOverSampCoeff defaultDac2xOvrSampCoeffs =
    { 0, 0, 0, 0, 6, -20, 78 };

VpsHal_SdVencConfig sdVencNtscCfg =
{
    FVID2_STD_NTSC,
    FALSE,                      /* Enable */
    VPS_SF_INTERLACED,          /* ScanFormat */
    FALSE,                      /* unEquallinesperField */
    FALSE,                      /* TestColorBar */
    FALSE,                      /* InputInversion */
    FALSE,                      /* isSalveMode */
    1716,                       /* Total Pixels */
    525,                        /* Total Lines */
    0x0,                        /* irqOutputStartPixel Not Supported */
    0x0,                        /* irqOutputStartLine Not Supported */
    FALSE,                      /* EnableGammaCorrection */
    FALSE,                      /* Upsampling2XEnable *//* TODO */
    TRUE,                      /* Dac2xOverSamplingEnable */
    VPSHAL_SDVENC_VERT_SYNC_WID_30, /* 3H */
    TRUE,                       /* blankingShapeEnable*/
    FALSE,                      /* vbiEnable */
    VPSHAL_SDVENC_PIX_CLK_1X,   /* pixelRateMultiplier TODO */ /* TODO */
    VPSHAL_SDVENC_FMT_525I,     /* fmt */

    /* DTV Config */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* hsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* vsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* fidPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* activVidPolarity */
        0,                                      /* hSyncStartPixel */
        4,                                      /* hSyncStopPixel */
        5,                                      /* vSyncStartPixel */
        9,                                      /* vSyncStopPixel */
        0x6,                                    /* vSyncStartLine */
        12,                                     /* vSyncStopLine */
        8,                                      /* fidTogglePixelPosition */
        14,                                     /* fidStartLine0 */
        0x0,                                    /* fidId0 */
        12,                                     /* fidStartLine1 */
        0x1,                                    /* fidId1 */
        242,                                    /* avidStartPixel TODO It should be 276 */
        1682,                                   /* avidStopPixel TODO It should be 1716 */
        36,                                     /* avidStartLine0 */
        518,                                    /* avidStopLine0 */
        37,                                     /* avidStartLine1 */
        519,                                    /* avidStopLine1 */
    },

    /* TODO */
    /* vpsHal_SdVencDvoConfig */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* hsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* vsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* fidPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* activVidPolarity */
        0,                             /* hSyncStartPixel */
        4,                             /* hSyncStopPixel */
        5,                             /* vSyncStartPixel */
        9,                             /* vSyncStopPixel */
        6,                             /* vSyncStartLine */
        12,                             /* vSyncStopLine */
        8,                             /* fidTogglePixelPosition */
        14,                             /* fidStartLine0 */
        0,                             /* fidId0 */
        12,                             /* fidStartLine1 */
        1,                             /* fidId1 */
        242,                                    /* avidStartPixel TODO It should be 276 */
        1682,                                   /* avidStopPixel TODO It should be 1716 */
        34,                                     /* avidStartLine0 */
        518,                                    /* avidStopLine0 */
        33,                                     /* avidStartLine1 */
        519,                                    /* avidStopLine1 */
        0x0,                             /* horzDelay */
    },
    /* tvDetectConfig */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* tvDetePolarity  Not supported*/
        0X0,                                   /* tvDeteStartPixel */
        0x0,                                   /* tvDeteStopPixel */
        0x0,                                   /* tvDeteStartLine */
        0x1,                                   /* teDeteStopLine */
    },
    /* slaveConfig */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* hsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* vsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* fidPolarity */
        3,                                     /* fieldDetectionMode */
    },
    /* encoderTiming */
    {
        244,                                   /* activeVidHorzStartPos */
        1684,                                  /* activeVidHorzStopPos */
        34,                                    /* activeVidVertStarPos0 */
        519,                                   /* activeVidVertStopPos0 */
        34,                                    /* activeVidVertStarPos1 */
        519,                                   /* activeVidVertStopPos1 */
        143,                                   /* colorBurstStartPos */
        211,                                   /* colorBurstStopPos */
        0x0,                                   /* vbiReqStartPos Not supported */
        0x0,                                   /* vbiReqStartPos */

    },
    /* cvbsConfig */
    {
        188,                                   /* synctipAmp */
#ifdef VPS_HAL_CVBS_GAIN
        777,                                   /* syncAmp */
#else
        836,                                   /* syncAmp */
#endif
        VPSHAL_SDVENC_PIC_SYNC_RAT_10_4,       /* pictSynRatio 10:4 */
        FALSE,                                 /* lumaLpfEnable */
        TRUE,                                 /* chromaLpfEnable */
        VPSHAL_SDVENC_COL_MODULATION_MODE_NTSC,/* colorModulation */
        0x0,                                   /* yDelayAdjustment */
#ifdef VPS_HAL_CVBS_GAIN
        362,                                   /* burstAmplitude */
#else
        418,                                   /* burstAmplitude */
#endif
        4095,                                  /* yClipUpperLimit */
        0,                                     /* yClipLowerLimit */
        2047,                                  /* uClipUpperLimit */
        -2048,                                 /* uClipLowerLimit */
        2047,                                  /* vClipUpperLimit */
        -2048,                                 /* vClipLowerLimit */
    },
    /* componentConfig */
    {
        188,                                   /* synctipAmp */
        836,                                   /* syncAmp */
        VPSHAL_SDVENC_PIC_SYNC_RAT_10_4,       /* pictSynRatio 10:4 */
        VPSHAL_SDVENC_OUTPUT_YUV,              /* outColorSpace RGB */
        TRUE,                                  /* syncOnYOrG */
        FALSE,                                 /* syncOnPbOrB */
        FALSE,                                 /* syncOnPrOrR */
        4095,                                  /* yClipUpperLimit */
        0,                                     /* yClipLowerLimit */
        4095,                                  /* uClipUpperLimit */
        0,                                     /* uClipLowerLimit */
        4095,                                  /* vClipUpperLimit */
        0,                                     /* vClipLowerLimit */
    },
    /* dacConfig */
    {
        VPSHAL_SDVENC_DAC_SEL_CVBS,            /* dac0 */ /* TODO */
        VPSHAL_SDVENC_DAC_SEL_CVBS,        /* dac1 */
        VPSHAL_SDVENC_DAC_SEL_CVBS,        /* dac2 */
        VPSHAL_SDVENC_DAC_SEL_CVBS,        /* dac3 */
        TRUE,                                  /* enableDac0 */
        TRUE,                                 /* enableDac1 */
        TRUE,                                 /* enableDac2 */
        TRUE,                                 /* enableDac3 */
        0,                                     /* dclevelControl */
        VPSHAL_SDVENC_DC_MODE_NORMAL,          /* dcOutputMode */
        FALSE,                                 /* invert */
    },
    /* ccConfig */
    {
        VPSHAL_SDVENC_CC_NO_DATA,              /* ccSelection */
        0x0,                                   /* ccDefaultData */
        {
            0x0,                               /* ccData0 */
            0x0,                               /* ccData0 */
        },
        {
            0x0,                               /* ccData1 */
            0x0,                               /* ccData1 */
        },
    },
    /* wssConfig */
    {
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        FALSE,                                 /* enable */
        VPSHAL_SDVENC_WSS_MODE0,
    },
    /* cgmsConfig */
    {
        0x0,
        FALSE,                                 /* enable */
        0x0,
        0x0,
        0x0,
        0x0,
    },
    NULL,                                      /* cvbsCoeffs */
    NULL,                                      /* componentCoeffs */
    NULL,                                      /* lumaLpfCoeffs */
    NULL,                                      /* chromaLpfCoeffs */
    NULL,                                      /* upSamplingCoeffs */
    NULL,                                      /* dacOverSamplingCoeffs */
    0,                                         /* subCarrierIniPhase */
    135,
    25,
    33
};

VpsHal_SdVencConfig sdVencPalCfg =
{
    FVID2_STD_PAL,
    FALSE,                      /* Enable */
    VPS_SF_INTERLACED,          /* ScanFormat */
    FALSE,                      /* unEquallinesperField TODO*/
    FALSE,                      /* TestColorBar */
    FALSE,                      /* InputInversion */
    FALSE,                      /* isSalveMode */
    1728,                       /* Total Pixels */
    625,                        /* Total Lines */
    0x0,                        /* irqOutputStartPixel Not Supported */
    0x0,                        /* irqOutputStartLine Not Supported */
    FALSE,                      /* EnableGammaCorrection */
    FALSE,                      /* Upsampling2XEnable */
    TRUE,                       /* Dac2xOverSamplingEnable */
    VPSHAL_SDVENC_VERT_SYNC_WID_25, /* 25H */
    TRUE,                       /* blankingShapeEnable*/
    FALSE,                      /* vbiEnable */
    VPSHAL_SDVENC_PIX_CLK_1X,   /* pixelRateMultiplier TODO */
    VPSHAL_SDVENC_FMT_625I,     /* fmt */

    /* DTV Config */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* hsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* vsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* fidPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* activVidPolarity */
        0x0,                                    /* hSyncStartPixel */
        0x5,                                    /* hSyncStopPixel */
        5,                                      /* vSyncStartPixel */
        9,                                      /* vSyncStopPixel */
        12,                                     /* vSyncStartLine */
        22,                                     /* vSyncStopLine */
        8,                                      /* fidTogglePixelPosition */
        0xe,                                    /* fidStartLine0 */
        0x0,                                    /* fidId0 */
        0xc,                                    /* fidStartLine1 */
        0x1,                                    /* fidId1 */
        264,/*286,*/                            /* avidStartPixel TODO It should be 276 */
        1704,/*1726,*/                          /* avidStopPixel TODO It should be 1716 */
        44,                                     /* avidStartLine0 */
        620,                                    /* avidStopLine0 */
        45,                                     /* avidStartLine1 */
        621,                                    /* avidStopLine1 */

    },

    /* TODO */
    /* vpsHal_SdVencDvoConfig */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* hsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* vsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* fidPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,     /* activVidPolarity */
        0xFFFFFFFF,                             /* hSyncStartPixel */
        0xFFFFFFFF,                             /* hSyncStopPixel */
        0x0,                             /* vSyncStartPixel */
        1728,                             /* vSyncStopPixel */
        0,                             /* vSyncStartLine */
        17,                             /* vSyncStopLine */
        6,                             /* fidTogglePixelPosition */
        0xc,                             /* fidStartLine0 */
        0x0,                             /* fidId0 */
        0xe,                             /* fidStartLine1 */
        0x1,                             /* fidId1 */
        286,                                    /* avidStartPixel TODO It should be 276 */
        1726,                                   /* avidStopPixel TODO It should be 1716 */
        48,                                     /* avidStartLine0 */
        624,                                    /* avidStopLine0 */
        49,                                     /* avidStartLine1 */
        625,                                    /* avidStopLine1 */
        0x0,                             /* horzDelay */
    },
    /* tvDetectConfig */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* tvDetePolarity  Not supported*/
        0X0,                                   /* tvDeteStartPixel */
        0x0,                                   /* tvDeteStopPixel */
        0x0,                                   /* tvDeteStartLine */
        0x1,                                   /* teDeteStopLine */
    },
    /* slaveConfig */
    {
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* hsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* vsPolarity */
        VPSHAL_SDVENC_POLARITY_ACTIVE_HIGH,    /* fidPolarity */
        3,                                     /* fieldDetectionMode */
    },
    /* encoderTiming */
    {
        264,                                   /* activeVidHorzStartPos */
        1704,                                  /* activeVidHorzStopPos */
        45,                                    /* activeVidVertStarPos0 */
        620,                                   /* activeVidVertStopPos0 */
        45,                                    /* activeVidVertStarPos1 */
        620,                                   /* activeVidVertStopPos1 */
        151,                                   /* colorBurstStartPos */
        212,                                   /* colorBurstStopPos */
        0x0,                                   /* vbiReqStartPos Not supported */
        0x0,                                   /* vbiReqStartPos */

    },
    /* cvbsConfig */
    {
        148,                                   /* synctipAmp */
        878,                                   /* syncAmp */
        VPSHAL_SDVENC_PIC_SYNC_RAT_7_3,        /* pictSynRatio 10:4 */
        FALSE,                                 /* lumaLpfEnable */
        TRUE,                                  /* chromaLpfEnable */
        VPSHAL_SDVENC_COL_MODULATION_MODE_PAL, /* colorModulation */
        0x0,                                   /* yDelayAdjustment */
#ifdef VPS_HAL_CVBS_GAIN
        263,                                   /* burstAmplitude */
#else
        310,                                   /* burstAmplitude */
#endif
        4095,                                  /* yClipUpperLimit */
        0,                                     /* yClipLowerLimit */
        2047,                                  /* uClipUpperLimit */
        -2048,                                 /* uClipLowerLimit */
        2047,                                  /* vClipUpperLimit */
        -2048,                                 /* vClipLowerLimit */
    },
    /* componentConfig */
    {
        148,                                   /* synctipAmp */
        878,                                   /* syncAmp */
        VPSHAL_SDVENC_PIC_SYNC_RAT_7_3,        /* pictSynRatio 7:3 */
        VPSHAL_SDVENC_OUTPUT_YUV,              /* outColorSpace RGB */
        TRUE,                                  /* syncOnYOrG */
        FALSE,                                 /* syncOnPbOrB */
        FALSE,                                 /* syncOnPrOrR */
        4095,                                  /* yClipUpperLimit */
        0,                                     /* yClipLowerLimit */
        4095,                                  /* uClipUpperLimit */
        0,                                     /* uClipLowerLimit */
        4095,                                  /* vClipUpperLimit */
        0,                                     /* vClipLowerLimit */
    },
    /* dacConfig */
    {
        VPSHAL_SDVENC_DAC_SEL_CVBS,            /* dac0 */
        VPSHAL_SDVENC_DAC_SEL_SVIDEO_Y,        /* dac1 */
        VPSHAL_SDVENC_DAC_SEL_SVIDEO_C,        /* dac2 */
        VPSHAL_SDVENC_DAC_SEL_SVIDEO_C,        /* dac3 */
        TRUE,                                  /* enableDac0 */
        FALSE,                                 /* enableDac1 */
        FALSE,                                 /* enableDac2 */
        FALSE,                                 /* enableDac3 */
        0,                                     /* dclevelControl */
        VPSHAL_SDVENC_DC_MODE_NORMAL,          /* dcOutputMode */
        FALSE,                                 /* invert */
    },
    /* ccConfig */
    {
        VPSHAL_SDVENC_CC_NO_DATA,              /* ccSelection */
        0x0,                                   /* ccDefaultData */
        {
            0x0,                               /* ccData0 */
            0x0,                               /* ccData0 */
        },
        {
            0x0,                               /* ccData1 */
            0x0,                               /* ccData1 */
        },
    },
    /* wssConfig */
    {
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        0x0,
        FALSE,                                 /* enable */
        VPSHAL_SDVENC_WSS_MODE0,
    },
    /* cgmsConfig */
    {
        0x0,
        FALSE,                                 /* enable */
        0x0,
        0x0,
        0x0,
        0x0,
    },
    NULL,                                      /* cvbsCoeffs */
    NULL,                                      /* componentCoeffs */
    NULL,                                      /* lumaLpfCoeffs */
    NULL,                                      /* chromaLpfCoeffs */
    NULL,                                      /* upSamplingCoeffs */
    NULL,                                      /* dacOverSamplingCoeffs */
    0,                                         /* subCarrierIniPhase */
    168,
    2516,
    16875
};

static Vps_DcModeInfo standardModes[] =
{
    /* For NTSC */
    {
        0x0,
        {FVID2_STD_NTSC, 720, 480, FVID2_SF_INTERLACED, 216000, 60,
	    12, 68, 64, 5, 41, 5},
        0,
        0,
        0
    },
    /* For PAL */
    {
        0x0,
        {FVID2_STD_PAL, 720, 576, FVID2_SF_INTERLACED, 216000, 50,
	    16, 58, 64, 6, 31, 6},
        0,
        0,
        0
    },
};

static Int32 VpsHalSdVencSineTable[31] =
{
0,
175,
349,
523,
698,
872,
1045,
1219,
1392,
1564,
1736,
1908,
2079,
2250,
2419,
2588,
2756,
2924,
3090,
3256,
3420,
3584,
3746,
3907,
4067,
4226,
4384,
4540,
4695,
4848,
5000
};
static Int32 VpsHalSdVencCosineTable[31] = {
10000,
9998,
9994,
9986,
9976,
9962,
9945,
9925,
9903,
9877,
9848,
9816,
9781,
9744,
9703,
9659,
9613,
9563,
9511,
9455,
9397,
9336,
9272,
9205,
9135,
9063,
8988,
8910,
8829,
8746,
8660
};
