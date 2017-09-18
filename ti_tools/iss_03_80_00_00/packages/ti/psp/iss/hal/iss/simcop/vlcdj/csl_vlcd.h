/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csl_vlcd.h
*
* This file contains register level values for VLCDJ module of SIMCOP in OMAP4/Monica
*
* @path VLCDJ\inc
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 8-Aug-2008 Sowmya Priya: Initial Release
*! 9-Sep-2008 Sowmya Priya : Updates in function Protypes
*! 19-Mar-2009          Phanish: Modified to remove the HWcontrol and GetHWstatus APIs 
*!                 and their dependencies. And now, HWsetup calls directly the CSL_FINS 
*========================================================================= */

#ifndef _CSL_VLCD_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_VLCD_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <ti/psp/iss/hal/iss/simcop/common/simcop.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl.h>

#include "cslr__vlcdj_1.h"

#define MAX_NMCUS 63

/* ======================================================================= */
/* CSL_VlcdjModeType enumeration for selecting mode of operation * * @param
 * CSL_SIMCOP_MODE_VLCD_DECODE VLCD decode mode * @param
 * CSL_SIMCOP_MODE_VLCD_ENCODE VLCD Encode Mode */
/* ======================================================================= */
typedef enum {
    CSL_SIMCOP_MODE_VLCD_ENCODE = CSL_VLCDJ_VLCDJ_CTRL_MODE_ENCODE,
    CSL_SIMCOP_MODE_VLCD_DECODE = CSL_VLCDJ_VLCDJ_CTRL_MODE_DECODE
} CSL_VlcdjModeType;

/* ======================================================================= */
/* CSL_VlcdjEncodeFmtType enumeration for selecting mode of operation * *
 * @param CSL_SIMCOP_VLCD_ENCODE_YUV420 YUV420 encode data * @param
 * CSL_SIMCOP_VLCD_ENCODE_YUV422 YUV422 encode data * @param
 * CSL_SIMCOP_VLCD_ENCODE_SEQBLK Sequential Blocks of encode data */
/* ======================================================================= */
typedef enum {
    CSL_SIMCOP_VLCD_ENCODE_YUV420 = CSL_VLCDJ_VLCDJE_CFG_FMT_YUV420,
    CSL_SIMCOP_VLCD_ENCODE_YUV422 = CSL_VLCDJ_VLCDJE_CFG_FMT_YUV422,
    CSL_SIMCOP_VLCD_ENCODE_SEQBLK = CSL_VLCDJ_VLCDJE_CFG_FMT_SEQBLOCKS
} CSL_VlcdjEncodeFmtType;

/* ======================================================================= */
/* CSL_VlcdjSpDataType enumeration for selecting mode of operation * * @param 
 * CSL_SIMCOP_VLCD_DECODE_YUV420 YUV420 decoded data * @param
 * CSL_SIMCOP_VLCD_DECODE_YUV422 YUV422 decoded data * @param
 * CSL_SIMCOP_VLCD_DECODE_SEQBLK Sequential Blocks of decoded data *
 * *======================================================================= */
typedef enum {
    CSL_SIMCOP_VLCD_DECODE_YUV420 = CSL_VLCDJ_VLCDJD_CFG_FMT_YUV420,
    CSL_SIMCOP_VLCD_DECODE_YUV422 = CSL_VLCDJ_VLCDJD_CFG_FMT_YUV422,
    CSL_SIMCOP_VLCD_DECODE_SEQBLK = CSL_VLCDJ_VLCDJD_CFG_FMT_SEQBLOCKS
} CSL_VlcdjDecodeFmtType;

/* ======================================================================= */
/* 
 * All  H/w signals generic to modules in SIMCOP
 
 * CSL_HwAutoGatingType enumeration for setting Autogating on or off
 * CSL_HwEnableType enumeration for setting the H/W on or off
 * CSL_HwTrgSrcType enumeration for checking trigger source (Sequencer/MMR)
 * CSL_HwStatus enumeration for checking busy status of H/W
 *CSL_HwIntEnableType enumeration for setting Interrupts
 */
/* ========================================================================== 
 */

typedef CSL_HwAutoGatingType CSL_VlcdjAutoGatingType;

typedef CSL_HwEnableType CSL_VlcdjHwEnableType;

typedef CSL_HwTrgSrcType CSL_VlcdjTrgSrcType;

typedef CSL_HwIntEnableType CSL_VlcdjIntEnableType;

typedef CSL_HwStatusType CSL_VlcdjHwStatusType;

/* ========================================================================== 
 */
/**
* This is a structure description for VLCDJ Object.
*
* ========================================================================== */
typedef struct {

        /** This is the mode which the CSL instance is opened     */
    CSL_OpenMode openMode;
        /** This is a unique identifier to the instance of VLCD being
         *  referred to by this object
         */
    CSL_Uid uid;
        /** This is the variable that contains the current state of a
         *  resource being shared by current instance of VLCD with
         *  other peripherals
         */
    CSL_Xio xio;
        /** This is a pointer to the registers of the instance of VLCD
         *  referred to by this object
         */
    CSL_VlcdjRegsOvly regs;
        /** This is the instance of VLCD being referred to by this object  */
    CSL_VlcdjNum perNum;

} CSL_VlcdjObj;

#if 0
/* ======================================================================= */
/* CSL_VlcdjHwCtrlCmd enumeration for selecting type of operation * * @param
 * CSL_VLCDJ_CMD_SETREVID Set Revision ID * @param CSL_VLCDJ_CMD_SETMODE Set 
 * mode is Encode or Decode * @param CSL_VLCDJ_CMD_SETAUTOGATING Set
 * Autogating is ON or OFF * @param CSL_VLCDJ_CMD_ENABLE Set Enable bit if
 * VLCDJ is triggered by MMR * @param CSL_VLCDJ_CMD_ENABLEBANK Set enabling
 * for Bitstream request bank signalling * @param CSL_VLCDJ_CMD_ENABLEINT Set 
 * Interrupt enable for task completion by VLCDJ * @param
 * CSL_VLCDJ_CMD_ENABLEINT_ERR Set decode Error Interrupt enable * @param
 * CSL_VLCDJ_CMD_SETTRIGGERSOURCE Set trigger source i.e H/w seq or MMR *
 * @param CSL_VLCDJ_CMD_CLEARBANK Set clrrb to clear request bank signals *
 * @param CSL_VLCDJE_CMD_SETENCFMT Set the encode format type YUV420,422 or
 * seq block * @param CSL_VLCDJE_CMD_SETRST_OFFSET Set Restart marker offset
 * to add to rst marker locations * @param CSL_VLCDJE_CMD_SETRST_EN Set
 * restart marker insertion enable * @param CSL_VLCDJE_CMD_SETRST_LOC Set
 * restart marker location recording enable * @param
 * CSL_VLCDJE_CMD_SETDCPREDY Set Encode DC predictor for Luma * @param
 * CSL_VLCDJE_CMD_SETDCPREDU Set Encode DC predictor for Chroma(Cb) * @param
 * CSL_VLCDJE_CMD_SETDCPREDV Set Encode DC predictor for Chroma(Cr) * @param
 * CSL_VLCDJE_CMD_SETNMCUS Set the number of MCU's to be operated by VLCDJ *
 * @param CSL_VLCDJE_CMD_SETBITPTR Set the bit pointer of BSMEM for encode *
 * @param CSL_VLCDJE_CMD_SETBYTEPTR Set the byte pointer of BSMEM for encode
 * * @param CSL_VLCDJE_CMD_SETCBUFS Set the bit stream circular buffer start
 * * @param CSL_VLCDJE_CMD_SETCBUFE Set the bit stream circular buffer
 * pointer end * @param CSL_VLCDJE_CMD_SETDCTPTR Set DCT matrix pointers *
 * @param CSL_VLCDJE_CMD_SETQMR Set Quantization Matrix pointers * @param
 * CSL_VLCDJE_CMD_SETVLCTBL Set Encode huffman Table Pointer * @param
 * CSL_VLCDJE_CMD_RSTPTR Set Restart Marker Pointer configuration * @param
 * CSL_VLCDJE_CMD_RSTINC Set Restart Marker Pointer configuration * @param
 * CSL_VLCDJE_CMD_RSTINIT Set Restart Marker Initialization * @param
 * CSL_VLCDJE_CMD_RSTPHASE Set Restart Marker Phase detection * @param
 * CSL_VLCDJE_CMD_RSTINTRVL Set Restart Marker Interval * @param
 * CSL_VLCDJD_CMD_SETDECFMT Set Decode Format * @param
 * CSL_VLCDJD_CMD_SETRSTEN Set Restart marker detection enabling in Decode *
 * @param CSL_VLCDJD_CMD_SETDCPREDY Set Decode DC predictor for Luma * @param 
 * CSL_VLCDJD_CMD_SETDCPREDU Set Decode DC predictor for Chroma(Cb) * @param
 * CSL_VLCDJD_CMD_SETDCPREDV Set Decode DC predictor for Chroma(Cr) * @param
 * CSL_VLCDJD_CMD_SETNMCUS Set the number of MCU's to be operated by VLCDJ * 
 * @param CSL_VLCDJD_CMD_SETSETBITPTR Set the bit pointer of BSMEM for DECODE
 * * @param CSL_VLCDJD_CMD_SETBYTEPTR Set the byte pointer of BSMEM for DECODE
 * * @param CSL_VLCDJD_CMD_SETCBUFS Set the bit stream circular buffer start
 * for DECODE * @param CSL_VLCDJD_CMD_SETCBUFE Set the bit stream circular
 * buffer pointer end for DECODE * @param CSL_VLCDJD_CMD_SETQMR Set
 * Quantization Matrix pointers for DECODE * @param CSL_VLCDJD_CMD_SETVLCTBL
 * Set huffman Table Pointer for DECODE * @param CSL_VLCDJD_CMD_SETDCTPTR Set
 * DCT matrix pointers for DECODE * @param CSL_VLCDJD_CMD_SETDEC_HUFF0 Set
 * Decode Huffman tables 0 * @param CSL_VLCDJD_CMD_SETDEC_HUFF1 Set Decode
 * Huffman tables 1 * @param CSL_VLCDJD_CMD_SETDEC_HUFF2 Set Decode Huffman
 * tables 2 * @param CSL_VLCDJD_CMD_SETDEC_HUFF3 Set Decode Huffman tables 3 */
/* ======================================================================= */
typedef enum {

    CSL_VLCDJ_CMD_SETREVID,
    CSL_VLCDJ_CMD_SETMODE,
    CSL_VLCDJ_CMD_SETAUTOGATING,
    CSL_VLCDJ_CMD_ENABLE,
    CSL_VLCDJ_CMD_CONFIGBANK,
    CSL_VLCDJ_CMD_ENABLEINT,
    CSL_VLCDJ_CMD_ENABLEINT_ERR,
    CSL_VLCDJ_CMD_SETTRIGGERSOURCE,
    CSL_VLCDJ_CMD_CLEARBANK,
    CSL_VLCDJE_CMD_SETENCFMT,
    CSL_VLCDJE_CMD_SETRST_OFFSET,
    CSL_VLCDJE_CMD_SETRST_EN,
    CSL_VLCDJE_CMD_SETRST_LOC,
    CSL_VLCDJE_CMD_SETDCPREDY,
    CSL_VLCDJE_CMD_SETDCPREDU,
    CSL_VLCDJE_CMD_SETDCPREDV,
    CSL_VLCDJE_CMD_SETNMCUS,
    CSL_VLCDJE_CMD_SETBITPTR,
    CSL_VLCDJE_CMD_SETBYTEPTR,
    CSL_VLCDJE_CMD_SETCBUFS,
    CSL_VLCDJE_CMD_SETCBUFE,
    CSL_VLCDJE_CMD_SETDCTPTR,
    CSL_VLCDJE_CMD_SETQMR,
    CSL_VLCDJE_CMD_SETVLCTBL,
    CSL_VLCDJE_CMD_RSTPTR,
    CSL_VLCDJE_CMD_RSTINC,
    CSL_VLCDJE_CMD_RSTINIT,
    CSL_VLCDJE_CMD_RSTPHASE,
    CSL_VLCDJE_CMD_RSTINTRVL,
    CSL_VLCDJD_CMD_SETDECFMT,
    CSL_VLCDJD_CMD_SETRSTEN,
    CSL_VLCDJD_CMD_SETDCPREDY,
    CSL_VLCDJD_CMD_SETDCPREDU,
    CSL_VLCDJD_CMD_SETDCPREDV,
    CSL_VLCDJD_CMD_SETNMCUS,
    CSL_VLCDJD_CMD_SETDCTQM,
    CSL_VLCDJD_CMD_SETSETBITPTR,
    CSL_VLCDJD_CMD_SETBYTEPTR,
    CSL_VLCDJD_CMD_SETCBUFS,
    CSL_VLCDJD_CMD_SETCBUFE,
    CSL_VLCDJD_CMD_SETQMR,
    CSL_VLCDJD_CMD_SETDCTPTR,
    CSL_VLCDJD_CMD_SETVLCTBL,
    CSL_VLCDJD_CMD_SETDEC_HUFF0,
    CSL_VLCDJD_CMD_SETDEC_HUFF1,
    CSL_VLCDJD_CMD_SETDEC_HUFF2,
    CSL_VLCDJD_CMD_SETDEC_HUFF3
} CSL_VlcdjHwCtrlCmd;

/* ======================================================================= */
/* CSL_VlcdjHwQuery enumeration to list query types * * @param
 * CSL_VLCDJ_QUERY_REVID, Get Revision ID * @param CSL_VLCDJ_QUERY_MODE, Get 
 * mode is Encode or Decode * @param CSL_VLCDJ_QUERY_AUTOGATING, Get
 * Autogating is ON or OFF * @param CSL_VLCDJ_QUERY_ENABLEBANK, Get enabling
 * for Bitstream request bank signalling * @param CSL_VLCDJ_QUERY_ENABLEINT,
 * Get Interrupt enable for task completion by VLCDJ * @param
 * CSL_VLCDJ_QUERY_INTDECERR, Get decode Error Interrupt enable * @param
 * CSL_VLCDJ_QUERY_BUSY, Get the state of VLCDJ i.e Busy/Idle * @param
 * CSL_VLCDJ_QUERY_BANK0, Get the status of Request Bank 0 * @param
 * CSL_VLCDJ_QUERY_BANK1, Get the status of Request Bank 0 * @param
 * CSL_VLCDJE_QUERY_ENCFMT, Get the encode format type YUV420,422 or seq
 * block * @param CSL_VLCDJ_QUERY_TRIGGERSOURCE, Get trigger source i.e H/w
 * seq or MMR * @param CSL_VLCDJE_QUERY_RST_EN, Get restart marker insertion 
 * enable * @param CSL_VLCDJE_QUERY_RST_LOC, Get restart marker location
 * recording enable * @param CSL_VLCDJE_QUERY_RST_OFFGet, Get Restart marker 
 * offGet to add to rst marker locations * @param CSL_VLCDJE_QUERY_DCPREDY,
 * Get Encode DC predictor for Luma * @param CSL_VLCDJE_QUERY_DCPREDU, Get
 * Encode DC predictor for Chroma(Cb) * @param CSL_VLCDJE_QUERY_DCPREDV, Get 
 * Encode DC predictor for Chroma(Cr) * @param CSL_VLCDJE_QUERY_NMCUS, Get
 * the number of MCU's to be operated by VLCDJ * @param
 * CSL_VLCDJE_QUERY_BITPTR, Get the bit pointer of BSMEM for encode * @param 
 * CSL_VLCDJE_QUERY_BYTEPTR, Get the byte pointer of BSMEM for encode *
 * @param CSL_VLCDJE_QUERY_CBUFS, Get the bit stream circular buffer start * 
 * @param CSL_VLCDJE_QUERY_CBUFE, Get the bit stream circular buffer pointer
 * end * @param CSL_VLCDJE_QUERY_DCTPTR, Get DCT matrix pointers * @param
 * CSL_VLCDJE_QUERY_QMR, Get Quantization Matrix pointers * @param
 * CSL_VLCDJE_QUERY_VLCTBL, Get Encode huffman Table Pointer * @param
 * CSL_VLCDJE_QUERY_RSTPTR, Get Restart Marker Pointer configuration *
 * @param CSL_VLCDJE_QUERY_RSTINC, Get Restart Marker Pointer configuration
 * * @param CSL_VLCDJE_QUERY_RSTINIT, Get Restart Marker Initialization *
 * @param CSL_VLCDJE_QUERY_RSTPHASE, Get Restart Marker Phase detection *
 * @param CSL_VLCDJE_QUERY_RSTINTRVL, Get Restart Marker Interval * @param
 * CSL_VLCDJD_QUERY_DECFMT, Get Decode Format * @param
 * CSL_VLCDJD_QUERY_RSTEN, Get Restart marker detection enabling in Decode * 
 * @param CSL_VLCDJD_QUERY_DCPREDY, Get Decode DC predictor for Luma *
 * @param CSL_VLCDJD_QUERY_DCPREDU, Get Decode DC predictor for Chroma(Cb) * 
 * @param CSL_VLCDJD_QUERY_DCPREDV, Get Decode DC predictor for Chroma(Cr) * 
 * @param CSL_VLCDJD_QUERY_NMCUS, Get the number of MCU's to be operated by
 * VLCDJ * @param CSL_VLCDJD_QUERY_GETBITPTR, Get the bit pointer of BSMEM
 * for DECODE * @param CSL_VLCDJD_QUERY_BYTEPTR, Get the byte pointer of
 * BSMEM for DECODE * @param CSL_VLCDJD_QUERY_CBUFS, Get the bit stream
 * circular buffer start for DECODE * @param CSL_VLCDJD_QUERY_CBUFE, Get the
 * bit stream circular buffer pointer end for DECODE * @param
 * CSL_VLCDJD_QUERY_QMR , Get Quantization Matrix pointers for DECODE *
 * @param CSL_VLCDJD_QUERY_DCTPTR, Get DCT matrix pointers for DECODE *
 * @param CSL_VLCDJD_QUERY_VLCTBL, Get control Table Pointer for DECODE *
 * @param CSL_VLCDJD_QUERY_DEC_HUFF0, Get Decode Huffman tables 0 * @param
 * CSL_VLCDJD_QUERY_DEC_HUFF1, Get Decode Huffman tables 1 * @param
 * CSL_VLCDJD_QUERY_DEC_HUFF2, Get Decode Huffman tables 2 * @param
 * CSL_VLCDJD_QUERY_DEC_HUFF3, Get Decode Huffman tables 3 * @param
 * CSL_VLCDJD_QUERY_DEC_ERROR Get VLCDJ Decode error if any */
/* ======================================================================= */
typedef enum {
    CSL_VLCDJ_QUERY_REVID,
    CSL_VLCDJ_QUERY_MODE,
    CSL_VLCDJ_QUERY_AUTOGATING,
    CSL_VLCDJ_QUERY_ENABLEBANK,
    CSL_VLCDJ_QUERY_ENABLEINT,
    CSL_VLCDJ_QUERY_INTDECERR,
    CSL_VLCDJ_QUERY_BUSY,
    CSL_VLCDJ_QUERY_BANK0,
    CSL_VLCDJ_QUERY_BANK1,
    CSL_VLCDJ_QUERY_TRIGGERSOURCE,
    CSL_VLCDJE_QUERY_ENCFMT,
    CSL_VLCDJE_QUERY_RST_OFFSET,
    CSL_VLCDJE_QUERY_RST_EN,
    CSL_VLCDJE_QUERY_RST_LOC,
    CSL_VLCDJE_QUERY_RST_OFFGet,
    CSL_VLCDJE_QUERY_DCPREDY,
    CSL_VLCDJE_QUERY_DCPREDU,
    CSL_VLCDJE_QUERY_DCPREDV,
    CSL_VLCDJE_QUERY_NMCUS,
    CSL_VLCDJE_QUERY_BITPTR,
    CSL_VLCDJE_QUERY_BYTEPTR,
    CSL_VLCDJE_QUERY_CBUFS,
    CSL_VLCDJE_QUERY_CBUFE,
    CSL_VLCDJE_QUERY_DCTPTR,
    CSL_VLCDJE_QUERY_QMR,
    CSL_VLCDJE_QUERY_VLCTBL,
    CSL_VLCDJE_QUERY_RSTPTR,
    CSL_VLCDJE_QUERY_RSTINC,
    CSL_VLCDJE_QUERY_RSTINIT,
    CSL_VLCDJE_QUERY_RSTPHASE,
    CSL_VLCDJE_QUERY_RSTINTRVL,
    CSL_VLCDJD_QUERY_DECFMT,
    CSL_VLCDJD_QUERY_RSTEN,
    CSL_VLCDJD_QUERY_DCPREDY,
    CSL_VLCDJD_QUERY_DCPREDU,
    CSL_VLCDJD_QUERY_DCPREDV,
    CSL_VLCDJD_QUERY_NMCUS,
    CSL_VLCDJD_QUERY_GETBITPTR,
    CSL_VLCDJD_QUERY_BYTEPTR,
    CSL_VLCDJD_QUERY_CBUFS,
    CSL_VLCDJD_QUERY_CBUFE,
    CSL_VLCDJD_QUERY_QMR,
    CSL_VLCDJD_QUERY_DCTPTR,
    CSL_VLCDJD_QUERY_VLCTBL,
    CSL_VLCDJD_QUERY_DEC_HUFF0,
    CSL_VLCDJD_QUERY_DEC_HUFF1,
    CSL_VLCDJD_QUERY_DEC_HUFF2,
    CSL_VLCDJD_QUERY_DEC_HUFF3,
    CSL_VLCDJD_QUERY_DEC_ERROR
} CSL_VlcdjHwQuery;

#endif

/*******************************************************************
* CSL_VlcdjHwSetup - VLCD Hardware Setup structure
* Type RW
*******************************************************************/
typedef struct CSL_VlcdHwSetup_ {
    /* revison ID */
    Uint16 nRevId;

    /* Autogating Parameter */
    CSL_VlcdjAutoGatingType nAutogating;
    /* H/w Enable */
    CSL_VlcdjHwEnableType nEnable;
    /* Trigger Source Type MMR_WRITE or HWSTART_SIGNAL */
    CSL_VlcdjTrgSrcType nTrgSrcType;
    /* Mode type, VLCD or IVLCD */
    CSL_VlcdjModeType nModeType;
    /* Interrupt enable for task completion */
    CSL_VlcdjIntEnableType nIntenDone;
    /* Interrupt enable for decode error */
    Uint16 nIntenErr;
    /* Enable memory bank 0 or 1 signalling */
    Uint16 nRben;
    /* Clear teh request signals i.e VLCDJ_REQ_B0/1 */
    Uint16 nClrrb;
    /* Encode Data type, YUV420/422, sequential blocks */
    CSL_VlcdjEncodeFmtType nEncFmtType;

    /* Encode restart marker insertion enable */
    Uint16 nEncRsten;
    /* Restart marker location recording enable */
    Uint16 nEncRlocen;
    /* No of MCU's to Encode */
    Uint16 nEncNmcus;
    /* Encode DC predictors Y U ANDV */
    Uint16 nEncDcpred[3];
    /* Encode byte pointer */
    Uint16 pEncByteptr;
    /* encode Bit Ptr */
    Uint16 pEncBitptr;
    /* Encode Circular buffer start */
    Uint16 pEncCbufStart;
    /* encode Circular buffer end */
    Uint16 pEncCbufEnd;
    /* Encode Restert marker configurations */
    Uint16 nEncRstIntrvl;
    Uint16 nEncRstPhase;
    Uint16 nEncRstInit;
    Uint16 nEncRstInc;

    /* Pointer to DCT coefficient table */
    Uint16 pEncDct;
    /* Encode pointer to the quantization matrix */
    Uint16 pEncQmr;
    /* Encode pointer to the huffman table */
    Uint16 pEncVlctbl;
    /* Encode Restart marker locations */
    Uint16 pEncRstptr;

    /* Address to add to encode restart marker locations */
    Uint32 nEncRstofst;

    /* Decode data type YUV420/422, sequential blocks */
    CSL_VlcdjDecodeFmtType nDecFmtType;

    /* Restart Marker detection enable */
    Uint16 nDecRsten;

    /* Decode number of MCU's */
    Uint16 nDecNmcus;
    /* dEcode DC predictors for Y U and v */
    Uint16 nDecDcpred[3];
    /* decode Byte pointer */
    Uint16 pDecByteptr;
    /* decode Bit Ptr */
    Uint16 pDecBitptr;
    /* Decode circular buffer pointers */
    Uint16 pDecCbufStart;
    Uint16 pDecCbufEnd;
    /* Decode DCT coefficient table */
    Uint16 pDecDct;
    /* Decode Quantization matrix table */
    Uint16 pDecQmr;
    /* Decode VLC control table */
    Uint16 pDecCtrltbl;
    /* Decode VLC decode table */
    Uint16 pDecDcdtbl[4];

    /* Decode DCT error */
    Uint16 pDecDcterr;
    Uint16 nDecinitCtrltbl;
} CSL_VlcdHwSetup;

typedef CSL_VlcdjObj *CSL_VlcdjHandle;

/*****************************************************************************/
/* CSL Level 1 function prototypes to Intialize , setup and control and
 * deinitialize the VLCDJ block */
 /*****************************************************************************/

/* ===================================================================
 *  @func     CSL_VlcdjInit                                               
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
CSL_Status CSL_VlcdjInit(CSL_VlcdjHandle hndl);

/* ===================================================================
 *  @func     CSL_VlcdjOpen                                               
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
CSL_Status CSL_VlcdjOpen(CSL_VlcdjObj * hVlcdjObj,
                         CSL_VlcdjNum VlcdjNum, CSL_OpenMode openMode);
/* ===================================================================
 *  @func     CSL_VlcdjHwSetup                                               
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
CSL_Status CSL_VlcdjHwSetup(CSL_VlcdjHandle hndl, CSL_VlcdHwSetup * setup);

#if 0
/* ===================================================================
 *  @func     CSL_VlcdjHwControl                                               
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
CSL_Status CSL_VlcdjHwControl(CSL_VlcdjHandle hndl, CSL_VlcdjHwCtrlCmd cmd,
                              void *data);
/* ===================================================================
 *  @func     CSL_VlcdjGetHwStatus                                               
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
CSL_Status CSL_VlcdjGetHwStatus(CSL_VlcdjHandle hndl, CSL_VlcdjHwQuery query,
                                void *data);
#endif
/* ===================================================================
 *  @func     CSL_VlcdjClose                                               
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
CSL_Status CSL_VlcdjClose(CSL_VlcdjHandle hndl);

/*****************************************************************************/
 /* DBC macros for VLCDJ */
 /*****************************************************************************/

#define CSL_VLCDJ_REQUIRE(C,V) { \
    if (!(C)) { \
        status = V; \
        SIMCOP_PRINT ("Error ::%s:: %s : %s : %d :: Exiting because : %s ::requirement is not met\n", \
                #V,__FILE__, __FUNCTION__, __LINE__, #C); \
        goto EXIT; \
    } \
}

#define CSL_VLCDJ_ASSERT CSL_EXIT_IF

/* Mainly check for handle corruption */
#define CSL_VLCDJ_ENSURE(C,V) { \
    if (!(C)) { \
        status = V; \
        SIMCOP_PRINT ("Error ::%s:: %s : %s : %d :: Exiting because : %s :: is not ensured\n", \
                #V,__FILE__, __FUNCTION__, __LINE__, #C); \
        goto EXIT; \
    } \
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
