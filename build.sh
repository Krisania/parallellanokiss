#!/bin/bash

set -e

ESDK=${EPIPHANY_HOME}
ELIBS="-L ${ESDK}/tools/host/lib"
EINCS="-I ${ESDK}/tools/host/include"
ELDF=${ESDK}/bsps/current/fast.ldf

SCRIPT=$(readlink -f "$0")
EXEPATH=$(dirname "$SCRIPT")
cd $EXEPATH

# Create the binaries directory
mkdir -p bin/

CROSS_PREFIX=
case $(uname -p) in
	arm*)
		# Use native arm compiler (no cross prefix)
		CROSS_PREFIX=
		;;
	   *)
		# Use cross compiler
		CROSS_PREFIX="arm-linux-gnueabihf-"
		;;
esac

# Build HOST side application
${CROSS_PREFIX}g++ src/main.cpp -o bin/main.elf ${EINCS} ${ELIBS} -le-hal -le-loader -lpthread

# Build DEVICE side program
e-g++ -O3  -T ${ELDF} src/e_task.cpp -o bin/e_task.elf -le-lib -lm -ffast-math -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-default-inline

# Convert ebinary to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_task.elf bin/e_task.srec

# Build DEVICE2 side program
e-g++ -O3  -T ${ELDF} src/e_huf_task.cpp -o bin/e_huf_task.elf -le-lib -lm -ffast-math -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-default-inline

# Convert ebinary2 to SREC file
e-objcopy --srec-forceS3 --output-target srec bin/e_huf_task.elf bin/e_huf_task.srec