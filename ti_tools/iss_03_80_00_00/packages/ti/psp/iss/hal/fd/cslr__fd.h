/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file cslr__fd.h
 *
 * This File contains declarations of register overlay structure for FD module. It also
 * defines tokens for register fields in FD module. It is targeted at MONICA/OMAP4. 
 * 
 *
 * @path  $(DUCATIVOB)\drivers\csl\fd\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 23-Nov-2009 Vijay Badiger: Initial Release
 *! 
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef _CSLR__FD_1_H_
#define _CSLR__FD_1_H_

#ifdef __cplusplus

extern "C" {
#endif
    /* User code goes here */
    /* ------compilation control switches ------------------------- */
   /****************************************************************
    *  INCLUDE FILES                                                 
    ****************************************************************/
    /* ----- system and platform files ---------------------------- */
   /*-------program files ----------------------------------------*/

#include "../iss/simcop/common/cslr.h"
#include "../iss/simcop/common/csl_types.h"

    /* Minimum unit = 1 byte */

   /****************************************************************
    *  PUBLIC DECLARATIONS Defined here, used elsewhere
    ****************************************************************/
   /*--------data declarations -----------------------------------*/

   /**************************************************************************\
    * Register Overlay Structure
    \**************************************************************************/

    typedef struct {
        volatile Uint32 FD_CENTERX;
        volatile Uint32 FD_CENTERY;
        volatile Uint32 FD_CONFSIZE;
        volatile Uint32 FD_ANGLE;
    } FD_RESULT_REG;

    /* ========================================================================== 
     */
   /**
    * This is a structure description for CSL_FdRegs. This structure is used for register overlay of the FD module. 
    *
    * @param  FDIF_REVISION  IP revision identifier register
    *
    * @param  FIDIF_HWINFO  IP module's hardware configuration register
    *
    * @param  FDIF_SYSCONFIG  Clock management configuration register
    *
    * @param  FDIF_IRQ_EOI  End Of Interrupt number specification register
    *
    * @param  FDIF_IRQSTATUS_RAW__0 --FDIF_IRQSTATUS_RAW__2  Per-event raw interrupt status vector register
    *
    * @param  FDIF_IRQSTATUS__0 -- FDIF_IRQSTATUS__2     Per-event "enabled" interrupt status vector register
    *
    * @param  FDIF_IRQENABLE_SET__0 -- FDIF_IRQENABLE_SET__2  Per-event interrupt enable bit vector register
    *
    * @param  FDIF_IRQENABLE_CLR__0 -- FDIF_IRQENABLE_CLR__2  Per-event interrupt enable bit vector register
    *
    * @param  FDIF_PICADDR  Picture data store address register
    *
    * @param  FDIF_CTRL  
    * 
    * @param  FDIF_WKADDR Work area address register
    *
    * @param  FDIF_TESTMON  Test register for internal debug purposes
    *
    * @param  FD_CTRL  Control register
    *
    * @param  FD_DNUM  Face Detection Result Count Register
    *
    * @param  FD_DCOND Detection Condition Setting Register

    * @param  FD_STARTX  Detection Area Setting Register: X Start Coordinate 
    *
    * @param  FD_STARTY  Detection Area Setting Register: Y Start Coordinate
    *
    * @param  FD_SIZEX  Detection Area Setting Register: X Direction Size
    *
    * @param  FD_SIZEY  Detection Area Setting Register: Y Direction Size
    
    * @param  FD_LHIT  Threshold Setting Register

    * @param  FD_CENTERX[35]  Detection Result: X Coordinate
    *
    * @param  FD_CENTERY[35]  Detection Result: Y Coordinate
    *
    * @param  FD_CONFSIZE[35]  Detection Result: Confidence Level and Size
    *
    * @param  FD_ANGLE[35]  Detection Result: Angle

    * @see 
    */
    /* ========================================================================== 
     */
    typedef struct {

        volatile Uint32 FDIF_REVISION;
        volatile Uint32 FIDIF_HWINFO;
        volatile Uint8 RSVD0[8];
        volatile Uint32 FDIF_SYSCONFIG;
        volatile Uint8 RSVD1[12];
        volatile Uint32 FDIF_IRQ_EOI;
        volatile Uint32 FDIF_IRQSTATUS_RAW__0;
        volatile Uint32 FDIF_IRQSTATUS__0;
        volatile Uint32 FDIF_IRQENABLE_SET__0;
        volatile Uint32 FDIF_IRQENABLE_CLR__0;
        volatile Uint32 FDIF_IRQSTATUS_RAW__1;
        volatile Uint32 FDIF_IRQSTATUS__1;
        volatile Uint32 FDIF_IRQENABLE_SET__1;
        volatile Uint32 FDIF_IRQENABLE_CLR__1;
        volatile Uint32 FDIF_IRQSTATUS_RAW__2;
        volatile Uint32 FDIF_IRQSTATUS__2;
        volatile Uint32 FDIF_IRQENABLE_SET__2;
        volatile Uint32 FDIF_IRQENABLE_CLR__2;
        volatile Uint8 RSVD2[12];
        volatile Uint32 FDIF_PICADDR;
        volatile Uint32 FDIF_CTRL;
        volatile Uint32 FDIF_WKADDR;
        volatile Uint32 FDIF_TESTMON;
        volatile Uint8 RSVD3[16];
        volatile Uint32 FD_CTRL;
        volatile Uint32 FD_DNUM;
        volatile Uint32 FD_DCOND;
        volatile Uint32 FD_STARTX;
        volatile Uint32 FD_STARTY;
        volatile Uint32 FD_SIZEX;
        volatile Uint32 FD_SIZEY;
        volatile Uint32 FD_LHIT;
        volatile Uint8 RSVD4[192];
        /* volatile Uint32 FD_CENTERX[35] ; volatile Uint32 FD_CENTERY[35] ;
         * volatile Uint32 FD_CONFSIZE[35]; volatile Uint32 FD_ANGLE [35]; */
        FD_RESULT_REG RESULTS[35];

    } CSL_FdRegs;

   /**************************************************************************\
    * Field Definition Macros
    \**************************************************************************/

    /* FDIF_RIVISION */

#define CSL_FDIF_RIVISION_Y_MINOR_MASK (0x0000003Fu)
#define CSL_FDIF_RIVISION_Y_MINOR_SHIFT (0x00000000u)
#define CSL_FDIF_RIVISION_Y_MINOR_RESETVAL  (0x00000000u)

#define CSL_FDIF_RIVISION_CUSTOM_MASK (0x000000C0u)
#define CSL_FDIF_RIVISION_CUSTOM_SHIFT (0x00000006u)
#define CSL_FDIF_RIVISION_CUSTOM_RESETVAL  (0x00000000u)

#define CSL_FDIF_RIVISION_X_MAJOR_MASK (0x00000700u)
#define CSL_FDIF_RIVISION_X_MAJOR_SHIFT (0x00000008u)
#define CSL_FDIF_RIVISION_X_MAJOR_RESETVAL (0x00000001u)

#define CSL_FDIF_RIVISION_R_RTL_MASK (0x0000F800u)
#define CSL_FDIF_RIVISION_R_RTL_SHIFT (0x00000011u)
#define CSL_FDIF_RIVISION_R_RTL_RESETVAL (0x00000000u)

#define CSL_FDIF_RIVISION_FUNC_MASK (0x0FFF0000u)
#define CSL_FDIF_RIVISION_FUNC_SHIFT (0x00000016u)
#define CSL_FDIF_RIVISION_FUNC_RESETVAL (0x00000000u)

#define CSL_FDIF_RIVISION_SCHEME_MASK (0xC0000000u)
#define CSL_FDIF_RIVISION_SCHEME_SHIFT (0x00000030u)
#define CSL_FDIF_RIVISION_SCHEME_RESETVAL (0x00000001u)

    /* FDIF_HWINFO */

#define CSL_FDIF_HWINFO_FDIF_OCP_TAGS_MASK (0x0000000Fu)
#define CSL_FDIF_HWINFO_FDIF_OCP_TAGS_SHIFT (0x00000000u)
#define CSL_FDIF_HWINFO_FDIF_OCP_TAGS_RESETVAL (0x0000000Fu)

    /* FDIF_SYSCONFIG */
#define CSL_FDIF_SYSCONFIG_SOFTRESET_MASK (0x00000001u)
#define CSL_FDIF_SYSCONFIG_SOFTRESET_SHIFT (0x00000000u)
#define CSL_FDIF_SYSCONFIG_SOFTRESET_RESETVAL (0x00000000u)

#define CSL_FDIF_SYSCONFIG_IDLEMODE_MASK (0x0000000Cu)
#define CSL_FDIF_SYSCONFIG_IDLEMODE_SHIFT (0x00000002u)
#define CSL_FDIF_SYSCONFIG_IDLEMODE_RESETVAL (0x00000002u)

#define CSL_FDIF_SYSCONFIG_STANDBYMODE_MASK (0x00000030u)
#define CSL_FDIF_SYSCONFIG_STANDBYMODE_SHIFT (0x00000004u)
#define CSL_FDIF_SYSCONFIG_STANDBYMODE_RESETVAL (0x00000002u)

    /* FDIF_IRQ_EOI */
#define CSL_FDIF_IRQ_EOI_LINE_NUMBER_MASK (0x00000003u)
#define CSL_FDIF_IRQ_EOI_LINE_NUMBER_SHIFT (0x00000000u)
#define CSL_FDIF_IRQ_EOI_LINE_NUMBER_RESETVAL  (0x00000000u)

    /* FDIF_IRQSTATUS_RAW__0 */
#define CSL_FDIF_IRQSTATUS_RAW__0_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQSTATUS_RAW__0_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQSTATUS_RAW__0_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQSTATUS_RAW__0_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQSTATUS_RAW__0_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQSTATUS_RAW__0_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQSTATUS_RAW__1 */

#define CSL_FDIF_IRQSTATUS_RAW__1_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQSTATUS_RAW__1_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQSTATUS_RAW__1_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQSTATUS_RAW__1_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQSTATUS_RAW__1_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQSTATUS_RAW__1_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQSTATUS_RAW__2 */

#define CSL_FDIF_IRQSTATUS_RAW__2_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQSTATUS_RAW__2_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQSTATUS_RAW__2_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQSTATUS_RAW__2_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQSTATUS_RAW__2_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQSTATUS_RAW__2_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQSTATUS__0 */

#define CSL_FDIF_IRQSTATUS__0_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQSTATUS__0_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQSTATUS__0_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQSTATUS__0_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQSTATUS__0_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQSTATUS__0_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQSTATUS_RAW__1 */

#define CSL_FDIF_IRQSTATUS__1_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQSTATUS__1_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQSTATUS__1_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQSTATUS__1_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQSTATUS__1_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQSTATUS__1_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQSTATUS_RAW__2 */

#define CSL_FDIF_IRQSTATUS__2_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQSTATUS__2_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQSTATUS__2_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQSTATUS__2_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQSTATUS__2_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQSTATUS__2_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQENABLE_SET__0 */

#define CSL_FDIF_IRQENABLE_SET__0_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQENABLE_SET__0_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQENABLE_SET__0_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQENABLE_SET__0_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQENABLE_SET__0_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQENABLE_SET__0_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQENABLE_SET__1 */

#define CSL_FDIF_IRQENABLE_SET__1_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQENABLE_SET__1_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQENABLE_SET__1_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQENABLE_SET__1_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQENABLE_SET__1_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQENABLE_SET__1_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQENABLE_SET__2 */
#define CSL_FDIF_IRQENABLE_SET__2_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQENABLE_SET__2_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQENABLE_SET__2_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQENABLE_SET__2_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQENABLE_SET__2_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQENABLE_SET__2_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQENABLE_CLR__0 */

#define CSL_FDIF_IRQENABLE_CLR__0_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQENABLE_CLR__0_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQENABLE_CLR__0_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQENABLE_CLR__0_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQENABLE_CLR__0_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQENABLE_CLR__0_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQENABLE_CLR__1 */

#define CSL_FDIF_IRQENABLE_CLR__1_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQENABLE_CLR__1_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQENABLE_CLR__1_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQENABLE_CLR__1_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQENABLE_CLR__1_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQENABLE_CLR__1_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_IRQENABLE_CLR__2 */

#define CSL_FDIF_IRQENABLE_CLR__2_OCP_ERR_IRQ_MASK (0x00000001u)
#define CSL_FDIF_IRQENABLE_CLR__2_OCP_ERR_IRQ_SHIFT (0x00000000u)
#define CSL_FDIF_IRQENABLE_CLR__2_OCP_ERR_IRQ_RESETVAL (0x00000000u)

#define CSL_FDIF_IRQENABLE_CLR__2_FINISH_IRQ_MASK (0x00000100u)
#define CSL_FDIF_IRQENABLE_CLR__2_FINISH_IRQ_SHIFT (0x00000008u)
#define CSL_FDIF_IRQENABLE_CLR__2_FINISH_IRQ_RESETVAL (0x00000000u)

    /* FDIF_PICADDR */
#define CSL_FDIF_PICADDR_MASK (0xFFFFFFE0u)
#define CSL_FDIF_PICADDR_SHIFT (0x00000000u)
#define CSL_FDIF_PICADDR_RESETVAL (0x00000000u)

    /* FDIF_CTRL */

#define CSL_FDIF_CTRL_OCP_WRNP_MASK (0x00000001u)
#define CSL_FDIF_CTRL_OCP_WRNP_SHIFT (0x00000000u)
#define CSL_FDIF_CTRL_OCP_WRNP_RESETVAL (0x00000000u)

#define CSL_FDIF_CTRL_OCP_MAX_TAGS_MASK (0x0000001Eu)
#define CSL_FDIF_CTRL_OCP_MAX_TAGS_SHIFT (0x00000001u)
#define CSL_FDIF_CTRL_OCP_MAX_TAGS_RESETVAL (0x0000000Fu)

#define CSL_FDIF_CTRL_MSTANDBY_MASK (0x00000020u)
#define CSL_FDIF_CTRL_MSTANDBY_SHIFT (0x00000005u)
#define CSL_FDIF_CTRL_MSTANDBY_RESETVAL (0x00000001u)

#define CSL_FDIF_CTRL_MSTANDBY_HDSHK_MASK (0x00000040u)
#define CSL_FDIF_CTRL_MSTANDBY_HDSHK_SHIFT (0x00000006u)
#define CSL_FDIF_CTRL_MSTANDBY_HDSHK_RESETVAL (0x00000000u)

    /* FDIF_WKADDR */
#define CSL_FDIF_WKADDR_MASK (0xFFFFFFE0u)
#define CSL_FDIF_WKADDR_SHIFT (0x00000000u)
#define CSL_FDIF_WKADDR_RESETVAL (0x00000000u)

    /* FDIF_TESTMON */

#define CSL_FDIF_TESTMON_MASK (0x00001FFFu)
#define CSL_FDIF_TESTMON_SHIFT (0x00000000u)
#define CSL_FDIF_TESTMON_RESETVAL (0x00001000u)

    /* FD_CTRL */
#define CSL_FD_CTRL_FINISH_MASK (0x00000004u)
#define CSL_FD_CTRL_FINISH_SHIFT (0x00000002u)
#define CSL_FD_CTRL_FINISH_RESETVAL (0x00000000u)

    /*----FINISH Tokens----*/

#define CSL_FD_CTRL_RUN_MASK (0x00000002u)
#define CSL_FD_CTRL_RUN_SHIFT (0x00000001u)
#define CSL_FD_CTRL_RUN_RESETVAL (0x00000000u)

    /*----RUN Tokens----*/

#define CSL_FD_CTRL_SRST_MASK (0x00000001u)
#define CSL_FD_CTRL_SRST_SHIFT (0x00000000u)
#define CSL_FD_CTRL_SRST_RESETVAL (0x00000000u)
    /*----SRST Tokens----*/

    /* FD_DNUM */
#define CSL_FD_DNUM_MASK  (0x0000003Fu)
#define CSL_FD_DNUM_SHIFT (0x00000000u)

    /* FD_DCOND */

#define CSL_FD_DCOND_DIR_MASK  (0x0000000Cu)
#define CSL_FD_DCOND_DIR_SHIFT (0x00000002u)
#define CSL_FD_DCOND_DIR_RESETVAL (0x00000000u)

    /*----DIR Tokens----*/
#define CSL_FD_DIR_MODE_UP (0x00000000u)
#define CSL_FD_DIR_MODE_RIGHT (0x00000001u)
#define CSL_FD_DIR_MODE_LEFT (0x00000002u)

#define CSL_FD_DCOND_MIN_MASK  (0x00000003u)
#define CSL_FD_DCOND_MIN_SHIFT (0x00000000u)
#define CSL_FD_DCOND_MIN_RESETVAL (0x00000000u)

    /*----MIN Tokens----*/
#define CSL_FD_MIN_SIZE_20 (0x00000000u)
#define CSL_FD_MIN_SIZE_25 (0x00000001u)
#define CSL_FD_MIN_SIZE_32 (0x00000002u)
#define CSL_FD_MIN_SIZE_40 (0x00000003u)

    /* FD_STARTX */
#define CSL_FD_STARTX_MASK  (0x000000FFu)
#define CSL_FD_STARTX_SHIFT (0x00000000u)
#define CSL_FD_STARTX_RESETVAL (0x00000000u)

    /* FD_STARTY */
#define CSL_FD_STARTY_MASK  (0x0000007Fu)
#define CSL_FD_STARTY_SHIFT (0x00000000u)
#define CSL_FD_STARTY_RESETVAL (0x00000000u)

    /* FD_SIZEX */
#define CSL_FD_SIZEX_MASK  (0x000001FFu)
#define CSL_FD_SIZEX_SHIFT (0x00000000u)
#define CSL_FD_SIZEX_RESETVAL (0x00000140u)

    /* FD_SIZEY */
#define CSL_FD_SIZEY_MASK  (0x000000FFu)
#define CSL_FD_SIZEY_SHIFT (0x00000000u)
#define CSL_FD_SIZEY_RESETVAL (0x000000F0u)

    /* FD_LHIT */
#define CSL_FD_LHIT_MASK  (0x0000000Fu)
#define CSL_FD_LHIT_SHIFT (0x00000000u)
#define CSL_FD_LHIT_RESETVAL (0x00000005u)

    /* FD_CENTERX */
#define CSL_FD_CENTERX_MASK  (0x000001FFu)
#define CSL_FD_CENTERX_SHIFT (0x00000000u)

    /* FD_CENTERY */
#define CSL_FD_CENTERY_MASK  (0x000000FFu)
#define CSL_FD_CENTERY_SHIFT (0x00000000u)

    /* FD_CONFSIZE */

#define CSL_FD_CONFSIZE_CONF_MASK  (0x00000F00u)
#define CSL_FD_CONFSIZE_CONF_SHIFT (0x00000008u)
    /*----CONF Tokens----*/

#define CSL_FD_CONFSIZE_SIZE_MASK  (0x000000FFu)
#define CSL_FD_CONFSIZE_SIZE_SHIFT (0x00000000u)
    /*----SIZE0 Tokens----*/

    /* FD_ANGLE */
#define CSL_FD_ANGLE_MASK  (0x000001FFu)
#define CSL_FD_ANGLE_SHIFT (0x00000000u)

#ifdef __cplusplus
}
#endif
#endif
