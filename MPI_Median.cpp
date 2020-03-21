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

 // Read the input image
 if(!Input.Read("Data/greatwall.jpg")){
  printf("Cannot read image\n");
  return;
 }

 // Allocated RAM for the output image
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

 //Splitting up the processing as evenly as possible
 int mod_check = Input.Height % (numprocs-1);
 int y_portions[numprocs-1];          //! y_portions: Rows to send to each slave

 for (int i = 0; i<numprocs-1; i++){
    if (mod_check > 0){
      y_portions[i] = int(Input.Height/(numprocs-1)) + 1;
      mod_check--;
    }
    else y_portions[i] = int(Input.Height/(numprocs-1));
 }

//unsigned char *buffer = (unsigned char*) malloc((numprocs-1)*y_portions[0]*(Input.Width*Input.Components)*sizeof(unsigned char));
 // Using pointers to use malloc (Array is too large)
 //unsigned char ***pointy = (unsigned char***)malloc((numprocs-1)*sizeof(unsigned char**));
 //unsigned char **row = (unsigned char**)malloc((y_portions[0])* sizeof(unsigned char*));
 //unsigned char *cols = (unsigned char*)malloc((Input.Width*Input.Components)*sizeof(unsigned char));

 /*// Nested for loops to assign memory and values (won't fit on stack)
 int indexer = 0;   //! indexer: Sets the j to appropriate value for splitting
 for (int proc = 0; proc < numprocs-1; proc++){
     if(proc > 0){
       indexer = indexer + y_portions[proc-1]; //TODO: If the last j is unassigned make it -1
     }
     copy(Input.Rows[0][0], Input.Rows[indexer][0], pointy[proc]);
   }
*/

 MPI_Status stat; //! stat: Status of the MPI application
 boolean ack = false;
 //Telling slaves what to expect
 int  j;             //! j: Loop counter
 for(j = 1; j < numprocs; j++) {
  int dimensions[3] = {y_portions[j-1] , Input.Width*Input.Components, Input.Components};
  MPI_Send(dimensions, 3, MPI_INT, j, TAG, MPI_COMM_WORLD);
  MPI_Recv(dimensions, 3, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
  printf("Process %d received a height of %d and length of %d pixels (RGB)\n", j, dimensions[0], dimensions[1]/3);
 }
 printf("\n");

 unsigned char slave [y_portions[0]][Input.Width*Input.Components];

 int indexer = 0;
 for (int proc = 0; proc < numprocs-1; proc++){
   for(int j = 0; j < y_portions[proc]; j++){
     if(proc > 0 && j ==0){
       indexer = indexer + y_portions[proc-1]; //TODO: If the last j is unassigned make it -1
     }
     for (int i =0; i < Input.Width*Input.Components; i++){
       slave[j][i] = Input.Rows[j+indexer][i];
     }
   }

   //TODO: Send to slave
   int size = sizeof(unsigned char)*y_portions[proc]*Input.Width*Input.Components;
   MPI_Send(slave, size, MPI_CHAR, proc+1, TAG, MPI_COMM_WORLD);
   MPI_Recv(&ack, 1, MPI_CHAR, proc+1, TAG, MPI_COMM_WORLD, &stat);
   if (ack){
     printf("Data partition %d successful\n", proc+1);
   }

 }
 //printf("Waiting for slavery to be abolished...\n");
 indexer= 0;
 int flag =0;
 boolean done =false;
 int counter = 0;
 for (j=1; j<numprocs; j++) {
  // This is blocking: normally one would use MPI_Iprobe, with MPI_ANY_SOURCE,
  counter =0;
  MPI_Recv(slave, y_portions[j-1]*Input.Width*Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);

  printf("%d per cent complete\n", (j)*100/(numprocs-1));
  for(int y = 0; y < y_portions[j-1]; y++){
    if (j > 1 && y ==0){
      indexer += y_portions[j-2];
      printf("%d\n", indexer);
    }
    for(int x = 0; x < Input.Width*Input.Components; x++){

      Output.Rows[y+indexer][x] = slave[y][x];
      counter++;
    }
  }
  printf("%d\n", counter);
}

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
 int dimensions[3];
 char buff[BUFSIZE];
 MPI_Status stat;
 unsigned char ack =0;


 // receive from rank 0 (master):
 // This is a blocking receive, which is typical for slaves.
 MPI_Recv(dimensions, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
 unsigned char input_arr[dimensions[0]][dimensions[1]];
 unsigned char out_arr[dimensions[0]][dimensions[1]];
 MPI_Send(dimensions, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD);

 //Do these return? For use with ACK/NACK
 MPI_Recv(input_arr, dimensions[0]*dimensions[1], MPI_CHAR, 0, TAG, MPI_COMM_WORLD, &stat);
 ack = 1;
 MPI_Send(&ack, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD);


  // Start of Distributed Median
 int window_y = 3;
 int window_x = 3;
 int divisor  = window_y*window_x;
 int window [3][divisor];
 int counter  = 0;
 int colcheck =0;

 int x, y;



 for(y = int(window_y/2); y < dimensions[0]-int(window_y/2); y++){
   //printf("Hello %d\n", ID);
   for(x = int(window_x/2)*dimensions[2]; x < dimensions[1]-int(window_x/2)*dimensions[2]; x=x+dimensions[2]){
     //printf("Hello %d\n", ID);
    int sum = 0;
    for(int j = -int(window_y/2); j<int(window_y/2)+1; j++){
      //printf("Hello %d\n", ID);
      for(int i = -int(window_x/2)*dimensions[2]; i<int(window_x/2)*dimensions[2]+1*dimensions[2]; i=i+dimensions[2]){
        //printf("Hello %d\n", ID);
        window[0][counter] = input_arr[y+j][x+i];
        window[1][counter] = input_arr[y+j][x+i+1];
        window[2][counter] = input_arr[y+j][x+i+2];
        counter++;
      }
    }
    sort(window[0], window[0]+sizeof(window[0])/sizeof(window[0][0]));
    sort(window[1], window[1]+sizeof(window[1])/sizeof(window[1][0]));
    sort(window[2], window[2]+sizeof(window[2])/sizeof(window[2][0]));
    out_arr[y][x] = window[0][int(counter/2)];
    out_arr[y][x+1] = window[1][int(counter/2)];
    out_arr[y][x+2] = window[2][int(counter/2)];
    counter =0;
    //printf("Help me I want to die\n");
   }
  }
  MPI_Send(out_arr, dimensions[0]*dimensions[1], MPI_CHAR, 0, TAG, MPI_COMM_WORLD);

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
