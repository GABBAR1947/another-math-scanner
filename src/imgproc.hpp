#ifndef IMGPROC_H
#define IMGPROC_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;


enum lighting_type {
    UNIFORM_LIGHTING,
    NON_UNIFORM_LIGHTING
};


int lighting_conditions(Mat img);
void binarize(Mat &img, int lighting);
void skew_correct(Mat &img);
void segment(Mat &img);

#endif
