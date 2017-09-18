/** ==================================================================
 *  @file   issdrv_mn34041I2c.c                                                  
 *                                                                    
 *  @path   /ti/psp/devices/mn34041/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/mn34041/src/issdrv_mn34041Priv.h>

/* 
 * write to device registers */
/* ===================================================================
 *  @func     Iss_Mn34041RegWrite                                               
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
Int32 Iss_Mn34041RegWrite(Iss_Mn34041Obj * pObj,
                          Iss_VideoDecoderRegRdWrParams * pPrm)
{
    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *pCreateArgs;

    if (pPrm == NULL)
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if (pPrm->deviceNum > pCreateArgs->numDevicesAtPort)
        return FVID2_EBADARGS;

    mn34_041_setgio(pPrm->regAddr[0], pPrm->regValue8[0]);
#if 0
    status = Iss_Mt9j003I2cWrite8(pObj, pCreateArgs->deviceI2cInstId,
                                  pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                                  pPrm->regAddr, pPrm->regValue8,
                                  pPrm->numRegs);
#endif

    return status;
}

/* 
 * read from device registers */
/* ===================================================================
 *  @func     Iss_Mn34041RegRead                                               
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
Int32 Iss_Mn34041RegRead(Iss_Mn34041Obj * pObj,
                         Iss_VideoDecoderRegRdWrParams * pPrm)
{
    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *pCreateArgs;

    if (pPrm == NULL)
        return FVID2_EBADARGS;

    if (pPrm->regAddr == NULL || pPrm->regValue8 == NULL || pPrm->numRegs == 0)
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if (pPrm->deviceNum > pCreateArgs->numDevicesAtPort)
        return FVID2_EBADARGS;

    memset(pPrm->regValue8, 0, pPrm->numRegs);
#if 0
    status = Iss_Mt9j003I2cRead8(pObj,
                                 pCreateArgs->deviceI2cInstId,
                                 pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                                 pPrm->regAddr, pPrm->regValue8, pPrm->numRegs);
#endif

    return status;

}
