/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file fvid2_drvmanager.c
 *
 *  \brief VPS FVID2 driver manager layer.
 *  This file implements driver management functionality.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/drivers/vps_init.h>
#ifdef VERSION_TS_ENABLE
#include <ti/psp/vps/test.h>
#endif


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Number of driver object to allocate. */
#define FDM_NUM_DRIVER_OBJECTS          (20u)

/** \brief Number of channel object to allocate. */
#define FDM_NUM_CHANNEL_OBJECTS         (60u)

/** \brief Max version string size */
#define FDM_VERSION_STRING_SIZE         (25u)

/** \brief Number of entries in FVID2 standard info table. */
#define FDM_NUM_STD_INFO_ENTRIES        (sizeof(gFdmStdInfoTable) /            \
                                            sizeof(FVID2_ModeInfo))

/** \brief Number of entries in FVID2 standard info table. */
#define FDM_NUM_DATA_FMT_STR_ENTRIES    (sizeof(gFdmDataFmtStrTable) /         \
                                            sizeof(Fdm_DataFmtString))

/** \brief Number of entries in FVID2 standard info table. */
#define FDM_NUM_STD_STR_ENTRIES         (sizeof(gFdmStdStrTable) /         \
                                            sizeof(Fdm_StdString))


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Fdm_Driver
 *  \brief Structure to store driver information.
 */
typedef struct
{
    const FVID2_DrvOps *drvOps;
    /**< Driver operation table pointer. */
    UInt32              numOpens;
    /**< Number of times the driver is opened using create API. */
    UInt32              isUsed;
    /**< Flag indicating whether the object is used or not. */
} Fdm_Driver;

/**
 *  struct Fdm_Channel
 *  \brief Structure to store channel information.
 */
typedef struct
{
    Fdm_Driver         *drv;
    /**< Pointer to the driver object to which this channel is created. */
    Fdrv_Handle         drvHandle;
    /**< Driver handle returned by the actual driver. */
    FVID2_CbParams      cbParams;
    /**< Application call back parameters. */
    UInt32              isUsed;
    /**< Flag indicating whether the object is used or not. */
} Fdm_Channel;

/**
 *  struct Fdm_DataFmtString
 *  \brief Structure to store data format and string pair.
 */
typedef struct
{
    FVID2_DataFormat    dataFmt;
    /*< Data format. */
    const Char         *dataFmtStr;
    /*< Pointer to data format string. */
} Fdm_DataFmtString;

/**
 *  struct Fdm_StdString
 *  \brief Structure to store standard and string pair.
 */
typedef struct
{
    FVID2_Standard      standard;
    /*< Standard. */
    const Char         *stdStr;
    /*< Pointer to data format string. */
} Fdm_StdString;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 fdmDriverCbFxn(Ptr fdmData, Ptr reserved);
static Int32 fdmDriverErrCbFxn(Ptr fdmData, Void *errList, Ptr reserved);
static Fdm_Driver *fdmAllocDriverObject(void);
static Int32 fdmFreeDriverObject(const FVID2_DrvOps *drvOps);
static Fdm_Channel *fdmAllocChannelObject(void);
static Int32 fdmFreeChannelObject(Fdm_Channel *channel);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief FDM Driver objects. */
static Fdm_Driver gFdmDriverObjects[FDM_NUM_DRIVER_OBJECTS];

/** \brief FDM Channel objects. */
static Fdm_Channel gFdmChannelObjects[FDM_NUM_CHANNEL_OBJECTS];

/** \brief FVID2 drivers version number as string. */
static Char gFdmVersionString[FDM_VERSION_STRING_SIZE] = VPS_VERSION_STRING;

/** \brief FVID2 drivers version number. */
static UInt32 gFdmVersionNumber = VPS_VERSION_NUMBER;

/** \brief FVID2 standard information table. */
static FVID2_ModeInfo gFdmStdInfoTable[] =
{
    /* Standard, FrameWidth, FrameHeight, ScanFormat, PixelClock (KHz), FPS,
       Reserved */
    {FVID2_STD_NTSC,             720u,  480u, FVID2_SF_INTERLACED,   27000u, 30u, 0u},
    {FVID2_STD_PAL,              720u,  576u, FVID2_SF_INTERLACED,   27000u, 25u, 0u},
    {FVID2_STD_480I,             720u,  480u, FVID2_SF_INTERLACED,   27000u, 30u, 0u},
    {FVID2_STD_576I,             720u,  576u, FVID2_SF_INTERLACED,   27000u, 25u, 0u},
    {FVID2_STD_480P,             720u,  480u, FVID2_SF_PROGRESSIVE,  27000u, 60u, 0u},
    {FVID2_STD_576P,             720u,  576u, FVID2_SF_PROGRESSIVE,  27000u, 50u, 0u},
    {FVID2_STD_720P_60,         1280u,  720u, FVID2_SF_PROGRESSIVE,  74250u, 60u, 0u},
    {FVID2_STD_720P_50,         1280u,  720u, FVID2_SF_PROGRESSIVE,  74250u, 50u, 0u},
    {FVID2_STD_1080I_60,        1920u, 1080u, FVID2_SF_INTERLACED,   74250u, 30u, 0u},
    {FVID2_STD_1080I_50,        1920u, 1080u, FVID2_SF_INTERLACED,   74250u, 25u, 0u},
    {FVID2_STD_1080P_60,        1920u, 1080u, FVID2_SF_PROGRESSIVE, 148500u, 60u, 0u},
    {FVID2_STD_1080P_50,        1920u, 1080u, FVID2_SF_PROGRESSIVE, 148500u, 50u, 0u},
    {FVID2_STD_1080P_24,        1920u, 1080u, FVID2_SF_PROGRESSIVE,  74250u, 24u, 0u},
    {FVID2_STD_1080P_30,        1920u, 1080u, FVID2_SF_PROGRESSIVE,  74250u, 30u, 0u},
    {FVID2_STD_VGA_60,           640u,  480u, FVID2_SF_PROGRESSIVE,  25175u, 60u, 0u},
    {FVID2_STD_VGA_72,           640u,  480u, FVID2_SF_PROGRESSIVE,  31500u, 72u, 0u},
    {FVID2_STD_VGA_75,           640u,  480u, FVID2_SF_PROGRESSIVE,  31500u, 75u, 0u},
    {FVID2_STD_VGA_85,           640u,  480u, FVID2_SF_PROGRESSIVE,  36000u, 85u, 0u},
    {FVID2_STD_WVGA_60,          800u,  480u, FVID2_SF_PROGRESSIVE,  33500u, 60u, 0u},
    {FVID2_STD_SVGA_60,          800u,  600u, FVID2_SF_PROGRESSIVE,  40000u, 60u, 0u},
    {FVID2_STD_SVGA_72,          800u,  600u, FVID2_SF_PROGRESSIVE,  50000u, 72u, 0u},
    {FVID2_STD_SVGA_75,          800u,  600u, FVID2_SF_PROGRESSIVE,  49500u, 75u, 0u},
    {FVID2_STD_SVGA_85,          800u,  600u, FVID2_SF_PROGRESSIVE,  56250u, 85u, 0u},
    {FVID2_STD_WSVGA_70,        1024u,  600u, FVID2_SF_PROGRESSIVE,  50800u, 70u, 0u},
    {FVID2_STD_XGA_60,          1024u,  768u, FVID2_SF_PROGRESSIVE,  65000u, 60u, 0u},
    {FVID2_STD_XGA_70,          1024u,  768u, FVID2_SF_PROGRESSIVE,  75000u, 70u, 0u},
    {FVID2_STD_XGA_75,          1024u,  768u, FVID2_SF_PROGRESSIVE,  78750u, 75u, 0u},
    {FVID2_STD_XGA_85,          1024u,  768u, FVID2_SF_PROGRESSIVE,  94500u, 85u, 0u},
    {FVID2_STD_WXGA_60,         1280u,  768u, FVID2_SF_PROGRESSIVE,  68250u, 60u, 0u},
    {FVID2_STD_WXGA_75,         1280u,  768u, FVID2_SF_PROGRESSIVE, 102250u, 75u, 0u},
    {FVID2_STD_WXGA_85,         1280u,  768u, FVID2_SF_PROGRESSIVE, 117500u, 85u, 0u},
    {FVID2_STD_1440_900_60,     1440u,  900u, FVID2_SF_PROGRESSIVE, 106500u, 60u, 0u},
    {FVID2_STD_1368_768_60,     1368u,  768u, FVID2_SF_PROGRESSIVE, 85860u, 60u, 0u},
    {FVID2_STD_1366_768_60,     1366u,  768u, FVID2_SF_PROGRESSIVE, 85500u, 60u, 0u},
    {FVID2_STD_1360_768_60,     1360u,  768u, FVID2_SF_PROGRESSIVE, 85500u, 60u, 0u},
    {FVID2_STD_SXGA_60,         1280u, 1024u, FVID2_SF_PROGRESSIVE, 108000u, 60u, 0u},
    {FVID2_STD_SXGA_75,         1280u, 1024u, FVID2_SF_PROGRESSIVE, 135000u, 75u, 0u},
    {FVID2_STD_SXGA_85,         1280u, 1024u, FVID2_SF_PROGRESSIVE, 157500u, 85u, 0u},
    {FVID2_STD_WSXGAP_60,       1680u, 1050u, FVID2_SF_PROGRESSIVE, 146250u, 60u, 0u},
    {FVID2_STD_SXGAP_60,        1400u, 1050u, FVID2_SF_PROGRESSIVE, 121750u, 60u, 0u},
    {FVID2_STD_SXGAP_75,        1400u, 1050u, FVID2_SF_PROGRESSIVE, 156000u, 75u, 0u},
    {FVID2_STD_UXGA_60,         1600u, 1200u, FVID2_SF_PROGRESSIVE, 162000u, 60u, 0u}
};

/** \brief Data format string table. */
static Fdm_DataFmtString gFdmDataFmtStrTable[] =
{
    {FVID2_DF_YUV422I_UYVY,             "YUV422I_UYVY"              },
    {FVID2_DF_YUV422I_YUYV,             "YUV422I_YUYV"              },
    {FVID2_DF_YUV422I_YVYU,             "YUV422I_YVYU"              },
    {FVID2_DF_YUV422I_VYUY,             "YUV422I_VYUY"              },
    {FVID2_DF_YUV422SP_UV,              "YUV422SP_UV"               },
    {FVID2_DF_YUV422SP_VU,              "YUV422SP_VU"               },
    {FVID2_DF_YUV422P,                  "YUV422P"                   },
    {FVID2_DF_YUV420SP_UV,              "YUV420SP_UV"               },
    {FVID2_DF_YUV420SP_VU,              "YUV420SP_VU"               },
    {FVID2_DF_YUV420P,                  "YUV420P"                   },
    {FVID2_DF_YUV444P,                  "YUV444P"                   },
    {FVID2_DF_YUV444I,                  "YUV444I"                   },
    {FVID2_DF_RGB16_565,                "RGB16_565"                 },
    {FVID2_DF_ARGB16_1555,              "ARGB16_1555"               },
    {FVID2_DF_RGBA16_5551,              "RGBA16_5551"               },
    {FVID2_DF_ARGB16_4444,              "ARGB16_4444"               },
    {FVID2_DF_RGBA16_4444,              "RGBA16_4444"               },
    {FVID2_DF_ARGB24_6666,              "ARGB24_6666"               },
    {FVID2_DF_RGBA24_6666,              "RGBA24_6666"               },
    {FVID2_DF_RGB24_888,                "RGB24_888"                 },
    {FVID2_DF_ARGB32_8888,              "ARGB32_8888"               },
    {FVID2_DF_RGBA32_8888,              "RGBA32_8888"               },
    {FVID2_DF_BGR16_565,                "BGR16_565"                 },
    {FVID2_DF_ABGR16_1555,              "ABGR16_1555"               },
    {FVID2_DF_ABGR16_4444,              "ABGR16_4444"               },
    {FVID2_DF_BGRA16_5551,              "BGRA16_5551"               },
    {FVID2_DF_BGRA16_4444,              "BGRA16_4444"               },
    {FVID2_DF_ABGR24_6666,              "ABGR24_6666"               },
    {FVID2_DF_BGR24_888,                "BGR24_888"                 },
    {FVID2_DF_ABGR32_8888,              "ABGR32_8888"               },
    {FVID2_DF_BGRA24_6666,              "BGRA24_6666"               },
    {FVID2_DF_BGRA32_8888,              "BGRA32_8888"               },
    {FVID2_DF_BITMAP8,                  "BITMAP8"                   },
    {FVID2_DF_BITMAP4_LOWER,            "BITMAP4_LOWER"             },
    {FVID2_DF_BITMAP4_UPPER,            "BITMAP4_UPPER"             },
    {FVID2_DF_BITMAP2_OFFSET0,          "BITMAP2_OFFSET0"           },
    {FVID2_DF_BITMAP2_OFFSET1,          "BITMAP2_OFFSET1"           },
    {FVID2_DF_BITMAP2_OFFSET2,          "BITMAP2_OFFSET2"           },
    {FVID2_DF_BITMAP2_OFFSET3,          "BITMAP2_OFFSET3"           },
    {FVID2_DF_BITMAP1_OFFSET0,          "BITMAP1_OFFSET0"           },
    {FVID2_DF_BITMAP1_OFFSET1,          "BITMAP1_OFFSET1"           },
    {FVID2_DF_BITMAP1_OFFSET2,          "BITMAP1_OFFSET2"           },
    {FVID2_DF_BITMAP1_OFFSET3,          "BITMAP1_OFFSET3"           },
    {FVID2_DF_BITMAP1_OFFSET4,          "BITMAP1_OFFSET4"           },
    {FVID2_DF_BITMAP1_OFFSET5,          "BITMAP1_OFFSET5"           },
    {FVID2_DF_BITMAP1_OFFSET6,          "BITMAP1_OFFSET6"           },
    {FVID2_DF_BITMAP1_OFFSET7,          "BITMAP1_OFFSET7"           },
    {FVID2_DF_BITMAP8_BGRA32,           "BITMAP8_BGRA32"            },
    {FVID2_DF_BITMAP4_BGRA32_LOWER,     "BITMAP4_BGRA32_LOWER"      },
    {FVID2_DF_BITMAP4_BGRA32_UPPER,     "BITMAP4_BGRA32_UPPER"      },
    {FVID2_DF_BITMAP2_BGRA32_OFFSET0,   "BITMAP2_BGRA32_OFFSET0"    },
    {FVID2_DF_BITMAP2_BGRA32_OFFSET1,   "BITMAP2_BGRA32_OFFSET1"    },
    {FVID2_DF_BITMAP2_BGRA32_OFFSET2,   "BITMAP2_BGRA32_OFFSET2"    },
    {FVID2_DF_BITMAP2_BGRA32_OFFSET3,   "BITMAP2_BGRA32_OFFSET3"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET0,   "BITMAP1_BGRA32_OFFSET0"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET1,   "BITMAP1_BGRA32_OFFSET1"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET2,   "BITMAP1_BGRA32_OFFSET2"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET3,   "BITMAP1_BGRA32_OFFSET3"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET4,   "BITMAP1_BGRA32_OFFSET4"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET5,   "BITMAP1_BGRA32_OFFSET5"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET6,   "BITMAP1_BGRA32_OFFSET6"    },
    {FVID2_DF_BITMAP1_BGRA32_OFFSET7,   "BITMAP1_BGRA32_OFFSET7"    },
    {FVID2_DF_BAYER_RAW,                "BAYER_RAW"                 },
    {FVID2_DF_RAW_VBI,                  "RAW_VBI"                   },
    {FVID2_DF_RAW,                      "RAW"                       },
    {FVID2_DF_MISC,                     "MISC"                      },
    {FVID2_DF_INVALID,                  "INVALID"                   },
};

/** \brief Standard string table. */
static Fdm_StdString gFdmStdStrTable[] =
{
    {FVID2_STD_NTSC,                    "NTSC"                      },
    {FVID2_STD_PAL,                     "PAL"                       },
    {FVID2_STD_480I,                    "480I"                      },
    {FVID2_STD_576I,                    "576I"                      },
    {FVID2_STD_CIF,                     "CIF"                       },
    {FVID2_STD_HALF_D1,                 "HALF_D1"                   },
    {FVID2_STD_D1,                      "D1"                        },
    {FVID2_STD_480P,                    "480P"                      },
    {FVID2_STD_576P,                    "576P"                      },
    {FVID2_STD_720P_60,                 "720P60"                    },
    {FVID2_STD_720P_50,                 "720P50"                    },
    {FVID2_STD_1080I_60,                "1080I60"                   },
    {FVID2_STD_1080I_50,                "1080I50"                   },
    {FVID2_STD_1080P_60,                "1080P60"                   },
    {FVID2_STD_1080P_50,                "1080P50"                   },
    {FVID2_STD_1080P_24,                "1080P24"                   },
    {FVID2_STD_1080P_30,                "1080P30"                   },
    {FVID2_STD_VGA_60,                  "VGA60"                     },
    {FVID2_STD_VGA_72,                  "VGA72"                     },
    {FVID2_STD_VGA_75,                  "VGA75"                     },
    {FVID2_STD_VGA_85,                  "VGA85"                     },
    {FVID2_STD_WVGA_60,                 "WVGA60"                    },
    {FVID2_STD_SVGA_60,                 "SVGA60"                    },
    {FVID2_STD_SVGA_72,                 "SVGA72"                    },
    {FVID2_STD_SVGA_75,                 "SVGA75"                    },
    {FVID2_STD_SVGA_85,                 "SVGA85"                    },
    {FVID2_STD_WSVGA_70,                "WSVGA70"                   },
    {FVID2_STD_XGA_60,                  "XGA60"                     },
    {FVID2_STD_XGA_70,                  "XGA70"                     },
    {FVID2_STD_XGA_75,                  "XGA75"                     },
    {FVID2_STD_XGA_85,                  "XGA85"                     },
    {FVID2_STD_WXGA_60,                 "WXGA60"                    },
    {FVID2_STD_WXGA_75,                 "WXGA75"                    },
    {FVID2_STD_WXGA_85,                 "WXGA85"                    },
    {FVID2_STD_1440_900_60,             "1440X900@60"               },
    {FVID2_STD_1368_768_60,             "1368X768@60"               },
    {FVID2_STD_1366_768_60,             "1366X768@60"               },
    {FVID2_STD_1360_768_60,             "1360X768@60"               },
    {FVID2_STD_SXGA_60,                 "SXGA60"                    },
    {FVID2_STD_SXGA_75,                 "SXGA75"                    },
    {FVID2_STD_SXGA_85,                 "SXGA85"                    },
    {FVID2_STD_WSXGAP_60,               "WSXGAP60"                  },
    {FVID2_STD_SXGAP_60,                "SXGAP60"                   },
    {FVID2_STD_SXGAP_75,                "SXGAP75"                   },
    {FVID2_STD_UXGA_60,                 "UXGA60"                    },
    {FVID2_STD_MUX_2CH_D1,              "MUX_2CH_D1"                },
    {FVID2_STD_MUX_2CH_HALF_D1,         "MUX_2CH_HALF_D1"           },
    {FVID2_STD_MUX_2CH_CIF,             "MUX_2CH_CIF"               },
    {FVID2_STD_MUX_4CH_D1,              "MUX_4CH_D1"                },
    {FVID2_STD_MUX_4CH_CIF,             "MUX_4CH_CIF"               },
    {FVID2_STD_MUX_4CH_HALF_D1,         "MUX_4CH_HALF_D1"           },
    {FVID2_STD_MUX_8CH_CIF,             "MUX_8CH_CIF"               },
    {FVID2_STD_MUX_8CH_HALF_D1,         "MUX_8CH_HALF_D1"           },
    {FVID2_STD_AUTO_DETECT,             "AUTO_DETECT"               },
    {FVID2_STD_CUSTOM,                  "CUSTOM"                    },
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  FVID2_init
 *  \brief FVID2 init function.
 *
 *  Initializes the drivers and the hardware.
 *  This function should be called before calling any of driver API's.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 FVID2_init(Ptr args)
{
    UInt32  cnt;
    Int32   retVal;

#ifdef VERSION_TS_ENABLE
    Vps_printf("\n\n");
    Vps_printf("Below is the commit id and short message "
               "from where this code is compiled:\n");
    Vps_printf("%s\n\n", version_id);
#endif

    /* Init all global variables to zero */
    VpsUtils_memset(gFdmDriverObjects, 0u, sizeof (gFdmDriverObjects));
    VpsUtils_memset(gFdmChannelObjects, 0u, sizeof (gFdmChannelObjects));

    /* Mark pool flags as free */
    for (cnt = 0u; cnt < FDM_NUM_DRIVER_OBJECTS; cnt++)
    {
        gFdmDriverObjects[cnt].isUsed = FALSE;
    }
    for (cnt = 0u; cnt < FDM_NUM_CHANNEL_OBJECTS; cnt++)
    {
        gFdmChannelObjects[cnt].isUsed = FALSE;
    }

    retVal = Vps_init(NULL);

    return (retVal);
}



/** FVID2_getVersionString
 *  \brief Get the HDVPSS driver version in string form. This API can be
 *  called before calling FVID2_init.
 *
 * \return          Returns pointer to HDVPSS version string
 */
const Char *FVID2_getVersionString(void)
{
    return (gFdmVersionString);
}



/** FVID2_getVersionNumber
 *  \brief Get the HDVPSS driver version in number form. This API can be
 * called before calling FVID2_init.
 *
 * \return                 HDVPSS version number
 */
UInt32 FVID2_getVersionNumber()
{
    return (gFdmVersionNumber);
}



/**
 *  FVID2_deInit
 *  \brief FVID2 deinit function.
 *
 *  Uninitializes the drivers and the hardware.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 FVID2_deInit(Ptr args)
{
    Int32   retVal;

    retVal = Vps_deInit(NULL);

    return (retVal);
}



/**
 *  FVID2_registerDriver
 *  \brief FVID2 register driver function.
 */
Int32 FVID2_registerDriver(const FVID2_DrvOps *drvOps)
{
    UInt32          cnt;
    UInt32          cookie;
    Int32           retVal = FVID2_SOK;
    Fdm_Driver     *drv;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != drvOps));

    cookie = Hwi_disable();         /* Disable global interrupts */

    /* Check whether the driver is already registered */
    for (cnt = 0u; cnt < FDM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if (TRUE == gFdmDriverObjects[cnt].isUsed)
        {
            /* Check for NULL pointers */
            GT_assert(FdmTrace, (NULL != gFdmDriverObjects[cnt].drvOps));

            if (drvOps->drvId == gFdmDriverObjects[cnt].drvOps->drvId)
            {
                GT_0trace(FdmTrace, GT_ERR,
                    "Driver with same id already registered!!\n");
                retVal = FVID2_EDRIVER_INUSE;
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get a free driver object */
        drv = fdmAllocDriverObject();
        if (NULL != drv)
        {
            drv->drvOps = drvOps;
            drv->numOpens = 0u;
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Alloc driver object failed!!\n");
            retVal = FVID2_EALLOC;
        }
    }

    Hwi_restore(cookie);            /* Restore global interrupts */

    return (retVal);
}



/**
 *  FVID2_unRegisterDriver
 *  \brief FVID2 unregister driver function.
 */
Int32 FVID2_unRegisterDriver(const FVID2_DrvOps *drvOps)
{
    Int32       retVal = FVID2_EALLOC;
    UInt32      cookie;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != drvOps));

    cookie = Hwi_disable();         /* Disable global interrupts */

    /* Free the driver object */
    retVal = fdmFreeDriverObject(drvOps);

    Hwi_restore(cookie);            /* Restore global interrupts */

    return (retVal);
}



/**
 *  FVID2_create
 *  \brief Opens the driver identified by the driver ID.
 */
FVID2_Handle FVID2_create(UInt32 drvId,
                          UInt32 instanceId,
                          Ptr createArgs,
                          Ptr createStatusArgs,
                          const FVID2_CbParams *cbParams)
{
    UInt32              cnt;
    Fdm_Driver         *drv = NULL;
    Fdrv_Handle         drvHandle = NULL;
    Fdm_Channel        *channel = NULL;
    UInt32              cookie;
    FVID2_DrvCbParams   fdmCbParams, *tempCbParams;

    /* Get the matching driver object */
    for (cnt = 0u; cnt < FDM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if (TRUE == gFdmDriverObjects[cnt].isUsed)
        {
            /* Check for NULL pointers */
            GT_assert(FdmTrace, (NULL != gFdmDriverObjects[cnt].drvOps));

            if (drvId == gFdmDriverObjects[cnt].drvOps->drvId)
            {
                drv = &gFdmDriverObjects[cnt];
                /* Allocate channel object */
                channel = fdmAllocChannelObject();
                break;
            }
        }
    }

    if (NULL != channel)
    {
        if (NULL != drv->drvOps->create)
        {
            if (NULL != cbParams)
            {
                if (NULL != cbParams->cbFxn)
                {
                    fdmCbParams.fdmCbFxn = fdmDriverCbFxn;
                }
                else
                {
                    fdmCbParams.fdmCbFxn = NULL;
                }
                if (NULL != cbParams->errCbFxn)
                {
                    fdmCbParams.fdmErrCbFxn = fdmDriverErrCbFxn;
                }
                else
                {
                    fdmCbParams.fdmErrCbFxn = NULL;
                }

                fdmCbParams.errList = cbParams->errList;
                fdmCbParams.fdmData = channel;
                fdmCbParams.reserved = cbParams->reserved;
                tempCbParams = &fdmCbParams;
            }
            else
            {
                tempCbParams = NULL;
            }

            /* Call the driver's create function */
            drvHandle = drv->drvOps->create(
                            drvId,
                            instanceId,
                            createArgs,
                            createStatusArgs,
                            tempCbParams);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
        }

        if (NULL != drvHandle)
        {
            cookie = Hwi_disable();         /* Disable global interrupts */
            drv->numOpens++;
            Hwi_restore(cookie);            /* Restore global interrupts */

            channel->drv = drv;
            channel->drvHandle = drvHandle;
            channel->cbParams.cbFxn = NULL;
            channel->cbParams.errCbFxn = NULL;
            channel->cbParams.errList = NULL;
            channel->cbParams.appData = NULL;
            channel->cbParams.reserved = NULL;
            if (NULL != cbParams)
            {
                channel->cbParams.cbFxn = cbParams->cbFxn;
                channel->cbParams.errCbFxn = cbParams->errCbFxn;
                channel->cbParams.errList = cbParams->errList;
                channel->cbParams.appData = cbParams->appData;
                channel->cbParams.reserved = cbParams->reserved;
            }
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver create failed!!\n");

            /* Free the allocated channel object */
            fdmFreeChannelObject(channel);
            channel = NULL;
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR,
            "EALLOC: Channel object allocation failed!!\n");
    }

    return (channel);
}



/**
 *  FVID2_delete
 *  \brief Application calls FVID2_delete to close the logical channel
 *  associated with FVID2 handle.
 */
Int32 FVID2_delete(FVID2_Handle handle, Ptr deleteArgs)
{
    Int32           retVal = FVID2_SOK;
    Fdm_Channel    *channel;
    UInt32          cookie;

    if (NULL != handle)
    {
        channel = (Fdm_Channel *) handle;

        /* Check for NULL pointers */
        GT_assert(FdmTrace, (NULL != channel->drv));
        GT_assert(FdmTrace, (NULL != channel->drv->drvOps));

        if (NULL != channel->drv->drvOps->delete)
        {
            /* Call the driver's delete function */
            retVal = channel->drv->drvOps->delete(
                         channel->drvHandle,
                         deleteArgs);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
        }

        cookie = Hwi_disable();         /* Disable global interrupts */
        channel->drv->numOpens--;
        Hwi_restore(cookie);            /* Restore global interrupts */

        /* Free the allocated channel object */
        channel->drv = NULL;
        channel->drvHandle = NULL;
        channel->cbParams.cbFxn = NULL;
        channel->cbParams.errCbFxn = NULL;
        channel->cbParams.errList = NULL;
        channel->cbParams.appData = NULL;
        channel->cbParams.reserved = NULL;
        fdmFreeChannelObject(channel);
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  FVID2_control
 *  \brief An application calls FVID2_control to send device-specific control
 *  commands to the video driver.
 */
Int32 FVID2_control(FVID2_Handle handle,
                    UInt32 cmd,
                    Ptr cmdArgs,
                    Ptr cmdStatusArgs)
{
    Int32           retVal = FVID2_SOK;
    Fdm_Channel    *channel;

    if (NULL != handle)
    {
        channel = (Fdm_Channel *) handle;

        /* Check for NULL pointers */
        GT_assert(FdmTrace, (NULL != channel->drv));
        GT_assert(FdmTrace, (NULL != channel->drv->drvOps));

        if (NULL != channel->drv->drvOps->control)
        {
            /* Call the driver's control function */
            retVal = channel->drv->drvOps->control(
                         channel->drvHandle,
                         cmd,
                         cmdArgs,
                         cmdStatusArgs);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  FVID2_queue
 *  \brief An application calls FVID2_queue to submit a video buffer to the
 *  video device driver.
 *  This is used in capture/display drivers.
 */
Int32 FVID2_queue(FVID2_Handle handle,
                  FVID2_FrameList *frameList,
                  UInt32 streamId)
{
    Int32           retVal = FVID2_SOK;
    Fdm_Channel    *channel;

    if (NULL != handle)
    {
        channel = (Fdm_Channel *) handle;

        /* Check for NULL pointers */
        GT_assert(FdmTrace, (NULL != channel->drv));
        GT_assert(FdmTrace, (NULL != channel->drv->drvOps));

        if (NULL != channel->drv->drvOps->queue)
        {
            /* Call the driver's queue function */
            retVal = channel->drv->drvOps->queue(
                         channel->drvHandle,
                         frameList,
                         streamId);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  FVID2_dequeue
 *  \brief An application calls FVID2_dequeue to request the video device
 *  driver to give ownership of a video buffer.
 *  This is used in capture/display drivers.
 */
Int32 FVID2_dequeue(FVID2_Handle handle,
                    FVID2_FrameList *frameList,
                    UInt32 streamId,
                    UInt32 timeout)
{
    Int32           retVal = FVID2_SOK;
    Fdm_Channel    *channel;

    if (NULL != handle)
    {
        channel = (Fdm_Channel *) handle;

        /* Check for NULL pointers */
        GT_assert(FdmTrace, (NULL != channel->drv));
        GT_assert(FdmTrace, (NULL != channel->drv->drvOps));

        if (NULL != channel->drv->drvOps->dequeue)
        {
            /* Call the driver's dequeue function */
            retVal = channel->drv->drvOps->dequeue(
                         channel->drvHandle,
                         frameList,
                         streamId,
                         timeout);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  FVID2_processFrames
 *  \brief An application calls FVID2_processFrames to submit a video buffer
 *  to the video device driver.
 *  This API is very similar to the FVID2_queue API except that this is
 *  used in mem to mem drivers only.
 */
Int32 FVID2_processFrames(FVID2_Handle handle,
                          FVID2_ProcessList *processList)
{
    Int32           retVal = FVID2_SOK;
    Fdm_Channel    *channel;

    if (NULL != handle)
    {
        channel = (Fdm_Channel *) handle;

        /* Check for NULL pointers */
        GT_assert(FdmTrace, (NULL != channel->drv));
        GT_assert(FdmTrace, (NULL != channel->drv->drvOps));

        if (NULL != channel->drv->drvOps->processFrames)
        {
            /* Call the driver's process frame function */
            retVal = channel->drv->drvOps->processFrames(
                         channel->drvHandle,
                         processList);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  FVID2_getProcessedFrames
 *  \brief An application calls FVID2_getProcessedFrames to request the video
 *  device driver to give ownership of a video buffer.
 *  This API is very similar to the FVID2_dequeue API except that this is
 *  used in mem to mem drivers only.
 */
Int32 FVID2_getProcessedFrames(FVID2_Handle handle,
                               FVID2_ProcessList *processList,
                               UInt32 timeout)
{
    Int32           retVal = FVID2_SOK;
    Fdm_Channel    *channel;

    if (NULL != handle)
    {
        channel = (Fdm_Channel *) handle;

        /* Check for NULL pointers */
        GT_assert(FdmTrace, (NULL != channel->drv));
        GT_assert(FdmTrace, (NULL != channel->drv->drvOps));

        if (NULL != channel->drv->drvOps->getProcessedFrames)
        {
            /* Call the driver's get process frame function */
            retVal = channel->drv->drvOps->getProcessedFrames(
                         channel->drvHandle,
                         processList,
                         timeout);
        }
        else
        {
            GT_0trace(FdmTrace, GT_ERR, "Driver Ops not supported!!\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  FVID2_getModeInfo
 *  \brief Function to get the information about various FVID2 modes/standards.
 */
Int32 FVID2_getModeInfo(FVID2_ModeInfo *modeInfo)
{
    Int32               retVal = FVID2_SOK;
    UInt32              entryCnt;
    FVID2_ModeInfo     *matchedEntry = NULL;

    if (NULL != modeInfo)
    {
        for (entryCnt = 0u; entryCnt < FDM_NUM_STD_INFO_ENTRIES; entryCnt++)
        {
            if (gFdmStdInfoTable[entryCnt].standard == modeInfo->standard)
            {
                matchedEntry = &gFdmStdInfoTable[entryCnt];
                break;
            }
        }

        if (NULL == matchedEntry)
        {
            GT_0trace(FdmTrace, GT_ERR, "Unsupported standard!!\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
        else
        {
            VpsUtils_memcpy(modeInfo, matchedEntry, sizeof(FVID2_ModeInfo));
        }
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }

    return (retVal);
}



/**
 *  \brief Function to get the name of the data format in printable string.
 *
 *  \param dataFmt      [IN] Data format to get the name.
 *                           For valid values see #FVID2_DataFormat.
 *
 *  \return Returns a const pointer to the string. If the data format is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *FVID2_getDataFmtString(UInt32 dataFmt)
{
    UInt32      entryCnt;
    const char *dataFmtStr = NULL;

    for (entryCnt = 0u; entryCnt < FDM_NUM_DATA_FMT_STR_ENTRIES; entryCnt++)
    {
        if (gFdmDataFmtStrTable[entryCnt].dataFmt == dataFmt)
        {
            dataFmtStr = gFdmDataFmtStrTable[entryCnt].dataFmtStr;
            break;
        }
    }

    if (NULL == dataFmtStr)
    {
        dataFmtStr = "UNKNOWN";
    }

    return (dataFmtStr);
}



/**
 *  \brief Function to get the name of the standard in printable string.
 *

 *  \param standard     [IN] Standard to get the name.
 *                           For valid values see #FVID2_Standard.
 *
 *  \return Returns a const pointer to the string. If the standard is not
 *  known, then it return the string as "UNKNOWN".
 */
const Char *FVID2_getStandardString(UInt32 standard)
{
    UInt32      entryCnt;
    const char *stdStr = NULL;

    for (entryCnt = 0u; entryCnt < FDM_NUM_STD_STR_ENTRIES; entryCnt++)
    {
        if (gFdmStdStrTable[entryCnt].standard == standard)
        {
            stdStr = gFdmStdStrTable[entryCnt].stdStr;
            break;
        }
    }

    if (NULL == stdStr)
    {
        stdStr = "UNKNOWN";
    }

    return (stdStr);
}



/**
 *  FVID2_checkFrameList
 *  \brief Checks the FVID2 frame list for error and returns appropriate error.
 *  This is used by the drivers and not by the application.
 */
Int32 FVID2_checkFrameList(const FVID2_FrameList *frameList, UInt32 maxFrames)
{
    Int32       retVal = FVID2_SOK;
    UInt32      frmCnt;

    /* Check for NULL pointer */
    if (NULL == frameList)
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Check whether num frames is within range */
        if (frameList->numFrames > maxFrames)
        {
            GT_0trace(FdmTrace, GT_ERR, "Number of frames exceeds max!!\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }

        /* Check whether num frames is within range */
        if (frameList->numFrames > FVID2_MAX_FVID_FRAME_PTR)
        {
            GT_0trace(FdmTrace, GT_ERR,
                "Number of frames exceeds FVID2 max!!\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }

        /* Check whether num frames is zero */
        if (0u == frameList->numFrames)
        {
            GT_0trace(FdmTrace, GT_ERR, "Number of frames is zero!!\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }

        if (FVID2_SOK == retVal)
        {
            /* Check the individual frame pointers */
            for (frmCnt = 0u; frmCnt < frameList->numFrames; frmCnt++)
            {
                /* Check for NULL pointer */
                if (NULL == frameList->frames[frmCnt])
                {
                    GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
                    retVal = FVID2_EBADARGS;
                    break;
                }
            }
        }
    }

    return (retVal);
}



/**
 *  FVID2_checkDqFrameList
 *  \brief Checks the FVID2 frame list of dequeue call for error and returns
 *  appropriate error. For dequeue operation, the frame pointers in the frames
 *  should not be checked as this will be filled by the driver.
 *  This is used by the drivers and not by the application.
 */
Int32 FVID2_checkDqFrameList(const FVID2_FrameList *frameList,
                             UInt32 maxFrames)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointer */
    if (NULL == frameList)
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Check whether max frames is within range */
        if (maxFrames > FVID2_MAX_FVID_FRAME_PTR)
        {
            GT_0trace(FdmTrace, GT_ERR,
                "Number of frames exceeds FVID2 max!!\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    return (retVal);
}



/**
 *  FVID2_checkProcessList
 *  \brief Checks the FVID2 process list for error and returns appropriate
 *  error.
 *  This is used by the drivers and not by the application.
 */
Int32 FVID2_checkProcessList(const FVID2_ProcessList *processList,
                             UInt32 inLists,
                             UInt32 outLists,
                             UInt32 maxFrames,
                             Bool isMosaicMode)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  listCnt;
    UInt32                  numFrames;
    const FVID2_FrameList  *frameList;

    if (NULL == processList)
    {
        GT_0trace(FdmTrace, GT_ERR, "EBADARGS: NULL pointer!!\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        if ((processList->numInLists != inLists) ||
            (processList->numOutLists != outLists))
        {
            GT_0trace(FdmTrace, GT_ERR,
                "Number of in/out list doesn't match with requried count!!\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check the in frame lists */
        for (listCnt = 0u;  listCnt < processList->numInLists; listCnt++)
        {
            retVal = FVID2_checkFrameList(
                         processList->inFrameList[listCnt],
                         maxFrames);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(FdmTrace, GT_ERR,
                    "Invalid in frame list: %d!!\n", listCnt);
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check the out frame lists */
        for (listCnt = 0u; listCnt < processList->numOutLists; listCnt++)
        {
            retVal = FVID2_checkFrameList(
                         processList->outFrameList[listCnt],
                         maxFrames);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(FdmTrace, GT_ERR,
                    "Invalid out frame list: %d!!\n", listCnt);
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check if the number of frames is same across in-framelists */
        numFrames = 0u;
        /* Compare in IN framelist first */
        for (listCnt = 0u;  listCnt < processList->numInLists; listCnt++)
        {
            frameList = processList->inFrameList[listCnt];
            if (0u == numFrames)
            {
                /* If first time, don't compare. Just take the value */
                numFrames = frameList->numFrames;
            }
            else
            {
                if (numFrames != frameList->numFrames)
                {
                    GT_0trace(FdmTrace, GT_ERR,
                        "Num frames don't match across framelist!!\n");
                    retVal = FVID2_EINVALID_PARAMS;
                    break;
                }
            }
        }
        if (FVID2_SOK == retVal)
        {
            if (isMosaicMode == TRUE) {
                /* Reinitialize numFrames to 0 to check the output lists.
                 * The number of frames can be different across input and output
                 * frame lists if isMosaicMode is TRUE.
                 */
                numFrames = 0u;
            }

            /* Compare in OUT framelist */
            for (listCnt = 0u; listCnt < processList->numOutLists; listCnt++)
            {
                frameList = processList->outFrameList[listCnt];
                if (0u == numFrames)
                {
                    /* If first time, don't compare. Just take the value */
                    numFrames = frameList->numFrames;
                }
                else
                {
                    if (numFrames != frameList->numFrames)
                    {
                        GT_0trace(FdmTrace, GT_ERR,
                            "Num frames don't match across framelist!!\n");
                        retVal = FVID2_EINVALID_PARAMS;
                        break;
                    }
                }
            }
        }
    }

    return retVal;
}



/**
 *  FVID2_copyFrameList
 *  \brief Copies the source frame list to the destination frame list.
 *  This also resets the frame pointers from the source frame list.
 *  This is used by the drivers and not by the application.
 */
Void FVID2_copyFrameList(FVID2_FrameList *dest, FVID2_FrameList *src)
{
    UInt32          frmCnt;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != dest));
    GT_assert(FdmTrace, (NULL != src));
    GT_assert(FdmTrace, (src->numFrames < FVID2_MAX_FVID_FRAME_PTR));

    dest->numFrames = src->numFrames;
    dest->perListCfg = src->perListCfg;
    src->perListCfg = NULL;
    dest->drvData = src->drvData;
    dest->reserved = src->reserved;

    /* Copy the individual frames */
    for (frmCnt = 0u; frmCnt < src->numFrames; frmCnt++)
    {
        dest->frames[frmCnt] = src->frames[frmCnt];
        src->frames[frmCnt] = NULL;
    }

    return;
}

/**
 *  FVID2_duplicateFrameList
 *  \brief Duplicate the source frame list to the destination frame list.
 *  This does not reset the frame pointers from the source frame list.
 *  This is used by the drivers and not by the application.
 */
Void FVID2_duplicateFrameList(FVID2_FrameList *dest, FVID2_FrameList *src)
{
    UInt32          frmCnt;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != dest));
    GT_assert(FdmTrace, (NULL != src));
    GT_assert(FdmTrace, (src->numFrames < FVID2_MAX_FVID_FRAME_PTR));

    dest->numFrames = src->numFrames;
    dest->perListCfg = src->perListCfg;
    dest->drvData = src->drvData;
    dest->reserved = src->reserved;

    /* Duplicate the individual frames */
    for (frmCnt = 0u; frmCnt < src->numFrames; frmCnt++)
    {
        dest->frames[frmCnt] = src->frames[frmCnt];
    }

    return;
}


/**
 *  FVID2_copyProcessList
 *  \brief Copies the source process list to the destination process list.
 *  This also resets the frame list pointers from the source process list.
 *  This is used by the drivers and not by the application.
 */
Void FVID2_copyProcessList(FVID2_ProcessList *dest, FVID2_ProcessList *src)
{
    UInt32 listCnt;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != dest));
    GT_assert(FdmTrace, (NULL != src));

    for (listCnt = 0; listCnt < src->numInLists; listCnt++)
    {
        dest->inFrameList[listCnt] = src->inFrameList[listCnt];
        src->inFrameList[listCnt] = NULL;
    }
    for (listCnt = 0; listCnt < src->numOutLists; listCnt++)
    {
        dest->outFrameList[listCnt] = src->outFrameList[listCnt];
        src->outFrameList[listCnt] = NULL;
    }
    dest->numInLists = src->numInLists;
    dest->numOutLists = src->numOutLists;
    dest->drvData = src->drvData;
    dest->reserved = src->reserved;

    return;
}



/**
 *  fdmDriverCbFxn
 *  \brief FVID2 driver manager driver callback function. Whenever the drivers
 *  wants to call the application callback function, this function will be
 *  called by the driver and FDM will in turn call the application callback
 *  function.
 *  This is used by the drivers and not by the application.
 */
static Int32 fdmDriverCbFxn(Ptr fdmData, Ptr reserved)
{
    Int32           retVal;
    Fdm_Channel    *channel;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != fdmData));

    channel = fdmData;
    GT_assert(FdmTrace, (NULL != channel->cbParams.cbFxn));
    retVal = channel->cbParams.cbFxn(channel, channel->cbParams.appData, NULL);

    return (retVal);
}



/**
 *  fdmDriverErrCbFxn
 *  \brief FVID2 driver manager driver error callback function.
 *  Whenever the drivers wants to call the application error callback function,
 *  this function will be called by the driver and FDM will in turn call the
 *  application error callback function.
 *  This is used by the drivers and not by the application.
 */
static Int32 fdmDriverErrCbFxn(Ptr fdmData, Void *errList, Ptr reserved)
{
    Int32           retVal;
    Fdm_Channel    *channel;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != fdmData));

    channel = fdmData;
    GT_assert(FdmTrace, (NULL != channel->cbParams.errCbFxn));
    retVal = channel->cbParams.errCbFxn(
                 channel,
                 channel->cbParams.appData,
                 errList,
                 NULL);

    return (retVal);
}



/**
 *  fdmAllocDriverObject
 *  \brief Allocate memory for driver object from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Fdm_Driver *fdmAllocDriverObject(void)
{
    UInt32          cnt;
    Fdm_Driver     *drv = NULL;

    /* Get a free driver object */
    for (cnt = 0u; cnt < FDM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if (FALSE == gFdmDriverObjects[cnt].isUsed)
        {
            drv = &gFdmDriverObjects[cnt];
            drv->isUsed = TRUE;
            break;
        }
    }

    return (drv);
}



/**
 *  fdmFreeDriverObject
 *  \brief Free-up the memory allocated for driver object.
 */
static Int32 fdmFreeDriverObject(const FVID2_DrvOps *drvOps)
{
    UInt32      cnt;
    Int32       retVal = FVID2_EFAIL;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != drvOps));

    /* Free the driver object */
    for (cnt = 0u; cnt < FDM_NUM_DRIVER_OBJECTS; cnt++)
    {
        if ((TRUE == gFdmDriverObjects[cnt].isUsed) &&
            (drvOps == gFdmDriverObjects[cnt].drvOps))
        {
            if (0u == gFdmDriverObjects[cnt].numOpens)
            {
                gFdmDriverObjects[cnt].isUsed = FALSE;
                gFdmDriverObjects[cnt].drvOps = NULL;
                gFdmDriverObjects[cnt].numOpens = 0u;
                retVal = FVID2_SOK;
            }
            else
            {
                GT_0trace(FdmTrace, GT_ERR, "Driver in use!!\n");
                retVal = FVID2_EDEVICE_INUSE;
            }
            break;
        }
    }

    if (FVID2_EFAIL == retVal)
    {
        GT_0trace(FdmTrace, GT_ERR, "Driver ops not found!!\n");
    }

    return (retVal);
}



/**
 *  fdmAllocChannelObject
 *  \brief Allocate memory for channel object from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Fdm_Channel *fdmAllocChannelObject(void)
{
    UInt32          cnt;
    Fdm_Channel    *channel = NULL;
    UInt32          cookie;

    cookie = Hwi_disable();         /* Disable global interrupts */

    /* Get a free channel object */
    for (cnt = 0u; cnt < FDM_NUM_CHANNEL_OBJECTS; cnt++)
    {
        if (FALSE == gFdmChannelObjects[cnt].isUsed)
        {
            channel = &gFdmChannelObjects[cnt];
            channel->isUsed = TRUE;
            break;
        }
    }

    Hwi_restore(cookie);            /* Restore global interrupts */

    return (channel);
}



/**
 *  fdmFreeChannelObject
 *  \brief Free-up the memory allocated for channel object.
 */
static Int32 fdmFreeChannelObject(Fdm_Channel *channel)
{
    Int32       retVal = FVID2_EALLOC;
    UInt32      cookie;

    /* Check for NULL pointers */
    GT_assert(FdmTrace, (NULL != channel));

    cookie = Hwi_disable();         /* Disable global interrupts */

    /* Free the channel object */
    if (TRUE == channel->isUsed)
    {
        channel->isUsed = FALSE;
        retVal = FVID2_SOK;
    }
    else
    {
        GT_0trace(FdmTrace, GT_ERR,
            "Freeing a channel object not in use!!\n");
    }

    Hwi_restore(cookie);            /* Restore global interrupts */

    return (retVal);
}
