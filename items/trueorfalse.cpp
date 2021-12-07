#include "trueorfalse.h"

#include <opencv2/opencv.hpp>
#include "../utils/util.h"

using namespace cv;


void TrueOrFalse::Do(Mat& img)
{
    Util util;
    util.recognitionItem(img, *this, 2, isSaveImage);
}
