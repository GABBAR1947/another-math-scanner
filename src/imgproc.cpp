#include "imgproc.hpp"
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
                CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 3);
}

void skew_correct(Mat &img){
    /* 
     * Expects a binarized image.
     * Corrects for skew using Hough Transform
     */

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
