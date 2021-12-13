#ifndef COMMON_H
#define COMMON_H
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Common
{
private:
    int offset;

public:
    Common();

    static bool sortX(vector<Point> a, vector<Point> b);

    static bool sortY(vector<Point> a, vector<Point> b);

};

#endif // COMMON_H
