#include "handwritingstudentno.h"

HandwritingStudentNo::HandwritingStudentNo()
{

}

void HandwritingStudentNo::Do(Mat& img)
{
    //设置ROI，切割手写数字区域
    Rect cRect(rect.x, rect.y, rect.width, rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }
    Mat cimg;

    if (rotation > 360 || rotation < -360) rotation = 0;

    std::vector<cv::Mat> digitalMat = extractGrid(img, cRect, itemNumber, row, cimg, rotation);

    //保存图像
    if (isSaveImage)
    {
        string format = ".jpg";
        if (settingContent->saveImageFormat == Format_PNG)
        {
            format = ".png";
        }
        string filename = settingContent->imageSavePath + id + format;
        if (saveImage(filename, settingContent, cimg))
        {
            saveFileUrl = filename;
        }
        else
        {
            saveFileUrl = "Can Not Saved.";
        }
    }
    std::vector<string> pre_value;

    //数字切割
    vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point> > contours;
    std::vector<std::vector<cv::Point> > fillContours;
    for (int i = 0; i < digitalMat.size(); i++)
    {
        Mat srcClone = digitalMat[i].clone();
        contours.clear();
        fillContours.clear();
        cv::findContours(srcClone, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        if (contours.size() > 1)
        {
            for (size_t j = 0; j < contours.size(); j++)
            {
                //获取区域的面积，如果小于设定值就当做噪点忽略
                double area = contourArea(contours[j]);
                if (area < 10)
                {
                    fillContours.push_back(contours[j]);
                    continue;
                }
            }

            if (fillContours.size() > 0)
            {
                cv::drawContours(digitalMat[i], fillContours, -1, cv::Scalar(0), CV_FILLED, 8);
            }
        }
        parting_stable_single(digitalMat[i]);
    }

    //返回结果
    pre_value = Predictions(digitalMat);
    isCorrectInput = true;
    string ss = "";
    for (int i = 0; i < pre_value.size(); i++)
    {
        ss += pre_value[i];
    }
    result = ss;
}
