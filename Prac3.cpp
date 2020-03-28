//==============================================================================
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
using namespace std; //for sort()

/** This is the master node function, describing the operations
that the master will be doing */
void Master ()
{
  //! Master function first reads the input image then partions it evenly
  //! according to its size and the number of processes. It sends the slave
  //! processes the size of the array expected, gets an ACK message
  //! and then sends the array to be processed (also receiving and ACK). It
  //! receives the processed arrays and
  //! reassembles them into an Output.jpg

  //! <h3>Input</h3> Input image read in from /Data folder
  // Read the input image
  if(!Input.Read("Data/baby.jpg"))
  {
    printf("Cannot read image\n");
    return;
  }

  //! <h3>Local Variables</h3>
  MPI_Status stat; //! stat: Status of the MPI application
  int mod_check = Input.Height % (numprocs-1);
  int y_portions[numprocs-1];        //! y_portions: Rows to send to each slave

 // Allocated RAM for the output image
  if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return;

  // Splitting up the processing as evenly as possible
  for (int i = 0; i<numprocs-1; i++)
  {
    if (mod_check > 0)
    {
      if(i==(numprocs-2) || i ==0)
      {
        y_portions[i] = int(Input.Height/(numprocs-1)) + 1 + (int)WINDOW_Y/2;
      }
      else
        y_portions[i] = int(Input.Height/(numprocs-1)) + 1 +2*((int)WINDOW_Y/2);
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

  boolean ack = false; //! ack: Slaves set true when array is received
  // Telling slaves what sizes to expect
  int largest = 0;
  for(int j = 1; j < numprocs; j++)
  {
    int dimensions[DIMS] = {y_portions[j-1] , Input.Width*Input.Components,
                                                              Input.Components};
    if (y_portions[j-1] > largest){
        largest = y_portions[j-1];
    }
    MPI_Send(dimensions, DIMS, MPI_INT, j, TAG, MPI_COMM_WORLD);
    MPI_Recv(dimensions, DIMS, MPI_INT, j, TAG, MPI_COMM_WORLD, &stat); // ACK
    //printf("Process %d received a height of %d and length of %d pixels\n", j,
      //                           dimensions[0], dimensions[1]/Input.Components);
  }
  //printf("\n");
  //! indexer: for aligning the Input and Output array with split arrays
  int indexer = 0;
  // Writing the portioned arrays and sending to respective slaves
  for (int proc = 0; proc < numprocs-1; proc++)
  {
    unsigned char slave [y_portions[proc]][Input.Width*Input.Components];
    for(int j = 0; j < y_portions[proc]; j++)
    {
      if(proc > 0 && j ==0)
        indexer = indexer + y_portions[proc-1] - 2*(WINDOW_Y/2);

      for (int i =0; i < Input.Width*Input.Components; i++)
        slave[j][i] = Input.Rows[j+indexer][i];
    }

    int size = sizeof(unsigned char)*y_portions[proc]*Input.Width
                                                              *Input.Components;

    MPI_Send(slave, size, MPI_CHAR, proc+1, TAG, MPI_COMM_WORLD); //send array
    MPI_Recv(&ack, 1, MPI_CHAR, proc+1, TAG, MPI_COMM_WORLD, &stat);// recv ACK

    //if (ack)
    //  printf("Data partition %d successful\n", proc+1);
  }

  indexer= 0; //For aligning output array with portioned arrays
  int flag =0;
  boolean done =false;
  //printf("\n");
  tic();
  //Waiting for slaves to finish and placing into output array
  for (int j=1; j<numprocs; j++)
  {
    unsigned char slave [y_portions[j-1]][Input.Width*Input.Components];
    // This is blocking: normally one would use MPI_Iprobe, with MPI_ANY_SOURCE,
    MPI_Recv(slave, y_portions[j-1]*Input.Width*Input.Components, MPI_CHAR, j,
                                                    TAG, MPI_COMM_WORLD, &stat);

    // The following checks eliminate black lines between slave arrays
    if (j==1)
    {

      for(int y = 0; y < y_portions[j-1]- (int)WINDOW_Y/2; y++)
      {
        //printf("Y is: %d,   %d\n", y, j);
        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y][x] = slave[y][x];
      }
    }
    else if(j==numprocs-1)
    {
      //printf("Indexer is: %d,   %d\n", indexer, j);
      for(int y = (int)WINDOW_Y/2; y < y_portions[j-1]; y++)
      {

        if (j > 1 && y == (int)WINDOW_Y/2)
          indexer += y_portions[j-2]-WINDOW_Y/2 -WINDOW_Y/2;

        //printf("Y is: %d,   %d\n", y+indexer, j);
        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y+indexer][x] = slave[y][x];
      }
    }
    else if(j == 2){
      for(int y = (int)WINDOW_Y/2; y < y_portions[j-1]- (int)WINDOW_Y/2; y++)
      {
        if (j > 1 && y ==(int)WINDOW_Y/2)
          indexer += y_portions[j-2]-WINDOW_Y/2-WINDOW_Y/2;

        //printf("Y is: %d,   %d\n", y+indexer, j);
        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y+indexer][x] = slave[y][x];
      }
    }
    else{
      //printf("%s\n", "Second");
      //printf("Indexer is: %d,   %d\n", indexer, j);
      for(int y = (int)WINDOW_Y/2; y < y_portions[j-1]- (int)WINDOW_Y/2; y++)
      {
        if (j > 1 && y ==(int)WINDOW_Y/2)
          indexer += y_portions[j-2]-WINDOW_Y/2-WINDOW_Y/2;

        //printf("Y is: %d,   %d\n", y+indexer, j);
        for(int x = 0; x < Input.Width*Input.Components; x++)
          Output.Rows[y+indexer][x] = slave[y][x];
      }
    }
  }
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
  //! <h3>Input</h3>Will first receive dimensions of incoming array, then the
  //! array itself
  //! <h3>Output</h3>Will first send ACKs for the dimensions and the received
  //! array and then send the filtered array portion to the Master
  char idstr[32];
  //! <h3>Local variables</h3>
  int dimensions[3]; //! dimensions[]: Used to inform slave
                     //! of incoming array dimensions
  MPI_Status stat; //! stat: Status of the MPI application
  unsigned char ack =0; //! ack: Tells master array was received successfully

  // receive from rank 0 (master):
  // This is a blocking receive, which is typical for slaves.
  MPI_Recv(dimensions, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat);
  unsigned char input_arr[dimensions[0]][dimensions[1]]; //! input_arr: array
                                                         //! for slave to
                                                         //! filter with median
                                                         //! function
  unsigned char out_arr[dimensions[0]][dimensions[1]]; //! out_arr: array
                                                       //! for slave to
                                                       //! send to master after
                                                       //! filtering

  MPI_Send(dimensions, 3, MPI_INT, 0, TAG, MPI_COMM_WORLD);

  MPI_Recv(input_arr, dimensions[0]*dimensions[1], MPI_CHAR, 0, TAG,
              MPI_COMM_WORLD, &stat);
  ack = 1;
  MPI_Send(&ack, 1, MPI_CHAR, 0, TAG, MPI_COMM_WORLD); // Tell master array
                                                       // received successfully

  // Start of Distributed Median
  int window_y = WINDOW_Y;  //! window_y: y-dimension of median window
  int window_x = WINDOW_X;  //! window_x: x-dimension of median window
  int elements  = window_y*window_x; //! elements: total num of items in window
  int window [dimensions[2]][elements]; //! window[][]: 2D for RGB
  int counter  = 0; //! counter: used to find columns of the same colour
                    //! to find median
  int x, y; //! x, y: iterating variables through array items
  //tic();
  // Performing the filter
  for(y = 0; y < dimensions[0]; y++)
  {
    for(x = 0; x < dimensions[1]; x=x+dimensions[2])
    {
      for(int j = -int(window_y/2); j<int(window_y/2)+1; j++)
      {
        for(int i = -int(window_x/2)*dimensions[2];
            i<int(window_x/2)*dimensions[2]+1*dimensions[2]; i=i+dimensions[2])
        {
          for (int colour = 0; colour < dimensions[2]; colour++)
          {
            // Checking if window is at edge (top or left)
            if (y+j > -1 && (x+i+colour)>-1 && (x+i+colour) < dimensions[1] &&
                                                            y+j < dimensions[0])
            {
              window[colour][counter] = input_arr[y+j][x+i+colour];
            }
            else if (y+j < 0)
            {
              if ((x+i+colour)<0)
              {
                window[colour][counter] = input_arr[y+j+window_y]
                                                          [x+i+colour+window_x];
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
                window[colour][counter] = input_arr[y+j-window_y]
                                                          [x+i+colour-window_x];
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
      //For each input.component, get the Median so image doesnt change colours
      for (int colour = 0; colour < dimensions[2]; colour++)
        sort(window[colour], window[colour]+sizeof(window[colour])/
                                                     sizeof(window[colour][0]));

      for (int colour = 0; colour < dimensions[2]; colour++)
        out_arr[y][x+colour] = window[colour][int(counter/2)];

      counter =0;
    }
  }
  //printf("%d:  %lg\n", ID, (double)toc()/1e-3);
  //Send back to Master
  MPI_Send(out_arr,dimensions[0]*dimensions[1], MPI_CHAR,0,TAG,MPI_COMM_WORLD);
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
