/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_dctrlDefaultsTI8107.h
 *
 * \brief VPS Display Controller header file for default Values
 * This file exposes the HAL APIs of the VPS Display Controller to the other
 * drivers.
 *
 */

#ifndef _VPS_DCTRLDEFAULTSTI8107_H
#define _VPS_DCTRLDEFAULTSTI8107_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Maximum number of nodes including Dummy Nodes */
#define DC_NUM_NODES                        (31u)

/* Maximum number of input nodes */
#define DC_MAX_INPUT                        (8u)

/* Maximum number of output nodes */
#define DC_MAX_OUTPUT                       (VPS_DC_MAX_VENC)

/* Defins Maximum resource DC handles. The resources are: VCOMP Main path,
 * VCOMP Aux path, CIG PIP path and three blenders. */
#define DC_MAX_RESOURCES                    (7u)

#define DC_NODEINFO_DEFAULTS                                                   \
{                                                                              \
    {VPS_DC_NODETYPE_MUX, "PRI_MUX", 0, TRUE, {0, {NULL}}, {0, {NULL}},        \
        NULL, dcMuxSetInput, dcMuxSetOutput, NULL, 0, NULL,                    \
        VPSHAL_VPS_VENC_MUX_PRI, 0u},                                          \
    {VPS_DC_NODETYPE_MUX, "VCOMP_MUX", 1, FALSE, {0, {NULL}}, {0, {NULL}},     \
        NULL, dcMuxSetInput, dcMuxSetOutput, NULL, 0, NULL,                    \
        VPSHAL_VPS_VENC_MUX_VCOMP, 0u},                                        \
    {VPS_DC_NODETYPE_MUX, "HDCOMP_MUX", 2, FALSE, {0, {NULL}}, {0, {NULL}},    \
        NULL, dcMuxSetInput, dcMuxSetOutput, dcMuxSetModeInfo, 0, NULL,        \
        VPSHAL_VPS_VENC_MUX_HDCOMP, 0u},                                       \
    {VPS_DC_NODETYPE_MUX, "SD_MUX", 3, FALSE, {0, {NULL}}, {0, {NULL}},        \
        NULL, dcMuxSetInput, dcMuxSetOutput, dcMuxSetModeInfo, 0, NULL,        \
        VPSHAL_VPS_VENC_MUX_SD, 0u},                                           \
    {VPS_DC_NODETYPE_SPLITTER, "AUX_SPLIT", 4, TRUE, {0, {NULL}},              \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_SPLITTER, "BP0_SPLIT", 5, TRUE, {0, {NULL}},              \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_SPLITTER, "BP1_SPLIT", 6, TRUE, {0, {NULL}},              \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_INPUT, "AUX", 7, TRUE, {0, {NULL}}, {0, {NULL}},          \
        NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                               \
    {VPS_DC_NODETYPE_INPUT, "BP0", 8, TRUE, {0, {NULL}}, {0, {NULL}},          \
        NULL, NULL, NULL, NULL, 0, NULL,                                       \
        VPSHAL_VCOMP_SOURCE_AUX, 0u},                                          \
    {VPS_DC_NODETYPE_INPUT, "BP1", 9, TRUE, {0, {NULL}}, {0, {NULL}},          \
        NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                               \
    {VPS_DC_NODETYPE_INPUT, "SEC1", 10, TRUE, {0, {NULL}}, {0, {NULL}},        \
        NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                               \
    {VPS_DC_NODETYPE_INPUT, "GRPX0", 11, TRUE, {0, {NULL}}, {0, {NULL}},       \
        NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                               \
    {VPS_DC_NODETYPE_INPUT, "GRPX1", 12, TRUE, {0, {NULL}}, {0, {NULL}},       \
        NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                               \
    {VPS_DC_NODETYPE_INPUT, "GRPX2", 13, TRUE, {0, {NULL}}, {0, {NULL}},       \
        NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                               \
    {VPS_DC_NODETYPE_COMP, "VCOMP", 14, FALSE, {0, {NULL}}, {0, {NULL}},       \
        NULL, dcVcompSetInput, dcVcompSetOutput, dcVcompSetModeInfo,           \
        0, NULL, 0, 0u},                                                       \
    {VPS_DC_NODETYPE_SPLITTER, "CIG_1", 15, TRUE, {0, {NULL}}, {0, {NULL}},    \
        NULL, dcCigSetInput, dcCigSetOutput, dcCigSetModeInfo,                 \
        0, NULL, 0, 0u},                                                       \
    {VPS_DC_NODETYPE_SPLITTER, "CIG_2", 16, FALSE, {0, {NULL}}, {0, {NULL}},   \
        NULL, dcCigSetInput, dcCigSetOutput, dcCigSetModeInfo,                 \
        0, NULL, NULL, 0u},                                                    \
    {VPS_DC_NODETYPE_SPLITTER, "CIG_1_SPLITTER", 17, TRUE, {0, {NULL}},        \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_SPLITTER, "CIG_2_SPLITTER", 18, TRUE, {0, {NULL}},        \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_SPLITTER, "GRPX0", 19, TRUE, {0, {NULL}},                 \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_SPLITTER, "GRPX1", 20, TRUE, {0, {NULL}},                 \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_SPLITTER, "GRPX2", 21, TRUE, {0, {NULL}},                 \
        {0, {NULL}}, NULL, NULL, NULL, NULL, 0, NULL, 0, 0u},                  \
    {VPS_DC_NODETYPE_COMP, "HDMI_BLEND", 22, FALSE, {0, {NULL}}, {0, {NULL}},  \
        NULL, dcCompSetInput, dcCompSetOutput, dcCompSetModeInfo,              \
        0, NULL, VPSHAL_COMP_SELECT_HDMI, 0u},                                 \
    {VPS_DC_NODETYPE_COMP, "HDCOMP_BLEND", 23, FALSE, {0, {NULL}}, {0, {NULL}},\
        NULL, dcCompSetInput, dcCompSetOutput, dcCompSetModeInfo,              \
        0, NULL, VPSHAL_COMP_SELECT_DVO2, 0u},                                 \
    {VPS_DC_NODETYPE_COMP, "DVO2_BLEND", 24, FALSE, {0, {NULL}}, {0, {NULL}},  \
        NULL, dcCompSetInput, dcCompSetOutput, dcCompSetModeInfo,              \
        0, NULL, VPSHAL_COMP_SELECT_HDCOMP, 0u},                               \
    {VPS_DC_NODETYPE_COMP, "SD_BLEND", 25, FALSE, {0, {NULL}}, {0, {NULL}},    \
        NULL, dcCompSetInput, dcCompSetOutput, dcCompSetModeInfo,              \
        0, NULL, VPSHAL_COMP_SELECT_SD, 0u},                                   \
    {VPS_DC_NODETYPE_OUTPUT, "HDMI_VENC", 26, TRUE, {0, {NULL}}, {0, {NULL}},  \
        NULL, NULL, NULL, NULL, 0, NULL,                                       \
        VPSHAL_VPDMA_FSEVENT_HDMI_FID, 0u},                                    \
    {VPS_DC_NODETYPE_OUTPUT, "HDCOMP_VENC", 27, TRUE, {0, {NULL}}, {0, {NULL}},\
        NULL, NULL, NULL, NULL, 0, NULL,                                       \
        VPSHAL_VPDMA_FSEVENT_HDCOMP_FID, 0u},                                  \
    {VPS_DC_NODETYPE_OUTPUT, "DVO2_VENC", 28, TRUE, {0, {NULL}}, {0, {NULL}},  \
        NULL, NULL, NULL, NULL, 0, NULL,                                       \
        VPSHAL_VPDMA_FSEVENT_DVO2_FID, 0u},                                    \
    {VPS_DC_NODETYPE_OUTPUT, "SD_VENC", 29, TRUE, {0, {NULL}}, {0, {NULL}},    \
        NULL, NULL, NULL, NULL, 0, NULL,                                       \
        VPSHAL_VPDMA_FSEVENT_SD_FID, 0u},                                      \
    {VPS_DC_NODETYPE_INPUT, "PRI", 30, TRUE, {0, {NULL}}, {0, {NULL}},         \
        NULL, NULL, NULL, NULL, 0, NULL,                                       \
        VPSHAL_VCOMP_SOURCE_MAIN, 0u}                                          \
}

#define DC_EDGEINFO_DEFAULTS                                                   \
{                                                                              \
    {30, 0},                                                                   \
    {0, 14},                                                                   \
    {1, 14},                                                                   \
    {2, 16},                                                                   \
    {3, 25},                                                                   \
    {4, 1},                                                                    \
    {4, 2},                                                                    \
    {4, 3},                                                                    \
    {5, 1},                                                                    \
    {5, 2},                                                                    \
    {5, 3},                                                                    \
    {6, 1},                                                                    \
    {6, 2},                                                                    \
    {6, 3},                                                                    \
    {7, 4},                                                                    \
    {8, 5},                                                                    \
    {9, 6},                                                                    \
    {10, 3},                                                                   \
    {11, 19},                                                                  \
    {12, 20},                                                                  \
    {13, 21},                                                                  \
    {14, 15},                                                                  \
    {15, 17},                                                                  \
    {15, 24},                                                                  \
    {16, 18},                                                                  \
    {17, 22},                                                                  \
    {18, 22},                                                                  \
    {18, 24},                                                                  \
    {19, 22},                                                                  \
    {19, 24},                                                                  \
    {19, 25},                                                                  \
    {20, 22},                                                                  \
    {20, 24},                                                                  \
    {20, 25},                                                                  \
    {21, 22},                                                                  \
    {21, 24},                                                                  \
    {21, 25},                                                                  \
    {22, 26},                                                                  \
    {23, 27},                                                                  \
    {17, 23},                                                                  \
    {18, 23},                                                                  \
    {19, 23},                                                                  \
    {20, 23},                                                                  \
    {21, 23},                                                                  \
    {24, 28},                                                                  \
    {25, 29}                                                                   \
}

#define DC_INPUTNODE_DEFAULTS                                                  \
{                                                                              \
    {&DcNodes[DC_NODE_PRI], FALSE, FALSE, FALSE, NULL, NULL,                   \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_PRI},             \
    {&DcNodes[DC_NODE_AUX], FALSE, FALSE, FALSE, NULL, NULL,                   \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_AUX},             \
    {&DcNodes[DC_NODE_BP0], FALSE, FALSE, FALSE, NULL, NULL,                   \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_BP0},             \
    {&DcNodes[DC_NODE_BP1], FALSE, FALSE, FALSE, NULL, NULL,                   \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_BP1},             \
    {&DcNodes[DC_NODE_SEC1], FALSE, FALSE, FALSE, NULL, NULL,                  \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_SD},              \
    {&DcNodes[DC_NODE_G0], FALSE, FALSE, FALSE, NULL, NULL,                    \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_GRPX0},           \
    {&DcNodes[DC_NODE_G1], FALSE, FALSE, FALSE, NULL, NULL,                    \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_GRPX1},           \
    {&DcNodes[DC_NODE_G2], FALSE, FALSE, FALSE, NULL, NULL,                    \
            {NULL, NULL}, {NULL, NULL}, {0, 0}, NULL, DLM_CT_GRPX2},           \
}

#define DC_OUTPUTNODE_DEFAULTS                                                 \
{                                                                              \
    {&DcNodes[DC_NODE_HDMI_VENC], VPS_DC_VENC_HDMI, NULL,                      \
        {VPS_DC_VENC_HDMI, FALSE, FVID2_STD_1080P_60, 1920, 1080,              \
        FVID2_SF_PROGRESSIVE}, 0,                                              \
        FALSE, VPSHAL_VPS_CLKC_HDMI_DVO1,                                      \
        VpsHal_hdVencGetMode, VpsHal_hdVencSetMode,                            \
        VpsHal_hdvencStartVenc, VpsHal_hdvencStopVenc,                         \
        VpsHal_hdvencIoctl,                                                    \
        0, VPSHAL_VPDMA_CHANNEL_INVALID, FALSE,                                \
        VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO1,                                      \
        VpsHal_hdVencGetOutput, VpsHal_hdVencSetOutput,                        \
        VpsHal_hdvencResetVenc},                                               \
    {&DcNodes[DC_NODE_HDCOMP_VENC], VPS_DC_VENC_HDCOMP, NULL,                  \
        {VPS_DC_VENC_HDCOMP, FALSE, FVID2_STD_1080P_60, 1920, 1080,            \
        FVID2_SF_PROGRESSIVE}, 0,                                              \
        FALSE, VPSHAL_VPS_CLKC_HDCOMP,                                         \
        VpsHal_hdVencGetMode, VpsHal_hdVencSetMode,                            \
        VpsHal_hdvencStartVenc, VpsHal_hdvencStopVenc,                         \
        NULL,                                                                  \
        0, VPSHAL_VPDMA_CHANNEL_INVALID, FALSE,                                \
        VPSHAL_VPS_VENC_OUT_PIX_CLK_INVALID,                                   \
        VpsHal_hdVencGetOutput, VpsHal_hdVencSetOutput,                        \
        VpsHal_hdvencResetVenc},                                               \
    {&DcNodes[DC_NODE_DVO2_VENC], VPS_DC_VENC_DVO2, NULL,                      \
        {VPS_DC_VENC_DVO2, FALSE, FVID2_STD_1080P_60, 1920, 1080,              \
        FVID2_SF_PROGRESSIVE}, 0,                                              \
        FALSE, VPSHAL_VPS_CLKC_DVO2,                                           \
        VpsHal_hdVencGetMode, VpsHal_hdVencSetMode,                            \
        VpsHal_hdvencStartVenc, VpsHal_hdvencStopVenc,                         \
        NULL,                                                                  \
        0, VPSHAL_VPDMA_CHANNEL_INVALID, FALSE,                                \
        VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO2,                                      \
        VpsHal_hdVencGetOutput, VpsHal_hdVencSetOutput,                        \
        VpsHal_hdvencResetVenc},                                               \
    {&DcNodes[DC_NODE_SD_VENC], VPS_DC_VENC_SD, NULL,                          \
        {VPS_DC_VENC_SD, FALSE, FVID2_STD_NTSC, 720, 480,                      \
        FVID2_SF_INTERLACED}, 0,                                               \
        FALSE, VPSHAL_VPS_CLKC_SDVENC,                                         \
        VpsHal_sdVencGetMode, VpsHal_sdVencSetMode,                            \
        VpsHal_sdvencStartVenc, VpsHal_sdvencStopVenc,                         \
        NULL,                                                                  \
        0, VPSHAL_VPDMA_CHANNEL_INVALID, FALSE,                                \
        VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO2,                                      \
        VpsHal_sdvencGetOutput, VpsHal_sdvencSetOutput,                        \
        VpsHal_sdvencResetVenc}                                                \
}

#define DC_PREDEF_CONFIGS                                                      \
{                                                                              \
    {                                                                          \
        VPS_DC_USERSETTINGS,                                                   \
        {                                                                      \
            {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},                         \
            {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},                                  \
            {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},            \
            {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},                        \
            {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},                         \
            {VPS_DC_CIG_PIP_INPUT, VPS_DC_CIG_PIP_OUTPUT},                     \
            {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},                        \
            {VPS_DC_SEC1_INPUT_PATH, VPS_DC_SDVENC_MUX},                       \
            {VPS_DC_SDVENC_MUX, VPS_DC_SDVENC_BLEND}                           \
        },                                                                     \
        9,                                                                     \
        {{{VPS_DC_VENC_HDMI, FALSE, FVID2_STD_1080P_60},                       \
          {VPS_DC_VENC_DVO2, FALSE, FVID2_STD_1080P_60},                       \
          {VPS_DC_VENC_SD, FALSE, FVID2_STD_NTSC}},                            \
           0, 3u}                                                              \
    },                                                                         \
    {                                                                          \
        VPS_DC_USERSETTINGS,                                                   \
        {                                                                      \
            {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},                         \
            {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},                                  \
            {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},            \
            {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},                        \
            {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},                         \
            {VPS_DC_CIG_PIP_INPUT, VPS_DC_CIG_PIP_OUTPUT},                     \
            {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND}                         \
        },                                                                     \
        7,                                                                     \
        {{{VPS_DC_VENC_HDMI, FALSE, FVID2_STD_1080P_60},                       \
          {VPS_DC_VENC_DVO2, FALSE, FVID2_STD_1080P_60}},                      \
           0, 2u}                                                              \
    },                                                                         \
    {                                                                          \
        VPS_DC_USERSETTINGS,                                                   \
        {                                                                      \
            {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},                         \
            {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},                                  \
            {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},            \
            {VPS_DC_SEC1_INPUT_PATH, VPS_DC_SDVENC_MUX},                       \
            {VPS_DC_SDVENC_MUX, VPS_DC_SDVENC_BLEND}                           \
        },                                                                     \
        5,                                                                     \
        {{{VPS_DC_VENC_HDMI, FALSE, FVID2_STD_1080P_60},                       \
          {VPS_DC_VENC_SD, FALSE, FVID2_STD_NTSC}},                            \
           0, 2u}                                                              \
    }                                                                          \
}

#define DC_RES_MGR_DEFAUTS                                                     \
{                                                                              \
    {VRM_RESOURCE_HD0_PATH, FALSE, 0u, {DC_NODE_PRI_MUX, DC_NODE_VCOMP_MUX,    \
                                    DC_NODE_VCOMP, DC_NODE_CIG_0, 17}, 5},     \
    {VRM_RESOURCE_HD1_PATH, FALSE, 0u, {DC_NODE_HDCOMP_MUX,                    \
                                    DC_NODE_CIG_1, 18}, 3},                    \
    {VRM_RESOURCE_SD_PATH, FALSE, 0u, {DC_NODE_SD_MUX}, 1},                    \
    {VRM_RESOURCE_HDMI_BLEND, FALSE, 0u, {DC_NODE_HDMI_BLEND}, 1},             \
    {VRM_RESOURCE_HDCOMP_BLEND, FALSE, 0u, {DC_NODE_HDCOMP_BLEND}, 1},         \
    {VRM_RESOURCE_DVO2_BLEND, FALSE, 0u, {DC_NODE_DVO2_BLEND}, 1},             \
    {VRM_RESOURCE_SD_BLEND, FALSE, 0u, {DC_NODE_SD_BLEND}, 1}                  \
}

#define DC_FREE_CLIENT_NUM_DEFAULTS                                            \
{                                                                              \
    VPSHAL_VPDMA_CHANNEL_WB0,                                                  \
    VPSHAL_VPDMA_CHANNEL_WB1,                                                  \
    VPSHAL_VPDMA_CHANNEL_WB2,                                                  \
    VPSHAL_VPDMA_CHANNEL_VIP1_MULT_ANCB_SRC0,                                  \
}

typedef enum
{
    VPS_DC_SD_CSC = 0,
    VPS_DC_HDCOMP_CSC,
    VPS_DC_VCOMP_CSC,
    VPS_DC_CSC_MAX
} Vps_DcCscIdx;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

#ifdef __cplusplus
}
#endif

#endif

