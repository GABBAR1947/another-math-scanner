#ifndef RECOGNIZE_HPP
#define RECOGNIZE_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <dirent.h>
#include <fstream>
#include <iostream>

using namespace cv;
using namespace std;

struct datamap{
    vector<Mat> features;
    vector<string> labels;

    datamap(vector <Mat> f, vector<string> l): features(f), labels(l){}
    void set(vector <Mat> f, vector<string> l){
        features = f;
        labels = l;
    }
    datamap(){}
    int size(){ return features.size(); }


};

class recognizer{
    datamap data;

    public:
        recognizer(string name);
        string recognize(Mat img);

};

int nearest_neighbour(vector<Mat> data, Mat query);
Mat hu_moments(Mat img);
#endif
