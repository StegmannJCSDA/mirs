#!/bin/bash

# This is an easy script to invoke CMake.

# Requirements:
# 1. cmake (conda install cmake)

# HOW TO BUILD MIRS:
# 1. Create a build directory. This is where MiRS will be built.
# 2. Switch to the build directory and run this script.
# 3. Build MiRS using 'make'.
#    You can parallelize the build using "make -j {num_cores}" (e.g. make -j 24).
#    You can turn on debug logging by setting the VERBOSE environment variable (e.g. VERBOSE=1 make).
# 4. Make sure that MiRS built correctly by running "make test".
# 5. Install MiRS to the installation directory using "make install".

# Set a few defaults here:

# The script automatically detects whether you have ccmake installed in addition to cmake.
#CMAKE=/data/data411/pub/ryan.honeyager/usr/anaconda3/bin/cmake
#CCMAKE=/data/data411/pub/ryan.honeyager/usr/anaconda3/bin/ccmake

echo -e "Welcome to the MiRS installer.\n\n\n"

CMAKE_D=`which cmake`
if [ -e "${CMAKE_D}" ]; then
	CMAKE=$CMAKE_D
fi
echo -e "Where can CMake be found? [Default is ${CMAKE}]: "
read CMAKE_NEW
if [ -n "$CMAKE_NEW" ]; then
	CMAKE=$CMAKE_NEW
fi

if [ ! -e "${CMAKE}" ]; then
	echo CMake cannot be found. Terminating.
	exit 1
fi

CMAKE_BUILD_TYPE_DEFAULT=Release
echo -e "What is the build type? This turns on/off debugging. Allowed options: Release, Debug. [Default is ${CMAKE_BUILD_TYPE_DEFAULT}]: "
read CMAKE_BUILD_TYPE
if [ -z "$CMAKE_BUILD_TYPE" ]; then
	CMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE_DEFAULT
fi
echo -e "Using build type $CMAKE_BUILD_TYPE."

CMAKE_INSTALL_PREFIX_DEFAULT=`pwd`/install
echo -e "\nWhere will the MiRS installation be located [Default is ${CMAKE_INSTALL_PREFIX_DEFAULT}]: "
read CMAKE_INSTALL_PREFIX
if [ -z "$CMAKE_INSTALL_PREFIX" ]; then
	CMAKE_INSTALL_PREFIX=$CMAKE_INSTALL_PREFIX_DEFAULT
fi
echo -e "Installing to: $CMAKE_INSTALL_PREFIX"



echo -e "\n\n\nWhere are your compilers located?\n"

if [ -z "$CC" ]; then
	CC=`which gcc`
fi
echo -e "\nC Compiler [$CC]: "
read NEWCC
if [ -n "$NEWCC" ]; then
	CC=$NEWCC
fi
echo "CC is `which $CC`"

if [ -z "$CXX" ]; then
	CXX=`which g++`
fi
echo -e "\n\nC++ Compiler [$CXX]: "
read NEWCXX
if [ -n "$NEWCXX" ]; then
	CXX=$NEWCXX
fi
echo "CXX is `which $CXX`"

if [ -z "$FC" ]; then
	FC=`which gfortran`
fi
echo -e "\n\nFortran Compiler [$FC]: "
read NEWF
if [ -n "$NEWF" ]; then
	FC=$NEWF
fi
echo "FC is `which $FC`"


echo -e "\n\n\nDo you have any hints regarding where various libraries are located?\nThese are all optional.\n"

# Find HDF5
HDF5_HINT=/usr/local/other/wrfDeps/Intel-2021.4.0/
HOST=`hostname -f`
if [ "$HOST" == "rhw1134.star1.nesdis.noaa.gov" ]; then
    HDF5_HINT=/data/starfs1_rw/libs/hdf5-1.8.13
fi
echo -e "\nHDF5 [$HDF5_HINT]: "
read HDF5_ROOT
if [ -z "$HDF5_ROOT" ]; then
	HDF5_ROOT=$HDF5_HINT
fi

# Find NetCDF
NETCDF4_HINT=/usr/local/other/wrfDeps/Intel-2021.4.0/
if [ "$HOST" == "rhw1134.star1.nesdis.noaa.gov" ]; then
    NETCDF4_HINT=/data/starfs1_rw/libs/netcdf-4.3.2
fi
echo -e "\nNetCDF 4 [${NETCDF4_HINT}]: "
read NETCDF4_ROOT
if [ -z "$NETCDF4_ROOT" ]; then
	NETCDF4_ROOT=$NETCDF4_HINT
fi

# Find SZIP and set SZIP_INSTALL
SZIP_HINT=/data/starfs1/libs/szip-2.1
echo -e "\nSZIP [$SZIP_HINT]: "
read SZIP_ROOT
if [ -z "$SZIP_ROOT" ]; then
	SZIP_ROOT=$SZIP_HINT
fi

# Find JPEG and set JPEG_INSTALL
JPEG_HINT=/usr
echo -e "\nJPEG [$JPEG_HINT]: "
read JPEG_ROOT
if [ -z "$JPEG_ROOT" ]; then
	JPEG_ROOT=$JPEG_HINT
fi

# Find ZLIB
ZLIB_HINT=/gpfsm/dswdev/jcsda/spack-stack/scu17/spack-stack-1.9.0/envs/ue-intel-2021.10.0/install/intel/2021.10.0/zlib-ng-2.2.1-f37hdv4/
echo -e "\nZLIB [$ZLIB_HINT]: "
read ZLIB_ROOT
if [ -z "$ZLIB_ROOT" ]; then
	ZLIB_ROOT=$ZLIB_HINT
fi

# Find HDF4
HDF4_HINT=/gpfsm/dswdev/jcsda/spack-stack/scu17/spack-stack-1.9.0/envs/ue-intel-2021.10.0/install/intel/2021.10.0/hdf-4.2.15-lvzjl7h/
echo -e "\nHDF4 [$HDF4_HINT]: "
read HDF4_ROOT
if [ -z "$HDF4_ROOT" ]; then
	HDF4_ROOT=$HDF4_HINT
fi

# Find HDF-EOS
HDFEOS_HINT=/gpfsm/dswdev/jcsda/spack-stack/scu17/spack-stack-1.9.0/envs/ue-intel-2021.10.0/install/intel/2021.10.0/hdf-eos2-2.20v1.00-2hussws/
echo -e "\nHDF-EOS [$HDFEOS_HINT]: "
read HDFEOS_ROOT
if [ -z "$HDFEOS_ROOT" ]; then
	HDFEOS_ROOT=$HDFEOS_HINT
fi




scriptdir=`dirname "$BASH_SOURCE"`


echo -e "\n\n\nInvoking CMake\n"

export NETCDF4_ROOT=$NETCDF4_ROOT
export HDFEOS_ROOT=$HDFEOS_ROOT
export HDFEOS2_ROOT=$HDFEOS_ROOT
export HDF4_ROOT=$HDF4_ROOT
#export ZLIB_ROOT=$ZLIB_ROOT
export SZIP_ROOT=$SZIP_ROOT

$CMAKE \
	-DHDF5_FIND_DEBUG=true -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
	-DHDF5_ROOT=$HDF5_ROOT -DZLIB_ROOT=$ZLIB_ROOT \
	-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} \
	$scriptdir


#echo -e "\n\nCMake step is complete. Do you want a GUI to set more options (no,yes) [no]?"
#read USECCMAKE
#if [ "$USECCMAKE" == "yes" ]; then
#	CCMAKE_D=`which ccmake`
#	if [ -e "${CCMAKE_D}" ]; then
#		CCMAKE=$CCMAKE_D
#	fi
#	echo -e "Where can ccmake be found? [Default is ${CCMAKE}]: "
#	read CCMAKE_NEW
#	if [ -n "$CCMAKE_NEW" ]; then
#		CCMAKE=$CCMAKE_NEW
#	fi
#
#	if [ -e "${CCMAKE}" ]; then
#		$CCMAKE $scriptdir
#	fi
#	if [ ! -e "${CCMAKE}" ]; then
#		echo ccmake cannot be found.
#	fi
#fi


echo -e "\n\nIf the configuration succeeded, then you may now build MiRS with the \"make\" command."
echo -e "After this, run \"make install\" to install it to the installation directory."

