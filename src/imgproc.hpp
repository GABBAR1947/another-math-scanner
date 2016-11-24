#ifndef IMGPROC_H
#define IMGPROC_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "component.hpp"
using namespace cv;
using namespace std;


enum lighting_type {
    UNIFORM_LIGHTING,
    NON_UNIFORM_LIGHTING
};


void binarize(Mat &img);
void skew_correct(Mat &img);
vector<component> segment(Mat &img);

#endif
