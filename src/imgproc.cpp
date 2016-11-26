#include "imgproc.hpp"
#include "utils.hpp"
#include <vector>
#include<iostream>
#include <cstdio>
#include <cstdlib>
#include "component.hpp"

vector<Rect> recursive_rect_resolve(Mat edgemap){
    int nLabels, prev_nLabels;
    Mat labels(edgemap.size(), CV_32S), stats, centroids;
    nLabels = connectedComponentsWithStats(edgemap, labels, stats, centroids);
    cout<<"found labels: "<<nLabels<<endl;
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

void rotateImage(Mat &src, double rho, double theta){
    auto y = [&](double x){
        return (rho - x*cos(theta))/sin(theta);
    };

    Point a,  b;
    a = Point(0, y(0));
    b = Point(src.cols, y(src.cols));
    clipLine(src.size(), a, b);
    Point center = (a + b)/2;
    //center = Point(0, 0);
    //center = Point(src.cols/2, src.rows/2);
    Mat R = getRotationMatrix2D(center, -(CV_PI/2.0-theta)*(180.0/CV_PI), 1);
    warpAffine(src, src, R, src.size(), 
            INTER_LINEAR, BORDER_CONSTANT, Scalar(255, 255, 255));
}

Point2f rotation_params(Mat &img, double dr, double dt){
    /* Using equation x*cos(t) + y*sin(t) = r */
    auto norm = [](double x, double y){
        return sqrt(x*x + y*y);
    };

    cout<<"Detecting max"<<endl;
    double r_max = norm(img.cols, img.rows);

    vector<Point2i> foreground;
    for(int x=0; x<img.cols; x++){
        for(int y=0; y<img.rows; y++){
            if(img.at<unsigned char>(y, x)!=0){
                foreground.push_back(Point2i(x, y));
            }
        }
    }

    Mat result;
    cvtColor(img, result, CV_GRAY2BGR);

    int r_max_i = (int)((r_max/dr)+1);
    int t_max_i = (int)((CV_PI/dt)+1);

    Size rt_size = Size(t_max_i, r_max_i);
    Mat count(rt_size, CV_32S);
    count = Scalar::all(0);
    cout<<"Initialized count"<<endl;
    for(int i=0; i<r_max_i; i++){
        for(int j=0; j<t_max_i; j++){
            assert(count.at<int>(i, j) == 0);
        }
    }
    cout<<"Sizes: "<<(int)((r_max/dr)+1)<<","<<int(CV_PI/dt)+1<<endl;
    for(double t=0; t < CV_PI; t += dt){
        for(auto p: foreground){
            int x, y;
            x = p.x, y = p.y;
            //cout<<"In (x,y,t): "<<x<<","<<y<<","<<","<<t<<endl;
            auto r = [=](double x, double y, double t){
                return (int)(x*cos(t) + y*sin(t));
            };

            int ri = (int)(r(x, y, t)/dr), ti = (int)(t/dt);

            if(ri > 0 and ri < (int)(r_max+1) and ti < 180){
                count.at<int>(ri, ti) += 1;
                int val = count.at<int>(ri, ti);
                if ( val > 100000){
                    cout<<ri<<","<<ti<<":";
                    cout<<val<<endl;
                }
            }

            else{
                //cout<<"What the fuck! - ";
                //cout<<ri<<","<<ti<<endl;
            }
        }
    }
    typedef pair<int, pair<int, int>> drt_struct;
    vector<drt_struct> S;
    for(double t=0; t < CV_PI; t += dt){
        for(double r=0; r < r_max; r += dr){
            int ti, ri;
            ti = (int)(t/dt), ri = (int)(r/dr);

            int val = count.at<int>(ri, ti);
            if ( val > 1){
                auto drt = make_pair(val, make_pair(ri, ti));
                S.push_back(drt);
            }
        }
    }

    sort(S.rbegin(), S.rend());
    int mSize = min((int)S.size(), 20);
    S.resize(mSize);


    /* Divide into bins of 10 degrees */
    vector<int> bin(180/10, 0);
    int max_bin = 0;
    for(auto e: S){
        int b = (int)(e.second.second/10);
        bin[b] += 1;
        if ( bin[b] > bin[max_bin])
            max_bin = b;
    }

    double sum_theta = 0;
    double sum_rho = 0;
    int ntheta = 0;
    for(auto e: S){
        int b = (int)(e.second.second/10);
        if ( b == max_bin){
            sum_theta += e.second.second;
            sum_rho += e.second.first;
            ntheta += 1;
        }
    }

    double mean_theta = sum_theta*dt/(double)(ntheta);
    double mean_rho = sum_rho*dr/(double)(ntheta);
    //drawLine(result, mean_rho, mean_theta, Scalar(0, 0, 255));
    rotateImage(img, mean_rho, mean_theta);
    /*
    imshow("output", result);
    waitKey(0);
    */
    return Point2f(mean_rho, mean_theta);


}


void binarize(Mat &src){
    /*
     * Binarizes an image according to lighting provided.
     * Does operations in place, modifies the img matrix passed.
     */

    Mat img;
    src.copyTo(img);
    cvtColor(img, img, CV_BGR2GRAY);
    Mat edgemap;
    img.copyTo(edgemap);
    GaussianBlur(edgemap, edgemap, Size(3, 3), 1, 1);
    Canny(edgemap, edgemap, 30, 120, 3);
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
    /*

    imshow("output", bboxes);
    waitKey(0);
    */
    result.copyTo(img);
    img.copyTo(src);
}



void skew_correct(Mat &img){
    Mat edgemap;
    img.copyTo(edgemap);
    //cvtColor(edgemap, edgemap, CV_BGR2GRAY);
    GaussianBlur(edgemap, edgemap, Size(3, 3), 1, 1);
    Canny(edgemap, edgemap, 100, 2*100, 3);
    Point2f rt = rotation_params(edgemap, 1, CV_PI/180.0);
    rotateImage(img, rt.x, rt.y);
    threshold(img, img, 127, 255, CV_THRESH_OTSU);
}


vector<component> segment(Mat &img){
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


    srand(time(NULL));
    vector<Vec3b> colors;
    for(int i=0; i<nLabels; i++){
        Vec3b v = Vec3b(rand()%255, rand()%255, rand()%255);
        colors.push_back(v);
    }

    Mat labelled(img.size(), CV_8UC3);

    vector<component> components;
    for(int i=0; i<nLabels; i++){
        int x, y, w, h, xc, yc;

        x = stats.at<int>(i, CC_STAT_LEFT);
        y = stats.at<int>(i, CC_STAT_TOP);
        w = stats.at<int>(i, CC_STAT_WIDTH);
        h = stats.at<int>(i, CC_STAT_HEIGHT);

        Rect bounding_box = Rect(x, y, w, h);

        xc = centroids.at<int>(i, 0);
        yc = centroids.at<int>(i, 1);

        Point2f centroid = Point2f(xc, yc);
        component c = component(bounding_box, centroid);
        components.push_back(c);
    
    }
    for(int i=0; i<img.rows; i++){
        for(int j=0; j<img.cols; j++){
            int label = labels.at<int>(i, j);
            labelled.at<Vec3b>(i, j) = colors[label];
            components[label].set(i, j, img.at<unsigned char>(i, j));
        }
    }
    
    return components;
}
