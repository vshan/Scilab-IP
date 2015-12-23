// This file is released under the 3-clause BSD license. See COPYING-BSD.

names = [	
			"imread","opencv_imread";
     		"blur","opencv_blur";
            "canny","opencv_canny";
            "circle","opencv_circle";
            "clipline","opencv_clipline";
            "convexhull","opencv_convexhull";
            "cornerEigenValsAndVecs","opencv_cornerEigenValsAndVecs";
            "cornerHarris","opencv_cornerHarris";
            "cornerMinEigenVal","opencv_cornerMinEigenVal";
            "dilate","opencv_dilate";
            "ellipse2poly","opencv_ellipse2poly";
            "erode","opencv_erode";
            "fillconvexpoly","opencv_fillconvexpoly";
            "filter2D","opencv_filter2D";
            "findContours","opencv_findContours";
            "gaussianblur","opencv_gaussianblur";
            "getStructuringElement","opencv_getStructuringElement";
            "getTextSize","opencv_getTextSize";
            "goodfeaturestotrack","opencv_goodfeaturestotrack";
            "houghcircles","opencv_houghcircles";
            "houghlines","opencv_houghlines";
            "im2double","opencv_im2double";
            "imcontrast","opencv_imcontrast";
            "imcrop","opencv_imcrop";
            "imresize","opencv_imresize";
            "imsharpen","opencv_imsharpen";
            "imwrite","opencv_imwrite";
            "laplacian","opencv_laplacian";
            "line","opencv_line";
            "medianblur","opencv_medianblur";
            "puttext","opencv_puttext";
            "scharr","opencv_scharr";
            "sobel","opencv_sobel";
            "threshold","opencv_threshold";
            "undistort","opencv_undistort";
            "boundingRect","opencv_boundingRect";
            "ellipse","opencv_ellipse";
            "montage","opencv_montage";
            "morphologyEx","opencv_morphologyEx";
            "pyrDown","opencv_pyrDown";
            "pyrUp","opencv_pyrUp";
            "rectangle","opencv_rectangle";
            "sepFilter2D","opencv_sepFilter2D";
            "copymakeborder","opencv_copymakeborder";
            "cvtColor","opencv_cvtColor";
            "getgaussiankernel","opencv_getgaussiankernel";
            "getrectsubpix","opencv_getrectsubpix";
            "getrotationmatrix2D","opencv_getrotationmatrix2D";
            "houghlinesp","opencv_houghlinesp";
            "riopoly", "opencv_roipoly";
            "imextendedmax", "opencv_imextendedmax";
            "imhmax", "opencv_imhmax";
            "imextendedmin", "opencv_imextendedmin";
            "imhmin", "opencv_imhmin";
            "imimposemin", "opencv_imimposemin";
            "imfill", "opencv_imfill";
            "bwulterode", "opencv_bwulterode";
            "regionfill", "opencv_regionfill";
            "roifill", "opencv_roifill";
            "bwdistgeodesic", "opencv_bwdistgeodesic";
		];

files = [
			"opencv_imread.cpp",
     		"opencv_blur.cpp",
            "opencv_canny.cpp",
            "opencv_circle.cpp",
            "opencv_clipline.cpp",
            "opencv_convexhull.cpp",
            "opencv_cornerEigenValsAndVecs.cpp"
            "opencv_cornerHarris.cpp",
            "opencv_cornerMinEigenVal.cpp",
            "opencv_dilate.cpp",
            "opencv_ellipse2poly.cpp",
            "opencv_erode.cpp",
            "opencv_fillconvexpoly.cpp",
            "opencv_filter2D.cpp",
            "opencv_findContours.cpp",
            "opencv_gaussianblur.cpp",
            "opencv_getStructuringElement.cpp",
            "opencv_getTextSize.cpp",
            "opencv_goodfeaturestotrack.cpp",
            "opencv_houghcircles.cpp",
            "opencv_houghlines.cpp",
            "opencv_im2double.cpp",
            "opencv_imcontrast.cpp",
            "opencv_imcrop.cpp",
            "opencv_imresize.cpp",
            "opencv_imsharpen.cpp",
            "opencv_imwrite.cpp",
            "opencv_laplacian.cpp",
            "opencv_line.cpp",
            "opencv_medianblur.cpp",
            "opencv_puttext.cpp",
            "opencv_scharr.cpp",
            "opencv_sobel.cpp",
            "opencv_threshold.cpp",
            "opencv_undistort.cpp",
            "opencv_boundingRect.cpp",
            "opencv_ellipse.cpp",
            "opencv_montage.cpp",
            "opencv_morphologyEx.cpp",
            "opencv_pyrDown.cpp",
            "opencv_pyrUp.cpp",
            "opencv_rectangle.cpp",
            "opencv_sepFilter2D.cpp",
            "opencv_copymakeborder.cpp",
            "opencv_cvtColor.cpp",
            "opencv_getgaussiankernel.cpp",
            "opencv_getrectsubpix.cpp",
            "opencv_getrotationmatrix2D.cpp",
            "opencv_houghlinesp.cpp",
            "opencv_roipoly.cpp",
            "opencv_imextendedmax.cpp",
            "opencv_imhmax.cpp",
            "opencv_imextendedmin.cpp",
            "opencv_imhmin.cpp",
            "opencv_imimposemin.cpp",
            "opencv_imfill.cpp",
            "opencv_bwulterode.cpp",
            "opencv_regionfill.cpp",
            "opencv_roifill.cpp",
            "opencv_bwdistgeodesic.cpp"
		];

function builder_gw_cpp()
    WITHOUT_AUTO_PUTLHSVAR = %t;
    copyfile("sci_gateway/cpp/common.h",TMPDIR);
    copyfile("sci_gateway/cpp/common.cpp",TMPDIR);
tbx_build_gateway("skeleton_imageprocessing", ..
	names, ..
    files, ..
    get_absolute_file_path("builder_gateway_cpp.sce"),[],"g++ -ggdb `pkg-config --cflags opencv` -o `basename opencv_imread.cpp .cpp` opencv_imread.cpp `pkg-config --libs opencv`");
endfunction

builder_gw_cpp();
clear builder_gw_cpp; // remove builder_gw_cpp on stack
