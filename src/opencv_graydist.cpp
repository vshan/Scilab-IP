/********************************************************
Author: Vinay Bhat
********************************************************
Usage: return_image = graydist(input_image, mask)
       return_image = graydist(input_image, column_list, row_list)
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
    int iRowsR=0,iColsR=0,iColsC=0,iRowsC=0;
    int *piAddr = NULL;
    int *piAddr2  = NULL;
    double *pstDataR = NULL;
    double *pstDataC = NULL;
    int i, number_of_points;
    int lineType = 8;

    //checking input argument
    CheckInputArgument(pvApiCtx, 2, 2);
    CheckOutputArgument(pvApiCtx, 1, 1);

    // Get the input image from the Scilab environment
    Mat marker, mask;
    retrieveImage(marker, 1);
    
    
    // Check if the marker image is grayscale or not,
    // if not, convert it to grayscale.
    if (marker.type() != CV_8UC1)
    {
      Mat temp = marker.clone();
      cvtColor(temp, marker, CV_BGR2GRAY);
    }

    // Check if the number of arguments passed is three,
    // if so, then the column_list and row_list are provided.
    // If not, then the mask image is directly provided.
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
    {
      retrieveImage(mask, 2);
    }

    // Check if the mask image is grayscale or not,
    // if not, convert it to grayscale.
    if (mask.type() != CV_8UC1)
    {
      Mat temp = mask.clone();
      cvtColor(temp, mask, CV_BGR2GRAY);
    }


    // This function uses Dijkstra's algorithm to
    // find the gray-weighted distance transform
    // of the given image.
    vector<Point> sources;
    Mat distances = Mat(mask.size(), CV_32F);
    Mat sptSet = Mat::zeros(mask.size(), CV_8UC1);

    // Initialize the distance matrix to FLT_MAX, and
    // zero for the sources.
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

    // The main dijkstra's algorithm
    // For all the pixels, update the distances
    // and get the shortest paths from each of them
    // to the nearest source.
    for (c = 0; c < mask.cols * mask.rows; c++)
    {
      Point p = minDistance(distances, sptSet);
      sptSet.at<uchar>(p.x, p.y) = 1;

      // The gray-weighted distance transform uses 3x3 connectivity,
      // hence the neighbours of a point p are the pixels directly
      // around it.
      vector<Point> neighbours = getNeighbours(p);
      markerVal = marker.at<uchar>(p.x, p.y);

      // Update the distance matrix for all the neighbours
      for (k = 0; k < neighbours.size(); k++)
      { 
        nVal = marker.at<uchar>(neighbours[k].x, neighbours[k].y);

        // The distance between two connected-pixels is the average of their
        // intensity values.
        distN = (markerVal + nVal) / 2;

        // Ensure that neighbour point isn't in sptSet, value of considered pixel
        // isn't FLT_MAX and that the sum of distance from u to v is less than
        // the distance to v. 
        // If so, update the distance matrix accordingly.
        if (!(sptSet.at<uchar>(neighbours[i].x, neighbours[i].y)) 
            && distances.at<float>(p.x, p.y) != FLT_MAX
            && (distances.at<float>(p.x, p.y) + distN < distances.at<float>(neighbours[i].x, neighbours[i].y)))
        {
          distances.at<float>(neighbours[i].x, neighbours[i].y) = distances.at<float>(p.x, p.y) + distN;
        }
      } 

    }

    // Return the final distance matrix as the image.
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

  // The minDistance function finds the minimum distance
  // in the given distance matrix and ensures it is not
  // in sptSet
  Point minDistance(Mat distances, Mat sptSet)
  { 
    int i,j;
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

  // This function returns a vector containing the neighbours of a
  // given point. 
  vector<Point> getNeighbours(Point p)
  {
    int i,j;
    vector<Point> neighs;
    Rect rect(Point(), marker.size());
    for (i = -1; i <= 1; i++) {
      for (j = -1; j <= 1; j++) {
        if ( (rect.contains(Point(p.x + i, p.y + j))) && !(i == 0 && j == 0) )
          neighs.push_back(Point(p.x + i, p.y + j));
      }
    }
  }
/* ==================================================================== */
}
