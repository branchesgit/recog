#ifndef RADIOSIMPLE_H
#define RADIOSIMPLE_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class RadioSimple : public SuperContent
{
public:
    virtual void Do(Mat& img);
};

#endif // RADIOSIMPLE_H
