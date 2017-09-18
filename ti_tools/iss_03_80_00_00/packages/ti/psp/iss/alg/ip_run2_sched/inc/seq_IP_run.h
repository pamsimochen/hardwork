#ifndef _SEQ_IP_RUN_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SEQ_IP_RUN_H

#include <xdc/std.h>
#include "IP_run.h"

typedef struct{
  Uint16 enable;
  Uint16 regularTferFlag;
  Uint32 dmaRowInc;
  Uint32 dmaPtrImgAddr1;
  Uint32 dmaPtrImgAddr2;
  Uint32 dmaPtrImgAddr;
  Uint32 dmaPtr2Addr1;
  Uint32 dmaPtr2Addr2;
  Uint32 dmaPtr2Addr;
  Uint32 dmaAddr;
  Uint32 dmaInc;
  Uint16 dmaImgAddrL;
} dmaBlkSeq_params;

typedef struct{

  Int16 chan;
  Uint16 param;
  Uint16 paramLnk1;
  Uint16 paramLnk2;
  Uint16 direction; // 0= in, 1= out
  Uint16 paramTferFlag;
  dmaBlkSeq_params *seqParams;
} dmaBlk_params;

typedef struct{
 Uint16 countThread; // keep count of thread using this channel
 Uint16 lastThreadId; // id of last thread which used it
} UsedChan;

typedef struct NumBlocks{
 Uint16 X;
 Uint16 Y;
} numBlocks;

typedef struct IP_RUN_Global{
    char version[10];
    Uint16 versionMajor;
    Uint16 versionMinor;
    Uint16 staticDmaAlloc;
    Uint16 numStaticDmaIn;
    Uint16 numStaticDmaOut; 
} IP_RUN_Global;

//Int32 _IP_RUN_processExtension(IP_run *handle, Int32 iMXthreadId);

typedef struct{
 Uint16 runningFlag;
 Uint16 debugFlag;  
 Uint16 MCUcountH;
 Uint16 MCUcountL;
 Uint16 numInChain;
 Uint16 compIntEna;
 Uint16 compCode;
 Uint16 compCodeList;
 Uint16 blkX;
 Uint16 numBlocksX;
 Uint16 blkY;
 Uint16 numBlocksY;
 struct NumBlocks numBlocks;
 Uint16 numDmaIn;
 Uint16 numDmaOut;
 Uint16 iMXid;
} ipSeqRun_params;  

typedef struct{
  Uint16 enable;
  Uint16 cmdStart;
  Uint16 cmdLen; 
} imxIPipe_params;

typedef struct{
  Uint32 dmaPtr2TrigAddr;
  Uint32 dmaPtr2ICRAddr;
  Uint32 copyPtr2TrigAddr;
  Uint32 copyPtr2ICRAddr;
  Uint16 dmaTCCval;
  Uint16 copyTCCval;
} Dma_params;


#ifndef FILL_CMD_BUFF

extern Uint16  _iPRun;
extern dmaBlkSeq_params  _dmaIn_ARG;
extern dmaBlkSeq_params  _dmaOut_ARG;
extern dmaBlkSeq_params  _dmaCopy_ARG;
extern ipSeqRun_params _ipSeqRun_ARG;
extern imxIPipe_params _imxIPipe_ARG;
extern Dma_params _dma_ARG;


#else

/* Error, shouldn't reach here */

#define IPRUN (SEQMEM_BASE + 0x0982)

#define DMAIN_ARG (SEQDATA_BASE + 0x0)
#define DMAOUT_ARG (SEQDATA_BASE + 0x00A8)
#define DMACOPY_ARG (SEQDATA_BASE + 0x0150)
#define DMA_ARG (SEQDATA_BASE + 0x01D8)
#define IMXIPIPE_ARG (SEQDATA_BASE + 0x017A)
#define IPSEQRUN_ARG (SEQDATA_BASE + 0x0188)

extern volatile imxIPipe_params * imxIPipe_ARG;
extern volatile ipSeqRun_params * ipSeqRun_ARG;
extern volatile Dma_params *dma_ARG;

#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
