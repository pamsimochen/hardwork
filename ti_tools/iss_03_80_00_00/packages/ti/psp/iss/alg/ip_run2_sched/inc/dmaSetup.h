#ifndef DMA_SETUP_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define DMA_SETUP_H

#include "seq_IP_run.h"

/*-----------------------------------------------------------------*/
/* Macros used to simplify address calculations.                   */
/*-----------------------------------------------------------------*/
#define MSB16(x)     ((Uint16)(((Uint32)(x) & 0xFFFF0000) >> 16))
#define LSB16(x)     ((Uint16)((Uint32)(x) & 0x0000FFFF))
#define ADDR16(x, y) ((Uint16 *)(&(x)) + y)

/****************************************************************************
   Function    : _IP_RUN_initDmaCopyChan()
   Parameters  :                    

   Returns     : -1 on failure, channel number on success
   Description : This function allocates and initializes a edma channel
                 whose functionality is to copy dma settings from structures
                 located in DDR into same structures located in sequencer memory.
                 This is to cope with limited sequencer data memory. Due to the
                 number of dma channels, number of iMX threads, the sequencer data
                 memory cannot store the settings for all the channels. So they are
                 transferred before processsing starts.
****************************************************************************/

Int32 _IP_RUN_initDmaChan(
   dmaBlkSeq_params * dmaInDDR,
   dmaBlkSeq_params * dmaOutDDR,
   dmaBlkSeq_params * dmaInSeq,
   dmaBlkSeq_params * dmaOutSeq);

Int32 _IP_RUN_resetDmaChan();

Int32 _IP_RUN_deInitDmaChan();

Int32 _IP_RUN_chainDma(
 dmaBlk_params    *dmaParams1,
 dmaBlk_params    *dmaParams2,
 Uint16         completionMode
 );

Int32 _IP_RUN_chainDmaFirst(
 dmaBlk_params    *dmaParamsIn,
 dmaBlk_params    *dmaParamsOut,
 Uint16 numBlockX,
 Uint16 numBlockY
 );
    
Int32 _IP_RUN_dmaAlloc(dmaBlk_params * dmaParams);

void _IP_RUN_dmaStart(void);
void _IP_RUN_dmaWait(void);

/****************************************************************************
   Function    : dmaSetup()
   Parameters  : srcAddress        Location of the source image
                 dstAddress        Location of the output image buffer
				 imxInputAddress   Location of the iMX input buffer to copy
				                   each block of data into.
				 imxOutputAddress  Location of the iMX output buffer to copy
				                   each block result from
                 imgWidth          Width of the image.
				 imgHeight         Height of the image.
				 blkWidth          Width of the processing block.
				 blkHeight         Height of the processing block.
				 dmaInParams       Pointer to the params structure used by
				                   the sequencer program for the input DMA.
				 dmaOutParams      Pointer to the params structure used by
				                   the sequencer program for the output DMA.
                 dmaParams         The host program uses this structure to 
                                   store the channel numbers for the input and
                                   output DMA transfers.				                   

   Returns     : -1 on failure, 0 on success
   Description : This function sets up an input and output DMA to copy data
                 from memory to the iMX image buffer and the result back to
				 memory.  The transfer parameters are stored in the 
				 sequencer structure so that the sequencer can initiate the
				 transfers.
****************************************************************************/
Int32 _IP_RUN_dmaSetup(
               DmaTferStruct_wrapper *handle,
               Uint16 direction,
               dmaBlk_params    * dmaParams
               );

Int32 _IP_RUN_dmaSetupParamUpdateChan(
               DmaTferStruct_wrapper *handle,
               dmaBlk_params    *dmaParams,
               dmaBlk_params    *dmaDataTferParams
               );

Int32 _IP_RUN_dmaReuse(dmaBlk_params * dmaParams, dmaBlk_params * dmaExistingParams);

Int32 _IP_RUN_saveDmaSetup(
               Uint16 index,
               Uint16 direction,
               Uint16 iMXthreadId,
               Uint16 imxId,
               dmaBlk_params    *dmaParams
               );

Int32 _IP_RUN_dmaUpdateSrcDst(
               Uint16 index,
               DmaTferStruct_wrapper * restrict handle,
               Uint16 direction,
               Uint16 iMXthreadId,
               Uint16 imxId,
               dmaBlk_params    *dmaParams
               );

Int32 _IP_RUN_dmaResetSrcDst(
               DmaTferStruct_wrapper * restrict handle,
               Uint16 direction,
               dmaBlk_params    *dmaParams
               );

Int32 _IP_RUN_dmaResetParamUpdateChan(
               DmaTferStruct_wrapper *handle,
               dmaBlk_params    *dmaParams,
               dmaBlk_params    *dmaDataTferParams /* pointer to associated data transfer channel's dmaBlk_params */
               );

void _IP_RUN_dmaUpdateParamEntry(dmaBlk_params  *dmaParams);

/****************************************************************************
   Function    : dmaDelete()
   Parameters  : dmaParams         The host program uses this structure to 
                                   store the channel numbers for the input and
                                   output DMA transfers.				                   

   Returns     : NONE
   Description : Uses the DMAC module to free the DMA channels.
****************************************************************************/
Int32 _IP_RUN_dmaDelete(dmaBlk_params * dmaParams);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
