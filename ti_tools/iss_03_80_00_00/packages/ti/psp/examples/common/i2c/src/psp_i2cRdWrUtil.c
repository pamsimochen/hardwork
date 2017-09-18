/** ==================================================================
 *  @file   psp_i2cRdWrUtil.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/i2c/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include "psp_i2cRdWrUtil.h"

#pragma DATA_ALIGN(gPspI2c_tskStackMain, 32)
#pragma DATA_SECTION(gPspI2c_tskStackMain, ".bss:taskStackSection")
UInt8 gPspI2c_tskStackMain[PSP_I2C_TSK_STACK_MAIN];

Int32 aewbVendor = 0;

PspI2c_Ctrl gPspI2c_ctrl;

/* ===================================================================
 *  @func     PspI2c_exeCmd                                               
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
Int32 PspI2c_exeCmd(FILE * fp, Bool echoCmd);

/* ===================================================================
 *  @func     PspI2c_tokenizeInput                                               
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
UInt32 PspI2c_tokenizeInput(char *inStr, char *argv[])
{
    char delimiters[] = " \r\n";

    char *token;

    UInt32 argc = 0;

    token = strtok(inStr, delimiters);

    while (token != NULL)
    {
        argv[argc] = token;
        argc++;

        token = strtok(NULL, delimiters);
    }

    return argc;
}

/* ===================================================================
 *  @func     xtod                                               
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
static char xtod(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return c = 0;                                          // not Hex digit
}

/* ===================================================================
 *  @func     HextoDec                                               
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
static int HextoDec(char *hex, int l)
{
    if (*hex == 0)
        return (l);

    return HextoDec(hex + 1, l * 16 + xtod(*hex));         // hex+1?
}

/* ===================================================================
 *  @func     xstrtoi                                               
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
int xstrtoi(char *hex)                                     // hex string to
                                                           // integer
{
    return HextoDec(hex, 0);
}

/* ===================================================================
 *  @func     PspI2c_showHelp                                               
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
Int32 PspI2c_showHelp()
{
    Vps_printf(" \r\n Commands,"
               " \r\n ========="
               " \r\n help                   - shows this screen"
               " \r\n quit                   - exits utility"
               " \r\n inst <i2c instance ID> - switch to another I2C instance (0 or 1)"
               " \r\n probe                  - probes the I2C instance (0 or 1) for all possibly connected slaves"
               " \r\n file <filename>        - execute I2C utility commands by reading from file <filename>"
               " \r\n wait <time in msecs>   - introduce a delay of <time in msecs> msecs"
               " \r\n"
               " \r\n rd <dev addr in hex> <reg offset in hex> <num regs to read in hex> "
               " \r\n wr <dev addr in hex> <reg offset in hex> <value to write in hex> "
               " \r\n rd16 <dev addr in hex> <reg offset in hex> <num regs to read in hex> "
               " \r\n wr16 <dev addr in hex> <reg offset in hex> <value to write in hex> "
               " \r\n"
               " \r\n rdio <dev addr in hex> <num regs to read in hex>"
               " \r\n       - Reads the specified no. of registers from a desired IO expander kind "
               " \r\n         of slave, which doesn't have the register offset"
               " \r\n"
               " \r\n wrio <dev addr in hex> <reg value 1 in hex> <reg value 2 in hex> "
               " \r\n       - Writes 2 bytes at the desired IO expander kind "
               " \r\n         of slave, which doesn't have the register offset"
               " \r\n" " \r\n");

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_changeInstId                                               
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
Int32 PspI2c_changeInstId()
{
    UInt32 value;

    if (gPspI2c_ctrl.numArgs > 1)
    {
        value = atoi(gPspI2c_ctrl.inputArgs[1]);

        if (value < ISS_DEVICE_I2C_INST_ID_MAX)
        {
            gPspI2c_ctrl.i2cInstId = value;

            Vps_printf(" I2C: Changed to instance %d !!!\r\n",
                       gPspI2c_ctrl.i2cInstId);
        }
        else
        {
            Vps_printf(" I2C: Illegal Instance ID (%d) specified !!!\r\n",
                       value);
        }
    }
    else
    {
        Vps_printf(" I2C: Instance ID not specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_writeReg                                               
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
Int32 PspI2c_writeReg()
{
    UInt32 devAddr;

    UInt32 regAddr;

    UInt32 regValue;

    Int32 status = FVID2_SOK;

    if (gPspI2c_ctrl.numArgs == 4)
    {
        devAddr = xstrtoi(gPspI2c_ctrl.inputArgs[1]);

        if (devAddr > 0 && devAddr < 0x100)
        {
            regAddr = xstrtoi(gPspI2c_ctrl.inputArgs[2]);

            if (regAddr < 0x100)
            {
                regValue = xstrtoi(gPspI2c_ctrl.inputArgs[3]);

                if (regValue < 0x100)
                {
                    Vps_printf
                        (" I2C: Writing 0x%02x to REG 0x%02x of device 0x%02x ... !!!\r\n",
                         regValue, regAddr, devAddr);

                    gPspI2c_ctrl.regAddr[0] = regAddr;
                    gPspI2c_ctrl.regValue[0] = regValue;

#ifdef PLATFORM_EVM_SI
                    status = Iss_deviceWrite8(gPspI2c_ctrl.i2cInstId,
                                              devAddr,
                                              gPspI2c_ctrl.regAddr,
                                              gPspI2c_ctrl.regValue, 1);
#endif
                    if (status != FVID2_SOK)
                    {
                        Vps_printf(" I2C: Write ERROR !!!\r\n");
                    }
                    else
                    {
                        Vps_printf(" I2C: Write Done !!!\r\n");
                    }

                    if (status == FVID2_SOK)
                    {
                        Vps_printf
                            (" I2C: Reading back the written register ... !!!\r\n");

                        gPspI2c_ctrl.regValue[0] = 0;
#ifdef PLATFORM_EVM_SI
                        status = Iss_deviceRead8(gPspI2c_ctrl.i2cInstId,
                                                 devAddr,
                                                 gPspI2c_ctrl.regAddr,
                                                 gPspI2c_ctrl.regValue, 1);
#endif
                        Vps_printf(" I2C: 0x%02x = 0x%02x\r\n",
                                   gPspI2c_ctrl.regAddr[0],
                                   gPspI2c_ctrl.regValue[0]);
                        if (status != FVID2_SOK)
                        {
                            Vps_printf(" I2C: Read ERROR !!!\r\n");
                        }
                        else
                        {
                            Vps_printf(" I2C: Read Done !!!\r\n");
                        }
                    }
                    Vps_printf(" \r\n");
                }
                else
                {
                    Vps_printf
                        (" I2C: Illegal register value (0x%x) specified !!!\r\n",
                         regValue);
                }
            }
            else
            {
                Vps_printf
                    (" I2C: Illegal Register address (0x%x) specified !!!\r\n",
                     regAddr);
            }
        }
        else
        {
            Vps_printf(" I2C: Illegal Device address (0x%x) specified !!!\r\n",
                       devAddr);
        }

    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c16_writeReg                                               
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
Int32 PspI2c16_writeReg()
{
    UInt32 devAddr;

    UInt32 regAddr;

    UInt32 regValue;

    Int32 status = FVID2_SOK;

    if (gPspI2c_ctrl.numArgs == 4)
    {
        devAddr = xstrtoi(gPspI2c_ctrl.inputArgs[1]);

        if (devAddr > 0 && devAddr < 0x100)
        {
            regAddr = xstrtoi(gPspI2c_ctrl.inputArgs[2]);

            if (regAddr < 0x6000)
            {
                regValue = xstrtoi(gPspI2c_ctrl.inputArgs[3]);

                if (regValue < 0x6000)
                {
                    Vps_printf
                        (" I2C: Writing 0x%02x to REG 0x%02x of device 0x%02x ... !!!\r\n",
                         regValue, regAddr, devAddr);

                    gPspI2c_ctrl.regAddr[0] = regAddr;
                    gPspI2c_ctrl.regValue[0] = regValue;

#ifdef PLATFORM_EVM_SI
                    status = Iss_deviceWrite16(gPspI2c_ctrl.i2cInstId,
                                               devAddr,
                                               gPspI2c_ctrl.regAddr,
                                               gPspI2c_ctrl.regValue, 1);
#endif
                    if (status != FVID2_SOK)
                    {
                        Vps_printf(" I2C: Write ERROR !!!\r\n");
                    }
                    else
                    {
                        Vps_printf(" I2C: Write Done !!!\r\n");
                    }

                    if (status == FVID2_SOK)
                    {
                        Vps_printf
                            (" I2C: Reading back the written register ... !!!\r\n");

                        gPspI2c_ctrl.regValue[0] = 0;
#ifdef PLATFORM_EVM_SI
                        status = Iss_deviceRead16(gPspI2c_ctrl.i2cInstId,
                                                  devAddr,
                                                  gPspI2c_ctrl.regAddr,
                                                  gPspI2c_ctrl.regValue, 1);
#endif
                        Vps_printf(" I2C: 0x%02x = 0x%02x\r\n",
                                   gPspI2c_ctrl.regAddr[0],
                                   gPspI2c_ctrl.regValue[0]);
                        if (status != FVID2_SOK)
                        {
                            Vps_printf(" I2C: Read ERROR !!!\r\n");
                        }
                        else
                        {
                            Vps_printf(" I2C: Read Done !!!\r\n");
                        }
                    }
                    Vps_printf(" \r\n");
                }
                else
                {
                    Vps_printf
                        (" I2C: Illegal register value (0x%x) specified !!!\r\n",
                         regValue);
                }
            }
            else
            {
                Vps_printf
                    (" I2C: Illegal Register address (0x%x) specified !!!\r\n",
                     regAddr);
            }
        }
        else
        {
            Vps_printf(" I2C: Illegal Device address (0x%x) specified !!!\r\n",
                       devAddr);
        }

    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_readRegs                                               
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
Int32 PspI2c_readRegs()
{
    UInt32 devAddr;

    UInt32 regAddr;

    UInt32 numRegs, i;

    Int32 status = FVID2_SOK;

    UInt32 currTime = 0, timeElapsed = 0;

    if (gPspI2c_ctrl.numArgs == 4)
    {
        devAddr = xstrtoi(gPspI2c_ctrl.inputArgs[1]);

        if (devAddr > 0 && devAddr < 0x100)
        {
            regAddr = xstrtoi(gPspI2c_ctrl.inputArgs[2]);

            if (regAddr < 0x100)
            {
                numRegs = xstrtoi(gPspI2c_ctrl.inputArgs[3]);

                if (numRegs < PSP_I2C_MAX_REG_RD_WR)
                {
                    Vps_printf
                        (" I2C: Reading 0x%02x registers starting from REG 0x%02x of device 0x%02x ... !!!\r\n",
                         numRegs, regAddr, devAddr);

                    for (i = 0; i < numRegs; i++)
                    {
                        gPspI2c_ctrl.regAddr[i] = regAddr + i;
                        gPspI2c_ctrl.regValue[i] = 0;
                    }

                    currTime = Clock_getTicks();

#ifdef PLATFORM_EVM_SI
                    status = Iss_deviceRead8(gPspI2c_ctrl.i2cInstId,
                                             devAddr,
                                             gPspI2c_ctrl.regAddr,
                                             gPspI2c_ctrl.regValue, numRegs);
#endif

                    timeElapsed = Clock_getTicks();

                    for (i = 0; i < numRegs; i++)
                    {
                        Vps_printf(" I2C: 0x%02x = 0x%02x\r\n",
                                   gPspI2c_ctrl.regAddr[i],
                                   gPspI2c_ctrl.regValue[i]);
                    }
                    if (status != FVID2_SOK)
                    {
                        Vps_printf(" I2C: Read ERROR !!!\r\n");
                    }
                    else
                    {
                        Vps_printf(" I2C: Read Done !!!\r\n");
                    }

                    Vps_printf(" I2C: Time Elapsed in Read = %d msec",
                               timeElapsed - currTime);
                    Vps_printf(" \r\n");
                }
                else
                {
                    Vps_printf
                        (" I2C: Illegal number of registers to read (0x%x) specified !!!\r\n",
                         numRegs);
                }
            }
            else
            {
                Vps_printf
                    (" I2C: Illegal Register address (0x%x) specified !!!\r\n",
                     regAddr);
            }
        }
        else
        {
            Vps_printf(" I2C: Illegal Device address (0x%x) specified !!!\r\n",
                       devAddr);
        }

    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c16_readRegs                                               
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
Int32 PspI2c16_readRegs()
{
    UInt32 devAddr;

    UInt32 regAddr;

    UInt32 numRegs, i;

    Int32 status = FVID2_SOK;

    UInt32 currTime = 0, timeElapsed = 0;

    if (gPspI2c_ctrl.numArgs == 4)
    {
        devAddr = xstrtoi(gPspI2c_ctrl.inputArgs[1]);

        if (devAddr > 0 && devAddr < 0x100)
        {
            regAddr = xstrtoi(gPspI2c_ctrl.inputArgs[2]);

            if (regAddr < 0x6000)
            {
                numRegs = xstrtoi(gPspI2c_ctrl.inputArgs[3]);

                if (numRegs < PSP_I2C_MAX_REG_RD_WR)
                {
                    Vps_printf
                        (" I2C: Reading 0x%02x registers starting from REG 0x%02x of device 0x%02x ... !!!\r\n",
                         numRegs, regAddr, devAddr);

                    for (i = 0; i < numRegs; i++)
                    {
                        gPspI2c_ctrl.regAddr[i] = regAddr + i;
                        gPspI2c_ctrl.regValue[i] = 0;
                    }

                    currTime = Clock_getTicks();

#ifdef PLATFORM_EVM_SI
                    status = Iss_deviceRead16(gPspI2c_ctrl.i2cInstId,
                                              devAddr,
                                              gPspI2c_ctrl.regAddr,
                                              gPspI2c_ctrl.regValue, numRegs);
#endif

                    timeElapsed = Clock_getTicks();

                    for (i = 0; i < numRegs; i++)
                    {
                        Vps_printf(" I2C: 0x%02x = 0x%02x\r\n",
                                   gPspI2c_ctrl.regAddr[i],
                                   gPspI2c_ctrl.regValue[i]);
                    }
                    if (status != FVID2_SOK)
                    {
                        Vps_printf(" I2C: Read ERROR !!!\r\n");
                    }
                    else
                    {
                        Vps_printf(" I2C: Read Done !!!\r\n");
                    }

                    Vps_printf(" I2C: Time Elapsed in Read = %d msec",
                               timeElapsed - currTime);
                    Vps_printf(" \r\n");
                }
                else
                {
                    Vps_printf
                        (" I2C: Illegal number of registers to read (0x%x) specified !!!\r\n",
                         numRegs);
                }
            }
            else
            {
                Vps_printf
                    (" I2C: Illegal Register address (0x%x) specified !!!\r\n",
                     regAddr);
            }
        }
        else
        {
            Vps_printf(" I2C: Illegal Device address (0x%x) specified !!!\r\n",
                       devAddr);
        }

    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_writeIoExpReg                                               
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
Int32 PspI2c_writeIoExpReg()
{
    UInt32 devAddr;

    UInt32 regValue1, regValue2;

    Int32 status = FVID2_SOK;

    if (gPspI2c_ctrl.numArgs == 4)
    {
        devAddr = xstrtoi(gPspI2c_ctrl.inputArgs[1]);

        if (devAddr > 0 && devAddr < 0x100)
        {
            regValue1 = xstrtoi(gPspI2c_ctrl.inputArgs[2]);
            regValue2 = xstrtoi(gPspI2c_ctrl.inputArgs[3]);

            Vps_printf(" \r\n");
            Vps_printf
                (" I2C: Writing 0x%02x, 0x%02x to device 0x%02x ... !!!\r\n",
                 regValue1, regValue2, devAddr);

            gPspI2c_ctrl.regAddr[0] = regValue1;
            gPspI2c_ctrl.regValue[0] = regValue2;

#ifdef PLATFORM_EVM_SI
            status = Iss_deviceWrite8(gPspI2c_ctrl.i2cInstId,
                                      devAddr,
                                      gPspI2c_ctrl.regAddr,
                                      gPspI2c_ctrl.regValue, 1);
#endif
            if (status != FVID2_SOK)
            {
                Vps_printf(" I2C: Write ERROR !!!\r\n");
            }
            else
            {
                Vps_printf(" I2C: Write Done !!!\r\n");
            }
        }
        else
        {
            Vps_printf(" I2C: Illegal Device address (0x%x) specified !!!\r\n",
                       devAddr);
        }

    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_readIoExpReg                                               
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
Int32 PspI2c_readIoExpReg()
{
    UInt32 devAddr;

    UInt32 numRegs, i;

    Int32 status = FVID2_SOK;

    UInt32 currTime = 0, timeElapsed = 0;

    if (gPspI2c_ctrl.numArgs == 3)
    {
        devAddr = xstrtoi(gPspI2c_ctrl.inputArgs[1]);

        if (devAddr > 0 && devAddr < 0x100)
        {
            numRegs = xstrtoi(gPspI2c_ctrl.inputArgs[2]);

            if (numRegs < PSP_I2C_MAX_REG_RD_WR)
            {
                Vps_printf(" \r\n");
                Vps_printf
                    (" I2C: Reading 0x%02x registers of device 0x%02x ... !!!\r\n",
                     numRegs, devAddr);

                for (i = 0; i < numRegs; i++)
                {
                    gPspI2c_ctrl.regValue[i] = 0;
                }

                currTime = Clock_getTicks();

#ifdef PLATFORM_EVM_SI
                status = Iss_deviceRawRead8(gPspI2c_ctrl.i2cInstId,
                                            devAddr,
                                            gPspI2c_ctrl.regValue, numRegs);
#endif

                timeElapsed = Clock_getTicks();

                for (i = 0; i < numRegs; i++)
                {
                    Vps_printf(" I2C: 0x%02x = 0x%02x\r\n",
                               0x0 + i, gPspI2c_ctrl.regValue[i]);
                }
                if (status != FVID2_SOK)
                {
                    Vps_printf(" I2C: Read ERROR !!!\r\n");
                }
                else
                {
                    Vps_printf(" I2C: Read Done !!!\r\n");
                }

                Vps_printf(" I2C: Time Elapsed in Read = %d msec",
                           timeElapsed - currTime);
                Vps_printf(" \r\n");
            }
            else
            {
                Vps_printf
                    (" I2C: Illegal number of registers to read (0x%x) specified !!!\r\n",
                     numRegs);
            }
        }
        else
        {
            Vps_printf(" I2C: Illegal Device address (0x%x) specified !!!\r\n",
                       devAddr);
        }

    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_delay                                               
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
Int32 PspI2c_delay()
{
    UInt32 timeInMsecs;

    if (gPspI2c_ctrl.numArgs > 1)
    {

        timeInMsecs = atoi(gPspI2c_ctrl.inputArgs[1]);

        Vps_printf(" I2C: Waiting for %d msecs ... \r\n", timeInMsecs);
        Task_sleep(timeInMsecs);
        Vps_printf(" I2C: Wait Done !!!\r\n");
    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_file                                               
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
Int32 PspI2c_file()
{
    FILE *fp;

    Bool done = FALSE;

    char filename[256];

    if (gPspI2c_ctrl.numArgs > 1)
    {
        strncpy(filename, gPspI2c_ctrl.inputArgs[1], sizeof(filename));

        Vps_printf(" I2C: Reading from file [%s] !!! \r\n", filename);

        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            Vps_printf(" I2C: File [%s] not found !!! \r\n", filename);
            return 0;
        }

        while (!done)
        {
            done = PspI2c_exeCmd(fp, TRUE);
        }

        fclose(fp);

        Vps_printf(" I2C: File closed [%s] !!! \r\n", filename);
    }
    else
    {
        Vps_printf(" I2C: Insufficient arguments specified !!!\r\n");
    }

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_exeCmd                                               
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
Int32 PspI2c_exeCmd(FILE * fp, Bool echoCmd)
{
    Bool done = FALSE;

    char *retStr;

    retStr = fgets(gPspI2c_ctrl.inputStr, sizeof(gPspI2c_ctrl.inputStr), fp);

    if (retStr == NULL)
        return TRUE;

    if (echoCmd)
    {
        Vps_printf(" \r\n");
        Vps_printf(" I2C: CMD: %s", gPspI2c_ctrl.inputStr);
    }

    gPspI2c_ctrl.numArgs = PspI2c_tokenizeInput(gPspI2c_ctrl.inputStr,
                                                gPspI2c_ctrl.inputArgs);

    if (gPspI2c_ctrl.numArgs)
    {
        if (strcmp(gPspI2c_ctrl.inputArgs[0], "quit") == 0)
        {
            done = TRUE;
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "help") == 0)
        {
            PspI2c_showHelp();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "inst") == 0)
        {
            PspI2c_changeInstId();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "rd") == 0)
        {
            PspI2c_readRegs();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "wr") == 0)
        {
            PspI2c_writeReg();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "rd16") == 0)
        {
            PspI2c16_readRegs();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "wr16") == 0)
        {
            PspI2c16_writeReg();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "wait") == 0)
        {
            PspI2c_delay();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "file") == 0)
        {
            PspI2c_file();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "wrio") == 0)
        {
            PspI2c_writeIoExpReg();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "rdio") == 0)
        {
            PspI2c_readIoExpReg();
        }
        else if (strcmp(gPspI2c_ctrl.inputArgs[0], "probe") == 0)
        {
            Iss_deviceI2cProbeAll(gPspI2c_ctrl.i2cInstId);
        }
    }
    return done;
}

/* ===================================================================
 *  @func     PspI2c_run                                               
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
Int32 PspI2c_run()
{
    Bool done = FALSE;

    gPspI2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

    Vps_printf(" I2C: Instance ID %d !!!\r\n", gPspI2c_ctrl.i2cInstId);

    PspI2c_showHelp();

    while (!done)
    {
        Vps_printf(" I2C-%d> ", gPspI2c_ctrl.i2cInstId);

        done = PspI2c_exeCmd(stdin, FALSE);
    }

    Vps_printf(" I2C: Bye Bye !!!\r\n");

    return 0;
}

/* ===================================================================
 *  @func     PspI2c_init                                               
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
Int32 PspI2c_init()
{
    Int32 status = FVID2_SOK;

    Iss_PlatformDeviceInitParams deviceInitPrms;

    Iss_PlatformInitParams platformInitPrms;

    Vps_printf(" \r\n");
    Vps_printf(" I2C: System Init in progress !!!\n");

    platformInitPrms.isPinMuxSettingReq = TRUE;
    status = Vps_platformInit(&platformInitPrms);
    GT_assert(GT_DEFAULT_MASK, status == 0);

    platformInitPrms.isPinMuxSettingReq = TRUE;
    status = Iss_platformInit(&platformInitPrms);
    GT_assert(GT_DEFAULT_MASK, status == 0);

    status = FVID2_init(NULL);
    GT_assert(GT_DEFAULT_MASK, status == 0);

    deviceInitPrms.isI2cInitReq = TRUE;
    status = Vps_platformDeviceInit(&deviceInitPrms);
    GT_assert(GT_DEFAULT_MASK, status == 0);

    deviceInitPrms.isI2cInitReq = TRUE;
    status = Iss_platformDeviceInit(&deviceInitPrms);
    GT_assert(GT_DEFAULT_MASK, status == 0);

    Vps_printf(" I2C: System Init Done !!!\n");

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     PspI2c_deInit                                               
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
Int32 PspI2c_deInit()
{
    Int32 status;

#ifdef PLATFORM_EVM_SI
    Iss_platformDeviceDeInit();
#endif

    // status = FVID2_deInit(NULL);
    // GT_assert( GT_DEFAULT_MASK, status == 0 );

    Iss_platformDeInit();

    Vps_printf(" I2C: System De-Init Done !!!\n");
    Vps_printf(" \r\n");

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     PspI2c_tskMain                                               
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
Void PspI2c_tskMain(UArg arg0, UArg arg1)
{
    PspI2c_init();
    PspI2c_run();
    PspI2c_deInit();
}

/* Create test task */
/* ===================================================================
 *  @func     PspI2c_createTsk                                               
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
Void PspI2c_createTsk()
{
    Task_Params tskParams;

    /* 
     * Set to 0
     */
    memset(&gPspI2c_ctrl, 0, sizeof(gPspI2c_ctrl));

    /* 
     * Create test task
     */
    Task_Params_init(&tskParams);

    tskParams.priority = PSP_I2C_TSK_PRI_MAIN;
    tskParams.stack = gPspI2c_tskStackMain;
    tskParams.stackSize = sizeof(gPspI2c_tskStackMain);

    gPspI2c_ctrl.tskMain = Task_create(PspI2c_tskMain, &tskParams, NULL);

    GT_assert(GT_DEFAULT_MASK, gPspI2c_ctrl.tskMain != NULL);
}

/* 
 * Application main */
/* ===================================================================
 *  @func     main                                               
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
Int32 main(void)
{
    /* 
     * Create test task
     */
    PspI2c_createTsk();

    /* 
     * Start BIOS
     */
    BIOS_start();

    return (0);
}
