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
CC=mpicc
CCC=mpic++
CXX=mpic++
FC=mpif90
AS=as

# Macros
CND_PLATFORM=OpenMPI-Linux-x86
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
	${OBJECTDIR}/HashAlgos/hashing_algo.o \
	${OBJECTDIR}/HashAlgos/md5.o \
	${OBJECTDIR}/HashAlgos/sha1.o \
	${OBJECTDIR}/HashAlgos/sha1_prop.o \
	${OBJECTDIR}/HashAlgos/sha2.o \
	${OBJECTDIR}/HashAlgos/sha256.o \
	${OBJECTDIR}/HashAlgos/sph_md5.o \
	${OBJECTDIR}/attack_types.o \
	${OBJECTDIR}/bruteforce.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/openmp.o \
	${OBJECTDIR}/password_algo.o \
	${OBJECTDIR}/pb_client.o \
	${OBJECTDIR}/pb_server.o \
	${OBJECTDIR}/sds.o \
	${OBJECTDIR}/utils.o


# C Compiler Flags
CFLAGS=-m64 -fopenmp -Ofast -Wextra

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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/parallelbruteforce

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/parallelbruteforce: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/parallelbruteforce ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/HashAlgos/hashing_algo.o: HashAlgos/hashing_algo.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/hashing_algo.o HashAlgos/hashing_algo.c

${OBJECTDIR}/HashAlgos/md5.o: HashAlgos/md5.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/md5.o HashAlgos/md5.c

${OBJECTDIR}/HashAlgos/sha1.o: HashAlgos/sha1.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/sha1.o HashAlgos/sha1.c

${OBJECTDIR}/HashAlgos/sha1_prop.o: HashAlgos/sha1_prop.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/sha1_prop.o HashAlgos/sha1_prop.c

${OBJECTDIR}/HashAlgos/sha2.o: HashAlgos/sha2.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/sha2.o HashAlgos/sha2.c

${OBJECTDIR}/HashAlgos/sha256.o: HashAlgos/sha256.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/sha256.o HashAlgos/sha256.c

${OBJECTDIR}/HashAlgos/sph_md5.o: HashAlgos/sph_md5.c 
	${MKDIR} -p ${OBJECTDIR}/HashAlgos
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashAlgos/sph_md5.o HashAlgos/sph_md5.c

${OBJECTDIR}/attack_types.o: attack_types.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/attack_types.o attack_types.c

${OBJECTDIR}/bruteforce.o: bruteforce.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/bruteforce.o bruteforce.c

${OBJECTDIR}/main.o: main.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.c

${OBJECTDIR}/openmp.o: openmp.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/openmp.o openmp.c

${OBJECTDIR}/password_algo.o: password_algo.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/password_algo.o password_algo.c

${OBJECTDIR}/pb_client.o: pb_client.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/pb_client.o pb_client.c

${OBJECTDIR}/pb_server.o: pb_server.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/pb_server.o pb_server.c

${OBJECTDIR}/sds.o: sds.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sds.o sds.c

${OBJECTDIR}/utils.o: utils.c 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.c) -std=c99 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/utils.o utils.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/parallelbruteforce

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
