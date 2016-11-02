#include  "utils.hpp"

sparse_image to_sparse(Mat &labels, int nLabels){
    /* All label = 0 is ignored, assumed to be background */
    sparse_image s_img(nLabels);
    sparse_entry e;
    int label;
    for(int i=0; i<labels.rows; i++){
        for(int j=0; j<labels.cols; j++){
            e.i = i, e.j = j;
            label = labels.at<int>(i, j);
            if ( label )
                s_img[label].push_back(e);
        }
    }

    return s_img;
}

