/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
 *  \file vps_dctrlPriv.h
 *
 *  \brief VPS Display Controller private header file.
 *  This file contains the internal data structures and functions used
 *  by the display controller driver.
 *
 */

#ifndef _VPS_DCTRLI_H
#define _VPS_DCTRLI_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/drivers/display/src/vps_dctrlDefaultsPriv.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Maximum number of input/output paths a node can have */
#define DC_MAX_PATHS                        (5u)

/* This macro will be stored in the private member of the CIG nodes to keep
 * track of whether interlacing is enabled or not */
#define DC_INTERLACING_ENABLED              (0x1u)

/* Following macros define size of the register overlays for shadowed,
 * nonshadowed and for runtime configuration. These macros are mainly
 * used for creating memory pool for these overlays. */
/* Overlay size for VCOMP, Blender and CIG */
#define DC_SHADOW_REG_OVLY_SIZE             (8u * (4u + 2u + 2u) * 4u)

/* Overlay size for Multiplexer Register */
#define DC_NONSHADOW_REG_OVLY_SIZE          (8u * (4u + 4u) * 4u)

/* Overlay size for VCOMP and CIG registers */
#define DC_RUNTIME_CONFIG_OVLY_SIZE         (((4u + 4u) + (4u + 4u)) * 4u *    \
                                             DC_NUM_DESC_SET *                 \
                                             DC_RT_CONFIG_NUM_IDX)

/* Configuration and Reload descriptor size in terms of words */
#define DC_DESC_MEM_SIZE                (2 * (VPSHAL_VPDMA_CTRL_DESC_SIZE +    \
                                              VPSHAL_VPDMA_CONFIG_DESC_SIZE) * \
                                         DC_NUM_DESC_SET)

/* Configuration and Reload descriptor size in terms of words */
#define DC_RT_DESC_MEM_SIZE             ((VPSHAL_VPDMA_CTRL_DESC_SIZE +        \
                                          VPSHAL_VPDMA_CONFIG_DESC_SIZE) *     \
                                         DC_NUM_DESC_SET * DC_RT_CONFIG_NUM_IDX)

#define DC_SEM_TIMEOUT                      (BIOS_WAIT_FOREVER)

/* Forward declaration */
typedef struct Dc_MemInfo_t Dc_MemInfo;

/* Function pointer for enabling/disabling input from the given node */
typedef Int32 (*DcSetIoFxnPtr) (Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo);
/* Function pointer for setting mode information */
typedef Int32 (*DcSetModeInfo) (UInt32 nodeNum, Vps_DcModeInfo *modeInfo);
/* Function pointer for the Venc Functions */
typedef Int32 (*DcGetModeFromVenc) (
                VpsHal_Handle handle,
                Vps_DcModeInfo *modeInfo);
typedef Int32 (*DcSetModeToVenc) (
                VpsHal_Handle handle,
                Vps_DcModeInfo *modeInfo);
typedef Void (*DcGetOutputFrmVenc) (
                VpsHal_Handle handle,
                Vps_DcOutputInfo *outInfo);
typedef Int32 (*DcSetOutputToVenc) (
                VpsHal_Handle handle,
                Vps_DcOutputInfo *outInfo);
/* Function Pointer to start the Venc */
typedef Int32 (*DcStartVenc) (VpsHal_Handle handle);
/* Function Pointer to stop the Venc */
typedef Int32 (*DcStopVenc) (VpsHal_Handle handle);
/* Function Pointer to start the Venc */
typedef Int32 (*DcResetVenc) (VpsHal_Handle handle);
/* Function Pointer to control the Venc */
typedef Int32 (*DcIoctlVenc)(VpsHal_Handle  handle,
                             UInt32         cmd,
                             Ptr            cmdArgs,
                             Ptr            cmdStatusArgs);
/**
 * enum Dc_RtNodeIdx
 * \brief Enum for indexes of the paths supporting runtime configuration.
 *  Display controller supports runtime configuration in three inputs
 *  paths i.e. VCOMP Main path, VCOMP Aux path and CIG PIP path. This enum
 *  defines indexes for these three paths. Display Controller contains DLM
 *  handles, overlay pointer and overlay size in its structure for these three
 *  paths. This enums are used as indexes into those arrays.
 */
typedef enum Dc_RtNodeIdx_t
{
    DC_RT_CONFIG_VCOMP_MAIN_IDX = 0,
    /**< Index for the VCOMP Main path runtime configuration */
    DC_RT_CONFIG_VCOMP_AUX_IDX,
    /**< Index for the VCOMP Aux path runtime configuration */
    DC_RT_CONFIG_CIG_PIP_IDX,
    /**< Index for the CIG PIP path runtime configuration */
    DC_RT_CONFIG_NUM_IDX
    /**< Last Enum */
} Dc_RtNodeIdx;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* Forward declaration of the nodeinfo structure */
struct Dc_NodeInfo_t;

/* Typedef for Dc_NodeInfo_t */
typedef struct Dc_NodeInfo_t Dc_NodeInfo;

/* Forward declaration of the display controller global information structure */
struct Dc_DisplayCtrlInfo_t;

/* Typedef for Dc_DisplayCtrlInfo_t */
typedef struct Dc_DisplayCtrlInfo_t Dc_DisplayCtrlInfo;

/**
 * struct Dc_NodeSet_t
 * \brief Structure containing pointers to the input/output nodes for a
 *  particular node and flags to indicate whether input/output is enabled or
 *  not. Display controller driver maintains a set of trees describing how
 *  VPS nodes are connected to one another. Each node has a set of possible
 *  inputs and a set of possible outputs. These inputs and outputs are nothing
 *  but the other VPS nodes This way, DC driver creates a tree of nodes from input
 *  node to leaf nodes. This tree has all possible inputs i.e. if mux has three
 *  possible inputs, the node for this mux will have pointer to three input
 *  nodes. Out of the possible nodes, which node is connected actually as
 *  input/output is determined the flag for that input/output. If the flag is
 *  set, input/output is connected. This structure maintains set of pointers
 *  for input/output node and set of flags for these nodes.
 */
typedef struct Dc_NodeSet_t
{
    UInt32         numNodes;
    /**< Number of input/output nodes */
    Dc_NodeInfo   *node[DC_MAX_PATHS];
    /**< Pointer to the input/output node */
    UInt32         isEnabled[DC_MAX_PATHS];
    /**< Flag to indicate whether input/output is enabled or not. */
} Dc_NodeSet;

/**
 * struct Dc_NodeInfo
 * \brief Structure containing node information.
 *  All vps modules handled by the display controller are represented as the
 *  nodes. All input paths and output vencs are also represented as the nodes.
 *  There are also some dummy nodes created to represent VPS mesh easily.
 *  This structure contains set of properties of the given node. It also has
 *  set of function pointers to enable/disable input or output and to set mode
 *  information in VPS module represented by this node.
 */
struct Dc_NodeInfo_t
{
    UInt32          nodeType;
    /**< Type of the node i.e. Compositor, multiplexer, splitter, input or
         output. Type of the node also tells whether this node can have multiple
         inputs or outputs. For valid values, see #Vps_DcNodeType*/
    Char            nodeName[VPS_DC_MAX_NODE_NAME];
    /**< Name describing this node */
    UInt32          nodeNum;
    /**< Each node is given a unique number for identification */
    UInt32          isDummy;
    /**< Flag to indicate whether this is dummy node or not */
    Dc_NodeSet      input;
    /**< Structure containing set of input nodes for given node and which of
         these are actually enabled */
    Dc_NodeSet      output;
    /**< Structure containing set of output nodes for given node and which of
         these are actually enabled */
    UInt32         *ovlyPtr;
    /**< Display controller driver uses overlay for enabling or for disabling
         input/output path for the node. This overlay pointer points to memory,
         within the display controller's overlay, where overlay for this node
         is available. This overlay pointer is passed to appropriate hal layer
         to enable/disable input/output in the overlay */
    DcSetIoFxnPtr  setInput;
    /**< Function pointer pointing to the function to enable or disable
         input for this node. It can be null to indicate there is no way or
         no need to enable/disable input since it is always enabled */
    DcSetIoFxnPtr  setOutput;
    /**< Function pointer pointing to the function to enable or disable
         output for this node. It can be null to indicate there is no way or
         no need to enable/disable output since it is always enabled */
    DcSetModeInfo   setModeInfo;
    /**< Function pointer pointing to the function to set the mode information
         in the VPS module represented by this node. This function mainly sets
         the size of the frame in the VPS modules. Also this will only set
         size dependent on the VENC mode. */
    UInt32          ovlySize;
    /**< Size of the overlay for this node */
    Ptr             parent;
    /**< For input and output nodes, this pointer points to the input and
         output info structure. For other nodes, this is set to null. This is
         used just to get associated input and output info structure. */
    Int32           priv;
    /**< Each node can store some private data in this pointer */
    UInt32          numEnabledInputs;
    /**< For the nodes having node type as Compositor, this keeps track of the
         number of enabled inputs. */
};

/**
 * struct Dc_InputNodeInfo_t
 * \brief Structure containing information about the input nodes.
 */
typedef struct Dc_InputNodeInfo_t
{
    Dc_NodeInfo        *node;
    /**< Pointer to the actual node in the mesh for this input node */
    UInt32              isStarted;
    /**< Flag indicates whether path connected to this input node is started
         or not. This is mainly used for error checking to know whether
         input driver is streaming or not */
    UInt32              isSwitched;
    /**< Flag indicates whether path connected to this input node is switched
         or not. */
    UInt32              isRegistered;
    /**< Flag indicates whether any input path is registered for this input
         node or not. If registered, inputinfo structure should have
         information about the registered client. This flag is also used for
         error checking to know whether client is registered or not */
    Dc_ClientInfo      *clientInfo;
    /**< Pointer to client information structure containing client information.
         This pointer will be null if client is not registered. */
    VpsDlm_Handle       dlmHandle;
    /**< Dlm handle for this client. When client registers to the display
         controller, display controller registers this client to the DLM. This
         handle points to the DLM handle for this client. This handle will
         be used to start/stop this client in the DLM. It will also be used
         create or remove descriptors for this client */
    VpsDlm_Handle       rtDlmHandle[DC_RTCONFIGMODULE_MAX];
    /**< DLM Handle for Runtime configuration. If this input path is connected
         to either CIG or VCOMP, it can support runtime changing of pip
         position and size and/or cropping. For runtime changes in VCOMP and
         CIG, display controller registers clients to the DLM and assign handle
         to this array if this input node is connected to VCOMP or CIG. */
    Ptr                 rtConfigOvlyPtr[DC_RTCONFIGMODULE_MAX][DC_NUM_DESC_SET];
    /**< Pointer to the overlay for runtime configuration. For runtime
         changes in VCOMP and CIG, display controller registers clients to
         the DLM and keeps the configuration overlay memory. This pointer
         points to the overlay memory for runtime configuration. */
    UInt32              rtConfigOvlySize[DC_RTCONFIGMODULE_MAX];
    /**< Size of the overlay for runtime configuration. */
    Dc_DisplayCtrlInfo *dcCtrlInfo;
    /**< Pointer the display controller information */
    VpsDlm_ClientType   dlmClientType;
    /**< DLM Client type for this input node. */
    UInt32              numDlmStartClient;
    /**< Indicates the number dlm clients, which are added along with
         this input path, when starting display. */
} Dc_InputNodeInfo;

/**
 * struct Dc_OutputNodeInfo_t
 * \brief structure containing information about the output nodes.
 */
typedef struct Dc_OutputNodeInfo_t
{
    Dc_NodeInfo            *node;
    /**< Pointer to the nodeinfo structure associated with this output node */
    UInt32                  nodeId;
    /**< Each output node is given a ID. This id identifies venc attached to
         the Venc connected to this output. A bit is reserved for each venc
         in 32 bit word. */
    VpsHal_Handle           vencHandle;
    /**< Handle to Venc associated with this output node */
    Vps_DcModeInfo          modeInfo;
    /**< Local structure keeps track of the mode information set to the venc,
         associated to this outputnode */
    UInt8                   listNum;
    /**< VPDMA List Number */
    UInt32                  isListAllocated;
    /**< Flag indicating List allocated to this output node or not */
    VpsHal_VpsClkcModule    clkcModule;
    /**< Venc should be enabled in the CLKC module when it is started. This
         keeps track of the value to passed to the VPS HAl for enabling
         this venc in the CLKC module */
    DcGetModeFromVenc       getMode;
    /**< Function to get the mode information from the VENC HAL*/
    DcSetModeToVenc         setMode;
    /**< Function to set the mode in the VENC HAL*/
    DcStartVenc             startVenc;
    /**< Pointer to the function to start the Venc */
    DcStopVenc              stopVenc;
    /**< Pointer to the function to stop the Venc */
    DcIoctlVenc             controlVenc;
    /**< Pointer to the function to control the venc */
    UInt32                  lmFidNum;
    /**< LM Fid Number used for this output node */
    VpsHal_VpdmaChannel     freeClientNum;
    /**< Free Write Client used for this output node */
    UInt32                  isVencStarted;
    /**< Flag to indicate venc is running or not */
    VpsHal_VpsVencOutPixClk vencOutPixClk;
    /**< Venc Output Pixel Clock Source. Used to enable output
         clock for the venc associated with this venc. */
    DcGetOutputFrmVenc      getOutput;
    /**< Function to get the output information from Venc */
    DcSetOutputToVenc       setOutput;
    /**< Function to set the output information in Venc */
    DcResetVenc             resetVenc;
    /**< Function Pointer to reset the VENC */
    Vps_DcVencClkSrcSel     vencClkSrc;
    /**< Clock source for this venc */
} Dc_OutputNodeInfo;

/**
 * struct Dc_DisplayCtrlInfo_t
 * \brief Global structure to keep track of all global information.
 */
struct Dc_DisplayCtrlInfo_t
{
    Vps_DcConfig        *predefConfig;
    /**< Standard UseCase specific Configuration */
    UInt32              tiedVencs;
    /**< Keeps track of which vencs are tied */
    Dc_InitParams       initParams;
    /**< Init Parameters containing Hal handles*/
    VpsDlm_Handle       dlmClientCtrlHandle;
    /**< DLM Handle for adding display driver client to the DLM */
    VpsDlm_Handle       rtDlmHandle[DC_RT_CONFIG_NUM_IDX];
    /**< DLM Handle for the runtime configuration. When
         an input path is connected through VCOMP or CIG, it can cropped and/or
         positioned at appropriate location on the output frame, so display
         controller registers an addition DLM client for VCOMP main and aux
         input and CIG pip input for runtime configuration. This handle
         points to the DLM handle for the runtime configuration. */
    Ptr                 ovlyPtr[DC_RT_CONFIG_NUM_IDX][DC_NUM_DESC_SET];
    /**< Pointer to the overlay memory for the runtime configuration. When
         an input path is connected through VCOMP or CIG, it can cropped and/or
         positioned at appropriate location on the output frame, so display
         controller registers an addition DLM client for VCOMP main and aux
         input and CIG pip input for runtime configuration. This
         overlay pointer points to the memory location, where run time
         configuration can be specified. */
    UInt32              ovlySize[DC_RT_CONFIG_NUM_IDX];
    /**< Size of the overlay memory for the runtime configuration */

    Semaphore_Handle    sem;
    /**< Semaphore to protect multiple access to all user and drive
         accessible APIs */
    Semaphore_Handle    isrWaitSem;
    /**< Semaphore used to signal from ISR that clients are added or removed. */
    Void               *vemHandle;
    /**< Event Manager callback handle for Display Controller */
    UInt32              openCnt;
    /**< Open Count, Keeps track of number of open */
    Vps_CscConfig       cscCfg[VPS_DC_CSC_MAX];
} ;

struct Dc_MemInfo_t {
    Ptr     shadowRegOvlyMem;
    Ptr     nonShadowRegOvlyMem;
    UInt32  shadowConfigOvlySize;
    UInt32  nonShadowConfigOvlySize;
} ;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 * \brief Function to get the display controller global object.
 *
 * \return Pointer to display controller object
 */
Dc_DisplayCtrlInfo *dcGetDctrlObj();

/**
 * \brief Function to get the pointer InputNodeInfo structure given the
 *        number of the node
 *
 * \param cnt     [IN] Node Number
 *
 * \return pointer InputNodeInfo structure or null
 */
Dc_InputNodeInfo *dcGetInputNode(UInt32 cnt);

/**
 * \brief Function to get the pointer OutputNodeInfo structure given the
 *        number of the node
 *
 * \param cnt     [IN] Node Number
 *
 * \return pointer OutputNodeInfo structure or null
 */
Dc_OutputNodeInfo  *dcGetOutputNode(UInt32 cnt);

/**
 * \brief Function to get the pointer NodeInfo structure given the
 *        number of the node
 *
 * \param cnt     [IN] Node Number
 *
 * \return pointer NodeInfo structure or null
 */
Dc_NodeInfo        *dcGetNodeInfo(UInt32 cnt);

Int32 dcSetModeInfo(UInt32 startNode,
                    Vps_DcModeInfo *modeInfo,
                    UInt32 isForward);

Int32 dcIntInit(Dc_DisplayCtrlInfo *dctrl);

Int32 dcIntDeInit(Dc_DisplayCtrlInfo *dctrl);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_DCTRLI_H */

