#ifndef PIXELSPERCENTSCORE_H
#define PIXELSPERCENTSCORE_H


#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class PixelsPercentScore : public SuperContent
{
public:
    int questionNumber;
    int itemNumber;
    float scoreValue;
    int row;
    virtual void Do(Mat& img);
};

#endif // PIXELSPERCENTSCORE_H
