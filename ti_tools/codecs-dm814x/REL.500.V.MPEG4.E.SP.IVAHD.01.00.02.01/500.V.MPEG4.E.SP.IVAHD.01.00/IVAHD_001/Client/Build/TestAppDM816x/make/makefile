# 
#********************************************************************************
#
#  HDVICP2.0 Based MPEG4 SP Encoder
# 
# "HDVICP2.0 Based MPEG4 SP Encoder" is software module developed on TI's 
#  HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw 
#  video into a simple profile bit-stream. Based on ISO/IEC 14496-2."
#
# Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
# ALL RIGHTS RESERVED 
#******************************************************************************
# 

#  
#******************************************************************************
# @file makefile
#                                       
# @brief This file contains all make rules required for compiling/building
#        Netra Test App
#   
# @author: Karthick Kumaran ASV(karthick.kumaran@ti.com)
#
# @version 0.0 (May 2010) : Base version created
#
#*****************************************************************************
# 

#-----------------------------------------------------------------------------#
# Install directires of all the components required for building the           #
# Test App software. These can either from environment variables or the       #
# absolute paths                                                              #
#-----------------------------------------------------------------------------#
HDVICP_DIR = %HDVICP2_INSTALL_DIR%
CSP_DIR  = %CSP_INSTALL_DIR%
FC_INST_DIR = %FC_INSTALL_DIR%
CG_TOOL = %CG_TOOL_DIR_M3%
XDC_DIR = %XDCROOT%

#-----------------------------------------------------------------------------#

#-----------------------------------------------------------------------------#
# Creating a common root to access all the files in the entire code base      #
#-----------------------------------------------------------------------------#
ROOT_DIR = ..\..\..\..


# Add inputs and outputs from these tool invocations to the build variables 
LIB_SRCS1 += \
$(HDVICP_DIR)\lib\ivahd_ti_api_vm3.lib

LIB_SRCS += \
$(ROOT_DIR)\lib\mpeg4venc_ti_host.lib


LIB_SRCS2 += \
$(ROOT_DIR)\client\Test\Src\tiler.lib

#-----------------------------------------------------------------------------#
# RTS library used by this software                                           #
#-----------------------------------------------------------------------------#
RTS_LIBS += \
-l"rtsv7M3_T_le_eabi.lib"


#-----------------------------------------------------------------------------#
# List of C source files needs to be compiled                                 #
#-----------------------------------------------------------------------------#
C_SRCS += \
$(ROOT_DIR)\client\Test\Src\alg_control.c \
$(ROOT_DIR)\client\Test\Src\alg_create.c \
$(ROOT_DIR)\client\Test\Src\alg_malloc.c \
$(ROOT_DIR)\client\Test\Src\buffermanager.c \
$(ROOT_DIR)\client\Test\Src\InterruptVecTable.c \
$(ROOT_DIR)\client\Test\Src\mpeg4enc.c \
$(ROOT_DIR)\client\Test\Src\mpeg4enc_ti_config.c \
$(ROOT_DIR)\client\Test\Src\MPEG4SPEnc_rman_config.c \
$(ROOT_DIR)\client\Test\Src\MPEG4SPEncTest.c 



#-----------------------------------------------------------------------------#
# Linker command file required to link this software                          #
#-----------------------------------------------------------------------------#
CMD_FILE += \
../Host_M3.cmd

#-----------------------------------------------------------------------------#
# Final target out file                                                       #
#-----------------------------------------------------------------------------#
OUT_FILE += \
$(ROOT_DIR)\client\Build\TestAppDM816x\Out\MPEG4SPEncIvahd_Host_TestApp.out

#-----------------------------------------------------------------------------#
# Object file directory                                                       #
#-----------------------------------------------------------------------------#
OBJ_DIR += $(ROOT_DIR)\client\Build\TestAppDM816x\Obj
#-----------------------------------------------------------------------------#
# Deriving OBJ and PPA(dependecies list file) files list for C source list    #
#-----------------------------------------------------------------------------#
C_OBJS      = $(C_SRCS:.c=.obj)
C_PPS       = $(C_SRCS:.c=.PP)
#-----------------------------------------------------------------------------#
# Extracting the file name alone (removing the directry name from the list)   #
#-----------------------------------------------------------------------------#
C_OBJS_FILE_TEMP = $(notdir $(C_OBJS))
C_PPS_FILE  = $(notdir $(C_PPS))
C_OBJS_FILE = $(addprefix $(OBJ_DIR)\,$(C_OBJS_FILE_TEMP))


RM := del /f

CC     = "$(CG_TOOL)/bin/armcl"

#-----------------------------------------------------------------------------#
# List of folder needs to be searched for a included header file              #
#-----------------------------------------------------------------------------#
INCDIR = --include_path="$(CG_TOOL)\include" \
	 --include_path="$(FC_INST_DIR)/fctools/packages/ti/xdais" \
	 --include_path="$(FC_INST_DIR)/fctools/packages" \
	 --include_path="$(FC_INST_DIR)/packages/ti/xdais" \
	 --include_path="$(FC_INST_DIR)/packages" \
	 --include_path="$(ROOT_DIR)/client/Test/Inc" \
	 --include_path="$(ROOT_DIR)/inc" \
	 --include_path="$(CSP_DIR)/" \
	 --include_path="$(CSP_DIR)/csl_soc" \
	 --include_path="$(CSP_DIR)/csl_ivahd"

CFLAGS = -mv7M3 --define="HOST_M3" --define="NETRA_SIM" --define="_SIMULATOR_" \
	 --diag_warning=225 --wchar_t=32 --keep_unneeded_types=false -me -mc -ml \
	 --align_structs=4 --symdebug:dwarf --enum_type=packed --elf \
	 --gen_func_subsections --abi=eabi --auto_inline=200 --signed_chars \
	 --obj_directory="$(OBJ_DIR)"
#-----------------------------------------------------------------------------#
# Default configuration This can be overridden by command line argument       #
#-----------------------------------------------------------------------------#

CONFIGURATION = DEBUG

ifeq ($(CONFIGURATION),DSS)
C_SRCS += $(ROOT_DIR)\client\Test\Src\pci.c
CFLAGS += --define="USE_DSS_SCRIPTING" --define="PROFILE_HOST"
OUT_FILE = ../Out/MPEG4SPEncIvahd_Host_TestApp_DSS.out
endif
	 
all: $(OUT_FILE)

$(OUT_FILE) : $(C_OBJS_FILE) $(CMD_FILE) $(LIB_SRCS)
	echo Linking ...
	$(CC) $(C_OBJS_FILE) --run_linker -I "$(CG_TOOL)\lib" \
	--library=$(CMD_FILE) $(RTS_LIBS) $(LIB_SRCS1) $(LIB_SRCS2) $(LIB_SRCS) \
	--warn_sections -z --reread_libs --diag_suppress=16032 \
	--rom_model -m"../Map/MPEG4SPEncIvahd_TestApp.map" -o $(OUT_FILE)
	echo Linking Completed...
	
#-----------------------------------------------------------------------------#
# Each source file can have list of header files includes. If there is any    #
# change in any of the header files then the corresponding C files needs to   #
# be remade. To get this dependecy, the compile can invoked with "-ppd" otion #
# for this corresponding depency file. This dependency file needs to added to #
# the reules while making the "all" targets                                   #
#-----------------------------------------------------------------------------#
ifeq ($(MAKECMDGOALS),all)
ifneq ($(strip $(C_PPS_FILE)),)
-include $(C_PPS_FILE)
endif

#-----------------------------------------------------------------------------#
# Compiling the C source files to get the corresponding object files          #
#-----------------------------------------------------------------------------#
$(C_OBJS_FILE) : 
	echo Compiling  $(<)
	$(CC) $(<) -g $(CFLAGS) $(INCDIR)
	echo Finished
	

endif

#-----------------------------------------------------------------------------#
# Creating all the required dependency file for OBJ file. Also creating the   #
# dependency for target file on library sources                               #
#-----------------------------------------------------------------------------#
deps:
	echo Scanning dependencies . . .
	FOR %%i IN ($(LIB_SRCS)) DO echo $(OUT_FILE) : %%i >> libs.dep
	$(CC) $(C_SRCS) -ppd $(CFLAGS) $(INCDIR)
	echo echo OFF > temp_pps.bat
	FOR %%i IN ($(C_PPS)) DO echo move %%i >> temp_pps.bat
	echo echo ON >> temp_pps.bat
	temp_pps.bat
	-$(RM) temp_pps.bat



#-----------------------------------------------------------------------------#
# List of .PHONY (just rules not the actual files) available in this makefile #
#-----------------------------------------------------------------------------#
.PHONY: clean all deps
	
#-----------------------------------------------------------------------------#
# List rule for removing all the intermediate file created during            #
# build of the target                                                         #
#-----------------------------------------------------------------------------#
clean :
	-$(RM) $(C_PPS_FILE) $(C_OBJS_FILE) $(OUT_FILE) libs.dep
#-----------------------------------------------------------------------------#
# End of the file                                                             #
#-----------------------------------------------------------------------------#

