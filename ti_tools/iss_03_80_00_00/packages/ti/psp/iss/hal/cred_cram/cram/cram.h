/** ==================================================================
 *  @file   cram.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/cred_cram/cram/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ============================================================================= 
 * TEXAS INSTRUMENTS INCORPORATED PROPRIETARY INFORMATION Property of Texas
 * Instruments For Unrestricted Internal Use Only Unauthorized reproduction
 * and/or distribution is strictly prohibited.  This product is protected
 * under copyright law and trade secret law as an unpublished work.  Created 
 * 2008, (C) Copyright 2008 Texas Instruments.  All rights reserved. */
/**
 *  @Component    CRAM   
 *  @Filename     cram.h
 *  @Description  C Register Access Macros - API
 *
    *//*======================================================================== */

#ifndef __CRAM_H
#define __CRAM_H

#ifdef __cplusplus
extern "C" {
#endif

    /* =============================================================================
     * EXPORTED DEFINITIONS
     * ============================================================================= */

    /* ============================================================= Read
     * memory volatile
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_MEM_8_VOLATILE
 *
 * @BRIEF      Read memory volatile - 8bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef RD_MEM_8_VOLATILE
#define RD_MEM_8_VOLATILE(addr)     ( (UWORD8)( *((REG_UWORD8 *)(addr)) ) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_MEM_16_VOLATILE
 *
 * @BRIEF      Read memory volatile - 16bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef RD_MEM_16_VOLATILE
#define RD_MEM_16_VOLATILE(addr)     ( (UWORD16)( *((REG_UWORD16 *)(addr)) ) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_MEM_32_VOLATILE
 *
 * @BRIEF      Read memory volatile - 32bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef RD_MEM_32_VOLATILE
#define RD_MEM_32_VOLATILE(addr)     ( (UWORD32)( *((REG_UWORD32 *)(addr)) ) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_MEM_64_VOLATILE
 *
 * @BRIEF      Read memory volatile - 64bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef RD_MEM_64_VOLATILE
#define RD_MEM_64_VOLATILE(addr)     ( (UWORD64)( *((REG_UWORD64 *)(addr)) ) )
#endif

    /* ============================================================= Write
     * memory volatile
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_MEM_8_VOLATILE
 *
 * @BRIEF      Write memory volatile - 8bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef WR_MEM_8_VOLATILE
#define WR_MEM_8_VOLATILE(addr, data) ( *((REG_UWORD8 *)(addr)) = (UWORD8)(data) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_MEM_16_VOLATILE
 *
 * @BRIEF      Write memory volatile - 16bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef WR_MEM_16_VOLATILE
#define WR_MEM_16_VOLATILE(addr, data) ( *((REG_UWORD16 *)(addr)) = (UWORD16)(data) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_MEM_32_VOLATILE
 *
 * @BRIEF      Write memory volatile - 32bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef WR_MEM_32_VOLATILE
#define WR_MEM_32_VOLATILE(addr, data) ( *((REG_UWORD32 *)(addr)) = (UWORD32)(data) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_MEM_64_VOLATILE
 *
 * @BRIEF      Write memory volatile - 64bit; accessing the memory address
 *             This is ok for ARM cores and C64, not ok for C55
    *//*------------------------------------------------------------------------ */
#ifndef WR_MEM_64_VOLATILE
#define WR_MEM_64_VOLATILE(addr, data) ( *((REG_UWORD64 *)(addr)) = (UWORD64)(data) )
#endif

    /* ============================================================= Bits
     * macros ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITS_8 
 *
 * @BRIEF      returns in_NbBits+1 bits to '1' starting from bit 0
    *//*------------------------------------------------------------------------ */
#ifndef BITS_8
#define BITS_8(in_NbBits) \
    ( ( ( (UWORD8)1 << in_NbBits ) - 1 ) | ( (UWORD8)1 << in_NbBits ) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITS_16 
 *
 * @BRIEF      returns in_NbBits+1 bits to '1' starting from bit 0
    *//*------------------------------------------------------------------------ */
#ifndef BITS_16
#define BITS_16(in_NbBits) \
    ( ( ( (UWORD16)1 << in_NbBits ) - 1 ) | ( (UWORD16)1 << in_NbBits ) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITS_32 
 *
 * @BRIEF      returns in_NbBits+1 bits to '1' starting from bit 0
    *//*------------------------------------------------------------------------ */
#ifndef BITS_32
#define BITS_32(in_NbBits) \
    ( ( ( (UWORD32)1 << in_NbBits ) - 1 ) | ( (UWORD32)1 << in_NbBits ) )
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITS_64 
 *
 * @BRIEF      returns in_NbBits+1 bits to '1' starting from bit 0
    *//*------------------------------------------------------------------------ */
#ifndef BITS_64
#define BITS_64(in_NbBits) \
    ( ( ( (UWORD64)1 << in_NbBits ) - 1 ) | ( (UWORD64)1 << in_NbBits ) )
#endif

    /* =============================================================
     * Bit-Fileld macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITFIELD_8 
 *
 * @BRIEF      returns 1s' on bit positions between in_UpBit and in_LowBit
    *//*------------------------------------------------------------------------ */
#ifndef BITFIELD_8
#define BITFIELD_8(in_UpBit, in_LowBit) \
    ( BITS_8(in_UpBit) & ~((BITS_8(in_LowBit)) >> 1 ))
#endif
                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITFIELD_16 
 *
 * @BRIEF      returns 1s' on bit positions between in_UpBit and in_LowBit
    *//*------------------------------------------------------------------------ */
#ifndef BITFIELD_16
#define BITFIELD_16(in_UpBit, in_LowBit) \
    ( BITS_16(in_UpBit) & ~((BITS_16(in_LowBit)) >> 1 ))
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITFIELD_32 
 *
 * @BRIEF      returns 1s' on bit positions between in_UpBit and in_LowBit
    *//*------------------------------------------------------------------------ */
#ifndef BITFIELD_32
#define BITFIELD_32(in_UpBit, in_LowBit) \
    ( BITS_32(in_UpBit) & ~((BITS_32(in_LowBit)) >> 1 ))
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION BITFIELD_64 
 *
 * @BRIEF      returns 1s' on bit positions between in_UpBit and in_LowBit
    *//*------------------------------------------------------------------------ */
#ifndef BITFIELD_64
#define BITFIELD_64(in_UpBit, in_LowBit) \
    ( BITS_64(in_UpBit) & ~((BITS_64(in_LowBit)) >> 1 ))
#endif

    /* ============================================================= Misc
     * macro - Register address
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION REG_ADDR_8
 *
 * @BRIEF      Register address; takes component instance name and register 
 *             offset as parameters
    *//*------------------------------------------------------------------------ */
#ifdef USE_PHYSICAL_ADDRESS
#define REG_ADDR_8(COMP, REG) (COMP##_U_BASE+(REG_UWORD8)(REG))
#else
#define REG_ADDR_8(COMP, REG) (CRED_instVirtAddr[COMP]+(REG_UWORD8)(REG))
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION REG_ADDR_16
 *
 * @BRIEF      Register address; takes component instance name and register 
 *             offset as parameters
    *//*------------------------------------------------------------------------ */
#ifdef USE_PHYSICAL_ADDRESS
#define REG_ADDR_16(COMP, REG) (COMP##_U_BASE+(REG_UWORD16)(REG))
#else
#define REG_ADDR_16(COMP, REG) (CRED_instVirtAddr[COMP]+(REG_UWORD16)(REG))
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION REG_ADDR_32
 *
 * @BRIEF      Register address; takes component instance name and register 
 *             offset as parameters
    *//*------------------------------------------------------------------------ */
#ifdef USE_PHYSICAL_ADDRESS
#define REG_ADDR_32(COMP, REG) (COMP##_U_BASE+(REG_UWORD32)(REG))
#else
#define REG_ADDR_32(COMP, REG) (CRED_instVirtAddr[COMP]+(REG_UWORD32)(REG))
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION REG_ADDR_64
 *
 * @BRIEF      Register address; takes component instance name and register 
 *             offset as parameters
    *//*------------------------------------------------------------------------ */
#ifdef USE_PHYSICAL_ADDRESS
#define REG_ADDR_64(COMP, REG) (COMP##_U_BASE+(REG))
#else
#define REG_ADDR_64(COMP, REG) (CRED_instVirtAddr[COMP]+(REG))
#endif

    /* ============================================================= Register 
     * Read macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_8 
 *
 * @BRIEF      Read register; takes component instance name and register offset
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_REG_8(COMP, REG) \
  RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, REG))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_16 
 *
 * @BRIEF      Read register; takes component instance name and register offset
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_REG_16(COMP, REG) \
  RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, REG))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_32 
 *
 * @BRIEF      Read register; takes component instance name and register offset
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_REG_32(COMP, REG) \
  RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, REG))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_64 
 *
 * @BRIEF      Read register; takes component instance name and register offset
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_REG_64(COMP, REG) \
  RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, REG))

    /* ============================================================= Register 
     * Write macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_8 
 *
 * @BRIEF      Write value to register; takes component instance name, 
 *             register offset and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_REG_8(COMP, REG, VAL) \
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, REG), (UWORD8)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_16 
 *
 * @BRIEF      Write value to register; takes component instance name, 
 *             register offset and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_REG_16(COMP, REG, VAL) \
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, REG), (UWORD16)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_32 
 *
 * @BRIEF      Write value to register; takes component instance name, 
 *             register offset and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_REG_32(COMP, REG, VAL) \
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, REG), (UWORD32)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_64 
 *
 * @BRIEF      Write value to register; takes component instance name, 
 *             register offset and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_REG_64(COMP, REG, VAL) \
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, REG), (UWORD64)(VAL))

    /* ============================================================= Register 
     * Mask&Write macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION MASKWR_REG_8 
 *
 * @BRIEF      Write masked value to register; takes component address, 
 *             register offset and value to write as parameters.
 *             The macro reads value first and sets only bits matching the mask,
 *             leaving the rest intact.
    *//*------------------------------------------------------------------------ */
#define MASKWR_REG_8(COMP, REG, MASK, VAL) \
  WR_REG_8(COMP, REG, \
    (RD_REG_8(COMP, REG) & (~(MASK))) | ( (UWORD8)(VAL)&(UWORD8)(MASK) ) )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION MASKWR_REG_16 
 *
 * @BRIEF      Write masked value to register; takes component address, 
 *             register offset and value to write as parameters.
 *             The macro reads value first and sets only bits matching the mask,
 *             leaving the rest intact.
    *//*------------------------------------------------------------------------ */
#define MASKWR_REG_16(COMP, REG, MASK, VAL) \
  WR_REG_16(COMP, REG, \
    (RD_REG_16(COMP, REG) & (~(MASK))) | ( (UWORD16)(VAL)&(UWORD16)(MASK) ) )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION MASKWR_REG_32 
 *
 * @BRIEF      Write masked value to register; takes component address, 
 *             register offset and value to write as parameters.
 *             The macro reads value first and sets only bits matching the mask,
 *             leaving the rest intact.
    *//*------------------------------------------------------------------------ */
#define MASKWR_REG_32(COMP, REG, MASK, VAL) \
  WR_REG_32(COMP, REG, \
    (RD_REG_32(COMP, REG) & (~(MASK))) | ( (UWORD32)(VAL)&(UWORD32)(MASK) ) )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION MASKWR_REG_64 
 *
 * @BRIEF      Write masked value to register; takes component address, 
 *             register offset and value to write as parameters.
 *             The macro reads value first and sets only bits matching the mask,
 *             leaving the rest intact.
    *//*------------------------------------------------------------------------ */
#define MASKWR_REG_64(COMP, REG, MASK, VAL) \
  WR_REG_64(COMP, REG, \
    (RD_REG_64(COMP, REG) & (~(MASK))) | ( (UWORD64)(VAL)&(UWORD64)(MASK) ) )

    /* =============================================================
     * Bit-Field Read macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_8  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset
 *             and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_8(COMP, REG, FIELD) \
  ( ( RD_REG_8(COMP, REG) & FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_16  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset
 *             and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_16(COMP, REG, FIELD) \
  ( ( RD_REG_16(COMP, REG) & FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_32  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset
 *             and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_32(COMP, REG, FIELD) \
  ( ( RD_REG_32(COMP, REG) & FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_64  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset
 *             and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_64(COMP, REG, FIELD) \
  ( ( RD_REG_64(COMP, REG) & FIELD ) >> FIELD##__POS )

    /* =============================================================
     * Bit-Field Write macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_8
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_8(COMP, REG, FIELD, VAL) \
  WR_REG_8(COMP, REG, \
    (RD_REG_8(COMP, REG) & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_16
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_16(COMP, REG, FIELD, VAL) \
  WR_REG_16(COMP, REG, \
    (RD_REG_16(COMP, REG) & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_32
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_32(COMP, REG, FIELD, VAL) \
  WR_REG_32(COMP, REG, \
    (RD_REG_32(COMP, REG) & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_64
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_64(COMP, REG, FIELD, VAL) \
  WR_REG_64(COMP, REG, \
    (RD_REG_64(COMP, REG) & (~(FIELD))) | (((UWORD64)(VAL) << FIELD##__POS) & FIELD))

    /* ============================================================= Extended 
     * Bit-Field Read macros taking mask & position
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_EX_8  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset,
 *             bit-field mask and position parameters;
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_EX_8(COMP, REG, FIELD, POS) \
  ( ( RD_REG_8(COMP, REG) & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_EX_16  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset,
 *             bit-field mask and position parameters;
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_EX_16(COMP, REG, FIELD, POS) \
  ( ( RD_REG_16(COMP, REG) & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_EX_32  
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset,
 *             bit-field mask and position parameters;
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_EX_32(COMP, REG, FIELD, POS) \
  ( ( RD_REG_32(COMP, REG) & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_EX_64
 *
 * @BRIEF      Read bit-field; takes component instance name, register offset,
 *             bit-field mask and position parameters;
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_EX_64(COMP, REG, FIELD, POS) \
  ( ( RD_REG_64(COMP, REG) & (FIELD) ) >> (POS))

    /* ============================================================= Extended 
     * Bit-Field Write macros taking mask & position
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_EX_8
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field mask, position and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_EX_8(COMP, REG, FIELD, POS, VAL)	\
  WR_REG_8(COMP, REG, \
    (RD_REG_8(COMP, REG) & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_EX_16
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field mask, position and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_EX_16(COMP, REG, FIELD, POS, VAL)	\
  WR_REG_16(COMP, REG, \
    (RD_REG_16(COMP, REG) & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_EX_32
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field mask, position and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_EX_32(COMP, REG, FIELD, POS, VAL)	\
  WR_REG_32(COMP, REG, \
    (RD_REG_32(COMP, REG) & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_EX_64
 *
 * @BRIEF      Write bit-field; takes component instance name, register offset,
 *             bit-field mask, position and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_EX_64(COMP, REG, FIELD, POS, VAL)	\
  WR_REG_64(COMP, REG, \
    (RD_REG_64(COMP, REG) & (~(FIELD))) | (((UWORD64)(VAL) << (POS)) & (FIELD)))

    /* =============================================================
     * Bit-Field Mask&Set macro
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION MASKSET_FIELD
 *
 * @BRIEF      Set bit-field value to variable; takes the variable, bit-field
 *             and value to set as parameters. The operation modifies only bits
 *             matching the bit-filed.
    *//*------------------------------------------------------------------------ */
#define MASKSET_FIELD(VAR, FIELD, VAL) \
  ( VAR = ( (VAR & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD) ) )

    /* ============================================================= Extended 
     * Bit-Field Mask&Set macro taking mask & position
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION MASKSET_FIELD_EX
 *
 * @BRIEF      Set bit-field value to variable; takes the variable, bit-field
 *             mask, position and value to set as parameters. 
 *             The operation modifies only bits matching the bit-filed.
    *//*------------------------------------------------------------------------ */
#define MASKSET_FIELD_EX(VAR, FIELD, POS, VAL) \
  ( VAR = ( (VAR & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)) ) )

    /* =============================================================
     * Bit-field Set (Shift & Mask) and Get (Mask & Shift)
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION GET_FIELD 
 *
 * @BRIEF      Get bit-field from variable; takes the variable and bit-field 
 *             as parameters; the whole input variable gets modified.
 *             This is size independent macro - use proper type for the variable
    *//*------------------------------------------------------------------------ */
#define GET_FIELD(REG_VAR, FIELD)   (( REG_VAR & FIELD ) >> FIELD##__POS)

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION SET_FIELD
 *
 * @BRIEF      Shift and Mask the variable; takes the variable and bit-field
 *             as parameters; the whole input variable gets modified.
 *             This is size independent macro - use proper type for the variable
    *//*------------------------------------------------------------------------ */
#define SET_FIELD(VAR, FIELD)    ((VAR << FIELD##__POS) & FIELD)

    /* ============================================================= Extended 
     * Bit-field Set (Shift & Mask) and Get (Mask & Shift)
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION GET_FIELD_EX 
 *
 * @BRIEF      Get bit-field from variable; takes the variable, bit-field mask
 *             and position as parameters; the whole input variable gets modified.
 *             This is size independent macro - use proper type for the variable
    *//*------------------------------------------------------------------------ */
#define GET_FIELD_EX(REG_VAR, FIELD, POS)   (( REG_VAR & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION SET_FIELD_EX
 *
 * @BRIEF      Mask and Shift the variable; takes the variable, bit-field mask
 *             and position as parameters; the whole input variable gets modified.
 *             This is size independent macro - use proper type for the variable
    *//*------------------------------------------------------------------------ */
#define SET_FIELD_EX(VAR, FIELD, POS)    ((VAR << (POS)) & (FIELD))

    /* ============================================================= Read
     * register in single-register array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_ARRAY_8 
 *
 * @BRIEF      Read one register in a register array; takes component instance
 *             name, register-array name and register index as parameters. 
    *//*------------------------------------------------------------------------ */
#define RD_REG_ARRAY_8(COMP, ARRAY, INDEX) \
  RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_ARRAY_16 
 *
 * @BRIEF      Read one register in a register array; takes component instance
 *             name, register-array name and register index as parameters. 
    *//*------------------------------------------------------------------------ */
#define RD_REG_ARRAY_16(COMP, ARRAY, INDEX) \
  RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_ARRAY_32 
 *
 * @BRIEF      Read one register in a register array; takes component instance
 *             name, register-array name and register index as parameters. 
    *//*------------------------------------------------------------------------ */
#define RD_REG_ARRAY_32(COMP, ARRAY, INDEX) \
  RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_REG_ARRAY_64 
 *
 * @BRIEF      Read one register in a register array; takes component instance
 *             name, register-array name and register index as parameters. 
    *//*------------------------------------------------------------------------ */
#define RD_REG_ARRAY_64(COMP, ARRAY, INDEX) \
  RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))))

    /* ============================================================= Write
     * register in single-register array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_ARRAY_8 
 *
 * @BRIEF      Write value to one register in a register array; takes component 
 *             instance name, register-array name, register index and value
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_REG_ARRAY_8(COMP, ARRAY, INDEX, VAL) \
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
                     (UWORD8)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_ARRAY_16 
 *
 * @BRIEF      Write value to one register in a register array; takes component 
 *             instance name, register-array name, register index and value
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_REG_ARRAY_16(COMP, ARRAY, INDEX, VAL) \
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
                     (UWORD16)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_ARRAY_32 
 *
 * @BRIEF      Write value to one register in a register array; takes component 
 *             instance name, register-array name, register index and value
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_REG_ARRAY_32(COMP, ARRAY, INDEX, VAL) \
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
                     (UWORD32)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_REG_ARRAY_64 
 *
 * @BRIEF      Write value to one register in a register array; takes component 
 *             instance name, register-array name, register index and value 
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_REG_ARRAY_64(COMP, ARRAY, INDEX, VAL) \
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
                     (UWORD64)(VAL))

    /* ============================================================= Read
     * register in register-bundle array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_BUNDLE_ARRAY_8
 *
 * @BRIEF      Read register in a register-bundle array; takes component
 *             instance name, bundle-array name, register name and bundle bank
 *             index as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_BUNDLE_ARRAY_8(COMP, ARRAY, REG, BANK) \
  RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_BUNDLE_ARRAY_16
 *
 * @BRIEF      Read register in a register-bundle array; takes component
 *             instance name, bundle-array name, register name and bundle bank
 *             index as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_BUNDLE_ARRAY_16(COMP, ARRAY, REG, BANK) \
  RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_BUNDLE_ARRAY_32
 *
 * @BRIEF      Read register in a register-bundle array; takes component
 *             instance name, bundle-array name, register name and bundle bank
 *             index as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_BUNDLE_ARRAY_32(COMP, ARRAY, REG, BANK) \
  RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_BUNDLE_ARRAY_64
 *
 * @BRIEF      Read register in a register-bundle array; takes component
 *             instance name, bundle-array name, register name and bundle bank
 *             index as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_BUNDLE_ARRAY_64(COMP, ARRAY, REG, BANK) \
  RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))))

    /* ============================================================= Write
     * register in register-bundle array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_BUNDLE_ARRAY_8
 *
 * @BRIEF      Write value to one register in a bundle array; takes component 
 *             instance name, bundle-array name, register name, bank index,
 *             and value as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_BUNDLE_ARRAY_8(COMP, ARRAY, REG, BANK, VAL)	\
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
                     (UWORD8)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_BUNDLE_ARRAY_16 
 *
 * @BRIEF      Write value to one register in a bundle array; takes component 
 *             instance name, bundle-array name, register name, bank index,
 *             and value as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_BUNDLE_ARRAY_16(COMP, ARRAY, REG, BANK, VAL)	\
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
                     (UWORD16)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_BUNDLE_ARRAY_32 
 *
 * @BRIEF      Write value to one register in a bundle array; takes component 
 *             instance name, bundle-array name, register name, bank index,
 *             and value as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_BUNDLE_ARRAY_32(COMP, ARRAY, REG, BANK, VAL)	\
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
                     (UWORD32)(VAL))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_BUNDLE_ARRAY_64
 *
 * @BRIEF      Write value to one register in a bundle array; takes component 
 *             instance name, bundle-array name, register name, bank index,
 *             and value as parameters.
    *//*------------------------------------------------------------------------ */
#define WR_BUNDLE_ARRAY_64(COMP, ARRAY, REG, BANK, VAL)	\
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
                     (UWORD64)(VAL))

    /* !@# New development for GoldenGate SW team */

    /* =============================================================
     * Bit-Field Read from register in register-array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_8  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index and bit-field
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_8(COMP, ARRAY, INDEX, FIELD) \
  ( ( RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                                          FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_16  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index and bit-field
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_16(COMP, ARRAY, INDEX, FIELD) \
  ( ( RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                                            FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_32  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index and bit-field
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_32(COMP, ARRAY, INDEX, FIELD) \
  ( ( RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                                            FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_64  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index and bit-field
 *             as parameters.
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_64(COMP, ARRAY, INDEX, FIELD) \
  ( ( RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                                            FIELD ) >> FIELD##__POS )

    /* =============================================================
     * Bit-Field Write to register in register-array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_8
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_8(COMP, ARRAY, INDEX, FIELD, VAL) \
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
    (RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                       (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_16
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_16(COMP, ARRAY, INDEX, FIELD, VAL) \
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
    (RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                        (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_32
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_32(COMP, ARRAY, INDEX, FIELD, VAL) \
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
    (RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                        (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_64
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_64(COMP, ARRAY, INDEX, FIELD, VAL) \
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE))), \
    (RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * ARRAY##__ELSIZE)))) & \
                        (~(FIELD))) | (((UWORD64)(VAL) << FIELD##__POS) & FIELD))

    /* ============================================================= Extended 
     * Bit-Field Read from register in register-array taking mask & position
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_EX_8  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position as parameters.
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_EX_8(COMP, ARRAY, INDEX, FIELD, POS) \
  ( ( RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * 1)))) & \
      (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_EX_16  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position as parameters.
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_EX_16(COMP, ARRAY, INDEX, FIELD, POS) \
  ( ( RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * 2)))) & \
      (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_EX_32  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position as parameters.
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_EX_32(COMP, ARRAY, INDEX, FIELD, POS) \
  ( ( RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * 4)))) & \
      (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_ARRAY_EX_64  
 *
 * @BRIEF      Read bit-field from register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position as parameters.
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_ARRAY_EX_64(COMP, ARRAY, INDEX, FIELD, POS) \
  ( ( RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * 8)))) & \
      (FIELD) ) >> (POS))

    /* ============================================================= Extended 
     * Bit-Field Write to register in register-array taking mask & position
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_EX_8
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position and value to write as parameters
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_EX_8(COMP, ARRAY, INDEX, FIELD, POS, VAL)	\
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * 1))), \
    (RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + (INDEX * 1)))) & \
     (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_EX_16
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position and value to write as parameters
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_EX_16(COMP, ARRAY, INDEX, FIELD, POS, VAL)	\
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * 2))), \
    (RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + (INDEX * 2)))) & \
     (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_EX_32
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position and value to write as parameters
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_EX_32(COMP, ARRAY, INDEX, FIELD, POS, VAL)	\
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * 4))), \
    (RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + (INDEX * 4)))) & \
     (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_ARRAY_EX_64
 *
 * @BRIEF      Write bit-field to register in register array; takes component
 *             instance name, register-array name, register index, bit-field
 *             mask and position and value to write as parameters
 *             The element size is hardcoded - as it is defined by the array
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_ARRAY_EX_64(COMP, ARRAY, INDEX, FIELD, POS, VAL)	\
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * 8))), \
    (RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + (INDEX * 8)))) & \
     (~(FIELD))) | (((UWORD64)(VAL) << (POS)) & (FIELD)))

    /* =============================================================
     * Bit-Field Read from register in register-bundle-array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_8 
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_8(COMP, ARRAY, REG, BANK, FIELD) \
  ( ( RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REG##__OFFSET + \
                         (BANK * ARRAY##__ELSIZE)))) & FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_16  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_16(COMP, ARRAY, REG, BANK, FIELD) \
  ( ( RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REG##__OFFSET + \
                          (BANK * ARRAY##__ELSIZE)))) & FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_32  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_32(COMP, ARRAY, REG, BANK, FIELD) \
  ( ( RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REG##__OFFSET + \
                          (BANK * ARRAY##__ELSIZE)))) & FIELD ) >> FIELD##__POS )

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_64  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank and bit-field as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_64(COMP, ARRAY, REG, BANK, FIELD) \
  ( ( RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REG##__OFFSET + \
                          (BANK * ARRAY##__ELSIZE)))) & FIELD ) >> FIELD##__POS )

    /* =============================================================
     * Bit-Field Write to register in register-bundle-array macros
     * ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_8
 *
 * @BRIEF      Write bit-field to one register in a bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank, bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_8(COMP, ARRAY, REG, BANK, FIELD, VAL) \
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
    (RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REG##__OFFSET + \
           (BANK * ARRAY##__ELSIZE)))) & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_16
 *
 * @BRIEF      Write bit-field to one register in a bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank, bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_16(COMP, ARRAY, REG, BANK, FIELD, VAL) \
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
    (RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REG##__OFFSET + \
             (BANK * ARRAY##__ELSIZE)))) & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_32
 *
 * @BRIEF      Write bit-field to one register in a bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank, bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_32(COMP, ARRAY, REG, BANK, FIELD, VAL) \
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
    (RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REG##__OFFSET + \
             (BANK * ARRAY##__ELSIZE)))) & (~(FIELD))) | (((VAL) << FIELD##__POS) & FIELD))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_64
 *
 * @BRIEF      Write bit-field to one register in a bundle array;
 *             takes component instance name, bundle-array name, register name,
 *             bundle bank, bit-field and value to write as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_64(COMP, ARRAY, REG, BANK, FIELD, VAL) \
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REG##__OFFSET + (BANK * ARRAY##__ELSIZE))),\
    (RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REG##__OFFSET + \
             (BANK * ARRAY##__ELSIZE)))) & (~(FIELD))) | (((UWORD64)(VAL) << FIELD##__POS) & FIELD))

    /* ============================================================= Extended 
     * Bit-Field Read from register in register-bundle-array taking mask &
     * position ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_EX_8  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register offset,
 *             bundle bank index, bank size, bit-field mask and position
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_EX_8(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS) \
  ( ( RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REGOFF + \
                               (BANK * BSIZE)))) & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_EX_16  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register offset,
 *             bundle bank index, bank size, bit-field mask and position
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_EX_16(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS) \
  ( ( RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REGOFF + \
                               (BANK * BSIZE)))) & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_EX_32  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register offset,
 *             bundle bank index, bank size, bit-field mask and position
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_EX_32(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS) \
  ( ( RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REGOFF + \
                               (BANK * BSIZE)))) & (FIELD) ) >> (POS))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION RD_FIELD_BUNDLE_ARRAY_EX_64  
 *
 * @BRIEF      Read bit-field from register in a register-bundle array;
 *             takes component instance name, bundle-array name, register offset,
 *             bundle bank index, bank size, bit-field mask and position
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define RD_FIELD_BUNDLE_ARRAY_EX_64(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS) \
  ( ( RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REGOFF + \
                               (BANK * BSIZE)))) & (FIELD) ) >> (POS))

    /* ============================================================= Extended 
     * Bit-Field Write to register in register-bundle-array taking mask &
     * position ============================================================= */

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_EX_8
 *
 * @BRIEF      Write bit-field to one register in a bundle array; takes component
 *             instance name, bundle-array name, register offset, bundle bank
 *             index, bank size, bit-field mask and position and value to write
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_EX_8(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS, VAL) \
  WR_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REGOFF + (BANK * BSIZE))),\
    (RD_MEM_8_VOLATILE(REG_ADDR_8(COMP, (ARRAY + REGOFF + \
                (BANK * BSIZE)))) & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_EX_16
 *
 * @BRIEF      Write bit-field to one register in a bundle array; takes component
 *             instance name, bundle-array name, register offset, bundle bank
 *             index, bank size, bit-field mask and position and value to write
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_EX_16(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS, VAL)	\
  WR_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REGOFF + (BANK * BSIZE))),\
    (RD_MEM_16_VOLATILE(REG_ADDR_16(COMP, (ARRAY + REGOFF + \
                  (BANK * BSIZE)))) & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_EX_32
 *
 * @BRIEF      Write bit-field to one register in a bundle array; takes component
 *             instance name, bundle-array name, register offset, bundle bank
 *             index, bank size, bit-field mask and position and value to write
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_EX_32(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS, VAL)	\
  WR_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REGOFF + (BANK * BSIZE))),\
    (RD_MEM_32_VOLATILE(REG_ADDR_32(COMP, (ARRAY + REGOFF + \
                  (BANK * BSIZE)))) & (~(FIELD))) | (((VAL) << (POS)) & (FIELD)))

                                                                             /*-------------------------------------------------------------------------*//**
 * @DEFINITION WR_FIELD_BUNDLE_ARRAY_EX_64
 *
 * @BRIEF      Write bit-field to one register in a bundle array; takes component
 *             instance name, bundle-array name, register offset, bundle bank
 *             index, bank size, bit-field mask and position and value to write
 *             as parameters
    *//*------------------------------------------------------------------------ */
#define WR_FIELD_BUNDLE_ARRAY_EX_64(COMP, ARRAY, REGOFF, BANK, BSIZE, FIELD, POS, VAL)	\
  WR_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REGOFF + (BANK * BSIZE))),\
    (RD_MEM_64_VOLATILE(REG_ADDR_64(COMP, (ARRAY + REGOFF + \
                  (BANK * BSIZE)))) & (~(FIELD))) | (((UWORD64)(VAL) << (POS)) & (FIELD)))

    /* =============================================================================
     * EXPORTED TYPES
     * ============================================================================= */

    /* 
     * Types below usually comes from header like GlobalTypes.h or similar
     */

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  UWORD8 
 *
 * @BRIEF unsigned byte type
    *//*------------------------------------------------------------------------ */
#ifndef UWORD8_FLAG
#define UWORD8_FLAG
    typedef unsigned char UWORD8;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  UWORD16
 *
 * @BRIEF unsigned 2-byte type
    *//*------------------------------------------------------------------------ */
#ifndef UWORD16_FLAG
#define UWORD16_FLAG
    typedef unsigned short UWORD16;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  UWORD32
 *
 * @BRIEF unsigned 4-byte type
    *//*------------------------------------------------------------------------ */
#ifndef UWORD32_FLAG
#define UWORD32_FLAG
    typedef unsigned int UWORD32;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  UWORD64
 *
 * @BRIEF unsigned 8-byte type
    *//*------------------------------------------------------------------------ */
#ifndef UWORD64_FLAG
#define UWORD64_FLAG
    typedef unsigned long long UWORD64;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  REG_UWORD8
 *
 * @BRIEF volatile unsigned byte type
    *//*------------------------------------------------------------------------ */
#ifndef REG_UWORD8_FLAG
#define REG_UWORD8_FLAG
    typedef volatile unsigned char REG_UWORD8;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  REG_UWORD16
 *
 * @BRIEF volatile unsigned 2-byte type
    *//*------------------------------------------------------------------------ */
#ifndef REG_UWORD16_FLAG
#define REG_UWORD16_FLAG
    typedef volatile unsigned short REG_UWORD16;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  REG_UWORD32
 *
 * @BRIEF volatile unsigned 4-byte type
    *//*------------------------------------------------------------------------ */
#ifndef REG_UWORD32_FLAG
#define REG_UWORD32_FLAG
    typedef volatile unsigned int REG_UWORD32;
#endif

                                                                             /*-------------------------------------------------------------------------*//**
 * @TYPE  REG_UWORD64
 *
 * @BRIEF volatile unsigned 8-byte type
    *//*------------------------------------------------------------------------ */
#ifndef REG_UWORD64_FLAG
#define REG_UWORD64_FLAG
    typedef volatile unsigned long long REG_UWORD64;
#endif

#ifdef __cplusplus
}
#endif
#endif                                                     /* #ifndef
                                                            * __CRAM_H */
