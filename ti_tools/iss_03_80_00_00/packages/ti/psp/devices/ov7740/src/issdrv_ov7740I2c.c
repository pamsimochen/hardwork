/** ==================================================================
 *  @file   issdrv_ov7740I2c.c                                                 
 *                                                                    
 *  @path   /ti/psp/devices/ov7740/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/ov7740/src/issdrv_ov7740Priv.h>

Int32 Iss_Ov7740I2cRead8 ( Iss_Ov7740Obj * pObj,
                        UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs )
{
    Int32 status = FVID2_SOK;

    #ifdef ISS_OV7740_USE_REG_CACHE
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
    status = Iss_deviceRead8(i2cInstId, i2cDevAddr, regAddr, regValue, numRegs);

    if(status!=FVID2_SOK)
        return status;
    #endif

    return status;
}

Int32 Iss_Ov7740I2cWrite8 ( Iss_Ov7740Obj * pObj,
                        UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs )
{
    Int32 status = FVID2_SOK;

    #ifdef ISS_OV7740_USE_REG_CACHE
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

    status = Iss_deviceWrite8(i2cInstId, i2cDevAddr,
                              regAddr, regValue, numRegs);

    if(status!=FVID2_SOK)
        return status;

    return status;
}

/* ===================================================================
 *  @func     Iss_Ov7740ResetRegCache                                               
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
Int32 Iss_Ov7740ResetRegCache(Iss_Ov7740Obj * pObj)
{
#ifdef ISS_OV7740_USE_REG_CACHE

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

/* ===================================================================
 *  @func     Iss_Ov7740UpdateRegCache                                               
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
Int32 Iss_Ov7740UpdateRegCache(Iss_Ov7740Obj * pObj)
{
    Int32 status = FVID2_SOK;

    #ifdef ISS_OV7740_USE_REG_CACHE

    Iss_SensorCreateParams *pCreateArgs;

    UInt8 numRegs = 0xf8;

    UInt8 devAddr[2];

    UInt8 regAddr[8];

    UInt8 regValue[8];

    pCreateArgs = &pObj->createArgs;

    devAddr[0] = pCreateArgs->deviceI2cAddr[0];
    devAddr[1] = pCreateArgs->deviceI2cAddr[0] + ISS_OV7740_I2C_PORT1_OFFSET;

    Iss_Ov7740ResetRegCache(pObj);

    regAddr[0] = 0;
    regAddr[1] = 1;

    // dummy read to make next read io work
    status = Iss_deviceRead8(pCreateArgs->deviceI2cInstId,
                             devAddr[0], regAddr, regValue, 2);
    if(status!=FVID2_SOK)
        return status;

    status = Iss_deviceRawRead8(pCreateArgs->deviceI2cInstId,
                                devAddr[0], &pObj->regCache[0][8], numRegs);
    if(status!=FVID2_SOK)
        return status;

    status = Iss_deviceRawRead8(pCreateArgs->deviceI2cInstId,
                                devAddr[1], &pObj->regCache[1][8], numRegs);

    if(status!=FVID2_SOK)
        return status;

    #endif

    return status;
}

/* 
 * This API - Disable output - Power-ON of all module - Manual reset of
 * OV7740 and then setup in auto reset mode - Select default TMDS core */
/* ===================================================================
 *  @func     Iss_Ov7740Reset                                               
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
Int32 Iss_Ov7740Reset ( Iss_Ov7740Obj * pObj )
{
    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *pCreateArgs;

    UInt8 regAddr[8];

    UInt8 regValue[8];

    UInt8 numRegs;

    UInt8 devAddr;

    Iss_Ov7740ResetRegCache(pObj);

    /*
     * disable outputs
     */
    status = Iss_Ov7740Stop ( pObj );
    if ( status != FVID2_SOK )
        return status;

    /*
     * Normal operation
     */
    status = Iss_Ov7740PowerDown ( pObj, FALSE );
    if ( status != FVID2_SOK )
        return status;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;

    regAddr[numRegs] = ISS_OV7740_REG_SW_RST_0;
    regValue[numRegs] = 0x2F;            /* reset AAC,
                                                            * HDCP, ACR,
                                                            * audio FIFO, SW */
    numRegs++;

    status = Iss_Ov7740I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
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

    regAddr[numRegs] = ISS_OV7740_REG_SW_RST_0;
    regValue[numRegs] = 0x10;   /* auto-reset SW */
    numRegs++;

    status = Iss_Ov7740I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    status = Iss_Ov7740UpdateRegCache(pObj);
    if ( status != FVID2_SOK )
        return status;

    return status;
}

/* 
 * Power-ON All modules */
/* ===================================================================
 *  @func     Iss_Ov7740PowerDown                                               
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
Int32 Iss_Ov7740PowerDown ( Iss_Ov7740Obj * pObj, UInt32 powerDown )
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
    regAddr[numRegs] = ISS_OV7740_REG_SYS_CTRL_1;
    regValue[numRegs] = 0;
    numRegs++;

    status = Iss_Ov7740I2cRead8 ( pObj,
                                pCreateArgs->deviceI2cInstId,
                               devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    if ( powerDown )
        regValue[0] = 0x04;  /* power down  */
    else
        regValue[0] = 0x05;                               /* normal
                                                            * operation */

    status = Iss_Ov7740I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/* 
 * Enable output port */
/* ===================================================================
 *  @func     Iss_Ov7740OutputEnable                                               
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
Int32 Iss_Ov7740OutputEnable ( Iss_Ov7740Obj * pObj, UInt32 enable )
{
    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *pCreateArgs;

    UInt8 regAddr[8];

    UInt8 regValue[8];

    UInt8 numRegs;

    UInt8 devAddr;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0] + ISS_OV7740_I2C_PORT1_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = ISS_OV7740_REG_SYS_PWR_DWN_2;
    regValue[numRegs] = 0xC3;                               /* Enable all
                                                            * expect outputs */
    if ( enable )
        regValue[numRegs] |= ( 1 << 2 );    /* Enable outputs  */

    regAddr[numRegs] = ISS_OV7740_REG_SYS_PWR_DWN;
    regValue[numRegs] = 0xAD;                              /* Enable all
                                                            * expect outputs */
    if ( enable )
        regValue[numRegs] |= ( 1 << 6 );    /* Enable outputs  */

    numRegs++;

    status = Iss_Ov7740I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/* 
 * Stop output port */
/* ===================================================================
 *  @func     Iss_Ov7740Stop                                               
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
Int32 Iss_Ov7740Stop ( Iss_Ov7740Obj * pObj )
{
    Int32 status = FVID2_SOK;

 //   status = Iss_Ov7740OutputEnable ( pObj, FALSE );

    return status;
}

/* 
 * Start output port */
/* ===================================================================
 *  @func     Iss_Ov7740Start                                               
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
Int32 Iss_Ov7740Start ( Iss_Ov7740Obj * pObj )
{
    Int32 status = FVID2_SOK;
        
    return status;
}

/* 
 * Get Chip ID and revision ID */
/* ===================================================================
 *  @func     Iss_Ov7740GetChipId                                               
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
Int32 Iss_Ov7740GetChipId ( Iss_Ov7740Obj * pObj,
                             Iss_SensorChipIdParams * pPrm,
                             Iss_SensorChipIdStatus * pStatus )
{
    Int32 status = FVID2_SOK;

    return status;
}

/* 
 * write to device registers */
/* ===================================================================
 *  @func     Iss_Ov7740RegWrite                                               
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
Int32 Iss_Ov7740RegWrite ( Iss_Ov7740Obj * pObj,
                            Iss_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = FVID2_SOK;

    return status;
}

/* 
 * read from device registers */
/* ===================================================================
 *  @func     Iss_Ov7740RegRead                                               
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
Int32 Iss_Ov7740RegRead ( Iss_Ov7740Obj * pObj,
                           Iss_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = FVID2_SOK;

    return status;
}
