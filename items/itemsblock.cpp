#include "itemsblock.h"

ItemsBlock::ItemsBlock()
{

}


void ItemsBlock::Do(Mat& img)
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
    Mat cimg2;  //填涂圆使用
    img(cRect).copyTo(cimg);
    img(cRect).copyTo(cimg2);

    std::vector<int> countBalckPix;
    int width = cimg.cols;
    int height = cimg.rows;
    //灰色图像转换为黑色二值图像
    threshold(cimg, cimg, 0, 255, THRESH_OTSU);
    //统计图片每行黑像素点的个数
    if (orientation == Horizontal)//横向题目，即ABCD是横这排列的
    {
        for (int i = 0; i < height; i++)
        {
            cv::Mat B = cimg.rowRange(i, i + 1).clone();
            countBalckPix.push_back(width - cv::countNonZero(B));
        }
        //预设置判断白色行的阈值，若图片的第一行及最后一行黑像素点的个数都大于阈值，即不为白色，则需调整图片；
        int Threshold = (int)(0.05 * width);
        if (countBalckPix[0] <= Threshold && countBalckPix[height - 1] <= Threshold)
        {

        }
        //找到图片白色区域(小于阈值)的位置存入一个向量中
        else
        {
            std::vector<int> writerange;
            for (int i = 0; i < height; i++)
            {
                if (countBalckPix[i] <= Threshold)
                {
                    writerange.push_back(i);
                    sort(writerange.begin(), writerange.end());
                }
                else
                    continue;
            }
            SettingContent* tm = settingContent;
            float choiceHeight = (*tm).choiceRectHeight;
            if (writerange.size() == 0 || writerange[writerange.size() - 1] - writerange[0] >= (int)choiceHeight)
            {

            }
            //可通过该向量的第一个及最后一个位置坐标和的一半，即白色区域中心，与图片的中心比较进行平移调整，调整后的图片为占比较多的选择题。
            else
            {
                int whitecenter = (int)(writerange[0] + writerange[writerange.size() - 1]) / 2;
                int heightcenter = (int)(height / 2);
                if (whitecenter >= heightcenter)      //白色区域中心位置位于图片的中心的下方，图像上移
                {
                    int move_y = (int)(height - whitecenter);
                    Rect cRect(rect.x, rect.y - move_y, rect.width, rect.height);
                    img(cRect).copyTo(cimg);
                }
                else                      //否则下移
                {
                    Rect cRect(rect.x, rect.y + whitecenter, rect.width, rect.height);
                    img(cRect).copyTo(cimg);
                }
            }

        }
    }
    else   //竖向题目，即ABCD是竖着排列的
    {
        for (int j = 0; j < width; j++)
        {
            cv::Mat M = cimg.colRange(j, j + 1).clone();
            countBalckPix.push_back(height - cv::countNonZero(M));
        }
        //预设置阈值，若图片的第一列及最后一列黑像素点的个数都大于阈值，即不为白色，则需调整图片；
        int Threshold = (int)(0.05 * height);
        if (countBalckPix[0] <= Threshold && countBalckPix[width - 1] <= Threshold)
        {

        }
        else
        {
            std::vector<int> writerange;
            for (int j = 0; j < width; j++)
            {
                if (countBalckPix[j] <= Threshold)
                {
                    writerange.push_back(j);
                    sort(writerange.begin(), writerange.end());
                }

                else
                    continue;
            }
            SettingContent* tm = settingContent;
            float choiceWidth = (*tm).choiceRectWidth;
            if (writerange.size() == 0 || writerange[writerange.size() - 1] - writerange[0] >= (int)choiceWidth)
            {

            }
            else
            {
                int whitecenter = (int)(writerange[0] + writerange[writerange.size() - 1]) / 2;
                int widthcenter = (int)(width / 2);
                if (whitecenter >= widthcenter)     //左移
                {
                    int move_x = (int)(width - whitecenter);
                    Rect cRect(rect.x - move_x, rect.y, rect.width, rect.height);
                    img(cRect).copyTo(cimg);
                }
                else    //右移
                {
                    Rect cRect(rect.x + whitecenter, rect.y, rect.width, rect.height);
                    img(cRect).copyTo(cimg);
                }
            }

        }
    }

    //保存图像
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

    if (itemNumber <= 0 || questionAmount <= 0)
    {
        isCorrectInput = false;
        result = "Error Parameters!";
        return;
    }

    if (itemSize.width > 0 && itemSize.height > 0)
    {
        int gridCols = 0;  //一共有多少列
        int gridRows = 0; //一共有多少行  ceil(questionAmount*1.0f / itemsPerRow);

        if (orientation == Horizontal)//横向题目，即ABCD是横这排列的
        {
            gridCols = itemNumber*itemsPerRow;
            if (containNumber) gridCols += itemsPerRow;
            gridRows = ceil(questionAmount*1.0f / itemsPerRow);
        }
        else//竖向题目，即ABCD是竖着排列的
        {
            gridCols = itemsPerRow;
            int tempRows = ceil(questionAmount*1.0f / itemsPerRow);
            gridRows = tempRows*itemNumber;
            if (containNumber) gridRows += tempRows;
        }

        //图像倾斜校正和二值化
        threshold(cimg, cimg, 50, 255, THRESH_OTSU);
        if (gridCols > 3)
        {
            cimg = skew_correction_rot(cimg, Scalar(255), 5);
            threshold(cimg, cimg, 128, 255, THRESH_BINARY_INV);
        }
        else
        {
            cimg = 255 - cimg;
        }

        int rows = cimg.rows;
        int cols = cimg.cols;
        int itemRows = itemSize.height;
        int itemCols = itemSize.width;

        //横向分割图像
        int* rowHist = new int[rows];
        for (int i = 0; i < rows; i++)
        {
            int cnz = countNonZero(cimg.row(i));
            if (gridCols>2 && cnz > cols*0.8) cnz = 0;
            rowHist[i] = cnz;
        }

        float* rowCenters = new float[gridRows];
        int getCount = 0;
        while (getCount<gridRows)
        {
            int maxSum = 0;
            int ss = 0;
            for (int r = 0; r <= rows - itemRows; r++)
            {
                int tempSum = 0;
                for (int s = r; s < r + itemRows; s++)
                {
                    tempSum += rowHist[s];
                }
                if (maxSum < tempSum)
                {
                    maxSum = tempSum;
                    ss = r;
                }
            }

            if (maxSum < gridCols * 5) break;

            float cc = ss + itemRows*0.5;

            bool isNear = false;
            for (int n = 0; n < getCount; n++)
            {
                if (abs(rowCenters[n] - cc) < itemRows)
                {
                    isNear = true;
                    break;
                }
            }

            if (!isNear)
            {
                rowCenters[getCount++] = cc;
                if (getCount >= gridRows) break;
            }

            //填充0
            for (int s = ss - 2; s < ss + itemRows + 2; s++)
            {
                if (s < 0 || s >= rows)continue;
                rowHist[s] = 0;
            }
        }

        if (getCount<gridRows)
        {
            isCorrectInput = false;
            result = "Error Image!";
            delete[] rowHist;
            delete[] rowCenters;
            return;
        }

        //纵向分割图像
        int* colHist = new int[cols];
        for (int i = 0; i < cols; i++)
        {
            int cnz = countNonZero(cimg.col(i));
            if (gridRows>2 && cnz > rows*0.8) cnz = 0;
            colHist[i] = cnz;
        }

        float* colCenters = new float[gridCols];
        getCount = 0;
        while (getCount<gridCols)
        {
            int maxSum = 0;
            int ss = 0;
            for (int c = 0; c <= cols - itemCols; c++)
            {
                int tempSum = 0;
                for (int s = c; s < c + itemCols; s++)
                {
                    tempSum += colHist[s];
                }
                if (maxSum < tempSum)
                {
                    maxSum = tempSum;
                    ss = c;
                }
            }

            if (maxSum < gridRows * 5) break;

            float cc = ss + itemCols*0.5;

            bool isNear = false;
            for (int n = 0; n < getCount; n++)
            {
                if (abs(colCenters[n] - cc) < itemCols)
                {
                    isNear = true;
                    break;
                }
            }

            if (!isNear)
            {
                colCenters[getCount++] = cc;
                if (getCount >= gridCols) break;
            }

            //填充0
            for (int s = ss - 2; s < ss + itemCols + 2; s++)
            {
                if (s < 0 || s >= cols)continue;
                colHist[s] = 0;
            }
        }

        if (getCount<gridCols)
        {
            isCorrectInput = false;
            result = "Error Image!";
            delete[] rowHist;
            delete[] colHist;
            delete[] rowCenters;
            delete[] colCenters;
            return;
        }

        //横向中点排序
        for (int j = 0; j < gridRows - 1; j++)
        {
            for (int i = 0; i < gridRows - 1 - j; i++)
            {
                if (rowCenters[i] > rowCenters[i + 1])
                {
                    float temp = rowCenters[i];
                    rowCenters[i] = rowCenters[i + 1];
                    rowCenters[i + 1] = temp;
                }
            }
        }
        //纵向中点排序
        for (int j = 0; j < gridCols - 1; j++)
        {
            for (int i = 0; i < gridCols - 1 - j; i++)
            {
                if (colCenters[i] > colCenters[i + 1])
                {
                    int temp = colCenters[i];
                    colCenters[i] = colCenters[i + 1];
                    colCenters[i + 1] = temp;
                }
            }
        }

        //预处理图像
        if (settingContent->isMorphology)
        {
            int ridus = min(itemRows, itemCols) / 8 * 2 + 1;
            if (ridus < 3) ridus = 3;
            Mat kernel = getStructuringElement(MORPH_RECT, Size(ridus, ridus));
            morphologyEx(cimg, cimg, MORPH_OPEN, kernel);
        }

        float minRowDelta = rows;
        float minColDelta = cols;

        for (int i = 0; i < gridRows - 1; i++)
        {
            float delta = rowCenters[i + 1] - rowCenters[i];
            if (minRowDelta > delta)
            {
                minRowDelta = delta;
            }
        }
        for (int i = 0; i < gridCols - 1; i++)
        {
            float delta = colCenters[i + 1] - colCenters[i];
            if (minColDelta > delta)
            {
                minColDelta = delta;
            }
        }

        float rowRidus = minRowDelta / 2;
        float colRidus = minColDelta / 2;

        Mat checkedMat(gridRows, gridCols, CV_32FC1, Scalar(0));
        Mat optionAllCircle(gridRows, gridCols, CV_32FC1, Scalar(0));
        double minStandRect = std::min(settingContent->choiceSize.width, settingContent->choiceSize.height);
        double standRectArea = (settingContent->choiceSize.width * settingContent->choiceSize.height);
        for (int r = 0; r < gridRows; r++)
        {
            for (int c = 0; c < gridCols; c++)
            {
                int x = colCenters[c] - colRidus;
                int y = rowCenters[r] - rowRidus;
                int xw = x + minColDelta;
                int yh = y + minRowDelta;

                if (x < 0)x = 0;
                if (y < 0)y = 0;
                if (xw > cols)xw = cols;
                if (yh > rows)yh = rows;

                Rect sr(x, y, xw - x, yh - y);
                Mat srm = cimg(sr);

                int noZeroCount = countNonZero(srm);
                int circleRadius = getCentroidCircleRadius(cimg2(sr));

                optionAllCircle.at<float>(r, c) = float(circleRadius * 2) / float(minStandRect > 0 ? minStandRect : sr.height);

                if (settingContent->selectedThreshold > 0)
                {
                    if (noZeroCount>settingContent->selectedThreshold*itemRows*itemCols)
                    {
                        checkedMat.at<float>(r, c) = 255;
                    }
                }
                else
                {
                    checkedMat.at<float>(r, c) = noZeroCount*1.0f / (standRectArea > 0 ? standRectArea : sr.area());
                }
            }
        }

        for (int q = 0; q < questionAmount; q++)
        {
            Json::Value val;
            vector<double> tempOptionCircleVal;  //存储各选项占比
            for (int i = 0; i < itemNumber; i++)
            {
                int x, y;
                if (questionOrientation == Horizontal)  //题目排列方向为横向
                {
                    int qr = q / itemsPerRow;
                    int qc = q % itemsPerRow;

                    if (orientation == Horizontal) //选项排列方式为横向
                    {
                        if (containNumber)
                        {
                            x = qc*(itemNumber + 1) + i + 1;
                            y = qr;
                        }
                        else
                        {
                            x = qc*(itemNumber)+i;
                            y = qr;
                        }
                    }
                    else //选项排列方式为纵向
                    {
                        if (containNumber)
                        {
                            x = qc;
                            y = qr*(itemNumber + 1) + i + 1;
                        }
                        else
                        {
                            x = qc;
                            y = qr*(itemNumber)+i;
                        }
                    }
                }
                else
                {
                    int perColItems = ceil(questionAmount*1.0f / itemsPerRow);
                    int qr = q % perColItems;
                    int qc = q / perColItems;

                    if (orientation == Horizontal)
                    {
                        if (containNumber)
                        {
                            x = qc*(itemNumber + 1) + i + 1;
                            y = qr;
                        }
                        else
                        {
                            x = qc*(itemNumber)+i;
                            y = qr;
                        }
                    }
                    else
                    {
                        if (containNumber)
                        {
                            x = qc;
                            y = qr*(itemNumber + 1) + i + 1;
                        }
                        else
                        {
                            x = qc;
                            y = qr*(itemNumber)+i;
                        }
                    }
                }

                if (settingContent->selectedThreshold > 0)
                {
                    if (checkedMat.at<float>(y, x) >128)
                    {
                        val.append(i);
                        tempOptionCircleVal.push_back(optionAllCircle.at<float>(y, x));
                    }
                }
                else
                {
                    val.append(checkedMat.at<float>(y, x));
                    tempOptionCircleVal.push_back(optionAllCircle.at<float>(y, x));
                }
            }

            //求质心圆直径方案
            //当占比方差满足条件时则进一步判断填涂笔迹圆直径
            if (itemNumber > 1)
            {
                for (int i = 0; i < itemNumber; i++)
                {
                    val.append(tempOptionCircleVal[i] < 0.1 ? 0.1 : tempOptionCircleVal[i]);//返回直径占比
                }
            }

            result.append(val);
        }

        delete[] rowHist;
        delete[] colHist;
        delete[] rowCenters;
        delete[] colCenters;
    }
    else
    {
        int gridCols = 0; //一共有多少列
        int gridRows = 0; //一共有多少行  ceil(questionAmount*1.0f / itemsPerRow);

        if (orientation == Horizontal)//横向题目，即ABCD是横这排列的
        {
            gridCols = itemNumber*itemsPerRow;
            if (containNumber) gridCols += itemsPerRow;
            gridRows = ceil(questionAmount*1.0f / itemsPerRow);
        }
        else//竖向题目，即ABCD是竖着排列的
        {
            gridCols = itemsPerRow;
            int tempRows = ceil(questionAmount*1.0f / itemsPerRow);
            gridRows = tempRows*itemNumber;
            if (containNumber) gridRows += tempRows;
        }

        //图像倾斜校正和二值化
        threshold(cimg, cimg, 50, 255, THRESH_OTSU);
        if (gridCols > 3)
        {
            threshold(cimg, cimg, 128, 255, THRESH_BINARY_INV);
        }
        else
        {
            cimg = 255 - cimg;
        }

        int rows = cimg.rows;
        int cols = cimg.cols;

        float xstep = cols * 1.0f / gridCols;
        float ystep = rows * 1.0f / gridRows;

        Mat checkedMat(gridRows, gridCols, CV_32FC1, Scalar(0));
        Mat optionAllCircle(gridRows, gridCols, CV_32FC1, Scalar(0));
        double minStandRect = std::min(settingContent->choiceSize.width, settingContent->choiceSize.height);
        double standRectArea = (settingContent->choiceSize.width * settingContent->choiceSize.height);
        for (int r = 0; r < gridRows; r++)
        {
            for (int c = 0; c < gridCols; c++)
            {
                int x = xstep * c;
                int y = ystep * r;
                int xw = x + xstep;
                int yh = y + ystep;

                if (x < 0)x = 0;
                if (y < 0)y = 0;
                if (xw > cols)xw = cols;
                if (yh > rows)yh = rows;

                Rect sr(x, y, xw - x, yh - y);
                Mat srm = cimg(sr);

                int noZeroCount = countNonZero(srm);
                int circleRadius = getCentroidCircleRadius(cimg2(sr));

                checkedMat.at<float>(r, c) = noZeroCount*1.0f / (standRectArea > 0 ? standRectArea : sr.area());
                optionAllCircle.at<float>(r, c) = float(circleRadius * 2) / float(minStandRect > 0 ? minStandRect : sr.height);
            }
        }

        for (int q = 0; q < questionAmount; q++)
        {
            Json::Value val;
            vector<double> tempOptionCircleVal; //存储各选项占比
            for (int i = 0; i < itemNumber; i++)
            {
                int x, y;
                if (questionOrientation == Horizontal) //题目排列方向为横向
                {
                    int qr = q / itemsPerRow;
                    int qc = q % itemsPerRow;

                    if (orientation == Horizontal) //选项排列方式为横向
                    {
                        if (containNumber)
                        {
                            x = qc*(itemNumber + 1) + i + 1;
                            y = qr;
                        }
                        else
                        {
                            x = qc*(itemNumber)+i;
                            y = qr;
                        }
                    }
                    else //选项排列方式为纵向
                    {
                        if (containNumber)
                        {
                            x = qc;
                            y = qr*(itemNumber + 1) + i + 1;
                        }
                        else
                        {
                            x = qc;
                            y = qr*(itemNumber)+i;
                        }
                    }
                }
                else
                {
                    int perColItems = ceil(questionAmount*1.0f / itemsPerRow);
                    int qr = q % perColItems;
                    int qc = q / perColItems;

                    if (orientation == Horizontal)
                    {
                        if (containNumber)
                        {
                            x = qc*(itemNumber + 1) + i + 1;
                            y = qr;
                        }
                        else
                        {
                            x = qc*(itemNumber)+i;
                            y = qr;
                        }
                    }
                    else
                    {
                        if (containNumber)
                        {
                            x = qc;
                            y = qr*(itemNumber + 1) + i + 1;
                        }
                        else
                        {
                            x = qc;
                            y = qr*(itemNumber)+i;
                        }
                    }
                }

                val.append(checkedMat.at<float>(y, x));
                tempOptionCircleVal.push_back(optionAllCircle.at<float>(y, x));
            }

            //求质心圆直径方案
            //当占比方差满足条件时则进一步判断填涂笔迹圆直径
            if (itemNumber > 1)
            {
                for (int i = 0; i < itemNumber; i++)
                {
                    val.append(tempOptionCircleVal[i] < 0.1 ? 0.1 : tempOptionCircleVal[i]); //返回直径占比
                }
            }

            result.append(val);
        }
    }
    isCorrectInput = true;
}
