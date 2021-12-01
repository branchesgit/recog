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

    void handleChoiceItems(Mat mat, vector<vector<Point>> contours, int offsetY, int offsetX);

    vector<vector<Point>> findNearContours(vector<Point> contour, vector<vector<Point>> contours,int offsetY, int offsetX);
};

#endif // CHOICE_H
