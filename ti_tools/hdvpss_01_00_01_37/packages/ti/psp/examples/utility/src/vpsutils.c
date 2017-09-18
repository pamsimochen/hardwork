/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include <ti/psp/examples/utility/vpsutils.h>
#include <stdio.h>

//#define ENABLE_REMOTE_GET_CHAR

int RemoteDebug_getChar(char *pChar, UInt32 timeout);
int RemoteDebug_putChar(char ch);
int RemoteDebug_getString(char *pChar, UInt32 timeout);

int VpsUtils_getChar(char *pChar, UInt32 timeout)
{
    int status=0;

    if(timeout==BIOS_WAIT_FOREVER)
    {
        #ifdef ENABLE_REMOTE_GET_CHAR
        status = RemoteDebug_getChar(pChar, timeout);
        #else
        *pChar = getchar();
        fflush(stdin);
        #endif

        return status;
    }

    status = RemoteDebug_getChar(pChar, BIOS_NO_WAIT);

    return status;
}

int VpsUtils_getString(char *pChar, UInt32 timeout)
{
    int status=0;

    if(timeout==BIOS_WAIT_FOREVER)
    {
        #ifdef ENABLE_REMOTE_GET_CHAR
        status = RemoteDebug_getString(pChar, timeout);
        #else
        gets(pChar);
        fflush(stdin);
        #endif

        return status;
    }

    status = RemoteDebug_getString(pChar, BIOS_NO_WAIT);

    return status;
}

int VpsUtils_remoteSendChar(char ch)
{
    int status=0;

    status = RemoteDebug_putChar(ch);

    return status;
}

int VpsUtils_setL3Pri(UInt32 initPressure0, UInt32 initPressure1)
{
    volatile UInt32 *pAddr[2];

    pAddr[0] = (volatile UInt32 *)0x48140608;
    pAddr[1] = (volatile UInt32 *)0x4814060C;

    *pAddr[0] = initPressure0;
    *pAddr[1] = initPressure1;

    return 0;
}

static char xtod(char c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='A' && c<='F') return c-'A'+10;
  if (c>='a' && c<='f') return c-'a'+10;
  return c=0;        // not Hex digit
}

static int HextoDec(char *hex, int l)
{
  if (*hex==0)
    return(l);

  return HextoDec(hex+1, l*16+xtod(*hex)); // hex+1?
}

int xstrtoi(char *hex)      // hex string to integer
{
  return HextoDec(hex,0);
}
