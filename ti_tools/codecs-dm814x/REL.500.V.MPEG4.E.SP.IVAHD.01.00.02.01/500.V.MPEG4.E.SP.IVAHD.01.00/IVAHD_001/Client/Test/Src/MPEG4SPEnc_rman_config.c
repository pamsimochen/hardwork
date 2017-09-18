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
 * @file <MPEG4SPEnc_rman_config.c>
 *
 * @brief IRES RMAN related functions.
 *        Creates an RMAN_PARAMS structure to be used by non-RTSC build.
 *
 * @author: Venugopala Krishna (venugopala@ti.com)
 *
 * @version 0.0 (Nov 2008) : Base version for IVAHD developement
 *                           [Venugopala]
 *
 *******************************************************************************
*/


/* -------------------- compilation control switches -------------------------*/
#define MAILBOX_CFG_START   (0x5A05A800)

#if defined(HOSTARM968_FPGA)
  #define VDMAVIEW_EXTMEM      (0x07FFFFFF)
#elif defined(HOSTCORTEXM3_OMAP4) || defined(NETRA_SIM)
  #define VDMAVIEW_EXTMEM      (0xFFFFFFFF)
#else /* Simulator M3 */
  #define VDMAVIEW_EXTMEM      (0x07FFFFFF)
#endif
/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/
#include <stdlib.h>
#include <stdio.h>

/*--------------------- program files ----------------------------------------*/
#include <TestAppComDataType.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/ires.h>
#include <ti/sdo/fc/utils/api/alg.h>
#include <ti/sdo/fc/utils/api/_alg.h>
#include <ti/sdo/fc/ires/hdvicp/ires_hdvicp2.h>
#include <ti/sdo/fc/ires/tiledmemory/ires_tiledmemory.h>
#include <ti/sdo/fc/memutils/memutils.h>
#include <mpeg4enc.h>
#include "MPEG4SPEnc_rman_config.h"
#include "tilerBuf.h"

#define HDVICP_STATUS 0


/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/
#if defined(HOST_M3) || defined(HOST_GAIA) || defined(OMAP4_SDC)
extern ISR_VECTOR_TABLE vector_table[];
#endif


/*----------------------------------------------------------------------------*/
/*  Global flag for non-blocking call.                                        */
/*----------------------------------------------------------------------------*/
extern volatile XDAS_Int8 gMPEG4EIvahdDoneFlag;
/*----------------------------------------------------------------------------*/
/*Instance of the config structure                                             */
/*----------------------------------------------------------------------------*/
extern MPEG4EncoderConfig  gConfig;

extern sTilerParams TilerParams;

enum {HDVICP_FREE,   HDVICP_BUSY, NO_FREE_HDVICP} HDVICPstate;
extern IRES_HDVICP2_Obj mpeg4IresHdvicp2_handle;
extern MPEG4ENC_Handle mpeg4encHandle ;
U32  hdvicpState;



/*----------------------------------------------------------------------------*/
/*  Global Algorithm handle which will be initialized with the argument to    */
/*  the algorithm's MB processing function.                                   */
/*----------------------------------------------------------------------------*/
IALG_Handle g_handle[NUM_IVAHD_RESOURCES];

/*----------------------------------------------------------------------------*/
/*  Global Arguments which will be initialized with the argument to           */
/*  the algorithm's MB processing function.                                   */
/*----------------------------------------------------------------------------*/
Uint32 g_cbArgs[NUM_IVAHD_RESOURCES];

IRES_TILEDMEMORY_ProtocolArgs protocolArgsBackUp[IRES_TOTAL_RESOURCES];
XDAS_UInt32                   numResourcesBackUp;


/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*  Global function pointer which will be initialized to the algorithm's MB   */
/*  processing function and will be called by the gem interrupt handler       */
/*  function.                                                                 */
/*----------------------------------------------------------------------------*/
void (* g_fnPtr[NUM_IVAHD_RESOURCES])(IALG_Handle handle, void *cbArgs);

/*----------------------------------------------------------------------------*/
/* Flag to check whetehr Aquire call made or not                              */
/*----------------------------------------------------------------------------*/
Uint32 isAcquireCallMade;

/*----------------------------------------------------------------------------*/
/*  Global variables which will hold IVAHD Config, SL2 PRCM base address      */
/*----------------------------------------------------------------------------*/
Uint32 MEM_BASE;
Uint32 REG_BASE;
Uint32 RST_CNTL_BASE;


/*---------------------------------------------------------------------------*/
/* Counter for keeping track of frame number                                 */
/*---------------------------------------------------------------------------*/
Uint32 FrameNum  = 0 ;

/** 
********************************************************************************
 *  @fn     Init_IVAHDAddrSpace
 *  @brief  Function to get the IVAHD Config, SL2, PRCM base address based
 *          on IVAHD ID
 *          
 *  @param[in]  None
 *
 *  @return     None
********************************************************************************
*/
void Init_IVAHDAddrSpace()
{
  /* set ivahdid to 0 if it is other than 0, 1, 2       */
  if(gConfig.ivahdid > 2)
  {
    gConfig.ivahdid = 0 ;
  }
#if defined NETRA_SIM
  /*  If IVAHD1 subsystem     */
  if(gConfig.ivahdid == 1)
  {
    /* IVAHD1 CONFIG Base address as understood by Cortex-M3 */
    MEM_BASE = MEM_BASE_IVAHD1;
    /* IVAHD1 SL2 Base address as understood by Cortex-M3 */
    REG_BASE = REG_BASE_IVAHD1;
    /* IVAHD1 PRCM Base address as understood by Cortex-M3 */
    RST_CNTL_BASE = RST_CNTL_BASE_IVAHD1;
  }
  /*  If IVAHD2 subsystem     */
  if(gConfig.ivahdid == 2)
  {
    /* IVAHD2 CONFIG Base address as understood by Cortex-M3 */
    MEM_BASE = MEM_BASE_IVAHD2;
    /* IVAHD2 SL2 Base address as understood by Cortex-M3 */
    REG_BASE = REG_BASE_IVAHD2;
    /* IVAHD2 PRCM Base address as understood by Cortex-M3 */
    RST_CNTL_BASE = RST_CNTL_BASE_IVAHD2;
  }
#else
  gConfig.ivahdid = 0 ;
#endif

  /*  If IVAHD0 subsystem     */
  if(gConfig.ivahdid == 0)
  {
    /* IVAHD0 CONFIG Base address as understood by Cortex-M3 */
    MEM_BASE = MEM_BASE_IVAHD0;
    /* IVAHD0 SL2 Base address as understood by Cortex-M3 */
    REG_BASE = REG_BASE_IVAHD0;
    /* IVAHD0 PRCM Base address as understood by Cortex-M3 */
    RST_CNTL_BASE = RST_CNTL_BASE_IVAHD0;
  }
}

/**
********************************************************************************
 *  @func     ISR_Function
 *  @brief  Call-Back function tied to IVA-HD
 *
 *  @return None
********************************************************************************
*/
void ISR_Function()
{
  /*------------------------------------------------*/
  /* Call-Back function tied to IVA-HD - 0.         */
  /* As with Ducati a single IVA-HD resource is tied*/
  /* this id shall be zero.                         */
  /*------------------------------------------------*/
  (*g_fnPtr[0])(g_handle[0], (void *)g_cbArgs[0]);
}

/** 
********************************************************************************
 *  @fn     RMAN_activateHDVICP2Resource
 *  @brief  Funtion to activate HDVICP2 resource
 *          This fucntion intern calls algorithm call back function to activate
 *          HDVICP2 resource
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *  
 *  @param[in] resourceHandle : Pointer to the HDVICP2 handle
 * 
 *  @return     Status of the activation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_activateHDVICP2Resource (IALG_Handle handle,
  IRES_Handle resourceHandle)
 {
    return (MPEG4ENC_TI_IRES.activateResource(handle, resourceHandle));
 }

/** 
********************************************************************************
 *  @fn     RMAN_deactivateHDVICP2Resource
 *  @brief  Funtion to deactivate HDVICP2 resource
 *          This fucntion intern calls algorithm call back function to 
 *          deactivate HDVICP2 resource
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *  
 *  @param[in] resourceHandle : Pointer to the HDVICP2 handle
 *  
 *  @return     Status of the deactivation
 *
********************************************************************************
*/
XDAS_UInt32 RMAN_deactivateHDVICP2Resource (IALG_Handle handle, 
  IRES_Handle resourceHandle)
{
  return (MPEG4ENC_TI_IRES.deactivateResource(handle, resourceHandle));
}
/**
********************************************************************************
 *  @func     HDVICP_Acquire
 *  @brief  Call to acquire the HDVICP resource alg may (or maynot) have
 *          specifically requested. Populates the hdvicp2Status to indicate the
 *          status of the particular HDVICP2 unit that is returned.
 *          The modeId  argument is both an input and output paramter which
 *          specifies the  mode in which this particular HDVICP2 was used
 *          last time, and the mode in which it will be used this time.
 *          This API takes a yieldContext as argument, as it may yield the
 *          processor to higher priority codecs in the system.
 *
 *  @param[in]  handle     : Pointer to the IALG_Handle structure
 *
 *  @param[in]  iresHandle : Pointer to the IRES_HDVICP2_Handle structure
 *
 *  @param[in]  yieldCtxt  : Pointer to the IRES_YieldContext structure
 *
 *  @param[in]  status     : Pointer to the IRES_HDVICP2_Status structure
 *
 *  @param[in/out] modeId : The mode in which this particular HDVICP2 was
 *                          used last time, and the mode in which it will be 
 *                          used this time 
 *  @param[in] lateAcquireArg : This is the channel id which codec returns
 *                              while acquiring the HDVICP2
 *  @return     None
********************************************************************************
*/
XDAS_Void HDVICP_Acquire(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle,
          IRES_YieldContext *yieldCtxt, XDAS_UInt32 *status,
          Uint32* modeId, Int lateAcquireArg )

{
  Int32  ID = 0;
  Uint32  regBaseAddr, memBaseAddr, resetControlAddress;
  /*--------------------------------------------------------------------------*/
  /* Check if algorithm handle is valid.                                      */
  /*--------------------------------------------------------------------------*/
  if((IALG_Handle)mpeg4encHandle != handle)
  {
    fprintf(stdout,"Algorithm handle not valid..\n");
  }

  /*--------------------------------------------------------------------------*/
  /* Check if HDVICP handle is valid.                                         */
  /*--------------------------------------------------------------------------*/
  if(&mpeg4IresHdvicp2_handle != iresHandle)
  {
    fprintf(stdout,"HDVICP handle not valid..\n");  
  }
   
  
  /*-------------------------------------------------------------------*/
  /* After the resource allocation the new resource ID is communicated */
  /*-------------------------------------------------------------------*/
  iresHandle->id = ID;

  /*-------------------------------------------------------------------*/
  /* Address space notification                                        */
  /* Notification of SL2 memory as seen by the Host ARM.               */
  /* IVAHD_CONFIG Base address as understood by Cortex-M3              */
  /*-------------------------------------------------------------------*/
  regBaseAddr = (Uint32)REG_BASE; /* Length: 0x00080000 bytes */

  /*-------------------------------------------------------------------*/
  /* IVAHD_SL2 Base address as understood by Cortex-M3                 */
  /*-------------------------------------------------------------------*/
  memBaseAddr = (Uint32)MEM_BASE; /* Length: 0x00080000 bytes */

  /*-------------------------------------------------------------------*/
  /* IVAHD_PRCM Base address as understood by Cortex-M3                */
  /*-------------------------------------------------------------------*/
  resetControlAddress = (Uint32)RST_CNTL_BASE; /* Length: 0x0 bytes */

  iresHandle->registerBaseAddress = (void *)regBaseAddr;
  iresHandle->memoryBaseAddress = (void *)memBaseAddr;
  iresHandle->resetControlAddress = (void *)resetControlAddress;

  if(FrameNum)
  {
    *modeId = (Uint32)DIFFERENTCODEC;
    if(*modeId != SAMECODEC)
    {
      RMAN_deactivateHDVICP2Resource(handle, (IRES_Handle)iresHandle) ;    
    }
  }
  else
  {
    *modeId = (Uint32)DIFFERENTCODEC;
    FrameNum = 1;
  }
  
#ifdef SCRATCH_CONTAMINATION_TEST
    /*------------------------------------------------------------------------*/
    /* Do the scratch area contamination                                      */
    /*------------------------------------------------------------------------*/
    MPEG4ENC_TI_Scratch_Contaminate(*modeId);
#endif    

  /*if(*modeId != SAMECODEC)
  {
    RMAN_activateHDVICP2Resource(handle, (IRES_Handle)iresHandle) ;
  } */
  
  *status = (IRES_HDVICP2_Status)(*modeId);

  if(isAcquireCallMade != 0)
  {
    fprintf(stdout,"HDVICP is being acquired without releasing..\n");
  }
  isAcquireCallMade = 1;
  
}



/**
********************************************************************************
 *  @func     arm_isr
 *  @brief
 *
 *  @return None
********************************************************************************
*/
arm_isr()
{
  (*g_fnPtr[0])(g_handle[0], (void *)g_cbArgs[0]);
}

/**
********************************************************************************
 *  @func     dummy_isr
 *  @brief
 *
 *  @return None
********************************************************************************
*/
void dummy_isr()
{
  asm(" NOP ");
}

/** 
********************************************************************************
 *  @fn     HDVICP_ReAcquire
 *  @brief  This function re-acquires the resources
 *          
 *  @param[in] handle                        : Alg handle 
 *  @param[out] iresHandle                   : Ires handle
 *           
 *  @return 1 - if acquire call is done before calling re-acquire 
 *           0 - if acquire call is not done
********************************************************************************
*/
XDAS_UInt32 HDVICP_ReAcquire( IALG_Handle handle,
  IRES_HDVICP2_Handle iresHandle)
{
   Uint32  regBaseAddr, memBaseAddr, resetControlAddress;

  /*--------------------------------------------------------------------------*/
  /* Check if algorithm handle is valid.                                      */
  /*--------------------------------------------------------------------------*/
  if((IALG_Handle)mpeg4encHandle != handle)
  {
    fprintf(stdout,"Algorithm handle not valid..\n");
  }

  /*--------------------------------------------------------------------------*/
  /* Check if HDVICP handle is valid.                                         */
  /*--------------------------------------------------------------------------*/
  if(&mpeg4IresHdvicp2_handle != iresHandle)
  {
    fprintf(stdout,"HDVICP handle not valid..\n");  
  }

  /*-------------------------------------------------------------------*/
  /* Address space notification                                        */
  /* Notification of SL2 memory as seen by the Host ARM.               */
  /* IVAHD_CONFIG Base address as understood by Cortex-M3              */
  /*-------------------------------------------------------------------*/
  regBaseAddr = (Uint32)REG_BASE; /* Length: 0x00080000 bytes */

  /*-------------------------------------------------------------------*/
  /* IVAHD_SL2 Base address as understood by Cortex-M3                 */
  /*-------------------------------------------------------------------*/
  memBaseAddr = (Uint32)MEM_BASE; /* Length: 0x00080000 bytes */

  /*-------------------------------------------------------------------*/
  /* IVAHD_PRCM Base address as understood by Cortex-M3                */
  /*-------------------------------------------------------------------*/
  resetControlAddress = (Uint32)RST_CNTL_BASE; /* Length: 0x0 bytes */

  iresHandle->registerBaseAddress = (void *)regBaseAddr;
  iresHandle->memoryBaseAddress = (void *)memBaseAddr;
  iresHandle->resetControlAddress = (void *)resetControlAddress;

  if(isAcquireCallMade)
    return TRUE;  
  else 
  {
  	RMAN_deactivateHDVICP2Resource(handle, (IRES_Handle)iresHandle) ;
    return FALSE;  
  }  
}


/**
********************************************************************************
 *  @func     HDVICP_Configure
 *  @brief
 *
 *  @param[in]  handle : Pointer to the IALG_Handle structure
 *
 *  @param[in]  iresHandle : Pointer to the IRES_HDVICP2_Handle structure
 *
 *  @param[in]  IRES_HDVICP2_CallbackFxn: This is a function pointer with
 *              arguments handle and cbArgs.
 *
 *  @param[in]  cbArgs :
 *
 *  @return None
********************************************************************************
*/
XDAS_Void HDVICP_Configure(IALG_Handle handle,
                      IRES_HDVICP2_Handle iresHandle,
                      void (*IRES_HDVICP2_CallbackFxn)
                      (IALG_Handle handle, void *cbArgs),
                      void *cbArgs)
{
  Int32 ivahdID = iresHandle->id;

  /*--------------------------------------------------------------------------*/
  /* Check if algorithm handle is valid.                                      */
  /*--------------------------------------------------------------------------*/
  if((IALG_Handle)mpeg4encHandle != handle)
  {
    fprintf(stdout,"Algorithm handle not valid..\n");
  }

  /*--------------------------------------------------------------------------*/
  /* Check if HDVICP handle is valid.                                         */
  /*--------------------------------------------------------------------------*/
  if(&mpeg4IresHdvicp2_handle != iresHandle)
  {
    fprintf(stdout,"HDVICP handle not valid..\n");  
  }
  

  /*-----------------------------------------------------------------------*/
  /*  Initialize the globalIVAHD flag upon which the HDVICP_Wait           */
  /*  shall poll.                                                          */
  /*-----------------------------------------------------------------------*/
  gMPEG4EIvahdDoneFlag = 0;

  /*-----------------------------------------------------------------------*/
  /*  Initialize the Handle to the MB processing function that is          */
  /*  required to be called by the ISR.                                    */
  /*-----------------------------------------------------------------------*/
  g_handle[ivahdID] = handle;

  /*-----------------------------------------------------------------------*/
  /*  Initialize the global function ptr to the MB processing function     */
  /*  which will be called from the ISR.                                   */
  /*-----------------------------------------------------------------------*/
  g_fnPtr[ivahdID]  = IRES_HDVICP2_CallbackFxn;


  /*-----------------------------------------------------------------------*/
  /*  Initialize the Argument to the MB processing function that is        */
  /*  required to be called by the ISR.                                    */
  /*-----------------------------------------------------------------------*/
  g_cbArgs[ivahdID] = (Uint32)cbArgs;

  /*-----------------------------------------------------------------------*/
  /* Register the interrupt function to the vector table in case of M3 host*/
  /*-----------------------------------------------------------------------*/
#ifdef HOST_M3
  if(gConfig.ivahdid == 0)
  {
    *((volatile int *)0xE000E100) = 0x00400000; // Interrupt enable on Ducati
  vector_table[38] = ISR_Function;
  }
  else if(gConfig.ivahdid == 1)
  {
    *((volatile int *)0xE000E100) = 0x00080000; // Interrupt enable on Ducati
    vector_table[35] = ISR_Function;  
  }
  else if(gConfig.ivahdid == 2)
  {
    *((volatile int *)0xE000E100) = 0x00010000; // Interrupt enable on Ducati
    vector_table[32] = ISR_Function;
  } 
#endif

}


/**
********************************************************************************
 *  @func     HDVICP_Release
 *  @brief
 *
 *  @return None
********************************************************************************
*/
void HDVICP_Release(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle)
{
  /*--------------------------------------------------------------------------*/
  /* Check if algorithm handle is valid.                                      */
  /*--------------------------------------------------------------------------*/
  if((IALG_Handle)mpeg4encHandle != handle)
  {
    fprintf(stdout,"Algorithm handle not valid..\n");
  }

  /*--------------------------------------------------------------------------*/
  /* Check if HDVICP handle is valid.                                         */
  /*--------------------------------------------------------------------------*/
  if(&mpeg4IresHdvicp2_handle != iresHandle)
  {
    fprintf(stdout,"HDVICP handle not valid..\n");  
  }
  
    /*--------------------------------------------------------------------*/
  /* Check if acquire call is made earlier                              */
  /*--------------------------------------------------------------------*/
  if(isAcquireCallMade ==0)
  {
    fprintf(stdout,"HDVICP is being released without acquire..\n");
  }
  isAcquireCallMade = 0 ;
  
  /*dummy call*/
  hdvicpState = HDVICP_FREE;

}


/**
********************************************************************************
 *  @func     HDVICP_Wait
 *  @brief  This function will make the host to wait for the frame encode to be
 *          complete
 *
 *  @param[in]  handle     : Pointer to the IALG_Handle structure
 *
 *  @param[in]  iresHandle : Pointer to the IRES_HDVICP2_Handle structure
 *
 *  @param[in]  yieldCtxt  : Pointer to the IRES_YieldContext structure
 *
 *  @return None
********************************************************************************
*/
XDAS_UInt32 HDVICP_Wait(IALG_Handle handle,
                 IRES_HDVICP2_Handle iresHandle,
                 IRES_YieldContext * yieldCtxt)
{
  /*--------------------------------------------------------------------------*/
  /* Check if algorithm handle is valid.                                      */
  /*--------------------------------------------------------------------------*/
  if((IALG_Handle)mpeg4encHandle != handle)
  {
    fprintf(stdout,"Algorithm handle not valid..\n");
  }

  /*--------------------------------------------------------------------------*/
  /* Check if HDVICP handle is valid.                                         */
  /*--------------------------------------------------------------------------*/
  if(&mpeg4IresHdvicp2_handle != iresHandle)
  {
    fprintf(stdout,"HDVICP handle not valid..\n");  
  }
  while(gMPEG4EIvahdDoneFlag == 0);
  gMPEG4EIvahdDoneFlag--;

  return(1); 
}


/**
********************************************************************************
 *  @func     HDVICP_Done
 *  @brief  This function will be called by the interrupt handler when it
 *          detects an end-of-frame processing.
 *
 *  @param[in]  handle     : Pointer to the IALG_Handle structure
 *
 *  @param[in]  iresHandle : Pointer to the IRES_HDVICP2_Handle structure
 *
 *  @return None
********************************************************************************
*/
XDAS_Void HDVICP_Done(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle)
{
  /*--------------------------------------------------------------------------*/
  /* Check if algorithm handle is valid.                                      */
  /*--------------------------------------------------------------------------*/
  if((IALG_Handle)mpeg4encHandle != handle)
  {
    fprintf(stdout,"Algorithm handle not valid..\n");
  }

  /*--------------------------------------------------------------------------*/
  /* Check if HDVICP handle is valid.                                         */
  /*--------------------------------------------------------------------------*/
  if(&mpeg4IresHdvicp2_handle != iresHandle)
  {
    fprintf(stdout,"HDVICP handle not valid..\n");  
  }
  /*-------------------------------------------------------*/
  /* This function will be called by the interrupt handler */
  /* function when it detects an end-of-frame processing.  */
  /*-------------------------------------------------------*/
  gMPEG4EIvahdDoneFlag++;
}

#define WR_MEM_32(addr, data)    *(volatile unsigned int*)(addr) =(unsigned int)(data)
#define RD_MEM_32(addr)    *(volatile unsigned int*)(addr) 

/** 
********************************************************************************
 *  @func     HDVICP_Reset
 *  @brief  Function to reset HDVICP resouce before using it
 *          This function will be called by algorithm when needed
 *
 *  @param[in] handle : Pointer to algorithm handle
 *  
 *  @param[in] iresHandle : Pointer to the HDVICP resource handle
 *
 *  @return     0: Success, non-zero : Failure 
********************************************************************************
*/
XDAS_UInt32 HDVICP_Reset(IALG_Handle handle, IRES_HDVICP2_Handle iresHandle)
{
#if ((defined(NETRA_SIM)) && !(defined(_SIMULATOR_)))
  unsigned int RM_IVAHD0_RSTCTRL =  
    (unsigned int)iresHandle->resetControlAddress; //(PRM_IVAHD0 + 0x10)
  unsigned int RM_IVAHD0_RSTST =  
    (RM_IVAHD0_RSTCTRL + 4); //(PRM_IVAHD0 + 0x14)
  unsigned int CM_IVAHD0_IVAHD_CLKCTRL = 
    ((RM_IVAHD0_RSTCTRL - 0x610 ) + 0x20); //(PRCM_BASE_ADDR + 0x0620)
  unsigned int CM_IVAHD0_SL2_CLKCTRL =  
    (CM_IVAHD0_IVAHD_CLKCTRL + 4); //(PRCM_BASE_ADDR + 0x0624)

  volatile unsigned int temp;
  /* CM_IVAHD0_IVAHD_CLKCTRL. MODULEMODE = 0x0;
  while(CM_IVAHD0_IVAHD_CLKCTRL.IDLEST != 0x3); */
  WR_MEM_32(CM_IVAHD0_IVAHD_CLKCTRL , 0);
  do{
  temp = RD_MEM_32(CM_IVAHD0_IVAHD_CLKCTRL);
  temp = ((temp >> 16) & 3);
  }while(temp != 3);

  /* CM_IVAHD0_SL2_CLKCTRL. MODULEMODE = 0x0;
  while(CM_IVAHD0_SL2_CLKCTRL.IDLEST != 0x3); */
  WR_MEM_32(CM_IVAHD0_SL2_CLKCTRL , 0);
  do{
  temp = RD_MEM_32(CM_IVAHD0_SL2_CLKCTRL);
  temp = ((temp >> 16) & 3);
  }while(temp != 3);
  /* RM_IVAHD0_RSTST     = IVA_RST3 | IVA_RST2 | IVA_RST1; : 0x7 :
  RM_IVAHD0_RSTCTRL = IVA_RST3 | IVA_RST2 | IVA_RST1; : 0x7 */
  WR_MEM_32(RM_IVAHD0_RSTST , 7);
  WR_MEM_32(RM_IVAHD0_RSTCTRL , 7);

  /* CM_IVAHD0_SL2_CLKCTRL. MODULEMODE = 0x2;
  CM_IVAHD0_IVAHD_CLKCTRL. MODULEMODE = 0x2; */

  WR_MEM_32(CM_IVAHD0_SL2_CLKCTRL , 2);
  WR_MEM_32(CM_IVAHD0_IVAHD_CLKCTRL , 2);

  /* RM_IVAHD0_RSTCTRL = !IVA_RST3 | IVA_RST2 | IVA_RST1; : 0x3 */
  WR_MEM_32(RM_IVAHD0_RSTCTRL , 3);

  /* Wait for !IDLE
  while(CM_IVAHD0_IVAHD_CLKCTRL.IDLEST != 0x0);
  while(CM_IVAHD0_SL2_CLKCTRL.IDLEST != 0x0); */ 
  do{
  temp = RD_MEM_32(CM_IVAHD0_IVAHD_CLKCTRL);
  temp = ((temp >> 16) & 3);
  }while(temp != 0);

  do{
  temp = RD_MEM_32(CM_IVAHD0_SL2_CLKCTRL);
  temp = ((temp >> 16) & 3);
  }while(temp != 0);

  /* while(RM_IVAHD0_RSTST.IVA_RST3 != 0x1); */
  do{
  temp = RD_MEM_32(RM_IVAHD0_RSTST);
  temp = ((temp >> 2) & 1);
  }while(temp != 1);

#elif ((defined(HOSTCORTEXM3_OMAP4)) && !(defined(_SIMULATOR_)))

  volatile unsigned int * resetControlRegister = NULL;
  volatile unsigned int * resetControlStatusRegister = NULL;
  volatile unsigned int * ivahdClockControlRegister = NULL;
  volatile unsigned int * ivahdClockControlStatusRegister = NULL;
  volatile unsigned int * sl2ClockControlRegister = NULL;
  volatile int debugCounter1=0,debugCounter2=0;
  
  /*Reset IVA HD,SL2 and ICONTs */
  resetControlRegister             = (volatile unsigned int *)(iresHandle->resetControlAddress);
  resetControlStatusRegister       = (volatile unsigned int *)(((unsigned int)resetControlRegister) + 4);
  ivahdClockControlStatusRegister  = (volatile unsigned int *)(((unsigned int)resetControlRegister) - 0x2FE010);
  ivahdClockControlRegister        = (volatile unsigned int *)(((unsigned int)ivahdClockControlStatusRegister) + 0x20);
  sl2ClockControlRegister          = (volatile unsigned int *)(((unsigned int)ivahdClockControlStatusRegister) + 0x28);
  
  /* First put IVA into HW Auto mode */
  *ivahdClockControlStatusRegister |= 0x3;
  
  /* Wait for IVA HD to  standby */
  while (!((*ivahdClockControlRegister) & 0x40000));
  
  /*Disable IVAHD and SL2 modules*/
  *ivahdClockControlRegister = 0;
  *sl2ClockControlRegister = 0;
  
  
  
  /*Ensure that IVAHD and SL2 are disabled*/
  do {
      if((*ivahdClockControlRegister) & 0x00030000) {
          break;
      }
  } while (1);
  
  do {
      if((*sl2ClockControlRegister) & 0x00030000) {
          break;
      }
  } while (1);
  
   
  /*Reset IVAHD sequencers and SL2*/
  *resetControlRegister |= 7;
  
  /*Check if modules are reset*/
  
  /*First clear the status bits*/
  *resetControlStatusRegister |= 7;
  
  /*Wait for confirmation that the systems have been reset*/
  /*THIS CHECK MAY NOT BE NECESSARY, AND MOST OF ALL GIVEN OUR STATE, MAY NOT BE POSSIBLE*/    
  
  /* Ensure that the wake up mode is set to SW_WAKEUP */
  *ivahdClockControlStatusRegister &= 0x2;
  
  /*Enable IVAHD and SL2 modules*/
  *ivahdClockControlRegister = 1;
  *sl2ClockControlRegister = 1;
  
  /*Deassert the SL2 reset*/
  *resetControlRegister &= 0xFFFFFFFB;
   
  /*Ensure that IVAHD and SL2 are enabled*/
  do {
      if(!((*ivahdClockControlRegister) & 0x00030000)) {
          break;
      }
      /*Just to check the number of iterations taken for the modules to get enabled*/
      debugCounter1++;
  } while (1);
  
  do {
      if(!((*sl2ClockControlRegister) & 0x00030000)) {
          break;
      }
      /*Just to check the number of iterations taken for the modules to get enabled*/
      debugCounter2++;
  } while (1);
#endif	
	
  return (XDAS_TRUE);
}


IRES_HDVICP2_Obj mpeg4IresHdvicp2_handle = {
  0,  /* IRES_Obj.persistent */
  0,  /* getStaticProperties */
  0,  /* info (reserved) */
  0,  /* ID of the HDVICP2 being granted, could be 0,1,2, or LATE_ACQUIRE */
  (Void*)REG_BASE_IVAHD0,       /* Reg. space base addr  */
  (Void*)MEM_BASE_IVAHD0,       /* Mem. Space base addr  */
  (Void*)RST_CNTL_BASE_IVAHD0,  /* PRCM register address */  
  HDVICP_Acquire,
  HDVICP_Release,
  HDVICP_ReAcquire,  
  HDVICP_Configure,
  HDVICP_Wait,
  HDVICP_Done,              /* Pointer to the HDVICP_Done API function */
  HDVICP_Reset              /* Pointer to the HDVICP_Reset API function */
  
};


IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_MBPARAMS =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_FRAMEINFO =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_PERSISTANT_MEM =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_DEBUGTRACESTRUCT_MEM =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_REFBUF_LUMA_0 =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_REFBUF_CHROMA_0 =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_16BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_REFBUF_LUMA_1 =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_REFBUF_CHROMA_1 =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_16BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_MBINFO =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_MVINFO =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_RESIDUALINFO =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_chroma3 =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_coloMVInfo =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};

IRES_TILEDMEMORY_Obj IRES_TILEDMEMORY_persistentMemory =
{
  {
    /*------------------------------------------------------------------------*/
    /*  Indicates if the resource has been allocated as persistent.           */
    /*------------------------------------------------------------------------*/
    1,
    /*------------------------------------------------------------------------*/
    /*  Obtain the static properties associated with this resource            */
    /*  This could include information like the register layer of the         */
    /*  device etc.                                                           */
    /*------------------------------------------------------------------------*/
    NULL
  },
  /*--------------------------------------------------------------------------*/
  /*   Void * info RSVD for future use                                        */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /*  Void * memoryBaseAddress,            Allocated Memory base addr         */
  /*--------------------------------------------------------------------------*/
  NULL,

  /*--------------------------------------------------------------------------*/
  /*  Int  isTiledMemory                                                      */
  /*--------------------------------------------------------------------------*/  
  0,
  /*--------------------------------------------------------------------------*/
  /* IRES_TILEDMEMORY_AccessUnit accessUnit                                   */
  /*--------------------------------------------------------------------------*/  
  IRES_TILEDMEMORY_8BIT,
  
  /*--------------------------------------------------------------------------*/
  /*  Void * tilerBaseAddress              Static TILER MEMORY base addr      */
  /*--------------------------------------------------------------------------*/
  TILERBASEADDRESS, 

  /*--------------------------------------------------------------------------*/
  /*  Void * systemSpaceBaseAddress        Static physical addr               */
  /* Kept NULL as codec is not using it, ideally it should be physical address*/
  /* of the tiled buffer to avoid MEMUTILS_getPhysicalAddr API                */
  /*--------------------------------------------------------------------------*/
  NULL 
};


/**
********************************************************************************
 *  @func     allignMalloc
 *  @brief  Allocate a chunk of memory with specified alignment
 *
 *  @param[in]  alignment : required alignement in bytes
 *
 *  @param[in]  size      : size of the memory chunk to be allocated
 *
 *  @return None
********************************************************************************
*/
void *allignMalloc(size_t alignment, size_t size)
{
  void **mallocPtr;
  void **retPtr;
  /*------------------------------------------------------------------*/
  /* return if invalid size value                                     */
  /*------------------------------------------------------------------*/
  if (size <= 0) {
    return (0);
  }

  /*------------------------------------------------------------------*/
  /* If alignment is not a power of two, return what malloc returns.  */
  /* This is how memalign behaves on the c6x.                         */
  /*------------------------------------------------------------------*/
  if ((alignment & (alignment - 1)) || (alignment <= 1)) {
    if ((mallocPtr = malloc(size + sizeof(mallocPtr))) != NULL) {
      *mallocPtr = mallocPtr;
      mallocPtr++;
    }
    return ((void *)mallocPtr);
  }

  /*------------------------------------------------------------------*/
  /* allocate block of memory                                         */
  /*------------------------------------------------------------------*/
  if (!(mallocPtr = malloc(alignment + size))) {
    return (0);
  }

  /*------------------------------------------------------------------*/
  /* Calculate aligned memory address                                 */
  /*------------------------------------------------------------------*/
  retPtr = (void *)(((Uns) mallocPtr + alignment) & ~(alignment - 1));

  /*------------------------------------------------------------------*/
  /* Set pointer to be used in the mem_free() fxn                     */
  /*------------------------------------------------------------------*/
  retPtr[-1] = mallocPtr;

  /*------------------------------------------------------------------*/
  /* return aligned memory                                            */
  /*------------------------------------------------------------------*/
  return ((void *)retPtr);
}

/**
********************************************************************************
 *  @func     allignFree
 *  @brief  frees a chunk of memory
 *
 *  @param[in]  alignment : Memory pointer to be freed
 *
 *  @return None
********************************************************************************
*/
void allignFree(void *ptr)
{
  /*--------------------------------------------------------------------------*/
  /* Go to the previous pointer to check exact address of the buffer allocated*/
  /*--------------------------------------------------------------------------*/
  free((void *)((void **)ptr)[-1]);
}

/** 
********************************************************************************
 *  @fn     RMAN_AssignResources
 *  @brief  Funtion to assigne the HDVICP and Memory resources, This is dummy 
 *          implimentation to emulate exact RMAN module API
 *
 *  @param[in] handle : Pointer to the algorithm handle
 *  
 *  @return     IRES_Status : status of the resource assigment
 *
********************************************************************************
*/
IRES_Status RMAN_AssignResources(IALG_Handle handle, XDAS_Int8 lumaTilerSpace, 
                                 XDAS_Int8 chromaTilerSpace)
{
  /*--------------------------------------------------------------------------*/
  /* Pointers to the resource discriptor, protocal arguments of the HDVICP and*/
  /* tiledmemory resoures                                                     */
  /*--------------------------------------------------------------------------*/
  IRES_ResourceDescriptor *resourceDescriptor;
  IRES_TILEDMEMORY_ProtocolArgs *tiledmemProtocolArgs;
  IRES_YieldFxn yieldFxn = NULL;  /* acquired from RMAN during init */
  IRES_YieldArgs yieldArgs = NULL;/* acquired from RMAN during init */
  Int32  numResources;
  IRES_Status retVal;
  XDAS_Int32 idx;

  isAcquireCallMade = 0;
  
  /*--------------------------------------------------------------------------*/
  /* Call codec api function to get number of resources required for algorithm*/
  /*--------------------------------------------------------------------------*/
  numResources = MPEG4ENC_TI_IRES.numResourceDescriptors(handle);
  numResourcesBackUp = numResources;

  /*--------------------------------------------------------------------------*/
  /* Allogate memory for the resource instances                               */
  /*--------------------------------------------------------------------------*/
  resourceDescriptor = (IRES_ResourceDescriptor *)malloc(numResources *
    sizeof(IRES_ResourceDescriptor));
  printf("Number of Resources required = %d\n", numResources);

  /*--------------------------------------------------------------------------*/
  /* Get properties of the resource with help codec API function              */
  /*--------------------------------------------------------------------------*/
  retVal = MPEG4ENC_TI_IRES.getResourceDescriptors(handle, resourceDescriptor);
  if (retVal != IRES_OK)
  {
    printf("Error in Get Reosource Descriptor \n");
    return retVal;
  }

  /*--------------------------------------------------------------------------*/
  /* assign default HDVICP resource handle pointer to the resource discriptor */
  /* hadle                                                                    */
  /*--------------------------------------------------------------------------*/
  resourceDescriptor[IRES_HDVICP_RESOURCE_IVAHD_0].handle =
    (IRES_Obj *)&mpeg4IresHdvicp2_handle;
  resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_MVINFO].handle =
     (IRES_Obj *)&IRES_TILEDMEMORY_MVINFO;

  /*--------------------------------------------------------------------------*/
  /* Assigne default memory resource handles all memory resource discriptors  */
  /*--------------------------------------------------------------------------*/
  resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_PERSISTENT_MEM].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_PERSISTANT_MEM;
  resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_LUMA_0].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_REFBUF_LUMA_0;
  resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_CHROMA_0].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_REFBUF_CHROMA_0;
  resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_LUMA_1].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_REFBUF_LUMA_1;
  resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_CHROMA_1].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_REFBUF_CHROMA_1;
  resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_MBINFO].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_MBINFO;
  resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_RESIDUALINFO].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_RESIDUALINFO;

  resourceDescriptor[DEBUGTRACESTRUCT].handle =
    (IRES_Obj *)&IRES_TILEDMEMORY_DEBUGTRACESTRUCT_MEM;

  /*---------------------------------*/
  /* Allocate memory in tiler space. */
  /*---------------------------------*/
  tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
    resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_MBINFO].protocolArgs;
  IRES_TILEDMEMORY_MBINFO.memoryBaseAddress =
    (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
    (tiledmemProtocolArgs->sizeDim0 * sizeof(XDAS_UInt8)));

  tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
    resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_RESIDUALINFO].protocolArgs;
  IRES_TILEDMEMORY_RESIDUALINFO.memoryBaseAddress =
    (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
    (tiledmemProtocolArgs->sizeDim0 * sizeof(XDAS_UInt8)));


  tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
    resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_MVINFO].protocolArgs;
  IRES_TILEDMEMORY_MVINFO.memoryBaseAddress =
    (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
    (tiledmemProtocolArgs->sizeDim0 * sizeof(XDAS_UInt8)));

  tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
    resourceDescriptor[IRES_TILED_MEMORY_1D_RESOURCE_PERSISTENT_MEM].\
    protocolArgs;
  IRES_TILEDMEMORY_PERSISTANT_MEM.memoryBaseAddress =
    (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
    (tiledmemProtocolArgs->sizeDim0 * sizeof(XDAS_UInt8)));


  tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
    resourceDescriptor[DEBUGTRACESTRUCT].\
    protocolArgs;
  IRES_TILEDMEMORY_DEBUGTRACESTRUCT_MEM.memoryBaseAddress =
    (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
    (tiledmemProtocolArgs->sizeDim0 * sizeof(XDAS_UInt8)));



  if (lumaTilerSpace)
  {
      IRES_TILEDMEMORY_REFBUF_LUMA_0.memoryBaseAddress =  (Void *)TilerParams.tiledBufferAddr[8]  ;
      IRES_TILEDMEMORY_REFBUF_LUMA_1.memoryBaseAddress =  (Void *)TilerParams.tiledBufferAddr[10] ;
      IRES_TILEDMEMORY_REFBUF_LUMA_0.isTiledMemory = 1;
      IRES_TILEDMEMORY_REFBUF_LUMA_1.isTiledMemory = 1;
  }
  else
  {  
    tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
      resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_LUMA_0].\
      protocolArgs;
    IRES_TILEDMEMORY_REFBUF_LUMA_0.memoryBaseAddress =
      (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
      (tiledmemProtocolArgs->sizeDim0 * tiledmemProtocolArgs->sizeDim1 *
      sizeof(XDAS_UInt8)));


    tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
      resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_LUMA_1].\
      protocolArgs;
    IRES_TILEDMEMORY_REFBUF_LUMA_1.memoryBaseAddress =
      (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
      (tiledmemProtocolArgs->sizeDim0 * tiledmemProtocolArgs->sizeDim1 *
      sizeof(XDAS_UInt8)));

  }
  
  if(chromaTilerSpace)
  {
    IRES_TILEDMEMORY_REFBUF_CHROMA_0.memoryBaseAddress =  (Void *)TilerParams.tiledBufferAddr[9]  ;
    IRES_TILEDMEMORY_REFBUF_CHROMA_1.memoryBaseAddress =  (Void *)TilerParams.tiledBufferAddr[11] ;
    IRES_TILEDMEMORY_REFBUF_CHROMA_0.isTiledMemory = 1;
    IRES_TILEDMEMORY_REFBUF_CHROMA_1.isTiledMemory = 1;

    if(chromaTilerSpace == XDM_MEMTYPE_TILED8)
    {
      IRES_TILEDMEMORY_REFBUF_CHROMA_0.accessUnit =  IRES_TILEDMEMORY_8BIT ;
      IRES_TILEDMEMORY_REFBUF_CHROMA_1.accessUnit =  IRES_TILEDMEMORY_8BIT ;
    }
    else if(chromaTilerSpace == XDM_MEMTYPE_TILED16)
    {
      IRES_TILEDMEMORY_REFBUF_CHROMA_0.accessUnit =  IRES_TILEDMEMORY_16BIT ;
      IRES_TILEDMEMORY_REFBUF_CHROMA_1.accessUnit =  IRES_TILEDMEMORY_16BIT ;
    }
  }
  else
  {

    tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
      resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_CHROMA_0].\
      protocolArgs;
    IRES_TILEDMEMORY_REFBUF_CHROMA_0.memoryBaseAddress =
      (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
      (tiledmemProtocolArgs->sizeDim0 * tiledmemProtocolArgs->sizeDim1 *
      sizeof(XDAS_UInt8)));

    tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)
      resourceDescriptor[IRES_TILED_MEMORY_2D_RESOURCE_REFBUF_CHROMA_1].\
      protocolArgs;
    IRES_TILEDMEMORY_REFBUF_CHROMA_1.memoryBaseAddress =
      (Void *)allignMalloc(tiledmemProtocolArgs->alignment,
      (tiledmemProtocolArgs->sizeDim0 * tiledmemProtocolArgs->sizeDim1 *
      sizeof(XDAS_UInt8)));
  }
  /*--------------------------------------------------------------------------*/
  /* Back up the protocol Args requested by the codec.                        */
  /* Later use this to check that same is requested during during             */
  /* RMAN_FreeResources()                                                     */
  /*--------------------------------------------------------------------------*/

  for(idx = 1; idx < numResources; idx++)
  {
    IRES_TILEDMEMORY_ProtocolArgs *tiledmemProtocolArgs;
    tiledmemProtocolArgs = 
      (IRES_TILEDMEMORY_ProtocolArgs*)resourceDescriptor[idx].protocolArgs;
    memcpy(&protocolArgsBackUp[idx-1], tiledmemProtocolArgs, 
      sizeof (IRES_TILEDMEMORY_ProtocolArgs));

  }
  /*--------------------------------------------------------------------------*/
  /* Codec API function init resources                                        */
  /*--------------------------------------------------------------------------*/    
  retVal = MPEG4ENC_TI_IRES.initResources(handle,
    resourceDescriptor, yieldFxn, yieldArgs);
  if (retVal != IRES_OK)
  {
    printf("Error in Init Resources \n");
    return retVal;
  }

  /*--------------------------------------------------------------------------*/
  /* Free the memory allocated for the discriptor instances                   */
  /*--------------------------------------------------------------------------*/
  free(resourceDescriptor);
  return retVal;
}


/**
********************************************************************************
 *  @func    RMAN_activateAllResources
 *  @brief   Funtion to activate all the resources
 *           This fucntion intern calls algorithm call back function to activate
 *           all the resources
 *
 *  @param[in]  handle : Pointer to the IALG_Handle structure
 *
 *  @return    Status of the activation
********************************************************************************
*/
XDAS_UInt32 RMAN_activateAllResources (IALG_Handle handle)
{
  /*--------------------------------------------------------------------------*/
  /* call to the codec activate function                                      */
  /*--------------------------------------------------------------------------*/
  return (MPEG4ENC_TI_IRES.activateAllResources(handle));
}


/**
********************************************************************************
 *  @func     RMAN_deactivateAllResources
 *  @brief  This function deactivates and frees the resources
 *
 *  @param[in]  handle : Pointer to the IALG_Handle structure
 *
 *  @return     Status of the deactivation
********************************************************************************
*/
XDAS_UInt32 RMAN_deactivateAllResources (IALG_Handle handle)
{
  /*--------------------------------------------------------------------------*/
  /* call to the codec deactivate function                                    */
  /*--------------------------------------------------------------------------*/
  return (MPEG4ENC_TI_IRES.deactivateAllResources(handle));
}


/**
********************************************************************************
 *  @func     RMAN_FreeResources
 *  @brief  This function frees the resources
 *
 *  @param[in]  handle : Pointer to the IALG_Handle structure
 *
 *  @return     0 if success else specified error code
********************************************************************************
*/
void RMAN_FreeResources(IALG_Handle handle, XDAS_Int8 lumaTilerSpace, 
                                 XDAS_Int8 chromaTilerSpace)
{
  XDAS_Int32  retVal, numResources, idx;
  IRES_ResourceDescriptor       *resourceDescriptor;  
  IRES_TILEDMEMORY_ProtocolArgs *tiledmemProtocolArgs;

  /*--------------------------------------------------------------------------*/
  /* Call codec api function to get number of resources required for algorithm*/
  /*--------------------------------------------------------------------------*/
  numResources = MPEG4ENC_TI_IRES.numResourceDescriptors(handle);

  if(numResources != numResourcesBackUp)
  {
    fprintf(stdout, "Err: Number of Resources requsted changed during Free\n",
           numResources); 
  }
  
  /*--------------------------------------------------------------------------*/
  /* Allogate memory for the resource instances                               */
  /*--------------------------------------------------------------------------*/
  resourceDescriptor = (IRES_ResourceDescriptor *)malloc(numResources *
    sizeof(IRES_ResourceDescriptor));

  retVal = MPEG4ENC_TI_IRES.getResourceDescriptors(handle, resourceDescriptor);
  if (retVal != IRES_OK)
  {
   printf("Error in Get Resource Descriptor \n");
  }
  /*--------------------------------------------------------------------------*/
  /* Free the allocated memory resources                                      */
  /*--------------------------------------------------------------------------*/

  for(idx = IRES_TILED_MEMORY_1D_RESOURCE_START ; 
    idx <= IRES_TILED_MEMORY_1D_RESOURCE_END;idx++)
  {
    /*------------------------------------------------------------------------*/
    /* Obtain attributes of the memory request                                */
    /*------------------------------------------------------------------------*/
    tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)resourceDescriptor
                            [idx].protocolArgs;
    /*------------------------------------------------------------------------*/
    /* Chk that same protocol Args are reported again. Otherwise flag an      */
    /* error.                                                                 */
    /*------------------------------------------------------------------------*/
    if(memcmp(&protocolArgsBackUp[idx-1], tiledmemProtocolArgs, 
          sizeof (IRES_TILEDMEMORY_ProtocolArgs)))
    {
       fprintf(stdout, "Err: Protocol Args requested changed for idx = %d \n", 
                        (idx));  
    }
  }
  allignFree(IRES_TILEDMEMORY_MVINFO.memoryBaseAddress);
  allignFree(IRES_TILEDMEMORY_PERSISTANT_MEM.memoryBaseAddress);
  allignFree(IRES_TILEDMEMORY_MBINFO.memoryBaseAddress);
  allignFree(IRES_TILEDMEMORY_RESIDUALINFO.memoryBaseAddress);


  allignFree(IRES_TILEDMEMORY_DEBUGTRACESTRUCT_MEM.memoryBaseAddress);


  if (!lumaTilerSpace)
  {

     for(idx = IRES_TILED_MEMORY_2D_RESOURCE_START ; 
       idx <= IRES_TILED_MEMORY_2D_RESOURCE_END;idx++)
     {
      /*----------------------------------------------------------------------*/
      /* Obtain attributes of the memory request                              */
      /*----------------------------------------------------------------------*/
      tiledmemProtocolArgs = (IRES_TILEDMEMORY_ProtocolArgs*)resourceDescriptor
                              [idx].protocolArgs;
      /*----------------------------------------------------------------------*/
      /* Chk that same protocol Args are reported again. Otherwise flag an    */
      /* error.                                                               */
      /*----------------------------------------------------------------------*/
      if(memcmp(&protocolArgsBackUp[idx-1], tiledmemProtocolArgs, 
          sizeof (IRES_TILEDMEMORY_ProtocolArgs)))
      {
         fprintf(stdout, "Err: Protocol Args requested changed for idx = %d \n", 
                        (idx));  
      }
     }
    allignFree(IRES_TILEDMEMORY_REFBUF_LUMA_0.memoryBaseAddress);
    allignFree(IRES_TILEDMEMORY_REFBUF_CHROMA_0.memoryBaseAddress);
    allignFree(IRES_TILEDMEMORY_REFBUF_LUMA_1.memoryBaseAddress);
    allignFree(IRES_TILEDMEMORY_REFBUF_CHROMA_1.memoryBaseAddress);
  }
  /*--------------------------------------------------------------------------*/
  /* Free the memory allocated for the discriptor instances                   */
  /*--------------------------------------------------------------------------*/
  free(resourceDescriptor);

}

/**
********************************************************************************
 *  @func       MEMUTILS_getPhysicalAddr
 *  @brief      This function will give the physical address on HOST_M3
 *
 *  @param[in]  addr : Pointer to the address for which physical address
 *                     is required
 *
 *  @return     Pointer to the physical memory
********************************************************************************
*/
Void * MEMUTILS_getPhysicalAddr(Ptr Addr)
{
  // return ((Void*)((U32)Addr & VDMAVIEW_EXTMEM)) ;
#if defined(A9_HOST_FILE_IO) 
  Uint32 temp1;

  if(((Uint32)Addr<(0x0+0x4000))){
   temp1= ((Uint32)Addr+0x9d000000);                       
  }else if(((Uint32)Addr>=0x4000)&&((Uint32)Addr<(0x4000+0x1fc000))){
   temp1= ((Uint32)Addr - 0x4000 + 0x9d004000);
  }else if(((Uint32)Addr>=0x200000)&&((Uint32)Addr<(0x200000+0x600000))){
   temp1= ((Uint32)Addr - 0x200000 + 0x9d200000);
  }else if(((Uint32)Addr>=0x800000)&&((Uint32)Addr<(0x800000+0x200000))){
   temp1= ((Uint32)Addr - 0x800000 + 0x9d800000);
  }else if(((Uint32)Addr>=0xa00000)&&((Uint32)Addr<(0xa00000+0x600000))){
   temp1= ((Uint32)Addr - 0xa00000 + 0x9da00000);

  }else if(((Uint32)Addr>=0x80000000)&&((Uint32)Addr<(0x80000000+0x100000))){
   temp1= ((Uint32)Addr - 0x80000000 +0x9e000000);
  }else if(((Uint32)Addr>=0x80100000)&&((Uint32)Addr<(0x80100000+0x100000))){
   temp1= ((Uint32)Addr - 0x80100000 + 0x9e100000);

  }else if(((Uint32)Addr>=0x80200000)&&((Uint32)Addr<(0x80200000+0x100000))){
   temp1= ((Uint32)Addr - 0x80200000 +0x9e200000);

  }else if(((Uint32)Addr>=0x80300000)&&((Uint32)Addr<(0x80300000+0x1000000))){
   temp1= ((Uint32)Addr - 0x80300000 +0x9e300000);

  }else if(((Uint32)Addr>=0x81300000)&&((Uint32)Addr<(0x81300000+0xc00000))){
   temp1= ((Uint32)Addr - 0x81300000 +0x9f300000);

  }else if(((Uint32)Addr>=0x81f00000)&&((Uint32)Addr<(0x81f00000+0x100000))){
   temp1= ((Uint32)Addr - 0x81f00000 +0x9ff00000);

  }else if(((Uint32)Addr>=0xa0000000)&&((Uint32)Addr<(0xa0000000+0x55000))){
   temp1= ((Uint32)Addr - 0xa0000000 +0x9cf00000);

  }else if(((Uint32)Addr>=0xa0055000)&&((Uint32)Addr<(0xa0055000+0x55000))){
   temp1= ((Uint32)Addr - 0xa0055000 +0x9cf55000);

  }else{
   temp1= ((Uint32)Addr & 0xFFFFFFFF);
  }
  return (Void *)temp1;
#else
  return ((Void*)((XDAS_UInt32)Addr & VDMAVIEW_EXTMEM));
#endif


}
