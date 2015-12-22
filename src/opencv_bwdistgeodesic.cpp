/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = bwdistgeodesic(input_image, mask)
                      bwdistgeodesic(input_image, column_list, row_list)
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

  int opencv_bwdistgeodesic(char *fname, unsigned long fname_len)
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

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 3);
    CheckOutputArgument(pvApiCtx, 1, 1);

    // Get the input image from the Scilab environment
    Mat marker, mask;
    retrieveImage(marker, 1);

    // If number of arguments is 3, then column list and
    // row list are given.
    // Otherwise mask image is directly given.
    if (nbInputArgument(pvApiCtx) == 3)
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

        // Number of points is number of columns
        number_of_points = iColsC;

        // Create the n points which define
        // the polygon
        Point points[1][number_of_points];
        
        for (i = 0; i < number_of_points; i++)
        {
            if (pstDataR[i] < 0 || pstDataC[i] < 0)
            {
                sciprint("Coordinates cannot be negative.\n");
                return 0;
            }
            else
                points[0][i] = Point(pstDataR[i], pstDataC[i]);
        }

        const Point* ppt[1] = { points[0] };

        int npt[] = { number_of_points };

        // Create a new, black, blank image same size as of input
        mask_orig = Mat::zeros(image.size(), image.type());
        cvtColor(mask_orig, mask, CV_BGR2GRAY);

        // Call the fillPoly OpenCV function
        // Fill the blank image in the polygon specified
        // by the points
        fillPoly(mask, ppt, npt, 1, Scalar(255, 255, 255), lineType);
    }
    else
      retrieveImage(mask, 2);

    // Check if the marker image is grayscale or not,
    // if not, convert it to grayscale.
    if (marker.type() != CV_8UC1)
    { 
      Mat tempM;
      tempM = marker.clone();
      cvtColor(tempM, marker, CV_BGR2GRAY);
    }

    // Check if the mask image is grayscale or not,
    // if not, convert it to grayscale.
    if (mask.type() != CV_8UC1)
    { 
      Mat tempM;
      tempM = mask.clone();
      cvtColor(tempM, mask, CV_8UC1);
    }

    int i = 1;
    
    Mat temp0;
    Mat temp2 = marker.clone();
    Mat fin_image = marker.clone();

    // Image reconstruction techniques are used.
    /******************************************************
    * Algorithm given in the research papers:
    [1] Vincent, L., "Morphological Grayscale Reconstruction 
       in Image Analysis: Applications and Efficient Algorithms,
       " IEEE Transactions on Image Processing, Vol. 2, 
       No. 2, April, 1993, pp. 176-201.
    [2] Soille, P., Morphological Image Analysis: Principles 
       and Applications, Springer-Verlag, 1999, pp. 170-171.

    *******************************************************/
    do {
      temp0 = temp2.clone();
      Mat temp1, temp3;
      dilate(temp0, temp1, Mat());
      min(temp1, mask, temp2);
      temp3 = temp2 - temp0;
      temp3.convertTo(temp3, CV_32F);
      temp3 = temp3 * ((255 - i)/255.0);
      temp3.convertTo(temp3, CV_8UC1);
      max(fin_image, temp3, fin_image);
      i++;
    } while((countNonZero(temp0 != temp2) != 0) && (i != 255));


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
