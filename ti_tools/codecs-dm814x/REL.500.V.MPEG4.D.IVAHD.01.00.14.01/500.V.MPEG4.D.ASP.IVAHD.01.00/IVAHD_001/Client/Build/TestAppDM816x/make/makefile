# 
#*******************************************************************************
# HDVICP2.0 Based MPEG4 ASP Decoder
# 
# "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
#  HDVICP2 based SOCs. This module is capable of decoding a video stream of 
#  MPEG -4 Advanced/Simple profile and also H.263 bit-streams to raw YUV 4:2:0
#  video. Based on  ISO/IEC 14496-2:2003."
# Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
# ALL RIGHTS RESERVED
#*******************************************************************************

#******************************************************************************
# @file makefile
#                                       
# @brief This file contains all make rules required for compiling\building
#        decoder .out file in standalone test application on host side
#   
# @author: Kumar Desappan (kumar.desappan@ti.com)
#
# @version 0.0 (May 2010) : Base version created
#
# @version 0.1 (Sep 2010) : Modified for mpeg4 decoder
#
#*****************************************************************************
# 

#-----------------------------------------------------------------------------#
# Install directires of all thecomponants required for building the           #
# IVAHD iCONT2 software. These can either from environment variables or the   #
# absolute paths                                                              #
#-----------------------------------------------------------------------------#
HDVICP_DIR = %HDVICP2_INSTALL_DIR%
CSP_DIR  = %CSP_INSTALL_DIR%
FC_DIR = %FC_INSTALL_DIR%
CG_TOOL = %CG_TOOL_DIR_M3%
XDC_INSTALL_VER = %XDCROOT%

#-----------------------------------------------------------------------------#
# Creating a common root to access all the files in the entire code basse     #
#-----------------------------------------------------------------------------#
ROOT_DIR = ..\..\..\..
 
#-----------------------------------------------------------------------------#
# List of Pre-requisite libraries required to build this software             #
#-----------------------------------------------------------------------------#
LIB_SRCS += \
$(ROOT_DIR)\lib\mpeg4vdec_ti_host.lib \
$(HDVICP_DIR)\lib\ivahd_ti_api_vm3.lib \
$(ROOT_DIR)\client\Test\Src\fileio.lib \
$(ROOT_DIR)\client\Test\Src\tiler.lib

#-----------------------------------------------------------------------------#
# RTS library used by this software                                           #
#-----------------------------------------------------------------------------#
RTS_LIBS += \
-l"rtsv5_A_le_eabi.lib"

#-----------------------------------------------------------------------------#
# List of C source files needs to be compiled                                 #
#-----------------------------------------------------------------------------#
C_SRCS += \
$(ROOT_DIR)\client\Test\Src\alg_control.c \
$(ROOT_DIR)\client\Test\Src\alg_create.c \
$(ROOT_DIR)\client\Test\Src\alg_malloc.c \
$(ROOT_DIR)\client\Test\Src\crc0.c \
$(ROOT_DIR)\client\Test\Src\mpeg4vdec.c \
$(ROOT_DIR)\client\Test\Src\InterruptVecTable.c \
$(ROOT_DIR)\client\Test\Src\TestApp_bufmanager.c \
$(ROOT_DIR)\client\Test\Src\TestApp_fileIO.c \
$(ROOT_DIR)\client\Test\Src\TestAppProfile.c \
$(ROOT_DIR)\client\Test\Src\TestApp_rmanConfig.c \
$(ROOT_DIR)\client\Test\Src\Testapp_configparser.c \
$(ROOT_DIR)\client\Test\Src\TestAppDecoder.c \
$(ROOT_DIR)\client\Test\Src\TestAppDecSupport.c



#-----------------------------------------------------------------------------#
# List of C source files needs to be compiled with special compile options    #
# Depending on the requirement more number of groups also can be created      #
#-----------------------------------------------------------------------------#
#DEBUG_C_SRCS += \

#-----------------------------------------------------------------------------#
# Linker command file required to link this software                          #
#-----------------------------------------------------------------------------#
CMD_FILE += \
..\mpeg4vdec_ti_host_netrasim.cmd

#-----------------------------------------------------------------------------#
# Final target out file                                                       #
#-----------------------------------------------------------------------------#
OUT_FILE += \
..\Out\mpeg4vdec_ti_hosttestapp.out

#-----------------------------------------------------------------------------#
# Deriving OBJ and PPA(dependecies list file) files list for C source list    #
#-----------------------------------------------------------------------------#
C_OBJS      = $(C_SRCS:.c=.obj)
C_PPS       = $(C_SRCS:.c=.PP)
#-----------------------------------------------------------------------------#
# Extracting the file name alone (removing the directry name from the list)   #
#-----------------------------------------------------------------------------#
C_OBJS_FILE = $(notdir $(C_OBJS))
C_PPS_FILE  = $(notdir $(C_PPS))

#-----------------------------------------------------------------------------#
# Deriving OBJ and PPA(dependecies list file) files list for C source list    #
#-----------------------------------------------------------------------------#
#DEBUG_C_OBJS      = $(DEBUG_C_SRCS:.c=.obj)
#DEBUG_C_PPS       = $(DEBUG_C_SRCS:.c=.PP)
#-----------------------------------------------------------------------------#
# Extracting the file name alone (removing the directry name from the list)   #
#-----------------------------------------------------------------------------#
#DEBUG_C_OBJS_FILE = $(notdir $(DEBUG_C_OBJS))
#DEBUG_C_PPS_FILE  = $(notdir $(DEBUG_C_PPS))

#-----------------------------------------------------------------------------#
# Definin RM as force delete. This is used while target clean                 #
#-----------------------------------------------------------------------------#
RM = del /f

#-----------------------------------------------------------------------------#
# Compiler executable                                                         #
#-----------------------------------------------------------------------------#
CC     = "$(CG_TOOL)\bin\armcl"

#-----------------------------------------------------------------------------#
# List of folder needs to be searched for a included header file              #
#-----------------------------------------------------------------------------#
INCDIR = --include_path="$(CG_TOOL)\include" \
		 --include_path="$(HDVICP_DIR)\inc" \
		 --include_path="$(CSP_DIR)\csl_soc" \
		 --include_path="$(CSP_DIR)\csl_ivahd" \
		 --include_path="$(FC_DIR)\fctools\packages" \
		 --include_path="$(FC_DIR)\packages" \
		 --include_path="$(ROOT_DIR)\client\Test\Inc" \
		 --include_path="$(ROOT_DIR)\inc" \
		 --include_path="$(XDC_INSTALL_VER)\packages"

#-----------------------------------------------------------------------------#
# Common compile flags                                                        #
#-----------------------------------------------------------------------------#
CFLAGS = -mv7M3 --define="NETRA" \
         --define="HOST_M3" \
         --define="xdc_target_types__= ti\targets\std.h" \
		 --define="xdc_target_name__= TMS470"  --diag_warning=225 -me \
		 --diag_warning=14 --diag_warning=48 --diag_warning=69 --diag_warning=77 \
   --diag_warning=97 --diag_warning=112 --diag_warning=129 --diag_warning=167 \
   --diag_warning=169 --diag_warning=172 --diag_warning=173 --diag_warning=174 \
   --diag_warning=188 --diag_warning=190 --diag_warning=225 --diag_warning=132 \
   --diag_warning=139 --diag_warning=303 --diag_warning=383 --diag_warning=452 \
   --diag_warning=515 --diag_warning=551 --diag_warning=552 --diag_warning=682 \
   --diag_warning=1107 --diag_warning=1116 --diag_warning=1311 \
				 --wchar_t=32 --keep_unneeded_types=false
				 --enum_type=packed --elf --gen_func_subsections --abi=eabi \
				 --signed_chars --auto_inline=200

#-----------------------------------------------------------------------------#
# Final target needs to be build                                              #
#-----------------------------------------------------------------------------#
all: $(OUT_FILE)

#-----------------------------------------------------------------------------#
# Rules to build the target. Here $(C_OBJS_FILE) $(DEBUG_C_OBJS_FILE)         #
# $(ASM_OBJS_FILE) $(CMD_FILE) are pre requisite files. Target wil be         #
# remade only if there is any change (time stamp) in these file               #
#-----------------------------------------------------------------------------#
$(OUT_FILE) : $(C_OBJS_FILE) $(CMD_FILE)
	echo Linking ...
	$(CC) $(C_OBJS_FILE) --run_linker -I \
	"$(CG_TOOL)\lib" --library=$(CMD_FILE) $(RTS_LIBS) $(LIB_SRCS) \
	--warn_sections -z --reread_libs --diag_suppress=16032 \
	--rom_model -m"..\Map\mpeg4vdec_ti_hosttestapp.map" -o $(OUT_FILE)
	echo Linking Completed------------------------------------------------------

	
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
#ifneq ($(strip $(DEBUG_C_PPS_FILE)),)
#-include $(DEBUG_C_PPS_FILE)
#endif

ifneq ($(strip $(LIB_SRCS)),)
-include libs.dep
endif

#-----------------------------------------------------------------------------#
# Compiling the C source files to get the corresponding object files          #
#-----------------------------------------------------------------------------#
$(C_OBJS_FILE) :
	echo Compiling  $(<)
	$(CC) $(<) --symdebug:dwarf  $(CFLAGS)  $(INCDIR)
	echo Finished
	
#-----------------------------------------------------------------------------#
# Compiling the C source files to get the corresponding object files          #
# with special compile otption                                                #
#-----------------------------------------------------------------------------#
#$(DEBUG_C_OBJS_FILE) :
#	echo Compiling  $(<)
#	$(CC) $(<) --symdebug:dwarf  $(CFLAGS)  $(INCDIR)
#	echo Finished

endif

#-----------------------------------------------------------------------------#
# Creating all the required dependency file for OBJ file. Also creating the   #
# dependency for target file on library sources                               #
#-----------------------------------------------------------------------------#
deps:
	echo Scaning dependencies...
	-$(RM) libs.dep
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
# List rule for removing all the intermidaitre file created during            #
# build of the target                                                         #
#-----------------------------------------------------------------------------#
clean :
	-$(RM) $(C_PPS_FILE) $(C_OBJS_FILE) \
	 $(OUT_FILE) libs.dep
#-----------------------------------------------------------------------------#
# End of the file                                                             #
#-----------------------------------------------------------------------------#

