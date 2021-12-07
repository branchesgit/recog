#ifndef NUMBER_H
#define NUMBER_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class Number : public SuperContent
{
public:
    int itemNumber;  //ѧ�ŵ�����λ��
    int rotation;    //��ת�Ƕ�
    virtual void Do(Mat& img);
};

#endif // NUMBER_H
