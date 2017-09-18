/** ==================================================================
 *  @file   issdrv_ov2710Api.c
 *                                                                    
 *  @path   /ti/psp/devices/ov2710/src/
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/devices/src/vpsdrv_devicePriv.h>
#include <ti/psp/devices/ov2710/src/issdrv_ov2710Priv.h>
#include <ti/psp/devices/ov2710/issdrv_ov2710_config.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/devices/src/issdrv_devicePriv.h>
#define LONG_EXPO                       0x3012
#define LONG_GAIN                       0x305e

/* Global object storing all information related to all
  OV2710 driver handles */
static Iss_Ov2710CommonObj 	gIss_Ov2710CommonObj;

static I2c_Ctrl 				gOv2710_I2c_ctrl;

extern ti2a_output 		ti2a_output_params;

/* Control API that gets called when FVID2_control is called
 * 
 * This API does handle level semaphore locking
 * 
 * handle - OV2710 driver handle cmd - command cmdArgs - command arguments
 * cmdStatusArgs - command status
 * 
 * returns error in case of - illegal parameters - I2C command RX/TX error */
Int32 Iss_device16AddWrite8 (UInt32 instId,
                           UInt32 devAddr,
                           UInt16 *regAddr,
			   UInt8 * regValue,
                           UInt32 numRegs)
{
    Int32 status;
    UInt32 regId;
    PSP_I2cXferParams i2cParams;
    UInt8 buffer[4];
    UInt8 tempBuf1,tempBuf2;

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

    status = Iss_deviceI2cLock(instId, TRUE);

    if(status==FVID2_SOK)
    {
        i2cParams.slaveAddr = devAddr;
        i2cParams.buffer    = buffer;
        i2cParams.bufLen    = 3;
        i2cParams.flags     = I2C_DEFAULT_WRITE;
        i2cParams.timeout   = ISS_DEVICE_I2C_TIMEOUT;
		
        for(regId=0; regId<numRegs; regId++)
        {
			tempBuf1 = (regAddr[regId]);
			tempBuf2 = ((regAddr[regId])>>8);
            buffer[0] = tempBuf2;
	    buffer[1] = tempBuf1;
			tempBuf1 = (regValue[regId]);
			//tempBuf2 = ((regValue[regId])>>8);
            buffer[2] = tempBuf1;
			//buffer[3] = tempBuf1;
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

        Iss_deviceI2cLock(instId, FALSE);
    }

    return status;
}

Int32 Iss_device16AddRead8 (UInt32 instId,
                           UInt32 devAddr,
                           UInt16 *regAddr,
			   UInt8 * regValue,
                           UInt32 numRegs)
{
    Int32 status;
    UInt32 regId;
    PSP_I2cXferParams i2cParams;
    UInt16 tempBuf;
    UInt8 buffer[2];
    UInt8 tempBuf1,tempBuf2;

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

    status = Iss_deviceI2cLock(instId, TRUE);

    if(status==FVID2_SOK)
    {
        i2cParams.slaveAddr = devAddr;
        i2cParams.bufLen    = 2;
        i2cParams.timeout   = ISS_DEVICE_I2C_TIMEOUT;
		i2cParams.buffer    = buffer;
        for(regId=0; regId<numRegs; regId++)
        {
			tempBuf1 = (regAddr[regId]);
			tempBuf2 = ((regAddr[regId])>>8);
            buffer[0] = tempBuf2;
			buffer[1] = tempBuf1;
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
			tempBuf1 = (regValue[regId]);
			tempBuf2 = ((regValue[regId])>>8);
            		buffer[0] = tempBuf2;
			buffer[1] = tempBuf1;
 //           i2cParams.buffer    = &regValue[regId];

		i2cParams.bufLen    = 1;
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
			
			tempBuf = buffer[0];
			//regValue[regId] = ((buffer[1])&0x00FF) | ((tempBuf<<8)&0xFF00);
			regValue[regId] = tempBuf;

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

        Iss_deviceI2cLock(instId, FALSE);
    }

    return status;

}

#if 0	// by denny
Uint16 Transplant_DRV_imgsCalcSW(int exposureTimeInUsecs)
{
	int hts = 2420;
	int	div_base = 1;
  int curFrameTime;
	Uint16 ret=0;
  
  curFrameTime = (int)((double)exposureTimeInUsecs/(double)33333*(double)(0x44a0));

  if(curFrameTime<1)
    curFrameTime = 1;

  if(curFrameTime>0x11280)
    curFrameTime = 0x11280 ;

	if(exposureTimeInUsecs <= 33333)
	{ 	
		div_base = 1;
	} 
	else if(exposureTimeInUsecs <= 66666)
	{		
		div_base = 2;
	} 
	else 
	{		
		div_base = 4;
	}

	hts *= div_base;
	//pFrame->SW /= div_base;
	ret = (div_base-1)<<4;

  //OSA_printf(" exposureTimeInUsecs = %d sw = %d\n", exposureTimeInUsecs,pFrame->SW);
  //OSA_printf(" div_base = 0x%X \n", div_base);

  return ret;
}
#endif


#if 0	// by denny
int Transplant_DRV_imgsSetEshutter(Uint32 eshutterInUsec, int setRegDirect)
{

  int status,i;
  Uint16 regAddr[20];
  int regValue=0; 
  Uint8 regValueSend[20]; 
  int regValue2=0; 
  Int32 devAddr;
  gOv2710_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
  devAddr = OV_2710_ADDR;

  if(setRegDirect) {
    regValue = eshutterInUsec;
  } else  {
     
    regValue = Transplant_DRV_imgsCalcSW(eshutterInUsec); 

    regValue=regValue>>4;
    regValue2 = regValue/16;
    if( regValue2 > 0x44a )
    {
	regValue2 = regValue2 - 0x44a;
    }else{
	regValue2 = 0;
    }
  }
  i = 0;
  regAddr[i] = GROUP_ACCESS; regValueSend[i] = 0x00; 		i++;

	//  regAddr[i] = PLL_CTRL01; regValueSend[i] = gDRV_imgsObj.curFrameTime.div_base;	i++;
  regAddr[i] = AEC_PK_EXPO1; regValueSend[i] = (regValue>>16)&0xff;	i++;
  regAddr[i] = AEC_PK_EXPO2; regValueSend[i] = (regValue>>8)&0xff; 	i++;  
  regAddr[i] = AEC_PK_EXPO3; regValueSend[i] = (regValue)&0xff; 		i++;

  regAddr[i] = AEC_PK_VTS_H; regValueSend[i] = (regValue2>>8)&0xff; 		i++;
  regAddr[i] = AEC_PK_VTS_L; regValueSend[i] = (regValue2)&0xff; 		i++;


  regAddr[i] = GROUP_ACCESS; regValueSend[i] = 0x10; 		i++;
  regAddr[i] = GROUP_ACCESS; regValueSend[i] = 0xA0; 		i++;

 status=Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,regAddr, regValueSend, i);
  
  if(status!= FVID2_SOK) {
    Vps_printf("DRV_i2cWrite16()\n");
    return status;
  } 
  return status;


}
#endif

#if 0 	// by denny
int Transplant_DRV_imgsReset()
{
  	Uint16 *regAddr=&gOv2710_I2c_ctrl.regAddr[0];
  	Uint8 *regValue=&gOv2710_I2c_ctrl.regValue[0];
 	int status, i,j;
  	Int32 delay = 100000;
  	Int32 devAddr;
    
	//char rawValue[300];
	i=0;  
	regAddr[i]  = PLL_CLOCK_SELECT;					regValue[i] = 0x93;  	i++;
	regAddr[i]  = SYSTEM_CONTROL00;					regValue[i] = 0x82;  	i++;
	regAddr[i]  = PAD_OUTPUT_ENABLE01; 				regValue[i] = 0x7f;  	i++;
	regAddr[i]  = PAD_OUTPUT_ENABLE02; 				regValue[i] = 0xfc;  	i++;
	regAddr[i]  = TIMING_CONTROL_HS_LOWBYTE; 			regValue[i] = 0xa0;  	i++;
	regAddr[i]  = SENSOR_RESV1;  					regValue[i] = 0x61;  	i++;
	regAddr[i]  = SENSOR_RESV2;  					regValue[i] = 0x0c;  	i++;
	regAddr[i]  = SENSOR_RESV3;  					regValue[i] = 0x6d;  	i++;
	regAddr[i]  = TIMING_CONTROL_HS_LOWBYTE; 			regValue[i] = 0xb4;  	i++;
	regAddr[i]  = ANA_ARRAY;  					regValue[i] = 0x04;  	i++;
	regAddr[i]  = SENSOR_RESV4;  					regValue[i] = 0x60;  	i++;
	regAddr[i]  = SENSOR_RESV5;  					regValue[i] = 0xa7;  	i++;
	regAddr[i]  = SENSOR_RESV6;  					regValue[i] = 0x26;  	i++;
	regAddr[i]  = SENSOR_RESV7;  					regValue[i] = 0x04;  	i++;
	regAddr[i]  = SENSOR_RESV8;  					regValue[i] = 0x37;/*0x07;*/  	i++;
	regAddr[i]  = SENSOR_RESV9;  					regValue[i] = 0x00;/*0x40;*/  	i++;
	regAddr[i]  = SENSOR_RESV10;  					regValue[i] = 0x9e;  	i++;
	regAddr[i]  = SENSOR_RESV11;  					regValue[i] = 0x74;  	i++;
	regAddr[i]  = SENSOR_RESV12;  					regValue[i] = 0x10;  	i++;
	regAddr[i]  = SENSOR_REG0D;  					regValue[i] = 0x07;  	i++;
	regAddr[i]  = SENSOR_RESV13;  					regValue[i] = 0x8b;  	i++;
	regAddr[i]  = SENSOR_RESV14;  					regValue[i] = 0x74;  	i++;
	regAddr[i]  = SENSOR_RESV15;  					regValue[i] = 0x9e;  	i++;
	regAddr[i]  = TIMING_CONTROL_HS_LOWBYTE; 			regValue[i] = 0xc4;  	i++;
	regAddr[i]  = SENSOR_RESV16;  					regValue[i] = 0x05;  	i++;
	regAddr[i]  = SENSOR_RESV17;  					regValue[i] = 0x12;  	i++;
	regAddr[i]  = SENSOR_RESV18;  					regValue[i] = 0x90;  	i++;
	regAddr[i]  = SENSOR_RESV19;  					regValue[i] = 0x40;  	i++;
	///updates1

	regAddr[i]  = SENSOR_RESV24;  					regValue[i] = 0x31;  	i++;
	
	regAddr[i]  = TIMING_CONTROL_HTS_LOWBYTE;			regValue[i] = 0x74;  	i++;
	regAddr[i]  = DVP_CTRL00;  					regValue[i] = 0x04;  	i++;
	
	regAddr[i]  = AWB_CONTROL_01;  					regValue[i] = 0x02;  	i++;
	regAddr[i]  = AWB_FRAME_COUNTER;				regValue[i] = 0x00;  	i++;
	regAddr[i]  = SENSOR_RESV20;  					regValue[i] = 0xff;  	i++;
	regAddr[i]  = SENSOR_RESV21;  					regValue[i] = 0x00;  	i++;
	regAddr[i]  = AEC_CONTROL0;  					regValue[i] = 0x78;  	i++;
	regAddr[i]  = PLL_CTRL00;  					regValue[i] = 0x88;  	i++;
	regAddr[i]  = PLL_CTRL02;  					regValue[i] = 0x28;  	i++;
	regAddr[i]  = SENSOR_RESV22;  					regValue[i] = 0x06;  	i++;
	regAddr[i]  = AEC_G_CEIL_H;  					regValue[i] = 0x00;  	i++;
	regAddr[i]  = AEC_G_CEIL_L;  					regValue[i] = 0x7a;  	i++;
	regAddr[i]  = AEC_PK_MANUAL;  					regValue[i] = 0x00;  	i++;
	regAddr[i]  = AEC_CONTROL13;  					regValue[i] = 0x54;  	i++;
	//updates2
	/*****used by prd26     ***/
	regAddr[i]  = SENSOR_RESV25; 					regValue[i] = 0x0F;  i++;
	regAddr[i]  = SENSOR_RESV26; 					regValue[i] = 0x1A;  i++;	
	
	
	regAddr[i]  = SENSOR_RESV23; 					regValue[i] = 0x03;  i++;
	
	regAddr[i]  = AVG_START_POSITION_AT_VERTICAL_H; 	regValue[i] = 0x07;  i++;
	regAddr[i]  = AVG_START_POSITION_AT_VERTICAL_L; 	regValue[i] = 0xa0;  i++;
	regAddr[i]  = AVG_END_POSITION_AT_VERTICAL_H; 		regValue[i] = 0x04;  i++;
	regAddr[i]  = AVG_END_POSITION_AT_VERTICAL_L; 		regValue[i] = 0x43;  i++;
	
	regAddr[i]  = AEC_CONTROLF; 				regValue[i] = 0x78;  i++;
	regAddr[i]  = AEC_CONTROL10; 				regValue[i] = 0x68;  i++;
	regAddr[i]  = AEC_CONTROL1B; 				regValue[i] = 0x78;  i++;
	regAddr[i]  = AEC_CONTROL1E; 				regValue[i] = 0x68;  i++;
	regAddr[i]  = AEC_CONTROL11; 				regValue[i] = 0xD0;  i++;
	regAddr[i]  = AEC_CONTROL1F; 				regValue[i] = 0x40;  i++;
	/***************************************/

	/*****************used by 26 trunk ************************
	regAddr[i]  = SENSOR_RESV25; 							regValue[i] = 0x0F;  i++;
	regAddr[i]  = SENSOR_RESV26; 							regValue[i] = 0x1A;  i++;	
	
	
	regAddr[i]  = SENSOR_RESV23; 							regValue[i] = 0x03;  i++;
	
	regAddr[i]  = AVG_START_POSITION_AT_VERTICAL_H; 	regValue[i] = 0x07;  i++;
	regAddr[i]  = AVG_START_POSITION_AT_VERTICAL_L; 	regValue[i] = 0xa0;  i++;
	regAddr[i]  = AVG_END_POSITION_AT_VERTICAL_H; 		regValue[i] = 0x04;  i++;
	regAddr[i]  = AVG_END_POSITION_AT_VERTICAL_L; 		regValue[i] = 0x43;  i++;
	
	regAddr[i]  = AEC_CONTROLF; 				regValue[i] = 0x78;  i++;
	regAddr[i]  = AEC_CONTROL10; 				regValue[i] = 0x68;  i++;
	regAddr[i]  = AEC_CONTROL1B; 				regValue[i] = 0x78;  i++;
	regAddr[i]  = AEC_CONTROL1E; 				regValue[i] = 0x68;  i++;
	regAddr[i]  = AEC_CONTROL11; 				regValue[i] = 0xD0;  i++;
	regAddr[i]  = AEC_CONTROL1F; 				regValue[i] = 0x40;  i++;
	*****************************************/

	/* BLC Setting */
	regAddr[i]  = BLC_CONTROL_00;					regValue[i] = 0x01;  	i++;
	regAddr[i]  = BLACK_LEVEL_TARGET_H;				regValue[i] = 0x00;  	i++;
	regAddr[i]  = BLACK_LEVEL_TARGET_L;				regValue[i] = 0x00;  	i++;//0x05;

	/* Disable AEC/AGC*/
	regAddr[i]  = AEC_PK_MANUAL;  					regValue[i] = 0x07;  	i++;
	
	/* Disable AWB */
	/*****************************************************************/
	regAddr[i]  = AWB_GAIN_PK_AWB;  				regValue[i] = 0x01;  	i++;	
	regAddr[i]	= AWB_GAIN_PK_RED_H;				regValue[i] = 0x04;  	i++;
	regAddr[i]	= AWB_GAIN_PK_RED_L;				regValue[i] = 0x00;  	i++;
	regAddr[i]	= AWB_GAIN_PK_GREEN_H;				regValue[i] = 0x04;  	i++;
	regAddr[i]	= AWB_GAIN_PK_GREEN_L;				regValue[i] = 0x00;  	i++;
	regAddr[i]	= AWB_GAIN_PK_BLUE_H; 				regValue[i] = 0x04;  	i++;
	regAddr[i]	= AWB_GAIN_PK_BLUE_L; 				regValue[i] = 0x00;  	i++;
	/*********************************************************************/
	/***************************************************************
	regAddr[i]  = AWB_GAIN_PK_AWB;  				regValue[i] = 0x01;  	i++;	
	regAddr[i]	= AWB_GAIN_PK_RED_H;				regValue[i] = 0x04;  	i++;
	regAddr[i]	= AWB_GAIN_PK_RED_L;				regValue[i] = 0x00;  	i++;
	regAddr[i]	= AWB_GAIN_PK_GREEN_H;				regValue[i] = 0x04;  	i++;
	regAddr[i]	= AWB_GAIN_PK_GREEN_L;				regValue[i] = 0x00;  	i++;
	regAddr[i]	= AWB_GAIN_PK_BLUE_H; 				regValue[i] = 0x04;  	i++;
	regAddr[i]	= AWB_GAIN_PK_BLUE_L; 				regValue[i] = 0x00;  	i++;
	*********************************************************************/

	//DBG_PrintforTest(gOv2710_I2c_ctrl.i2cInstId,"gOv2710_I2c_ctrl.i2cInstId");
	//DBG_PrintforTest(devAddr,"OV_2710_ADDR");


	
	
	//60fps
	regAddr[i] = TIMING_CONTROL_HTS_HIGHBYTE; 	regValue[i] = 0x09; 		i++;		
	regAddr[i] = TIMING_CONTROL_HTS_LOWBYTE; 	regValue[i] = 0x74; 		i++;
		



	gOv2710_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	devAddr = OV_2710_ADDR;

	gOv2710_I2c_ctrl.numRegs = 1;
	for(j=0; j<i; j++)
	{

		//regValue[i]=(regValue[i]<<8)&0xff00;	
		//regValue[i]=regValue[i]&0x00ff;		
		//status = Iss_deviceWrite16(gOv2710_I2c_ctrl.i2cInstId,devAddr,&regAddr[i],&regValue[i],gOv2710_I2c_ctrl.numRegs);
		status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&regAddr[j],&regValue[j],gOv2710_I2c_ctrl.numRegs);
		
		while(delay--){}
		delay = 100000;
	}
		//i=0;
			 
		//i = 0;
	/*

			0x00 : 30fps, 
			0x10 : 15fps, 

			0x20 : 10fps
	*/
	
	//regAddr[i]	= PLL_CTRL01;			regValue[i] = 0x20 ;	i++;
		regAddr[i]	= PLL_CTRL01;			regValue[i] = 0x00;	i++;

		//status = DRV_i2c16Write8(&gDRV_imgsObj.i2cHndl, regAddr, regValue, i);	
		status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&regAddr[i],&regValue[i],1);
	/*		
	Uint16 MyregAddr[i];
	memset(MyregAddr,0,sizeof(Uint16)*i);
	Uint8 MyregVal[i];
	memset(MyregVal,0,sizeof(Uint8)*i);
	MyregAddr[0]=0x300A;
	MyregAddr[1]=0x300B;

	//status = Iss_deviceRead16(gOv2710_I2c_ctrl.i2cInstId,devAddr,&MyregAddr[0],&MyregVal[0],1);
	//status = Iss_deviceRead16(gOv2710_I2c_ctrl.i2cInstId,devAddr,&MyregAddr[1],&MyregVal[1],1);
	Iss_device16AddRead8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&MyregAddr[0],&MyregVal[0],1);
	Iss_device16AddRead8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&MyregAddr[1],&MyregVal[1],1);



	//MyregVal[2]=((MyregVal[0]<<8)&0xff00)|(MyregVal[1]&0x00ff);


	for(j=0; j<i; j++)
	{

		//regValue[i]=(regValue[i]<<8)&0xff00;	
		//regValue[i]=regValue[i]&0x00ff;		
		//status = Iss_deviceWrite16(gOv2710_I2c_ctrl.i2cInstId,devAddr,&regAddr[i],&regValue[i],gOv2710_I2c_ctrl.numRegs);
		//status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&regAddr[i],&regValue[i],gOv2710_I2c_ctrl.numRegs);
		Iss_device16AddRead8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&MyregAddr[j],&MyregVal[j],1);


		while(delay--){}
		delay = 100000;
	}	
	*/

	


  return status;
}
#endif
/* ===================================================================
 *  @func     Iss_Ov2710Control
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
Int32 Iss_Ov2710Control ( Fdrv_Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Iss_Ov2710Obj *pObj = ( Iss_Ov2710Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * lock handle
     */
    Iss_Ov2710LockObj ( pObj );

    switch ( cmd )
    {
        case FVID2_START:
       //     status = Iss_Ov2710Start ( pObj );
            break;

        case FVID2_STOP:
      //      status = Iss_Ov2710Stop ( pObj );
            break;

        case IOCTL_ISS_SENSOR_GET_CHIP_ID:
      //      status = Iss_Ov2710GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_ISS_SENSOR_RESET:
      //      status = Iss_Ov2710Reset ( pObj );
            break;

        case IOCTL_ISS_SENSOR_REG_WRITE:
            status = Iss_Ov2710RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_REG_READ:
            status = Iss_Ov2710RegRead ( pObj, cmdArgs );
            break;

        case IOCTL_ISS_SENSOR_UPDATE_EXP_GAIN:
             status = Iss_Ov2710UpdateExpGain ( pObj, cmdArgs);
             break;

		case IOCTL_ISS_SENSOR_UPDATE_ITT:
          //   status = Iss_Ov2710UpdateItt ( pObj, cmdArgs);
             break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /*
     * unlock handle
     */
    Iss_Ov2710UnlockObj ( pObj );

    return status;
}

/* 
 * Create API that gets called when FVID2_create is called
 * 
 * This API does not configure the OV2710 is any way.
 * 
 * This API - validates parameters - allocates driver handle - stores create
 * arguments in its internal data structure.
 * 
 * Later the create arguments will be used when doing I2C communcation with
 * OV2710
 * 
 * drvId - driver ID, must be FVID2_ISS_SENSOR_OV2710_DRV instanceId - must
 * be 0 createArgs - create arguments createStatusArgs - create status
 * fdmCbParams - NOT USED, set to NULL
 * 
 * returns NULL in case of any error */
/* ===================================================================
 *  @func     Iss_Ov2710Create
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
Fdrv_Handle Iss_Ov2710Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams * fdmCbParams )
{
    Iss_Ov2710Obj *pObj;
    Iss_SensorCreateParams *sensorCreateArgs
        = ( Iss_SensorCreateParams * ) createArgs;

    Iss_SensorCreateStatus *sensorCreateStatus
        = ( Iss_SensorCreateStatus * ) createStatusArgs;

    /*
     * check parameters
     */
    if ( sensorCreateStatus == NULL )
        return NULL;

    sensorCreateStatus->retVal = FVID2_SOK;

    if ( drvId != FVID2_ISS_SENSOR_OV2710_DRV
         || instanceId != 0 || sensorCreateArgs == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    if ( sensorCreateArgs->deviceI2cInstId >= ISS_DEVICE_I2C_INST_ID_MAX )
    {
        sensorCreateStatus->retVal = FVID2_EINVALID_PARAMS;
        return NULL;
    }

    /*
     * allocate driver handle
     */
    pObj = Iss_Ov2710AllocObj (  );
    if ( pObj == NULL )
    {
        sensorCreateStatus->retVal = FVID2_EALLOC;
        return NULL;
    }

    /*
     * copy parameters to allocate driver handle
     */
    memcpy ( &pObj->createArgs, sensorCreateArgs, sizeof ( *sensorCreateArgs ) );

    Iss_Ov2710ResetRegCache(pObj);

    /*
     * return driver handle object pointer
     */
    return pObj;
}

/* 
 * Delete function that is called when FVID2_delete is called
 * 
 * This API - free's driver handle object
 * 
 * handle - driver handle deleteArgs - NOT USED, set to NULL
 * 
 */
/* ===================================================================
 *  @func     Iss_Ov2710Delete
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
Int32 Iss_Ov2710Delete ( Fdrv_Handle handle, Ptr deleteArgs )
{
    Iss_Ov2710Obj *pObj = ( Iss_Ov2710Obj * ) handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /*
     * free driver handle object
     */
    Iss_Ov2710FreeObj ( pObj );

    return FVID2_SOK;
}

/* 
 * System init for OV2710 driver
 * 
 * This API - create semaphore locks needed - registers driver to FVID2
 * sub-system - gets called as part of Iss_deviceInit() */

/* ===================================================================
 *  @func     Iss_Ov2710Init
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
Int32 Iss_Ov2710Init (  )
{
    Semaphore_Params semParams;

    Int32 status = FVID2_SOK;

    Int32 count;

    Int32 devAddr;
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gIss_Ov2710CommonObj, 0, sizeof ( gIss_Ov2710CommonObj ) );

    /*
     * Create global OV2710 lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gIss_Ov2710CommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gIss_Ov2710CommonObj.lock == NULL )
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
        /*
         * Register OV2710 driver with FVID2 sub-system
         */
		gOv2710_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
		Vps_printf("\ngOv2710_I2c_ctrl.i2cInstId is %d\n",gOv2710_I2c_ctrl.i2cInstId);
		devAddr = OV_2710_ADDR;

		for(count=0; count< REGISTERNUMBER/*122*/; count++){
			gOv2710_I2c_ctrl.regAddr[count] = SensorConfigScript_1080p30[count][0];
			gOv2710_I2c_ctrl.regValue[count] = SensorConfigScript_1080p30[count][2];
			gOv2710_I2c_ctrl.numRegs = 1;

			if(SensorConfigScript_1080p30[count][1] == 2){
				status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,
						&gOv2710_I2c_ctrl.regAddr[count],&gOv2710_I2c_ctrl.regValue[count],
						gOv2710_I2c_ctrl.numRegs);

                {
                    UInt8 value = 0;
                    status = Iss_device16AddRead8(gOv2710_I2c_ctrl.i2cInstId,devAddr,
						&gOv2710_I2c_ctrl.regAddr[count],&value,gOv2710_I2c_ctrl.numRegs);

                }
			}
		}
		Vps_printf("Iss_device16AddWrite8 finished %x\n", count);
		
#if 0 //dm385 evm		
		gOv2710_I2c_ctrl.regAddr[0] = 0x80;
		gOv2710_I2c_ctrl.regValue[0] = 0x00;
		gOv2710_I2c_ctrl.numRegs = 1;
		Iss_deviceWrite8(gOv2710_I2c_ctrl.i2cInstId,0x20,&gOv2710_I2c_ctrl.regAddr[0],&gOv2710_I2c_ctrl.regValue[0],gOv2710_I2c_ctrl.numRegs);
#endif		
		

        gIss_Ov2710CommonObj.fvidDrvOps.create = ( FVID2_DrvCreate ) Iss_Ov2710Create;
        gIss_Ov2710CommonObj.fvidDrvOps.delete = Iss_Ov2710Delete;
        gIss_Ov2710CommonObj.fvidDrvOps.control = Iss_Ov2710Control;
        gIss_Ov2710CommonObj.fvidDrvOps.queue = NULL;
        gIss_Ov2710CommonObj.fvidDrvOps.dequeue = NULL;
        gIss_Ov2710CommonObj.fvidDrvOps.processFrames = NULL;
        gIss_Ov2710CommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gIss_Ov2710CommonObj.fvidDrvOps.drvId = FVID2_ISS_SENSOR_OV2710_DRV;

		Vps_printf("ov2710 FVID2_registerDriver start\n");
        status = FVID2_registerDriver ( &gIss_Ov2710CommonObj.fvidDrvOps );
		Vps_printf("ov2710 FVID2_registerDriver end\n");

        if ( status != FVID2_SOK )
        {
            /*
             * Error - free acquired resources
             */
            Semaphore_delete ( &gIss_Ov2710CommonObj.lock );
        }
    }

    if ( status != FVID2_SOK )
    {
        Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                        __LINE__ );
    }

    return status;
}

/* 
 * System de-init for OV2710 driver
 * 
 * This API - de-registers driver from FVID2 sub-system - delete's allocated
 * semaphore locks - gets called as part of Iss_deviceDeInit() */
/* ===================================================================
 *  @func     Iss_Ov2710DeInit
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
Int32 Iss_Ov2710DeInit (  )
{
    /*
     * Unregister FVID2 driver
     */
    FVID2_unRegisterDriver ( &gIss_Ov2710CommonObj.fvidDrvOps );

    /*
     * Delete semaphore's
     */
    Semaphore_delete ( &gIss_Ov2710CommonObj.lock );

    return 0;
}

/* ===================================================================
 *  @func     Iss_Ov2710PinMux
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

Int32 Iss_Ov2710PinMux (  )
{
	/* setup CAM input pin mux */
	*PINCNTL156 = 0x00050002;				// select function 2 with receiver enabled and pullup/down disabled  - only works in supervisor mode
	*PINCNTL157 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL158 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL159 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL160 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL161 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL162 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL163 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL164 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL165 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL166 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL167 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL168 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL169 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL170 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL171 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL172 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL173 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL174 = 0x00050002;				// select function 2  - only works in supervisor mode
	*PINCNTL175 = 0x00050002;				// select function 2  - only works in supervisor mode
#if 0
   *(UInt32 *)0x48140AAC = 0x00060002;    // CAM_HSYNC        PINCNTL172[1]
    *(UInt32 *)0x48140AB0 = 0x00060002;    // CAM_VSYNC        PINCNTL173[1]
    *(UInt32 *)0x48140AB8 = 0x00060002;    // CAM_PCLK         PINCNTL175[1]
    *(UInt32 *)0x48140A60 = 0x00060080;    // gpio2[18]        PINCNTL153[7] // 0x00060080   CAM_RST          PINCNTL153[5]
   // *(UInt32 *)0x48140A58 = 0x00060020;    // CAM_WEn          PINCNTL151[5] cam_de_mux1
   *(UInt32 *)0x48140A64 = 0x00060020;    // CAM_STROBE       PINCNTL154[5]
    *(UInt32 *)0x48140A68 = 0x00060020;    // CAM_SHTR         PINCNTL155[5]
    *(UInt32 *)0x48140AA8 = 0x00060002;    // CAM_D0           PINCNTL171[1]
    *(UInt32 *)0x48140AA4 = 0x00060002;    // CAM_D1           PINCNTL170[1]
    *(UInt32 *)0x48140AA0 = 0x00060002;    // CAM_D2           PINCNTL169[1]
    *(UInt32 *)0x48140A9C = 0x00060002;    // CAM_D3           PINCNTL168[1]
    *(UInt32 *)0x48140A98 = 0x00060002;    // CAM_D4           PINCNTL167[1]
    *(UInt32 *)0x48140A94 = 0x00060002;    // CAM_D5           PINCNTL166[1]
    *(UInt32 *)0x48140A90 = 0x00060002;    // CAM_D6           PINCNTL165[1]
    *(UInt32 *)0x48140A8C = 0x00060002;    // CAM_D7           PINCNTL164[1]
    *(UInt32 *)0x48140A6C = 0x00060002;    // CAM_D8           PINCNTL156[1]
    *(UInt32 *)0x48140A70 = 0x00060002;    // CAM_D9           PINCNTL157[1]
    *(UInt32 *)0x48140A74 = 0x00060002;    // CAM_D10          PINCNTL158[1]
    *(UInt32 *)0x48140A78 = 0x00060002;    // CAM_D11          PINCNTL159[1]
#endif

	/* setup I2C2 pin mux */
	*PINCNTL135 |= 0x00000040;				// select function 6  - only works in supervisor mode
	*PINCNTL136 |= 0x00000040;				// select function 6  - only works in supervisor mode
	
	return 0;
}
/* 
 * Handle level lock */
/* ===================================================================
 *  @func     Iss_Ov2710LockObj
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
Int32 Iss_Ov2710LockObj ( Iss_Ov2710Obj * pObj )
{
    Semaphore_pend ( pObj->lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/* 
 * Handle level unlock */
/* ===================================================================
 *  @func     Iss_Ov2710UnlockObj
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
Int32 Iss_Ov2710UnlockObj ( Iss_Ov2710Obj * pObj )
{
    Semaphore_post ( pObj->lock );

    return FVID2_SOK;
}

/* 
 * Global driver level lock */
/* ===================================================================
 *  @func     Iss_Ov2710Lock
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
Int32 Iss_Ov2710Lock (  )
{
    Semaphore_pend ( gIss_Ov2710CommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/* 
 * Global driver level unlock */
/* ===================================================================
 *  @func     Iss_Ov2710Unlock
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
Int32 Iss_Ov2710Unlock (  )
{
    Semaphore_post ( gIss_Ov2710CommonObj.lock );

    return FVID2_SOK;
}

/* 
 * Allocate driver object
 * 
 * Searches in list of driver handles and allocate's a 'NOT IN USE' handle
 * Also create's handle level semaphore lock
 * 
 * return NULL in case handle could not be allocated */
/* ===================================================================
 *  @func     Iss_Ov2710AllocObj
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
Iss_Ov2710Obj *Iss_Ov2710AllocObj (  )
{
    UInt32 handleId;

    Iss_Ov2710Obj *pObj;

    Semaphore_Params semParams;

    UInt32 found = FALSE;

    /*
     * Take global lock to avoid race condition
     */
    Iss_Ov2710Lock (  );

    /*
     * find a unallocated object in pool
     */
    for ( handleId = 0; handleId < ISS_DEVICE_MAX_HANDLES; handleId++ )
    {

        pObj = &gIss_Ov2710CommonObj.Ov2710Obj[handleId];

        if ( pObj->state == ISS_OV2710_OBJ_STATE_UNUSED )
        {
            /*
             * free object found
             */

            /*
             * init to 0's
             */
            memset ( pObj, 0, sizeof ( *pObj ) );

            /*
             * init state and handle ID
             */
            pObj->state = ISS_OV2710_OBJ_STATE_IDLE;
            pObj->handleId = handleId;

            /*
             * create driver object specific semaphore lock
             */
            Semaphore_Params_init ( &semParams );

            semParams.mode = Semaphore_Mode_BINARY;

            pObj->lock = Semaphore_create ( 1u, &semParams, NULL );

            found = TRUE;

            if ( pObj->lock == NULL )
            {
                /*
                 * Error - release object
                 */
                found = FALSE;
                pObj->state = ISS_OV2710_OBJ_STATE_UNUSED;
            }

            break;
        }
    }

    /*
     * Release global lock
     */
    Iss_Ov2710Unlock (  );

    if ( found )
        return pObj;                                       /* Free object
                                                            * found return it 
                                                            */

    /*
     * free object not found, return NULL
     */
    return NULL;
}

/* 
 * De-Allocate driver object
 * 
 * Marks handle as 'NOT IN USE' Also delete's handle level semaphore lock */
/* ===================================================================
 *  @func     Iss_Ov2710FreeObj
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
Int32 Iss_Ov2710FreeObj ( Iss_Ov2710Obj * pObj )
{
    /*
     * take global lock
     */
    Iss_Ov2710Lock (  );

    if ( pObj->state != ISS_OV2710_OBJ_STATE_UNUSED )
    {
        /*
         * mark state as unused
         */
        pObj->state = ISS_OV2710_OBJ_STATE_UNUSED;

        /*
         * delete object locking semaphore
         */
        Semaphore_delete ( &pObj->lock );
    }

    /*
     * release global lock
     */
    Iss_Ov2710Unlock (  );

    return FVID2_SOK;
}

/* 
 * Update exposure and gain value from the 2A */
/* ===================================================================
 *  @func     Iss_Ov2710UpdateExpGain
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
Int32 Iss_Ov2710UpdateExpGain ( Iss_Ov2710Obj * pObj, Ptr createArgs )
{
	static int first = 1;
	
	if (first)
	{
		*(unsigned int *)(0x5505144C) = 0x60267CF;
	}
  //      Vps_printf("Exp gain: %d Exp: %d",ti2a_output_params.sensorGain,ti2a_output_params.sensorExposure);

	Int32 status = FVID2_SOK;
	Int32 exp_time_rows = 0;
	Int32 exp_time_rows_max;
	Int32 g_int;
	Int32 c_int;
	UInt32 i2cInstId = Iss_platformGetI2cInstId();
	Int32 devAddr = OV_2710_ADDR;
	Int32 count = 0;


	gOv2710_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();

	//PixelClock = 160;
	//LineLength = 2177;
	exp_time_rows_max = 960;

	g_int = OV2710_GainTableMap(ti2a_output_params.sensorGain);

	c_int = (ti2a_output_params.sensorExposure)/70;  // exposure is in unit of 1/16 line
	//f_int = PixelClock - (c_int * LineLength)/(ti2a_output_params.sensorExposure);


	// clamp the calculated exposure time to its maximum value
	if( exp_time_rows > exp_time_rows_max )
	{
		exp_time_rows = exp_time_rows_max;
	}

	/*
     * take global lock
     */
    Iss_Ov2710Lock (  );
    {
    	extern Int32 aewbVendor;
		   	if (ti2a_output_params.mask) 
			{
				gOv2710_I2c_ctrl.regAddr[count] = 0x10;
				gOv2710_I2c_ctrl.regValue[count] = (c_int & 0xFF);
				gOv2710_I2c_ctrl.numRegs = 1;
				//Rfile_printf("sensor Exposure Time reg 0 value   :  < %5d, %5d > \n",gOv2710_I2c_ctrl.regValue[count],c_int );
				status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&gOv2710_I2c_ctrl.regAddr[count],
									&gOv2710_I2c_ctrl.regValue[count],gOv2710_I2c_ctrl.numRegs);

				gOv2710_I2c_ctrl.regAddr[count] = 0xF;
				gOv2710_I2c_ctrl.regValue[count] = ((c_int >> 8) & (0xFF));
				gOv2710_I2c_ctrl.numRegs = 1;
				//Rfile_printf("sensor Exposure Time reg 1 value   :  < %5d, %5d> \n",gOv2710_I2c_ctrl.regValue[count] ,c_int );
				status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&gOv2710_I2c_ctrl.regAddr[count],
									&gOv2710_I2c_ctrl.regValue[count],gOv2710_I2c_ctrl.numRegs);
			}
		    if (ti2a_output_params.mask) 
			{
				gOv2710_I2c_ctrl.regAddr[count] = 0x0;
				gOv2710_I2c_ctrl.regValue[count] = g_int;
				gOv2710_I2c_ctrl.numRegs = 1;

			    //Rfile_printf("sensor Analog gain reg value  :  < 0x%2x > \n",gOv2710_I2c_ctrl.regValue[count]  );
				status = Iss_device16AddWrite8(gOv2710_I2c_ctrl.i2cInstId,devAddr,&gOv2710_I2c_ctrl.regAddr[count],
									&gOv2710_I2c_ctrl.regValue[count],gOv2710_I2c_ctrl.numRegs);
			}
		}
     /*
     * release global lock
     */
    Iss_Ov2710Unlock (  );

	return status;

}
#if 0
/*
  Update ITT Values
*/
Int32 Iss_Ov2710UpdateItt ( Iss_Ov2710Obj * pObj, Ptr createArgs )
{
	int status,devAddr,count  = 0;
	I2c_Ctrl 				ov2710_I2c_ctrl;

	ov2710_I2c_ctrl.i2cInstId = Iss_platformGetI2cInstId();
	devAddr = OV_2710_ADDR;
	if(gItt_RegisterParams.Control == 1){
	ov2710_I2c_ctrl.regAddr[count]  = gItt_RegisterParams.regAddr;
	ov2710_I2c_ctrl.regValue[count] = gItt_RegisterParams.regValue;
	ov2710_I2c_ctrl.numRegs = 1;

	status = Iss_deviceWrite8(ov2710_I2c_ctrl.i2cInstId,devAddr,&ov2710_I2c_ctrl.regAddr[count],&ov2710_I2c_ctrl.regValue[count],ov2710_I2c_ctrl.numRegs);
	}
	else if(gItt_RegisterParams.Control == 0){

	ov2710_I2c_ctrl.regAddr[count]  = gItt_RegisterParams.regAddr;
	ov2710_I2c_ctrl.regValue[count] = 0;
	ov2710_I2c_ctrl.numRegs = 1;
	status = Iss_deviceRead8(ov2710_I2c_ctrl.i2cInstId,devAddr,&ov2710_I2c_ctrl.regAddr[count],&ov2710_I2c_ctrl.regValue[count],ov2710_I2c_ctrl.numRegs);
	count  = 0;
	gItt_RegisterParams.regValue = ov2710_I2c_ctrl.regValue[count];
	}
	return status;
}
#endif
