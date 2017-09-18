/** ==================================================================
 *  @file   issdrv_imx122I2c.c
 *
 *  @path   /ti/psp/devices/imx122/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/imx122/src/issdrv_imx122Priv.h>

/*
 * write to device registers */
/* ===================================================================
 *  @func     Iss_Imx122RegWrite
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
Int32 Iss_Imx122RegWrite(Iss_Imx122Obj * pObj,
                          Iss_VideoDecoderRegRdWrParams * pPrm)
{
    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *pCreateArgs;

    if (pPrm == NULL)
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if (pPrm->deviceNum > pCreateArgs->numDevicesAtPort) {
    	Vps_printf("Imx122 FVID2_EBADARGS\n");
        return FVID2_EBADARGS;
    }

    imx122_spiWrite(&pPrm->regAddr16, &pPrm->regValue16, 1);

    return status;
}

/*
 * read from device registers */
/* ===================================================================
 *  @func     Iss_Imx122RegRead
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
Int32 Iss_Imx122RegRead(Iss_Imx122Obj * pObj,
                         Iss_VideoDecoderRegRdWrParams * pPrm)
{
    Int32 status = FVID2_SOK;

    Iss_SensorCreateParams *pCreateArgs;

    if (pPrm == NULL)
        return FVID2_EBADARGS;

    if (pPrm->regAddr8 == NULL || /*pPrm->regValue8 == NULL || */ pPrm->numRegs == 0)
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if (pPrm->deviceNum > pCreateArgs->numDevicesAtPort)
        return FVID2_EBADARGS;

	imx122_spiRead(&pPrm->regAddr8, &pPrm->regValue8, 1);
    //memset(pPrm->regValue8, 0, pPrm->numRegs);

    return status;

}
