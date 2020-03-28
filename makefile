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

# Current directory to fetch jpeglib
current_dir = $(shell pwd)

# Precompiled libraries to link in:
LDLIBS  = -L/usr/lib/openmpi/lib -L/$(current_dir)/jpeg-6b/.libs/ -lm -lmpi -ljpeg
# Included H files needed during compiling:
INCLUDE = -ITools -I/usr/lib/openmpi/include -Ijpeg-6b

.PHONY: clean Prac3 run
all:    clean Prac3 run

clean:
	rm -f -r $(PROG) $(OBJS)

Prac3:
	$(CC) $(INCLUDE) -c Prac3.cpp -o obj/Prac3.o
	$(CC) $(INCLUDE) -c Tools/JPEG.cpp -o obj/JPEG.o
	$(CC) $(INCLUDE) -c Tools/Timer.cpp -o obj/Timer.o
	$(CC) -o bin/Prac3 obj/Prac3.o obj/JPEG.o obj/Timer.o $(LDLIBS)

Sequential:
	$(CC) $(INCLUDE) -c Sequential_Filter.cpp -o obj/Sequential_Filter.o
	$(CC) $(INCLUDE) -c Tools/JPEG.cpp -o obj/JPEG.o
	$(CC) $(INCLUDE) -c Tools/Timer.cpp -o obj/Timer.o
	$(CC) -o bin/Sequential_Filter obj/Sequential_Filter.o obj/JPEG.o obj/Timer.o $(LDLIBS)

# you can type "make run" to execute the program in this case with a default of 5 nodes
run:
	mpirun -np 5 bin/Prac3

GP = 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20

NUMBERS = 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 25 \
					30 35 40 45 50 \
					55 60 65 70 \
					75 80 85 90 95 \
					100

loop:
	@for procs in ${NUMBERS};                 \
	do                                        \
		for i in ${GP};                					\
		do                                     	\
			mpirun -np $${procs} bin/Prac3;				\
		done;                                  	\
		echo $${procs};													\
	done

run_serial:
	bin/Sequential_Filter

doxy: Prac3.cpp
	doxygen Prac3.doxy
