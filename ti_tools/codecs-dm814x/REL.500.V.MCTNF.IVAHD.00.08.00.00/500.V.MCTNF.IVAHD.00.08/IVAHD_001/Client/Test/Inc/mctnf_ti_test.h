/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from a
 *  4:2:0 semi planar Raw data.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

/**
********************************************************************************
 * @file mctnf_ti_test.h
 *
 * @brief This is top level client file for IVAHD MCTNF
 *        MCTNF Call using IVIDNF1 XDM Interface
 *
 * @author: Shyam Jagannathan (shyam.jagannathan@ti.com)
 *
 ********************************************************************************
*/

#ifndef _MCTNF_TI_TEST_H_
#define _MCTNF_TI_TEST_H_

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <mctnf.h>

/**
 *  Maximum size of the string to hold file name along with path
*/
 #define FILE_NAME_SIZE 255
/**
 *  Macro defining the maximum number of parameters to be parsed from the input
 *  configuration file
*/
#define MAX_ITEMS_TO_PARSE  512

/**
 *  Output buffer size, this buffer is used to place filtered data,should be
 *  big enough to hold the size of  typical HD sequence
*/
#define OUTPUT_BUFFER_SIZE      0x0600000

/**
 *  Analytic info output buffer size, this buffer is used to place MV & SAD of
 *  filtered frame, should be big enough to hold the size of  typical HD sequence
*/
#define ANALYTICINFO_OUTPUT_BUFF_SIZE      0x00028000

/**
 * META DATA index for user define SEI is fixed to 0 in this test application.
 * But it can be any. This is to have simplified implemntation of test app
*/
#define TEST_SEI_META_DATA_IDX   (0)

/**
 * META DATA index for user define SM is fixed to 1 in this test application.
 * But it can be any. This is to have simplified implemntation of test app
*/
#define TEST_UD_SM_META_DATA_IDX (1)


/**
 * Some Random number is given to this channel, just to test
 * XDM_SETLATEACQUIREARG
*/
#define CHANNEL_ID 0x100

/**
 * Total number of blocks possible in a process call
 */
#define TOT_BLOCKS_IN_PROCESS 0x4000

/**
********************************************************************************
 *  @fn     TestApp_SetInitParams
 *  @brief  Function to set default parameters
 *          This function will set default params for params function and
 *          dynamicparams function. This must be called before parsing config
 *          file so that default values are set when config file does not
 *          specify any value
 *
 *  @param[in] params : pointer to the XDM params interface structure
 *
 *  @param[in] dynamicParams : Pointer to t he XDM dynamicparams structure
 *
 *  @return    None
********************************************************************************
*/
XDAS_Void TestApp_SetInitParams(MCTNF_Params *params,
                                          MCTNF_DynamicParams *dynamicParams);


/**
********************************************************************************
 *  @fn     readparamfile
 *  @brief  Reads the entire param file contents into a global buffer,which is
 *          used for parsing and updates the params to given addresses.
 *
 *  @param[in]  file name : Name of the configuration file with path
 *
 *  @return     0 - if successfully parsed all the elements in param file and
 *                  their  values read into the memory addresses given in
 *                  token mappign array.
 *             XDM_EFAIL - For any file read operation related errors or if
 *                  unknown parameter names are entered or if the parameter
 *                  file syntax is not in compliance with the below
 *                  implementation.
********************************************************************************
*/

XDAS_Int32 readparamfile(XDAS_Int8 *configFile);

/**
********************************************************************************
 *  @fn     MCTNF_TI_Report_Error
 *  @brief  This function will print error messages
 *
 *          This function will check for MCTNF errors which are mapped to
 *          extended errors in vidnf1status structure and prints them in cosole
 *          Returns XDM_EFAIL in case of fatal error
 *
 *  @param[in]  uiErrorMsg  : Extended error message
 *
 *  @param[in]  fTrace_file : File pointer to the trace log file
 *
 *  @return     XDM_EOK -  when there is no fatal error
 *              XDM_EFAIL - when it is fatal error
********************************************************************************
*/

XDAS_Int32 MCTNF_TI_Report_Error(FILE * fTrace_file,XDAS_Int32 uiErrorMsg);


#endif /* #ifndef _MCTNF_TI_TEST_H_ */
