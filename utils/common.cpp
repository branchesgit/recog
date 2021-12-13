#include "common.h"
#include <opencv2/opencv.hpp>

using namespace cv;

Common::Common()
{

}

 bool Common::sortX(vector<Point> a, vector<Point> b) {
    return boundingRect(a).x < boundingRect(b).x;
}

 bool Common::sortY(vector<Point> a, vector<Point> b) {
    return boundingRect(a).y < boundingRect(b).y;
}
