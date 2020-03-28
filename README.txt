# README
This is the MPI Practical 3 for EEE4120F class of 2020.

The practical should run using the libjpeg library in the jpeg-6b folder. If
this fails for some reason and libjpeg cannot be found, change the path to
where you have libjpeg v6b on your pc.

If cloned from github, execute the following in terminal:
  $ mkdir bin
  $ mkdir obj

To get information of the MPI implementation, navigate to /doxy/html and open
prac3_8cpp.html

To compile and run with a default of 5 processes and window radius of 3 execute
the following in terminal:
  $ make

To compile and run the sequential filter execute the following in terminal:
  $ make Sequential
  $ make run_serial

To loop for testing times at different numbers of processes, once the MPI
version is compiled, execute the following in terminal:
  $ make loop

Window size can be changed in Prac3.h
Number of processes can be changed in the Makefile
Picture used can be changed in Prac3.cpp
