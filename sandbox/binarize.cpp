#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
using namespace std;

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

vector<Rect> recursive_rect_resolve(Mat edgemap){
    int nLabels, prev_nLabels;
    Mat labels(edgemap.size(), CV_32S), stats, centroids;
    nLabels = connectedComponentsWithStats(edgemap, labels, stats, centroids);
    do{
        prev_nLabels = nLabels;
        for(int i=1; i<nLabels; i++){
            int x, y, w, h;
            x = stats.at<int>(i, CC_STAT_LEFT);
            y = stats.at<int>(i, CC_STAT_TOP);
            w = stats.at<int>(i, CC_STAT_WIDTH);
            h= stats.at<int>(i, CC_STAT_HEIGHT);

            Rect roi = Rect(x, y, w, h);
            edgemap(roi) = Scalar(255);
        }
        nLabels = connectedComponentsWithStats(edgemap, labels, stats, centroids);
        //imshow("output", edgemap);
        //waitKey(0);
    }while(nLabels < prev_nLabels);

    vector<Rect> result;
    for(int i=1; i<nLabels; i++){
        int x, y, w, h;
        x = stats.at<int>(i, CC_STAT_LEFT);
        y = stats.at<int>(i, CC_STAT_TOP);
        w = stats.at<int>(i, CC_STAT_WIDTH);
        h= stats.at<int>(i, CC_STAT_HEIGHT);

        Rect roi = Rect(x, y, w, h);
        result.push_back(roi);

    }
    return result;
}


void binarize(Mat &img, Mat &edgemap){
    Mat result(img.size(), img.type());

    Mat bboxes(edgemap.size(), CV_8UC3);
    cvtColor(edgemap, bboxes, CV_GRAY2BGR);
    
    result.setTo(255);

    vector<Rect> boundaries;
    boundaries = recursive_rect_resolve(edgemap);

    for(auto roi: boundaries){
        rectangle(bboxes, roi, Scalar(0, 0, 255), 3, 8);
        Mat subimg = img(roi);
        Scalar mean, std_dev;
        meanStdDev(subimg, mean, std_dev);
        if (std_dev[0] > 25 ){
            threshold(subimg, subimg, 127, 255, CV_THRESH_OTSU);
        }
        else{
            subimg = Scalar(255);
        }
        subimg.copyTo(result(roi));
    }

    imshow("output", bboxes);
    waitKey(0);

    result.copyTo(img);
}


int main(int argc, char *argv[]){
    Mat img;
    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat original;

    cvtColor(img, img, CV_BGR2GRAY);
    img.copyTo(original);
    GaussianBlur(img, img, Size(3, 3), 1, 1);
    Canny(img, img, 30, 120, 3);
    //img = 255 - img;
    namedWindow("output", CV_WINDOW_NORMAL);
    binarize(original, img);
    //detectLines3(img, 1, CV_PI/180);
    imshow("output", original);
    waitKey(0);
    return 0;
}
