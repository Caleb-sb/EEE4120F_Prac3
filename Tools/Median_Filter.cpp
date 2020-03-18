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

      if ((boundaryCond == 0 ) & ((x > (inputSizeX - windowSizeX)) || (y > (inputSizeY - windowSizeY))))
      { // approaching edge of boundary for ignore boundary case
        output[y][x] = input[y][x];
      } else
      {
        for(int insideY = 0; insideY < windowSizeY; insideY ++)
        {
          for(int insideX = 0; insideX < windowSizeX; insideX ++)
          {

            tempList[counter] = input[y + insideY][x + insideX];
            counter ++ ;
          }
        }
        // find median in that selection block - this method is using
        // top-left-corner. This means that the top left corner of a
        // block gets the median value of the block
        output[y][x] = find_median_func(tempList, counter);
      }
    }
  }



}

double find_median_func(int list[], int list_length)
{

  int temp; // temporary variable to hold int value when swapping

  // Sort array - using bubble sort algorithm
  for(int i = 0; i < list_length - 1; i ++)
  {
    for(int j = 0; j < list_length - 1 - i; j ++)
    {
      if( list[j] > list[j+1])
       {
         // swap the elements
         temp = list[j];
         list[j] = list[j+1];
         list[j+1] = temp;
       }
    }
  }
  // Now list is sorted
  if (list_length % 2 == 0) // if list length is even
  {
    // find average value between two centre values
    return ((double)(list[(list_length / 2) - 1] + list[(list_length / 2)]) / 2);
  } else // if list length is odd
  {
    // centre value = list length / 2 (without remainder)
    return (list[int(list_length / 2)];
  }

}
