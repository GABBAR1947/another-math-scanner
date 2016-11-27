#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;



Mat fourier_descriptors(Mat img, int k){
    Mat edgemap;
    Canny(img, edgemap, 50, 150, 3);
    vector<complex<double>> points;
    for(int i=0; i<edgemap.rows; i++){
        for(int j=0; j<edgemap.cols; j++){
            if(img.at<unsigned char>(i, j)){
                complex<double> c(j, i);
                points.push_back(c);
            }
        }
    }

    vector<complex<double>> output, output_truncated;
    dft(points, output, DFT_COMPLEX_OUTPUT|DFT_SCALE);

    for(int i=1; i<min(k, (int)output.size()); i++){
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


int main(int argc, char *argv[]){
    Mat img = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);
    threshold(img, img, 127, 255, CV_THRESH_OTSU);

    Mat x = fourier_descriptors(img, 10);
    cout<<x<<endl;
    return 0;


}
