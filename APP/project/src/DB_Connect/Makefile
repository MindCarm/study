include /home/mg1/APP/project/build/make_defines.mk

GIS_SIM_DIR			=	/home/mg1/APP/project/bin

####################################################################
#	Sample Interface
####################################################################

TARGET_TYPE			=	executable

TARGET				=	mg_DB_Connect

SRC					=	maria \
						main

OBJS				=	$(SRC:=.o)

MY_CXXFLAGS			=	$(PVS_CFLAGS) 

MY_LDFLAGS			=	$(PVS_LDFLAGS)


#===============================================================================
# target definition
#===============================================================================

build	:	build_$(TARGET_TYPE)

new		:	clean build

clean	:
		@echo
		@echo ---------------------------------------------------------------------
		@echo Cleaning-up......
		@echo ---------------------------------------------------------------------
		$(RM) *.o *.a *.test core core.* $(TARGET) $(CLASS:=.test)
		@echo ---------------------------------------------------------------------
		@echo ORACLE Pro*C precompiled files Cleaning-up......
		@echo ---------------------------------------------------------------------
		for name in $(SRC); do \
		        if [ -f "$$name.pc" ]; then $(RM) $$name.cpp $$name.lis tp*; fi \
		done
		for name in $(SRC); do \
		        if [ -f "$$name.sc" ]; then $(RM) $$name.cpp $$name.lis; fi \
		done

install				:	install_$(TARGET_TYPE)


build_executable	:	$(OBJS)
		@echo
		@echo ---------------------------------------------------------------------
		@echo $(TARGET) Linking......build_executable
		@echo ---------------------------------------------------------------------
		$(CXX) -o $(TARGET) $(OBJS) $(MY_LDFLAGS)

install_executable	:
		@echo
		@echo ---------------------------------------------------------------------
		@echo $(TARGET) Installing......install_executable
		@echo ---------------------------------------------------------------------
		cp $(TARGET) $(PVS_SIM_DIR)/

#===============================================================================
# SUFFIX rule definition
#===============================================================================

.SUFFIXES	:	.o .c .pc

.cpp.o		:
		@echo
		@echo ---------------------------------------------------------------------
		@echo $< Compiling......
		@echo ---------------------------------------------------------------------
		$(CXX) -c $< $(MY_CXXFLAGS) 

.pc.c		:
		@echo ---------------------------------------------------------------------
		@echo $< Pro*C Compiling......
		@echo ---------------------------------------------------------------------
		$(PROC) $< $(PROC_CFLAGS) $(MY_PROC_CFLAGS)

.pc.o		:
		@echo ---------------------------------------------------------------------
		@echo $< Pro*C Compiling......
		@echo ---------------------------------------------------------------------
		$(PROC) $< $(PROC_CFLAGS) $(MY_PROC_CFLAGS)
		@echo ---------------------------------------------------------------------
		@echo $*.cpp Compiling......
		@echo ---------------------------------------------------------------------
		$(CXX) -c $*.cpp $(MY_CXXFLAGS)

####################################################################
#   Sample Interface
####################################################################