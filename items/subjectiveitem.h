#ifndef SUBJECTIVEITEM_H
#define SUBJECTIVEITEM_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"
using namespace cv;


class SubjectiveItem : public SuperContent
{
public:
    virtual void Do(Mat& img);
};

#endif // SUBJECTIVEITEM_H
