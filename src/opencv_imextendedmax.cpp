/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = imextendedmax(input_image, h)
Example:
  im = imextendedmax(image, 80)
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

  int opencv_imextendedmax(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int *piAddr = NULL;
    double h;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 2);
    CheckOutputArgument(pvApiCtx, 1, 1) ;

    // Get the input image from the Scilab environment
    Mat image;
    retrieveImage(image, 1);

    // Get the address of 2nd argument, the H-Maxima transform scalar
    sciErr = getVarAddressFromPosition(pvApiCtx, 2, &piAddr);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    // Get the H-Maxima transform scalar
    intErr = getScalarDouble(pvApiCtx, piAddr, &h);
    if(intErr)
    {
        return intErr;
    }   

    if (h < 0)
    {
        sciprint("Please enter a nonnegative scalar for H-maxima transform.\n");
        return 0;
    }
    
    Mat gray_image, dst, fin_image, m, m2;
    cvtColor(image, gray_image, CV_BGR2GRAY);
    max((gray_image-h), 0, m);

    // Extended maxima is the the regional maxima of the 
    // h-maxima transform of the given image.

    // * Algorithm given in the research papers:
    //  [1] Vincent, L., "Morphological Grayscale Reconstruction 
    //      in Image Analysis: Applications and Efficient Algorithms,
    //      " IEEE Transactions on Image Processing, Vol. 2, 
    //      No. 2, April, 1993.
    //  [2] Soille, P., Morphological Image Analysis: Principles 
    //      and Applications, Springer-Verlag, 1999.


    /******************************************************
      ** IMAGE RECONSTRUCTION **
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
    imreconstruct_by_dilation(gray_image, m, dst);
    subtract(dst, 1, m2);
    imreconstruct_by_dilation(dst, m2, m);
    subtract(dst, m, m2);
    fin_image = m2 * 255;

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
