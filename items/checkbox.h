#ifndef CHECKBOX_H
#define CHECKBOX_H
#include "supercontent.h"

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class CheckBox : public SuperContent
{
public:
    int itemNumber;
    virtual void Do(Mat& img);
};

#endif // CHECKBOX_H
