/** ==================================================================
 *  @file   psp_i2cdrv.c                                                  
 *                                                                    
 *  @path   /ti/psp/i2c/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file psp_i2cdrv.c
 *
 *  \brief I2C driver
 *  This file implements I2C driver functionality.
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <xdc/std.h>
#include <stdio.h>

#include <ti/psp/vps/common/trace.h>

#include <ti/psp/i2c/src/psp_i2cPriv.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */
#define I2C_READ_REG_BYTE(reg)          (*(volatile unsigned char *)(reg))
#define I2C_READ_REG_WORD(reg)          (*(volatile unsigned short *)(reg))
#define I2C_READ_REG_LONG(reg)          (*(volatile unsigned int *)(reg))

#define I2C_WRITE_REG_BYTE(val, reg)    (*(volatile unsigned char *)(reg) = (val))
#define I2C_WRITE_REG_WORD(val, reg)    (*(volatile unsigned short *)(reg) = (val))
#define I2C_WRITE_REG_LONG(val, reg)    (*(volatile unsigned int *)(reg) = (val))

#define I2C_STANDARD   100000
#define I2C_FAST_MODE  400000
#define I2C_HIGH_SPEED 3400000

/* I2C functional clock is 48MHz */
#define I2C_FUNC_CLOCK  (48000000)

#define I2C_FASTSPEED_SCLL_TRIM     (7)
#define I2C_FASTSPEED_SCLH_TRIM     (5)

#define I2C_DELAY_BIG   (10000)
#define I2C_DELAY_MED   (10000)
#define I2C_DELAY_SMALL ( 1000)

#define I2C_ERR_TIMEDOUT                (-100)
#define I2C_ERR_INVALID_PARAMS          (-101)
#define I2C_ERR_IO                      (-102)

#undef I2C_DEBUG

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/* ========================================================================== 
 */
/* Function Declarations */
/* ========================================================================== 
 */
/* ===================================================================
 *  @func     wait_for_bb                                               
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
static int wait_for_bb(UInt32 instanceId);

/* ===================================================================
 *  @func     flush_fifo                                               
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
static void flush_fifo(UInt32 instanceId);

/* ===================================================================
 *  @func     wait_for_pin                                               
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
static UInt16 wait_for_pin(UInt32 instanceId);

/* ===================================================================
 *  @func     Iss_i2cTransferMsgPoll                                               
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
static Int32 Iss_i2cTransferMsgPoll(PSP_I2cDriverObject * i2cDrvObj,
                                    PSP_I2cXferParams * i2cXferParams);
/* ===================================================================
 *  @func     Iss_i2cTransferMsgInt                                               
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
static Int32 Iss_i2cTransferMsgInt(PSP_I2cDriverObject * i2cDrvObj,
                                   PSP_I2cXferParams * i2cXferParams);
/* ===================================================================
 *  @func     i2c_isr                                               
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
static void i2c_isr(UArg arg);

/* ===================================================================
 *  @func     udelay                                               
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
static void udelay(int delay_usec);

/* ===================================================================
 *  @func     i2c_init                                               
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
static int i2c_init(UInt32 instanceId);

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

/**
 * I2C Instance Object, one per I2C controller.
 */
PSP_I2cInstObj i2CInstObj[VPS_DEVICE_I2C_INST_ID_MAX] = {
    I2C_INSTANCE_OBJ_DEFAULT,
    I2C_INSTANCE_OBJ_DEFAULT
};

/**
 * I2C Driver object, one per i2cCreate.
 */
PSP_I2cDriverObject i2CDriverObj[VPS_DEVICE_I2C_INST_ID_MAX][I2C_NUM_OPENS];

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

/**
 *  \brief Initialize I2C Controller
 */
/* ===================================================================
 *  @func     PSP_i2cInit                                               
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
Int32 PSP_i2cInit(UInt32 numInstance, PSP_I2cInitParams * initParams)
{
    Int32 retVal = FVID2_SOK;

    PSP_I2cInstObj *instObj = NULL;

    Semaphore_Params semPrms;

    static UInt32 resetFlag = 0;

    /* Init all global variables to zero */
    if (!resetFlag)
    {
        memset(i2CInstObj, 0u, sizeof(i2CInstObj));
        memset(i2CDriverObj, 0u, sizeof(i2CDriverObj));

        resetFlag = 1;
    }

    /* Validate arguments */
    if ((numInstance > VPS_DEVICE_I2C_INST_ID_MAX) || (NULL == initParams))
    {
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Validate initParams, leave bool ones */
        if ((initParams->i2cBusFreq < I2C_MIN_BUS_FREQ)
            || (initParams->i2cBusFreq > I2C_MAX_BUS_FREQ))
        {
            retVal = FVID2_EBADARGS;
        }

        /* DMA mode is currently NOT supported */
        if ((FVID2_SOK == retVal) && (initParams->opMode == I2C_OPMODE_DMA))
        {
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        instObj = &i2CInstObj[numInstance];

        instObj->isMasterMode = initParams->isMasterMode;
        instObj->opMode = (PSP_I2cOpMode) initParams->opMode;
        instObj->i2cBusFreq = initParams->i2cBusFreq;
        instObj->is10BitAddr = initParams->is10BitAddr;
        instObj->i2cOwnAddr = initParams->i2cOwnAddr;
        instObj->i2cRegs = (CSL_I2cRegs *) initParams->i2cRegs;
        instObj->intNum = initParams->i2cIntNum;

        Semaphore_Params_init(&semPrms);
        semPrms.mode = Semaphore_Mode_BINARY;
        instObj->devBusySem = Semaphore_create(1u, &semPrms, NULL);
        if (NULL == instObj->devBusySem)
        {
            Vps_printf(" I2C: devBusySem semaphore create failed !!!\n");
            retVal = FVID2_EALLOC;
        }
        else
        {
            Semaphore_Params_init(&semPrms);
            semPrms.mode = Semaphore_Mode_BINARY;
            instObj->sem = Semaphore_create(1u, &semPrms, NULL);
            if (NULL == instObj->sem)
            {
                Vps_printf(" I2C: sem semaphore create failed !!!\n");
                Semaphore_delete(&instObj->devBusySem);
                instObj->devBusySem = NULL;

                retVal = FVID2_EALLOC;
            }
            else
            {
                Semaphore_Params_init(&semPrms);
                semPrms.mode = Semaphore_Mode_BINARY;
                instObj->completionSem = Semaphore_create(0u, &semPrms, NULL);
                if (NULL == instObj->completionSem)
                {
                    Vps_printf
                        (" I2C: completionSem semaphore create failed !!!\n");
                    Semaphore_delete(&instObj->sem);
                    Semaphore_delete(&instObj->devBusySem);
                    instObj->sem = NULL;
                    instObj->devBusySem = NULL;
                    retVal = FVID2_EALLOC;
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Everything went fine, change state and instanceId */
        instObj->state = I2C_CREATED;
        instObj->instanceId = numInstance;
        instObj->numOpens = 0;
    }

    return retVal;
}

/**
 *  \brief De-initializes I2C Controllers
 */
/* ===================================================================
 *  @func     PSP_i2cDeinit                                               
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
void PSP_i2cDeinit(Ptr args)
{
    UInt32 cnt;

    PSP_I2cInstObj *instObj = NULL;

    for (cnt = 0; cnt < VPS_DEVICE_I2C_INST_ID_MAX; cnt++)
    {
        instObj = &i2CInstObj[cnt];

#if 0
        /**
         * If number of Driver Objects is 0, then state should be
         * I2C_CLOSED OR I2C_CREATED.
         */
        if ((NULL == instObj->numOpens) &&
            ((instObj->state != I2C_CLOSED) && (instObj->state != I2C_CREATED)))
        {
            Vps_printf(" I2C: PSP_i2cDeinit called illegally !!!\n");
        }
        else
#endif
        {
            /**
             * If number of Driver Objects is NOT 0, then this function
             * SHOULD NOT be called.
             */
            if (NULL != instObj->numOpens)
            {
                Vps_printf(" I2C: PSP_i2cDeinit called illegally !!!\n");
            }
            else
            {
                /* State is fine, delete semaphore if required */
                if (NULL != instObj->completionSem)
                {
                    Semaphore_delete(&instObj->completionSem);
                    instObj->completionSem = NULL;
                }

                if (NULL != instObj->devBusySem)
                {
                    Semaphore_delete(&instObj->devBusySem);
                    instObj->devBusySem = NULL;
                }

                if (NULL != instObj->sem)
                {
                    Semaphore_delete(&instObj->sem);
                    instObj->sem = NULL;
                }
            }
        }

        /* Change state to deleted */
        instObj->numOpens = 0u;
        instObj->state = I2C_DELETED;
    }
}

/**
 *  \brief Create instance for the specific I2C controller
 */
/* ===================================================================
 *  @func     PSP_i2cCreate                                               
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
PSP_I2cHandle PSP_i2cCreate(UInt32 instanceId,
                            PSP_I2cAppCallback callback, Ptr appData)
{
    UInt32 cnt, reg_val;

    Int32 retVal = FVID2_SOK;

    PSP_I2cInstObj *instObj = NULL;

    PSP_I2cDriverObject *i2cDrvObj = NULL;
    Hwi_Params hwiParams = { 0 };

    if (instanceId < VPS_DEVICE_I2C_INST_ID_MAX)
    {
        instObj = &i2CInstObj[instanceId];
    }
    else
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    /* Wait for the first create to finish */
    if (NULL != instObj)
    {
        Semaphore_pend(instObj->sem, BIOS_WAIT_FOREVER);
    }

    if (FVID2_SOK == retVal)
    {
        /* Verify the state first. If no driver object is opened and this is
         * the first one, then state should be created/closed. */
        if (((NULL == instObj->numOpens) && (instObj->state != I2C_CREATED))
            && (instObj->state != I2C_CLOSED))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
        else
        {
            if (((instObj->numOpens) && (instObj->numOpens < I2C_NUM_OPENS))
                && (instObj->state != I2C_OPENED))
            {
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        for (cnt = 0; cnt < I2C_NUM_OPENS; cnt++)
        {
            if (NULL == i2CDriverObj[instanceId][cnt].pi2cInstHandle)
            {
                /* Free handle found, allocate it */
                i2CDriverObj[instanceId][cnt].pi2cInstHandle = instObj;

                /* Callback/appData not used currently */
                i2CDriverObj[instanceId][cnt].callBack = callback;
                i2CDriverObj[instanceId][cnt].appData = appData;

                i2cDrvObj = &i2CDriverObj[instanceId][cnt];
                break;
            }
        }
    }

    if (NULL != i2cDrvObj)
    {
        /* Initialize hardware for the first open */
        if (!instObj->numOpens)
        {
            /* Reset registers */
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_CON);
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_PSC);
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_SCLL);
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_SCLH);
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_BUF);
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_SYSC);
            I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_WE);

            /* Initialize I2C Controller */
            retVal = i2c_init(instObj->instanceId);

            if (FVID2_SOK == retVal)
            {
                /* Register the ISR if opMode=Interrupt */
                if (instObj->opMode == I2C_OPMODE_INTERRUPT)
                {
                    reg_val = I2C_IE_XRDY | I2C_IE_RRDY | I2C_IE_ARDY |
                        I2C_IE_NACK | I2C_IE_AL;
                    I2C_WRITE_REG_WORD(reg_val, &instObj->i2cRegs->I2C_IE);

                    Hwi_Params_init(&hwiParams);
                    hwiParams.arg = (UInt32) i2cDrvObj;
                    hwiParams.enableInt = TRUE;
                    instObj->hwiHandle = Hwi_create(instObj->intNum,
                                                    i2c_isr, &hwiParams, 0);
                    if (NULL == instObj->hwiHandle)
                    {
                        Vps_printf(" I2C: Interrupt registration failed !!!");
                        i2cDrvObj = NULL;
                    }
                }

                /* Clear STAT register */
                I2C_WRITE_REG_WORD(0xFFFFu, &instObj->i2cRegs->I2C_STAT);

                /* Read it again */
                reg_val = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT);
            }
            else
            {
                Vps_printf(" I2C: I2C Initialization failed !!!");
                i2cDrvObj = NULL;
            }
        }
    }

    if (NULL != i2cDrvObj)
    {
        /* Increment open count */
        instObj->numOpens++;

        /* Update the instance object state */
        instObj->state = I2C_OPENED;
    }

    if (NULL != instObj)
    {
        Semaphore_post(instObj->sem);
    }

    return (PSP_I2cHandle) i2cDrvObj;
}

/**
 *  \brief Deletes I2C driver object
 */
/* ===================================================================
 *  @func     PSP_i2cDelete                                               
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
Int32 PSP_i2cDelete(PSP_I2cHandle handle)
{
    Int32 retVal = FVID2_SOK;

    PSP_I2cInstObj *instObj = NULL;

    PSP_I2cDriverObject *i2cDrvObj = NULL;

    if (NULL == handle)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }
    else
    {
        i2cDrvObj = (PSP_I2cDriverObject *) handle;

        if (NULL != i2cDrvObj)
        {
            instObj = i2cDrvObj->pi2cInstHandle;
            if (NULL == instObj)
            {
                retVal = FVID2_EBADARGS;
            }
        }
        else
        {
            retVal = FVID2_EBADARGS;
        }
    }

    /* Wait for the first delete to finish */
    if (NULL != instObj)
    {
        Semaphore_pend(instObj->sem, BIOS_WAIT_FOREVER);
    }

    if (retVal == FVID2_SOK)
    {
        /* Check state */
        if (I2C_OPENED != instObj->state)
        {
            retVal = FVID2_EBADARGS;
        }
        else
        {
            instObj->numOpens--;
            if (!instObj->numOpens)
            {
                /* Change state to deleted */
                instObj->state = I2C_DELETED;

                /* Unregister ISR, if registered */
                if (instObj->hwiHandle)
                {
                    Hwi_delete(&instObj->hwiHandle);
                }
            }

            i2cDrvObj->pi2cInstHandle = NULL;
            i2cDrvObj->callBack = NULL;
            i2cDrvObj->appData = NULL;
            i2cDrvObj = NULL;
        }
    }

    if (NULL != instObj)
    {
        Semaphore_post(instObj->sem);
    }

    return retVal;
}

/**
 *  \brief Read/write from/to a specific slave device
 */
/* ===================================================================
 *  @func     PSP_i2cTransfer                                               
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
Int32 PSP_i2cTransfer(PSP_I2cHandle handle, PSP_I2cXferParams * i2cXferParams)
{
    Int32 retVal = FVID2_SOK;

    PSP_I2cInstObj *instObj = NULL;

    PSP_I2cDriverObject *i2cDrvObj = NULL;

    if (NULL == handle || NULL == i2cXferParams)
    {
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Check input params */
        if (NULL == i2cXferParams->buffer)
        {
            retVal = FVID2_EBADARGS;
        }

        if ((FVID2_SOK == retVal)
            && ((0 == i2cXferParams->bufLen)
                || (i2cXferParams->bufLen > I2C_BUFFER_MAXLENGTH)))
        {
            /* Buffer length should be between 1 and I2C_BUFFER_MAXLENGTH */
            retVal = FVID2_EBADARGS;
        }

        if ((FVID2_SOK == retVal) && (0 == i2cXferParams->timeout))
        {
            retVal = FVID2_ETIMEOUT;
        }
    }

    if (FVID2_SOK == retVal)
    {
        i2cDrvObj = (PSP_I2cDriverObject *) handle;

        if (NULL != i2cDrvObj)
        {
            instObj = i2cDrvObj->pi2cInstHandle;
            if (NULL == instObj)
            {
                retVal = FVID2_EBADARGS;
            }
        }
        else
        {
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check whether the I2C controller is being used or not */
        Semaphore_pend(instObj->devBusySem, I2C_DEVICE_BUSY_TIMEOUT_SECS);

        if (FVID2_SOK == retVal)
        {
            if (instObj->opMode == I2C_OPMODE_POLLED)
            {
                retVal = Iss_i2cTransferMsgPoll(i2cDrvObj, i2cXferParams);
            }
            else
            {
                retVal = Iss_i2cTransferMsgInt(i2cDrvObj, i2cXferParams);
            }
        }

        /* Post the semaphore now */
        Semaphore_post(instObj->devBusySem);
    }

    return retVal;
}

/* ===================================================================
 *  @func     PSP_i2cProbe                                               
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
int PSP_i2cProbe(UInt32 instanceId, UInt8 slaveAddr)
{
    int res = 1;                                           /* default = fail */

    PSP_I2cInstObj *instObj = NULL;

    UInt32 reg_val;

    instObj = &i2CInstObj[instanceId];

    if (slaveAddr == I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_OA))
    {
        return res;
    }

    /* Disable interrupts first */
    reg_val = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_IE);
    I2C_WRITE_REG_WORD(0x00u, &instObj->i2cRegs->I2C_IE);

    /* wait until bus not busy */
    wait_for_bb(instanceId);

    /* try to write one byte */
    I2C_WRITE_REG_WORD(1, &instObj->i2cRegs->I2C_CNT);

    /* set slave address */
    I2C_WRITE_REG_WORD(slaveAddr, &instObj->i2cRegs->I2C_SA);

    /* stop bit needed here */
    I2C_WRITE_REG_WORD(I2C_CON_EN | I2C_CON_MST | I2C_CON_STT | I2C_CON_STP,
                       &instObj->i2cRegs->I2C_CON);

    /* enough delay for the NACK bit set */
    udelay(I2C_DELAY_BIG);

    if (!(I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT) & I2C_STAT_NACK))
    {
        res = 0;                                           /* success case */
        flush_fifo(instanceId);
        I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);
    }
    else
    {
        I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);    /* failue, 
                                                                     * clear
                                                                     * sources */
        I2C_WRITE_REG_WORD(I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_CON) | I2C_CON_STP, &instObj->i2cRegs->I2C_CON);    /* finish 
                                                                                                                         * up 
                                                                                                                         * xfer 
                                                                                                                         */
        udelay(I2C_DELAY_MED);
        wait_for_bb(instanceId);
    }

    flush_fifo(instanceId);

    I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_CNT);
    I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);

    /* Enable interrupts now */
    I2C_WRITE_REG_WORD(reg_val, &instObj->i2cRegs->I2C_IE);

    return res;
}

/* ===================================================================
 *  @func     wait_for_bb                                               
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
static int wait_for_bb(UInt32 instanceId)
{
    volatile int timeout = 5000;

    UInt16 stat;

    PSP_I2cInstObj *instObj = NULL;

    int res = FVID2_SOK;

    instObj = &i2CInstObj[instanceId];

    /* Clear current interrupts... */
    I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);

    while ((stat = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT) & I2C_STAT_BB)
           && timeout--)
    {
        I2C_WRITE_REG_WORD(stat, &instObj->i2cRegs->I2C_STAT);
        udelay(I2C_DELAY_SMALL);
    }

    if (timeout <= 0)
    {
        Vps_printf(" I2C: timed out in wait_for_bb: I2C_STAT=%x !!!\n",
                   I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT));
        res = I2C_ERR_TIMEDOUT;
    }

    /* clear delayed stuff */
    I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);

    return res;
}

/* ===================================================================
 *  @func     flush_fifo                                               
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
static void flush_fifo(UInt32 instanceId)
{
    UInt16 stat;

    PSP_I2cInstObj *instObj = NULL;

    instObj = &i2CInstObj[instanceId];

    /* note: if you try and read data when its not there or ready you get a
     * bus error */
    while (1)
    {
        stat = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT);
        if (stat == I2C_STAT_RRDY)
        {
            I2C_READ_REG_BYTE(&instObj->i2cRegs->I2C_DATA);
            I2C_WRITE_REG_WORD(I2C_STAT_RRDY, &instObj->i2cRegs->I2C_STAT);
            udelay(I2C_DELAY_SMALL);
        }
        else
            break;
    }
}

/* ===================================================================
 *  @func     wait_for_pin                                               
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
static UInt16 wait_for_pin(UInt32 instanceId)
{
    UInt16 status;

    volatile int timeout = 5000;

    PSP_I2cInstObj *instObj = NULL;

    instObj = &i2CInstObj[instanceId];

    do
    {
        udelay(I2C_DELAY_SMALL);
        status = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT);
    } while (!(status &
               (I2C_STAT_ROVR | I2C_STAT_XUDF | I2C_STAT_XRDY |
                I2C_STAT_RRDY | I2C_STAT_ARDY | I2C_STAT_NACK |
                I2C_STAT_AL)) && timeout--);

    if (timeout <= 0)
    {
        printf("timed out in wait_for_pin: I2C_STAT=%x\n",
               I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT));
        I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);
    }

    return status;
}

/* ===================================================================
 *  @func     Iss_i2cTransferMsgPoll                                               
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
static Int32 Iss_i2cTransferMsgPoll(PSP_I2cDriverObject * i2cDrvObj,
                                    PSP_I2cXferParams * i2cXferParams)
{
    PSP_I2cInstObj *instObj = NULL;

    UInt16 w = 0;

    UInt16 status = 0;

    int i2c_error = 0;

    int buflen;

    Int32 retVal = FVID2_SOK;

    instObj = i2cDrvObj->pi2cInstHandle;

    if (!(i2cXferParams->flags & I2C_IGNORE_BUS_BUSY))
        retVal = wait_for_bb(instObj->instanceId);
    if (FVID2_SOK != retVal)
        return retVal;

    /* Write slave address */
    if (TRUE == instObj->is10BitAddr)
    {
        I2C_WRITE_REG_WORD(i2cXferParams->slaveAddr & 0x3FFu,
                           &instObj->i2cRegs->I2C_SA);
    }
    else
    {
        I2C_WRITE_REG_WORD(i2cXferParams->slaveAddr & 0x7Fu,
                           &instObj->i2cRegs->I2C_SA);
    }

    I2C_WRITE_REG_WORD(i2cXferParams->bufLen, &instObj->i2cRegs->I2C_CNT);

    /* Enable I2C controller */
    w = I2C_CON_EN;

    /* High speed configuration */
    if (instObj->i2cBusFreq > 400)
        w |= I2C_CON_OPMODE_HS;

    if (instObj->is10BitAddr)
        w |= I2C_CON_XA;

    if (i2cXferParams->flags & I2C_MASTER)
        w |= I2C_CON_MST;

    if (i2cXferParams->flags & I2C_WRITE)
        w |= I2C_CON_TRX;

    if (i2cXferParams->flags & I2C_START)
        w |= I2C_CON_STT;

    if (i2cXferParams->flags & I2C_STOP)
        w |= I2C_CON_STP;

    I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_CON);

    status = wait_for_pin(instObj->instanceId);

    if (i2cXferParams->flags & I2C_WRITE)
    {
        /* Transmit */
        for (buflen = 0; buflen < i2cXferParams->bufLen; buflen++)
        {
            if (status & I2C_STAT_XRDY)
            {
                I2C_WRITE_REG_BYTE(i2cXferParams->buffer[buflen],
                                   &instObj->i2cRegs->I2C_DATA);

                /* dont wait for the last byte */
                if (buflen < i2cXferParams->bufLen - 1)
                    status = wait_for_pin(instObj->instanceId);
            }
            else
            {
                i2c_error = 1;
            }
        }

        /* must have enough delay to allow BB bit to go low */
        udelay(I2C_DELAY_BIG);
        if (I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT) & I2C_STAT_NACK)
        {
            i2c_error = 1;
        }
    }
    else
    {
        /* Receive */
        for (buflen = 0; buflen < i2cXferParams->bufLen; buflen++)
        {
            if (status & I2C_STAT_RRDY)
            {
                i2cXferParams->buffer[buflen] =
                    I2C_READ_REG_BYTE(&instObj->i2cRegs->I2C_DATA);
                udelay(I2C_DELAY_MED);

                /* dont wait for the last byte */
                if (buflen < i2cXferParams->bufLen - 1)
                    status = wait_for_pin(instObj->instanceId);
            }
            else
            {
                i2c_error = 1;
            }
        }

        /* must have enough delay to allow BB bit to go low */
        udelay(I2C_DELAY_BIG);
        if (I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT) & I2C_STAT_NACK)
        {
            i2c_error = 1;
        }
    }

    if (i2cXferParams->flags & I2C_STOP)
    {
        flush_fifo(instObj->instanceId);

        I2C_WRITE_REG_WORD(0xFFFF, &instObj->i2cRegs->I2C_STAT);
        I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_CNT);
    }

    /* Re-initialize I2C again in case of error */
    if (i2c_error)
        i2c_init(instObj->instanceId);

    return i2c_error;
}

/* ===================================================================
 *  @func     Iss_i2cTransferMsgInt                                               
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
static Int32 Iss_i2cTransferMsgInt(PSP_I2cDriverObject * i2cDrvObj,
                                   PSP_I2cXferParams * i2cXferParams)
{
    PSP_I2cInstObj *instObj = NULL;

    UInt16 w = 0;

    int i2c_error = 0;

    Int32 retVal = FVID2_SOK;

    instObj = i2cDrvObj->pi2cInstHandle;

    if (i2cXferParams->bufLen == 0)
        return I2C_ERR_INVALID_PARAMS;

    if (!(i2cXferParams->flags & I2C_IGNORE_BUS_BUSY))
        retVal = wait_for_bb(instObj->instanceId);
    if (FVID2_SOK != retVal)
        return retVal;

    /* Write slave address */
    if (TRUE == instObj->is10BitAddr)
    {
        I2C_WRITE_REG_WORD(i2cXferParams->slaveAddr & 0x3FFu,
                           &instObj->i2cRegs->I2C_SA);
    }
    else
    {
        I2C_WRITE_REG_WORD(i2cXferParams->slaveAddr & 0x7Fu,
                           &instObj->i2cRegs->I2C_SA);
    }

    /* store the buffer and length */
    instObj->currBuffer = i2cXferParams->buffer;
    instObj->currBufferLen = i2cXferParams->bufLen;
    instObj->currFlags = i2cXferParams->flags;
    instObj->currError = 0;

    /* Clear the FIFO Buffers */
    w = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_BUF);
    w |= I2C_BUF_RXFIF_CLR | I2C_BUF_TXFIF_CLR;
    I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_BUF);

    I2C_WRITE_REG_WORD(i2cXferParams->bufLen, &instObj->i2cRegs->I2C_CNT);

    /* Enable I2C controller */
    w = I2C_CON_EN;

    /* High speed configuration */
    if (instObj->i2cBusFreq > 400)
        w |= I2C_CON_OPMODE_HS;

    if (instObj->is10BitAddr)
        w |= I2C_CON_XA;

    if (i2cXferParams->flags & I2C_MASTER)
        w |= I2C_CON_MST;

    if (i2cXferParams->flags & I2C_WRITE)
        w |= I2C_CON_TRX;

    if (i2cXferParams->flags & I2C_START)
        w |= I2C_CON_STT;

    if (i2cXferParams->flags & I2C_STOP)
        w |= I2C_CON_STP;

    I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_CON);

    /* Wait for the transfer to finish */
    i2c_error = Semaphore_pend(instObj->completionSem, i2cXferParams->timeout);

    instObj->currBufferLen = 0;

    /* Semaphore_pend returns TRUE if successful, FALSE if timeout */
    if (i2c_error)
    {
        /* Check for error, if any */
        if (!instObj->currError)
            return FVID2_SOK;

        if (instObj->currError & (I2C_STAT_AL | I2C_STAT_ROVR | I2C_STAT_XUDF))
        {
            i2c_init(instObj->instanceId);
            return I2C_ERR_IO;
        }

        if (instObj->currError & I2C_STAT_NACK)
        {
            if (i2cXferParams->flags & I2C_M_IGNORE_NAK)
            {
                i2c_init(instObj->instanceId);
                return FVID2_SOK;
            }

            if (i2cXferParams->flags & I2C_STOP)
            {
                w = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_CON);
                w |= I2C_CON_STP;
                I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_CON);
            }

            i2c_init(instObj->instanceId);
            return FVID2_EFAIL;
        }
    }

    /* Either timeout or error */
    Vps_printf(" I2C: Either timeout or some error !!!\n");
    i2c_init(instObj->instanceId);

    return I2C_ERR_TIMEDOUT;
}

/* ===================================================================
 *  @func     i2c_isr                                               
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
static void i2c_isr(UArg arg)
{
    PSP_I2cInstObj *instObj = NULL;

    PSP_I2cDriverObject *i2cDrvObj = NULL;

    UInt32 bits, stat, w;

    Int32 err, count = 0;

    i2cDrvObj = (PSP_I2cDriverObject *) arg;
    instObj = i2cDrvObj->pi2cInstHandle;

    /* check enabled interrupt bits */
    bits = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_IE);

    while ((stat = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_STAT)) & bits)
    {
        if (count++ == 1000)
        {
            Vps_printf(" I2C: too much time in one irq !!!\n");
            break;
        }

        err = 0;

        /* 
         * Ack the stat in one go, but [R/X]DR and [R/X]RDY should be
         * acked after the data operation is complete.
         */
        w = stat & ~(I2C_STAT_RRDY | I2C_STAT_RDR |
                     I2C_STAT_XRDY | I2C_STAT_XDR);
        I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_STAT);

        if (stat & I2C_STAT_NACK)
        {
            err |= I2C_STAT_NACK;
            I2C_WRITE_REG_WORD(I2C_CON_STP, &instObj->i2cRegs->I2C_CON);
        }

        if (stat & I2C_STAT_AL)
        {
            Vps_printf(" I2C: Arbitration lost !!!\n");
            err |= I2C_STAT_AL;
        }

        if (stat & (I2C_STAT_ARDY | I2C_STAT_NACK | I2C_STAT_AL))
        {
            w = stat & (I2C_STAT_RRDY | I2C_STAT_RDR |
                        I2C_STAT_XRDY | I2C_STAT_XDR);
            I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_STAT);

            instObj->currError |= err;

            /* Post completion semaphore */
            Semaphore_post(instObj->completionSem);

            return;
        }

        if (stat & (I2C_STAT_RRDY | I2C_STAT_RDR))
        {
            UInt32 num_bytes = 1;

            while (num_bytes)
            {
                num_bytes--;
                w = I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_DATA);

                if (instObj->currBufferLen)
                {
                    *instObj->currBuffer++ = w;
                    instObj->currBufferLen--;
                }
                else
                {
#ifdef I2C_DEBUG
                    if (stat & I2C_STAT_RRDY)
                        Vps_printf
                            (" I2C: RRDY IRQ while no data requested !!!\n");
                    if (stat & I2C_STAT_RDR)
                        Vps_printf
                            (" I2C: RDR IRQ while no data requested !!!\n");
#endif
                    break;
                }
            }
            I2C_WRITE_REG_WORD(stat & (I2C_STAT_RRDY | I2C_STAT_RDR),
                               &instObj->i2cRegs->I2C_STAT);
            continue;
        }

        if (stat & (I2C_STAT_XRDY | I2C_STAT_XDR))
        {
            UInt32 num_bytes = 1;

            while (num_bytes)
            {
                num_bytes--;
                w = 0;
                if (instObj->currBufferLen)
                {
                    w = *instObj->currBuffer++;
                    instObj->currBufferLen--;
                }
                else
                {
#ifdef I2C_DEBUG
                    if (stat & I2C_STAT_XRDY)
                        Vps_printf
                            (" I2C: XRDY IRQ while no data to send !!!\n");
                    if (stat & I2C_STAT_XDR)
                        Vps_printf(" I2C: XDR IRQ while no data to send !!!\n");
#endif
                    break;
                }
                I2C_WRITE_REG_WORD(w, &instObj->i2cRegs->I2C_DATA);
            }
            I2C_WRITE_REG_WORD(stat & (I2C_STAT_XRDY | I2C_STAT_XDR),
                               &instObj->i2cRegs->I2C_STAT);
            continue;
        }

        if (stat & I2C_STAT_ROVR)
        {
            Vps_printf(" I2C: Receive overrun !!!\n");
            instObj->currError |= I2C_STAT_ROVR;
        }
        if (stat & I2C_STAT_XUDF)
        {
            Vps_printf(" I2C: Transmit underflow !!!\n");
            instObj->currError |= I2C_STAT_XUDF;
        }
    }

    return;
}

/* ===================================================================
 *  @func     i2c_init                                               
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
static int i2c_init(UInt32 instanceId)
{
    UInt32 psc = 0, scll = 0, sclh = 0;

    unsigned long fclk_rate;

    unsigned long internal_clk = 0;

    PSP_I2cInstObj *instObj = NULL;

    Uint32 delay = 1000;

    UInt32 reg_val = 0;

    UInt16 selfAddress = 0xCC;

    instObj = &i2CInstObj[instanceId];

    I2C_WRITE_REG_WORD(SYSC_SOFTRESET_MASK, &instObj->i2cRegs->I2C_SYSC);

    /* For some reason we need to set the EN bit before the * reset done bit
     * gets set. */
    I2C_WRITE_REG_WORD(I2C_CON_EN, &instObj->i2cRegs->I2C_CON);

    /* Wait for the reset to get complete */
    while ((!
            (I2C_READ_REG_WORD(&instObj->i2cRegs->I2C_SYSS) &
             SYSS_RESETDONE_MASK)) && delay)
    {
        delay--;
        udelay(I2C_DELAY_SMALL);
    }

    if (!delay)
    {
        /* reset has failed, return!!! */
        return I2C_ERR_TIMEDOUT;
    }

    /* SYSC register is cleared by the reset, rewrite it */
    reg_val = SYSC_AUTOIDLE_MASK
        | SYSC_ENAWAKEUP_MASK
        | (SYSC_IDLEMODE_SMART << SYSC_IDLEMODE_SHIFT)
        | (SYSC_CLOCKACTIVITY_FCLK << SYSC_CLOCKACTIVITY_SHIFT);
    I2C_WRITE_REG_WORD(reg_val, &instObj->i2cRegs->I2C_SYSC);

    /* Disable it again */
    I2C_WRITE_REG_WORD(0, &instObj->i2cRegs->I2C_CON);

    if (instObj->i2cBusFreq > 100)
        internal_clk = 12000;                              /* 12 MHz */
    else
        internal_clk = 4000;                               /* 4 MHz */

    fclk_rate = I2C_FUNC_CLOCK / 1000;

    /* Compute prescaler divisor */
    psc = fclk_rate / internal_clk;
    psc = psc - 1;

    /* Standard and fast speed (upto 400Khz) */
    scll = sclh = internal_clk / (2 * instObj->i2cBusFreq);

    scll -= I2C_FASTSPEED_SCLL_TRIM;
    sclh -= I2C_FASTSPEED_SCLH_TRIM;

    I2C_WRITE_REG_WORD(psc, &instObj->i2cRegs->I2C_PSC);
    I2C_WRITE_REG_WORD(scll, &instObj->i2cRegs->I2C_SCLL);
    I2C_WRITE_REG_WORD(sclh, &instObj->i2cRegs->I2C_SCLH);

    /* Own address */
    I2C_WRITE_REG_WORD(selfAddress, &instObj->i2cRegs->I2C_OA);

    /* Take the I2C module out of reset: */
    I2C_WRITE_REG_WORD(I2C_CON_EN, &instObj->i2cRegs->I2C_CON);

    /* Enable interrupts, if required */
    if (instObj->opMode == I2C_OPMODE_INTERRUPT)
    {
        I2C_WRITE_REG_WORD(I2C_IE_XRDY | I2C_IE_RRDY | I2C_IE_ARDY |
                           I2C_IE_NACK | I2C_IE_AL, &instObj->i2cRegs->I2C_IE);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     udelay                                               
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
static void udelay(int delay_usec)
{
    Int32 delay_msec;

    delay_msec = delay_usec / 1000;
    if (delay_msec == 0)
        delay_msec = 1;
    Task_sleep(delay_msec);
}
