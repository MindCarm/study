#===============================================================================
# Compiler & Linker definition
#===============================================================================
CXX					=	/usr/bin/g++

PVS_DEBUG_MODE		=
ARCH_MODE       	=	-m64
DEBUG_MODE      	=	-g -ggdb -ffor-scope -Wno-deprecated -D_DEBUG_MODE

#OPT_OPTIONS    	=	-Wall -O3
OPT_OPTIONS     	=	-Wall 

#DEFINES			=	-D_REENTRANT -D__LINUX -D__HAVE_STRRSTR -D_POSIX_C_SOURCE
DEFINES   	 		=	-D_REENTRANT -D__LINUX -D__LITTLE_ENDIAN -D__ARCH64 -fPIC
#DEFINES		 	=	-D_REENTRANT -D__LINUX 

STDCPP				=	-std=gnu++20

#===============================================================================
# 64bit data model
#===============================================================================
DATA_MODEL			=   -q64

CXXFLAGS			=	$(ARCH_MODE) $(DEBUG_MODE) $(OPT_OPTIONS) $(DEFINES) $(STDCPP)

#===============================================================================
# -Wl,+s : runtime path specification
#===============================================================================
LDFLAGS				=	$(ARCH_MODE) $(OPT_OPTIONS)

#===============================================================================
# Produces a relocatable object
#===============================================================================
AR					=	/usr/bin/ar
ARFLAGS				=	-r

RM					=	rm -f

################################################################################
# END of file
################################################################################