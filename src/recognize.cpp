#include "recognize.hpp"

int nearest_neighbour(vector<Mat> data, Mat query){
    int index = 0; 
    int min_index(-1);
    double min_distance;

    auto distance_metric = [](Mat a, Mat b){
        return norm(a, b, NORM_L1);
    };
    for(auto p: data){
        if(!index){
            //min_distance = norm(p, query, NORM_L2);
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


Mat hu_moments(Mat img){
    Mat features(Size(1, 8), CV_64F);

    double I = 0;
    Mat edgemap;

    Canny(img, edgemap, 50, 150, 3);
    Moments mu = moments(edgemap);
    double SZ = img.rows * img.cols;

    // Centralized moment of inertia.
    //I = (mu.mu20 + mu.mu02)/SZ;
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

    cout<<I<<" "<<SZ<<endl;
    if (N!=0)
        I = I/(double)(N*N);
    //features.at<double>(0, 7) = I;
    features.at<double>(0, 7) = I;


    double HU[7];
    HuMoments(mu , HU);
    for(int i=0; i<7; i++){
        features.at<double>(0, i) = HU[i];
    }

    return features;
}

Mat misc_features(Mat &img){
    Mat features(Size(1, 3), CV_64F);
    features = Scalar(0);
    //features.at<double>(0, 0) = img.cols/(double)(img.rows);
    return features;
}

Mat extractFeatures(Mat &img){
    Mat cat;

    vector<Mat> features = {
        hu_moments(img),
        misc_features(img)
    };

    vconcat(features, cat);
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

        //Mat feature = hu_moments(img);
        Mat feature = extractFeatures(img);
        features.push_back(feature);
        cout<<label<<" "<<feature<<endl;
    }

    data.set(features, labels);
}

string recognizer::recognize(Mat img){
    //Mat query = hu_moments(img);
    Mat query = extractFeatures(img);
    cout<<query<<endl;
    int nearest;
    nearest = nearest_neighbour(data.features, query);
    return data.labels[nearest];
}
