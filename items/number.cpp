#include "number.h"
#include "../utils/imagemat.h"
#include "../utils/util.h"

void Number::Do(cv::Mat& img)
{
    cv::Rect cRect(rect.x, rect.y, rect.width, rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }
    //图像区域
    cv::Mat cimg;
    img(cRect).copyTo(cimg);

    //处理和旋转图像
    if (rotation > 360 || rotation < -360) rotation = 0;
    if (rotation == 180)
    {
        flip(cimg, cimg, -1);
    }
    else if (rotation == 90)
    {
        flip(cimg, cimg, 1);
        transpose(cimg, cimg);
    }
    else if (rotation == -90)
    {
        flip(cimg, cimg, 0);
        transpose(cimg, cimg);
    }

    if (isSaveImage)
    {
        std::string format = ".jpg";
        if (settingContent->saveImageFormat == Format_PNG)
            format = ".png";
        std::string filename = settingContent->imageSavePath + id + format;
        ImageMat imageMat;

        if (imageMat.saveImage(filename, settingContent, cimg))
            saveFileUrl = filename;
        else
            saveFileUrl = "Can Not Saved";
    }

    medianBlur(cimg, cimg, 3);
    threshold(cimg, cimg, 128, 255, cv::THRESH_OTSU);

    if (settingContent->isMorphology)
    {
        int cw = settingContent->choiceSize.width;
        int ch = settingContent->choiceSize.height;
        int ridus = std::min(cw, ch) / 10 * 2 + 1;
        if (ridus < 3)ridus = 3;
        cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(ridus, ridus));
        morphologyEx(cimg, cimg, cv::MORPH_CLOSE, kernel);
    }

    Util util;
    double standRect = settingContent->choiceSize.area();  //标准填涂块的大小

    bool isCorrect = true;
    int* sn = new int[itemNumber];
    int correctCount = 0;

    Json::Value rawValues;

    if (orientation == Vertical)
    {
        float wstep = cimg.cols * 1.0f / itemNumber;
        float hstep = cimg.rows / 10.0f;

        cv::Mat cloneimg = cimg.clone();

        float standarheight = util.horizontalProject(cloneimg, itemNumber);

        cv::transpose(cloneimg, cloneimg);
        cv::flip(cloneimg, cloneimg, 0);

        float standarweight = util.horizontalProject(cloneimg, 10);

        float standaRate = standarweight / standarheight;
        float standarArea = standarweight * standarheight;

        // 判断是否需要矫正 x,y
        float areaMargin = std::min(wstep, hstep) / 8.0f;

        int x = floor(0 * wstep + areaMargin);
        int y = floor(0 * hstep + areaMargin);
        int width = floor(wstep - 2 * areaMargin);
        int height = floor(hstep - 2 * areaMargin);

        if (width < standarweight) width = standarweight - width + 3;
        if (height < standarheight) height = standarheight - height + 3;

        cv::Rect numRect(x, y, width, height);
        cv::Mat num_img = cimg(numRect);

        bool modifyCoord = true;
        std::vector<int>  matchCoord = util.objectDetect(num_img, standarweight, standarheight);
        int xStart = matchCoord[0];
        int yStart = matchCoord[1];
        if (xStart != -1 && yStart != -1) {
            modifyCoord = false;
        }
        if (modifyCoord) {
            matchCoord.clear();

            cv::Rect numRect(x, y, width * 1.5, 2 * height);
            cv::Mat num_img = cimg(numRect);
            matchCoord = util.objectDetect(num_img, standarweight, standarheight);
        }

        int biasx = 0;
        int biasy = 0;
        if (matchCoord[0] > 0 && matchCoord[1] > 0) {
            int halfwidthDis = (width - standarweight) / 2;
            int halfHeightDis = (height - standarheight) / 2;

            int bestx = 0;
            if (matchCoord[0] > halfwidthDis) {
                bestx = matchCoord[0] - halfwidthDis;
            }

            int besty = 0;
            if (matchCoord[1] > halfHeightDis) {
                besty = matchCoord[1] - halfHeightDis;
            }


            biasx = bestx - areaMargin;
            biasy = besty - areaMargin;
        }


        for (int col = 0; col < itemNumber; col++)
        {
            if (settingContent->selectedThreshold > 0)
            {
                int maxZeroCount = 0;
                int num = 0;

                for (int row = 0; row < 10; row++)
                {
                    cv::Rect numRect(floor(col*wstep), floor(row*hstep), floor(wstep), floor(hstep));
                    cv::Mat num_img = cimg(numRect);
                    int noZeroCount = countNonZero(num_img);
                    int zeroCount = numRect.area() - noZeroCount;

                    if (maxZeroCount < zeroCount)
                    {
                        maxZeroCount = zeroCount;
                        num = row;
                    }
                }
                if (maxZeroCount < standRect*settingContent->selectedThreshold)
                {
                    isCorrect = false;
                    break;
                }
                sn[col] = num;
                correctCount++;
            }
            else
            {
                Json::Value itemValues;
                float areaMargin = std::min(wstep, hstep) / 8.0f;
                for (int row = 0; row < 10; row++)
                {
                    int x = floor(col*wstep + areaMargin) + biasx;
                    int y = floor(row*hstep + areaMargin) + biasy;
                    int width = floor(wstep - 2 * areaMargin);
                    int height = floor(hstep - 2 * areaMargin);

                    if (x < 0)x = 0;
                    if (y < 0)y = 0;

                    if (x + width > cimg.cols) {
                        x = cimg.cols - width - 1;
                    }
                    if (y + height > cimg.rows) {
                        y = cimg.rows - height - 1;
                    }

                    cv::Rect numRect(x, y, width, height);
                    cv::Mat num_img = cimg(numRect);

                    int noZeroCount = countNonZero(num_img);
                    int zeroCount = numRect.area() - noZeroCount;

                    itemValues.append(zeroCount*1.0f / numRect.area());
                }
                rawValues.append(itemValues);
            }

        }
    }
    else
    {
        float wstep = cimg.cols / 10.0f;
        float hstep = cimg.rows *1.0f / itemNumber;

        for (int row = 0; row < itemNumber; row++)
        {
            if (settingContent->selectedThreshold > 0)
            {
                int maxZeroCount = 0;
                int num = 0;
                for (int col = 0; col < 10; col++)
                {
                    cv::Rect numRect(floor(col*wstep), floor(row*hstep), floor(wstep), floor(hstep));
                    cv::Mat num_img = cimg(numRect);
                    int noZeroCount = countNonZero(num_img);
                    int zeroCount = numRect.area() - noZeroCount;

                    if (maxZeroCount < zeroCount)
                    {
                        maxZeroCount = zeroCount;
                        num = col;
                    }
                }
                if (maxZeroCount < standRect*settingContent->selectedThreshold)
                {
                    isCorrect = false;
                    break;
                }
                sn[row] = num;
                correctCount++;
            }
            else
            {
                Json::Value itemValues;
                float areaMargin = std::min(wstep, hstep) / 8.0f;
                for (int col = 0; col < 10; col++)
                {
                    cv::Rect numRect(floor(col*wstep + areaMargin), floor(row*hstep + areaMargin), floor(wstep - 2 * areaMargin), floor(hstep - 2 * areaMargin));
                    cv::Mat num_img = cimg(numRect);
                    int noZeroCount = countNonZero(num_img);
                    int zeroCount = numRect.area() - noZeroCount;

                    itemValues.append(zeroCount*1.0f / numRect.area());
                }
                rawValues.append(itemValues);
            }
        }
    }

    if (settingContent->selectedThreshold > 0)
    {
        char str[32];
        std::string ss = "";
        for (int i = 0; i < correctCount; i++)
        {
            sprintf(str, "%d", sn[i]);
            ss += str;
        }

        if (correctCount>0)
        {
            isCorrectInput = true;
            result = ss;
        }
        else
        {
            isCorrectInput = false;
            result = "No student number";
        }
    }
    else
    {
        isCorrectInput = true;
        result = rawValues;
    }
    delete[] sn;
}


