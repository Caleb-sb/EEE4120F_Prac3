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


/** \mainpage Sequential_Filter Main Page
 *
 * \section intro_sec Introduction
 *
 * The purpose of Prac3 is to learn some basics of MPI coding.
 *
 * This is the sequential version of the filter for speedup comparison.
 */

//---------- STUDENT NUMBERS --------------------------------------------------
//
// Please note:  BRDCAL003, DPRMIC007
//
//-----------------------------------------------------------------------------

/* Note that Doxygen comments are used in this file. */
/** \file Sequential_Filter
 *  Sequential_Filter - Serial verion of the filter
 */

// Includes needed for the program
#include "Prac3.h"
using namespace std;

/** This is the entry point to the program. */
int main(){
 int myid;

 // Read the input image
 if(!Input.Read("Data/baby.jpg")){
  printf("Cannot read image\n");
  return 0;
 }

 // Allocated RAM for the output image
 if(!Output.Allocate(Input.Width, Input.Height, Input.Components)) return 0;

 // This is sequential code to median filter -----------------------------------
 printf("Start of serial code...\n");
 int window_y = WINDOW_Y;
 int window_x = WINDOW_X;
 int divisor  = window_y*window_x;
 int window [3][divisor];
 int counter  = 0;
 int colcheck =0;
 // Repeated 10 times for getting average time
for(int j = 0; j < 10; j++){
  tic();
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
  printf("Time = %lg ms\n", (double)toc()/1e-3);
}
 printf("End of serial code...\n\n");
 // End of serial---------------------------------------------------------------

 // Write the output image
 if(!Output.Write("Data/Output.jpg")){
  printf("Cannot write image\n");
  return 0;
 }
 return 0;
}
//------------------------------------------------------------------------------
