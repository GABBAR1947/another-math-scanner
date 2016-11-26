#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;

void segment(Mat &img){
    img = 255 - img;

    Mat labels(img.size(), CV_32S), stats, centroids;
    int nLabels;
    nLabels = connectedComponentsWithStats(img, labels, 
            stats, centroids);

    vector<Vec3b> colors;
    for(int i=0; i<nLabels; i++){
        Vec3b v = Vec3b(rand()%255, rand()%255, rand()%255);
        colors.push_back(v);
    }

    Mat labelled(img.size(), CV_8UC3);
    for(int i=0; i<img.rows; i++){
        for(int j=0; j<img.cols; j++){
            labelled.at<Vec3b>(i, j) = colors[labels.at<int>(i, j)];
        }
    }

    imshow("window", labelled);
    waitKey(0);
}

int main(int argc, char *argv[]){
    Mat img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    cvtColor(img, img, CV_BGR2GRAY);
    threshold(img, img, 127, 255, CV_THRESH_OTSU);
    namedWindow("window", CV_WINDOW_NORMAL);
    segment(img);
    return 0;
}


