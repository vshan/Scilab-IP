/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = graydist(input_image, mask)
********************************************************/

#include <numeric>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <limits>
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

  Point minDistance(Mat, Mat);
  vector<Point> getNeighbours(Point);

  int opencv_graydist(char *fname, unsigned long fname_len)
  {

    SciErr sciErr;
    int intErr = 0;
    int *piAddr = NULL;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 2);
    CheckOutputArgument(pvApiCtx, 1, 1);

    // Get the input image from the Scilab environment
    Mat marker, mask;
    retrieveImage(marker, 1);
    retrieveImage(mask, 2);
    
    // do conversion of marker to gray
    // do conversion of mask to gray

    if (marker.type() != CV_8UC1)
    {
      Mat temp = marker.clone();
      cvtColor(temp, marker, CV_BGR2GRAY);
    }

    if (mask.type() != CV_8UC1)
    {
      Mat temp = mask.clone();
      cvtColor(temp, mask, CV_BGR2GRAY);
    }


    vector<Point> sources;
    Mat distances = Mat(mask.size(), CV_32F);
    Mat sptSet = Mat::zeros(mask.size(), CV_8UC1);
    for (i = 0; i < mask.cols; i++)
    {
      for (j = 0; j < mask.rows; j++)
      {
        if (mask.at<uchar>(i,j)) {
          sources.push_back(Point(i,j));
          distances.at<float>(i,j) = 0.0;
        }
        else
          distances.at<float>(i,j) = FLT_MAX;
      }
    }

    for (c = 0; c < mask.cols * mask.rows; c++)
    {
      Point p = minDistance(distances, sptSet);
      sptSet.at<uchar>(p.x, p.y) = 1;
      vector<Point> neighbours = getNeighbours(p);
      markerVal = marker.at<uchar>(p.x, p.y);
      for (k = 0; k < neighbours.length(); k++)
      { 
        nVal = marker.at<uchar>(neighbours[k].x, neighbours[k].y);
        distN = (markerVal + nVal) / 2;
        if (!(sptSet.at<uchar>(neighbours[i].x, neighbours[i].y)) && distances.at<float>(p.x, p.y) != FLT_MAX
            && (distances.at<float>(p.x, p.y) + distN < distances.at<float>(neighbours[i].x, neighbours[i].y)))
          distances.at<float>(neighbours[i].x, neighbours[i].y) = distances.at<float>(p.x, p.y) + distN;
      } 

    }


    string tempstring = type2str(distances.type());
    char *checker;
    checker = (char *)malloc(tempstring.size() + 1);
    memcpy(checker, tempstring.c_str(), tempstring.size() + 1);
    returnImage(checker, distances, 1);
    free(checker);

    //Assigning the list as the Output Variable
    AssignOutputVariable(pvApiCtx, 1) = nbInputArgument(pvApiCtx) + 1;
    //Returning the Output Variables as arguments to the Scilab environment
    ReturnArguments(pvApiCtx);
    return 0;

  }

  Point minDistance(Mat distances, Mat sptSet)
  {
    float min = FLT_MIN;
    Point p;
    for (i = 0; i < distances.cols; i++)
    {
      for (j = 0; j < distances.rows; j++)
      {
        if (distances.at<float>(i,j) < min && sptSet.at<uchar>(i,j) == 0)
        {  
          min = distances.at<float>(i,j);
          p = Point(i,j);
        }
      }
    }
    return p;
  }

  vector<Point> getNeighbours(Point p)
  {
    vector<Point> neighs;
    Rect rect(Point(), marker.size());
    for (i = -1; i <= 1; i++) {
      for (j = -1; j <= 1; j++) {
        if (rect.contains(Point(p.x + i, p.y + j)))
          neighs.push_back(Point(p.x + i, p.y + j));
      }
    }
  }
/* ==================================================================== */
}
