#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "imgproc.hpp"

#include <iostream>
#include <cstdio>
#include <string>
#include "component.hpp"
#include "recognize.hpp"

using namespace std;
using namespace cv;

/*
 * Usage: <executable name> <input file> <output_file>
 * Input file should be any standard image file readable using opencv 
 * imread, and outfile should be a .tex file containing just the 
 * equation.
 */

string process(Mat &img){
    string result;
    vector<component> components;

    binarize(img);
    skew_correct(img);
    components = segment(img);

    recognizer R("labels.txt");

    for(auto &c : components){
        c.label = R.recognize(c.image);
        cout<<"Label: "<<c.label<<endl;
        imshow("output", c.image);
        waitKey(0);
    }

    //result = assemble(components);
    return result;
}

int main(int argc, char *argv[]){
    if (argc < 3){
        fprintf(stderr, "Usage: %s <input_image> <output_file>\n", 
                    argv[0]);
        return -1;
    }

    char *input_image_file, *output_tex_file;
    input_image_file = argv[1];
    output_tex_file = argv[2];

    // Read Image
    Mat img;
    img = imread(input_image_file, CV_LOAD_IMAGE_COLOR);

    if (!img.data){
        fprintf(stderr, "Error opening image file.\n");
        fprintf(stderr, "Usage: %s <input_image> <output_file>\n", 
                    argv[0]);
        return -1;
    
    }

    namedWindow("output", CV_WINDOW_NORMAL);

    string equation_tex = process(img);
    
    FILE *output_fp;
    output_fp = fopen(output_tex_file, "w");
    fprintf(output_fp, "%s\n", equation_tex.c_str());
    fclose(output_fp);
    return 0;
}
