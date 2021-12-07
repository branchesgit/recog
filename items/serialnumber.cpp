#include "serialnumber.h"
#include "../utils/imagemat.h"

using namespace cv::ml;

Ptr<SVM> DigitSVMModel;
Ptr<SVM> TickSVMModel;

void SerialNumber::Do(Mat& img)
{
    Rect cRect(rect.x, rect.y, rect.width, rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }
    //图像区域
    Mat cimg;
    img(cRect).copyTo(cimg);
    //保存图像
    if (isSaveImage)
    {
        string format = ".jpg";
        if (settingContent->saveImageFormat == Format_PNG)
            format = ".png";
        string filename = settingContent->imageSavePath + id + format;
        ImageMat imageMat;
        if (imageMat.saveImage(filename, settingContent, cimg))
            saveFileUrl = filename;
        else
            saveFileUrl = "Can Not Saved";
    }

    //处理和旋转图像
    if (rotation > 360 || rotation < -360) rotation = 0;

    if (orientation == Horizontal && rotation == 180)
    {
        flip(cimg, cimg, -1);
    }
    else if (orientation == Horizontal && rotation == 90)
    {
        flip(cimg, cimg, 1);
        transpose(cimg, cimg);
    }
    else if (orientation == Horizontal && rotation == -90)
    {
        flip(cimg, cimg, 0);
        transpose(cimg, cimg);
    }
    else if (orientation == Vertical && rotation == 180)
    {
        flip(cimg, cimg, -1);
    }
    else if (orientation == Vertical && rotation == 90)
    {
        flip(cimg, cimg, 1);
        transpose(cimg, cimg);
    }
    else if (orientation == Vertical && rotation == -90)
    {
        flip(cimg, cimg, 0);
        transpose(cimg, cimg);
    }

    threshold(cimg, cimg, 50, 255, THRESH_OTSU);
    cimg = 255 - cimg;

    Mat simg = cimg.clone();
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    findContours(simg, contours, hierarchy, 0, CHAIN_APPROX_SIMPLE);

    int* cns = new int[128];
    int* xypt = new int[128];
    int cn = 0;

    const int HIST_COUNT = 45;
    float hist[HIST_COUNT];
    //要判断顺序
    for (int idx = 0; idx < contours.size(); idx = hierarchy[idx][0])
    {
        double area = contourArea(contours[idx]);
        if (area < 9) continue;

        Rect rect = boundingRect(contours[idx]);
        if ((rect.width*rect.height) >(1.5*cimg.rows*cimg.cols / itemNumber))continue;//x
        if (area / (rect.width*rect.height)<0.05f) continue;//x
        if (rect.x == 0 || rect.y == 0 || rect.x + rect.width == cimg.cols || rect.y + rect.height == cimg.rows)
            continue;

        if ((rect.width*1.0 / rect.height)<0.3)
        {
            cns[cn] = 1;
            if (orientation == Horizontal)
                xypt[cn] = rect.x;
            else
                xypt[cn] = rect.y;
            cn++;
            continue;
        }

        Mat dmat;
        cimg(rect).copyTo(dmat);

        resize(dmat, dmat, Size(10, 18));
        memset(hist, 0, sizeof(float)* HIST_COUNT);

        int chid = 0;
        for (int chr = 0; chr < 9; chr++)
        {
            for (int chc = 0; chc < 5; chc++)
            {
                Rect chrect(chc * 2, chr * 2, 2, 2);
                Mat chmat = dmat(chrect);

                hist[chid++] = sum(chmat)[0] / (4.0f * 255);
            }
        }

        Mat pmat(1, HIST_COUNT, CV_32F, hist);
        float rd = DigitSVMModel->predict(pmat);
        cns[cn] = (int)(rd);
        if (orientation == Horizontal)
            xypt[cn] = rect.x;
        else
            xypt[cn] = rect.y;
        cn++;
    }

    if (cn == itemNumber)
    {
        isCorrectInput = true;
        char str[32];
        string ss = "";
        for (int i = 0; i < cn; i++)
        {
            //获取x坐标最小的索引
            int ix = 0;
            int minIX = xypt[0];
            for (int is = 0; is < cn; is++)
            {
                if (minIX>xypt[is])
                {
                    ix = is;
                    minIX = xypt[is];
                }
            }
            xypt[ix] = 2147483647;

            sprintf(str, "%d", cns[ix]);
            ss += str;
        }

        result = ss;
    }
    else
    {
        isCorrectInput = false;
        result = "Recognition failed!";
    }

    delete[] cns;
    delete[] xypt;
}
