#ifndef TRUEORFALSE_H
#define TRUEORFALSE_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class TrueOrFalse : public SuperContent
{
public:
    virtual void Do(Mat& img);
};

#endif // TRUEORFALSE_H
