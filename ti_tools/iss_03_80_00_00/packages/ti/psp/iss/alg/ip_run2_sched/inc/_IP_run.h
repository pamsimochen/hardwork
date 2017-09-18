#ifndef __IPRUN_H

#define __IPRUN_H

#include "../../ip_run2/inc/ip_run.h"
#include <seq_IP_run.h>
#include <IP_run.h>

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/


typedef struct {
    
    IP_RUN_InitParams initParams;
    Uint32 active;
    IP_RUN_DebugFunc debugCB; /*  default NULL */
    void *debugCB_arg;
    IP_run algoConfig;
    Uint16 iMX0CmdPtrOffset;
    Uint16 iMX1CmdPtrOffset;
//    Uint16 *pCmdMem0; /* Currently not used */
//    Uint32 lenCmdMem0;/* Currently not used */
//    Uint16 *pCmdMem1; /* Currently not used */
//    Uint32 lenCmdMem1;/* Currently not used */
//    Uint16 *pCoefMem0;/* Currently not used */
//    Uint32 lenCoefMem0;/* Currently not used */
//    Uint16 *pCoefMem1;/* Currently not used */
//    Uint32 lenCoefMem1;/* Currently not used */
    Uint16 noActivateFlag;
    Uint16 multiCoreFlag;
    Uint16 singleBlockProcessing;
    Uint16 numDmaIn;
    dmaBlk_params dmain[IP_RUN_MAX_NUM_DMAIN_CHAN];

    Uint16 numDmaOut;
    dmaBlk_params dmaout[IP_RUN_MAX_NUM_DMAOUT_CHAN];

    Int16 iMXid;
} IP_RUN_Vars;

Int32 _IP_RUN_init(IP_RUN_Vars *vars, IP_RUN_InitParams* init);
Int32 _IP_RUN_deInit(IP_RUN_Vars *vars);
Int32 _IP_RUN_registerAlgo(IP_RUN_Vars *vars, IP_run_wrapper *handle, Int32 iMXthreadId);
Int32 _IP_RUN_unregisterAlgo(IP_RUN_Vars *vars, IP_run_wrapper * handle, Int32 iMXthreadId );
Int32 _IP_RUN_deInit(IP_RUN_Vars *vars);
Int32 _IP_RUN_setDebugCB(IP_RUN_Vars *vars, IP_RUN_DebugFunc debugCB, void *arg);
Int32 _IP_RUN_registerScatterGather(IP_RUN_Vars *vars, IP_run_wrapper *handle);
Int32 _IP_RUN_resetAlgo(IP_RUN_Vars *vars, IP_run_wrapper * handle, Int32 iMXthreadId, Int32 resetCmd);
void _IP_RUN_start(void *handle, IP_RUN_Vars *vars, Int32 iMXthreadIdx);
void _IP_RUN_startScatterGather(IP_RUN_Vars *vars);
Int32 _IP_RUN_activate(IP_RUN_Vars *vars);
Int32 _IP_RUN_deactivate(IP_RUN_Vars *vars);
Int32 _IP_RUN_getCompCode(IP_RUN_Vars *vars);
void _IP_RUN_wait(void *handle, IP_RUN_Vars *var);
Int32 _IP_RUN_isBusy(void *handle, IP_RUN_Vars *var);
void _IP_RUN_dma2dCopy(IP_RUN_Dma2dCopy *dma2dCopy);
void _IP_RUN_dma2dCopySetup(IP_RUN_Dma2dCopy *dma2dCopy);
void _IP_RUN_dma2dUpdateAddr(IP_RUN_Dma2dCopy *dma2dCopy);
void _IP_RUN_dma2dCopyStart(Uint16 chanNo);
void _IP_RUN_dma2dCopyWait(Uint16 chanNo);
void _IP_RUN_copy(void *dst, void *src, Uint32 numBytes);
Int32 _IP_RUN_setupSimcopDma(IP_run *internalhandle, IP_run_wrapper *handle);
Int32 _IP_RUN_setupEdma(IP_RUN_Vars *vars, IP_run_wrapper *handle);
Int32 _IP_RUN_resetEdma(IP_RUN_Vars *vars, IP_run_wrapper *handle, Int32 resetCmd);
Int32 _IP_RUN_freeEdma(IP_RUN_Vars *vars);
void _IP_RUN_runDma(IP_RUN_Vars *vars);

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif
