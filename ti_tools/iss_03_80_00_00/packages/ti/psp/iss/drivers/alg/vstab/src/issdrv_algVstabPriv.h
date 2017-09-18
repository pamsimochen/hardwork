/** ==================================================================
 *  @file   issdrv_algVstabPriv.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/drivers/alg/vstab/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Stack size for Alg processing task */
#define ISSALG_CAPT_TSK_STACK_UPDATE     (10*1024)

/* Task priority for Alg processing task */
#define ISSALG_CAPT_TSK_PRI_UPDATE       (12) //(10)

/** CLM State - Idle  */
#define ISSALG_CAPT_STATE_IDLE     (0)

/** CLM State - Running/Active */
#define ISSALG_CAPT_STATE_RUNNING  (1)

/* ===================================================================
 *  @func     IssAlg_captIsrComplete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void IssAlg_captIsrComplete();

/**
  \brief Alg Manager - "Do Alg processing" callback prototype
*/
typedef Void(*IssAlg_CaptCompleteCb) ();

/* 
 * Capture Alg manager object */
typedef struct {
    Int32 state;                                          
	/** state -
	* VPS_CAPT_LM_STATE_xxx 
	*/
    Semaphore_Handle lock;                                 
	/** CLM lock */

    IssAlg_CaptCompleteCb completeCb;
    /** 
	 * CLM completion driver callback
     */
	 
    UInt32 clockTicks;

#ifdef VPS_CFG_CAPT_USE_GP_TIMER
    Timer_Handle timerHndl;
#else
    Clock_Handle clockHndl;
#endif
    /** 
     * Timer handle - used to periodically wake up driver task
     */
    UInt32 maxAlgPostTime;
    UInt32 minAlgPostTime;
    UInt32 avgAlgPostTime;
    UInt32 totalAlgPostTime;
    UInt32 AlgPostCount;
    UInt32 AlgStallCount;

    UInt32 recvDescCount;
    UInt32 errorDescCount;
    UInt32 dropFrameCount;
    UInt32 extraDescProgrammedCount;

    UInt32 errorDescWrCount;
    UInt32 intcTimeoutCount;
    UInt32 intcTimeoutCount2;
    UInt32 pidSpecialConditionCount;
    UInt32 zeroPidCnt;
    UInt32 minLoopCnt;
    UInt32 loopNum;
    UInt32 minLoop2Cnt;
    UInt32 loop2Num;

} IssAlg_CaptObj;

/* ===================================================================
 *  @func     IssAlg_captDeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssAlg_captDeInit();

/* ===================================================================
 *  @func     IssAlg_captInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 IssAlg_captInit();

/* ===================================================================
 *  @func     Iss_captTskDoCallback                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_captTskDoCallback();

/* ===================================================================
 *  @func     IssAlg_captTskUpdate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void IssAlg_captTskUpdate(UArg arg1, UArg arg2);

/* ===================================================================
 *  @func     video_bsc_copyBuffers                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void video_bsc_copyBuffers();
#ifdef __cplusplus
}
#endif /* __cplusplus */
