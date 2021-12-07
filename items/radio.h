#ifndef RADIO_H
#define RADIO_H
#include "supercontent.h"

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class Radio : public SuperContent
{
public:
    int itemNumber;  //ѡ�����
    virtual void Do(Mat& img);
};
#endif // RADIO_H
