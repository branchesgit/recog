#ifndef HANDWRITINGCARDSCORE_H
#define HANDWRITINGCARDSCORE_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class HandwritingCardScore : public SuperContent
{
public:
    int questionNumber;
    float scoreValue;
    int row;
    int itemNumber;
    virtual void Do(Mat& img);
    float denoisingScale;
    vector<Rect> childrenRect;
};
#endif // HANDWRITINGCARDSCORE_H
