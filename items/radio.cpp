#include "radio.h"
#include "../utils/util.h"

void Radio::Do(Mat& img)
{
    Util util;
    util.recognitionItem(img, *this, itemNumber, isSaveImage);
}
