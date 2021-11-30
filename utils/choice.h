#ifndef CHOICE_H
#define CHOICE_H
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Choice
{
public:
    Choice();

    void handleRecognition(string choiceFilePath);

    void fillBoundary(Mat dstImg, vector<vector<Point>> contours, int max, int min);

    vector<Rect> handleChoiceItems(vector<vector<Point>> contours, int offset, int itemDis);
};

#endif // CHOICE_H
