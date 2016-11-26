#include <iostream>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
using namespace std;

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

void drawLine(Mat &src, double rho, double theta, Scalar color){
    auto y = [&](double x){
        return (rho - x*cos(theta))/sin(theta);
    };

    Point a,  b;
    a = Point(0, y(0));
    b = Point(src.cols, y(src.cols));
    bool inside = clipLine(src.size(), a, b);
    cout<<"For (rho, theta):"<<rho<<","<<theta;
    cout<<"\n\tlines: "<<a<<b<<inside<<endl;
    line(src, a, b, Scalar(0, 0, 255), 3, 8);
}

void rotateImage(Mat &src, double rho, double theta){
    auto y = [&](double x){
        return (rho - x*cos(theta))/sin(theta);
    };

    Point a,  b;
    a = Point(0, y(0));
    b = Point(src.cols, y(src.cols));
    clipLine(src.size(), a, b);
    Point center = (a + b)/2;
    //center = Point(0, 0);
    //center = Point(src.cols/2, src.rows/2);
    Mat R = getRotationMatrix2D(center, -(CV_PI/2.0-theta)*(180.0/CV_PI), 1);
    warpAffine(src, src, R, src.size());
}

Point2f rotation_angle(Mat &img, double dr, double dt){
    /* Using equation x*cos(t) + y*sin(t) = r */
    auto norm = [](double x, double y){
        return sqrt(x*x + y*y);
    };

    cout<<"Detecting max"<<endl;
    double r_max = norm(img.cols, img.rows);

    vector<Point2i> foreground;
    for(int x=0; x<img.cols; x++){
        for(int y=0; y<img.rows; y++){
            if(img.at<unsigned char>(y, x)!=0){
                foreground.push_back(Point2i(x, y));
            }
        }
    }

    Mat result;
    cvtColor(img, result, CV_GRAY2BGR);

    int r_max_i = (int)((r_max/dr)+1);
    int t_max_i = (int)((CV_PI/dt)+1);

    Size rt_size = Size(t_max_i, r_max_i);
    Mat count(rt_size, CV_32S);
    count = Scalar::all(0);
    cout<<"Initialized count"<<endl;
    for(int i=0; i<r_max_i; i++){
        for(int j=0; j<t_max_i; j++){
            assert(count.at<int>(i, j) == 0);
        }
    }
    cout<<"Sizes: "<<(int)((r_max/dr)+1)<<","<<int(CV_PI/dt)+1<<endl;
    for(double t=0; t < CV_PI; t += dt){
        for(auto p: foreground){
            int x, y;
            x = p.x, y = p.y;
            //cout<<"In (x,y,t): "<<x<<","<<y<<","<<","<<t<<endl;
            auto r = [=](double x, double y, double t){
                return (int)(x*cos(t) + y*sin(t));
            };

            int ri = (int)(r(x, y, t)/dr), ti = (int)(t/dt);

            if(ri > 0 and ri < (int)(r_max+1) and ti < 180){
                count.at<int>(ri, ti) += 1;
                int val = count.at<int>(ri, ti);
                if ( val > 100000){
                    cout<<ri<<","<<ti<<":";
                    cout<<val<<endl;
                }
            }

            else{
                //cout<<"What the fuck! - ";
                //cout<<ri<<","<<ti<<endl;
            }
        }
    }
    typedef pair<int, pair<int, int>> drt_struct;
    vector<drt_struct> S;
    for(double t=0; t < CV_PI; t += dt){
        for(double r=0; r < r_max; r += dr){
            int ti, ri;
            ti = (int)(t/dt), ri = (int)(r/dr);

            int val = count.at<int>(ri, ti);
            if ( val > 1){
                auto drt = make_pair(val, make_pair(ri, ti));
                S.push_back(drt);
            }
        }
    }

    sort(S.rbegin(), S.rend());
    int mSize = min((int)S.size(), 20);
    S.resize(mSize);


    /* Divide into bins of 10 degrees */
    vector<int> bin(180/10, 0);
    int max_bin = 0;
    for(auto e: S){
        int b = (int)(e.second.second/10);
        bin[b] += 1;
        if ( bin[b] > bin[max_bin])
            max_bin = b;
    }

    double sum_theta = 0;
    double sum_rho = 0;
    int ntheta = 0;
    for(auto e: S){
        int b = (int)(e.second.second/10);
        if ( b == max_bin){
            sum_theta += e.second.second;
            sum_rho += e.second.first;
            ntheta += 1;
        }
    }

    double mean_theta = sum_theta*dt/(double)(ntheta);
    double mean_rho = sum_rho*dr/(double)(ntheta);
    drawLine(result, mean_rho, mean_theta, Scalar(0, 0, 255));
    rotateImage(img, mean_rho, mean_theta);
    imshow("output", result);
    waitKey(0);
    return Point2f(mean_rho, mean_theta);


}


void detectLines2(Mat &src){
    vector<Vec2f> lines;
    float rho_step, theta_step;
    int threshold_pixels;

    Mat result;
    cvtColor(src, result, CV_GRAY2BGR);
    rho_step = 1;
    theta_step = CV_PI/180.0;


    double theta_sum = 0.0;
    double theta_mean;

    threshold_pixels = 50;

    HoughLines(src, lines, rho_step, theta_step, threshold_pixels);
    for(auto l: lines){
        float rho, theta;
        rho = l[0], theta = l[1];
        theta_sum += theta;
        cout<<"rho, theta: "<<rho<<","<<theta<<endl;

        /* Draw the line. */

        auto y = [&](double x){
            return (rho - x*cos(theta))/sin(theta);
        };

        Point a,  b;
        a = Point(0, y(0));
        b = Point(src.cols, y(src.cols));

        line(result, a, b, Scalar(0, 0, 255), 3, 8, CV_AA);
    }
    theta_mean = theta_sum/(double)(lines.size());

    double scale = 1.0;
    Point2f center(src.rows/2, src.cols/2);
    Mat transform_matrix  = getRotationMatrix2D(center, theta_mean, scale);
    warpAffine(result, result, transform_matrix, result.size());

    imshow("output", result);
    waitKey(0);
}

void detectLines(Mat &src){
    vector<Vec4i> lines;
    Size size = src.size();
    HoughLinesP(src, lines, 1, CV_PI/180, 100, size.width / 2.f, 20);
    Mat disp_lines(size, CV_8UC1, Scalar(0, 0, 0));
    double angle = 0.;
    unsigned nb_lines = lines.size();
    for (unsigned i = 0; i < nb_lines; ++i)
    {
        line(disp_lines, Point(lines[i][0], lines[i][1]),
                Point(lines[i][2], lines[i][3]), Scalar(255, 0 ,0));
        angle += atan2((double)lines[i][3] - lines[i][1],
                (double)lines[i][2] - lines[i][0]);
    }
    angle /= nb_lines; // mean angle, in radians.
    cout << "File " << "Bleh" << ": " << angle * 180 / CV_PI << endl;
    imshow("output", disp_lines);
    waitKey(0);

}


int main(int argc, char *argv[]){
    Mat img;
    img = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    Mat original;
    img.copyTo(original);
    cvtColor(img, img, CV_BGR2GRAY);
    GaussianBlur(img, img, Size(3, 3), 1, 1);
    Canny(img, img, 100, 2*100, 3);
    //img = 255 - img;
    namedWindow("output", CV_WINDOW_NORMAL);
    Point2f rt = rotation_angle(img, 1, CV_PI/180);
    rotateImage(original, rt.x, rt.y);
    /*
    imshow("output", img);
    waitKey(0);
    */
    imshow("output", original);
    waitKey(0);
    return 0;
}
