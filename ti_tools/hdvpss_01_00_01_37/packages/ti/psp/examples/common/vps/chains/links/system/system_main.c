/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>
#include <ti/psp/examples/common/vps/chains/links/capture/captureLink_priv.h>
#include <ti/psp/examples/common/vps/chains/links/scalarSwMs/scalarSwMsLink_priv.h>
#include <ti/psp/examples/common/vps/chains/links/display/displayLink_priv.h>


#pragma DATA_ALIGN(gSystem_tskStack, 32)
#pragma DATA_SECTION(gSystem_tskStack, ".bss:taskStackSection")
UInt8 gSystem_tskStack[SYSTEM_TSK_STACK_SIZE];


System_Obj gSystem_obj;

Void System_main(UArg arg0, UArg arg1)
{
    System_init();

    GT_assert( GT_DEFAULT_MASK, gSystem_obj.chainsMainFunc!=NULL);

    gSystem_obj.chainsMainFunc(NULL, NULL);

    System_deInit();
}

/* Create test task */
Int32 System_start(Task_FuncPtr chainsMainFunc)
{
    Task_Params tskParams;

    memset(&gSystem_obj, 0, sizeof(gSystem_obj));

    gSystem_obj.chainsMainFunc = chainsMainFunc;

    /*
     * Create test task
     */
    Task_Params_init ( &tskParams );

    tskParams.priority = SYSTEM_TSK_PRI;
    tskParams.stack = gSystem_tskStack;
    tskParams.stackSize = sizeof ( gSystem_tskStack );

    gSystem_obj.tsk = Task_create ( System_main,
                                    &tskParams, NULL );

    GT_assert( GT_DEFAULT_MASK,  gSystem_obj.tsk != NULL );

    return FVID2_SOK;
}

Int32 System_init()
{
    Int32         status;
#ifdef SYSTEM_DEBUG
    const Char    *versionStr;
#endif
    Vps_PlatformDeviceInitParams deviceInitPrms;
    Vps_PlatformInitParams platformInitPrms;
    Semaphore_Params semParams;

    #ifdef SYSTEM_DEBUG
    Vps_printf ( " %d: SYSTEM  : System Init in progress !!!\n", Clock_getTicks());
    #endif

    VpsUtils_prfInit();

    VpsUtils_mbxInit();

    /*
     * Init memory allocator
     */
    VpsUtils_memInit_largeHeap (  );

    #ifndef PLATFORM_ZEBU
    VpsUtils_memClearOnAlloc(TRUE);
    #endif

    #ifdef SYSTEM_USE_TILER
    #ifdef TI_816X_BUILD
    VpsUtils_tilerInit (  );
    #endif /* TI_814X_BUILD */
    #endif

    #ifdef SYSTEM_DEBUG_TILER_ALLOC
    VpsUtils_tilerDebugLogEnable ( TRUE );
    #endif
    platformInitPrms.isPinMuxSettingReq = TRUE;
    status = Vps_platformInit(&platformInitPrms);
    GT_assert( GT_DEFAULT_MASK,  status == 0 );

    #ifdef SYSTEM_DEBUG
    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf(" %d: SYSTEM : HDVPSS Drivers Version: %s\n",
        Clock_getTicks(),
        versionStr
        );
    #endif

    /*
     * FVID2 system init
     */
    status = FVID2_init ( NULL );
    GT_assert( GT_DEFAULT_MASK,  status == 0 );

    deviceInitPrms.isI2cInitReq = TRUE;
    deviceInitPrms.isI2cProbingReq = TRUE;
    status = Vps_platformDeviceInit(&deviceInitPrms);
    GT_assert( GT_DEFAULT_MASK,  status == 0 );

    status = VpsUtils_appInit();
    GT_assert( GT_DEFAULT_MASK,  status == 0 );

#ifdef SYSTEM_DEBUG_VIP_RES_ALLOC
    /*
     * enable logs from VIP resource allocator
     */
    Vcore_vipResDebugLogEnable ( TRUE );
#endif

    status = VpsUtils_mbxCreate(&gSystem_obj.mbx);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    Semaphore_Params_init ( &semParams );
    semParams.mode = Semaphore_Mode_BINARY;

    gSystem_obj.vipLock[SYSTEM_VIP_0] = Semaphore_create ( 1u, &semParams, NULL );

    Semaphore_Params_init ( &semParams );
    semParams.mode = Semaphore_Mode_BINARY;

    gSystem_obj.vipLock[SYSTEM_VIP_1] = Semaphore_create ( 1u, &semParams, NULL );

    System_clearVipResetFlag(SYSTEM_VIP_0);
    System_clearVipResetFlag(SYSTEM_VIP_1);

    VpsUtils_prfLoadRegister(gSystem_obj.tsk, "SYSTEM  ");

    #ifdef SYSTEM_DEBUG
    Vps_printf ( " %d: SYSTEM  : System Init Done !!!\n", Clock_getTicks());
    #endif

    return status;

}

Int32 System_deInit()
{
    #ifdef SYSTEM_DEBUG
    Vps_printf ( " %d: SYSTEM  : System De-Init in progress !!!\n", Clock_getTicks());
    #endif

    Semaphore_delete ( &gSystem_obj.vipLock[SYSTEM_VIP_0] );
    Semaphore_delete ( &gSystem_obj.vipLock[SYSTEM_VIP_1] );

    VpsUtils_prfLoadUnRegister(gSystem_obj.tsk);

    VpsUtils_mbxDelete(&gSystem_obj.mbx);

    VpsUtils_appDeInit();

    Vps_platformDeviceDeInit();

    /*
     * FVID2 system de-init
     */
    FVID2_deInit ( NULL );

    Vps_platformDeInit();

    #ifdef SYSTEM_USE_TILER
    VpsUtils_tilerDeInit (  );
    #endif

    /*
     * De-init memory allocator
     */
    VpsUtils_memDeInit (  );

    VpsUtils_mbxDeInit();

    VpsUtils_prfDeInit();

    #ifdef SYSTEM_DEBUG
    Vps_printf ( " %d: SYSTEM  : System De-Init Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

void System_memPrintHeapStatus()
{
    #ifdef SYSTEM_DEBUG
    Vps_printf(" %d: SYSTEM : Descriptor Heap Free Space (Current = %d bytes, Minimum = %d bytes) \r\n",
        Clock_getTicks(),
        VpsUtils_getDescMemHeapFreeSpace(),
        VpsUtils_getDescMemHeapMinFreeSpace()
    );
    Vps_printf(" %d: SYSTEM : Heap Free Space (System = %d bytes, Buffer = %d bytes, Mbx = %d msgs) \r\n",
        Clock_getTicks(),
        VpsUtils_memGetSystemHeapFreeSpace(),
        VpsUtils_memGetBufferHeapFreeSpace(),
        VpsUtils_mbxGetFreeMsgCount()
    );
    #endif

    return;
}

Int32 System_lockVip(UInt32 vipInst)
{
    if(vipInst<SYSTEM_VIP_MAX)
    {
        Semaphore_pend ( gSystem_obj.vipLock[vipInst], BIOS_WAIT_FOREVER );
    }

    return FVID2_SOK;
}

Int32 System_unlockVip(UInt32 vipInst)
{
    if(vipInst<SYSTEM_VIP_MAX)
    {
        Semaphore_post ( gSystem_obj.vipLock[vipInst] );
    }

    return FVID2_SOK;
}

Int32 System_setVipResetFlag(UInt32 vipInst)
{
    if(vipInst<SYSTEM_VIP_MAX)
    {
        gSystem_obj.vipResetFlag[vipInst] = TRUE;
    }

    return FVID2_SOK;
}

Bool System_clearVipResetFlag(UInt32 vipInst)
{
    Bool isReset = FALSE;

    if(vipInst<SYSTEM_VIP_MAX)
    {
        isReset = gSystem_obj.vipResetFlag[vipInst];

        gSystem_obj.vipResetFlag[vipInst] = FALSE;
    }

    return isReset;

}

int System_resumeExecution()
{
    gSystem_obj.haltExecution = FALSE;

    return 0;
}

int System_haltExecution()
{
    gSystem_obj.haltExecution = TRUE;
    Vps_rprintf(" %d: SYSTEM: Executing Halted !!!\n", Clock_getTicks());
    while(gSystem_obj.haltExecution)
    {
        Task_sleep(100);
    }

    return 0;
}
