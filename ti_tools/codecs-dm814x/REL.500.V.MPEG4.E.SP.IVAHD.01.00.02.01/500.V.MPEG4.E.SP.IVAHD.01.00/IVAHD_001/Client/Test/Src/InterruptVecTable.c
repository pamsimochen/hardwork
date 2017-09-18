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
 * @file <InterruptVecTable.c>
 *
 * @brief This File contains Interrupt vector table for Ducati Host.
 *
 * @author: Venugopala Krishna (venugopala@ti.com)
 *
 * @version 0.0 (Nov 2008) : Base version for IVAHD developement
 *                           [Venugopala]
 *
 *******************************************************************************
*/


/* -------------------- compilation control switches -------------------------*/
#define STACK_SIZE 0x4000

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/
#include <stdio.h>

/*--------------------- program files ----------------------------------------*/

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/
#pragma DATA_SECTION(vector_table, ".intvecs");
unsigned int stack_base[STACK_SIZE];

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/
typedef void (*ISR_VECTOR_TABLE)();
void resetHandler();
void NMIHandler();
void HFHandler();
void SVCallISR();
extern void Done();
void MemManageHandler();
void init_ivt();
extern int _c_int00();
typedef unsigned int U32;

const U32 vector_table[] =
{
   (U32)stack_base,//stack_base + STACK_SIZE*4,
   (U32)_c_int00,//resetHandler,
   (U32)NMIHandler,
   (U32)HFHandler,
   (U32)MemManageHandler, // Populate if using MemManage (MPU)
   (U32)10, // Populate if using Bus fault
   0, // Populate if using Usage Fault
   0,
   0,
   0,
   0, // reserved slots
   (U32)SVCallISR,
   0, /* Populate if using a debug monitor*/
   0, /* Reserved*/
   0, /* Populate if using pendable service request*/
   0, /* Populate if using SysTick*/
   0, /* Timer1ISR,  external interrupts start here*/ 
   0, /* GpioInISR*/
   0, /* GpioOutISR,*/
   0, /* I2CIsr*/
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,
   0,/*IVAHD ISR*/
   0,
   0,
   0,
   0,
   0,
   0,
   0
};

/*extern void c_int00();*/
interrupt void resetHandler()
{
   /*asm ( " B _c_int00");*/
}

interrupt void NMIHandler()
{
}

interrupt void HFHandler()
{
   printf( "hardfault");
   while(1);
}

interrupt void SVCallISR()
{
   int i=0;
   i++;
   printf("swi");
   return;
}

interrupt void MemManageHandler()
{
   printf("address =%x", vector_table);
   return;
}

void init_ivt()
{
   printf("address = %x", vector_table);
}

