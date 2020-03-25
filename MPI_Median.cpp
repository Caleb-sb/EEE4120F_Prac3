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
void Master ()
{
  //! <h3>Local vars</h3>
  // The above outputs a heading to doxygen function entry

  // Read the input image

  if(!Input.Read("Data/baby.jpg"))
  {
    printf("Cannot read image\n");
    return;
  }

// Allocated RAM for the output image
  if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

// Splitting up the processing as evenly as possible
  int mod_check = Input.Height % (numprocs-1);
  int y_portions[numprocs-1];          //! y_portions: Rows to send to each slave

  for (int i = 0; i<numprocs-1; i++)
  {
    if (mod_check > 0)
    {

      if(i==(numprocs-2) || i ==0)
      {
        y_portions[i] = int(Input.Height/(numprocs-1)) + 1 + (int)WINDOW_Y/2;
      }
      else
        y_portions[i] = int(Input.Height/(numprocs-1)) + 1 + 2*((int)WINDOW_Y/2);

      mod_check--;
    }
    else
    {
      if (i==(numprocs-2) || i ==0)
      {
        y_portions[i] = int(Input.Height/(numprocs-1)) + (int)WINDOW_Y/2;
      }
      else
        y_portions[i] = int(Input.Height/(numprocs-1)) + 2*((int)WINDOW_Y/2);
    }
  }

  MPI_Status stat; //! stat: Status of the MPI application
  boolean ack = false;
  //Telling slaves what to expect
  int  j;             //! j: Loop counter
  int largest = 0;
  for(j = 1; j < numprocs; j++)
  {
    int dimensions[DIMS] = {y_portions[j-1] , Input.Width*Input.Components, Input.Components};
    if (y_portions[j-1] > largest){
        largest = y_portions[j-1];
    }
    MPI_Send(dimensions, DIMS, MPI_INT, j, TAG, MPI_COMM_WORLD);
    MPI_Recv(dimensions, DIMS, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat);
    //printf("Process %d received a height of %d and length of %d pixels\n", j, dimensions[0], dimensions[1]/Input.Components);
  }
  printf("\n");


  int indexer = 0;

  for (int proc = 0; proc < numprocs-1; proc++)
  {
    unsigned char slave [y_portions[proc]][Input.Width*Input.Components];
    for(int j = 0; j < y_portions[proc]; j++)
    {

      if(proc > 0 && j ==0)
        indexer = indexer + y_portions[proc-1] - 2*(WINDOW_Y/2);//TODO: If the last j is unassigned make it -1
      for (int i =0; i < Input.Width*Input.Components; i++)
        slave[j][i] = Input.Rows[j+indexer][i];

    }


    int size = sizeof(unsigned char)*y_portions[proc]*Input.Width*Input.Components;
    MPI_Send(slave, size, MPI_CHAR, proc+1, TAG, MPI_COMM_WORLD);
    MPI_Recv(&ack, 1, MPI_CHAR, proc+1, TAG, MPI_COMM_WORLD, &stat);
    //if (ack)
      //printf("Data partition %d successful\n", proc+1);
  }
  // Last array not working
  // Size size chnaged, getting the values back might be an issue
  // to output
  indexer= 0;
  int flag =0;
  boolean done =false;
  tic();

  for (j=1; j<numprocs; j++)
  {
    unsigned char slave [y_portions[j-1]][Input.Width*Input.Components];
    // This is blocking: normally one would use MPI_Iprobe, with MPI_ANY_SOURCE,
    MPI_Recv(slave, y_portions[j-1]*Input.Width*Input.Components, MPI_CHAR, j, TAG, MPI_COMM_WORLD, &stat);
    //printf("%d per cent complete\n", (j)*100/(numprocs-1));
/*
    for(int y = 0; y < y_portions[j-1]; y++)
    {
      if (j > 1 && y ==0)
        indexer += y_portions[j-2];

      for(int x = 0; x < Input.Width*Input.Components; x++)
        Output.Rows[y+indexer][x] = slave[y][x];
    }
*/
    if (j==1)
    {
      for(int y = 0; y < y_portions[j-1]- (int)WINDOW_Y/2; y++)
      {
        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y][x] = slave[y][x];
      }
    }
    else if(j==numprocs-1)
    {
      for(int y = (int)WINDOW_Y/2; y < y_portions[j-1]; y++)
      {
        if (j > 1 && y == (int)WINDOW_Y/2)
          indexer += y_portions[j-2]-WINDOW_Y+WINDOW_Y/2;

        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y+indexer][x] = slave[y][x];


      }
    }
    else{
      for(int y = (int)WINDOW_Y/2; y < y_portions[j-1]- (int)WINDOW_Y/2; y++)
      {
        if (j > 1 && y ==(int)WINDOW_Y/2)
          indexer += y_portions[j-2]-WINDOW_Y+WINDOW_Y/2;

        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y+indexer][x] = slave[y][x];
      }
    }
  }
  //printf("Time = %lg ms\n", (double)toc()/1e-3);
  printf("%lg\n", (double)toc()/1e-3);
  //printf("Writing JPEG...\n\n");


  // Write the output image
  if(!Output.Write("Data/Output.jpg"))
  {
    printf("Cannot write image\n");
    return;
  }
//! <h3>Output</h3> The file Output.jpg will be created on success to save
//! the processed output.
}
//------------------------------------------------------------------------------

/** This is the Slave function, the workers of this MPI application. */
void Slave(int ID)
{
  int winSize = 7; // median filter window size
  char idstr[32];
  int dimensions[winSize];
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
  int window_y = WINDOW_Y;
  int window_x = WINDOW_X;
  int divisor  = window_y*window_x;
  int window [dimensions[2]][divisor];
  int counter  = 0;
  int colcheck =0;
  int x, y;

  for(y = 0; y < dimensions[0]; y++)
  {

    for(x = 0; x < dimensions[1]; x=x+dimensions[2])
    {

      int sum = 0;
      for(int j = -int(window_y/2); j<int(window_y/2)+1; j++)
      {
        for(int i = -int(window_x/2)*dimensions[2]; i<int(window_x/2)*dimensions[2]+1*dimensions[2]; i=i+dimensions[2])
        {
          for (int colour = 0; colour < dimensions[2]; colour++)
          {
            // Checking if window is at edge (top or left)
            if (y+j > -1 && (x+i+colour)>-1 && (x+i+colour) < dimensions[1] && y+j < dimensions[0])
            {
              window[colour][counter] = input_arr[y+j][x+i+colour];
            }
            else if (y+j < 0)
            {
              if ((x+i+colour)<0)
              {
                window[colour][counter] = input_arr[y+j+window_y][x+i+colour+window_x];
              }
              else
                window[colour][counter] = input_arr[y+j+window_y][x+i+colour];
            }
            else if (y+j > -1 && (x+i+colour)<0)
            {
              window[colour][counter] = input_arr[y+j][x+i+colour+window_x];
            }

            // Checking if window is at edge (bottom or right)
            else if (y+j >= dimensions[0])
            {
              if ((x+i+colour) >= dimensions[1])
              {
                window[colour][counter] = input_arr[y+j-window_y][x+i+colour-window_x];
              }
              else
                window[colour][counter] = input_arr[y+j-window_y][x+i+colour];
            }
            else if (y+j < dimensions[0] && (x+i+colour) >= dimensions[1])
            {
              window[colour][counter] = input_arr[y+j][x+i+colour-window_x];
            }

          }
          counter++;
        }
      }
      for (int colour = 0; colour < dimensions[2]; colour++)
        sort(window[colour], window[colour]+sizeof(window[colour])/sizeof(window[colour][0]));

      for (int colour = 0; colour < dimensions[2]; colour++)
        out_arr[y][x+colour] = window[colour][int(counter/2)];


      counter =0;
      //printf("Help me I want to die\n");
      //  hahahahaha, i felt the same coding this shit
    }
  }
  MPI_Send(out_arr, dimensions[0]*dimensions[1], MPI_CHAR, 0, TAG, MPI_COMM_WORLD);
}
//------------------------------------------------------------------------------

/** This is the entry point to the program. */
int main(int argc, char** argv)
{
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
