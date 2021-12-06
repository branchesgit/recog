#ifndef CHOICEITEM_H
#define CHOICEITEM_H
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class ChoiceItem
{
public:
    ChoiceItem();

    vector<Mat> cuttingChoiceItem(Mat mat);
};

#endif // CHOICEITEM_H
