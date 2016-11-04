#include "imgproc.hpp"
#include "utils.hpp"
#include <vector>

int lighting_conditions(Mat img){
    /* 
     * Expects a color image. 
     * Convert to grayscale; 
     * Using naive methods find if uniform or non-uniform lighting.
     */

    return UNIFORM_LIGHTING;
}

void binarize(Mat &img, int lighting){
    /*
     * Binarizes an image according to lighting provided.
     * Does operations in place, modifies the img matrix passed.
     */

    cvtColor(img, img, CV_BGR2GRAY);
    if (lighting == UNIFORM_LIGHTING)
        threshold(img, img, 127, 255, CV_THRESH_OTSU);
    else
        adaptiveThreshold(img, img, 255, 
                CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3, 5);
}

void skew_correct(Mat &img){

    /* 
     * Expects a binarized image.
     * Corrects for skew using Hough Transform
     */
    vector<Vec4i> lines;
    HoughLinesP(img , lines , 1, CV_PI/180, 100 , (img.size()).width/2.f,20);

    Mat Dlines(img.size(), CV_8UC1, Scalar(0, 0, 0));

    double angle = 0.;
    unsigned LS = lines.size();
    for (unsigned i = 0; i < LS; i++)
    {
        line(Dlines, Point(lines[i][0], lines[i][1]),
                Point(lines[i][2], lines[i][3]), Scalar(255, 0 ,0));
        angle += atan2((double)lines[i][3] - lines[i][1],(double)lines[i][2] - lines[i][0]);
    }

    angle /= LS; 

    cerr<<angle<<endl;

    vector<Point> points;
    Mat_<uchar>::iterator it;
    Mat_<uchar>::iterator end = img.end<uchar>();
    for(it = img.begin<uchar>() ; it!=end;it++)
    {
        if(*it)
            points.push_back(it.pos());
    }

    RotatedRect box = minAreaRect(Mat(points));

    Mat RM = getRotationMatrix2D(box.center, angle, 1);

    warpAffine(img, img, RM, img.size(), INTER_CUBIC);

}

void segment(Mat &img){
    /*
     * Expects a binary image. Only 0/255.
     * To be done post skew correction. 
     * Segments out symbols, hopefully.
     */

    /* Invert the image. OpenCV CCA assumes black background. */
    img = 255 - img;

    Mat labels(img.size(), CV_32S), stats, centroids;
    int nLabels;
    nLabels = connectedComponentsWithStats(img, labels, 
            stats, centroids);


    /* 
     * Use bounding boxes inclusion to generate a tree like structure.
     * Image is going to be sparse, could save memory a little.
     */

}
