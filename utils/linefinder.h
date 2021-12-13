#ifndef LINEFINDER_H
#define LINEFINDER_H
#include <opencv2/opencv.hpp>

#define PI 3.1415926


class LineFinder
{
public:
    LineFinder();
    void setAccResolution(double dRho, double dTheta);
    void findLines(cv::Mat& binary);
    void drawDetectedLines(cv::Mat& result);
    void setMinVote(int minv);

private:
    std::vector<cv::Vec2f> lines;
    double deltaRho; // 参数坐标系的步长(theta表示与直线垂直的角度)
    double deltaTheta;
    int minVote;  // 判断是直线的最小投票数
};

#endif // LINEFINDER_H
