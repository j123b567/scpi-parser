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
CND_PLATFORM=GNU-Linux-x86
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/760632520/scpi_debug.o \
	${OBJECTDIR}/_ext/760632520/scpi_utils.o \
	${OBJECTDIR}/_ext/760632520/scpi_fifo.o \
	${OBJECTDIR}/_ext/760632520/scpi_ieee488.o \
	${OBJECTDIR}/_ext/760632520/scpi_minimal.o \
	${OBJECTDIR}/_ext/760632520/scpi_parser.o \
	${OBJECTDIR}/_ext/1472/test-parser.o \
	${OBJECTDIR}/_ext/760632520/scpi_units.o \
	${OBJECTDIR}/_ext/760632520/scpi_error.o

# Test Directory
TESTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}/tests

# Test Files
TESTFILES= \
	${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans \
	${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/scpi_parser

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/scpi_parser: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/scpi_parser ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/760632520/scpi_debug.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_debug.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_debug.o ../scpi/scpi_debug.c

${OBJECTDIR}/_ext/760632520/scpi_utils.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_utils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_utils.o ../scpi/scpi_utils.c

${OBJECTDIR}/_ext/760632520/scpi_fifo.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_fifo.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_fifo.o ../scpi/scpi_fifo.c

${OBJECTDIR}/_ext/760632520/scpi_ieee488.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_ieee488.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_ieee488.o ../scpi/scpi_ieee488.c

${OBJECTDIR}/_ext/760632520/scpi_minimal.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_minimal.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_minimal.o ../scpi/scpi_minimal.c

${OBJECTDIR}/_ext/760632520/scpi_parser.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_parser.o ../scpi/scpi_parser.c

${OBJECTDIR}/_ext/1472/test-parser.o: nbproject/Makefile-${CND_CONF}.mk ../test-parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/test-parser.o ../test-parser.c

${OBJECTDIR}/_ext/760632520/scpi_units.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_units.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_units.o ../scpi/scpi_units.c

${OBJECTDIR}/_ext/760632520/scpi_error.o: nbproject/Makefile-${CND_CONF}.mk ../scpi/scpi_error.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_error.o ../scpi/scpi_error.c

# Subprojects
.build-subprojects:

# Build Test Targets
.build-tests-conf: .build-conf ${TESTFILES}
${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans: ${TESTDIR}/tests/test_fifo.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c}   -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans $^ ${LDLIBSOPTIONS} 

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans: ${TESTDIR}/tests/test_scpi_utils.o ${OBJECTFILES:%.o=%_nomain.o}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c}   -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans $^ ${LDLIBSOPTIONS} 


${TESTDIR}/tests/test_fifo.o: tests/test_fifo.c 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${TESTDIR}/tests/test_fifo.o tests/test_fifo.c


${TESTDIR}/tests/test_scpi_utils.o: tests/test_scpi_utils.c 
	${MKDIR} -p ${TESTDIR}/tests
	${RM} $@.d
	$(COMPILE.c) -O2 -MMD -MP -MF $@.d -o ${TESTDIR}/tests/test_scpi_utils.o tests/test_scpi_utils.c


${OBJECTDIR}/_ext/760632520/scpi_debug_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_debug.o ../scpi/scpi_debug.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_debug.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_debug_nomain.o ../scpi/scpi_debug.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_debug.o ${OBJECTDIR}/_ext/760632520/scpi_debug_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_utils_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_utils.o ../scpi/scpi_utils.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_utils.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_utils_nomain.o ../scpi/scpi_utils.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_utils.o ${OBJECTDIR}/_ext/760632520/scpi_utils_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_fifo_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_fifo.o ../scpi/scpi_fifo.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_fifo.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_fifo_nomain.o ../scpi/scpi_fifo.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_fifo.o ${OBJECTDIR}/_ext/760632520/scpi_fifo_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_ieee488_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_ieee488.o ../scpi/scpi_ieee488.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_ieee488.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_ieee488_nomain.o ../scpi/scpi_ieee488.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_ieee488.o ${OBJECTDIR}/_ext/760632520/scpi_ieee488_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_minimal_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_minimal.o ../scpi/scpi_minimal.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_minimal.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_minimal_nomain.o ../scpi/scpi_minimal.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_minimal.o ${OBJECTDIR}/_ext/760632520/scpi_minimal_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_parser_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_parser.o ../scpi/scpi_parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_parser.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_parser_nomain.o ../scpi/scpi_parser.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_parser.o ${OBJECTDIR}/_ext/760632520/scpi_parser_nomain.o;\
	fi

${OBJECTDIR}/_ext/1472/test-parser_nomain.o: ${OBJECTDIR}/_ext/1472/test-parser.o ../test-parser.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1472
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/1472/test-parser.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1472/test-parser_nomain.o ../test-parser.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/1472/test-parser.o ${OBJECTDIR}/_ext/1472/test-parser_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_units_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_units.o ../scpi/scpi_units.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_units.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_units_nomain.o ../scpi/scpi_units.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_units.o ${OBJECTDIR}/_ext/760632520/scpi_units_nomain.o;\
	fi

${OBJECTDIR}/_ext/760632520/scpi_error_nomain.o: ${OBJECTDIR}/_ext/760632520/scpi_error.o ../scpi/scpi_error.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/760632520
	@NMOUTPUT=`${NM} ${OBJECTDIR}/_ext/760632520/scpi_error.o`; \
	if (echo "$$NMOUTPUT" | ${GREP} '|main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T main$$') || \
	   (echo "$$NMOUTPUT" | ${GREP} 'T _main$$'); \
	then  \
	    ${RM} $@.d;\
	    $(COMPILE.c) -O2 -Dmain=__nomain -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/760632520/scpi_error_nomain.o ../scpi/scpi_error.c;\
	else  \
	    ${CP} ${OBJECTDIR}/_ext/760632520/scpi_error.o ${OBJECTDIR}/_ext/760632520/scpi_error_nomain.o;\
	fi

# Run Test Targets
.test-conf:
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans || true; \
	    ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/netbeans || true; \
	else  \
	    ./${TEST} || true; \
	fi

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/scpi_parser

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
