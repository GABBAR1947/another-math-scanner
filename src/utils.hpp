#ifndef UTILS_H
#define UTILS_H

#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
using namespace std;
using namespace cv;

struct sparse_entry{
    int i, j;
};

typedef vector<vector<sparse_entry>> sparse_image;

sparse_image to_sparse(Mat &labels, int nLabels);

#endif
