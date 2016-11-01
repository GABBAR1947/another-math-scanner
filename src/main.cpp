#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

#include <iostream>
#include <cstdio>
#include <string>
using namespace std;

/*
 * Usage: <executable name> <input file> <output_file>
 * Input file should be any standard image file readable using opencv 
 * imread, and outfile should be a .tex file containing just the 
 * equation.
 */

string process(Mat &img){
    string result = "tada";
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

    string equation_tex = process(img);
    
    FILE *output_fp;
    output_fp = fopen(output_tex_file, "w");
    fprintf(output_fp, "%s\n", equation_tex.c_str());
    fclose(output_fp);
    return 0;
}
