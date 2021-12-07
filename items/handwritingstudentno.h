#ifndef HANDWRITINGSTUDENTNO_H
#define HANDWRITINGSTUDENTNO_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class HandwritingStudentNo : public SuperContent
{
public:
    int row; //����
    int itemNumber;
    int rotation;
    virtual void Do(Mat& img);
};

#endif // HANDWRITINGSTUDENTNO_H
