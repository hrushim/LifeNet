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
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=
AS=

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=build/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/socket_functions.o \
	${OBJECTDIR}/network_functions.o \
	${OBJECTDIR}/supporting_functions.o \
	${OBJECTDIR}/stats_log_functions.o \
	${OBJECTDIR}/display_functions.o \
	${OBJECTDIR}/manifold_routing.o \
	${OBJECTDIR}/sniff.o

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
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-Release.mk dist/Release/GNU-Linux-x86/sniff

dist/Release/GNU-Linux-x86/sniff: ${OBJECTFILES}
	${MKDIR} -p dist/Release/GNU-Linux-x86
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/sniff ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/socket_functions.o: nbproject/Makefile-${CND_CONF}.mk socket_functions.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/socket_functions.o socket_functions.c

${OBJECTDIR}/network_functions.o: nbproject/Makefile-${CND_CONF}.mk network_functions.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/network_functions.o network_functions.c

${OBJECTDIR}/supporting_functions.o: nbproject/Makefile-${CND_CONF}.mk supporting_functions.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/supporting_functions.o supporting_functions.c

${OBJECTDIR}/stats_log_functions.o: nbproject/Makefile-${CND_CONF}.mk stats_log_functions.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/stats_log_functions.o stats_log_functions.c

${OBJECTDIR}/display_functions.o: nbproject/Makefile-${CND_CONF}.mk display_functions.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/display_functions.o display_functions.c

${OBJECTDIR}/manifold_routing.o: nbproject/Makefile-${CND_CONF}.mk manifold_routing.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/manifold_routing.o manifold_routing.c

${OBJECTDIR}/sniff.o: nbproject/Makefile-${CND_CONF}.mk sniff.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/sniff.o sniff.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/Release
	${RM} dist/Release/GNU-Linux-x86/sniff

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
