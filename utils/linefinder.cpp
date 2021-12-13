#include "linefinder.h"

LineFinder::LineFinder()
{
    deltaRho = 1;
    deltaTheta = PI / 180;
    minVote = 80;
}


void LineFinder::setAccResolution(double dRho, double dTheta) {
    deltaRho = dRho;
    deltaTheta = dTheta;
}
void LineFinder::setMinVote(int minv) {
    minVote = minv;
}
// Hough变换检测直线;rho=1,theta=PI/180参数坐标系里的步长,threshold=最小投票数
void LineFinder::findLines(cv::Mat& binary){
    lines.clear();
    cv::HoughLines(binary, lines, deltaRho, deltaTheta, minVote);
}

void LineFinder::drawDetectedLines(cv::Mat& result){
    std::vector<cv::Vec2f>::const_iterator it = lines.begin();
    while (it != lines.end())
    {
        // 以下两个参数用来检测直线属于垂直线还是水平线
        float rho = (*it)[0];
        float theta = (*it)[1];
        if (theta < PI / 4. || theta > 3.*PI / 4.)
        { // 若检测为垂直线,直线交于图片的上下两边,先找交点
            cv::Point pt1(rho / cos(theta), 0);
            cv::Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
            cv::line(result, pt1, pt2, cv::Scalar(255), 1); //
        }
        else // 若检测为水平线,直线交于图片的左右两边,先找交点
        {
            cv::Point pt1(0, rho / sin(theta));
            cv::Point pt2(result.cols, (rho - result.cols*cos(theta)) / sin(theta));
            cv::line(result, pt1, pt2, cv::Scalar(255), 1);
        }
        ++it;
    }
}

