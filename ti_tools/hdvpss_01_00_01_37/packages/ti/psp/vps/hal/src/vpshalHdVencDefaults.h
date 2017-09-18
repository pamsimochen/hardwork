static VpsHal_HdVencConfig gHdVencDefCfg =
{
    FVID2_STD_1080P_60,                         /* hdVencMode */
    VPSHAL_HDVENC_DMODE_720P,                   /* dispMode*/
    FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    FALSE,                                      /* bypass2XUpSampling*/
    /* This is set to false for HD Dac depending upon instance
     * inside the code. VpsHal_hdVencSetMode function.
     */
    TRUE,                                       /* bypassCsc */
    TRUE,                                       /* bypassGammaCor*/
    FALSE,                                      /* rdGammaCorMem */
    FALSE,                                      /* selfTest */
    FALSE,                                      /* startEncoder*/
    VPSHAL_HDVENC_CSCMODE_HDTV_GRAPHICS_R2Y,    /* cscMode */
    NULL,                                       /* cscCoefficients */
    NULL,                                       /* Gamma Coeffs*/
    /* Analog Config */
    {
        VPSHAL_HDVENC_COLORSPACE_YUV,           /* outColorSpace */
        FALSE,                                  /* dac0PwDn */
        FALSE,                                  /* dac1PwDn */
        FALSE,                                  /* dac2PwDn */
        FALSE,                                  /* dacInvertData */
        FALSE,                                  /* dac0RfPdn */
        FALSE,                                  /* dac1RfPdn */
        0xC77,                                  /* scale0Coeff */
        0xC0,                                   /* shift0Coeff */
        0x184,                                  /* blank0Level */
        0xC30,                                  /* scale1Coeff */
        0xC0,                                   /* shift1Coeff */
        0x1C0,                                  /* blank1Level */
        0xC30,                                  /* scale2Coeff */
        0xC0,                                   /* shift2Coeff */
        0x1C0,                                  /* blank2Level */
        2200,                                   /* totalPixelsPerLine */
        1125,                                   /* totalLinesPerFrame */
        132,                                    /* clampPeriod */
        44,                                     /* vbiLineEnd0 */
        0x245,                                  /* vbiLineend1 */
        0x3F,                                   /* equPulseWidth */
        191,                                    /* hbiPixelEnd */
        1920,                                   /* numActivePixels */
        0x3F,                                   /* hsWidth */
        1124,                                   /* lastActiveLineFirstField */
        0,                                      /* pbPrShifting */
        0,                                      /* VencEnableDelayCount */
        0x338,                                  /* syncHighLevelAmp */
        0x38                                    /* syncLowLevelAmp */
    },
    /* DVO  Config */
    {
        VPSHAL_HDVENC_OUTPUTSRC_COLORSPC,        /* dataSource */
        /* bypassCsc This is set to
         * VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_EMBSYNC for HD Dac
         * depending upon instance inside the code.
         * VpsHal_hdVencSetMode function.
         */
        VPSHAL_HDVENC_DVOFMT_10BIT_TRIPLECHAN_DISCSYNC, /* fmt */
        FALSE,                                  /* invtfidPol */
        FALSE,                                  /* invtVsPol */
        FALSE,                                  /* invtHsPol */
        FALSE,                                  /* invtActsPol */
        VPSHAL_HDVENC_444TO422_DECIMATION,      /* decimationMethod */
        FALSE,                                  /* dvoOff */
        VPSHAL_HDVENC_HFLINE_START_LINE_BEGINNING,  /* vsFidLineStart */
        FALSE,                                  /* jeidaFmt */
        192,                                    /* firstActivePixelLine */
        1920,                                   /* activeVideoLineWidth */
        44,                                     /* hsWidth */
        0,                                      /* hsStartPixel */
        45,                                     /* activeLineStart0 */
        0,                                      /* activeLineStart1 */
        1080,                                   /* activeVideoLines0 */
        0,                                      /* activeVideoLines1 */
        4,                                      /* vsStartLine0 */
        0,                                      /* vsStartLine1 */
        5,                                      /* vsWidth0 */
        0,                                      /* vsWidth1 */
        1,                                      /* fidToggleLine0 */
        0                                       /* fidToggleLine1 */
    },
    /* OSD Config */
    {
        FALSE,                                  /* invtfidPol */
        0,                                      /* HbiSignalStartPixel */
        184,                                    /* lastNonActivePixelInLine */
        1920,                                   /* activeVideoLineWidth */
        44,                                     /* hsWidth */
        1,                                      /* hsStartPixel */
        45,                                     /* activeLineStart0 */
        0,                                      /* activeLineStart1 */
        1080,                                   /* activeVideoLines0 */
        0,                                      /* activeVideoLines1 */
        1,                                      /* vsStartLine0 */
        0,                                      /* vsStartLine1 */
        5,                                      /* vsWidth0 */
        0,                                      /* vsWidth1 */
        1,                                      /* fidToggleLine0 */
        0                                       /* fidToggleLine1 */
    },
    /* Macro Vision Config */
    {
        0,                                      /* macroVisionEnable  */
        0,                                      /* peakingValue */
        0,                                      /* backPorchPeakValue */
        0,                                      /* mv100mvAgcPulseYSection */
        0,                                      /* mv450mvAgcPulseXSection */
        (VpsHal_HdVencMvBlankLevelReduction)0,
                                                /* blankLevelReductionMethod */
        (VpsHal_HdVencAgcAmpControl)0,
                                                /* agcAmp */
        0,                                      /* backPorchPulseControl */
        0,                                      /* mvEnable480i */
        0,                                      /* syncPulseDurationFormatB */
        0,                                      /* syncPulseDurationFormatA */
        0,                                      /* syncPulseLocationFormatB */
        0,                                      /* syncPulseLocationFormatA */
        0,                                      /* syncPulseSpacingFormatB */
        0,                                      /* syncPulseSpacingFormatA */
        0,                                      /* enablePseSyncPulseAndAgc */
        0,                                      /* PseSyncPulseAndAgcFormat */
        0,                                      /* enableFormatAPulsePair */
        0,                                      /* enableFormatBPulsePair */
        0,                                      /* beforevSyncPulseInsertion */
        0,                                      /* AftervSyncPulseInsertion */
        0,                                      /* blankLevelReduction */
    },
    /* VBI Config */
    {
        { 0, 0 },                               /* VbiLineLoc0 */
        { 0, 0 },                               /* VbiLineLoc1 */
        { FALSE, FALSE },                       /* enableVbiLine0 */
        { FALSE, FALSE },                       /* enableVbiLine1 */
        (VpsHal_HdVencVbiStartSegShift)0,
                                                /* startSegShift */
        (VpsHal_HdVencVbiHeaderSegShift)0,
                                                /* headerSegShift */
        (VpsHal_HdVencVbiPayloadSegShift)0,
                                                /* pdsSegShift */
        { 0, 0 },                               /* VbiPixelLoc */
        { 0, 0 },                               /* startSymbolWidth */
        { 0, 0 },                               /* numSymbolsStartSeg */
        { 0, 0 },                               /* headerSymbolWidth */
        { 0, 0 },                               /* numSymbolHeaderSeg */
        { 0, 0 },                               /* payLoadSymbolWidth */
        { 0, 0 },                               /* numSymbolPayloadSeg */
        { 0, 0 },                               /* startSegStream0 */
        { 0, 0 },                               /* startSegStream1 */
        { 0, 0 },                               /* headerSegStream0 */
        { 0, 0 },                               /* headerSegStream1 */
        { 0, 0 },                               /* payLoadSegStream0 */
        { 0, 0 },                               /* payLoadSegStream1 */
        { 0, 0 },                               /* payLoadSegStream2 */
        { 0, 0 },                               /* payLoadSegStream3 */
        { 0, 0 },                               /* hstateAmplitude */
        { 0, 0 },                               /* lstateAmplitude */
        0,                                      /* startPixelExtVbi */
        0,                                      /* endPixelExtVbi */
        FALSE                                   /* enableExternalVbi */
    }
};

static Vps_DcModeInfo standardModes[] =
{
    /* For 1080P60 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_1080P_60,                         /* hBackPorch */
        1920,                                       /* width */
        1080,                                       /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For 1080P50 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_1080P_50,                         /* hBackPorch */
        1920,                                       /* width */
        1080,                                       /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For 1080I60 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_1080I_60,                         /* hBackPorch */
        1920,                                       /* width */
        1080,                                       /* height0 */
        FVID2_SF_INTERLACED,                        /* scanFormat */
    },
    /* For 1080I50 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_1080I_50,                         /* hBackPorch */
        1920,                                       /* width */
        1080,                                       /* height0 */
        FVID2_SF_INTERLACED,                        /* scanFormat */
    },
    /* For 720P60 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_720P_60,                          /* hBackPorch */
        1280,                                       /* width */
        720,                                        /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For 720P50 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_720P_50,                          /* hBackPorch */
        1280,                                       /* width */
        720,                                        /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For 1080P30 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_1080P_30,                         /* hBackPorch */
        1920,                                       /* width */
        1080,                                       /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For XGAP60 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_XGA_60,                           /* hBackPorch */
        1024,                                       /* width */
        768,                                        /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For XGAP75 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_XGA_75,                           /* hBackPorch */
        1024,                                       /* width */
        768,                                        /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For SXGAP60 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_SXGA_60,                          /* hBackPorch */
        1280,                                       /* width */
        1024,                                       /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For SXGAP75 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_SXGA_75,                          /* hBackPorch */
        1280,                                       /* width */
        1024,                                       /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For WUXGAP60 */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_UXGA_60,                          /* hBackPorch */
        1600,                                       /* width */
        1200,                                       /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
    /* For 480P */
    {
        0xFFFFFFFF,                                 /* vencId */
        FALSE,                                      /* hFrontPorch */
        FVID2_STD_480P,                             /* hBackPorch */
        720,                                        /* width */
        480,                                        /* height0 */
        FVID2_SF_PROGRESSIVE,                       /* scanFormat */
    },
};


static Int32 VpsHalHdVencSineTable[31] =
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
static Int32 VpsHalHdVencCosineTable[31] = {
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

