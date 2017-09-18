/*!
 *  @file dmac.h
 *   Contain DMAC module API definitions for DaVinci
 *
 *           Copyright (C) 2004 Texas Instruments Incorporated.
 *                           All Rights Reserved
 *
 *  @date    6/29/2004
 *  @version 1.0
 *
 *
 */

#ifndef _DMAC_DAV_H
#define _DMAC_DAV_H

#include <xdc/std.h>
//#include <_csl_resource.h>
#include <csl_edma.h>
//#include <_csl_edma.h>

#define MAX_DMAC_CHAN (MAX_QDMAC_CHAN + MAX_EDMA_CHAN + 4)
#define MAX_QDMAC_CHAN (DMAC_QDMACHAN_END-DMAC_QDMACHAN_START+1)
#define MAX_EDMA_CHAN (DMAC_EDMACHAN_END-DMAC_EDMACHAN_START+1)

#define DMAC_PARAM_START 96
#define DMAC_PARAM_END   127
#define DMAC_PARAM_ANY (DMAC_PARAM_END+1)

#define DMAC_EDMACHAN_START CSL_EDMA_GPINT4
#define DMAC_EDMACHAN_END   CSL_EDMA_CHA63
#define DMAC_CHAN_ANY (MAX_DMAC_CHAN+1)

#define DMAC_QDMACHAN_START CSL_EDMA_QCHA4
#define DMAC_QDMACHAN_END CSL_EDMA_QCHA7

#define DMAC_BYTEWORD_ACCESS 1  /* on Davinci DMA can access byte resolution */

/** Error symbol returned by API
 */
#define DMAC_ERROR -1 /** error */

/** Symbols that defines channel numbers linked to
    coprocessor related events: ASQINT, DSQINT, IMXINT, VCDINT
 */
#define DMAC_IMXINT  (CSL_EDMA_CHA8 |0x8000)
#define DMAC_VLCDINT (CSL_EDMA_CHA9 |0x8000)
#define DMAC_ASQINT  (CSL_EDMA_CHA10|0x8000)
#define DMAC_DSQINT  (CSL_EDMA_CHA11|0x8000)

/** Enumeration for trigger word possibilities
 *  for QDMA channels.
 *
 */
typedef enum {
    DMAC_QDMA_TRIGWORD_OPT          = 0,
    DMAC_QDMA_TRIGWORD_SRC          = 1,
    DMAC_QDMA_TRIGWORD_ABCNT        = 2,
    DMAC_QDMA_TRIGWORD_DST          = 3,
    DMAC_QDMA_TRIGWORD_BIDX         = 4,
    DMAC_QDMA_TRIGWORD_BCNTRLD_LINK = 5,
    DMAC_QDMA_TRIGWORD_CIDX         = 6,
    DMAC_QDMA_TRIGWORD_CCNT         = 7
} DMAC_QdmaTrigWord;

/** Internal globla structure
 * to store the gvariables
 * returned by CSL EDMA functions
 */

typedef struct{
    CSL_EdmaHandle hModule;
    CSL_EdmaContext context;
    CSL_EdmaRegionObj  region;
    CSL_EdmaRegionHandle hRegion;
    Uint16 edmaChan[MAX_EDMA_CHAN];
} DMAC_Vars;

/** Internal structure
 * used by DMAC module to store information for each channel
 */
typedef struct{
    CSL_EdmaChanObj chanObj;
    CSL_EdmaParamHandle parmH;
    Int16 tcc;
    Uint16 ccnt;
    Uint16 chId;
} DMAC_Chan;

/** Structure used to configure DMA controller
 * Not used anymore by present software.
 * Left in the code for backward compatibility
 */
typedef struct{

    Uint16 sdram_emif;  // 0 -> sdram
    Uint16 int_en;      // 0 -> disable
    Uint16 imx_low_hi;  // 0 -> low
    Uint16 seq_low_hi;
    Uint16 swapRead;
    Uint16 swapWrite;
    Uint16 byteToWord;
    Uint16 shift;

} DMAC_Config;

/** Global variable set to default DMAC configuration
 * Not used anymore by present software.
 * Left in the code for backward compatibility
 */
extern DMAC_Config DMAC_CONFIG_DEFAULT;
/** Global array used to store channel information
 */
extern DMAC_Chan DMAC_channel[MAX_DMAC_CHAN];
/** Global variable used to store CSL edma handle
 */
extern DMAC_Vars DMAC_vars;

/** Function that configures DMA controller
 * Not used anymore by present software.
 * Left in the code for backward compatibility
 */
void DMAC_Dmaconfig(DMAC_Config *config);
/* Macro because in jpeg encoder, DMAC_config() is used instead of DMAC_Dmaconfig() */
#define DMAC_config(config) DMAC_Dmaconfig(config)

/** Clear a channel's pending interrupt bit.
 * This function is usually called after a pending interrupt is detected
 * by the DSP.
 *
 * @param chan channel number
 */
static inline void DMAC_clearChanInt(Uint16 chan){
    Uint32 intrCl[2]={0,0};
    Uint16 tcc= DMAC_channel[chan].tcc;

    intrCl[tcc >> 5]= 1 << (tcc -  32*(tcc>>5));
    CSL_edmaHwRegionControl(DMAC_vars.hRegion, CSL_EDMA_CMD_REGION_INTERRUPT_CLEAR, intrCl);
}

/** Enable DMA channel.
 * This function set the QEER/EER register to enable the appropriate DMA
 * channel.
 *
 * @param chan channel number
 */
static inline void DMAC_enableChan(Uint16 chan)
{
    /* Is this EDMA or QDMA? */
    if (DMAC_channel[chan].chanObj.chaNum>=CSL_EDMA_QCHA0)
    {
        DMAC_vars.hModule->QEESR =
                (1 << (DMAC_channel[chan].chanObj.chaNum - CSL_EDMA_QCHA0));
    }
    else
    {
        if (DMAC_channel[chan].chanObj.chaNum>=32)
            DMAC_vars.hModule->EESRH =
                    (1 << (DMAC_channel[chan].chanObj.chaNum - 32));
        else
            DMAC_vars.hModule->EESR =
                    (1 << DMAC_channel[chan].chanObj.chaNum);
    }
}

/** Disable DMA channel.
 * This function clears the QEER/EER register to disable the appropriate DMA
 * channel.
 *
 * @param chan channel number
 */
static inline void DMAC_disableChan(Uint16 chan)
{
    /* Is this EDMA or QDMA? */
    if (DMAC_channel[chan].chanObj.chaNum>=CSL_EDMA_QCHA0)
    {
        DMAC_vars.hModule->QEECR =
                (1 << (DMAC_channel[chan].chanObj.chaNum - CSL_EDMA_QCHA0));
    }
    else
    {
        if (DMAC_channel[chan].chanObj.chaNum>=32)
            DMAC_vars.hModule->EECRH =
                    (1 << (DMAC_channel[chan].chanObj.chaNum - 32));
        else
            DMAC_vars.hModule->EECR =
                    (1 << DMAC_channel[chan].chanObj.chaNum);
    }
}


/** Setup trigger word on QDMA channel
 * This function sets the trigger word in QCHMAP
 *
 * @param chan channel number
 */
static inline void DMAC_qdmaSetTrigWord(Uint16 chan, DMAC_QdmaTrigWord trig)
{
    /* Is this EDMA or QDMA? */
    if (DMAC_channel[chan].chanObj.chaNum >= CSL_EDMA_QCHA0)
    {
        DMAC_vars.hModule->QCHMAP[DMAC_channel[chan].chanObj.chaNum - CSL_EDMA_QCHA0] &=
                ~CSL_EDMACC_QCHMAP_TRWORD_MASK;
        DMAC_vars.hModule->QCHMAP[DMAC_channel[chan].chanObj.chaNum - CSL_EDMA_QCHA0] |=
                (trig << CSL_EDMACC_QCHMAP_TRWORD_SHIFT) & CSL_EDMACC_QCHMAP_TRWORD_MASK;
    }
}


/** Start transfer of corresponding Edma channel.
 * This function set the event set register
 * associated to the channel. This has as effect to start the Edma.
 *
 * @param chan channel number
 */
static inline void DMAC_startEChan(Uint16 chan){
#if 1
    if (DMAC_channel[chan].chanObj.chaNum>=32)
        DMAC_channel[chan].chanObj.ccregs->SHADOW[1].ESRH = (1 << (DMAC_channel[chan].chanObj.chaNum - 32));
    else
        DMAC_channel[chan].chanObj.ccregs->SHADOW[1].ESR = (1 << DMAC_channel[chan].chanObj.chaNum);
#else
    if (DMAC_channel[chan].chanObj.chaNum>=32)
        DMAC_vars.hModule->ESRH = (1 << (DMAC_channel[chan].chanObj.chaNum - 32));
    else
        DMAC_vars.hModule->ESR = (1 << DMAC_channel[chan].chanObj.chaNum);
#endif
}


/** Start transfer of corresponding Qdma channel.
 * This function writes 1 to the trigger word of the PaRAM entry
 * associated to the channel. This has as effect to start the Qdma.
 *
 * @param chan channel number
 */
static inline void DMAC_startQChan(Uint16 chan){

    DMAC_channel[chan].parmH->CCNT = DMAC_channel[chan].ccnt;
}

/** Start transfer of corresponding Qdma or Edma channel.
 * This function first determines if the channel is Qdma or Edma
 * and call the corresponding DMAC_startQChan or DMAC_startEChan
 * @param chan channel number
 */
static inline void DMAC_startChan(Uint16 chan){

    if (DMAC_channel[chan].chanObj.chaNum>=CSL_EDMA_QCHA0)
        DMAC_startQChan(chan);
    else
        DMAC_startEChan(chan);
}

/** Start transfer of channel 0.
 * This function writes 1 to the trigger word of the PaRAM entry
 * associated to the channel 0. This has as effect to start the Qdma.
 * Channel 0 is the channel associated to function DMAC_sdramRd(), DMAC_sdramWr()
 *
 * @param chan channel number
 */
static inline void DMAC_start(){
    DMAC_clearChanInt(0);
    if (DMAC_channel[0].chanObj.chaNum>=CSL_EDMA_QCHA0)
        DMAC_startQChan(0);
    else
        DMAC_startEChan(0);

}

/** Check if corresponding channel is still busy.
 * This function uses CSL function to request interrupt status bit of
 * corresponding channel.
 *
 * @param chan channel number
 */
static inline Int16 DMAC_busyChan(Uint16 chan){
    Uint32 intrQuery[2];
    Uint16 tcc= DMAC_channel[chan].tcc;

    CSL_edmaGetHwRegionStatus(DMAC_vars.hRegion,CSL_EDMA_QUERY_REGION_INTERRUPT_PENDSTATUS,intrQuery);
    return !(intrQuery[tcc>>5] & (1 << (tcc -  32*(tcc>>5)) ));
}

/** Check if channel 0 is still busy.
 * This function uses CSL function to request interrupt status bit of
 * channel 0. Channel 0 is the channel associated to function DMAC_sdramRd(),
 * DMAC_sdramWr().
 *
 * @param chan channel number
 */
static inline DMAC_busy(){
    DMAC_busyChan(0);
}

/** Wait until corresponding channel completes transfer
 *
 * @param chan channel number
 */
static inline void DMAC_waitChan(Uint16 chan){
    while(DMAC_busyChan(chan));
}

/** Wait until channel 0 completes transfer
 *
 * @param chan channel number
 */
static inline void DMAC_wait(){
    DMAC_waitChan(0);
    DMAC_clearChanInt(0);
}

void DMAC_reset();

/* Enable/Disable DMA Interrupt */
/* DMA needs to have interrupt enabled if to be used with sequencer */
static inline void DMAC_intr(Uint16 enable){
    //  IMGBUF_FSET (DMA_CTRL, DMAINT, enable);
}

/** Initialize DMAC module
 *  This function must be called before using any DMAC_<..> API
 *  This function does the following:
 * - initialize the EDMA module
 * - Open the unique instance CSL_EDMA_0 of the module
 * - Allocate one channel that will be used by DMAC_sdramRd()/DMAC_sdramWr()
 *
 * @return DMAC_ERROR(-1) if allocation of channel failed
 *
 * <b> Usage Constraints: </b>
 * CSL system initialization must be successfully completed by invoking
 * @a CSL_sysInit() before calling this function.
 */
Int16 DMAC_init();

/** Block allocation of specific physical EDMA channels
 *  @ param chanNum pointer to an array containing the physical channel nubmers to be blocked
 *  @ param numChan size of the array. Must be <= 8.
 *  @ return DMAC_error if numChan > 8
 */
Int16 DMAC_blockChanAlloc(Uint8 *chanNum, Uint8 numChan);

/** Reset main registers of EDMA3
 */
void DMAC_reset();

/** Allocate a DMAC channel
 *
 *  This function uses CSL functions related to EDMA (3PCC) to do:
 *  - Open the next available Qdma channel
 *  - Acquire a PaRAM entry
 *  - Configure the field OPT of the PaRAM entry so the dma transfers are 3-D
 *  - Get a transfer completion code (tcc)
 *  - Enable corresponding interrupt and the channel
 *
 * @param chan channel number to be allocated.
 *        Must be between 1 and MAX_DMAC_CHAN-1
 *        Channel 1 to CSL_EDMA_NUM_QDMACH-1 are QDMA channels, the rest is EDMA
 * @return DMAC_ERROR(-1) if allocation error. Allocation errors can have one
 *         of the causes:
 *            - input parameter 'chan' was out of range
 *            - channel number already taken
 *            - no physical QDMA channel available in the 3rd party channel
 *              controller
 *            - could not allocate PaRAM memory
 *            - could not get a tcc
 *         If no error then returns the channel number
 * @see DMAC_initChannel()
 * @see DMAC_freeChannel()
 *
 */
Int16 DMAC_allocChannel(Uint16 chan);

/** Free a DMAC channel
 *
 *  This function uses CSL functions related to EDMA (3PCC) to do:
 *  - Release the PaRAM entry associated to the dma channel
 *  - Release the tcc code
 *  - Close the channel
 *
 * @param chan channel number to be freed. Must be between 1 and 7
 * @return DMAC_ERROR(-1) if allocation error. Allocation errors can have one
 *         of the causes:
 *            - input parameter 'chan' was out of range
 *            - channel number is already free
 *         If no error then returns the channel number
 * @see DMAC_initChannel()
 * @see DMAC_allocChannel()
 */
Int16 DMAC_freeChannel(Uint16 chan);

/** Allocate a param entry
 *
 *
 * @param param: pram number to be allocated.
 *        Must be between DMAC_PARAM_START and DMAC_PARAM_END included
 *        Or can be set to value DMAC_PARAM_ANY
 * @return DMAC_ERROR(-1) if allocation error. Allocation errors can have one
 *         of the causes:
 *            - input parameter 'param' was out of range
 *            - param already taken
 *         If no error` then returns the param number
 * @see DMAC_freeParamEntry()
 *
 */
Int16 DMAC_allocParamEntry(Uint16 param);

/** Convert an param entry's address to its index in the param memory
 */
static inline Uint16 DMAC_getParamEntryNum(void *entryAddr){
    Uint16 entry= (((Uint32)entryAddr<<18)>>23);
    if (entry > 127)
        return 0;
    return entry;

}

/** Copy one param entry onto another
 *
 */
Int16 DMAC_copyParamEntry(Uint16 dstEntry, Uint16 srcEntry);

/** Free a param entry
 *
 *  This function releases a param entry previously allocated
 *  with DMAC_allocParamEntry()
 *
 * @param chan channel number to be freed. Must be between 1 and 7
 * @return DMAC_ERROR(-1) if allocation error param entry already free.
 * @see DMAC_allocParamEntry()
 *
 */
Int16 DMAC_freeParamEntry(Uint16 entry);

/** Initialize a param entry with the parameters of the desired
 *  transfer.
 *
 *  This function takes the param number and parameters of a
 *  DMA transfer, including completion options, and fills out
 *  the param entry.  This function supports the three-dimension
 *  transfer of the TPCC module.
 *
 * @param paramEntry           Number of param entry to be filled out.
 * @param srcAddress           Location of source data.
 * @param dstAddress           Location of destination buffer.
 * @param arraySize            Size of the first transfer dimension, in bytes.
 * @param arrayCount           Size of the second transfer dimension.
 * @param frameCount           Size of the third transfer dimension.
 * @param inputArrayUpdate     Source address update after each array transfer.
 * @param outputArrayUpdate    Destination address update after each array
 *                             transfer.
 * @param inputFrameUpdate     Source address update after each frame transfer.
 * @param outputFrameUpdate    Destination address update after each frame
 *                             transfer.
 * @param syncType             CSL_EDMA_SYNC_ARRAY - A-sync transfer type
 *                             CSL_EDMA_SYNC_ARRAY - AB-sync transfer type
 * @param txfrCompleteCode     Completion code used to indicate transfer
 *                             completion.
 * @param txfrCompleteMode     CSL_EDMA_TCC_NORMAL or CSL_EDMA_TCC_EARLY
 * @param txfrCompleteChain    Upon completion of transfer, this is used to
 *                             trigger another transfer.  The completion code
 *                             indicates the channel to chain.
 * @param txfrCompleteIntr     Upon completion of transfer, this is used to
 *                             set an interrupt on the channel indicated by the
 *                             completion code.
 * @param intTxfrCompleteChain Enables chaining on an intermediate transfer
 *                             completion.  For example, after each array in
 *                             an A-sync transfer or after each frame in an
 *                             AB-sync transfer.
 * @param intTxfrCompleteIntr  Enables an interrupt signal on an intermediate
 *                             transfer completion.
 *
 */
void DMAC_initParamEntry(Uint16            paramEntry,
        Int8            * srcAddress,
        Int8            * dstAddress,
        Uint16            arraySize,
        Uint16            arrayCount,
        Uint16            frameCount,
        Int16            inputArrayUpdate,
        Int16            outputArrayUpdate,
        Int16            inputFrameUpdate,
        Int16            outputFrameUpdate,
        Bool              staticEntry,
        CSL_EdmaSyncType  syncType,
        Uint8             txfrCompleteCode,
        Bool              tccCompleteMode,
        Bool              txfrCompleteChain,
        Bool              txfrCompleteIntr,
        Bool              intTxfrCompleteChain,
        Bool              intTxfrCompleteIntr);


/** Initialize a DMAC channel with 3-D transfer parameters
 *
 * @param chanIdx channel number to initialize. Must be between 1 and 7
 * @param x_num number of bytes per row (1st dimension)
 * @params y_num number of rows (2nd dimension)
 * @params z_num number of 2-D blocks
 * @param srcPtr pointer to source
 * @param srcLineOffset number of bytes separating beginning of each row
 *                      in the source
 * @param srcBlockOffset number of bytes separating beginning of each 2-d block
 *                       in the source
 * @param dstPtr pointer to destination
 * @param dstLineOffset number of bytes separating beginning of each row
 *                      in the destination
 * @param dstBlockOffset number of bytes separating beginning of each 2-d block
 *                       in the destination
 * @see DMAC_allocChannel()
 */
void DMAC_initChan(Uint16 chanIdx, Uint16 x_num, Uint16 y_num, Uint16 z_num,\
        Uint32 srcPtr, Int16 srcLineOffset, Int16 srcBlockOffset,\
        Uint32 dstPtr, Int16 dstLineOffset, Int16 dstBlockOffset);

/** Setup in advance a read transfer from SDRAM/DDR to IMGBUF
 * Setup in advance the transfer's parameters
 * So just need to call DMAC_start() next to fire the transfer
 *
 * @param sdramPtr points to source sdram/ddr memory
 * @param imgPtr points to destination image buffer
 * @param sdramOfst offset in 16-bits words between rows, in sdram
 * @param imgOfst offset in 16-bits words between rows, in imgbuf
 * @param x number of 16-bits words per row
 * @param y number of lines
 * @see DMAC_start()
 * @see DMAC_wait()
 */
static inline DMAC_sdramRdSetup(
        Uint32 sdramPtr, Uint16 *imgPtr, \
        Uint16 sdramOfst, Uint16 imgOfst, Uint16 x, Uint16 y){

    // Set transfer parameters for channel 0
    // Multiply by 2 the numbers of elements per row and offsets
    // because EDMA expects elements are bytes
    DMAC_initChan(0, 2*x, y, 1, sdramPtr, 2*sdramOfst, 0, (Uint32)imgPtr, 2*imgOfst, 0);

}

/** Setup, start, wait for completion of read transfer from SDRAM/DDR to IMGBUF
 * Setup and start dma transfer, wait for completion
 *
 * @param sdramPtr points to source sdram/ddr memory
 * @param imgPtr points to destination image buffer
 * @param sdramOfst offset in 16-bits words between rows, in sdram
 * @param imgOfst offset in 16-bits words between rows, in imgbuf
 * @param x number of 16-bits words per row
 * @param y number of lines
 */
static inline DMAC_sdramRdAsync(
        Uint32 sdramPtr, Uint16 *imgPtr, \
        Uint16 sdramOfst, Uint16 imgOfst, Uint16 x, Uint16 y){

    // Set transfer parameters for channel 0
    DMAC_sdramRdSetup(sdramPtr, imgPtr, sdramOfst, imgOfst, x, y);
    // Clear channel interrupt pending bit
    DMAC_clearChanInt(0);
    // run dma
    DMAC_start();

}

/** Setup and start a read transfer from SDRAM/DDR to IMGBUF
 * Setup and start dma transfer, doesn't wait for completion
 *
 * @param sdramPtr points to source sdram/ddr memory
 * @param imgPtr points to destination image buffer
 * @param sdramOfst offset in 16-bits words between rows, in sdram
 * @param imgOfst offset in 16-bits words between rows, in imgbuf
 * @param x number of 16-bits words per row
 * @param y number of lines
 * @see DMAC_wait()
 */
static inline DMAC_sdramRd(
        Uint32 sdramPtr, Uint16 *imgPtr, \
        Uint16 sdramOfst, Uint16 imgOfst, Uint16 x, Uint16 y){

    DMAC_sdramRdAsync(sdramPtr, imgPtr, sdramOfst, imgOfst, x, y);
    DMAC_wait();

}

/** Setup in advance a write transfer from IMGBUF to SDRAM/DDR
 * Setup in advance the transfer's parameters
 * So just need to call DMAC_start() next to fire the transfer
 *
 * @param imgPtr points to source image buffer
 * @param sdramPtr points to destination sdram/ddr memory
 * @param sdramOfst offset in 16-bits words between rows, in sdram
 * @param imgOfst offset in 16-bits words between rows, in imgbuf
 * @param x number of 16-bits words per row
 * @param y number of lines
 * @see DMAC_start()
 * @see DMAC_wait()
 */
static inline DMAC_sdramWrSetup(
        Uint16 *imgPtr, Uint32 sdramPtr, \
        Uint16 sdramOfst, Uint16 imgOfst, Uint16 x, Uint16 y){

    // Set transfer parameters for channel 0
    // Multiply by 2 the numbers of elements per row and offsets
    // because EDMA expects elements are bytes
    DMAC_initChan(0, 2*x, y, 1, (Uint32)imgPtr, 2*imgOfst, 0, sdramPtr, 2*sdramOfst, 0);

}

/** Setup and start a write transfer from IMGBUF to SDRAM/DDR
 * Setup and start dma transfer, doesn't wait for completion.
 *
 * @param imgPtr points to source image buffer
 * @param sdramPtr points to destination sdram/ddr memory
 * @param sdramOfst offset in 16-bits words between rows, in sdram
 * @param imgOfst offset in 16-bits words between rows, in imgbuf
 * @param x number of 16-bits words per row
 * @param y number of lines
 * @see DMAC_wait()
 */
static inline DMAC_sdramWrAsync(
        Uint16 *imgPtr, Uint32 sdramPtr, \
        Uint16 sdramOfst, Uint16 imgOfst, Uint16 x, Uint16 y){

    // Set transfer parameters for channel 0
    DMAC_sdramWrSetup(imgPtr, sdramPtr, sdramOfst, imgOfst, x, y);
    // Clear channel interrupt pending bit
    DMAC_clearChanInt(0);
    // run dma
    DMAC_start();

}

/** Setup,start, wait for completion of write transfer from IMGBUF to SDRAM/DDR
 * Setup and start dma transfer, wait for completion.
 *
 * @param imgPtr points to source image buffer
 * @param sdramPtr points to destination sdram/ddr memory
 * @param sdramOfst offset in 16-bits words between rows, in sdram
 * @param imgOfst offset in 16-bits words between rows, in imgbuf
 * @param x number of 16-bits words per row
 * @param y number of lines
 */
static inline DMAC_sdramWr(
        Uint16 *imgPtr, Uint32 sdramPtr, \
        Uint16 sdramOfst, Uint16 imgOfst, Uint16 x, Uint16 y){

    DMAC_sdramWrAsync(imgPtr, sdramPtr, sdramOfst, imgOfst, x, y);
    DMAC_wait();

}


void DMAC_link(Uint16 param1, Uint16 param2);

/** Exit DMAC module
 * To be called once program doesn't need to call DMAC_<...> functions
 * anymore.
 */
void DMAC_exit(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  // #ifndef _DMAC320_H
