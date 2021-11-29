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
};

#endif // CHOICE_H
