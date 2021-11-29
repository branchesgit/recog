#ifndef BOUNDARY_H
#define BOUNDARY_H
#include <opencv4/opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Boundary
{
public:
    Boundary();

    vector<vector<Point>> handleBoundary(Mat mat);
};

#endif // BOUNDARY_H
