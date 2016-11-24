#ifndef _COMPONENT_HPP
#define _COMPONENT_HPP

#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

struct component{
    Mat image;
    Rect bounding_box;
    Point2f centroid;

    component(Rect bounding_box, Point2f centroid):
        bounding_box(bounding_box), centroid(centroid){
            Size s = Size(bounding_box.width, bounding_box.height);
            //Size s = Size(bounding_box.height, bounding_box.width);
            image = Mat::zeros(s, CV_8UC1);
    }

    void set(int i, int j, int val){
        /*
        if ( !(bounding_box.x ==0 and bounding_box.y == 0))
            cout<<i<<","<<j<<"->"<<i-bounding_box.x<<","<<j-bounding_box.y<<endl;
            */
        i -= bounding_box.y;
        j -= bounding_box.x;
        image.at<unsigned char>(i, j) = val;
    }

    ~component(){
    }
};

#endif
