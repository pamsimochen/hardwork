/*
********************************************************************************
 * HDVICP2.0 Based MPEG4 SP Encoder
 *
 * "HDVICP2.0 Based MPEG4 SP Encoder" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw
 *  video into a simple profile bit-stream. Based on ISO/IEC 14496-2."
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/
/**
********************************************************************************
 * @file <TestAppComDataType.h>
 *
 * @brief data type definitions used by the Test Applications
 *
 * @author: Venugopala Krishna   <venugopala@ti.com, TI India>
 *
 * @version 0.0 (Jun 2009) : Initial version
 *                           [Venugopala Krishna]
 *
 *******************************************************************************
*/

/*--------------------- compilation control switches -------------------------*/
#ifndef _TESTAPPDATATYPE_
#define _TESTAPPDATATYPE_

/**
 *  Type casting the ANSI C const to Const
*/
#define Const const
/**
 *  Type casting the ANSI C void to void
*/
#define Void void
/**
 *  Macro to find the maximum among two numbers
*/
#define mmax(a, b) ((a) > (b) ? (a) : (b))
/**
 *  Macro to find the minimum among two numbers
*/
#define mmin(a, b) ((a) < (b) ? (a) : (b))
/**
 *  Macro to find the sign of a number
*/
#define sign(a)         ((a) < 0 ? -1 : 1)
/**
 *  Typecasting long long data type to S64
*/
#define S64 long long

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/*--------------------- program files ----------------------------------------*/
#include <tistdtypes.h>

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/**
 *  Type defining unsigned character pointer
*/
typedef unsigned char * pU8;



/**
 *  Type defining signed character
*/
typedef char S01;
/**
 *  Type defining signed character
*/
typedef char S02;
/**
 *  Type defining signed character
*/
typedef char S03;
/**
 *  Type defining signed character
*/
typedef char S04;
/**
 *  Type defining signed character
*/
typedef char S05;
/**
 *  Type defining signed character
*/
typedef char S06;
/**
 *  Type defining signed character
*/
typedef char S07;
/**
 *  Type defining signed character
*/
typedef char S08;

/**
 *  Type defining unsigned character
*/
typedef unsigned char U01;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U02;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U03;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U04;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U05;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U06;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U07;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U08;
/**
 *  Type defining unsigned character
*/
typedef unsigned char U8;

/**
 *  Type defining short integer
*/
typedef short S09;
/**
 *  Type defining short integer
*/
typedef short S10;
/**
 *  Type defining short integer
*/
typedef short S11;
/**
 *  Type defining short integer
*/
typedef short S12;
/**
 *  Type defining short integer
*/
typedef short S13;
/**
 *  Type defining short integer
*/
typedef short S14;
/**
 *  Type defining short integer
*/
typedef short S15;
/**
 *  Type defining short integer
*/
typedef short S16;

/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U09;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U10;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U11;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U12;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U13;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U14;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U15;
/**
 *  Type defining unsigned short integer
*/
typedef unsigned short U16;

/**
 *  Type defining signed long
*/
typedef long S17;
/**
 *  Type defining signed long
*/
typedef long S18;
/**
 *  Type defining signed long
*/
typedef long S19;
/**
 *  Type defining signed long
*/
typedef long S20;
/**
 *  Type defining signed long
*/
typedef long S21;
/**
 *  Type defining signed long
*/
typedef long S22;
/**
 *  Type defining signed long
*/
typedef long S23;
/**
 *  Type defining signed long
*/
typedef long S24;
/**
 *  Type defining signed long
*/
typedef long S25;
/**
 *  Type defining signed long
*/
typedef long S26;
/**
 *  Type defining signed long
*/
typedef long S27;
/**
 *  Type defining signed long
*/
typedef long S28;
/**
 *  Type defining signed long
*/
typedef long S29;
/**
 *  Type defining signed long
*/
typedef long S30;
/**
 *  Type defining signed long
*/
typedef long S31;
/**
 *  Type defining signed long
*/
typedef long S32;

/**
 *  Type defining unsigned long
*/
typedef unsigned long U17;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U18;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U19;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U20;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U21;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U22;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U23;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U24;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U25;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U26;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U27;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U28;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U29;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U30;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U31;
/**
 *  Type defining unsigned long
*/
typedef unsigned long U32;
/**
 *  Type defining unsigned long
*/
typedef unsigned int  UInt;
/**
 *  Type defining float
*/
typedef float Float;

/**
 *  Type defining unsigned long long
*/
typedef unsigned long long Uint64;
/**
 *  Type defining signed long long
*/
typedef long long Int64;


/**
 *  Type defining double
*/
typedef double Double;




/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/



#endif   /* _TESTAPPDATATYPE_ */
