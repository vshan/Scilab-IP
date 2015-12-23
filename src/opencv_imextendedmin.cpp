/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = imextendedmin(input_image, h)
Example:
  im = imextendedmin(image, 80)
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

  int opencv_imextendedmin(char *fname, unsigned long fname_len)
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

    // Get the address of 2nd argument, the H-Minima transform scalar
    sciErr = getVarAddressFromPosition(pvApiCtx, 2, &piAddr);
    if (sciErr.iErr)
    {
        printError(&sciErr, 0);
        return 0;
    }

    // Get the H-Minima transform scalar
    intErr = getScalarDouble(pvApiCtx, piAddr, &h);
    if(intErr)
    {
        return intErr;
    }   

    if (h < 0)
    {
        sciprint("Please enter a nonnegative scalar for H-minima transform.\n");
        return 0;
    }
    
    Mat gray_image, dst, fin_image, m, m2;
    cvtColor(image, gray_image, CV_BGR2GRAY);
    min((gray_image + h), 255, m);

    // Extended minima is the the regional minima of the 
    // h-minima transform of the given image.

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

    * Image reconstruction by erosion uses erosion and
     expanding the marker image by the mask, hence the
     `max`.

    *******************************************************/

    // Function defined in common.cpp
    imreconstruct_by_erosion(gray_image, m, dst);
    add(dst, 1, m2);
    imreconstruct_by_erosion(dst, m2, m);
    subtract(m, dst, m2);
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
