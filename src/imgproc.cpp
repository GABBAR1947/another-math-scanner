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


    if (lighting == 0)
        threshold(img, img, 0, 255, CV_THRESH_OTSU);
    else{

        GaussianBlur(img,img,Size(3,3),0.7,0.7);
        adaptiveThreshold(img, img, 255,CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY, 19, 21); /*other option is to do LBP adaptive thresholding*/
        //medianBlur( img,img, 1 ); can be used to remove salt and pepper noise
    }

}

void invert(Mat &img)
{
    bitwise_not(img, img);
}

void skew_correct(Mat &img){

    vector<Vec4i> lines;

    HoughLinesP(img , lines , 1, CV_PI/180, 10 ,(img.size()).width / 4.f, (img.size()).width );


    Mat Dlines(img.size(), CV_8UC1, Scalar(0, 0, 0));
    double angle = 0.;
    unsigned LS = lines.size();
    for (unsigned i = 0; i < LS; i++)
    {
        line(Dlines, Point(lines[i][0], lines[i][1]),Point(lines[i][2], lines[i][3]), Scalar(255, 0 ,0));

        angle += atan2((double)lines[i][3] - lines[i][1],(double)lines[i][2] - lines[i][0]);
    }


    angle /= LS; 

    vector<Point> points;

    //cerr<<angle*180/CV_PI<<endl;

    Mat_<uchar>::iterator it;
    Mat_<uchar>::iterator end = img.end<uchar>();

    for(it = img.begin<uchar>() ; it!=end;it++)
    {
        if(*it)
            points.push_back(it.pos());
    }

    RotatedRect box = minAreaRect(Mat(points));

    //cerr<<box.angle<<endl;
    Mat RM = getRotationMatrix2D(box.center, angle*180/CV_PI, 1.0);

    Mat fin,cropped;
    warpAffine(img, img, RM, img.size(), INTER_CUBIC);
    //  getRectSubPix(fin, box.size, box.center, cropped);

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
