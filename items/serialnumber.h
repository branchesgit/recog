#ifndef SERIALNUMBER_H
#define SERIALNUMBER_H

#include "supercontent.h"
#include <opencv2/opencv.hpp>
#include <opencv2/ml.hpp>

using namespace cv;

class SerialNumber : public SuperContent
{
public:
    int itemNumber;
    int rotation;


    virtual void Do(Mat& img);
};
#endif // SERIALNUMBER_H
