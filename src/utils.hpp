#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

struct sparse_entry{
    int i, j;
};

typedef vector<vector<sparse_entry>> sparse_image;

sparse_image to_sparse(Mat &labels, int nLabels){
    /* All label = 0 is ignored, assumed to be background */
    sparse_image s_img(nLabels);
    sparse_entry e;
    int label;
    for(int i=0; i<img.rows; i++){
        for(int j=0; j<img.cols; j++){
            e.i = i, e.j = j;
            label = labels.at<int>(i, j);
            sparse_image[label].push_back(e);
        }
    }

    return sparse_image;
}
