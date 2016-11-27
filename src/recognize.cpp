
#include "recognize.hpp"
#include <cmath>
#include <algorithm>


int nearest_neighbour(vector<Mat> data, Mat query){
    int index = 0; 
    int min_index(-1);
    double min_distance;

    auto distance_metric = [](Mat a, Mat b){
        return norm(a, b, NORM_L1);
    };
    for(auto p: data){
        if(!index){
            min_distance = distance_metric(p, query); 
            min_index = index;
        
        }
        else{
            double distance = distance_metric(p, query);
            if (distance < min_distance){
                min_distance = distance;
                min_index = index;
            }
        }
        index = index + 1;
    }
    return min_index;
}

Mat fourier_descriptors(Mat img, int k){
    Mat edgemap;
    Canny(img, edgemap, 50, 150, 3);
    vector<complex<double>> points;
    int count = 0;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(edgemap, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    Point mean(0, 0);
    for(auto point: contours[0])
        mean += point;
    cout<<mean<<endl;
    if ( contours[0].size()){
        mean.x /= round(contours[0].size());
        mean.y /= round(contours[0].size());
    }

    for(auto p: contours[0]){
        Point r = p - mean;
        complex<double> c(r.x, r.y);
        points.push_back(c);
    }



    vector<complex<double>> output, output_truncated;
    dft(points, output, DFT_COMPLEX_OUTPUT|DFT_SCALE);

    for(int i=0; i<min(k, (int)output.size()); i++){
        output_truncated.push_back(output[i]);
    }
    while((int)output_truncated.size() < k){
        output_truncated.push_back(complex<double>(0, 0));
    }

    vector<complex<double>> inversed;
    dft(output_truncated, inversed, DFT_COMPLEX_OUTPUT|DFT_SCALE|DFT_INVERSE);
    Mat result(Size(2*output_truncated.size(), 1), CV_64F);
    int index = 0;
    for(auto o: inversed){
        result.at<double>(0, index) = o.real();
        result.at<double>(0, index+1) = o.imag();
        index = index + 2;
    }

    return result;
}

Mat hu_moments(Mat img){
    Mat features(Size(8, 1), CV_64F);

    double I = 0;
    Mat edgemap;

    Canny(img, edgemap, 50, 150, 3);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(edgemap, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    int index = 0;
    double MAX = -1.0;
    
    for( int i=0; i<contours.size();i++)
    {
        if(contourArea(contours[i]) > MAX)
        {
            MAX = contourArea(contours[i]);
            index = i;
        }
    }

    Moments mu = moments(contours[index]);
    double SZ = img.rows * img.cols;

    // Centralized moment of inertia.
    int N = 0;
    int cx = edgemap.cols/2, cy = edgemap.rows/2;
    for(int i=0; i<edgemap.rows; i++){
        for(int j=0; j<edgemap.cols; j++){
            if(edgemap.at<unsigned char>(i, j)!=0){
                I += (j-cx)*(j-cx) + (i-cy)*(i-cy);
                N += 1;
            }
        }
    }

    if (N!=0)
        I = I/(double)(N*N);
    features.at<double>(0, 7) = I;


    double HU[7];
    HuMoments(mu , HU);
    for(int i=0; i<7; i++){
        features.at<double>(0, i) = HU[i];
    }

    return features;
}

Mat misc_features(Mat &img){
    Mat features(Size(3, 1), CV_64F);
    features = Scalar(0);
    features.at<double>(0, 0) = img.cols/(double)(img.rows);
    return features;
}

int calculate_crossings(vector<int> V){
    int crossings = 0;
    for(int i=1; i<V.size(); i++){
        if  ( V[i] != V[i-1])
            crossings = crossings + 1;
    }
    return crossings;
}

Mat padd_image(Mat img, int width){

    resize(img, img, Size(width, width));
    return img;
    int pr, pc;
    double scale;
    if ( img.cols > img.rows)
        scale = img.cols/(double)width;
    else
        scale = img.rows/(double)width;
    pr = img.rows * scale;
    pc = img.cols * scale;
    resize(img, img, Size(pc, pr));

    if ( img.cols > img.rows ){
        int delta = img.cols - img.rows;
        Mat result(Size(img.cols, img.cols), img.type());
        result = Scalar::all(255);
        int w = delta/2;
        Rect r = Rect(0, w, img.cols, img.rows);
        img.copyTo(result(r));
        result.copyTo(img);
    }
    else{
        int delta = img.rows - img.cols;
        Mat result(Size(img.rows, img.rows), img.type());
        result = Scalar::all(255);
        int w = delta/2;
        Rect r = Rect(w, 0, img.cols, img.rows);
        img.copyTo(result(r));
        result.copyTo(img);
    }

    threshold(img, img, 127, 255, CV_THRESH_OTSU);
    return img;
}

Mat circular_topology_features(Mat image){
    Mat padded = padd_image(image, 256);
    Canny(padded, padded, 50, 150, 3);
    double max_radius, spacing;
    int count = 9;
    max_radius = (double)((padded.rows - 10)/2);
    spacing = max_radius/(double)count;

    Mat features(Size(count-1, 1), CV_64F);
    int index = 0;
    for(double r=spacing; r < max_radius; r += spacing ){
        Mat mask = Mat::zeros(padded.size(), padded.type());
        Point center(mask.cols/2, mask.rows/2);
        int thickness=1, lineType=8;
        circle(mask, center, r, Scalar(255), thickness, lineType);

        vector<Point> contour;
        Size axes(r, r);
        ellipse2Poly(center, axes, 0, 0, 360, 1, contour);

        vector<int> contourmap;
        for(auto p: contour){
            //p = p+center;
            //int val = mask.at<unsigned char>(p.y, p.x);
            int val = padded.at<unsigned char>(p.y, p.x);
            //cout<<"Point"<<p<<": "<<val<<endl;
            contourmap.push_back(val);
        }
        int crossing = calculate_crossings(contourmap);
        features.at<double>(0, index) = (double)crossing;
        index = index + 1;
        //imshow("output", mask);
        //waitKey(0);
    }
    return features;
}


Mat extractFeatures(Mat &img){
    Mat cat;

    vector<Mat> features = {
        //hu_moments(img),
        //misc_features(img),
        //circular_topology_features(img),
        fourier_descriptors(img, 10)
    };

    //vconcat(features, cat);
    hconcat(features, cat);
    return cat;

}

Mat extractForeground(Mat &img){
    int min_x, max_x, min_y, max_y;
    min_y = img.cols + 1;
    min_x = img.rows + 1;

    max_x = -1;
    max_y = -1;


    for(int i=0; i<img.rows; i++){
        for(int j=0; j<img.cols; j++){
            if(img.at<unsigned char>(i, j)==0){
                min_y = min(min_y, i);
                max_y = max(max_y, i);
                min_x = min(min_x, j);
                max_x = max(max_x, j);
            }
        }
    }
    Rect roi = Rect(min_x, min_y, max_x-min_x+1, max_y-min_y+1);
    cout<<"Boundary:"<<roi<<endl;

    return img(roi);
}

recognizer::recognizer(string name){
    fstream datastream(name);
    string label, filename;
    vector<Mat> features;
    vector<string> labels;
    while(datastream>>label>>filename){
        cout<<"Learning:"<<label<<","<<filename<<endl;
        labels.push_back(label);

        Mat img = imread("pngs/"+filename, CV_LOAD_IMAGE_GRAYSCALE);
        //imshow("output", img);
        //waitKey(0);
        /* Get only black region from image. */
        threshold(img, img, 127, 255, CV_THRESH_OTSU);
        img = extractForeground(img);
        cout<<"Extracted foreground"<<endl;

        //Mat feature = hu_moments(img);
        Mat feature = extractFeatures(img);
        features.push_back(feature);
        cout<<label<<" "<<feature<<endl;
    }

    data.set(features, labels);
}

string recognizer::recognize(Mat img){
    //Mat query = hu_moments(img);
    cout<<"Recognize: "<<endl;
    Mat query = extractFeatures(img);
    cout<<query<<endl;
    int nearest;
    nearest = nearest_neighbour(data.features, query);
    cout<<"Nearest: "<<nearest<<endl;
    return data.labels[nearest];
}
