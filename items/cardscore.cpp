#include "cardscore.h"

CardScore::CardScore()
{

}

void CardScore::Do(Mat& img)
{
    Rect cRect(rect.x, rect.y, rect.width, rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }
    Mat cimg;
    img(cRect).copyTo(cimg);

    if (isSaveImage)
    {
        string format = ".jpg";
        if (settingContent->saveImageFormat == Format_PNG)
            format = ".png";
        string filename = settingContent->imageSavePath + id + format;
        if (saveImage(filename, settingContent, cimg))
            saveFileUrl = filename;
        else
            saveFileUrl = "Can Not Saved";
    }

    threshold(cimg, cimg, 50, 255, THRESH_OTSU);

    //缩放到标准高度
    float deltaY = 0;
    if (orientation == Horizontal)
    {
        deltaY = cimg.rows*1.0f / row;
    }
    else
    {
        deltaY = cimg.rows*1.0f / itemNumber;
    }
    if (deltaY > 40 || deltaY < 25)
    {
        float f = 32.0f / deltaY;
        resize(cimg, cimg, Size(f*cimg.cols, f*cimg.rows));

        threshold(cimg, cimg, 250, 255, THRESH_BINARY);
    }

    //移除孤立点
    for (int r = 0; r < cimg.rows; r++)
    {
        for (int c = 0; c < cimg.cols; c++)
        {
            int rt = r - 1;
            int rb = r + 1;
            int cl = c - 1;
            int cr = c + 1;

            int sum = 0;
            int si = 0;

            if (rt >= 0)
            {
                sum += cimg.data[rt*cimg.cols + c];
                si++;
            }
            if (rb < cimg.rows)
            {
                sum += cimg.data[rb*cimg.cols + c];
                si++;
            }
            if (cl >= 0)
            {
                sum += cimg.data[r*cimg.cols + cl];
                si++;
            }
            if (cr<cimg.cols)
            {
                sum += cimg.data[r*cimg.cols + cr];
                si++;
            }

            if (sum == 0)
            {
                cimg.data[r*cimg.cols + c] = 0;
            }
            else if (sum == si * 255)
            {
                cimg.data[r*cimg.cols + c] = 255;
            }
        }
    }

    Mat qurey_sobel_dx, qurey_sobel_dy;
    Sobel(cimg, qurey_sobel_dx, CV_32F, 1, 0, 1);
    Sobel(cimg, qurey_sobel_dy, CV_32F, 0, 1, 1);
    Mat qurey_mg, qurey_ag;
    cartToPolar(qurey_sobel_dx, qurey_sobel_dy, qurey_mg, qurey_ag);

    Mat maxResultMat = Mat::zeros(qurey_mg.size(), CV_32FC1);

    for (int i = 0; i < TickTempDataCount; i++)
    {
        Mat temp_ag(TickTempDatas[i].rows, TickTempDatas[i].cols, CV_32F, TickTempDatas[i].ag_data);
        Mat temp_mg(TickTempDatas[i].rows, TickTempDatas[i].cols, CV_32F, TickTempDatas[i].mg_data);
        Mat res = matchTickTemplate(qurey_ag, cimg, temp_ag, temp_mg);
        int standMatch = countNonZero(temp_mg);

        Mat resFloat;
        res.convertTo(resFloat, CV_32F, 1.0 / standMatch);
        max(maxResultMat, resFloat, maxResultMat);
    }

    float THRESH_VALUE = 0.65f;
    isCorrectInput = foundTricks(maxResultMat, qurey_sobel_dy, orientation, row, itemNumber, THRESH_VALUE, result);
}
