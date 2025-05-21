#==============================================================================
#
# Makefile for SFR
#
#==============================================================================

#-----------------------------------------------------------------------------
#                          -- Define macros --
#-----------------------------------------------------------------------------

#---Paths are read from a common file {used across the MIRS package}
paths_path = ../../../setup
include ${paths_path}/paths

MAIN_path     = .
CURR_path     = `pwd`
#---Default macros and rules {from CRTM}
include ${CRTM_path}/make.macros
#include ${CRTM_path}/make.rules
CFLAGS = -Wall -g 
FFLAGS = $(FC_FLGS_SFR)
CXXFLAGS = -Wall -g

# -------------
# This makefile
# -------------
MAKEFILE = makefile

# ----------------
# Executable 
# ----------------
EXE_FILE = ${exec_path}/sfr

# ------------
# Object files
# ------------
SRCS = sfr.c process.c sat_data.c avn_data.c prod_data.c \
	   fun_sat.c fun_tis.c mask_stype.c snowfall.c avn_add_wth.c \
	   sfr_intg.c rd_mhs.c rd_gmi.c cal_vars.c rd_ssmis.c fun_tis_nn.c \
		dsr_run.c fun_tis_xgb.c
	   
OBJS = ${SRCS:.c=.o}

SDIR = ./d_fun
SRCS0 = ${SDIR}/d_darrc.c ${SDIR}/d_jsec.c ${SDIR}/d_sys.c
OBJS0 = ${SRCS0:.c=.o}

SRCS2 = GET_EMISS_1DALG.f90 onelyr.f90
OBJS2 = ${SRCS2:.f90=.o}

INCS = -I${SDIR} -I${HDFEOSINC} -I${HDF4INC} -I${HDF5INC} -I${JPEGINC}

LIBS =  -L${HDFEOSLIB} -L${HDF4LIB} -L${HDF5LIB} -L${SZIPLIB} -L${JPEGLIB} \
		  -lhdf5_hl -lhdf5 -lhdfeos -lGctp -lmfhdf -ldf \
		  -ljpeg -lz -lm -lsz

#                          -- Define dependecies --
#------------------------------------------------------------------

# ---------------------
# Make based on OS type
# ---------------------

all:
#	@echo ${paths_path} 
#	@echo ${LINUX_FLAG}
#	@echo ${PROG_FOR_CMAIN_OPTS}
#	@echo "-------------------"

	@echo "OS type detected: "`uname -s` 
	@case `uname -s` in \
		"SunOS"  ) make -f ${MAKEFILE} the_program ${SUNOS_FLAGS}  ;; \
		"AIX"    ) make -f ${MAKEFILE} the_program ${AIX_FLAGS}    ;; \
		"IRIX64" ) make -f ${MAKEFILE} the_program ${IRIX64_FLAGS} ;; \
		"Linux"  ) make -f ${MAKEFILE} the_program ${LINUX_FLAGS}  ;; \
		*        ) echo "This system is not supported"             ;; \
	esac

# ---------------------
# Make the program
# ---------------------
the_program : ${EXE_FILE}

# ---------------
# Dependency list
# ---------------
#${SDIR}/.o: ${SDIR}/.c
#	echo ${CC} -o $@ -c $<

${SDIR}/%.o: ${SDIR}/%.c
	${CC} ${INCS} -o $@ -c $<

${OBJS}: ${SRCS}
	${CC} ${INCS} -c ${SRCS}

${OBJS2}: ${SRCS2}
	${FC} ${FC_FLGS_SFR} -c ${SRCS2}

${EXE_FILE}:	${OBJS} ${OBJS2} ${OBJS0}
	${FC} -o  ${@} ${OBJS} ${OBJS2} ${OBJS0} ${LIBS} $(PROG_FOR_CMAIN_OPTS)
#${EXE_FILE}:	${OBJS} ${OBJS2} ${OBJS0}
#	${FL} -o  ${@} ${OBJS} ${OBJS2} ${LIBS} -nofor_main
#${EXE_FILE}:	${OBJS} ${OBJS0}
#	${CC} -o ${@} ${OBJS} ${OBJS0} ${LIBS}

# --------
# Clean up
# --------
clean:
	${REMOVE} ${EXE_FILE} ${OBJS} ${OBJS2} ${OBJS0} *.mod




