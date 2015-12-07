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
  
  int isRegionalMaxima(Mat, int, int, int*);
  void setRegionalMaxima(Mat&, int, int, double);
  unsigned char getSNeighbour(Mat, int, int);
  unsigned char getNeighbour(Mat, int, int);

  int opencv_imextendedmax(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int *piAddr = NULL;
    double h;
    unsigned char val;
    int i, j;
    int flag;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 2);
    CheckOutputArgument(pvApiCtx, 1, 1) ;

    // Get the input image from the Scilab environment
    Mat image;
    retrieveImage(image, 1);

    // Get the address of 2nd argument, the H-Maxima transform
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
    
    Mat gray_image, fin_image;
    cvtColor(image, gray_image, CV_BGR2GRAY);

    fin_image = Mat::zeros(gray_image.size(), gray_image.type());

    for (i = 0; i < gray_image.cols-2; i++)
    {
        for (j = 0; j < gray_image.rows-2; j++)
        {   val = gray_image.at<uchar>(i,j);
            if (isRegionalMaxima(gray_image, i, j, &flag))
            {   
               if (flag)
               {
                  unsigned char v = getNeighbour(gray_image, i, j);
                  if ((val - v) > h)
                    setRegionalMaxima(fin_image, i, j, 1);
               }
               else
               {
                  unsigned char v = getSNeighbour(gray_image, i, j);
                  if ((val - v) > h)
                    fin_image.at<uchar>(i,j) = 1;
               }
            }
        }
    }

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
  int isRegionalMaxima(Mat image, int i, int j, int* flag)
  { 
    unsigned char val = image.at<uchar>(i,j);

    if ((image.at<uchar>(i-1,j) < val) &&
        (image.at<uchar>(i+1,j) < val) &&
        (image.at<uchar>(i,j-1) < val) &&
        (image.at<uchar>(i,j+1) < val))
    {
        *flag = 0;
        return 1;
    }


    if ((image.at<uchar>(i-1,j-1) == val) &&
        (image.at<uchar>(i,j-1)   == val) &&
        (image.at<uchar>(i+1,j-1) == val) &&
        (image.at<uchar>(i-1,j)   == val) &&
        (image.at<uchar>(i+1,j)   == val) &&
        (image.at<uchar>(i-1,j+1) == val) &&
        (image.at<uchar>(i,j+1)   == val) &&
        (image.at<uchar>(i+1,j+1) == val) &&
        (image.at<uchar>(i-2,j-2)  < val) &&
        (image.at<uchar>(i-1,j-2)  < val) &&
        (image.at<uchar>(i,j-2)    < val) &&
        (image.at<uchar>(i+1,j-2)  < val) &&
        (image.at<uchar>(i+2,j-2)  < val) &&
        (image.at<uchar>(i-2,j+2)  < val) &&
        (image.at<uchar>(i-1,j+2)  < val) &&
        (image.at<uchar>(i,j+2)    < val) &&
        (image.at<uchar>(i+1,j+2)  < val) &&
        (image.at<uchar>(i+2,j+2)  < val) &&
        (image.at<uchar>(i-2,j-1)  < val) &&
        (image.at<uchar>(i-2,j)    < val) &&
        (image.at<uchar>(i-2,j+1)  < val) &&
        (image.at<uchar>(i+2,j-1)  < val) &&
        (image.at<uchar>(i+2,j)    < val) &&
        (image.at<uchar>(i+2,j+1)  < val))
    {
        *flag = 1;
        return 1;
    }
    else
    {
        return 0;
    }
  }
  void setRegionalMaxima(Mat& image, int i, int j, double value)
  {  
    unsigned char val = (unsigned char) value;
    image.at<uchar>(i-1,j-1) = val;
    image.at<uchar>(i,j-1)   = val;
    image.at<uchar>(i+1,j-1) = val;
    image.at<uchar>(i-1,j)   = val;
    image.at<uchar>(i+1,j)   = val;
    image.at<uchar>(i-1,j+1) = val;
    image.at<uchar>(i,j+1)   = val;
    image.at<uchar>(i+1,j+1) = val;
    image.at<uchar>(i,j)     = val;
  }

  unsigned char getSNeighbour(Mat image, int i, int j)
  {
    return ((unsigned char) (image.at<uchar>(i-1,j) +
                             image.at<uchar>(i+1,j) +
                             image.at<uchar>(i,j-1) +
                             image.at<uchar>(i,j+1))/4);
  }

  unsigned char getNeighbour(Mat image, int i, int j)
  {
    return ((unsigned char) (image.at<uchar>(i,j-2) +
                             image.at<uchar>(i+2,j) +
                             image.at<uchar>(i,j+2) +
                             image.at<uchar>(i-2,j))/4);
  }
/* ==================================================================== */
}
