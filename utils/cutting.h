#ifndef CUTING_H
#define CUTING_H
#include <opencv4/opencv2/opencv.hpp>

using namespace std;
using namespace cv;

/**
 * @brief The Cuting class
 * contours cutting
 */
class Cutting
{
public:
    Cutting();
    // save image.
    void saveContoures2Local(Mat mat, string filePath, vector<vector<Point>> contoures, int area);

};

#endif // CUTING_H
