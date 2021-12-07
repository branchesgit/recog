#include "checkbox.h"
#include "../utils/util.h"

void CheckBox::Do(Mat& img)
{
    Util util;
    util.recognitionItem(img, *this, itemNumber, isSaveImage);
}
