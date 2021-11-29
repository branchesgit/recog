#ifndef BOUNDARY_H
#define BOUNDARY_H
#include <opencv4/opencv2/opencv.hpp>

using namespace cv;

class Boundary
{
public:
    Boundary();

    void handleBoundary(Mat mat);
};

#endif // BOUNDARY_H
