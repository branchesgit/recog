#ifndef CARDSCORE_H
#define CARDSCORE_H

#include <opencv2/opencv.hpp>
#include "supercontent.h"

using namespace cv;

class CardScore : public SuperContent
{
public:
    int questionNumber;
    int itemNumber;
    float scoreValue;
    int row;
    virtual void Do(Mat& img);
};

#endif // CARDSCORE_H
