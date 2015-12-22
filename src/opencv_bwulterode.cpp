/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = bwulterode(input_image)
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

  int opencv_bwulterode(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int *piAddr = NULL;

    //checking input argument
    CheckInputArgument(pvApiCtx, 1, 1);
    CheckOutputArgument(pvApiCtx, 1, 1);

    // Get the input image from the Scilab environment
    Mat gray_image, fin_image, m, image;
    retrieveImage(gray_image, 1);

    // Ensure that the image entered is binary in nature
    for (int i = 0; i < gray_image.cols; i++)
    {
      for (int j = 0; j < gray_image.rows; j++)
      {
        unsigned char val = gray_image.at<uchar>(i,j);
        if (!(val == 0 || val == 1 || val == 255))
        {
          sciprint("Please enter binary image.");
          return 0;
        } 
      }
    }

    // Check if image is grayscale or not,
    // if not, then convert it to grayscale
    if (gray_image.type() != CV_8UC1)
    { 
      Mat temp;
      temp = gray_image.clone();
      cvtColor(temp, gray_image, CV_BGR2GRAY);
    }

    // Ultimate erosion is the the regional maxima of the 
    // euclidean distance transform of the given image.

    // * Algorithm given in the research papers:
    //  [1] Vincent, L., "Morphological Grayscale Reconstruction 
    //      in Image Analysis: Applications and Efficient Algorithms,
    //      " IEEE Transactions on Image Processing, Vol. 2, 
    //      No. 2, April, 1993.
    //  [2] Soille, P., Morphological Image Analysis: Principles 
    //      and Applications, Springer-Verlag, 1999.

    // Take the euclidean distance transform of the given image.
    distanceTransform(gray_image, image, CV_DIST_L2, DIST_MASK_5);

    // Take the regional maxima of the result
    // by using morphological techniques
    subtract(image, 1, gray_image);

    /******************************************************

    * Algorithm given in the research papers:
     [1] Vincent, L., "Morphological Grayscale Reconstruction 
         in Image Analysis: Applications and Efficient Algorithms,
         " IEEE Transactions on Image Processing, Vol. 2, 
         No. 2, April, 1993, pp. 176-201.
     [2] Soille, P., Morphological Image Analysis: Principles 
         and Applications, Springer-Verlag, 1999, pp. 170-171.

    * Image reconstruction by dilation uses dilation and
     suppressing the marker image by the mask, hence the
     `min`.

    *******************************************************/

    // Function defined in common.cpp
    imreconstruct_by_dilation(image, gray_image, m);
    
    subtract(image, m, gray_image);

    // The result is given in the form of a binary image
    // (0s and 1s)
    // Scale the image to make it 0s and 255s
    fin_image = gray_image * 255;

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
