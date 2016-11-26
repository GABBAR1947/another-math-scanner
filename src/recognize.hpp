#ifndef RECOGNIZE_HPP
#define RECOGNIZE_HPP

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <dirent.h>
#include <fstream>

using namespace cv;
using namespace std;

struct datamap{
    vector<Mat> features;
    vector<string> labels;

    datamap(vector <Mat> f, vector<string> l): features(f), labels(l){}
    int size(){ return features.size(); }


};


int nearest_neighbour(vector<Mat> data, Mat query);
Mat hu_moments(Mat img);
datamap read_data(string name);
string recognize(Mat img);
#endif
