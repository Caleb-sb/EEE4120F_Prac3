// EEE4120F - Prac 3 - Median_Filter.cpp
// DPRMIC007 & BRDCAL003

int [][] median_filter_func(int input[][], int windowSizeX, int windowSizeY, int boundaryCond)
{
  // boundaryCond: 0 = ignore(without cropping), 1 = ignore(with cropping). 2 = wrap, 3 = shrink window

  // Setting up output array
  if(boundaryCond == 1)
  {
    inputSizeX = (sizeof(input) / sizeof(input[0])) - windowSizeX;
    inputSizeY = (sizeof(input[0]) / sizeof(input[0][0])) - windowSizeY;
  } else
  {
    inputSizeX = sizeof(input) / sizeof(input[0]);
    inputSizeY = sizeof(input[0]) / sizeof(input[0][0]);
  }

  int output [inputSizeX][inputSizeY];





}
