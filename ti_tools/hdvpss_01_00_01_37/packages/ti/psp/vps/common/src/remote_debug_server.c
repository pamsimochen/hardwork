/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "remote_debug_if.h"
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <string.h>
#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/psp/vps/common/vps_utils.h>

#define ENABLE_STATIC_ALLOCATION

#define REMOTE_DEBUG_SERVER_CURRENT_CORE         REMOTE_DEBUG_CORE_ID_M3VPSS

#define REMOTE_DEBUG_SERVER_PRINT_BUF_LEN        (1024*2)

#define STATIC static

typedef struct {

    unsigned int coreId;

    char printBuf[REMOTE_DEBUG_SERVER_PRINT_BUF_LEN];

} RemoteDebug_ServerObj;

#ifdef ENABLE_STATIC_ALLOCATION
#pragma DATA_SECTION(gRemoteDebug_coreObjMem, ".bss:extMemNonCache:remoteDebugCoreShm");
#pragma DATA_ALIGN(gRemoteDebug_coreObjMem, 4*1024);
STATIC RemoteDebug_CoreObj gRemoteDebug_coreObjMem[REMOTE_DEBUG_CORE_ID_MAX];
#endif  /* #ifdef ENABLE_STATIC_ALLOCATION */

#ifdef ENABLE_STATIC_ALLOCATION
#ifdef _IPNC_HW_PLATFORM_
STATIC RemoteDebug_CoreObj *gRemoteDebug_coreObj = gRemoteDebug_coreObjMem;
#endif
#else
STATIC RemoteDebug_CoreObj *gRemoteDebug_coreObj = NULL;
#endif

STATIC RemoteDebug_ServerObj gRemoteDebug_serverObj = { REMOTE_DEBUG_SERVER_CURRENT_CORE };

Int32 RemoteDebug_init(void)
{
    Int32       retVal = FVID2_SOK;

#ifdef ENABLE_STATIC_ALLOCATION
    gRemoteDebug_coreObj = &gRemoteDebug_coreObjMem[0u];
#else
    gRemoteDebug_coreObj = VpsUtils_allocDescMem(
        (sizeof(RemoteDebug_CoreObj) * REMOTE_DEBUG_CORE_ID_MAX),
        (4u * 1024u));
    if (NULL == gRemoteDebug_coreObj)
    {
        retVal = FVID2_EALLOC;
    }
#endif  /* #ifdef ENABLE_STATIC_ALLOCATION */

    return (retVal);
}

Int32 RemoteDebug_deInit(void)
{
    Int32       retVal = FVID2_SOK;

#ifdef ENABLE_STATIC_ALLOCATION
    #ifndef _IPNC_HW_PLATFORM_
        gRemoteDebug_coreObj = NULL;
	#endif
#else
    if (NULL != gRemoteDebug_coreObj)
    {
        VpsUtils_freeDescMem(
            gRemoteDebug_coreObj,
            (sizeof(RemoteDebug_CoreObj) * REMOTE_DEBUG_CORE_ID_MAX));
        gRemoteDebug_coreObj = NULL;
    }
#endif  /* #ifdef ENABLE_STATIC_ALLOCATION */

    return (retVal);
}

STATIC int RemoteDebug_serverPutString(unsigned int coreId, char *pString)
{
    unsigned int maxBytes, numBytes, copyBytes, serverIdx, clientIdx;
    volatile unsigned char *pDst;

    RemoteDebug_CoreObj *pCoreObj;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    if (NULL == gRemoteDebug_coreObj)
        return 0;

    pCoreObj = &gRemoteDebug_coreObj[coreId];

    if(pCoreObj->headerTag != REMOTE_DEBUG_HEADER_TAG)
        return -1;

    numBytes = strlen(pString);

    if(numBytes<=0)
        return -1;

    serverIdx = pCoreObj->serverIdx;
    clientIdx = pCoreObj->clientIdx;

    if(serverIdx < clientIdx )
        maxBytes = clientIdx-serverIdx;
    else
        maxBytes = (REMOTE_DEBUG_LOG_BUF_SIZE - serverIdx) + clientIdx;

    if(numBytes > maxBytes )
        return -1;

    pDst = &pCoreObj->serverLogBuf[0];

    for(copyBytes=0; copyBytes<numBytes; copyBytes++)
    {
        if(serverIdx>=REMOTE_DEBUG_LOG_BUF_SIZE)
            serverIdx = 0;

        pDst[serverIdx] = *pString++;
        serverIdx++;
    }

    if(serverIdx>=REMOTE_DEBUG_LOG_BUF_SIZE)
        serverIdx = 0;

    pDst[serverIdx] = 0;
    serverIdx++;

    pCoreObj->serverIdx = serverIdx;

    return 0;
}

int Vps_printf(char * format, ... )
{
    int retVal;
    va_list vaArgPtr;
    char *buf=NULL;
    UInt32 cookie;

    cookie = Hwi_disable (  );

    buf = &gRemoteDebug_serverObj.printBuf[0];

    va_start(vaArgPtr, format);
    vsnprintf(buf, REMOTE_DEBUG_SERVER_PRINT_BUF_LEN, format, vaArgPtr);
    va_end(vaArgPtr);

    retVal = RemoteDebug_serverPutString(gRemoteDebug_serverObj.coreId, buf);

    Hwi_restore ( cookie );

    System_printf(buf);

    return (retVal);
}

int Vps_rprintf(char * format, ... )
{
    int retVal;
    va_list vaArgPtr;
    char *buf=NULL;
    UInt32 cookie;

    cookie = Hwi_disable (  );

    buf = &gRemoteDebug_serverObj.printBuf[0];

    va_start(vaArgPtr, format);
    vsnprintf(buf, REMOTE_DEBUG_SERVER_PRINT_BUF_LEN, format, vaArgPtr);
    va_end(vaArgPtr);

    retVal = RemoteDebug_serverPutString(gRemoteDebug_serverObj.coreId, buf);

    Hwi_restore ( cookie );

    return (retVal);
}

int RemoteDebug_putChar(char ch)
{
    volatile RemoteDebug_CoreObj *pCoreObj;
    volatile int coreId;

    coreId = gRemoteDebug_serverObj.coreId;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    if (NULL == gRemoteDebug_coreObj)
        return 0;

    pCoreObj = &gRemoteDebug_coreObj[coreId];

    if(pCoreObj->headerTag == REMOTE_DEBUG_HEADER_TAG)
    {
      pCoreObj->clientFlags[0] = (REMOTE_DEBUG_FLAG_TYPE_CHAR | ch);
    }

    return 0;
}

int RemoteDebug_getChar(char *pChar, UInt32 timeout)
{
    volatile RemoteDebug_CoreObj *pCoreObj;
    volatile int coreId, value;

    *pChar = 0;

    coreId = gRemoteDebug_serverObj.coreId;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    if (NULL == gRemoteDebug_coreObj)
        return -1;

    pCoreObj = &gRemoteDebug_coreObj[coreId];

    while(1)
    {
        if(pCoreObj->headerTag != REMOTE_DEBUG_HEADER_TAG)
            return -1;

        value = pCoreObj->serverFlags[0];
        if(value & REMOTE_DEBUG_FLAG_TYPE_CHAR)
        {
            *pChar = value & 0xFF;
            pCoreObj->serverFlags[0] = 0;
            break;
        }
        if(timeout==BIOS_WAIT_FOREVER)
            Task_sleep(10);
        else
            return -1;
    }

    return 0;
}

int RemoteDebug_getString(char *pChar, UInt32 timeout)
{

    volatile RemoteDebug_CoreObj *pCoreObj;
    volatile int coreId, value;
    volatile int length;

    *pChar = 0;

    coreId = gRemoteDebug_serverObj.coreId;

    if(coreId>=REMOTE_DEBUG_CORE_ID_MAX)
        return -1;

    if (NULL == gRemoteDebug_coreObj)
        return -1;

    pCoreObj = &gRemoteDebug_coreObj[coreId];

    while(1)
    {
        if(pCoreObj->headerTag != REMOTE_DEBUG_HEADER_TAG)
            return -1;

        value = pCoreObj->serverFlags[0];

        if(value & REMOTE_DEBUG_FLAG_TYPE_STRING)
        {

        length = pCoreObj->serverFlags[1];

            if(length>0 && length<REMOTE_DEBUG_PARAM_BUF_SIZE)
                strncpy(pChar,(char*)pCoreObj->serverParamBuf,length+1);

            pCoreObj->serverFlags[1] = 0;
        pCoreObj->serverFlags[0] = 0;
        break;

        }
        if(timeout==BIOS_WAIT_FOREVER)
            Task_sleep(10);
        else
            return -1;
    }

    return 0;
}
