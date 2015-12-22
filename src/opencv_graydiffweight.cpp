/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = graydiffweight(input_image, refGrayVal)
       return_image = graydiffweight(input_image, mask)
       return_image = graydiffweight(input_image, column_list, row_list)
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

  int opencv_graydiffweight(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int iRowsR=0,iColsR=0,iColsC=0,iRowsC=0;
    int *piAddr = NULL;
    int *piAddr2  = NULL;
    double *pstDataR = NULL;
    double *pstDataC = NULL;
    int i, number_of_points;
    int lineType = 8;
    double refGrayVal;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 3);
    CheckOutputArgument(pvApiCtx, 1, 1);

    // Get the input image from the Scilab environment
    Mat image;
    retrieveImage(image, 1);

    // Check if the image is grayscale or not,
    // if not, convert it to grayscale.
    if (image.type() != CV_8UC1)
    {
      Mat temp = image.clone();
      cvtColor(temp, image, CV_BGR2GRAY);
    }

    // Check if the number of arguments are 2,
    // if so, then either the reference gray value
    // has been given, or mask image is provided.
    // If not 2, then column list and row list is
    // provided.
    if (nbInputArgument(pvApiCtx) == 2)
    {
      sciErr = getVarAddressFromPosition(pvApiCtx, 2, &piAddr);
      if (sciErr.iErr)
      {
          printError(&sciErr, 0);
          return 0;
      }
      if (isDoubleType(pvApiCtx, piAddr))
      {
        intErr = getScalarDouble(pvApiCtx, piAddr, &refGrayVal);
        if(intErr)
        {
          return intErr;
        }
        if (refGrayVal < 0 || refGrayVal > 255)
        {
          sciprint("Please ensure reference gray value is between 0 and 255.");
          return 0;
        }
      }
      else // The mask image has been given.
      {
        retrieveImage(mask, 2);

        // Check if the mask is grayscale or not,
        // if not, convert it to grayscale.
        if (mask.type() != CV_8UC1)
        {
          Mat temp = mask.clone();
          cvtColor(temp, mask, CV_BGR2GRAY);
        }

        // Compute the average of the values 
        // specified by the mask (wherever it is non-zero),
        // then use it as reference gray value.
        float sum, count;
        sum = count = 0.0;
        for (i = 0; i < image.cols; i++)
        {
          for (j = 0; j < image.rows; j++)
          {
            if (mask.at<uchar>(i,j))
            {
              sum += image.at<uchar>(i,j);
              count++;
            } 
          }
        }
        refGrayVal = sum / count;
      }
    }
    else
    {
      // Get the address of 2nd argument, the column list
        sciErr = getVarAddressFromPosition(pvApiCtx, 2, &piAddr);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        // Get the address of the 3rd agument, the row list
        sciErr = getVarAddressFromPosition(pvApiCtx, 3, &piAddr2);
        if (sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        // Get the column list in the form of a matrix 
        // No. of columns = No. of elements in the list
        // No. of rows = 1 
        sciErr = getMatrixOfDouble(pvApiCtx, piAddr, &iRowsC, &iColsC, &pstDataC);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if (iRowsC != 1)
        {
            sciprint("Please enter a list of column coordinates.\n");
            return 0;
        }
        
        // Get the row list in the form of a matrix
        // No. of columns = No. of elements in the list
        // No. of rows = 1
        sciErr = getMatrixOfDouble(pvApiCtx, piAddr2, &iRowsR, &iColsR, &pstDataR);
        if(sciErr.iErr)
        {
            printError(&sciErr, 0);
            return 0;
        }

        if (iRowsR != 1)
        {
            sciprint("Please enter a list of row coordinates.\n");
            return 0;
        }

        if (iColsC != iColsR)
        {
            sciprint("Please ensure number of elements in both row and column lists are equal.\n");
            return 0;
        }
        float sum, count;
        sum = count = 0;
        // Number of points is number of columns
        number_of_points = iColsC;
        
        for (i = 0; i < number_of_points; i++)
        {
            if (pstDataR[i] < 0 || pstDataC[i] < 0)
            {
                sciprint("Coordinates cannot be negative.\n");
                return 0;
            }
            else
            {
              sum += image.at<uchar>(pstDataR[i], pstDataC[i]);
              count++;
            }
        }

        refGrayVal = sum / count;
    }
    
    Mat fin_image;

    // With the reference gray value computed, find the 
    // absolute difference between the image and 
    // the reference gray value scalar.
    absdiff(image, refGrayVal, temp);

    string tempstring = type2str(fin_image.type());
    char *checker;
    checker = (char *)malloc(tempstring.size() + 1);
    memcpy(checker, tempstring.c_str(), tempstring.size() + 1);
    returnImage(checker, fin_image, 1);
    free(checker);

    //Assigning the list as the Output Variable
    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;
    //Returning the Output Variables as arguments to the Scilab environment
    ReturnArguments(pvApiCtx);
    return 0;

  }
/* ==================================================================== */
}
