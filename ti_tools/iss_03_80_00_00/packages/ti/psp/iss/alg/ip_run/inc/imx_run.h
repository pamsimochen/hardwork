/* ====================================================================
 *   Copyright (C) 2010 Texas Instruments Incorporated
 *
 *   All rights reserved. Property of Texas Instruments Incorporated.
 *   Restricted rights to use, duplicate or disclose this code are
 *   granted through contract.
 *
 *   The program may not be used without the written permission
 *   of Texas Instruments Incorporated or against the terms and conditions
 *   stipulated in the agreement under which this program has been
 *   supplied.
 * ==================================================================== */

/*!
 *****************************************************************************
 * \file
 *    imx_run.h
 *
 * \brief
 *  TODO: Add description
 *
 * \version 1.0
 *
 *****************************************************************************
 */
#include "ip_run.h"

#ifndef _IMX_RUN_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define _IMX_RUN_H

#define ERROR_CHAN_ALLOC -1
#define ERROR_CHAN_DEALLOC -2


#define IMX_RUN_MAX_NUM_DMAIN_CHAN 8
#define IMX_RUN_MAX_NUM_DMAOUT_CHAN 8
#define IMX_RUN_MAX_NUM_DMA_CHAN 8

//typedef TIMM_OSAL_S8 IP_RUN_RET_TYPE;

#ifndef _OLD_IPRUN_API

#define IPRUN_IMX_SINGLE     0 // Single iMXA processing
#define IPRUN_IMX_DUAL_CLONE 1 // Dual iMX processing, clone mode
#define IPRUN_IMX_DUAL_SPLIT 2 // Dual iMX processing, split mode
#define IPRUN_IMX_SINGLE_B     3 // Single iMX -- iMXB processing


        typedef struct IMX_RUN_Context {
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
            void  *internal;
            void  *dmaregs;
        } IMX_RUN_Context;

        typedef struct IMX_RUN_extStruct {
            Uint32          magicWord;
            Uint16          iMXid; // IPRUN_IMX_SINGLE, IPRUN_IMX_DUAL_CLONE, IPRUN_IMX_DUAL_SPLIT, IPRUN_IMX_SINGLE_B
            Uint16          async; /* 0: sync execution, 1: async execution */
            Uint16          cmd1ptr_ofst; // iMX cmdptr
            IMX_RUN_Context context;
            void (*callback)(void *);
        } IMX_RUN_extension;



        typedef struct DmaTferStruct {

            Uint32 ddrAddr; // starting address in DDR
            Uint32 ddrWidth; // data width in DDR, in bytes
            Int16  reserved0; // not used
            Int32  ddrOfstNextBlock; // offset to next block, in bytes
            Int32  ddrOfstNextBlockRow; // offset to next row of blocks, in bytes

            Uint32 imgBufAddr; //starting address in image buffer
            Int16  imgBufWidth; //data width in image buffer, in bytes

            Int16 reserved1; // not used

            Uint16 blockWidth; // block width in bytes
            Uint16 blockHeight; // block height in bytes
            Uint16 reserved2;

            Uint16 dmaChNo; // DMA channel number

        }  imxRunDmaTferStruct;

/* Module's object */
        typedef struct IMX_run {

            // all widths and heights in terms of words
            imxRunDmaTferStruct *dmaIn;
            Uint16               numDmaIn;

            imxRunDmaTferStruct *dmaOut;
            Uint16               numDmaOut;

            Uint16 numVertBlocks;
            Uint16 numHorzBlocks;

            IMX_RUN_extension *extension; // for future IPRUN extension
            Uint32             chunksize;
            Uint16             compIntEna;
            Uint16             compCode;
            Uint16             cmdptr_ofst; // iMX cmdptr
            void              *customExtension; // for customer extension
            IP_run             internalhandle;
        } IMX_run;



#else

        typedef struct Sdram_Imgbuf {

            Uint32 loc1_addr; // location one addr --- normally ddram addr
            Int16  loc1_Bindex; // image width
                                // can also b used for read/writing interleaved
                                // data patterns; real_width in this case will be
                                // loc1_Cindex
            Int16 loc1_Cindex; // not used
            Int32 inc0; // offset to next block
            Int32 inc1; // offset to next row of blocks


            Uint32 loc2_addr; //location 2 addr -- normally inptr in imgbuf or DSP memory
            Int16  loc2_Bindex; //normally dma_realwidth in imgbuf
                                // can also b used for read/writing interleaved
                                // data patterns; real_width in this case will be
                                // loc2_Cindex
            Int16 loc2_Cindex; // not used



            Uint16 dma_width; // A CNT
            Uint16 dma_height; // B CNT
            Uint16 C_CNT;

            Uint16 dmaChNo; // DMA channel number

        }  Sdram_Imgbuf;

/* Module's object */
        typedef struct IMX_run {

            // all widths and heights in terms of words
            Sdram_Imgbuf *dmaIn;
            Uint16        num_in_comp;

            Sdram_Imgbuf *dmaOut;
            Uint16        num_out_comp;

            Uint16 num_vert;
            Uint16 num_horz;

            Uint16 x;
            Uint16 y;
            Uint32 chunksize;
            Uint16 compIntEna;
            Uint16 compCode;
            Uint16 cmdptr_ofst;     // iMX cmdptr

        } IMX_run;

#endif


    typedef struct IMX_run *IMX_run_handle;

//Int32 IMX_RUN_init(IP_RUN_InitParams* init);
    Int32 IMX_Run_Init();

//Int32 IMX_RUN_deInit(IP_RUN_InitParams* init);
    Int32 IMX_Run_Deinit();

//Int32 IMX_RUN_registerAlgo(IP_run * handle, Int32 threadId);
    Int32 IMX_RUN_RegisterAlgo(IMX_run *handle, Int32 threadId);

//Int32 IMX_RUN_unregisterAlgo(IP_run * handle, Int32 threadId);
    Int32 IMX_RUN_UnRegisterAlgo (IMX_run *handle, Int32 threadId, Int32 lasthndlflag);

//Int32 IMX_RUN_resetAlgo(IP_run * handle, Int32 threadId, Int32 resetCmd);


//void IMX_RUN_start(Int32 threadId);
    void IMX_Run_Start(IMX_run *handle);

//void IMX_RUN_wait();
    void IMX_Run_Wait();

    static void IMX_Run_Callback();

    void IMX_Run_SIMCOP_Reconfigure();


//Int32 IMX_RUN_isBusy();


//Int32 IMX_RUN_getCompCode();



/* Macros used for backward compatibility with earlier version of IMX_RUN */
#define IMX_runseq_dm350_Init(a) (IMX_RUN_init(), IMX_RUN_registerAlgo(a, 0))
#define IMX_runseq_dm350_Run()  IMX_RUN_start(0)
#define IMX_runseq_dm350_Wait() IMX_RUN_wait()
#define IMX_runseq_dm350_DeInit(a) IMX_RUN_unregisterAlgo(a, 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

