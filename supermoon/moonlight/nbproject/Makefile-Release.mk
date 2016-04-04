#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=None-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1360920745/bmc.o \
	${OBJECTDIR}/_ext/1685954798/daq.o \
	${OBJECTDIR}/_ext/1181056713/bmcnet.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs comedilib` `pkg-config --libs xaw7`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moonlight

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moonlight: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moonlight ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1360920745/bmc.o: ../bmc/bmc.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1360920745
	${RM} "$@.d"
	$(COMPILE.c) -O2 `pkg-config --cflags comedilib` `pkg-config --cflags xaw7`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1360920745/bmc.o ../bmc/bmc.c

${OBJECTDIR}/_ext/1685954798/daq.o: ../bmc/bmc/daq.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1685954798
	${RM} "$@.d"
	$(COMPILE.c) -O2 `pkg-config --cflags comedilib` `pkg-config --cflags xaw7`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1685954798/daq.o ../bmc/bmc/daq.c

${OBJECTDIR}/_ext/1181056713/bmcnet.o: ../bmc/bmc_x86/bmcnet.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1181056713
	${RM} "$@.d"
	$(COMPILE.c) -O2 `pkg-config --cflags comedilib` `pkg-config --cflags xaw7`   -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1181056713/bmcnet.o ../bmc/bmc_x86/bmcnet.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/moonlight

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
