#include "handwritingcardscore.h"

HandwritingCardScore::HandwritingCardScore()
{

}

void HandwritingCardScore::Do(Mat& img)
{
    Rect cRect(rect.x, rect.y, rect.width, rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }

    Mat cimg;
    std::vector<cv::Mat> digitalMat = splitGrid(img, cRect, childrenRect, itemNumber, row, cimg, 0);



    // 如果识别区域为空，那么不做识别
    std::vector<cv::Mat> digitalMatFinally;
    for (size_t i = 0; i < digitalMat.size(); i++)
    {
        std::vector<cv::Vec4i> hierarchy;
        std::vector<std::vector<cv::Point> > fillContours;
        double threshold = digitalMat[0].rows * 0.15;
        cv::Mat srcClone = digitalMat[i].clone();

        //cv::imwrite("D:\\aDongNiShibie\\handwrite\\question\\result2222.png" , srcClone);


        //腐蚀运算
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::morphologyEx(srcClone, srcClone, cv::MORPH_ERODE, kernel);

        std::vector<std::vector<cv::Point> > contours;

        fillContours.clear();
        cv::findContours(srcClone, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
        if (contours.size() > 1)
        {
            for (size_t j = 0; j < contours.size(); j++)
            {
                cv::Rect box = boundingRect(contours[j]);
                if (box.height < threshold && box.width < threshold)
                {
                    fillContours.push_back(contours[j]);
                    continue;
                }
            }
        }

        cv::drawContours(srcClone, fillContours, -1, cv::Scalar(0), CV_FILLED, 8);

        int height = srcClone.rows;
        int weight = srcClone.cols;
        int centerX = weight / 2;
        int centerY = height / 2;

        int piasX = weight / 4;
        int paisY = height / 4;

        cv::Rect rect(centerX - piasX, centerY - paisY, piasX * 2, paisY * 2);

        cv::Mat digitalCenter;
        srcClone(rect).copyTo(digitalCenter);

        height = digitalCenter.rows;
        weight = digitalCenter.cols;
        int pixSum = cv::countNonZero(digitalCenter);
        if (pixSum > 5) {
            digitalMatFinally.push_back(digitalMat[i]);
        }
        else {

            cv::Mat mat(digitalMat[i].rows, digitalMat[i].cols, CV_8UC1, cv::Scalar(0));
            digitalMatFinally.push_back(mat);

        }
    }
    digitalMat = digitalMatFinally;

    //保存图像
    if (isSaveImage)
    {
        std::string format = ".jpg";
        if (settingContent->saveImageFormat == Format_PNG)
        {
            format = ".png";
        }
        std::string filename = settingContent->imageSavePath + id + format;
        if (saveImage(filename, settingContent, cimg))
        {
            saveFileUrl = filename;
        }
        else
        {
            saveFileUrl = "Can Not Saved.";
        }
    }

    //兼容旧方案和第三方模板
    if (childrenRect.empty())
    {
        std::vector<cv::Vec4i> hierarchy;
        std::vector<std::vector<cv::Point> > contours;
        std::vector<std::vector<cv::Point> > fillContours;
        double threshold = digitalMat[0].rows * 0.1;
        int height = digitalMat[0].rows;
        int weight = digitalMat[0].cols;
        int centerX = weight / 2;
        int centerY = height / 2;

        int  halfDiagonalLine = sqrt(centerX*centerX + centerY*centerY);

        for (int i = 0; i < digitalMat.size(); i++)
        {

            cv::Mat srcClone = digitalMat[i].clone();
            //cv::imwrite("D:\\aDongNiShibie\\handwrite\\question\\result1.png", srcClone);
            cv::Mat srcClone2 = digitalMat[i].clone();
            int noeZeroNumber = cv::countNonZero(srcClone2);

            contours.clear();
            fillContours.clear();
            cv::findContours(srcClone, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

            // 去除噪声
            if (contours.size() > 1)
            {
                for (size_t j = 0; j < contours.size(); j++)
                {
                    cv::Rect box = boundingRect(contours[j]);

                    int boxX = box.x;
                    int boxY = box.y;
                    int  distance = sqrt((boxX - centerX)*(boxX - centerX) + (boxY - centerY)*(boxY - centerY));
                    threshold = threshold + distance / halfDiagonalLine;

                    if (box.height < threshold && box.width < threshold)
                    {
                        fillContours.push_back(contours[j]);
                        continue;
                    }
                }

                cv::drawContours(digitalMat[i], fillContours, -1, cv::Scalar(0), CV_FILLED, 8);
                //cv::imwrite("D:\\aDongNiShibie\\handwrite\\question\\result2.png" , digitalMat[i]);

            }

            // 提取最大轮廓
            cv::Mat mat(digitalMat[i].rows, digitalMat[i].cols, CV_8UC1, cv::Scalar(0));
            fillContours.clear();
            cv::findContours(digitalMat[i], fillContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
            std::vector<std::vector<cv::Point> > otherfillContours;
            // 最大轮廓的外接矩形
            cv::Rect box;

            if (fillContours.size() > 1) {
                std::vector<std::int64_t> area;
                for (size_t i = 0; i < fillContours.size(); i++)
                {
                    area.push_back(cv::contourArea(fillContours[i]));
                }
                int index = 0;
                int areaMax = 0;
                for (size_t i = 0; i < area.size(); i++)
                {
                    int are = area[i];
                    if (are > areaMax) {
                        areaMax = are;
                        index = i;
                    }
                }
                std::vector<std::vector<cv::Point> > fillContourFinally;
                fillContourFinally.push_back(fillContours[index]);

                box = boundingRect(fillContours[index]);

                for (size_t i = 0; i < fillContours.size(); i++)
                {
                    if (i != index) {
                        otherfillContours.push_back(fillContours[i]);
                    }
                }
                cv::drawContours(mat, fillContourFinally, -1, cv::Scalar(255, 255, 255), 5, 8);
            }
            else {
                mat = digitalMat[i];
            }
            //cv::imwrite("D:\\aDongNiShibie\\handwrite\\question\\result3.png", mat);


            // 判断右上角是否有小横杠
            int x = box.x;
            int y = box.y;
            int width = box.width;
            int height = box.height;

            int biasX = width / 2;
            int biasY = height / 2;

            int threshX = x + biasX;
            int threshY = y + biasY;
            // 小横杠轮廓
            std::vector<cv::Point> contoursHorizontal;
            for (size_t i = 0; i < otherfillContours.size(); i++)
            {
                std::vector<cv::Point> contours = otherfillContours[i];
                cv::Rect box = boundingRect(contours);
                int width = box.width;
                int height = box.height;
                // 轮廓面积
                int area = cv::contourArea(contours);
                int rectArea = width * height;

                // 轮廓面积/矩形面积
                double areaRate = area / rectArea;
                // 长宽比
                double hwRate = height / width;
                // 满足阈值 则判断为小横杠
                if (hwRate < 0.4 && areaRate >0.8) {
                    // 小横杠左下角坐标
                    int x = box.x;
                    int y = box.y + height;

                    if (x > threshX && y < threshY) {
                        for (size_t i = 0; i < contours.size(); i++)
                        {
                            contoursHorizontal.push_back(contours[i]);
                        }
                        break;
                    }
                }
            }

            // tensorflow识别
            std::vector<cv::Mat> digitalMatTemp;
            cv::Mat srcMat = mat.clone();
            cv::Mat kernel2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
            cv::dilate(srcMat, srcMat, kernel2);
            parting_stable_single(srcMat);
            digitalMatTemp.push_back(srcMat);

            std::vector<std::string> pre_value = Predictions(digitalMatTemp);
            std::string value = pre_value[0];
            if (value == "3" && contoursHorizontal.size()>0) {
                cv::drawContours(mat, contoursHorizontal, -1, cv::Scalar(255, 255, 255), CV_FILLED, 8);
                //imwrite("D:\\answercordPicture\\" + SuperContent::data + "\\result5_" + std::to_string(i) + "_" + ".png", srcClone);
            }

            digitalMat[i] = mat;

            //膨胀处理 优化书写或扫描过细问题
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
            cv::dilate(digitalMat[i], digitalMat[i], kernel);

            //cv::imwrite("D:\\aDongNiShibie\\handwrite\\question\\result4.png", digitalMat[i]);

            if (noeZeroNumber == 0) {
                digitalMat[i] = srcClone2;
            }
            parting_stable_single(digitalMat[i]);

        }
    }
    else
    {
        std::vector<cv::Vec4i> hierarchy;
        std::vector<cv::Point> maxContours;
        std::vector<std::vector<cv::Point>> allContours;
        std::vector<std::vector<cv::Point>> fillContours;
        double distanceThreshold = childrenRect[0].height / 7;
        for (int matInx = 0; matInx < digitalMat.size(); matInx++)
        {
            hierarchy.clear();
            maxContours.clear();
            allContours.clear();
            fillContours.clear();
            cv::findContours(digitalMat[matInx], allContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0)); //TODO RETR_LIST
            if (allContours.size() > 1)
            {
                double area = 0;
                for (size_t j = 0; j < allContours.size(); j++)
                {
                    double coutourArea = contourArea(allContours[j]);
                    if (coutourArea > area)
                    {
                        maxContours = allContours[j];
                        area = coutourArea;
                    }
                }

                cv::Rect childrenRectItem = childrenRect[matInx];
                if (!maxContours.empty()) {
                    cv::Rect box = boundingRect(maxContours);
                    if (box.height < distanceThreshold)
                    {
                        digitalMat[matInx] = cv::Mat(digitalMat[matInx].size(), digitalMat[matInx].type(), cv::Scalar(0));
                    }
                    else
                    {
                        //删除虚线框，如果目标轮廓和虚线重叠则保留
                        cv::Mat_<uchar> matChars = digitalMat[matInx];
                        int itemHeight = childrenRectItem.height;
                        int itemWidth = childrenRectItem.width;
                        int height = cv::min((itemHeight + childrenRectItem.y), digitalMat[matInx].rows - 1);
                        int width = cv::min((itemWidth + childrenRectItem.x), digitalMat[matInx].cols - 1);
                        for (int i = childrenRectItem.y, y = 0; i < height; i++, y++)
                        {
                            for (int j = childrenRectItem.x, x = 0; j < width; j++, x++)
                            {
                                if (y < 15 || y >(itemHeight - 15))
                                {
                                    if (x < (itemWidth * 0.3) || x >(itemWidth * 0.7))
                                    {
                                        if (pointPolygonTest(maxContours, cv::Point(j, i), false) == -1)
                                            matChars(i, j) = 0;
                                    }
                                }
                                if (x < 15 || x >(itemWidth - 15))
                                {
                                    if (y < (itemHeight * 0.4) || y >(itemHeight * 0.6))
                                    {
                                        if (pointPolygonTest(maxContours, cv::Point(j, i), false) == -1)
                                            matChars(i, j) = 0;
                                    }
                                }
                            }
                        }

                        //轮廓距离排序，以目标轮廓为中心扩散排序
                        std::vector<int> minDistanceVector;
                        unsigned int msize = maxContours.size();
                        for (int rdInx = 0; rdInx < allContours.size(); rdInx++)
                        {
                            double minDistance = 1000;
                            std::vector<cv::Point> contours = allContours[rdInx];
                            unsigned int size = contours.size();
                            for (int rdInx2 = 0; rdInx2 < size; rdInx2++)
                            {
                                cv::Point p1 = contours[rdInx2];
                                for (int mInx = 0; mInx < msize; mInx++)
                                {
                                    cv::Point p0 = maxContours[mInx];
                                    double distance = sqrt(powf((p0.x - p1.x), 2) + powf((p0.y - p1.y), 2));
                                    if (distance < minDistance)
                                        minDistance = distance;
                                }
                            }
                            minDistanceVector.push_back(minDistance);
                        }
                        for (int minInx1 = 0; minInx1 < minDistanceVector.size() - 1; ++minInx1)
                        {
                            for (int minInx2 = 1; minInx2 < minDistanceVector.size(); ++minInx2)
                            {
                                if (minDistanceVector[minInx2] < minDistanceVector[minInx2 - 1])
                                {
                                    swap(allContours[minInx2], allContours[minInx2 - 1]);
                                    cv::swap(minDistanceVector[minInx2], minDistanceVector[minInx2 - 1]);
                                }
                            }
                        }

                        //保留距离接近的轮廓
                        bool needReMinDistance = false;
                        for (int rdInx = 0; rdInx < allContours.size(); rdInx++)
                        {
                            std::vector<cv::Point> contours = allContours[rdInx];
                            unsigned int size = contours.size();

                            //重新计算距离
                            if (needReMinDistance)
                            {
                                msize = maxContours.size();
                                double minDistance = 1000;
                                for (int rdInx2 = 0; rdInx2 < size; rdInx2++)
                                {
                                    cv::Point p1 = contours[rdInx2];
                                    for (int mInx = 0; mInx < msize; mInx++)
                                    {
                                        cv::Point p0 = maxContours[mInx];
                                        double distance = sqrt(powf((p0.x - p1.x), 2) + powf((p0.y - p1.y), 2));
                                        if (distance < minDistance)
                                            minDistance = distance;
                                    }
                                }
                                minDistanceVector[rdInx] = minDistance;
                            }

                            //删除当前轮廓
                            if (minDistanceVector[rdInx] > distanceThreshold) {
                                std::vector<std::vector<cv::Point> > tempContours;
                                tempContours.push_back(allContours[rdInx]);
                                cv::drawContours(digitalMat[matInx], tempContours, -1, cv::Scalar(0), CV_FILLED, 8);
                                tempContours.clear();
                            }
                            else
                            {
                                double area = contourArea(contours);
                                if (area < distanceThreshold * 2)
                                {
                                    std::vector<std::vector<cv::Point> > tempContours;
                                    tempContours.push_back(allContours[rdInx]);
                                    cv::drawContours(digitalMat[matInx], tempContours, -1, cv::Scalar(0), CV_FILLED, 8);
                                    tempContours.clear();
                                }
                                else
                                {
                                    fillContours.push_back(contours);
                                    for (int i = 0; i < size; i++)
                                        maxContours.push_back(contours[i]);
                                    needReMinDistance = true;
                                }
                            }
                        }

                        //处理噪声
                        if (maxContours.size() > 0)
                        {
                            cv::Rect box = boundingRect(maxContours);
                            float w = box.width;
                            float h = box.height;
                            if (h < (childrenRectItem.height * 0.2))
                                cv::drawContours(digitalMat[matInx], allContours, -1, cv::Scalar(0), CV_FILLED, 8);
                        }
                    }
                }
            }

            //膨胀处理 优化书写或扫描过细问题
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
            dilate(digitalMat[matInx], digitalMat[matInx], kernel);
            parting_stable_single(digitalMat[matInx]);
        }
    }




    //返回结果
    std::vector<string> pre_value = Predictions(digitalMat);
    isCorrectInput = true;
    string ss = "";
    for (int i = 0; i < pre_value.size(); i++)
    {
        ss += pre_value[i];
    }
    result = ss;
}
