/** ==================================================================
 *  @file   debug.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/core/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ========================================================================== 
 * Filename: general.h Description: This file contains common macros,
 * constant and type definitions.  Author: Pavel Nedev
 * ========================================================================== 
 */

#ifndef __GENERAL_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __GENERAL_H__

/* ========================================================================== 
 * TYPE DEFINITIONS
 * ========================================================================== 
 */
/* -------------------------------------------------------------------------- 
 * 32-bit signed integer types
 * -------------------------------------------------------------------------- 
 */
typedef signed int s32;

typedef volatile signed int vs32;

/* -------------------------------------------------------------------------- 
 * 32-bit unsigned integer types
 * -------------------------------------------------------------------------- 
 */
typedef unsigned int u32;

typedef volatile unsigned int vu32;

/* -------------------------------------------------------------------------- 
 * 16-bit signed integer types
 * -------------------------------------------------------------------------- 
 */
typedef signed short s16;

typedef volatile signed short vs16;

/* -------------------------------------------------------------------------- 
 * 16-bit unsigned integer types
 * -------------------------------------------------------------------------- 
 */
typedef unsigned short u16;

typedef volatile unsigned short vu16;

/* -------------------------------------------------------------------------- 
 * 8-bit signed integer types
 * -------------------------------------------------------------------------- 
 */
typedef signed char s8;

typedef volatile signed char vs8;

/* -------------------------------------------------------------------------- 
 * 8-bit unsigned integer types
 * -------------------------------------------------------------------------- 
 */
typedef unsigned char u8;

typedef volatile unsigned char vu8;

/* ========================================================================== 
 * CONSTANT DEFINITIONS
 * ========================================================================== 
 */
#ifndef NULL
#define NULL    (void*)0
#endif

/* ========================================================================== 
 * 32-BIT REGISTER OPERATION MACROS
 * ========================================================================== 
 */
/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_RD(addr)  This macro grants 32-bit register R/W access. Mainly used
 *                 to read register.
 *
 * @param  addr  Register address
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_RD(addr)    (*((vu32*)(addr)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_WR(addr, value)  This macro writes 32-bit register.
 *
 * @param  addr   Register address
 *
 * @param  value  Value to be written
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_WR(addr, value)    (REG32_RD(addr) = (u32)(value))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_TEST_BIT(addr, bit)  This macro tests 32-bit register bit value.
 *                            Masks all other bits in 32 bit register except
 *                            the tested one.
 *
 * @param  addr  Register address
 *
 * @param  bit   Tested bit position (range: 0 - 31)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_TEST_BIT(addr, bit)    (REG32_RD(addr) & ((u32)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_SET_BIT(addr, bit)  This macro sets bit in 32-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be set (range: 0 - 31)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_SET_BIT(addr, bit)    (REG32_RD(addr) |= ((u32)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_CLR_BIT(addr, bit)  This macro clears bit in 32-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be cleared (range: 0 - 31)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_CLR_BIT(addr, bit)    (REG32_RD(addr) &= ~((u32)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_TGL_BIT(addr, bit)  This macro toggles bit in 32-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be toggled (range: 0 - 31)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_TGL_BIT(addr, bit)    (REG32_RD(addr) ^= ((u32)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_FIELD_MASK(pos, width)  This macro generates 32-bit field mask.
 *
 * @param  pos    Lowest position of the field (range: 0 - 31)
 *
 * @param  width  Field width (range: 1 - 31)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_FIELD_MASK(pos, width)    ((((u32)1 << (width)) - 1) << (pos))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_RD_FIELD(addr, pos, width)  This macro reads 32-bit register field
 *                                   value.
 *
 * @param  addr   Register address
 *
 * @param  pos    Lowest position of the field (range: 0 - 31)
 *
 * @param  width  Field width (range: 1 - 31)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_RD_FIELD(addr, pos, width) \
                    ((REG32_RD(addr) & REG32_FIELD_MASK(pos, width)) >> (pos))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG32_WR_FIELD(addr, pos, width, value)  This macro writes 32-bit register
 *                                          field value.
 *
 * @param  addr   Register address
 *
 * @param  pos    Lowest position of the field (range: 0 - 31)
 *
 * @param  width  Field width (range: 1 - 31)
 *
 * @param  value  Field value
*/
/* -------------------------------------------------------------------------- 
 */
#define REG32_WR_FIELD(addr, pos, width, value) \
                    (REG32_RD(addr) = (REG32_RD(addr) & \
                     ~REG32_FIELD_MASK(pos, width)) | ((u32)(value) << (pos)))

/* ========================================================================== 
 * 16-BIT REGISTER OPERATION MACROS
 * ========================================================================== 
 */
/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_RD(addr)  This macro grants 16-bit register R/W access. Mainly used
 *                 to read register.
 *
 * @param  addr  Register address
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_RD(addr)    (*((vu16*)(addr)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_WR(addr, value)  This macro writes 16-bit register.
 *
 * @param  addr   Register address
 *
 * @param  value  Value to be written
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_WR(addr, value)    (REG16_RD(addr) = (u16)(value))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_TEST_BIT(addr, bit)  This macro tests 16-bit register bit value.
 *                            Masks all other bits in 16 bit register except
 *                            the tested one.
 *
 * @param  addr  Register address
 *
 * @param  bit   Tested bit position (range: 0 - 15)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_TEST_BIT(addr, bit)    (REG16_RD(addr) & ((u16)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_SET_BIT(addr, bit)  This macro sets bit in 16-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be set (range: 0 - 15)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_SET_BIT(addr, bit)    (REG16_RD(addr) |= ((u16)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_CLR_BIT(addr, bit)  This macro clears bit in 16-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be cleared (range: 0 - 15)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_CLR_BIT(addr, bit)    (REG16_RD(addr) &= ~((u16)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_TGL_BIT(addr, bit)  This macro toggles bit in 16-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be toggled (range: 0 - 15)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_TGL_BIT(addr, bit)    (REG16_RD(addr) ^= ((u16)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_FIELD_MASK(pos, width)  This macro generates 16-bit field mask.
 *
 * @param  pos    Lowest position of the field (range: 0 - 15)
 *
 * @param  width  Field width (range: 1 - 15)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_FIELD_MASK(pos, width)    ((((u16)1 << (width)) - 1) << (pos))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_RD_FIELD(addr, pos, width)  This macro reads 16-bit register field
 *                                   value.
 *
 * @param  addr   Register address
 *
 * @param  pos    Lowest position of the field (range: 0 - 15)
 *
 * @param  width  Field width (range: 1 - 15)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_RD_FIELD(addr, pos, width) \
                    ((REG16_RD(addr) & REG16_FIELD_MASK(pos, width)) >> (pos))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG16_WR_FIELD(addr, pos, width, value)  This macro writes 16-bit register
 *                                          field value.
 *
 * @param  addr   Register address
 *
 * @param  pos    Lowest position of the field (range: 0 - 15)
 *
 * @param  width  Field width (range: 1 - 15)
 *
 * @param  value  Field value
*/
/* -------------------------------------------------------------------------- 
 */
#define REG16_WR_FIELD(addr, pos, width, value) \
                    (REG16_RD(addr) = (REG16_RD(addr) & \
                     ~REG16_FIELD_MASK(pos, width)) | ((u16)(value) << (pos)))

/* ========================================================================== 
 * 8-BIT REGISTER OPERATION MACROS
 * ========================================================================== 
 */
/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_RD(addr)  This macro grants 8-bit register R/W access. Mainly used
 *                to read register.
 *
 * @param  addr  Register address
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_RD(addr)    (*((vu8*)(addr)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_WR(addr, value)  This macro writes 8-bit register.
 *
 * @param  addr   Register address
 *
 * @param  value  Value to be written
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_WR(addr, value)    (REG8_RD(addr) = (u8)(value))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_TEST_BIT(addr, bit)  This macro tests 8-bit register bit value.
 *                            Masks all other bits in 8 bit register except
 *                            the tested one.
 *
 * @param  addr  Register address
 *
 * @param  bit   Tested bit position (range: 0 - 7)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_TEST_BIT(addr, bit)    (REG8_RD(addr) & ((u8)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_SET_BIT(addr, bit)  This macro sets bit in 8-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be set (range: 0 - 7)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_SET_BIT(addr, bit)    (REG8_RD(addr) |= ((u8)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_CLR_BIT(addr, bit)  This macro clears bit in 8-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be cleared (range: 0 - 7)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_CLR_BIT(addr, bit)    (REG8_RD(addr) &= ~((u8)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_TGL_BIT(addr, bit)  This macro toggles bit in 8-bit register.
 *
 * @param  addr  Register address
 *
 * @param  bit   Bit position to be toggled (range: 0 - 7)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_TGL_BIT(addr, bit)    (REG8_RD(addr) ^= ((u8)1 << (bit)))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_FIELD_MASK(pos, width)  This macro generates 8-bit field mask.
 *
 * @param  pos    Lowest position of the field (range: 0 - 7)
 *
 * @param  width  Field width (range: 1 - 7)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_FIELD_MASK(pos, width)    ((((u8)1 << (width)) - 1) << (pos))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_RD_FIELD(addr, pos, width)  This macro reads 8-bit register field
 *                                   value.
 *
 * @param  addr   Register address
 *
 * @param  pos    Lowest position of the field (range: 0 - 7)
 *
 * @param  width  Field width (range: 1 - 7)
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_RD_FIELD(addr, pos, width) \
                      ((REG8_RD(addr) & REG8_FIELD_MASK(pos, width)) >> (pos))

/* -------------------------------------------------------------------------- 
 */
/**
 * REG8_WR_FIELD(addr, pos, width, value)  This macro writes 8-bit register
 *                                          field value.
 *
 * @param  addr   Register address
 *
 * @param  pos    Lowest position of the field (range: 0 - 7)
 *
 * @param  width  Field width (range: 1 - 7)
 *
 * @param  value  Field value
*/
/* -------------------------------------------------------------------------- 
 */
#define REG8_WR_FIELD(addr, pos, width, value) \
                      (REG8_RD(addr) = (REG8_RD(addr) & \
                       ~REG8_FIELD_MASK(pos, width)) | ((u8)(value) << (pos)))

/* ========================================================================== 
 * OTHER MACROS
 * ========================================================================== 
 */
/* -------------------------------------------------------------------------- 
 */
/**
 * MIN(x, y)   This macro picks the smaller of two numbers.
 *
 * @param  x   First number
 *
 * @param  y   Second number
*/
/* -------------------------------------------------------------------------- 
 */
#define MIN(x, y)    (((x) < (y)) ? (x) : (y))

/* -------------------------------------------------------------------------- 
 */
/**
 * MAX(x, y)   This macro picks the bigger of two numbers.
 *
 * @param  x   First number
 *
 * @param  y   Second number
*/
/* -------------------------------------------------------------------------- 
 */
#define MAX(x, y)    (((x) > (y)) ? (x) : (y))

/* -------------------------------------------------------------------------- 
 */
/**
 * CEIL(x, y)  This macro divides two numbers rounding up the result.
 *
 * @param  x   First number
 *
 * @param  y   Second number
*/
/* -------------------------------------------------------------------------- 
 */
#define CEIL(x, y)    ((((x) % (y)) == 0) ? ((x) / (y)) : (((x) / (y)) + 1))

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* __GENERAL_H__ */
