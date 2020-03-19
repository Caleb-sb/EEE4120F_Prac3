//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//
// This is an adaptition of The "Hello World" example avaiable from
// https://en.wikipedia.org/wiki/Message_Passing_Interface#Example_program
//==============================================================================


/** \mainpage Prac3 Main Page
 *
 * \section intro_sec Introduction
 *
 * The purpose of Prac3 is to learn some basics of MPI coding.
 *
 * Look under the Files tab above to see documentation for particular files
 * in this project that have Doxygen comments.
 */



//---------- STUDENT NUMBERS --------------------------------------------------
//
// Please note:  BRDCAL003, DPRMIC007
//
//-----------------------------------------------------------------------------

/* Note that Doxygen comments are used in this file. */
/** \file Prac3
 *  Prac3 - MPI Main Module
 *  The purpose of this prac is to get a basic introduction to using
 *  the MPI libraries for prallel or cluster-based programming.
 */

// Includes needed for the program
#include "Prac3.h"
using namespace std;

/** This is the master node function, describing the operations
    that the master will be doing */
void Master () {
 //! <h3>Local vars</h3>
 // The above outputs a heading to doxygen function entry
 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return;
 }

 // Allocated RAM for the output image
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

 //Splitting up the processing as evenly as possible
 int mod_check = Input.Height % (numprocs-1);
 int y_portions[numprocs-1];          //! y_portion: Rows to send to each slave
 if (mod_check != 0){
   for (int i = 0; i<numprocs-1; i++){
     if (mod_check > 0){
       y_portion[i] = int(Input.Height/(numprocs-1)) + 1;
       mod_check--;
     }
     y_portion[i] = int(Input.Height/(numprocs-1))
     mod_check--;
   }
 }
 else y_portion = Height/(numprocs-1);

 int  j;             //! j: Loop counter
 char buff[BUFSIZE][BUFSIZE]; //! buff: Buffer for transferring message data
 MPI_Status stat;    //! stat: Status of the MPI application

 //TODO: Make a loop to fill up an array of 2D Arrays

 //TODO: Make a loop to send each 2D array to the slaves

 //TODO: Make a loop to receive all the values back from the slaves (Output)


 printf("0: We have %d processors\n", numprocs);
 for(j = 1; j < numprocs; j++) {
  MPI_Send(buff, BUFSIZE, MPI_CHAR, j, TAG, MPI_COMM_WORLD);
 }
 for(j = 1; j < numprocs; j++) {
  // This is blocking: normally one would use MPI_Iprobe, with MPI_ANY_SOURCE,
  // to check for messages, and only when there is a message, receive it
  // with MPI_Recv.  This would let the master receive messages from any
  // slave, instead of a specific one only.
  MPI_Recv(buff, BUFSIZE, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);
  printf("0: %s\n", buff);
 }
 // End of "Hello World" example................................................

 // Read the input image


 // This is example code of how to copy image files ----------------------------
 printf("Start of example code...\n");

 tic();

 printf("Time = %lg ms\n", (double)toc()/1e-3);
 //}
 printf("End of example code...\n\n");
 // End of example -------------------------------------------------------------

 // Write the output image
 if(!Output.Write("Data/Output.jpg")){
  printf("Cannot write image\n");
  return;
 }
 //! <h3>Output</h3> The file Output.jpg will be created on success to save
 //! the processed output.
}
//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID){

 char idstr[32];
 char buff [BUFSIZE][BUFSIZE];

 MPI_Status stat;

 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 MPI_Recv(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);

 strncat(buff, idstr, BUFSIZE-1);
 strncat(buff, "reporting for duty", BUFSIZE-1);

  //TODO: Create array to insert completed values into
  // Start of Distributed Median
  printf("Start of example code...\n");
  int window_y = 3;
  int window_x = 3;
  int divisor  = window_y*window_x;
  int window [3][divisor];
  int counter  = 0;
  int colcheck =0;
  int x, y;
  for(y = int(window_y/2); y < Input.Height-int(window_y/2); y++){

   for(x = int(window_x/2)*Input.Components; x < Input.Width*Input.Components-int(window_x/2)*Input.Components; x=x+3){

     int sum = 0;
     for(int j = -int(window_y/2); j<int(window_y/2)+1; j++){
       for(int i = -int(window_x/2)*Input.Components; i<int(window_x/2)*Input.Components+1*Input.Components; i=i+3){
         window[0][counter] = Input.Rows[y+j][x+i];
         window[1][counter] = Input.Rows[y+j][x+i+1];
         window[2][counter] = Input.Rows[y+j][x+i+2];
         counter++;
       }
     }
     sort(window[0], window[0]+sizeof(window[0])/sizeof(window[0][0]));
     sort(window[1], window[1]+sizeof(window[1])/sizeof(window[1][0]));
     sort(window[2], window[2]+sizeof(window[2])/sizeof(window[2][0]));
     Output.Rows[y][x] = window[0][int(counter/2)];
     Output.Rows[y][x+1] = window[1][int(counter/2)];
     Output.Rows[y][x+2] = window[2][int(counter/2)];
     counter =0;
   }
  }

  //TODO: Send the "output" array back to master
  // send to rank 0 (master):
  MPI_Send(buff, BUFSIZE, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
}
//------------------------------------------------------------------------------

/** This is the entry point to the program. */
int main(int argc, char** argv){
 int myid;

 // MPI programs start with MPI_Init
 MPI_Init(&argc, &argv);

 // find out how big the world is
 MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

 // and this processes' rank is
 MPI_Comm_rank(MPI_COMM_WORLD, &myid);

 // At this point, all programs are running equivalently, the rank
 // distinguishes the roles of the programs, with
 // rank 0 often used as the "master".
 if(myid == 0) Master();
 else          Slave (myid);

 // MPI programs end with MPI_Finalize
 MPI_Finalize();
 return 0;
}
//------------------------------------------------------------------------------
