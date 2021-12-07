#ifndef ABSENTFLAG_H
#define ABSENTFLAG_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class AbsentFlag : public SuperContent
{
public:
    bool isCircleDiameter;
    virtual void Do(Mat& img);
};


#endif // ABSENTFLAG_H
