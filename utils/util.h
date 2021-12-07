#ifndef UTIL_H
#define UTIL_H


class Util
{
public:
    Util();
    float  horizontalProject(cv::Mat mat, int itemNumber);

    std::vector<int> objectDetect(cv::Mat search, float standarweight, float standarheight);

    void recognitionItem(Mat& img, SuperContent& content, int itemNumber, bool isSaveImage);
};

#endif // UTIL_H
