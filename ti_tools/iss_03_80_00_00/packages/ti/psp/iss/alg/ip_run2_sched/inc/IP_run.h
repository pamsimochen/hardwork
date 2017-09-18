/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  VICP Signal Processing Library                                          */
/*                                                                          */
/*  This library contains proprietary intellectual property of Texas        */
/*  Instruments, Inc.  The library and its source code are protected by     */
/*  various copyrights, and portions may also be protected by patents or    */
/*  other legal protections.                                                */
/*                                                                          */
/*  This software is licensed for use with Texas Instruments TMS320         */
/*  family DSPs.  This license was provided to you prior to installing      */
/*  the software.  You may review this license by consulting the file       */
/*  TI_license.PDF which accompanies the files in this library.             */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*                                                                          */
/*     NAME                                                                 */
/*       IP_run.h -- Signal Processing Library Interface header file        */
/*                                                                          */
/*     DESCRIPTION                                                          */
/*        This file includes the definitions and the interfaces required    */
/*        by the internal layer of Library                                  */
/*                                                                          */
/*     REV                                                                  */ 
/*        version 0.0.2:  19 Nov                                            */
/*        Initial version                                                   */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */

#ifndef _IP_RUN_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _IP_RUN_H

#define ERROR_CHAN_ALLOC -1
#define ERROR_CHAN_DEALLOC -2

// If you want to increase number of simcop dma channels
// change below symbols and rebuild IP_RUN library.
#define MAX_NUM_IN_DMA 8 // # simcop input dma channels
#define MAX_NUM_OUT_DMA 8 // # simcop output dma channels

/* Maximum number of input EDMA channels allocatable */
#define IP_RUN_MAX_NUM_DMAIN_CHAN 8
/* Maximum number of output EDMA channels allocatable */
#define IP_RUN_MAX_NUM_DMAOUT_CHAN 8
#define IP_RUN_MAX_NUM_DMAINOUT_CHAN (IP_RUN_MAX_NUM_DMAIN_CHAN + IP_RUN_MAX_NUM_DMAOUT_CHAN)

 
#ifndef _OLD_IPRUN_API

#define IP_RUN_MAGICWORD 0xA1B2C3D4
#define IP_RUN_IMX_0 0
#define IP_RUN_IMX_1 1
#define IP_RUN_IMX_ALL 2 

#define IP_RUN_RESET_INPUT_ADDR  0x1
#define IP_RUN_RESET_OUTPUT_ADDR 0x2

/* Structure used as input argument of IP_RUN_init() */
typedef struct IP_RUN_InitParams{
    char *magicString;          /* Must be set to "IP_run" by application */
    Uint16 versionMajor;        /* Major number of version which the application is compatible with. For example if version is 1.2, set versionMajor to 1.*/
    Uint16 versionMinor;        /* Minor number of version which the application is compatible with. For example if version is 1.2, set versionMinor to 2.*/
    Uint16 staticDmaAlloc;      /* Enable static allocation of EDMA channels and param entries. */
    Uint16 numStaticDmaIn;      /* Number of input EDMA channels that must be statically allocated inside IP_RUN_init() and will be later used. Ignored if staticDmaAlloc= 0.*/
    Uint16 numStaticDmaOut;     /* Number of output EDMA channels that must be statically allocated inside IP_RUN_init() and will be later used. Ignored if staticDmaAlloc= 0. */
    Uint16 numStaticDma;        /* Number of input or output EDMA channels that must be statically allocated inside IP_RUN_init() and will be later used. Ignored if staticDmaAlloc= 0.*/    
    Uint16 initFC;              /* Initialize frame components according to bitmask:
                                   bit #0 ON: initialize CE
                                   bit #1 ON: initialize RMAN
                                   bit #2 ON: register VICP res manager 
                                   bit #3 ON: register EDMA3 res manager
                                   bit #4 ON: register ADDRSPACE res manager
                                */
    char *engineName;        /* Codec engine name, must match name used in vicpLib365.cfg
                                normally, it is "alg_server"  
                                This member is irrelevant if initFC's bit #0 is set to 0   
                             */
    void *codecEngineHandle;   /* Codec engine handle */
    Uint32 sysBaseAddr;        /* Pass In Linux, sysBaseAddr = DM365MM_mmap(0x01C40000,0x4000); On CCS, N/A */ 
} IP_RUN_InitParams;

typedef struct DmaTferStruct_wrapper{

  Uint32 ddrAddr ;      	// starting address in DDR
  Uint32  ddrWidth ;    	// data width in DDR, in bytes
  Int16  reserved0 ;     // not used
  Int32  ddrOfstNextBlock ; // offset to next block, in bytes
  Int32  ddrOfstNextBlockRow ; // offset to next row of blocks, in bytes

  Uint32 imgBufAddr ;     	//starting address in image buffer
  Int16  imgBufWidth ; 		//data width in image buffer, in bytes
						 
  Int16  reserved1 ; // not used

  Uint16 blockWidth ;     // block width in bytes
  Uint16 blockHeight ;    // block height in bytes
  Uint16 useTferParamTable;   /* 1: useTferParamTable, 0: don't use TferParamTable */

  Uint16 dmaChNo;             /* DMA channel number */
  void   *tferParamTable;     /* Pointer to transfer table composed of TferParamEntry, only valid if useTferParamtable =1 */

}  DmaTferStruct_wrapper ;

/* Type TferParamEntry is the type of elements in the table pointed by tferParamTable */
typedef union TferParamEntryUnion {

    struct {
        Uint32 ddrAddr;
        Uint16 blockWidth;
        Uint16 blockHeight;
        Uint32 imgBufAddr;
        Uint16 ddrWidth;
        Uint16 imgBufWidth;
    } in;

    struct {
        Uint32 imgBufAddr;
        Uint16 blockWidth;
        Uint16 blockHeight;
        Uint32 ddrAddr;
        Uint16 imgBufWidth;
        Uint16 ddrWidth;
    } out;

} TferParamEntry;

#define IPRUN_IMX_SINGLE     0 // Single iMX processing
#define IPRUN_IMX_DUAL_CLONE 1 // Dual iMX processing, clone mode
#define IPRUN_IMX_DUAL_SPLIT 2 // Dual iMX processing, split mode

#define IPRUN_CONTEXT_NONE           0
#define IPRUN_CONTEXT_SAVE_CMD0      (1<<1)
#define IPRUN_CONTEXT_SAVE_CMD1      (1<<2)
#define IPRUN_CONTEXT_SAVE_COEF0     (1<<3)
#define IPRUN_CONTEXT_SAVE_COEF1     (1<<4)
#define IPRUN_CONTEXT_SAVE_SEQ       (1<<5)
#define IPRUN_CONTEXT_RESTORE_CMD0   (1<<8)
#define IPRUN_CONTEXT_RESTORE_CMD1   (1<<9)
#define IPRUN_CONTEXT_RESTORE_COEF0  (1<<10)
#define IPRUN_CONTEXT_RESTORE_COEF1  (1<<11)
#define IPRUN_CONTEXT_RESTORE_SEQ    (1<<12)

typedef struct {
  Uint16 action;
  Uint16 cmd0Len;
  Uint16 cmd1Len;
  Uint16 coef0Len;
  Uint16 coef1Len;
  Int16 *cmd0Ptr;
  Int16 *cmd1Ptr;
  Int16 *coef0Ptr;
  Int16 *coef1Ptr;
  Int16 *seqPtr;
  Uint16 coef0Ofst;
  Uint16 coef1Ofst;
} IP_RUN_Context;

typedef struct IP_RUN_extStruct{
  Uint32 magicWord;
  Uint16 iMXid; // IPRUN_IMX_SINGLE, IPRUN_IMX_DUAL_CLONE, IPRUN_IMX_DUAL_SPLIT
  Uint16 async; /* 0: sync execution, 1: async execution */
  Uint16 cmd1ptr_ofst ;          	// iMX cmdptr
  IP_RUN_Context context;
} IP_RUN_extension;


/* Module's object */
typedef struct IP_run{

  // all widths and heights in terms of words
  DmaTferStruct_wrapper *dmaIn;
  Uint16 numDmaIn ;
  DmaTferStruct_wrapper *dmaOut;
  Uint16 numDmaOut ;
  Uint16 numVertBlocks ;
  Uint16 numHorzBlocks;
  Uint32 chunksize ;
  Uint16 compIntEna;
  Uint16 compCode;
  Uint16 cmdptr_ofst ;          	// iMX cmdptr
  void *extension; // for future IPRUN extension
  void *customExtension;       // for customer extension
} IP_run_wrapper;

#ifndef _IP_RUN_DEBUG_DEF
#define _IP_RUN_DEBUG_DEF
/**
 * Structure used as input argument of the debug callback function
 */
typedef struct {

	Int16 blkX; // blkX is the X-axis coordinate of the block just being processed by the iMX
				// the result of the processing sits in image buffer
	Int16 blkY; // blkY is the Y-axis coordinate of the block just being processed
	Int16 numBlocksX; // Total number of blocks in one row
	Int16 numBlocksY; // Total number of rows of blocks.
	Int8 *imBuf0Ptr;   // Pointer to the image buffer where the block of data has just been processed
	Int8 *coefBuf0Ptr; // Pointer to the coefficient memory
    Int8 *imBuf1Ptr;     /* Pointer to the image buffer where the block of data has just been processed by iMX1 */
    Int8 *coefBuf1Ptr;   /* Pointer to the coefficient memory of iMX1 */

} IP_RUN_DebugStruct;
#endif

/**
 * Definition of the debug callback function
 * set by IP_RUN_setDebugCB()
 */
typedef void (*IP_RUN_DebugFunc) (IP_RUN_DebugStruct *debug, void*arg);


typedef struct {
    void *src;
    void *dst;
    Uint16 numCols;
    Uint16 numRows;
    Uint32 strideSrc;
    Uint32 strideDst;
    Uint16 chanNo;
} IP_RUN_Dma2dCopy;


#else

// Below is legacy structure definition used for backward compatibility with old code
typedef struct Sdram_Imgbuf{

  Uint32 loc1_addr ;      	// location one addr --- normally ddram addr
  Int16  loc1_Bindex ;    	// image width
						  	// can also b used for read/writing interleaved
							// data patterns; real_width in this case will be
							// loc1_Cindex
  Int16  loc1_Cindex ; // not used
  Int32  inc0 ; // offset to next block
  Int32  inc1 ; // offset to next row of blocks


  Uint32 loc2_addr ;     	//location 2 addr -- normally inptr in imgbuf or DSP memory
  Int16  loc2_Bindex ; 		//normally dma_realwidth in imgbuf
						  	// can also b used for read/writing interleaved
							// data patterns; real_width in this case will be
							// loc2_Cindex
  Int16  loc2_Cindex ; // not used



  Uint16 dma_width ;     // A CNT
  Uint16 dma_height ;    // B CNT
  Uint16  C_CNT ;

  Uint16 dmaChNo;        // DMA channel number

}  Sdram_Imgbuf ;

/* Module's object */
typedef struct IP_run{

  // all widths and heights in terms of words
  Sdram_Imgbuf *dmaIn;
  Uint16 num_in_comp ;

  Sdram_Imgbuf *dmaOut;
  Uint16 num_out_comp ;

  Uint16 num_vert ;
  Uint16 num_horz ;

  Uint16 x ;
  Uint16 y ;
  Uint32 chunksize ;
  Uint16 compIntEna;
  Uint16 compCode;
  Uint16 cmdptr_ofst ;          	// iMX cmdptr
   



  

} IP_run;

#endif

typedef struct IP_run *IP_run_handle;

/* Declaration of global variable being used as default configuration for IP_RUN_init() */
extern IP_RUN_InitParams IP_RUN_DEFAULT_INIT;

Int32 IP_RUN_init();
Int32 IP_RUN_deInit(IP_RUN_InitParams* init);
Int32 IP_RUN_registerAlgo(IP_run_wrapper * handle, Int32 iMXthreadId);
Int32 IP_RUN_unregisterAlgo(IP_run_wrapper * handle, Int32 iMXthreadId);
Int32 IP_RUN_resetAlgo(IP_run_wrapper * handle, Int32 iMXthreadId, Int32 resetCmd);
void IP_RUN_start(Int32 iMXthreadId);
void IP_RUN_wait();
Int32 IP_RUN_isBusy();
Int32 IP_RUN_getCompCode();
Int32 IP_RUN_setDebugCB(IP_RUN_DebugFunc debugCB, void *arg);
void _IP_RUN_2dCopy(IP_RUN_Dma2dCopy *dma2dCopy);
Int32 IP_RUN_dma2dCopySetup(IP_RUN_Dma2dCopy *dma2dCopy);
Int32 IP_RUN_dma2dCopyStart();
Int32 IP_RUN_dma2dCopyWait();
Int32 IP_RUN_dma2dCopySetupChan(Uint16 chan, IP_RUN_Dma2dCopy *dma2dCopy);
Int32 IP_RUN_dma2dCopyStartChan(Uint16 chan);
Int32 IP_RUN_dma2dCopyWaitChan(Uint16 chan);
Int32 IP_RUN_dma2dUpdateAddrChan(Uint16 chan, IP_RUN_Dma2dCopy *dma2dCopy);

Uint32 IP_RUN_fillInputTferParamEntry(TferParamEntry *tferParamTable, TferParamEntry *entry, Uint16 index);

Uint32 IP_RUN_fillOutputTferParamEntry(TferParamEntry *tferParamTable, TferParamEntry *entry, Uint16 index);

/* Macros used for backward compatibility with earlier version of IP_RUN */
#define IP_runseq_dm350_Init(a) (IP_RUN_init(),IP_RUN_registerAlgo(a, 0))
#define IP_runseq_dm350_Run()  IP_RUN_start(0)
#define IP_runseq_dm350_Wait() IP_RUN_wait()
#define IP_runseq_dm350_DeInit(a) (IP_RUN_unregisterAlgo(a, 0), IP_RUN_deInit())

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif  /* define _IP_RUN_H */

/* ======================================================================== */
/*                       End of file                                        */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2008 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */


