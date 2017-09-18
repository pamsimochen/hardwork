/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file _csl_vlcd.h
*
* This file contains register level values for VLCDJ module  of SIMCOP in OMAP4/Monica
*
* @path VLCDJ\inc
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 8-August 2008 Sowmya Priya: Initial Release
*! 9-Sep     2008 Sowmya Priya : Updates in function Protypes
*! 19-Mar-2009 Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS 
*========================================================================= */

#ifndef __CSL_VLCD_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __CSL_VLCD_H_

#include "../csl_vlcd.h"

#if 0
/* ======================================================================= */
/* CSL_VlcdjRstEnType enumeration for selecting restart marker related
 * configuration * * @param CSL_VLCDJ_RLOCEN Selecting restart marker
 * location recording enable * @param CSL_VLCDJ_RSTEN Selecting restart
 * marker insertion enable */
/* ======================================================================= */

typedef enum {
    CSL_VLCDJ_RLOCEN,
    CSL_VLCDJ_RSTEN
} CSL_VlcdjRstEnType;

/* ======================================================================= */
/* CSL_VlcdjDCPredictorType enumeration for selecting DC predictors for
 * configuration/query * * @param CSL_VLCDJ_DCPREDY Selecting restart marker
 * location recording enable * @param CSL_VLCDJ_DCPREDU Selecting restart
 * marker insertion enable * @param CSL_VLCDJ_DCPREDV Selecting restart
 * marker insertion enable */
/* ======================================================================= */

typedef enum {
    CSL_VLCDJ_DCPREDY,
    CSL_VLCDJ_DCPREDU,
    CSL_VLCDJ_DCPREDV
} CSL_VlcdjDCPredictorType;

/* ======================================================================= */
/* CSL_VlcdjPtrType enumeration for selecting Vlcdj buffer pointers * *
 * @param CSL_VLCDJ_BSP_BITPTR Select the bit pointer of BSMEM * @param
 * CSL_VLCDJ_BSP_BYTEPTR Select the byte pointer of BSMEM * @param
 * CSL_VLCDJ_CBUF_START Select the bit stream circular buffer start * @param
 * CSL_VLCDJ_CBUF_END Select the bit stream circular buffer end * @param
 * CSL_VLCDJ_QMR_PTR Select Quantization Matrix pointers for * @param
 * CSL_VLCDJ_DCT_PTR Select DCT pointer * @param CSL_VLCDJ_HUFF_PTR Select
 * Encode huffman Table Pointer * @param CSL_VLCDJ_RST_PTR Select Restart
 * marker pointer * @param CSL_VLCDJ_DEC_CTRLTBL Select the decode control
 * table pointer * @param CSL_VLCDJ_DEC_HUFF0 Select Decode Huffman tables 0
 * * @param CSL_VLCDJ_DEC_HUFF1 Select Decode Huffman tables 1 * @param
 * CSL_VLCDJ_DEC_HUFF2 Select Decode Huffman tables 2 * @param
 * CSL_VLCDJ_DEC_HUFF3 Select Decode Huffman tables 3 */
/* ======================================================================= */

typedef enum {
    CSL_VLCDJ_BSP_BITPTR,
    CSL_VLCDJ_BSP_BYTEPTR,
    CSL_VLCDJ_CBUF_START,
    CSL_VLCDJ_CBUF_END,
    CSL_VLCDJ_QMR_PTR,
    CSL_VLCDJ_DCT_PTR,
    CSL_VLCDJ_HUFF_PTR,
    CSL_VLCDJ_RST_PTR,
    CSL_VLCDJ_DEC_CTRLTBL,
    CSL_VLCDJ_DEC_HUFF0,
    CSL_VLCDJ_DEC_HUFF1,
    CSL_VLCDJ_DEC_HUFF2,
    CSL_VLCDJ_DEC_HUFF3
} CSL_VlcdjPtrType;

/* ======================================================================= */
/* CSL_VlcdjRstConfigType enumeration for selecting restart marker related
 * configuration * * @param CSL_VLCDJ_INC Select Restart Marker Increment *
 * @param CSL_VLCDJ_INIT Select Restart Marker Initialization * @param
 * CSL_VLCDJ_PHASE Select Restart Marker Phase * @param CSL_VLCDJ_INTRVL
 * Select Restart Marker Interval */
/* ======================================================================= */

typedef enum {
    CSL_VLCDJ_INC,
    CSL_VLCDJ_INIT,
    CSL_VLCDJ_PHASE,
    CSL_VLCDJ_INTRVL
} CSL_VlcdjRstConfigType;

/*****************************************************************************/
 /* individual functions to Set writable registers in the * VLCDJ module of
  * SIMCOP */
 /*****************************************************************************/

/* ===================================================================
 *  @func     _CSL_VlcdjSetRevId                                               
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
CSL_Status _CSL_VlcdjSetRevId(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjEnable                                               
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
CSL_Status _CSL_VlcdjEnable(CSL_VlcdjHandle hndl, CSL_VlcdjHwEnableType data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetModeType                                               
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
CSL_Status _CSL_VlcdjSetModeType(CSL_VlcdjHandle hndl, CSL_VlcdjModeType data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetTrigSrcType                                               
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
CSL_Status _CSL_VlcdjSetTrigSrcType(CSL_VlcdjHandle hndl,
                                    CSL_VlcdjTrgSrcType data);
/* ===================================================================
 *  @func     _CSL_VlcdjSetAutoGating                                               
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
CSL_Status _CSL_VlcdjSetAutoGating(CSL_VlcdjHandle hndl,
                                   CSL_VlcdjAutoGatingType data);
/* ===================================================================
 *  @func     _CSL_VlcdjSetInterruptEnable                                               
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
CSL_Status _CSL_VlcdjSetInterruptEnable(CSL_VlcdjHandle hndl,
                                        CSL_VlcdjIntEnableType data);
/* ===================================================================
 *  @func     _CSL_VlcdjSetInterruptDecErr                                               
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
CSL_Status _CSL_VlcdjSetInterruptDecErr(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetRbEnable                                               
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
CSL_Status _CSL_VlcdjSetRbEnable(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetClrrb                                               
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
CSL_Status _CSL_VlcdjSetClrrb(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetEncNMcus                                               
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
CSL_Status _CSL_VlcdjSetEncNMcus(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetEncFmtDataType                                               
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
CSL_Status _CSL_VlcdjSetEncFmtDataType(CSL_VlcdjHandle hndl,
                                       CSL_VlcdjEncodeFmtType data);
/* ===================================================================
 *  @func     _CSL_VlcdjSetRSTEnc                                               
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
CSL_Status _CSL_VlcdjSetRSTEnc(CSL_VlcdjHandle hndl, Uint16 data,
                               CSL_VlcdjRstEnType type);
/* ===================================================================
 *  @func     _CSL_VlcdjSetPREDEnc                                               
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
CSL_Status _CSL_VlcdjSetPREDEnc(CSL_VlcdjHandle hndl, Uint16 data,
                                CSL_VlcdjDCPredictorType dcpred);
/* ===================================================================
 *  @func     _CSL_VlcdjSetEncPtrTable                                               
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
CSL_Status _CSL_VlcdjSetEncPtrTable(CSL_VlcdjHandle hndl, Uint16 data,
                                    CSL_VlcdjPtrType ptrtype);
/* ===================================================================
 *  @func     _CSL_VlcdjSetRSTConfig                                               
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
CSL_Status _CSL_VlcdjSetRSTConfig(CSL_VlcdjHandle hndl, Uint16 data,
                                  CSL_VlcdjRstConfigType rstconfig);
/* ===================================================================
 *  @func     _CSL_VlcdjSetEncRSTOffset                                               
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
CSL_Status _CSL_VlcdjSetEncRSTOffset(CSL_VlcdjHandle hndl, Uint32 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetDecNMcus                                               
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
CSL_Status _CSL_VlcdjSetDecNMcus(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetDecFmtDataType                                               
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
CSL_Status _CSL_VlcdjSetDecFmtDataType(CSL_VlcdjHandle hndl,
                                       CSL_VlcdjDecodeFmtType data);
/* ===================================================================
 *  @func     _CSL_VlcdjSetRSTDec                                               
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
CSL_Status _CSL_VlcdjSetRSTDec(CSL_VlcdjHandle hndl, Uint16 data);

/* ===================================================================
 *  @func     _CSL_VlcdjSetDecPtrTable                                               
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
CSL_Status _CSL_VlcdjSetDecPtrTable(CSL_VlcdjHandle hndl, Uint16 data,
                                    CSL_VlcdjPtrType ptrtype);
/* ===================================================================
 *  @func     _CSL_VlcdjSetPREDDec                                               
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
CSL_Status _CSL_VlcdjSetPREDDec(CSL_VlcdjHandle hndl, Uint16 data,
                                CSL_VlcdjDCPredictorType dcpred);

/*****************************************************************************/
 /* individual functions to Query readable registers in the * VLCDJ module
  * of SIMCOP */
 /*****************************************************************************/

/* ===================================================================
 *  @func     _CSL_VlcdjGetRevId                                               
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
CSL_Status _CSL_VlcdjGetRevId(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetModeType                                               
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
CSL_Status _CSL_VlcdjGetModeType(CSL_VlcdjHandle hndl,
                                 CSL_VlcdjModeType * data);
/* ===================================================================
 *  @func     _CSL_VlcdjGetTrigSrcType                                               
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
CSL_Status _CSL_VlcdjGetTrigSrcType(CSL_VlcdjHandle hndl,
                                    CSL_VlcdjTrgSrcType * data);
/* ===================================================================
 *  @func     _CSL_VlcdjGetAutoGating                                               
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
CSL_Status _CSL_VlcdjGetAutoGating(CSL_VlcdjHandle hndl,
                                   CSL_VlcdjAutoGatingType * data);
/* ===================================================================
 *  @func     _CSL_VlcdjGetnterruptEnable                                               
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
CSL_Status _CSL_VlcdjGetnterruptEnable(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetInterruptDecErr                                               
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
CSL_Status _CSL_VlcdjGetInterruptDecErr(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetRbEnable                                               
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
CSL_Status _CSL_VlcdjGetRbEnable(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetState                                               
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
CSL_Status _CSL_VlcdjGetState(CSL_VlcdjHandle hndl,
                              CSL_VlcdjHwStatusType * data);
CSL_Status _CSL_VlcdjStatusBnk0(CSL_VlcdjHandle hndl, Uint16 * data);

CSL_Status _CSL_VlcdjStatusBnk1(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetEncNMcus                                               
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
CSL_Status _CSL_VlcdjGetEncNMcus(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetEncFmtDataType                                               
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
CSL_Status _CSL_VlcdjGetEncFmtDataType(CSL_VlcdjHandle hndl,
                                       CSL_VlcdjEncodeFmtType * data);
/* ===================================================================
 *  @func     _CSL_VlcdjGetRSTEnc                                               
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
CSL_Status _CSL_VlcdjGetRSTEnc(CSL_VlcdjHandle hndl, Uint16 * data,
                               CSL_VlcdjRstEnType type);
/* ===================================================================
 *  @func     _CSL_VlcdjGetPREDEnc                                               
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
CSL_Status _CSL_VlcdjGetPREDEnc(CSL_VlcdjHandle hndl, Uint16 * data,
                                CSL_VlcdjDCPredictorType dcpred);
/* ===================================================================
 *  @func     _CSL_VlcdjGetEncPtrTable                                               
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
CSL_Status _CSL_VlcdjGetEncPtrTable(CSL_VlcdjHandle hndl, Uint16 * data,
                                    CSL_VlcdjPtrType ptrtype);
/* ===================================================================
 *  @func     _CSL_VlcdjGetRSTOffSet                                               
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
CSL_Status _CSL_VlcdjGetRSTOffSet(CSL_VlcdjHandle hndl, Uint32 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetDecNMcus                                               
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
CSL_Status _CSL_VlcdjGetDecNMcus(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetRSTConfig                                               
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
CSL_Status _CSL_VlcdjGetRSTConfig(CSL_VlcdjHandle hndl, Uint16 * data,
                                  CSL_VlcdjRstConfigType rstconfig);
/* ===================================================================
 *  @func     _CSL_VlcdjGetDecFmtDataType                                               
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
CSL_Status _CSL_VlcdjGetDecFmtDataType(CSL_VlcdjHandle hndl,
                                       CSL_VlcdjDecodeFmtType * data);
/* ===================================================================
 *  @func     _CSL_VlcdjGetPREDDec                                               
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
CSL_Status _CSL_VlcdjGetPREDDec(CSL_VlcdjHandle hndl, Uint16 * data,
                                CSL_VlcdjDCPredictorType dcpred);
/* ===================================================================
 *  @func     _CSL_VlcdjGetRSTDec                                               
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
CSL_Status _CSL_VlcdjGetRSTDec(CSL_VlcdjHandle hndl, Uint16 * data);

/* ===================================================================
 *  @func     _CSL_VlcdjGetDecPtrTable                                               
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
CSL_Status _CSL_VlcdjGetDecPtrTable(CSL_VlcdjHandle hndl, Uint16 * data,
                                    CSL_VlcdjPtrType ptrtype);
/* ===================================================================
 *  @func     _CSL_VlcdjGetDecErr                                               
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
CSL_Status _CSL_VlcdjGetDecErr(CSL_VlcdjHandle hndl, Uint16 * data);
#endif

/******************************************************************************
 * Function Prototypes
 ******************************************************************************/
/*****************************************************************************/
 /* Functions to get the VLCDJ unique identifier & resource allocation mask
  * into the handle */
 /*****************************************************************************/
/* ===================================================================
 *  @func     _CSL_VlcdjGetAttrs                                               
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
CSL_Status _CSL_VlcdjGetAttrs(CSL_VlcdjNum vlcdjNum, CSL_VlcdjHandle hVlcdj);

/* ===================================================================
 *  @func     _CSL_Vlcdj_Reset_Val_Read                                               
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
void _CSL_Vlcdj_Reset_Val_Read(CSL_VlcdjHandle hndl);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*__CSL_VLCD_H_*/
