#include "absentflag.h"



void AbsentFlag::Do(Mat& img)
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

    medianBlur(cimg, cimg, 3);
    threshold(cimg, cimg, 128, 255, THRESH_OTSU);

    double standRect = settingContent->choiceSize.area();  //标准填涂块的大小
                                                           //求质心圆直径方案
    if (isCircleDiameter)
    {
        int circleRadius = 0, circleDiameter = 0;
        circleRadius = getCentroidCircleRadius(cimg);
        circleDiameter = circleRadius * 2;
        isCorrectInput = true;
        result = circleDiameter;
    }
    else
    { //像素占比方案
        if (settingContent->isMorphology)
        {
            int ks = (int)(settingContent->choiceSize.height) / 8 * 2 + 1;
            if (ks < 3)ks = 3;
            Mat kernel = getStructuringElement(MORPH_RECT, Size(ks, ks));
            dilate(cimg, cimg, kernel);
            erode(cimg, cimg, kernel);
        }

        int* cns = new int[2];
        int cn = 0;

        float proportion = 0;

        if (orientation == Horizontal)
        {
            float wstep = cimg.cols*1.0f / 2;
            for (int col = 1; col < 2; col++)
            {
                Rect numRect(floor(col*wstep), 0, floor(wstep), rect.height);
                Mat num_img = cimg(numRect);
                int noZeroCount = countNonZero(num_img);
                int zeroCount = numRect.area() - noZeroCount;

                proportion = zeroCount*1.0f / numRect.area();

                if (zeroCount > standRect*settingContent->selectedThreshold)
                {
                    cns[cn++] = col;
                }
            }
        }
        else
        {
            float hstep = cimg.rows * 1.0f / 2;
            for (int row = 1; row < 2; row++)
            {
                Rect numRect(0, floor(row*hstep), rect.width, hstep);
                Mat num_img = cimg(numRect);
                int noZeroCount = countNonZero(num_img);
                int zeroCount = numRect.area() - noZeroCount;

                proportion = zeroCount*1.0f / numRect.area();

                if (zeroCount > standRect*settingContent->selectedThreshold)
                {
                    cns[cn++] = row;
                }
            }
        }

        isCorrectInput = true;

        if (settingContent->selectedThreshold > 0)
        {
            if (cn == 1)
            {
                result = 1;
            }
            else
            {
                result = 0;
            }
        }
        else
        {
            result = proportion;
        }

        delete[] cns;
    }
}
