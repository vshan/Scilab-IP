/***************************************************
Author : Sukul Bagai
***************************************************/

string type2str(int type);
int no_of_channels(int type);
int retrieveImage(Mat &image,int pos);
int returnImage(char *checker,Mat img,int pos);

/**************************************************
Author: Vinay Bhat
***************************************************/

void imreconstruct_by_dilation(Mat g, Mat f, Mat& dest);
void imreconstruct_by_erosion(Mat g, Mat f, Mat& dest);
