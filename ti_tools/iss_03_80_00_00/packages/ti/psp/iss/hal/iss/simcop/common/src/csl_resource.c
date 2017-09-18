/** ==================================================================
 *  @file   csl_resource.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ---- File: <_csl_resource.c> ---- */
// #include <csl.h>
#include "../csl_resource.h"
#include "../inc/csl_sysdata.h"

/* Assuming resInUse array elements to be 32 bits long Calculate index and
 * shift for the bit that represents given resId */
#define _CSL_GETINDEX(resId)		(resId >> 5)
#define _CSL_GETSHIFT(resId, index)	(resId -  32*index)

/* ===================================================================
 *  @func     _CSL_certifyOpen                                               
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
CSL_ResHandle _CSL_certifyOpen(CSL_ResHandle rHandle, CSL_Status * st)
{

    *st = CSL_SOK;

    /* Bug fix: Clearquest bug PSG00000025 */
    /* Check for CSL_FAIL is done instead of CSL_ESYS_FAIL */
    // if(_CSL_resourceTest(((CSL_ResAttrs *)rHandle)->uid) == CSL_ESYS_FAIL)
    if (_CSL_resourceTest(((CSL_ResAttrs *) rHandle)->uid) == CSL_FAIL)
    {
        /* This specific resource is already in use */
        *st = CSL_ESYS_INUSE;
        return ((CSL_ResHandle) NULL);
    }

    /* If no need for pin-sharing, treat this as Exclusive Open */
    if (((CSL_ResAttrs *) rHandle)->xio == 0)
        ((CSL_ResAttrs *) rHandle)->openMode = CSL_EXCLUSIVE;

    if (((CSL_ResAttrs *) rHandle)->openMode == CSL_EXCLUSIVE)
    {
        /* Check for absolute access to XIOs */
        if (_CSL_xioTest(((CSL_ResAttrs *) rHandle)->xio) == CSL_FAIL)
        {
            *st = CSL_ESYS_XIO;
        }
        else
        {
            _CSL_xioSet(((CSL_ResAttrs *) rHandle)->xio);
            *st = CSL_SOK;
        }
    }

    if (*st == CSL_SOK)
        _CSL_resourceSet(((CSL_ResAttrs *) rHandle)->uid);

    if (CSL_sysDataHandle->altRouteHandler != NULL)
        (CSL_sysDataHandle->altRouteHandler) (rHandle);

    if (CSL_sysDataHandle->p2vHandler != NULL)
        (CSL_sysDataHandle->p2vHandler) (rHandle);
    return (rHandle);
}

/* CSL Private Function Definition: 'CSL_certifyClose' */
/* ===================================================================
 *  @func     _CSL_certifyClose                                               
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
CSL_Status _CSL_certifyClose(CSL_ResHandle rHandle)
{
    if (((CSL_ResAttrs *) rHandle)->openMode == CSL_EXCLUSIVE)
        _CSL_xioClear(((CSL_ResAttrs *) rHandle)->xio);

    _CSL_resourceClear(((CSL_ResAttrs *) rHandle)->uid);

    return CSL_SOK;
}

/* FUNCTIONS DEFINED BELOW MODIFIES GLOBAL DATA STRUCTURE, BELOW FUNCTIONS
 * MUST BE CALLED IN ATOMIC REGION. */
/* ===================================================================
 *  @func     _CSL_resourceTest                                               
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
CSL_Test _CSL_resourceTest(CSL_Uid resId)
{
    Uint32 index, shift;

    index = _CSL_GETINDEX(resId);
    shift = _CSL_GETSHIFT(resId, index);

    if ((CSL_sysDataHandle->resInUse[index] >> shift) & 0x1)
        return CSL_FAIL;
    else
        return CSL_PASS;
}

/* ===================================================================
 *  @func     _CSL_xioTest                                               
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
CSL_Test _CSL_xioTest(CSL_Xio xio)
{
    if (xio == 0)
        return CSL_PASS;
    if (xio & CSL_sysDataHandle->xioInUse)
        return CSL_FAIL;
    else
        return CSL_PASS;
}

/* ===================================================================
 *  @func     _CSL_xioSet                                               
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
void _CSL_xioSet(CSL_Xio xio)
{
    CSL_sysDataHandle->xioInUse |= xio;
    return;
}

/* ===================================================================
 *  @func     _CSL_resourceSet                                               
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
void _CSL_resourceSet(CSL_Uid resId)
{
    Uint32 index, shift;

    index = _CSL_GETINDEX(resId);
    shift = _CSL_GETSHIFT(resId, index);

    CSL_sysDataHandle->resInUse[index] |= (1 << shift);
    return;
}

/* ===================================================================
 *  @func     _CSL_resourceClear                                               
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
void _CSL_resourceClear(CSL_Uid resId)
{
    Uint32 index, shift;

    index = _CSL_GETINDEX(resId);
    shift = _CSL_GETSHIFT(resId, index);

    CSL_sysDataHandle->resInUse[index] &= ~(1 << shift);
    return;
}

/* ===================================================================
 *  @func     _CSL_xioClear                                               
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
void _CSL_xioClear(CSL_Xio xio)
{
    CSL_sysDataHandle->xioInUse &= ~xio;
    return;
}
