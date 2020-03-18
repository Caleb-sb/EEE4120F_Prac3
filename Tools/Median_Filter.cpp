// EEE4120F - Prac 3 - Median_Filter.cpp
// DPRMIC007 & BRDCAL003

int[][] median_filter_func (int input[][],int xSize, int ySize, int windowSizeX, int windowSizeY, int boundaryCond)
{
  // boundaryCond: 0 = ignore(without cropping), 1 = ignore(with cropping). 2 = wrap, 3 = shrink window

  // Setting up output array
  if(boundaryCond == 1)
  {
    inputSizeX = xSize - windowSizeX;
    inputSizeY = ySize- windowSizeY;
  } else
  {
    inputSizeX = xSize;
    inputSizeY = ySize;
  }

  int output [inputSizeX][inputSizeY];


  for(int y = 0; y < inputSizeY; y ++)
  {
    for(int x = 0; x < inputSizeX; x ++)
    {
      int tempList[windowSizeY*windowSizeX]; // temp list to hold matrix items
      int counter = 0; // counter used to itterate through tempList
      // now add matrix items to list:
        for(int insideY = 0; insideY < windowSizeY; insideY ++)
        {
          for(int insideX = 0; insideX < windowSizeX; insideX ++)
          {
            tempList[counter] = input[y + insideY][x + insideX];
            counter ++ ;
          }
        }
        //input[y][x] =
    }
  }


}
