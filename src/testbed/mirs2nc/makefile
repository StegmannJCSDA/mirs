SHELL=/bin/sh

paths_path = ../../../setup
include $(paths_path)/paths

NAME=mirs2nc

OBJS=   $(NAME).o mirs2nc_class.o rmirs_wnetcdf.o memallocedr.o memallocdep.o rmirs_dep.o rmirs_edr.o \
	set_nc_snd.o gnrt_nc_snd.o wrt_nc_snd.o openfs_sw.o utc2hms.o jd2md.o leapyear.o \
	set_nc_img.o gnrt_nc_img.o wrt_nc_img.o wrt_nc_gdata.o defdim.o

#SRCS = ${OBJS:.o=.c}
SRCS = ${OBJS: %.o : %.cpp %.h}

INCLUDES = -I$(NETCDF4INC) -I${HDF5INC} -I${SZIPINC} -I${OMPINC}
LIBS    =  -lz -lm -L$(NETCDF4LIB) -lnetcdf -L${HDF5LIB} -lhdf5_hl -lhdf5 -L${SZIPLIB} -lsz -L${OMPLIB} -lgomp

FC_FLAG = 
LDFLAGS = 

EXE = $(exec_path)/$(NAME)

all: $(EXE)

$(EXE):	$(OBJS)
	$(CXX)  -o $(@) $(OBJS) $(LIBS)

#.c.o:
.cpp.o:
	${CXX} -mcmodel=large -Wall -Wextra -std=c++11 ${CXX_FLAGS} $(INCLUDES) -c  $<

clean:
	-rm -f $(OBJS) ${EXE}
