#include "recognize.hpp"

int nearest_neighbour(vector<Mat> data, Mat query){
    int index = 0; 
    int min_index(-1);
    double min_distance;
    for(auto p: data){
        if(!index){
            min_distance = norm(p, query, NORM_L2);
            min_index = index;
        
        }
        else{
            double distance = norm(p, query, NORM_L2);
            if (distance < min_distance){
                min_distance = distance;
                min_index = index;
            }
        }
        index = index + 1;
    }
    return min_index;
}


Mat hu_moments(Mat img){
    Mat features(Size(1, 8), CV_64F);

    double I;
    Mat edgemap;

    Canny(img, edgemap, 50, 150, 3);
    Moments mu = moments(edgemap);
    double SZ = img.rows * img.cols;

    // Centralized moment of inertia.
    I = (mu.mu20 + mu.mu02)/SZ;

    double HU[7];
    HuMoments(mu , HU);
    for(int i=0; i<7; i++){
        features.at<double>(0, i) = HU[i];
    }

    features.at<double>(0, 7) = 0;
    return features;
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
                min_x = min(min_x, i);
                max_x = max(max_x, i);
            }
        }
    }

    return img(Rect(min_x, min_y, max_x-min_x+1, max_y-min_y+1));
}

recognizer::recognizer(string name){
    fstream datastream(name);
    string label, filename;
    vector<Mat> features;
    vector<string> labels;
    while(datastream>>label>>filename){
        labels.push_back(label);

        Mat img = imread("pngs/"+filename, CV_LOAD_IMAGE_GRAYSCALE);
        /* Get only black region from image. */
        threshold(img, img, 127, 255, CV_THRESH_OTSU);
        img = extractForeground(img);

        Mat feature = hu_moments(img);
        features.push_back(feature);
    }

    data.set(features, labels);
}

string recognizer::recognize(Mat img){
    Mat query = hu_moments(img);
    cout<<query<<endl;
    int nearest;
    nearest = nearest_neighbour(data.features, query);
    return data.labels[nearest];
}
