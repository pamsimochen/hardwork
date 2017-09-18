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
 * @file <alg_malloc.c>
 *
 * @brief This module implements an algorithm memory management "policy" in
 *        which no memory is shared among algorithm objects.  Memory is, however
 *        reclaimed when objects are deleted.
 *
 *        preemption      sharing             object deletion
 *        ----------      -------             ---------------
 *        yes(*)          none                yes
 *
 *        Note 1: This module uses run-time support functions malloc() and
 *        free() to allocate and free memory.  Since these functions are *not*
 *        reentrant, either all object creation and deletion must be performed
 *        by a single thread or reentrant versions or these functions must be
 *        created.
 *
 *        Note 2: Memory alignment is supported for the c5000 targets with the
 *        memalign5000() function which allocates 'size + align' memory.  This
 *        is necessary since the compiler's run-time library does not support
 *        memalign().  This wastes 'align' extra memory.
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
#include <stdlib.h> /* malloc/free declarations */
#include <string.h> /* memset declaration */

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

#define INT_MEM_SIZE 2048
#pragma DATA_SECTION(hostIntMemory, ".InternalMemory");
U01 hostIntMemory[INT_MEM_SIZE];
typedef struct _sInternalMemory
{
  U01 *memoryBasePtr;
  U32 sizeLeft;
}sInternalMemory;

sInternalMemory gInternalMemory = {hostIntMemory, INT_MEM_SIZE};

/*---------------------- function prototypes ---------------------------------*/
#if defined (_54_) || (_55_) || (_28_)
void *mem_align(size_t alignment, size_t size);
void mem_free(void *ptr);
#define myMemalign mem_align
#define myFree mem_free
#else
#define myMemalign memalign
#define myFree free
#endif


/**
********************************************************************************
 *  @func     ALG_activate
 *  @brief  This dunction does do app specific initialization of scratch memory
 *
 *  @param[in]  alg : Pointer to the IALG_Handle structure
 *
 *  @return None
********************************************************************************
*/
Void ALG_activate(IALG_Handle alg)
{
  /*--------------------------------------*/
  /* restore all persistant shared memory */
  /* nothing to do since memory allocation*/
  /* never shares any data                */
  /*--------------------------------------*/

  /*--------------------------------------*/
  /* do app specific initialization of    */
  /* scratch memory                       */
  /*--------------------------------------*/
  if (alg->fxns->algActivate != NULL) {
    alg->fxns->algActivate(alg);
  }
}


/**
********************************************************************************
 *  @func     ALG_deactivate
 *  @brief  This dunction does do app specific store of persistent data
 *
 *  @param[in]  alg : Pointer to the IALG_Handle structure
 *
 *  @return None
********************************************************************************
*/
Void ALG_deactivate(IALG_Handle alg)
{
  /*--------------------------------------*/
  /* do app specific store of persistent  */
  /* data                                 */
  /*--------------------------------------*/
  if (alg->fxns->algDeactivate != NULL) {
    alg->fxns->algDeactivate(alg);
  }

  /*--------------------------------------*/
  /* save all persistant shared memory    */
  /* nothing to do since memory allocation*/
  /* never shares any data                */
  /*--------------------------------------*/

}

/**
********************************************************************************
 *  @func     ALG_exit
 *  @brief
 *
 *  @param[in]
 *
 *  @return None
********************************************************************************
*/
Void ALG_exit(Void)
{
}

/**
********************************************************************************
 *  @func     ALG_init
 *  @brief
 *
 *  @param[in]
 *
 *  @return None
********************************************************************************
*/
Void ALG_init(Void)
{
}


/**
********************************************************************************
 *  @func     _ALG_freeMemory
 *  @brief  This function frees the specified number of memory chunks
 *
 *  @param[in]  memTab : Memory chunk array
 *
 *  @param[in]  n      : number of memory chunks to be freed
 *
 *  @return None
********************************************************************************
*/
Void _ALG_freeMemory(IALG_MemRec memTab[], Int n)
{
  Int i;

  for (i = 0; i < n; i++) {
    if (memTab[i].base != NULL) {
      if(memTab[i].space == IALG_EXTERNAL)
      {
        myFree(memTab[i].base);
      }
      else
      {
        gInternalMemory.sizeLeft += memTab[i].size ;
      }
    }
  }
}

void* myMemInternal(int alignment, int size)
{
  void *addr;  
  if(size > gInternalMemory.sizeLeft)
  {
    addr = NULL ;
  }
  else
  {
    addr = (void*) (gInternalMemory.memoryBasePtr + 
            (INT_MEM_SIZE - gInternalMemory.sizeLeft));
    gInternalMemory.sizeLeft -= size ;
  }         
  
  return addr;          
}

/**
********************************************************************************
 *  @func     _ALG_allocMemory
 *  @brief  This function allocates the specified number of memory chunks with
 *          alignment.
 *
 *  @param[in]  memTab : Memory chunk array
 *
 *  @param[in]  n      : number of memory chunks to be allocated
 *
 *  @return TRUE if allocated else FALSE
********************************************************************************
*/
Bool _ALG_allocMemory(IALG_MemRec memTab[], Int n)
{
  Int i;

  for (i = 0; i < n; i++) {
    if (memTab[i].space == IALG_EXTERNAL)
      memTab[i].base = (void *)myMemalign(memTab[i].alignment, memTab[i].size);
    else
      memTab[i].base = (void *)myMemInternal(memTab[i].alignment, memTab[i].size);  

    if (memTab[i].base == NULL) {
      _ALG_freeMemory(memTab, i);
      return (FALSE);
    }
  }

  return (TRUE);
}


#if defined (_54_) || (_55_) || (_28_)
/**
********************************************************************************
 *  @func     mem_align
 *  @brief  This function allocates the specified number of memory chunks with
 *          alignment.
 *
 *  @param[in]  memTab : Memory chunk array
 *
 *  @param[in]  n      : number of memory chunks to be allocated
 *
 *  @return     Aligned memory pointer if success or else zero if error
********************************************************************************
*/
void *mem_align(size_t alignment, size_t size)
{
  void **mallocPtr;
  void **retPtr;

  /* return if invalid size value */
  if (size <= 0) {
    return (0);
  }

  /*
   * If alignment is not a power of two, return what malloc returns. This is
   * how memalign behaves on the c6x.
   */
  if ((alignment & (alignment - 1)) || (alignment <= 1)) {
    if ((mallocPtr = malloc(size + sizeof(mallocPtr))) != NULL) {
      *mallocPtr = mallocPtr;
      mallocPtr++;
    }
    return ((void *)mallocPtr);
  }

  /* allocate block of memory */
  if (!(mallocPtr = malloc(alignment + size))) {
    return (0);
  }

  /* Calculate aligned memory address */
  retPtr = (void *)(((Uns) mallocPtr + alignment) & ~(alignment - 1));

  /* Set pointer to be used in the mem_free() fxn */
  retPtr[-1] = mallocPtr;

  /* return aligned memory */
  return ((void *)retPtr);
}

/**
********************************************************************************
 *  @func     mem_free
 *  @brief  This function frees a memory pointer
 *
 *  @param[in]  ptr : Pointer to the memory to be freed
 *
 *  @return None
********************************************************************************
*/
Void mem_free(void *ptr)
{
  free((void *)((void **)ptr)[-1]);
}

#endif
