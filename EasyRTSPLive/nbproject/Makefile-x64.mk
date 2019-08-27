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
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=x64
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ini.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/trace.o


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
LDLIBSOPTIONS=-L../../easyrtmp/Lib/${CND_CONF} -L../../EasyRTSPClient/Lib/${CND_CONF} -L../../EasyAACEncoder/Lib/${CND_CONF}

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_CONF}/EasyRTSPLive

${CND_CONF}/EasyRTSPLive: ${OBJECTFILES}
	${MKDIR} -p ${CND_CONF}
	${LINK.cc} -o ${CND_CONF}/EasyRTSPLive ${OBJECTFILES} ${LDLIBSOPTIONS} -pthread -lrt -leasyrtmp -leasyrtspclient -leasyaacencoder

${OBJECTDIR}/ini.o: ini.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../Include -I../../EasyAACEncoder/Include -I../../easyrtmp/Include -I../../EasyRTSPClient/Include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ini.o ini.cpp

${OBJECTDIR}/main.o: main.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../Include -I../../EasyAACEncoder/Include -I../../easyrtmp/Include -I../../EasyRTSPClient/Include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/trace.o: trace.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I../../Include -I../../EasyAACEncoder/Include -I../../easyrtmp/Include -I../../EasyRTSPClient/Include -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/trace.o trace.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
