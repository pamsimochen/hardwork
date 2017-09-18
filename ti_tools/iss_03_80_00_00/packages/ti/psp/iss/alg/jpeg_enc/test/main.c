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
#include <ti/psp/iss/alg/jpeg_enc/inc/msp.h>
#include <ti/psp/iss/core/msp_types.h>
#include <ti/psp/iss/alg/jpeg_enc/inc/msp_jpege_common.h>
#include "main.h"

#pragma DATA_ALIGN(gGLBCE_TEST_tskStackMain, 32)
#pragma DATA_SECTION(gGLBCE_TEST_tskStackMain, ".bss:taskStackSection")
UInt8 gGLBCE_TEST_tskStackMain[GLBCE_TEST_TSK_STACK_MAIN];

Int32 aewbVendor = 0;

GLBCE_TEST_Ctrl gGLBCE_TEST_ctrl;

Int32 HEIGHT = 256;
Int32 WIDTH  = 256;
void *MEMUTILS_getPhysicalAddr(void* Addr)
{
   return(Addr);
}

void JPEG_Test_run()
{
    MSP_COMPONENT_TYPE *jcomponent;
    FILE *fp;
	FVID2_Frame inputImageBuffer;
	unsigned char * lumaPtr;
	unsigned char * chromaPtr;
	unsigned char * bits;
	unsigned int filesize;
	unsigned int paddingBytes;
	unsigned int isChromaFormat420 = 0;
	
	
    MSP_JPEGE_INC_init(&jcomponent);
		
		
    lumaPtr = MSP_JPEGE_Malloc(sizeof(unsigned char)* WIDTH*HEIGHT, 16);
	chromaPtr = MSP_JPEGE_Malloc(sizeof(unsigned char)* WIDTH*HEIGHT/2, 16);
	bits  = MSP_JPEGE_Malloc(sizeof(unsigned char)* WIDTH*HEIGHT, 16);
	
	printf("Hello World!!!!!!!!!!!!!!!!\n");
	printf("Reading the Input File...........\n");
	//Read the 420 input
	if(isChromaFormat420)
	{
	   FILE *fp;
	   fp = fopen("X://DM812x//IPNetCam_rel_1_8//ti_tools//iss_algorithms//jpeg_enc//test//vectors//sfish_p256x256_30fps_420pl_60fr_nv12.yuv", "rb");
	   if(fp == NULL) { printf("ERROR FILE opening\n"); exit(0); }
	   fread(lumaPtr, WIDTH*HEIGHT, sizeof(unsigned char), fp);
	   fread(chromaPtr, WIDTH*HEIGHT/2, sizeof(unsigned char), fp);
	   fclose(fp);
	
	}
	else //Read the 422 	
	{
	   FILE *fp;
	   fp = fopen("X://DM812x//IPNetCam_rel_1_8//ti_tools//iss_algorithms//jpeg_enc//test//vectors//tulips_uyvy422_prog_packed_128x128.yuv", "rb");
	   if(fp == NULL) { printf("ERROR FILE opening\n"); exit(0); }
	   WIDTH = HEIGHT = 128;
	   fread(lumaPtr, WIDTH*HEIGHT*2, sizeof(unsigned char), fp);
	   fclose(fp);
	
	}
	
	inputImageBuffer.addr[0][0] = lumaPtr;
	inputImageBuffer.addr[0][1] = chromaPtr;

	printf("Encoding...........\n");
	if(isChromaFormat420)
		filesize = McFW_JPEG_Enc(jcomponent, &inputImageBuffer, bits, WIDTH, HEIGHT, WIDTH, 0, 0, 80, WIDTH*HEIGHT, &paddingBytes, isChromaFormat420);
	else
	    filesize = McFW_JPEG_Enc(jcomponent, &inputImageBuffer, bits, WIDTH, HEIGHT, WIDTH*2, 0, 0, 80, WIDTH*HEIGHT, &paddingBytes, isChromaFormat420);
		
	
	printf("Writing the Output File...........\n");
	//Save the output
	{
	   FILE *fp;
	   fp = fopen("X://DM812x//IPNetCam_rel_1_8//ti_tools//iss_algorithms//jpeg_enc//test//vectors//output//OutputImage.jpg", "wb");
	   if(fp == NULL) { printf("ERROR FILE opening\n"); exit(0); }
	   fwrite(bits+paddingBytes, filesize, sizeof(unsigned char), fp);
	   fclose(fp);
	
	}
	
	MSP_JPEGE_INC_deInit(jcomponent);
	
	printf("Done...........\n");
	
     
}




/* ===================================================================
 *  @func     GLBCE_TEST_tskMain                                               
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
Void GLBCE_TEST_tskMain(UArg arg0, UArg arg1)
{
    FVID2_init(0);
    Iss_init();
	JPEG_Test_run();
	//box_car_main();
	//glbce_main();
	//EVF_test();
    Iss_deInit();
    exit(0);
}

/* Create test task */
/* ===================================================================
 *  @func     GLBCE_TEST_createTsk                                               
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
Void GLBCE_TEST_createTsk()
{
    Task_Params tskParams;

    /* 
     * Set to 0
     */
    memset(&gGLBCE_TEST_ctrl, 0, sizeof(gGLBCE_TEST_ctrl));

    /* 
     * Create test task
     */
    Task_Params_init(&tskParams);

    tskParams.priority = GLBCE_TEST_TSK_PRI_MAIN;
    tskParams.stack = gGLBCE_TEST_tskStackMain;
    tskParams.stackSize = sizeof(gGLBCE_TEST_tskStackMain);

    gGLBCE_TEST_ctrl.tskMain = Task_create(GLBCE_TEST_tskMain, &tskParams, NULL);

    GT_assert(GT_DEFAULT_MASK, gGLBCE_TEST_ctrl.tskMain != NULL);
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
    GLBCE_TEST_createTsk();

    /* 
     * Start BIOS
     */
    BIOS_start();

    return (0);
}
