/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/sysbios/knl/Clock.h>

typedef struct
{
    Bool isAlloc;
    char name[32];
    Task_Handle pTsk;
    UInt64                  totalTskThreadTime;

} VpsUtils_PrfLoadObj;

typedef struct
{
    VpsUtils_PrfTsHndl tsObj[VPSUTILS_PRF_MAX_HNDL];
    VpsUtils_PrfLoadObj loadObj[VPSUTILS_PRF_MAX_HNDL];

} VpsUtils_PrfObj;

typedef struct
{
    UInt64  totalSwiThreadTime;
    UInt64  totalHwiThreadTime;
    UInt64  totalTime;
    UInt64  totalIdlTskTime;
}VpsUtils_AccPrfLoadObj;

VpsUtils_PrfObj gVpsUtils_prfObj;
VpsUtils_AccPrfLoadObj gVpsUtils_accPrfLoadObj;
UInt32                 gVpsUtils_startLoadCalc = 0;

Int32 VpsUtils_prfInit()
{
    memset(&gVpsUtils_prfObj, 0, sizeof(gVpsUtils_prfObj));
    memset(&gVpsUtils_accPrfLoadObj, 0 , sizeof(VpsUtils_AccPrfLoadObj));

    return 0;
}

Int32 VpsUtils_prfDeInit()
{

    return 0;
}

VpsUtils_PrfTsHndl *VpsUtils_prfTsCreate(char *name)
{
    UInt32 hndlId;
    VpsUtils_PrfTsHndl *pHndl = NULL;

    UInt32 cookie;

    cookie = Hwi_disable();

    for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gVpsUtils_prfObj.tsObj[hndlId];

        if(pHndl->isAlloc==FALSE)
        {
            strncpy(pHndl->name, name, sizeof(pHndl->name));

            pHndl->isAlloc = TRUE;

            VpsUtils_prfTsReset(pHndl);
            break;
        }
    }

    Hwi_restore(cookie);

    return pHndl;
}

Int32 VpsUtils_prfTsDelete(VpsUtils_PrfTsHndl *pHndl)
{
    pHndl->isAlloc = FALSE;
    return 0;
}

UInt64 VpsUtils_prfTsBegin(VpsUtils_PrfTsHndl *pHndl)
{
    /* Currently thi is not used as 64bit timestamp is not working TODO*/
    pHndl->startTs = VpsUtils_prfTsGet64();

    return pHndl->startTs;
}

UInt64 VpsUtils_prfTsEnd(VpsUtils_PrfTsHndl *pHndl, UInt32 numFrames)
{
    return VpsUtils_prfTsDelta(pHndl, pHndl->startTs, numFrames);
}

UInt64 VpsUtils_prfTsDelta(VpsUtils_PrfTsHndl *pHndl, UInt64 startTime, UInt32 numFrames)
{
    UInt64 endTs;
    UInt32 cookie;

    /* Currently thi is not used as 64bit timestamp is not working TODO*/
    endTs = VpsUtils_prfTsGet64();


    cookie = Hwi_disable();

    pHndl->totalTs += (endTs - pHndl->startTs);
    pHndl->count++;
    pHndl->numFrames += numFrames;

    Hwi_restore(cookie);

    return endTs;
}

Int32 VpsUtils_prfTsReset(VpsUtils_PrfTsHndl *pHndl)
{
    UInt32 cookie;

    cookie = Hwi_disable();

    pHndl->totalTs  = 0;
    pHndl->count    = 0;
    pHndl->numFrames= 0;

    Hwi_restore(cookie);

    return 0;
}

UInt64 VpsUtils_prfTsGet64()
{
    Types_Timestamp64 ts64;
    UInt64 curTs;

    Timestamp_get64(&ts64);

    curTs = ((UInt64)ts64.hi << 32) | ts64.lo;

    return curTs;
}

Int32 VpsUtils_prfTsPrint(VpsUtils_PrfTsHndl *pHndl, Bool resetAfterPrint)
{
    UInt32 timeMs, fps, fpc;
    Types_FreqHz cpuHz;
    /* This is not used as 64 bit timestamp is not working TODO */
    UInt32 cpuKhz;

    Timestamp_getFreq(&cpuHz);

    /* Currently thi is not used as 64bit timestamp is not working TODO*/
    cpuKhz = cpuHz.lo/1000; /* convert to Khz */

    /* Currently thi is not used as 64bit timestamp is not working TODO*/
    timeMs = (pHndl->totalTs)/cpuKhz;

    fps = (pHndl->numFrames*1000)/timeMs;
    fpc = (pHndl->numFrames)/pHndl->count;

    Vps_printf(" %d: PRF : %s : t: %d ms, c: %d, f: %d, fps: %d, fpc: %d \r\n",
            Clock_getTicks(),
            pHndl->name,
            timeMs, /* in msecs    */
            pHndl->count,
            pHndl->numFrames,
            fps, /* frames per second */
            fpc  /* frames per count */
        );

    if(resetAfterPrint)
        VpsUtils_prfTsReset(pHndl);

    return 0;
}

Int32 VpsUtils_prfTsPrintAll(Bool resetAfterPrint)
{
    UInt32 hndlId;
    VpsUtils_PrfTsHndl *pHndl;

    Vps_printf("\r\n");

    for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gVpsUtils_prfObj.tsObj[hndlId];

        if(pHndl->isAlloc)
        {
            VpsUtils_prfTsPrint(pHndl, resetAfterPrint);
        }
    }

    Vps_printf("\r\n");

    return 0;
}

Int32 VpsUtils_prfLoadRegister(Task_Handle pTsk,
                               char *name)
{
    UInt32 hndlId;
    UInt32 cookie;
    Int32  status = FVID2_EFAIL;
    VpsUtils_PrfLoadObj *pHndl;

    cookie = Hwi_disable();

    for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gVpsUtils_prfObj.loadObj[hndlId];

        if(pHndl->isAlloc==FALSE)
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

Int32 VpsUtils_prfLoadUnRegister(Task_Handle pTsk)
{
    UInt32 hndlId;
    UInt32 cookie;
    Int32  status = FVID2_EFAIL;
    VpsUtils_PrfLoadObj *pHndl;

    cookie = Hwi_disable();

    for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gVpsUtils_prfObj.loadObj[hndlId];

        if(pHndl->isAlloc && pHndl->pTsk==pTsk)
        {
            pHndl->isAlloc = FALSE;
            status = FVID2_SOK;
            break;
        }
    }

    Hwi_restore(cookie);

    return status;
}

Int32 VpsUtils_prfLoadPrintAll(Bool printTskLoad)
{
    #ifdef PLATFORM_ZEBU
    return 0;
    #else

    Int32 hwiLoad, swiLoad, tskLoad, hndlId, cpuLoad;
    VpsUtils_PrfLoadObj *pHndl;


    hwiLoad = swiLoad = tskLoad = -1;

    hwiLoad = (gVpsUtils_accPrfLoadObj.totalHwiThreadTime  * 100) /
                gVpsUtils_accPrfLoadObj.totalTime;
    swiLoad = (gVpsUtils_accPrfLoadObj.totalSwiThreadTime * 100) /
                gVpsUtils_accPrfLoadObj.totalTime;
    cpuLoad = 100 - ((gVpsUtils_accPrfLoadObj.totalIdlTskTime * 100) /
                gVpsUtils_accPrfLoadObj.totalTime);

    Vps_printf("\r\n");
    Vps_printf(" %d: LOAD: CPU: %d%% HWI: %d%%, SWI:%d%% \n", Clock_getTicks(),
                        cpuLoad,
                        hwiLoad,
                        swiLoad);

    if(printTskLoad)
    {
        for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
        {
            pHndl = &gVpsUtils_prfObj.loadObj[hndlId];

            if(pHndl->isAlloc)
            {
                tskLoad = -1;

                tskLoad = (pHndl->totalTskThreadTime * 100 ) /
                        gVpsUtils_accPrfLoadObj.totalTime;

                Vps_printf(" %d: LOAD: TSK: %s: %d%% \r\n", Clock_getTicks(),
                                pHndl->name,
                                tskLoad);
            }
        }
    }
    Vps_printf("\r\n");

    return 0;
    #endif
}

Void VpsUtils_prfLoadCalcStart()
{
    UInt32 cookie;

    cookie = Hwi_disable();
    gVpsUtils_startLoadCalc = TRUE;
    Hwi_restore(cookie);
}

Void VpsUtils_prfLoadCalcStop()
{
    UInt32 cookie;

    cookie = Hwi_disable();
    gVpsUtils_startLoadCalc = FALSE;
    Hwi_restore(cookie);
}

Void VpsUtils_prfLoadCalcReset()
{
    VpsUtils_PrfLoadObj *pHndl;
    UInt32              hndlId;

    gVpsUtils_accPrfLoadObj.totalHwiThreadTime = 0;
    gVpsUtils_accPrfLoadObj.totalSwiThreadTime = 0;
    gVpsUtils_accPrfLoadObj.totalTime = 0;
    gVpsUtils_accPrfLoadObj.totalIdlTskTime = 0;
    /* Reset the performace loads accumulator */
    for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
    {
        pHndl = &gVpsUtils_prfObj.loadObj[hndlId];

        if(pHndl->isAlloc && pHndl->pTsk != NULL)
        {
            pHndl->totalTskThreadTime = 0;

        }
    }
}

Int32 VpsUtils_prfTsTest(UInt32 count, UInt32 delayInMs)
{
    VpsUtils_PrfTsHndl *pPrf;

    pPrf = VpsUtils_prfTsCreate("TEST");

    while(count--)
    {
        VpsUtils_prfTsBegin(pPrf);
        Task_sleep(delayInMs);
        VpsUtils_prfTsEnd(pPrf, 1);

        VpsUtils_prfTsPrintAll(FALSE);
    }

    VpsUtils_prfTsDelete(pPrf);

    return 0;
}

/* Function called by Loadupdate for each update cycle */
Void VpsUtils_prfLoadUpdate()
{
    VpsUtils_PrfLoadObj *pHndl;
    Load_Stat hwiLoadStat, swiLoadStat, tskLoadStat, idlTskLoadStat;
    Task_Handle idlTskHndl = NULL;
    UInt32 hndlId;

    if (TRUE == gVpsUtils_startLoadCalc)
    {
        idlTskHndl = Task_getIdleTask();
        /* Get the all loads first */
        Load_getGlobalHwiLoad(&hwiLoadStat);
        Load_getGlobalSwiLoad(&swiLoadStat);
        Load_getTaskLoad(idlTskHndl, &idlTskLoadStat);


        gVpsUtils_accPrfLoadObj.totalHwiThreadTime += hwiLoadStat.threadTime;
        gVpsUtils_accPrfLoadObj.totalSwiThreadTime += swiLoadStat.threadTime;
        gVpsUtils_accPrfLoadObj.totalTime += hwiLoadStat.totalTime;
        gVpsUtils_accPrfLoadObj.totalIdlTskTime += idlTskLoadStat.threadTime;


        /* Call the load updated function of each registered task one by one
         * along with the swiLoad, hwiLoad, and Task's own load
         */
        for(hndlId=0; hndlId<VPSUTILS_PRF_MAX_HNDL; hndlId++)
        {
            pHndl = &gVpsUtils_prfObj.loadObj[hndlId];

            if(pHndl->isAlloc && pHndl->pTsk != NULL)
            {
                Load_getTaskLoad(pHndl->pTsk, &tskLoadStat);
                pHndl->totalTskThreadTime += tskLoadStat.threadTime;

            }
        }
    }
}
