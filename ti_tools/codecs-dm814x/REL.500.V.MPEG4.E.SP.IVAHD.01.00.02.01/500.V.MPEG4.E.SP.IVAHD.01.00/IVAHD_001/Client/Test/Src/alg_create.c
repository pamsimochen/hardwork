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
 * @file <alg_create.c>
 *
 * @brief This file contains a simple implementation of the ALG_create API
 *        operation.
 *
 * @author:
 *
 * @version 0.0 (Mon Year) : Change description
 *                           [author name]
 *
 *******************************************************************************
*/


/* -------------------- compilation control switches -------------------------*/

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/
#include <stdlib.h>

/*--------------------- program files ----------------------------------------*/
#include <TestAppComDataType.h>
#include <ti/xdais/ialg.h>

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/
extern Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n);
extern Void _ALG_freeMemory(IALG_MemRec memTab[], Int n);


/**
********************************************************************************
 *  @func     ALG_create
 *  @brief  This function contains the creation of algorithm
 *
 *  @param[in]  fxns   : Pointer to the IALG_Fxns structure
 *
 *  @param[in]  p      : Pointer to the IALG_Handle structure
 *
 *  @param[in]  params : Pointer to the IALG_Params structure
 *
 *  @return     Poniter to the created algorithm or NULL
********************************************************************************
*/
IALG_Handle ALG_create(IALG_Fxns * fxns, IALG_Handle p, IALG_Params * params)
{
  IALG_MemRec *memTab;
  Int n;
  IALG_Handle alg;
  IALG_Fxns *fxnsPtr;

  if (fxns != NULL) {
    n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

    if ((memTab = (IALG_MemRec *) malloc(n * sizeof(IALG_MemRec)))) {

      n = fxns->algAlloc(params, &fxnsPtr, memTab);
      if (n <= 0) {
        return (NULL);
      }

      if (_ALG_allocMemory(memTab, n)) {
        alg = (IALG_Handle) memTab[0].base;
        alg->fxns = fxns;
        if (fxns->algInit(alg, memTab, p, params) == IALG_EOK) {
          free(memTab);
          return (alg);
        }
        fxns->algFree(alg, memTab);
        _ALG_freeMemory(memTab, n);
      }

      free(memTab);
    }
  }

  return (NULL);
}

/**
********************************************************************************
 *  @func     ALG_create
 *  @brief  This function deletes the algorithm
 *
 *  @param[in]  alg    : Pointer to the IALG_Handle structure
 *
 *  @return     Poniter to the created algorithm or NULL
********************************************************************************
*/
Void ALG_delete(IALG_Handle alg)
{
  IALG_MemRec *memTab;
  Int n;
  IALG_Fxns *fxns;

  if (alg != NULL && alg->fxns != NULL) {
    fxns = alg->fxns;
    n = fxns->algNumAlloc != NULL ? fxns->algNumAlloc() : IALG_DEFMEMRECS;

    if ((memTab = (IALG_MemRec *) malloc(n * sizeof(IALG_MemRec)))) {
      memTab[0].base = alg;
      n = fxns->algFree(alg, memTab);
      _ALG_freeMemory((IALG_MemRec *) memTab, n);

      free(memTab);
    }
  }
}
