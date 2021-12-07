#include "util.h"

Util::Util()
{

}

float  Util::horizontalProject(cv::Mat mat, int itemNumber) {

    cv::Mat cmat;
    mat.copyTo(cmat);

    cmat.convertTo(cmat, CV_32F);

    int weight = cmat.cols;
    int height = cmat.rows;

    std::vector<int> horizontalVals;
    for (size_t i = 0; i < height; i++)
    {
        int zeroNumber = 0;
        for (size_t j = 0; j < weight; j++)
        {
            float val = cmat.at<float>(i, j);
            if (val == 0) {
                zeroNumber++;
            }
        }
        horizontalVals.push_back(zeroNumber);
    }

    std::vector<std::vector<int>>  horizontalArr;
    float threshVal = weight / (itemNumber * 2);
    for (size_t i = 0; i < horizontalVals.size(); i++)
    {
        std::vector<int> horizontalTwoVal;
        int val = horizontalVals[i];
        if (val > threshVal && horizontalTwoVal.size() == 0) {
            int nubmer = 0;
            for (size_t j = 0; j < 5; j++)
            {
                int val = horizontalVals[i + j];
                if (val > threshVal) {
                    nubmer++;
                }
            }
            if (nubmer == 5) {
                horizontalTwoVal.push_back(i);
            }
        }


        if (val > threshVal && horizontalTwoVal.size() == 1) {

            for (size_t j = i; j < horizontalVals.size(); j++)
            {
                int val = horizontalVals[j];
                int val2 = horizontalVals[j + 1];

                if (val > threshVal && val2 < threshVal) {
                    horizontalTwoVal.push_back(j);
                    i = j;
                    horizontalArr.push_back(horizontalTwoVal);
                    break;
                }
            }
        }
    }

    std::vector<int> diffArr;
    for (size_t i = 0; i < horizontalArr.size(); i++)
    {
        std::vector<int> tempVal = horizontalArr[i];
        int diff = tempVal[1] - tempVal[0];
        diffArr.push_back(diff);
    }

    std::sort(diffArr.begin(), diffArr.end());
    int medium = diffArr.size() / 2;
    int mediumVal = diffArr[medium];


    std::vector<int> diffArrFilter;
    for (size_t i = 0; i < diffArr.size(); i++)
    {
        int tempVal = diffArr[i];
        int diff = abs(tempVal - mediumVal);
        if (diff < mediumVal*0.35) {
            diffArrFilter.push_back(diffArr[i]);
        }
    }

    int sum = 0;
    for (size_t i = 0; i < diffArrFilter.size(); i++)
    {
        sum = sum + diffArrFilter[i];
    }
    float standarheight = sum / diffArrFilter.size();
    return standarheight;
}


// 目标检测
std::vector<int>  Util::objectDetect(cv::Mat search, float standarweight, float standarheight) {


    int seachWidth = search.cols;
    int seachHeight = search.rows;


    int seachEndY = seachHeight - standarheight;
    int seachEndX = seachWidth - standarweight;

    int  bestMatchValue = 10000;
    int bestX = -1;
    int bestY = -1;

    for (int sy = 0; sy <= seachEndY; sy++)
    {
        for (int sx = 0; sx <= seachEndX; sx++)
        {
            cv::Rect region(sx, sy, standarweight, standarheight);
            cv::Mat mat;
            search(region).copyTo(mat);

            int zeroNumber = standarweight*standarheight - cv::countNonZero(mat);
            int thre = standarweight*standarheight * 0.45;
            if (zeroNumber < thre) {
                continue;
            }

            int halfweight = (int)standarweight / 2;

            cv::Rect rightRect(0, 0, halfweight, standarheight);
            cv::Mat rightmat;
            mat(rightRect).copyTo(rightmat);

            cv::Rect leftRect(halfweight + 1, 0, standarweight - halfweight - 1, standarheight);
            cv::Mat leftmat;
            mat(leftRect).copyTo(leftmat);

            int rightzeroNumber = rightmat.cols*rightmat.rows - cv::countNonZero(rightmat);
            int leftzeroNumber = leftmat.rows*leftmat.cols - cv::countNonZero(leftmat);

            int diffPix = abs(rightzeroNumber - leftzeroNumber);

            int thresh = rightmat.cols*rightmat.rows / 2 * 0.1;
            if (diffPix > thresh) {
                continue;
            }

            int upheight = (int)standarheight / 2;

            cv::Rect upRect(0, 0, standarweight, upheight);
            cv::Mat upmat;
            mat(upRect).copyTo(upmat);

            cv::Rect downRect(0, upheight + 1, standarweight, standarheight - upheight - 1);
            cv::Mat downmat;
            mat(downRect).copyTo(downmat);

            int upzeroNumber = upmat.cols*upmat.rows - cv::countNonZero(upmat);
            int downzeroNumber = downmat.rows*downmat.cols - cv::countNonZero(downmat);

            int diffPixupdown = abs(upzeroNumber - downzeroNumber);


            int threshupdown = downmat.cols*downmat.rows / 2 * 0.1;
            if (diffPixupdown > threshupdown) {
                continue;
            }

            int diffsum = diffPixupdown + diffPix;
            if (bestMatchValue > diffsum) {
                bestMatchValue = diffsum;
                bestX = sx;
                bestY = sy;
            }

        }
    }

    std::vector<int>  point;
    point.push_back(bestX);
    point.push_back(bestY);

    return point;

}


void Util::RecognitionItem(Mat& img, SuperContent& content, int itemNumber, bool isSaveImage)
{
    //区域太小不操作
    if (content.rect.width < 3 || content.rect.height < 3)
        return;

    Rect cRect(content.rect.x, content.rect.y, content.rect.width, content.rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        content.isCorrectInput = false;
        content.result = "The rect is out of the image.";
        return;
    }

    Mat cimg;
    img(cRect).copyTo(cimg);

    if (isSaveImage)
    {
        string format = ".jpg";
        if (content.settingContent->saveImageFormat == Format_PNG)
            format = ".png";
        string filename = content.settingContent->imageSavePath + content.id + format;
        if (saveImage(filename, content.settingContent, cimg))
            content.saveFileUrl = filename;
        else
            content.saveFileUrl = "Can Not Saved";
    }

    //中值滤波
    medianBlur(cimg, cimg, 3);
    threshold(cimg, cimg, 128, 255, THRESH_OTSU);

    double standRect = content.settingContent->choiceSize.area(); //标准填涂块的大小

                                                                  //形态学操作
    if (content.settingContent->isMorphology)
    {
        int ks = (int)(content.settingContent->choiceSize.height) / 4 / 2 * 2 + 1;
        if (ks < 3)ks = 3;
        Mat kernel = getStructuringElement(MORPH_RECT, Size(ks, ks));
        dilate(cimg, cimg, kernel);
        erode(cimg, cimg, kernel);
    }

    int* cns = new int[itemNumber];
    int cn = 0;

    Json::Value rawValues;

    vector<Mat> optionAllMats;
    if (content.orientation == Horizontal)
    {
        float wstep = itemNumber == 1 ? cimg.cols : cimg.cols*1.0f / (itemNumber + 1);
        for (int col = 1, offsetCol = itemNumber == 1 ? 0 : col; col < itemNumber + 1; col++, offsetCol++)
        {
            Rect numRect(floor(offsetCol*wstep), 0, floor(wstep), content.rect.height);
            Mat num_img = cimg(numRect);
            int noZeroCount = countNonZero(num_img);
            int zeroCount = numRect.area() - noZeroCount;
            optionAllMats.push_back(num_img);
            rawValues.append(zeroCount*1.0f / standRect);

            if (zeroCount>standRect*content.settingContent->selectedThreshold)
            {
                cns[cn++] = col - 1;
            }
        }
    }
    else
    {
        float hstep = itemNumber == 1 ? cimg.rows : cimg.rows * 1.0f / (itemNumber + 1);
        for (int row = 1, offsetRow = itemNumber == 1 ? 0 : row; row < itemNumber + 1; row++, offsetRow++)
        {
            Rect numRect(0, floor(offsetRow*hstep), content.rect.width, hstep);
            Mat num_img = cimg(numRect);
            int noZeroCount = countNonZero(num_img);
            int zeroCount = numRect.area() - noZeroCount;
            optionAllMats.push_back(num_img);
            rawValues.append(zeroCount*1.0f / standRect);

            if (zeroCount>standRect*content.settingContent->selectedThreshold)
            {
                cns[cn++] = row - 1;
            }
        }
    }

    if (itemNumber > 1)
    {
        for (int i = 0; i < itemNumber; i++)
        {
            //求质心圆直径方案
            int circleRadius = getCentroidCircleRadius(optionAllMats[i]);
            float circleDiameterRate = float(circleRadius * 2) / float(content.settingContent->choiceSize.height);
            rawValues.append(circleDiameterRate < 0.1 ? 0.1 : circleDiameterRate);  //返回直径占比
        }
    }

    content.isCorrectInput = true;

    if (content.settingContent->selectedThreshold>0)
    {
        for (int i = 0; i < cn; i++)
        {
            content.result.append(cns[i]);
        }
    }
    else
    {
        content.result = rawValues;
    }

    delete[] cns;
}
