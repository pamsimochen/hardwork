/*
* test_arm_reloc.c
*
* ARM Relocation Unit Tests
*
* Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the
* distribution.
*
* Neither the name of Texas Instruments Incorporated nor the names of
* its contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "test_arm_reloc.h"
#include <stdlib.h>
#include <stdio.h>


/*****************************************************************************/
/* ARM_TestRelocDo                                                           */
/* Tests the ARM version of reloc_do.  In cases where multiple relocation    */
/* types are implemented in the same way, only one type is tested.  For      */
/* instance, R_ARM_PC24, R_ARM_CALL, and R_ARM_PLT32 are implemented in the  */
/* exact same way and so only R_ARM_PC24 is tested.                          */
/*                                                                           */
/* Each test follows the same flow:                                          */
/* 1. A valid instruction is constructed for the relocation type being       */
/*    tested.                                                                */
/* 2. Addend, symbol value, and pc are then created.                         */
/*    (NOTE: static base is not needed, and so 0 is passed. Also, same       */
/*     endianness is assumed.)                                               */
/* 3. reloc_do() is called                                                   */
/* 4. The result is checked.                                                 */
/* 5. Repeat if variations should be considered.                             */
/*****************************************************************************/

void ARM_TestRelocDo::test_R_ARM_PC24()
{
    uint32_t address_space = 0xEBFFFFFF;
    uint32_t addend = 0x4;
    uint32_t symval = 0x2001000;
    uint32_t pc = 0x2000000;

    /* BL */
    unit_arm_reloc_do(R_ARM_PC24,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xEB000401);

    /* BLX (ARM -> THUMB) H bit set to 0 */
    symval++;
    unit_arm_reloc_do(R_ARM_PC24,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xFA000401);

    /* BLX (ARM -> THUMB) H bit set to 1 */

    addend = 0x6;
    unit_arm_reloc_do(R_ARM_PC24,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xFB000401);
}

void ARM_TestRelocDo::test_R_ARM_JUMP24()
{
    uint32_t address_space = 0xEA000000;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x2001000;
            
    unit_arm_reloc_do(R_ARM_JUMP24,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xEA000401);
}

void ARM_TestRelocDo::test_R_ARM_ABS32()
{
    uint32_t address_space = 0x0;
    uint32_t addend = 0x4;
    uint32_t symval = 0x2001000;
    uint32_t pc = 0x0;
            
    unit_arm_reloc_do(R_ARM_ABS32,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x2001004);
}

void ARM_TestRelocDo::test_R_ARM_THM_CALL()
{
    uint32_t address_space;
    uint16_t* ins1 = (uint16_t*) &address_space;
    uint16_t* ins2 = ins1 + 1;

    /* THUMB -> ARM (BL -> BLX) */
    *ins1 = 0xF000;
    *ins2 = 0xF800;

    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x2001000;

    unit_arm_reloc_do(R_ARM_THM_CALL,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1, 0xF001);
    TS_ASSERT_EQUALS(*ins2, 0xE802);

    /* THUMB -> THUMB */
    *ins1 = 0xF000;
    *ins2 = 0xF800;

    unit_arm_reloc_do(R_ARM_THM_CALL,
             (uint8_t*) &address_space,
             addend, symval | 0x1, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1, 0xF001);
    TS_ASSERT_EQUALS(*ins2, 0xF802);

    /* THUMB2 Large Offset */
    *ins1 = 0xF000;
    *ins2 = 0xF800;

    pc = 0x20;
    symval = 0x800020;

    unit_arm_reloc_do(R_ARM_THM_CALL,
             (uint8_t*) &address_space,
             addend, symval | 0x1, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1, 0xF000);
    TS_ASSERT_EQUALS(*ins2, 0xD802);
}

void ARM_TestRelocDo::test_R_ARM_THM_JUMP11()
{
    uint16_t address_space = 0xE000;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x2000101;

    unit_arm_reloc_do(R_ARM_THM_JUMP11,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE082);
}

void ARM_TestRelocDo::test_R_ARM_THM_JUMP8()
{
    uint16_t address_space = 0xDA00;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x2000081;

    unit_arm_reloc_do(R_ARM_THM_JUMP8,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xDA42);
}

void ARM_TestRelocDo::test_R_ARM_THM_PC8()
{
    uint16_t address_space = 0x9F00;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000002;
    uint32_t symval = 0x2000080;

    unit_arm_reloc_do(R_ARM_THM_PC8,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x9F21);
}

void ARM_TestRelocDo::test_R_ARM_THM_ABS5()
{
    uint16_t address_space = 0x683F;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000002;
    uint32_t symval = 0x2000040;

    unit_arm_reloc_do(R_ARM_THM_ABS5,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x6C7F);
}

void ARM_TestRelocDo::test_R_ARM_THM_JUMP6()
{
    uint16_t address_space = 0xB907;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x2000040;

    unit_arm_reloc_do(R_ARM_THM_JUMP6,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xBB17);
}

void ARM_TestRelocDo::test_R_ARM_ABS12()
{
    uint32_t address_space = 0xE5BFF000;
    uint32_t addend = 0x3;
    uint32_t symval = 0xFFC;
    uint32_t pc = 0x0;

    unit_arm_reloc_do(R_ARM_ABS12,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE5BFFFFF);

    address_space = 0xE5BFF000;

    addend = -1UL;
    symval = 0x0;

    unit_arm_reloc_do(R_ARM_ABS12,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE53FF001);
}

void ARM_TestRelocDo::test_R_ARM_ABS8()
{
    uint8_t address_space = 0x0;
    uint32_t addend = 0x4;
    uint32_t symval = 0xE;
    uint32_t pc = 0x0;

    unit_arm_reloc_do(R_ARM_ABS8, &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x12);
}

void ARM_TestRelocDo::test_R_ARM_ABS16()
{
    uint16_t address_space = 0x0;
    uint32_t addend = 0x4;
    uint32_t symval = 0xFFE;
    uint32_t pc = 0x0;

    unit_arm_reloc_do(R_ARM_ABS16,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x1002);
}

void ARM_TestRelocDo::test_R_ARM_THM_JUMP19()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend = 0x4;
    uint32_t symval = 0x2004000;
    uint32_t pc = 0x2000000;
            
    /* Positive */
    *ins1_ptr = 0xF3C0;
    *ins2_ptr = 0x8000;
            
    unit_arm_reloc_do(R_ARM_THM_JUMP19,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF3C4);
    TS_ASSERT_EQUALS(*ins2_ptr, 0x8002);

    /* Negative */
    *ins1_ptr = 0xF3C0;
    *ins2_ptr = 0x8000;

    symval = 0x2000000;
    pc     = 0x2004000;

    unit_arm_reloc_do(R_ARM_THM_JUMP19,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);
            
    TS_ASSERT_EQUALS(*ins1_ptr, 0xF7FC);
    TS_ASSERT_EQUALS(*ins2_ptr, 0xA802);
}

void ARM_TestRelocDo::test_R_ARM_PREL31()
{
    uint32_t address_space = 0x80000000;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x2001000;

    unit_arm_reloc_do(R_ARM_PREL31,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x80001004);
}

void ARM_TestRelocDo::test_R_ARM_MOVW_ABS_NC()
{
    uint32_t address_space = 0xE300F000;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x3FFFFFFB;

    unit_arm_reloc_do(R_ARM_MOVW_ABS_NC,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE30FFFFF);
}

void ARM_TestRelocDo::test_R_ARM_MOVT_ABS()
{
    uint32_t address_space = 0xE300F000;
    uint32_t addend = 0x4;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0xFFFFFFFB;

    unit_arm_reloc_do(R_ARM_MOVT_ABS,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE30FFFFF);
}

void ARM_TestRelocDo::test_R_ARM_MOVW_PREL_NC()
{
    uint32_t address_space = 0xE300F000;
    uint32_t addend = 0x7004;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x3200F000;

    unit_arm_reloc_do(R_ARM_MOVW_PREL_NC,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE306F004);
}

void ARM_TestRelocDo::test_R_ARM_MOVT_PREL()
{
    uint32_t address_space = 0xE300F000;
    uint32_t addend = 0x7004;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x3200F000;

    unit_arm_reloc_do(R_ARM_MOVT_PREL,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0xE303F001);

}

void ARM_TestRelocDo::test_R_ARM_THM_MOVW_ABS_NC()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend = 0x7004;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0xE200F000;

    *ins1_ptr = 0xF64F;
    *ins2_ptr = 0x7FFF;

    unit_arm_reloc_do(R_ARM_THM_MOVW_ABS_NC,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF246);
    TS_ASSERT_EQUALS(*ins2_ptr, 0x0F04);
}

void ARM_TestRelocDo::test_R_ARM_THM_MOVT_ABS()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend = 0x7004;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0xE200F000;

    *ins1_ptr = 0xF6CF;
    *ins2_ptr = 0x7FFF;

    unit_arm_reloc_do(R_ARM_THM_MOVT_ABS,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF2CE);
    TS_ASSERT_EQUALS(*ins2_ptr, 0x2F01);
}
 
void ARM_TestRelocDo::test_R_ARM_THM_MOVW_PREL_NC()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend = 0x7004;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0x3200F000;

    *ins1_ptr = 0xF64F;
    *ins2_ptr = 0x7FFF;

    unit_arm_reloc_do(R_ARM_THM_MOVW_PREL_NC,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF246);
    TS_ASSERT_EQUALS(*ins2_ptr, 0x0F04);
}

void ARM_TestRelocDo::test_R_ARM_THM_MOVT_PREL()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend = 0x7004;
    uint32_t pc = 0x2000000;
    uint32_t symval = 0xE200F000;

    *ins1_ptr = 0xF6CF;
    *ins2_ptr = 0x7FFF;

    unit_arm_reloc_do(R_ARM_THM_MOVT_PREL,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF2CE);
    TS_ASSERT_EQUALS(*ins2_ptr, 0x0F01);
}

void ARM_TestRelocDo::test_R_ARM_ABS32_NOI()
{
    uint32_t address_space = 0x0;
    uint32_t addend = 0x4;
    uint32_t symval = 0x2000001;
    uint32_t pc = 0x0;

    unit_arm_reloc_do(R_ARM_ABS32_NOI,
             (uint8_t*) &address_space,
             addend, symval, pc, 
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x2000004);
    
}

void ARM_TestRelocDo::test_R_ARM_REL32_NOI()
{
    uint32_t address_space = 0x0;
    uint32_t addend = 0x4;
    uint32_t symval = 0x2010001;
    uint32_t pc = 0x2000000;

    unit_arm_reloc_do(R_ARM_REL32_NOI,
             (uint8_t*) &address_space,
             addend, symval, pc, 
             0, 0);

    TS_ASSERT_EQUALS(address_space, 0x10004);
}

/*****************************************************************************/
/* Group Relocation Tests                                                    */
/* These tests are adopted from the bugslayer tests for the linker:          */
/* elflnk_arm_reloc1                                                         */
/*****************************************************************************/

void ARM_TestRelocDo::test_R_ARM_ALU_PC_G0_NC()
{
    /*-----------------------------------------------------------------------*/
    /* The RelMaskForGroup test handles cases of different group numbers.    */
    /* Here we are more concerned about converting add to subtract, and      */
    /* writing the correct immediate.  Therefore, we only need to test one   */
    /* type of relocation.                                                   */
    /*-----------------------------------------------------------------------*/

    uint32_t sub_instruction = 0xE24FC008; // SUB R12,PC,#8
    uint32_t address_space = sub_instruction;
    uint32_t addend = -0x8;
    uint32_t symval = 0x04234569;
    uint32_t pc = 0x01234568;

    unit_arm_reloc_do(R_ARM_ALU_PC_G0_NC,
             (uint8_t*) &address_space,
             addend, symval, pc, 
             0, 0);

    // ADD R12,PC,0x2FC0000
    TS_ASSERT_EQUALS(address_space, 0xE28FC7BF);

    address_space = sub_instruction;
    pc = 0x04234569;
    symval = 0x01234568;

    unit_arm_reloc_do(R_ARM_ALU_PC_G0_NC,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    // SUB R12,PC,0x3000000
    TS_ASSERT_EQUALS(address_space, 0xE24FC7C0);
    
}

void ARM_TestRelocDo::test_R_ARM_LDR_PC_G0()
{
    uint32_t address_space = 0xE53FF000;
    uint32_t addend = 0xABC;
    uint32_t pc = 0x0;
    uint32_t symval = 0x0;

    unit_arm_reloc_do(R_ARM_LDR_PC_G0,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    /* Positive, bit 23 should be 1 */
    TS_ASSERT_EQUALS(address_space, 0xE5BFFABC);

    address_space = 0xE5BFF000;
    addend = -0xABC;

    unit_arm_reloc_do(R_ARM_LDR_PC_G0,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    /* Negative, bit 23 should be 0 */
    TS_ASSERT_EQUALS(address_space, 0xE53FFABC);
}

void ARM_TestRelocDo::test_R_ARM_LDRS_PC_G0()
{
    uint32_t address_space = 0xE16FF0D0;
    uint32_t addend = 0xAB;
    uint32_t pc = 0x0;
    uint32_t symval = 0x0;

    unit_arm_reloc_do(R_ARM_LDRS_PC_G0,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    /* Positive, bit 23 should be 1 */
    TS_ASSERT_EQUALS(address_space, 0xE1EFFADB);

    address_space = 0xE1EFF0D0;
    addend = -0xAB;

    unit_arm_reloc_do(R_ARM_LDRS_PC_G0,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    /* Negative, bit 23 should be 0 */
    TS_ASSERT_EQUALS(address_space, 0xE16FFADB);
}

void ARM_TestRelocDo::test_R_ARM_LDC_PC_G0()
{
    uint32_t address_space = 0xED7FFF00;
    uint32_t addend = 0xAB;
    uint32_t pc = 0x0;
    uint32_t symval = 0x0;

    unit_arm_reloc_do(R_ARM_LDC_PC_G0,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    /* Positive, bit 23 should be 1 */
    TS_ASSERT_EQUALS(address_space, 0xEDFFFFAB);

    address_space = 0xEDFFFF00;
    addend = -0xAB;

    unit_arm_reloc_do(R_ARM_LDC_PC_G0,
                      (uint8_t*) &address_space,
                      addend, symval, pc,
                      0, 0);

    /* Negative, bit 23 should be 0 */
    TS_ASSERT_EQUALS(address_space, 0xED7FFFAB);
}

void ARM_TestRelocDo::test_R_ARM_THM_PC12()
{
    uint32_t address_space;
    uint16_t* ins1_ptr;
    uint16_t* ins2_ptr;
    uint32_t addend;
    uint32_t symval;
    uint32_t pc;

    ins1_ptr = (uint16_t*) &address_space;
    ins2_ptr = ins1_ptr + 1;

    /*----------------------------------------------------------------*/
    /* Positive                                                       */
    /*----------------------------------------------------------------*/
    *ins1_ptr = 0xF81F;
    *ins2_ptr = 0xFFFF;

    symval = 0x2000AB8;
    pc     = 0x2000000;
    addend = 0x4;

    unit_arm_reloc_do(R_ARM_THM_PC12,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF89F);
    TS_ASSERT_EQUALS(*ins2_ptr, 0xFABC);

    /*----------------------------------------------------------------*/
    /* Negative                                                       */
    /*----------------------------------------------------------------*/

    *ins1_ptr = 0xF89F;
    *ins2_ptr = 0xFFFF;

    symval = 0x2000000;
    pc     = 0x2000AB8;
    addend = -0x4;

    unit_arm_reloc_do(R_ARM_THM_PC12,
             (uint8_t*) &address_space,
             addend, symval, pc,
             0, 0);

    TS_ASSERT_EQUALS(*ins1_ptr, 0xF81F);
    TS_ASSERT_EQUALS(*ins2_ptr, 0xFABC);
}

/*****************************************************************************/
/* ARM_TestRelUnpackAddend                                                   */
/*                                                                           */
/* Tests the ARM rel_unpack_addend function.                                 */
/* In cases where the addends are unpacked in the same way, only one is      */ 
/* tested.                                                                   */
/* All tests follow the same flow:                                           */
/* 1. Create a valid instruction for the relocation type, where the addend   */
/*    is packed in the instruction.                                          */
/* 2. Call rel_unpack_addend().                                              */
/* 3. Check that the addend is correct.                                      */
/*                                                                           */
/* Relocations may be tested multiple times to handle variations, such as    */
/* positive/negative addends, extra bits depending on the encoding, etc.     */
/*****************************************************************************/
void ARM_TestRelUnpackAddend::test_R_ARM_PC24()
{
    uint32_t address_space = 0xEB7FFFFF;
    uint32_t addend;

    /* BL Positive */
    unit_arm_rel_unpack_addend(R_ARM_PC24, 
                      (uint8_t*) &address_space, 
                      &addend);
    TS_ASSERT_EQUALS(addend, 0x1FFFFFC);

    /* BL Negative */
    address_space = 0xEB800000;
    unit_arm_rel_unpack_addend(R_ARM_PC24, 
                               (uint8_t*) &address_space, 
                               &addend);
    TS_ASSERT_EQUALS(addend, 0xFE000000);

    /* BLX */
    address_space = 0xFB7FFFFF;
    unit_arm_rel_unpack_addend(R_ARM_PC24, 
                               (uint8_t*) &address_space, 
                               &addend);
    TS_ASSERT_EQUALS(addend, 0x1FFFFFE);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_JUMP19()
{
    /* Boundary condition : +/- */
            
    /* Addend should be positive */
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend;
            
    *ins1_ptr = 0xF3FF;
    *ins2_ptr = 0xBFFF;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP19, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFE);

    /* addend should be negative */
    *ins1_ptr = 0xF400;
    *ins2_ptr = 0x9000;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP19, 
                               (uint8_t*) &address_space, 
                               &addend);
                              
    TS_ASSERT_EQUALS(addend, 0xFFF00000);

}
void ARM_TestRelUnpackAddend::test_R_ARM_ABS32()
{
    uint32_t address_space=0xFEDCBA9;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_ABS32, 
                               (uint8_t*)&address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, address_space);
}
void ARM_TestRelUnpackAddend::test_R_ARM_ABS16()
{
    uint16_t address_space=0x7FFF;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_ABS16, 
                               (uint8_t*)&address_space, 
                               &addend);
            
    TS_ASSERT_EQUALS(addend, 0x7FFF);

    address_space = 0x8000;

    unit_arm_rel_unpack_addend(R_ARM_ABS16, 
                               (uint8_t*)&address_space, 
                               &addend);
            
    TS_ASSERT_EQUALS(addend, 0xFFFF8000);
}

void ARM_TestRelUnpackAddend::test_R_ARM_ABS8()
{
    uint8_t address_space = 0x7F;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_ABS8, 
                               &address_space, 
                               &addend);
    
    TS_ASSERT_EQUALS(addend, 0x7F);

    address_space = 0x80;

    unit_arm_rel_unpack_addend(R_ARM_ABS8, 
                               &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFFF80);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_JUMP11()
{
    uint16_t address_space = 0xE3FF;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP11,
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0x7FE);

    address_space = 0xE700;
            
    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP11,
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFFE00);
}
    
void ARM_TestRelUnpackAddend::test_R_ARM_THM_JUMP8()
{
    uint16_t address_space = 0xDE7F;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP8,
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFE);

    address_space = 0xDE80;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP8, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFFF00);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_ABS5()
{
    uint16_t address_space = 0x6FC0;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_THM_ABS5, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0x7C);
}


void ARM_TestRelUnpackAddend::test_R_ARM_THM_CALL()
{
    uint32_t address_space;
    uint32_t addend;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;

    /* Thumb2 Positive */
    *ins1_ptr = 0xF3FF;
    *ins2_ptr = 0xD7FF;

    unit_arm_rel_unpack_addend(R_ARM_THM_CALL, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFFE);

    /* Thumb2 Negative */
    *ins1_ptr = 0xF400;
    *ins2_ptr = 0xD000;

    unit_arm_rel_unpack_addend(R_ARM_THM_CALL, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFF000000);

    /* Thumb Positive */
    *ins1_ptr = 0xF3FF;
    *ins2_ptr = 0xFFFF;

    unit_arm_rel_unpack_addend(R_ARM_THM_CALL, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0x3FFFFE);

    /* Thumb Negative */
    *ins1_ptr = 0xF400;
    *ins2_ptr = 0xF800;

    unit_arm_rel_unpack_addend(R_ARM_THM_CALL, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFC00000);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_JUMP6()
{
    uint16_t address_space = 0xB3F8;
    uint32_t addend;
    
    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP6, 
                               (uint8_t*) &address_space, 
                               &addend);
    
    TS_ASSERT_EQUALS(addend, 0xFFFFFFFE);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_PC8()
{
    uint16_t address_space = 0x98FF;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_THM_PC8, 
                               (uint8_t*) &address_space, 
                               &addend);
    
    TS_ASSERT_EQUALS(addend, -4U);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_JUMP24()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend;

    *ins1_ptr = 0xF3FF;
    *ins2_ptr = 0x97FF;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP24, 
                               (uint8_t*)&address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFFE);

    *ins1_ptr = 0xF400;
    *ins2_ptr = 0x9000;

    unit_arm_rel_unpack_addend(R_ARM_THM_JUMP24, 
                               (uint8_t*)&address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFF000000);
}

void ARM_TestRelUnpackAddend::test_R_ARM_PREL31()
{
    uint32_t address_space = 0xBFFFFFFF;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_PREL31, 
                               (uint8_t*) &address_space, 
                               &addend);
    
    TS_ASSERT_EQUALS(addend, 0x3FFFFFFF);

    address_space = 0x40000000;

    unit_arm_rel_unpack_addend(R_ARM_PREL31, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xC0000000);
}

void ARM_TestRelUnpackAddend::test_R_ARM_MOVW_ABS_NC()
{
    uint32_t address_space = 0xE3070FFF;
    uint32_t addend;

    unit_arm_rel_unpack_addend(R_ARM_MOVW_ABS_NC, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0x7FFF);

    address_space = 0xE30F0FFF;

    unit_arm_rel_unpack_addend(R_ARM_MOVW_ABS_NC, 
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFFFFFF);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_MOVW_ABS_NC()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend;

    *ins1_ptr = 0xF647;
    *ins2_ptr = 0x7FFF;

    unit_arm_rel_unpack_addend(R_ARM_THM_MOVW_ABS_NC,
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0x7FFF);

    *ins1_ptr = 0xF248;
    *ins2_ptr = 0x0F00;

    unit_arm_rel_unpack_addend(R_ARM_THM_MOVW_ABS_NC,
                               (uint8_t*) &address_space, 
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFFF8000);
    
}

void ARM_TestRelUnpackAddend::test_R_ARM_ABS12()
{
    uint32_t address_space;
    uint32_t addend;

    address_space = 0xE5BFFFFF;

    unit_arm_rel_unpack_addend(R_ARM_ABS12,
                               (uint8_t*) &address_space,
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFF);
    
    address_space = 0xE53FF001;

    unit_arm_rel_unpack_addend(R_ARM_ABS12,
                               (uint8_t*) &address_space,
                               &addend);

    TS_ASSERT_EQUALS(addend, -1);
}

void ARM_TestRelUnpackAddend::test_R_ARM_THM_PC12()
{
    uint32_t address_space;
    uint16_t* ins1_ptr = (uint16_t*) &address_space;
    uint16_t* ins2_ptr = ins1_ptr + 1;
    uint32_t addend;

    *ins1_ptr = 0xF89F;
    *ins2_ptr = 0xFFFF;

    unit_arm_rel_unpack_addend(R_ARM_THM_PC12,
                               (uint8_t*) &address_space,
                               &addend);

    TS_ASSERT_EQUALS(addend, 0xFFF);

    *ins1_ptr = 0xF81F;
    *ins2_ptr = 0xF001;

    unit_arm_rel_unpack_addend(R_ARM_THM_PC12,
                               (uint8_t*) &address_space,
                               &addend);

    TS_ASSERT_EQUALS(addend, -1);
}

/*****************************************************************************/
/* ARM_TestRelOverflow                                                       */
/*                                                                           */
/* Test the ARM rel_overflow function.                                       */
/* In each case we test the upper and lower bounds of each relocation type.  */
/* Only relocation types where the overflow is checked in rel_overflow are   */
/* considered.  In most cases four tests are performed to test the upper and */
/* lower bounds (1 pass and 1 fail for each).                                */
/* For ARM, some instructions use two's complement encoding and others just  */
/* have a sign bit, this is taken into consideration for testing.            */
/*****************************************************************************/
void ARM_TestRelOverflow::test_R_ARM_PC24()
{
    int32_t reloc_val = 0x1FFFFFC;
    int rval;

    rval = unit_arm_rel_overflow(R_ARM_PC24, reloc_val);

    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 0x2000000;

    rval = unit_arm_rel_overflow(R_ARM_PC24, reloc_val);

    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -0x2000000;

    rval = unit_arm_rel_overflow(R_ARM_PC24, reloc_val);

    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -0x2000001;

    rval = unit_arm_rel_overflow(R_ARM_PC24, reloc_val);
    
    TS_ASSERT_EQUALS(rval, 1);

}

void ARM_TestRelOverflow::test_R_ARM_ABS12()
{
    int32_t reloc_val;
    int rval; 

    reloc_val = 0xFFF;
    rval = unit_arm_rel_overflow(R_ARM_ABS12, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 0x1000;
    rval = unit_arm_rel_overflow(R_ARM_ABS12, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -0xFFF;
    rval = unit_arm_rel_overflow(R_ARM_ABS12, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -0x1000;
    rval = unit_arm_rel_overflow(R_ARM_ABS12, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);
}

void ARM_TestRelOverflow::test_R_ARM_THM_CALL()
{
    int32_t reloc_val;
    int rval;

    reloc_val = 0xFFFFFF;
    rval = unit_arm_rel_overflow(R_ARM_THM_CALL, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 0x1000000;
    rval = unit_arm_rel_overflow(R_ARM_THM_CALL, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -0x1000000;
    rval = unit_arm_rel_overflow(R_ARM_THM_CALL, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -0x1000001;
    rval = unit_arm_rel_overflow(R_ARM_THM_CALL, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);
}

void ARM_TestRelOverflow::test_R_ARM_THM_JUMP19()
{
    int32_t reloc_val;
    int rval;

    reloc_val = 0xFFFFF;
    rval = unit_arm_rel_overflow(R_ARM_THM_JUMP19, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 0x100000;
    rval = unit_arm_rel_overflow(R_ARM_THM_JUMP19, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -0x100000;
    rval = unit_arm_rel_overflow(R_ARM_THM_JUMP19, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -0x100001;
    rval = unit_arm_rel_overflow(R_ARM_THM_JUMP19, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);
}
        
void ARM_TestRelOverflow::test_R_ARM_ABS16()
{
    int32_t reloc_val;
    int rval;

    reloc_val = 65535;
    rval = unit_arm_rel_overflow(R_ARM_ABS16, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 65536;
    rval = unit_arm_rel_overflow(R_ARM_ABS16, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -32768;
    rval = unit_arm_rel_overflow(R_ARM_ABS16, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -32769;
    rval = unit_arm_rel_overflow(R_ARM_ABS16, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

}

void ARM_TestRelOverflow::test_R_ARM_ABS8()
{
    int32_t reloc_val;
    int rval;

    reloc_val = 255;
    rval = unit_arm_rel_overflow(R_ARM_ABS8, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 256;
    rval = unit_arm_rel_overflow(R_ARM_ABS8, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -128;
    rval = unit_arm_rel_overflow(R_ARM_ABS8, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -129;
    rval = unit_arm_rel_overflow(R_ARM_ABS8, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);
}

void ARM_TestRelOverflow::test_R_ARM_PREL31()
{
    int32_t reloc_val;
    int rval;

    reloc_val = 0x3FFFFFFF;
    rval = unit_arm_rel_overflow(R_ARM_PREL31, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = 0x40000000;
    rval = unit_arm_rel_overflow(R_ARM_PREL31, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);

    reloc_val = -0x40000000;
    rval = unit_arm_rel_overflow(R_ARM_PREL31, reloc_val);
    TS_ASSERT_EQUALS(rval, 0);

    reloc_val = -0x40000001;
    rval = unit_arm_rel_overflow(R_ARM_PREL31, reloc_val);
    TS_ASSERT_EQUALS(rval, 1);
}

void ARM_TestGroupRelocations::test_RelMaskForGroup()
{
    int32_t reloc_val;
    int32_t standard = 0xABCDEF;

    reloc_val = standard;
    unit_arm_rel_mask_for_group(R_ARM_ALU_PC_G0_NC, &reloc_val);
    TS_ASSERT_EQUALS(reloc_val, 0xAB0000);

    reloc_val = standard;
    unit_arm_rel_mask_for_group(R_ARM_ALU_PC_G1_NC, &reloc_val);
    TS_ASSERT_EQUALS(reloc_val, 0xCD00);

    reloc_val = standard;
    unit_arm_rel_mask_for_group(R_ARM_ALU_PC_G2, &reloc_val);
    TS_ASSERT_EQUALS(reloc_val, 0xEF);
}
