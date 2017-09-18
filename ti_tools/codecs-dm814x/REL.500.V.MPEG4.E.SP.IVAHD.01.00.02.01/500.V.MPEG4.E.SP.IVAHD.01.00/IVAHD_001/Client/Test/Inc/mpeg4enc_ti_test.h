/*
 ===========================================================================
 * HDVICP2 Based H.264 HP Encoder
 * 
 * "HDVICP2 Based H.264 HP Encoder" is software module developed on TI's 
 * HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw 
 * video into a high/main/baseline profile bit-stream.
 *
 * Copyright (C) {2009} Texas Instruments Incorporated - http://www.ti.com/ 
 * ALL RIGHTS RESERVED 
 *
 ===========================================================================
*/


/**  
 *****************************************************************************
 * @file mpeg4enc_ti_test.h                          
 *                                       
 * @brief This File contains function definitions for a standard 
 *        implementation of a test configuration file parser. 
 *        These functiosn parses the input  configuration files and 
 *        assigns user provided values to global instance structures
 *  
 *
 * @author: Radhesh Bhat (rama.mr@ti.com)
 *
 * @version 0.0 (Feb 2010) : Base version created
 *                           [Radhesh Bhat]
 *****************************************************************************
*/
#ifndef _MPEG4_ENC_TI_TEST_H_
#define _MPEG4_ENC_TI_TEST_H_

#include <ti/xdais/xdas.h>
#include <mpeg4enc.h>
#ifdef USE_DSS_SCRIPTING
#include <pci.h>
#endif

/** 
 *  Maximum size of the string to hold file name along with path
*/
#define FILE_NAME_SIZE 255
/** 
 *  Macro defining the maximum number of parameters to be parsed from the input
 *  configuration file
*/
#define MAX_ITEMS_TO_PARSE  256 

/** 
 *  Output buffer size, this buffer is used to place encoded data,should be 
 *  big enough to hold the size of  typical HD sequence
*/
#define OUTPUT_BUFFER_SIZE      0x1FA400

/** 
 *  Analytic info output buffer size, this buffer is used to place MV & SAD of
 *  encoded frame, should be big enough to hold the size of  typical HD sequence
*/
#define ANALYTICINFO_OUTPUT_BUFF_SIZE      0x00004


#define NUMBER_OF_SLICES      6500


#define INIT_BUFFER_SIZE 3000

/**
 * Some Random number is given to this channel, just to test 
 * XDM_SETLATEACQUIREARG
*/
#define CHANNEL_ID 0x100


#if !defined (USE_PCI)
#define ENABLE_RECON_DUMP  0
#else
#define ENABLE_RECON_DUMP  0
#endif

/*
 * User defined file handling functions
 */
#ifdef A9_HOST_FILE_IO
  #define MY_FOPEN   my_fopen
  #define MY_FCLOSE  my_fclose
  #define MY_FREAD   my_fread
  #define MY_FWRITE  my_fwrite
  #define MY_FSEEK   my_fseek
  #define MY_FTELL   my_ftell
  #define MY_FFLUSH  my_fflush
  typedef FILE MY_FILE;
#elif USE_DSS_SCRIPTING
  #define MY_FOPEN   PCIopen
  #define MY_FCLOSE  PCIclose
  #define MY_FREAD   PCIread
  #define MY_FWRITE  PCIwrite
  #define MY_FSEEK   PCIseek
  #define MY_FTELL   PCItell
  #define MY_FFLUSH  PCIflush
  typedef PCIFILE MY_FILE;
#else
  #define MY_FOPEN   fopen
  #define MY_FCLOSE  fclose
  #define MY_FREAD   fread
  #define MY_FWRITE  fwrite
  #define MY_FSEEK   fseek
  #define MY_FTELL   ftell
  #define MY_FFLUSH  fflush
  typedef FILE MY_FILE;
#endif


#endif /* #ifndef _MPEG4_ENC_TI_TEST_H_ */
