/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = imfill(image)
       return_image = imfill(image, 'holes')
       return_image = imfill(image, points)
Example:
  im = imfill(image, [1 1; 10 10; 11 12])
********************************************************/

#include <numeric>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
using namespace cv;
using namespace std;
extern "C"
{
  #include "api_scilab.h"
  #include "Scierror.h"
  #include "BOOL.h"
  #include <localization.h>
  #include "sciprint.h"
  #include "../common.h"
  
  int opencv_imfill(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int *piAddr = NULL;
    int i, j;
    double maxVal;
    int iRet;
    int iRows, iCols;
    double *pstData = NULL;

    //checking input argument
    CheckInputArgument(pvApiCtx, 1, 2);
    CheckOutputArgument(pvApiCtx, 1, 1);

    // Get the input image from the Scilab environment
    Mat image, dst;
    retrieveImage(image, 1);

    // Check if the number of arguments passed is 2,
    // if so then check if it's a string that says 'holes'
    // or if it's a matrix containing the points.
    // If not 2, then just the image is passed.
    if (nbInputArgument(pvApiCtx) == 2)
    {
      sciErr = getVarAddressFromPosition(pvApiCtx, 2, &piAddr);
      if (sciErr.iErr)
      {
        printError(&sciErr, 0);
        return 0;
      }
      
      // Check if the marker image is grayscale or not,
      // if not, convert it to grayscale.
      if (image.type() != CV_8UC1)
      { 
        Mat temp;
        temp = image.clone();
        cvtColor(temp, image, CV_BGR2GRAY);
      }

      // If it's string
      if(isStringType(pvApiCtx, piAddr))
      {
        char* pstData = NULL;
        
        // Get the string from the Scilab environment
        iRet = getAllocatedSingleString(pvApiCtx, piAddr, &pstData);
        
        if(iRet)
        {
          freeAllocatedSingleString(pstData);
          return iRet;
        }
        
        // Does the string say 'holes' ?
        if (strcmp(pstData, "holes") == 0)
        {
          freeAllocatedSingleString(pstData);
          // Floodfill from point (0, 0)
          Mat im_floodfill = image.clone();
          floodFill(im_floodfill, Point(0,0), Scalar(255));
           
          // Invert floodfilled image
          Mat im_floodfill_inv;
          bitwise_not(im_floodfill, im_floodfill_inv);
           
          // Combine the two images to get the foreground.
          Mat dst2 = (image | im_floodfill_inv);
          erode(dst2, dst, Mat());

        }
        else
        {
          sciprint("Option not identified. Maybe you meant 'holes'.");
          return 0;
        }
      }
      else
      {

        // Get the points list in the form of a matrix 
        // No. of columns = 2
        // No. of rows = No. of points
        sciErr = getMatrixOfDouble(pvApiCtx, piAddr, &iRows, &iCols, &pstData);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if (iCols != 2)
        {
          sciprint("Please enter a valid column and row index.");
          return 0;
        }

        for (i = 0; i < iRows*iCols; i += iCols)
        {
            if (pstData[i] < 0 || pstData[i+1] < 0)
            {
                sciprint("Coordinates cannot be negative.\n");
                return 0;
            }
            else
                floodFill(image, Point(pstData[i], pstData[i+1]), Scalar(255));
        }

        dst = image.clone();
      }
   
    }
    else
    {

      /*
      The fill operation on a grayscale image is defined as a 
      morphological operation as seen in the following research 
      papers:
      */

      /******************************************************
       
       * Algorithm given in the research papers:
         [1] Vincent, L., "Morphological Grayscale Reconstruction 
             in Image Analysis: Applications and Efficient Algorithms,
             " IEEE Transactions on Image Processing, Vol. 2, 
             No. 2, April, 1993, pp. 176-201.
         [2] Soille, P., Morphological Image Analysis: Principles 
             and Applications, Springer-Verlag, 1999, pp. 170-171.
       * Morphological image reconstruction is a common function
         used in morphological transformation functions such as
         imhmax, imhmin, imextendedmin, imextendedmax, imfill,
         imimposemin
       * Image reconstruction by erosion uses erosion and
         expanding the marker image by the mask, hence the
         `max`.
      *******************************************************/
      Mat gray_image;
      cvtColor(image, gray_image, CV_BGR2GRAY);

      minMaxLoc(gray_image, NULL, &maxVal, NULL, NULL);

      Mat marker = Mat(gray_image.size(), gray_image.type());

      marker = Scalar::all(maxVal);

      for (i = 0; (i == 0 || i == marker.cols-1); i += (marker.cols - 1))
      {
        for (j = 0; j < marker.rows; j++)
        {
          marker.at<uchar>(i,j) = gray_image.at<uchar>(i,j);
        } 
      }

      for (i = 1; i < marker.cols-1; i++)
      {
        for (j = 0; (j == 0 || j == marker.rows-1); j += (marker.rows - 1))
        {
          marker.at<uchar>(i,j) = gray_image.at<uchar>(i,j);
        }
      }
      
      // Defined in common.c
      imreconstruct_by_erosion(gray_image, marker, dst);
    }
    
    
    
    string tempstring = type2str(dst.type());
    char *checker;
    checker = (char *)malloc(tempstring.size() + 1);
    memcpy(checker, tempstring.c_str(), tempstring.size() + 1);
    returnImage(checker, dst, 1);
    free(checker);

    //Assigning the list as the Output Variable
    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;
    //Returning the Output Variables as arguments to the Scilab environment
    ReturnArguments(pvApiCtx);
    return 0;

  } 

/* ==================================================================== */
}
