/** ==================================================================
 *  @file   issutils_prf.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#include <ti/psp/platforms/utility/issutils_prf.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/sysbios/knl/Clock.h>

typedef struct {
    Bool isAlloc;
    char name[32];
    Task_Handle pTsk;
    UInt64 totalTskThreadTime;

} IssUtils_PrfLoadObj;

typedef struct {
    IssUtils_PrfTsHndl tsObj[ISSUTILS_PRF_MAX_HNDL];
    IssUtils_PrfLoadObj loadObj[ISSUTILS_PRF_MAX_HNDL];

} IssUtils_PrfObj;

typedef struct {
    UInt64 totalSwiThreadTime;
    UInt64 totalHwiThreadTime;
    UInt64 totalTime;
    UInt64 totalIdlTskTime;
} IssUtils_AccPrfLoadObj;

IssUtils_PrfObj gIssUtils_prfObj;

IssUtils_AccPrfLoadObj gIssUtils_accPrfLoadObj;

UInt32 gIssUtils_startLoadCalc = 0;

/* ===================================================================
 *  @func     IssUtils_prfInit                                               
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
Int32 IssUtils_prfInit()
{
    memset(&gIssUtils_prfObj, 0, sizeof(gIssUtils_prfObj));
    memset(&gIssUtils_accPrfLoadObj, 0, sizeof(IssUtils_AccPrfLoadObj));

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_prfDeInit                                               
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
Int32 IssUtils_prfDeInit()
{

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_prfTsCreate                                               
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
IssUtils_PrfTsHndl *IssUtils_prfTsCreate(char *name)
{
    UInt32 hndlId;

    IssUtils_PrfTsHndl *pHndl = NULL;

    UInt32 cookie;

    cookie = Hwi_disable();

    for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gIssUtils_prfObj.tsObj[hndlId];

        if (pHndl->isAlloc == FALSE)
        {
            strncpy(pHndl->name, name, sizeof(pHndl->name));

            pHndl->isAlloc = TRUE;

            IssUtils_prfTsReset(pHndl);
            break;
        }
    }

    Hwi_restore(cookie);

    return pHndl;
}

/* ===================================================================
 *  @func     IssUtils_prfTsDelete                                               
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
Int32 IssUtils_prfTsDelete(IssUtils_PrfTsHndl * pHndl)
{
    pHndl->isAlloc = FALSE;
    return 0;
}

/* ===================================================================
 *  @func     IssUtils_prfTsBegin                                               
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
UInt64 IssUtils_prfTsBegin(IssUtils_PrfTsHndl * pHndl)
{
    /* Currently thi is not used as 64bit timestamp is not working TODO */
    pHndl->startTs = IssUtils_prfTsGet64();

    return pHndl->startTs;
}

/* ===================================================================
 *  @func     IssUtils_prfTsEnd                                               
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
UInt64 IssUtils_prfTsEnd(IssUtils_PrfTsHndl * pHndl, UInt32 numFrames)
{
    return IssUtils_prfTsDelta(pHndl, pHndl->startTs, numFrames);
}

/* ===================================================================
 *  @func     IssUtils_prfTsDelta                                               
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
UInt64 IssUtils_prfTsDelta(IssUtils_PrfTsHndl * pHndl, UInt64 startTime,
                           UInt32 numFrames)
{
    UInt64 endTs;

    UInt32 cookie;

    /* Currently thi is not used as 64bit timestamp is not working TODO */
    endTs = IssUtils_prfTsGet64();

    cookie = Hwi_disable();

    pHndl->totalTs += (endTs - pHndl->startTs);
    pHndl->count++;
    pHndl->numFrames += numFrames;

    Hwi_restore(cookie);

    return endTs;
}

/* ===================================================================
 *  @func     IssUtils_prfTsReset                                               
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
Int32 IssUtils_prfTsReset(IssUtils_PrfTsHndl * pHndl)
{
    UInt32 cookie;

    cookie = Hwi_disable();

    pHndl->totalTs = 0;
    pHndl->count = 0;
    pHndl->numFrames = 0;

    Hwi_restore(cookie);

    return 0;
}

UInt64 IssUtils_prfTsGet64()
{
    Types_Timestamp64 ts64;

    UInt64 curTs;

    Timestamp_get64(&ts64);

    curTs = ((UInt64) ts64.hi << 32) | ts64.lo;

    return curTs;
}

/* ===================================================================
 *  @func     IssUtils_prfTsPrint                                               
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
Int32 IssUtils_prfTsPrint(IssUtils_PrfTsHndl * pHndl, Bool resetAfterPrint)
{
    UInt32 timeMs, fps, fpc;

    Types_FreqHz cpuHz;

    /* This is not used as 64 bit timestamp is not working TODO */
    UInt32 cpuKhz;

    Timestamp_getFreq(&cpuHz);

    /* Currently thi is not used as 64bit timestamp is not working TODO */
    cpuKhz = cpuHz.lo / 1000;                              /* convert to Khz */

    /* Currently thi is not used as 64bit timestamp is not working TODO */
    timeMs = (pHndl->totalTs) / cpuKhz;

    fps = (pHndl->numFrames * 1000) / timeMs;
    fpc = (pHndl->numFrames) / pHndl->count;

    Iss_printf(" %d: PRF : %s : t: %d ms, c: %d, f: %d, fps: %d, fpc: %d \r\n", Clock_getTicks(), pHndl->name, timeMs,  /* in 
                                                                                                                         * msecs 
                                                                                                                         */
               pHndl->count, pHndl->numFrames, fps,        /* frames per
                                                            * second */
               fpc                                         /* frames per
                                                            * count */
        );

    if (resetAfterPrint)
        IssUtils_prfTsReset(pHndl);

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_prfTsPrintAll                                               
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
Int32 IssUtils_prfTsPrintAll(Bool resetAfterPrint)
{
    UInt32 hndlId;

    IssUtils_PrfTsHndl *pHndl;

    Iss_printf("\r\n");

    for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gIssUtils_prfObj.tsObj[hndlId];

        if (pHndl->isAlloc)
        {
            IssUtils_prfTsPrint(pHndl, resetAfterPrint);
        }
    }

    Iss_printf("\r\n");

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_prfLoadRegister                                               
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
Int32 IssUtils_prfLoadRegister(Task_Handle pTsk, char *name)
{
    UInt32 hndlId;

    UInt32 cookie;

    Int32 status = FVID2_EFAIL;

    IssUtils_PrfLoadObj *pHndl;

    cookie = Hwi_disable();

    for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gIssUtils_prfObj.loadObj[hndlId];

        if (pHndl->isAlloc == FALSE)
        {
            pHndl->isAlloc = TRUE;
            pHndl->pTsk = pTsk;
            strncpy(pHndl->name, name, sizeof(pHndl->name));
            status = FVID2_SOK;
            break;
        }
    }

    Hwi_restore(cookie);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_prfLoadUnRegister                                               
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
Int32 IssUtils_prfLoadUnRegister(Task_Handle pTsk)
{
    UInt32 hndlId;

    UInt32 cookie;

    Int32 status = FVID2_EFAIL;

    IssUtils_PrfLoadObj *pHndl;

    cookie = Hwi_disable();

    for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gIssUtils_prfObj.loadObj[hndlId];

        if (pHndl->isAlloc && pHndl->pTsk == pTsk)
        {
            pHndl->isAlloc = FALSE;
            status = FVID2_SOK;
            break;
        }
    }

    Hwi_restore(cookie);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_prfLoadPrintAll                                               
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
Int32 IssUtils_prfLoadPrintAll(Bool printTskLoad)
{
    Int32 hwiLoad, swiLoad, tskLoad, hndlId, cpuLoad;

    IssUtils_PrfLoadObj *pHndl;

    hwiLoad = swiLoad = tskLoad = -1;

    hwiLoad = (gIssUtils_accPrfLoadObj.totalHwiThreadTime * 100) /
        gIssUtils_accPrfLoadObj.totalTime;
    swiLoad = (gIssUtils_accPrfLoadObj.totalSwiThreadTime * 100) /
        gIssUtils_accPrfLoadObj.totalTime;
    cpuLoad = 100 - ((gIssUtils_accPrfLoadObj.totalIdlTskTime * 100) /
                     gIssUtils_accPrfLoadObj.totalTime);

    Iss_printf("\r\n");
    Iss_printf(" %d: LOAD: CPU: %d%% HWI: %d%%, SWI:%d%% \n", Clock_getTicks(),
               cpuLoad, hwiLoad, swiLoad);

    if (printTskLoad)
    {
        for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
        {
            pHndl = &gIssUtils_prfObj.loadObj[hndlId];

            if (pHndl->isAlloc)
            {
                tskLoad = -1;

                tskLoad = (pHndl->totalTskThreadTime * 100) /
                    gIssUtils_accPrfLoadObj.totalTime;

                Iss_printf(" %d: LOAD: TSK: %s: %d%% \r\n", Clock_getTicks(),
                           pHndl->name, tskLoad);
            }
        }
    }
    Iss_printf("\r\n");

    return 0;
}

/* ===================================================================
 *  @func     IssUtils_prfLoadCalcStart                                               
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
Void IssUtils_prfLoadCalcStart()
{
    UInt32 cookie;

    cookie = Hwi_disable();
    gIssUtils_startLoadCalc = TRUE;
    Hwi_restore(cookie);
}

/* ===================================================================
 *  @func     IssUtils_prfLoadCalcStop                                               
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
Void IssUtils_prfLoadCalcStop()
{
    UInt32 cookie;

    cookie = Hwi_disable();
    gIssUtils_startLoadCalc = FALSE;
    Hwi_restore(cookie);
}

/* ===================================================================
 *  @func     IssUtils_prfLoadCalcReset                                               
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
Void IssUtils_prfLoadCalcReset()
{
    IssUtils_PrfLoadObj *pHndl;

    UInt32 hndlId;

    gIssUtils_accPrfLoadObj.totalHwiThreadTime = 0;
    gIssUtils_accPrfLoadObj.totalSwiThreadTime = 0;
    gIssUtils_accPrfLoadObj.totalTime = 0;
    gIssUtils_accPrfLoadObj.totalIdlTskTime = 0;
    /* Reset the performace loads accumulator */
    for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gIssUtils_prfObj.loadObj[hndlId];

        if (pHndl->isAlloc && pHndl->pTsk != NULL)
        {
            pHndl->totalTskThreadTime = 0;

        }
    }
}

/* ===================================================================
 *  @func     IssUtils_prfTsTest                                               
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
Int32 IssUtils_prfTsTest(UInt32 count, UInt32 delayInMs)
{
    IssUtils_PrfTsHndl *pPrf;

    pPrf = IssUtils_prfTsCreate("TEST");

    while (count--)
    {
        IssUtils_prfTsBegin(pPrf);
        Task_sleep(delayInMs);
        IssUtils_prfTsEnd(pPrf, 1);

        IssUtils_prfTsPrintAll(FALSE);
    }

    IssUtils_prfTsDelete(pPrf);

    return 0;
}

/* Function called by Loadupdate for each update cycle */
/* ===================================================================
 *  @func     IssUtils_prfLoadUpdate                                               
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
Void IssUtils_prfLoadUpdate()
{
    IssUtils_PrfLoadObj *pHndl;

    Load_Stat hwiLoadStat, swiLoadStat, tskLoadStat, idlTskLoadStat;

    Task_Handle idlTskHndl = NULL;

    UInt32 hndlId;

    if (TRUE == gIssUtils_startLoadCalc)
    {
        idlTskHndl = Task_getIdleTask();
        /* Get the all loads first */
        Load_getGlobalHwiLoad(&hwiLoadStat);
        Load_getGlobalSwiLoad(&swiLoadStat);
        Load_getTaskLoad(idlTskHndl, &idlTskLoadStat);

        gIssUtils_accPrfLoadObj.totalHwiThreadTime += hwiLoadStat.threadTime;
        gIssUtils_accPrfLoadObj.totalSwiThreadTime += swiLoadStat.threadTime;
        gIssUtils_accPrfLoadObj.totalTime += hwiLoadStat.totalTime;
        gIssUtils_accPrfLoadObj.totalIdlTskTime += idlTskLoadStat.threadTime;

        /* Call the load updated function of each registered task one by one
         * along with the swiLoad, hwiLoad, and Task's own load */
        for (hndlId = 0; hndlId < ISSUTILS_PRF_MAX_HNDL; hndlId++)
        {
            pHndl = &gIssUtils_prfObj.loadObj[hndlId];

            if (pHndl->isAlloc && pHndl->pTsk != NULL)
            {
                Load_getTaskLoad(pHndl->pTsk, &tskLoadStat);
                pHndl->totalTskThreadTime += tskLoadStat.threadTime;

            }
        }
    }
}
