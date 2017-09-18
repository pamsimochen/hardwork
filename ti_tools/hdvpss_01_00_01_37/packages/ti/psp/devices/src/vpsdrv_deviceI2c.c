/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "vpsdrv_devicePriv.h"

Vps_DeviceObj gVps_deviceObj;

Int32 Vps_deviceRead8 (UInt32 instId, UInt32 devAddr,
                       const UInt8 * regAddr,
                       UInt8 * regValue, UInt32 numRegs)
{
    Int32 status;
    UInt32 regId;
    PSP_I2cXferParams i2cParams;

    if(regAddr==NULL||regValue==NULL||numRegs==0)
    {
        if(gVps_deviceObj.i2cDebugEnable)
        {
            Vps_printf(" I2C%d: DEV 0x%02x: Illegal Params ... ERROR !!!\n",
                instId,
                devAddr
            );
        }
        return FVID2_EFAIL;
    }

    status = Vps_deviceI2cLock(instId, TRUE);

    if(status==FVID2_SOK)
    {
        i2cParams.slaveAddr = devAddr;
        i2cParams.bufLen    = 1;
        i2cParams.timeout   = VPS_DEVICE_I2C_TIMEOUT;

        for(regId=0; regId<numRegs; regId++)
        {
            i2cParams.buffer    = (UInt8*)&regAddr[regId];
            i2cParams.flags
                    = (I2C_WRITE | I2C_MASTER | I2C_START /* | I2C_STOP */ );

            status = PSP_i2cTransfer(
                        gVps_deviceObj.i2cHndl[instId],
                        &i2cParams
                        );

            if(status!=FVID2_SOK)
            {
                if(gVps_deviceObj.i2cDebugEnable)
                {
                    Vps_printf(" I2C%d: DEV 0x%02x: RD 0x%02x ... ERROR !!!\n",
                        instId,
                        devAddr,
                        regAddr[regId]
                    );
                }
                break;
            }

            i2cParams.buffer    = &regValue[regId];
            i2cParams.flags
                    = (I2C_READ | I2C_MASTER | I2C_START | I2C_STOP | I2C_IGNORE_BUS_BUSY);

            status = PSP_i2cTransfer(
                        gVps_deviceObj.i2cHndl[instId],
                        &i2cParams
                        );

            if(status!=FVID2_SOK)
            {
                if(gVps_deviceObj.i2cDebugEnable)
                {
                    Vps_printf(" I2C%d: DEV 0x%02x: RD 0x%02x ... ERROR !!!\n",
                        instId,
                        devAddr,
                        regAddr[regId]
                    );
                }
                break;
            }

            if(gVps_deviceObj.i2cDebugEnable)
            {
                Vps_printf(" I2C%d: DEV 0x%02x: RD 0x%02x = 0x%02x \n",
                    instId,
                    devAddr,
                    regAddr[regId],
                    regValue[regId]
                );
            }
        }

        Vps_deviceI2cLock(instId, FALSE);
    }

    return status;
}

Int32 Vps_deviceRawRead8 (UInt32 instId,
                          UInt32 devAddr,
                          UInt8 *regValue,
                          UInt32 numRegs)
{
    Int32 status, regId;
    PSP_I2cXferParams i2cParams;

    if((NULL == regValue) || (0 == numRegs))
    {
        if(gVps_deviceObj.i2cDebugEnable)
        {
            Vps_printf(" I2C%d: DEV 0x%02x: Illegal Params ... ERROR !!!\n",
                instId,
                devAddr
            );
        }
        return FVID2_EFAIL;
    }

    status = Vps_deviceI2cLock(instId, TRUE);

    if(FVID2_SOK == status)
    {
        i2cParams.slaveAddr = devAddr;
        i2cParams.bufLen    = numRegs;
        i2cParams.timeout   = VPS_DEVICE_I2C_TIMEOUT;
        i2cParams.buffer    = (UInt8 *)regValue;
        i2cParams.flags     = (I2C_DEFAULT_READ);
        status = PSP_i2cTransfer(
                        gVps_deviceObj.i2cHndl[instId],
                        &i2cParams);

        if(gVps_deviceObj.i2cDebugEnable)
        {
            if(FVID2_SOK != status)
            {
                Vps_printf(" I2C%d: DEV 0x%02x: RD ERROR !!!\n",
                    instId,
                    devAddr);
            }
            else
            {
                for(regId=0; regId<numRegs; regId++)
                {
                    Vps_printf(" I2C%d: DEV 0x%02x: RD 0x%02x = 0x%02x \n",
                        instId,
                        devAddr,
                        0x8 + regId,
                        regValue[regId]
                    );


                }
            }
        }
    }

    Vps_deviceI2cLock(instId, FALSE);

    return status;
}

Int32 Vps_deviceWrite8 (UInt32 instId, UInt32 devAddr,
                        const UInt8 * regAddr,
                        const UInt8 * regValue, UInt32 numRegs)
{
    Int32 status;
    UInt32 regId;
    PSP_I2cXferParams i2cParams;
    UInt8 buffer[2];

    if(regAddr==NULL||regValue==NULL||numRegs==0)
    {
        if(gVps_deviceObj.i2cDebugEnable)
        {
            Vps_printf(" I2C%d: DEV 0x%02x: Illegal Params ... ERROR !!!\n",
                instId,
                devAddr
            );
        }
        return FVID2_EFAIL;
    }

    status = Vps_deviceI2cLock(instId, TRUE);

    if(status==FVID2_SOK)
    {
        i2cParams.slaveAddr = devAddr;
        i2cParams.buffer    = buffer;
        i2cParams.bufLen    = 2;
        i2cParams.flags     = I2C_DEFAULT_WRITE;
        i2cParams.timeout   = VPS_DEVICE_I2C_TIMEOUT;

        for(regId=0; regId<numRegs; regId++)
        {
            buffer[0] = regAddr[regId];
            buffer[1] = regValue[regId];

            status = PSP_i2cTransfer(
                        gVps_deviceObj.i2cHndl[instId],
                        &i2cParams
                        );

            if(status!=FVID2_SOK)
            {
                if(gVps_deviceObj.i2cDebugEnable)
                {
                    Vps_printf(" I2C%d: DEV 0x%02x: WR 0x%02x = 0x%02x ... ERROR !!! \n",
                        instId,
                        devAddr,
                        regAddr[regId],
                        regValue[regId]
                    );
                }
                break;
            }

            if(gVps_deviceObj.i2cDebugEnable)
            {
                Vps_printf(" I2C%d: DEV 0x%02x: WR 0x%02x = 0x%02x \n",
                    instId,
                    devAddr,
                    regAddr[regId],
                    regValue[regId]
                );
            }
        }

        Vps_deviceI2cLock(instId, FALSE);
    }

    return status;
}

Int32 Vps_deviceRawWrite8 (UInt32 instId,
                           UInt32 devAddr,
                           UInt8 *regValue,
                           UInt32 numRegs)
{
    Int32 status;
    PSP_I2cXferParams i2cParams;

    if((NULL == regValue) || (0u == numRegs))
    {
        if(gVps_deviceObj.i2cDebugEnable)
        {
            Vps_printf("I2C%d: DEV 0x%02x: Illegal Params ... ERROR !!!\n",
                instId,
                devAddr);
        }
        return FVID2_EFAIL;
    }

    status = Vps_deviceI2cLock(instId, TRUE);

    if(FVID2_SOK == status)
    {
        i2cParams.slaveAddr = devAddr;
        i2cParams.buffer    = regValue;
        i2cParams.bufLen    = numRegs;
        i2cParams.flags     = I2C_DEFAULT_WRITE;
        i2cParams.timeout   = VPS_DEVICE_I2C_TIMEOUT;

        status = PSP_i2cTransfer(
                    gVps_deviceObj.i2cHndl[instId],
                    &i2cParams);

        if(FVID2_SOK != status)
        {
            if(gVps_deviceObj.i2cDebugEnable)
            {
                Vps_printf(" I2C%d: DEV 0x%02x: ERROR !!! \n",
                    instId,
                    devAddr);
            }
        }

        if(gVps_deviceObj.i2cDebugEnable)
        {
            Vps_printf(" I2C%d: DEV 0x%02x: \n",
                instId,
                devAddr);
        }
    }
    Vps_deviceI2cLock(instId, FALSE);

    return status;
}

Int32 Vps_deviceI2cDebugEnable(UInt32 enable)
{
    gVps_deviceObj.i2cDebugEnable = enable;

    return FVID2_SOK;
}

Int32 Vps_deviceI2cLock(UInt32 instId, Bool lock)
{
    if(instId>=VPS_DEVICE_I2C_INST_ID_MAX)
        return FVID2_EFAIL;

    if(    gVps_deviceObj.i2cHndl[instId]==NULL
        || gVps_deviceObj.i2cHndlLock[instId]==NULL
      )
    {
        if(gVps_deviceObj.i2cDebugEnable)
        {
            Vps_printf(" I2C%d: Instance NOT enabled during init ... ERROR !!! \n",
                instId
            );
        }
        return FVID2_EFAIL;
    }

    if(lock)
    {
        Semaphore_pend (
             gVps_deviceObj.i2cHndlLock[instId], BIOS_WAIT_FOREVER
            );
    }
    else
    {
        Semaphore_post ( gVps_deviceObj.i2cHndlLock[instId]);
    }

    return FVID2_SOK;
}

Int32 Vps_deviceI2cInit(Vps_DeviceInitParams *pPrm)
{
    UInt16 i2cId;
    PSP_I2cInitParams i2cInitParams[VPS_DEVICE_I2C_INST_ID_MAX];
    Semaphore_Params semParams;
    Int32 status=FVID2_SOK;

    Vps_deviceI2cDebugEnable(FALSE);

    for(i2cId=0; i2cId<VPS_DEVICE_I2C_INST_ID_MAX; i2cId++ )
    {
        gVps_deviceObj.i2cHndl[i2cId] = NULL;
        gVps_deviceObj.i2cHndlLock[i2cId] = NULL;

        i2cInitParams[i2cId].opMode = I2C_OPMODE_INTERRUPT;
        i2cInitParams[i2cId].isMasterMode = TRUE;
        i2cInitParams[i2cId].is10BitAddr = FALSE;
        i2cInitParams[i2cId].i2cBusFreq = 400; /* Default, in Khz    */
        i2cInitParams[i2cId].i2cOwnAddr = 0;
        i2cInitParams[i2cId].i2cRegs = pPrm->i2cRegs[i2cId];
        i2cInitParams[i2cId].i2cIntNum = pPrm->i2cIntNum[i2cId];

        if(pPrm->i2cClkKHz[i2cId]!=VPS_DEVICE_I2C_INST_NOT_USED)
        {
            i2cInitParams[i2cId].i2cBusFreq = pPrm->i2cClkKHz[i2cId];
        }

        status |= PSP_i2cInit(i2cId, &i2cInitParams[i2cId]);
    }

    if(status==FVID2_SOK)
    {
        for(i2cId=0; i2cId<VPS_DEVICE_I2C_INST_ID_MAX; i2cId++ )
        {
            if(pPrm->i2cClkKHz[i2cId]!=VPS_DEVICE_I2C_INST_NOT_USED)
            {
                gVps_deviceObj.i2cHndl[i2cId] =
                    PSP_i2cCreate(i2cId, NULL, NULL);

                if(gVps_deviceObj.i2cHndl[i2cId]==NULL)
                {
                    status = FVID2_EFAIL;
                    Vps_printf ("%s:PSP_i2cCreate failed at line %d:\r\n",__FUNCTION__,__LINE__);
                    break;
                }

                Semaphore_Params_init ( &semParams );

                semParams.mode = Semaphore_Mode_BINARY;

                gVps_deviceObj.i2cHndlLock[i2cId]
                    = Semaphore_create ( 1u, &semParams, NULL );

                if(gVps_deviceObj.i2cHndlLock[i2cId]==NULL)
                {
                    PSP_i2cDelete(gVps_deviceObj.i2cHndl[i2cId]);
                    status = FVID2_EFAIL;
                    break;
                }
            }
        }
    }

    return status;
}

Int32 Vps_deviceI2cDeInit(void)
{
    UInt16 i2cId;

    for(i2cId=0; i2cId<VPS_DEVICE_I2C_INST_ID_MAX; i2cId++ )
    {
        if(gVps_deviceObj.i2cHndl[i2cId]!=NULL)
        {
            PSP_i2cDelete(gVps_deviceObj.i2cHndl[i2cId]);
            Semaphore_delete ( &gVps_deviceObj.i2cHndlLock[i2cId] );
            gVps_deviceObj.i2cHndl[i2cId] = NULL;
        }
    }

    PSP_i2cDeinit(NULL);

    return FVID2_SOK;
}

Int32 Vps_deviceI2cProbeAll(UInt16 i2cInstId)
{
    UInt8 slaveAddr;

    /*
     * PSP_i2cCreate should have been called for this instance
     * for its proper initialization.
     */
    if(gVps_deviceObj.i2cHndl[i2cInstId] != NULL)
    {
        for (slaveAddr = 0; slaveAddr < 128; slaveAddr++)
            {
            if (PSP_i2cProbe(i2cInstId, slaveAddr) == 0)
                {
                Vps_printf (" I2C%d: Passed for address 0x%x !!! \r\n", i2cInstId, slaveAddr);
                }
            }
    }

    return FVID2_SOK;
}

Int32 Vps_deviceI2cProbeDevice(UInt16 i2cInstId, UInt8 slaveAddr)
{
    /*
     * PSP_i2cCreate should have been called for this instance
     * for its proper initialization.
     */
    if(gVps_deviceObj.i2cHndl[i2cInstId] != NULL)
    {
            if (PSP_i2cProbe(i2cInstId, slaveAddr) == 0)
                {
                    return FVID2_SOK;
                }
    }

    return FVID2_EFAIL;
}


