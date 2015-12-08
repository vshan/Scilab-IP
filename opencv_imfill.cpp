/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = imfill(image)
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

  void imfill_imreconstruct(Mat, Mat, Mat&);
  
  int opencv_imfill(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int *piAddr = NULL;
    int i, j;
    double maxVal;

    //checking input argument
    CheckInputArgument(pvApiCtx, 1, 1);
    CheckOutputArgument(pvApiCtx, 1, 1) ;

    // Get the input image from the Scilab environment
    Mat image;
    retrieveImage(image, 1);
    
    Mat gray_image, dst;
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

    imfill_imreconstruct(gray_image, marker, dst);
    
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
  void imfill_imreconstruct(Mat g, Mat f, Mat& dest)
  {
    Mat m0, m1, m;
    m1 = f;
    do {
      m0 = m1.clone();
      erode(m0, m, Mat());
      max(g, m, m1);
    } while(countNonZero(m1 != m0) != 0);
    dest = m1.clone();
  }  

/* ==================================================================== */
}