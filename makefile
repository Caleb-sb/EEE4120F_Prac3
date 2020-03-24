# This Makefile requires GNU make, which is called gmake on Solaris systems
#
# 'make'        : builds and runs the project
# 'make clean'  : remove build products

# where the executable program binary is placed:
PROG = bin/*
# Where the object files are placed:
OBJS = obj/*

# Which compiler to use, note for MPI a special purpose compiler is used
CC = mpic++

# Precompiled libraries to link in:
LDLIBS  = -L/usr/lib/openmpi/lib -L/home/mikedupi/Documents/EEE4120F/Prac3/jpeg-6b/.libs/ -lm -lmpi -ljpeg
# Included H files needed during compiling:
INCLUDE = -ITools -I/usr/lib/openmpi/include -Ijpeg-6b

.PHONY: clean Prac3 run
all:    clean Prac3 run

clean:
	rm -f -r $(PROG) $(OBJS)

Prac3:
	$(CC) $(INCLUDE) -c Prac3.cpp -o obj/Prac3.o
	$(CC) $(INCLUDE) -c MPI_Median.cpp -o obj/MPI_Median.o
	$(CC) $(INCLUDE) -c Tools/JPEG.cpp -o obj/JPEG.o
	$(CC) $(INCLUDE) -c Tools/Timer.cpp -o obj/Timer.o
	$(CC) -o bin/MPI_Median obj/MPI_Median.o obj/JPEG.o obj/Timer.o $(LDLIBS)

# you can type "make run" to execute the program in this case with a default of 5 nodes
#	mpirun -np 5 bin/MPI_Median
run:
		for i in 1 2 3 4 5 6 7 8 9 10; do \
			mpirun -np 3 bin/MPI_Median; \
		done


doxy: Prac3.cpp
	doxygen Prac3.doxy
