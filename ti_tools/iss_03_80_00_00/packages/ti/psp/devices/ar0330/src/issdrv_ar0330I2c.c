/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/ar0330/src/issdrv_ar0330Priv.h>

Int32 Iss_Ar0330I2cRead8 ( Iss_Ar0330Obj * pObj,
                        UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs )
{
    Int32 status = FVID2_SOK;

    #ifdef ISS_AR0330_USE_REG_CACHE
    UInt32 reg, port;

    if(pObj->createArgs.deviceI2cAddr[0]==i2cDevAddr)
        port = 0;
    else
        port = 1;

    for(reg=0; reg<numRegs; reg++)
    {
        regValue[reg] = pObj->regCache[port][regAddr[reg]];
    }
    #else
    status = Iss_deviceRead8(
                    i2cInstId, i2cDevAddr,
                    regAddr, regValue,
                    numRegs
                    );

    if(status!=FVID2_SOK)
        return status;
    #endif

    return status;
}

Int32 Iss_Ar0330I2cWrite8 ( Iss_Ar0330Obj * pObj,
                        UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs )
{
    Int32 status = FVID2_SOK;

    #ifdef ISS_AR0330_USE_REG_CACHE
    UInt32 reg, port;

    if(pObj->createArgs.deviceI2cAddr[0]==i2cDevAddr)
        port = 0;
    else
        port = 1;

    for(reg=0; reg<numRegs; reg++)
    {
        pObj->regCache[port][regAddr[reg]] = regValue[reg];
    }
    #endif

    status = Iss_deviceWrite8(
                    i2cInstId, i2cDevAddr,
                    regAddr, regValue,
                    numRegs
                    );

    if(status!=FVID2_SOK)
        return status;

    return status;
}

Int32 Iss_Ar0330ResetRegCache(Iss_Ar0330Obj * pObj)
{
    #ifdef ISS_AR0330_USE_REG_CACHE

    memset(pObj->regCache, 0, sizeof(pObj->regCache));

    // for now hard code default values in registers 0x0 to 0x7
    pObj->regCache[0][0] = 0x01;
    pObj->regCache[0][1] = 0x00;
    pObj->regCache[0][2] = 0x35;
    pObj->regCache[0][3] = 0x91;
    pObj->regCache[0][4] = 0x03;
    pObj->regCache[0][5] = 0x00;
    pObj->regCache[0][6] = 0x1F; // assume signal is detected
    pObj->regCache[0][7] = 0x00;

    #endif

    return FVID2_SOK;
}

Int32 Iss_Ar0330UpdateRegCache(Iss_Ar0330Obj * pObj)
{
    Int32 status = FVID2_SOK;

    #ifdef ISS_AR0330_USE_REG_CACHE

    Iss_SensorCreateParams *pCreateArgs;
    UInt8 numRegs = 0xf8;
    UInt8 devAddr[2];
    UInt8 regAddr[8];
    UInt8 regValue[8];

    pCreateArgs = &pObj->createArgs;

    devAddr[0] = pCreateArgs->deviceI2cAddr[0];
    devAddr[1] = pCreateArgs->deviceI2cAddr[0] + ISS_AR0330_I2C_PORT1_OFFSET;

    Iss_Ar0330ResetRegCache(pObj);

    regAddr[0] = 0;
    regAddr[1] = 1;

    // dummy read to make next read io work
    status = Iss_deviceRead8(
                pCreateArgs->deviceI2cInstId,
                devAddr[0],
                regAddr,
                regValue,
                2
                );
    if(status!=FVID2_SOK)
        return status;

    status = Iss_deviceRawRead8(
                pCreateArgs->deviceI2cInstId,
                devAddr[0],
                &pObj->regCache[0][8],
                numRegs
                );
    if(status!=FVID2_SOK)
        return status;

    status = Iss_deviceRawRead8(
                pCreateArgs->deviceI2cInstId,
                devAddr[1],
                &pObj->regCache[1][8],
                numRegs
                );

    if(status!=FVID2_SOK)
        return status;

    #endif

    return status;
}





/*
  This API
  - Disable output
  - Power-ON of all module
  - Manual reset of AR0330 and then setup in auto reset mode
  - Select default TMDS core
*/
Int32 Iss_Ar0330Reset ( Iss_Ar0330Obj * pObj )
{
    Int32 status = FVID2_SOK;
    Iss_SensorCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    Iss_Ar0330ResetRegCache(pObj);

    /*
     * disable outputs
     */
    status = Iss_Ar0330Stop ( pObj );
    if ( status != FVID2_SOK )
        return status;

    /*
     * Normal operation
     */
    status = Iss_Ar0330PowerDown ( pObj, FALSE );
    if ( status != FVID2_SOK )
        return status;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;

    regAddr[numRegs] = ISS_AR0330_REG_SW_RST_0;
    regValue[numRegs] = 0x2F;   /* reset AAC, HDCP, ACR, audio FIFO, SW */
    numRegs++;

    status = Iss_Ar0330I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    /*
     * wait for reset to be effective
     */
    Task_sleep ( 50 );

    /*
     * remove reset and setup in auto-reset mode
     */
    numRegs = 0;

    regAddr[numRegs] = ISS_AR0330_REG_SW_RST_0;
    regValue[numRegs] = 0x10;   /* auto-reset SW */
    numRegs++;

    status = Iss_Ar0330I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;


    status = Iss_Ar0330UpdateRegCache(pObj);
    if ( status != FVID2_SOK )
        return status;

    return status;
}

/*
  Power-ON All modules
*/
Int32 Iss_Ar0330PowerDown ( Iss_Ar0330Obj * pObj, UInt32 powerDown )
{
    Int32 status = FVID2_SOK;
    Iss_SensorCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;
    regAddr[numRegs] = ISS_AR0330_REG_SYS_CTRL_1;
    regValue[numRegs] = 0;
    numRegs++;

    status = Iss_Ar0330I2cRead8 ( pObj,
                                pCreateArgs->deviceI2cInstId,
                               devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    if ( powerDown )
        regValue[0] = 0x04;  /* power down  */
    else
        regValue[0] = 0x05;  /* normal operation */

    status = Iss_Ar0330I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Enable output port
*/
Int32 Iss_Ar0330OutputEnable ( Iss_Ar0330Obj * pObj, UInt32 enable )
{
    Int32 status = FVID2_SOK;
    Iss_SensorCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0] + ISS_AR0330_I2C_PORT1_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = ISS_AR0330_REG_SYS_PWR_DWN_2;
    regValue[numRegs] = 0xC3;   /* Enable all expect outputs  */
    if ( enable )
        regValue[numRegs] |= ( 1 << 2 );    /* Enable outputs  */

    regAddr[numRegs] = ISS_AR0330_REG_SYS_PWR_DWN;
    regValue[numRegs] = 0xAD;   /* Enable all expect outputs  */
    if ( enable )
        regValue[numRegs] |= ( 1 << 6 );    /* Enable outputs  */

    numRegs++;

    status = Iss_Ar0330I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Stop output port
*/
Int32 Iss_Ar0330Stop ( Iss_Ar0330Obj * pObj )
{
    Int32 status = FVID2_SOK;

 //   status = Iss_Ar0330OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Start output port
*/
Int32 Iss_Ar0330Start ( Iss_Ar0330Obj * pObj )
{
    Int32 status = FVID2_SOK;
   
    return status;
}

/*
  Get Chip ID and revision ID
*/
Int32 Iss_Ar0330GetChipId ( Iss_Ar0330Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus )
{
    Int32 status = FVID2_SOK;

    return status;
}


/*
  write to device registers
*/
Int32 Iss_Ar0330RegWrite ( Iss_Ar0330Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = FVID2_SOK;

    return status;
}

/*
  read from device registers
*/
Int32 Iss_Ar0330RegRead ( Iss_Ar0330Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = FVID2_SOK;

    return status;
}
