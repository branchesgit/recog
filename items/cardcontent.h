#ifndef CARDCONTENT_H
#define CARDCONTENT_H

#include <opencv2/opencv.hpp>

using namespace cv;

typedef struct _rectPair {
    int width;
    int height;
}CandidateRect;


typedef struct _ImgPointPair {
    int loc;
    int isPredict;
    Point2f standPoint;
    Point2f imgPoint;
    CandidateRect imgPointRect;
}ImgPointPair;

class CardContent
{
public:
    CardContent();
};

#endif // CARDCONTENT_H
