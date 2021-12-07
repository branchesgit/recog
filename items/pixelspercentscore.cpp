#include "pixelspercentscore.h"

PixelsPercentScore::PixelsPercentScore()
{

}

void PixelsPercentScore::Do(Mat& img)
{
    if (rect.x < 0 || rect.y < 0 || rect.x + rect.width >= img.cols || rect.y + rect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }

    //首先计算要扩大的区域
    float cellw = rect.width * 1.0f / itemNumber;   //每个格子的宽度
    float cellh = rect.height * 1.0f / row;			//每个格子的高度

    int margin = max(cellw, cellh) / 5;			//如果偏移超过这个值则一般看看定位是否有问题
    if (margin < 15)margin = 15;				//最少15像素

    int marginLeft = margin;
    int marginRight = margin;
    int marginTop = margin;
    int marginBottom = margin;
    //重新确认搜索区域
    int x = rect.x - marginLeft;
    if (x < 0)
    {
        x = 0;
        marginLeft = rect.x;
    }
    int y = rect.y - marginTop;
    if (y < 0)
    {
        y = 0;
        marginTop = rect.y;
    }
    int x2 = rect.x + rect.width + marginRight;
    if (x2 > img.cols - 1)
    {
        x2 = img.cols - 1;
        marginRight = img.cols - 1 - rect.x - rect.width;
    }
    int y2 = rect.y + rect.height + marginBottom;
    if (y2 > img.rows - 1)
    {
        y2 = img.rows - 1;
        marginBottom = img.rows - 1 - rect.y - rect.height;
    }
    //要搜索的图像区域
    Rect cRect(x, y, x2 - x + 1, y2 - y + 1);
    //要搜索的图像
    Mat cimg;
    img(cRect).copyTo(cimg);

    //暂时先不保存图像
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

    //图像倾斜校正
    threshold(cimg, cimg, 10, 255, THRESH_OTSU);
    cimg = 255 - cimg;

    //直线模板匹配的方法
    int rows = cimg.rows;
    int cols = cimg.cols;

    int* rowps = new int[rows];
    findHorizontalLine(cimg, rowps);
    for (int i = 1; i < rows - 1; i++)
    {
        if ((rowps[i] < rowps[i - 1]) && (rowps[i] < rowps[i + 1])
            && abs(rowps[i - 1] - rowps[i + 1])<rect.width*0.2)
        {
            rowps[i] = (rowps[i - 1] + rowps[i + 1]) / 2;
        }
    }

    //横线的阈值
    float rowLineThresholdPixels = rect.width*0.6;
    //查找横线的位置
    std::vector<float> horizontalLinePos;
    std::vector<int> candidateLinePos;
    int lastPos = -100;
    int maxRowLineWidth = -1;
    for (int i = 0; i < rows; i++)
    {
        if (rowps[i]>rowLineThresholdPixels)
        {
            if (i - lastPos > 2)
            {
                if (candidateLinePos.size()>0)
                {
                    float cs = 0;
                    int cc = candidateLinePos.size();
                    for (int c = 0; c <cc; c++)
                    {
                        cs += candidateLinePos[c];
                    }
                    horizontalLinePos.push_back(cs / cc);
                    if (maxRowLineWidth < cc)
                    {
                        maxRowLineWidth = cc;
                    }
                }
                candidateLinePos.clear();
                candidateLinePos.push_back(i);
            }
            else
            {
                for (int p = lastPos + 1; p <= i; p++)
                {
                    candidateLinePos.push_back(p);
                }
            }
            lastPos = i;
        }
    }
    if (candidateLinePos.size()>0)
    {
        float cs = 0;
        int cc = candidateLinePos.size();
        for (int c = 0; c <cc; c++)
        {
            cs += candidateLinePos[c];
        }
        horizontalLinePos.push_back(cs / cc);
    }
    delete[] rowps;

    //预测最接近的横线
    vector<float> standRowLinePos;
    vector<float> trueRowLinePos;
    for (int i = 0; i <= row; i++)
    {
        float y = marginTop + i*cellh;
        standRowLinePos.push_back(y);

        int cc = -1;
        float minValue = 2 * rect.height;
        for (int c = 0; c < horizontalLinePos.size(); c++)
        {
            float v = fabsf(horizontalLinePos[c] - y);
            if (v < minValue)
            {
                minValue = v;
                cc = c;
            }
        }

        if (minValue < cellh / 4)
        {
            trueRowLinePos.push_back(horizontalLinePos[cc]);
        }
        else
        {
            trueRowLinePos.push_back(-1);
        }
    }
    //估计所有值
    for (int i = 0; i < trueRowLinePos.size(); i++)
    {
        if (trueRowLinePos[i] < 0)
        {
            //找到最近的非false的值
            int minDist = trueRowLinePos.size() * 2;
            int ti = -1;
            for (int j = 0; j < trueRowLinePos.size(); j++)
            {
                if (trueRowLinePos[j] > -1)
                {
                    if (abs(j - i) < minDist)
                    {
                        minDist = abs(j - i);
                        ti = j;
                    }
                }
            }
            if (ti != -1)
            {
                trueRowLinePos[i] = trueRowLinePos[ti] + cellh*(i - ti);
            }
            else
            {
                for (int j = 0; j < trueRowLinePos.size(); j++)
                {
                    trueRowLinePos[j] = standRowLinePos[j];
                }
                break;
            }
        }
    }

    //判断完横线在判断竖线
    //要搜索的图像区域
    Rect vRect(0, trueRowLinePos[0], cimg.cols, trueRowLinePos[1] - trueRowLinePos[0] + 1);
    if (vRect.y<0 || vRect.y + vRect.height>cimg.rows)
    {
        Json::Value val;
        result.append(val);
        return;
    }
    //要搜索的图像
    Mat vimg;
    cimg(vRect).copyTo(vimg);

    int* colps = new int[vimg.cols];
    findVerticalLine(vimg, colps);
    for (int i = 1; i < cols - 1; i++)
    {
        if ((colps[i] < colps[i - 1]) && (colps[i] < colps[i + 1])
            && abs(colps[i - 1] - colps[i + 1])<rect.height*0.2)
        {
            colps[i] = (colps[i - 1] + colps[i + 1]) / 2;
        }
    }
    float colLineThresholdPixels = vimg.rows*0.65;
    std::vector<float> verticalLinePos;
    int maxColLineWidth = -1;
    lastPos = -100;
    candidateLinePos.clear();
    for (int i = 0; i < cols; i++)
    {
        if (colps[i]>colLineThresholdPixels)
        {
            if (i - lastPos > 2)
            {
                if (candidateLinePos.size()>0)
                {
                    float cs = 0;
                    int cc = candidateLinePos.size();
                    for (int c = 0; c <cc; c++)
                    {
                        cs += candidateLinePos[c];
                    }
                    verticalLinePos.push_back(cs / cc);
                    if (maxColLineWidth < cc)
                    {
                        maxColLineWidth = cc;
                    }
                }
                candidateLinePos.clear();
                candidateLinePos.push_back(i);
            }
            else
            {
                for (int p = lastPos + 1; p <= i; p++)
                {
                    candidateLinePos.push_back(p);
                }
            }
            lastPos = i;
        }
    }
    if (candidateLinePos.size()>0)
    {
        float cs = 0;
        int cc = candidateLinePos.size();
        for (int c = 0; c <cc; c++)
        {
            cs += candidateLinePos[c];
        }
        verticalLinePos.push_back(cs / cc);
    }
    delete[] colps;

    //自动补线
    //根据相对间距推算出所有缺失的线
    if (verticalLinePos.size() > 1)
    {
        if (verticalLinePos[1] - verticalLinePos[0] < (cellw * 0.8))
        {
            if (verticalLinePos[0] - cellw > 5)
                verticalLinePos.insert(verticalLinePos.begin(), verticalLinePos[0] - cellw);
            else if (verticalLinePos[1] - cellw > 5)
                verticalLinePos.insert(verticalLinePos.begin(), verticalLinePos[1] - cellw);
        }
        if (verticalLinePos.size() < (itemNumber + 1))
        {
            for (int i = 0; i < verticalLinePos.size() - 1; i++)
            {
                int spacingw = verticalLinePos[i + 1] - verticalLinePos[i];
                if (spacingw >(cellw * 1.5))
                {
                    verticalLinePos.insert(verticalLinePos.begin() + (i + 1), verticalLinePos[i] + cellw);
                }
            }
        }
        while (verticalLinePos[0] > cellw)
        {
            verticalLinePos.insert(verticalLinePos.begin(), verticalLinePos[0] - cellw);
        }
        while (verticalLinePos[verticalLinePos.size() - 1] < vimg.cols - cellw)
        {
            verticalLinePos.insert(verticalLinePos.end(), verticalLinePos[verticalLinePos.size() - 1] + cellw);
        }
    }

    vector<float> standColLinePos;
    vector<float> trueColLinePos;
    for (int i = 0; i <= itemNumber; i++)
    {
        float x = marginLeft + i*cellw;
        if (verticalLinePos.size() > 0)
        {
            if (verticalLinePos[0] > marginLeft)
            {
                x = x + fabsf(marginLeft - verticalLinePos[0]);
            }
            else if (verticalLinePos[0] < marginLeft)
            {
                x = x - fabsf(marginLeft - verticalLinePos[0]);
            }
        }

        standColLinePos.push_back(x);

        int cc = -1;
        float minValue = 2 * rect.width;
        for (int c = 0; c < verticalLinePos.size(); c++)
        {
            float v = fabsf(verticalLinePos[c] - x);
            if (v < minValue)
            {
                minValue = v;
                cc = c;
            }
        }

        if (minValue < cellw / 5)
        {
            trueColLinePos.push_back(verticalLinePos[cc]);
        }
        else
        {
            trueColLinePos.push_back(-1);
        }
    }
    for (int i = 0; i < trueColLinePos.size(); i++)
    {
        if (trueColLinePos[i] < 0)
        {
            //找到最近的非false的值
            int minDist = trueColLinePos.size() * 2;
            int ti = -1;
            for (int j = 0; j < trueColLinePos.size(); j++)
            {
                if (trueColLinePos[j] > -1)
                {
                    if (abs(j - i) < minDist)
                    {
                        minDist = abs(j - i);
                        ti = j;
                    }
                }
            }
            if (ti != -1)
            {
                trueColLinePos[i] = trueColLinePos[ti] + cellw*(i - ti);
            }
            else
            {
                for (int j = 0; j < trueColLinePos.size(); j++)
                {
                    trueColLinePos[j] = standColLinePos[j];
                }
                break;
            }
        }
    }

    if (maxRowLineWidth < 0 || maxRowLineWidth>cellh / 8)
        maxRowLineWidth = cellh / 9;
    else
        maxRowLineWidth += 1;

    if (maxColLineWidth < 0 || maxColLineWidth>cellw / 8)
        maxColLineWidth = cellw / 9;
    else
        maxColLineWidth += 1;

    for (int r = 0; r < row; r++)
    {
        Json::Value val;

        float th = trueRowLinePos[r + 1] - trueRowLinePos[r] - (3 * maxRowLineWidth) - 2;
        for (int c = 0; c < itemNumber; c++)
        {
            float shiftCell = (trueColLinePos[c + 1] - trueColLinePos[c]) / 3.0f;
            float tw = trueColLinePos[c + 1] - trueColLinePos[c] - shiftCell - 2 * maxColLineWidth;

            if (tw <= 0 || th <= 0)
            {
                val.append(0);
                continue;
            }

            float sx = trueColLinePos[c] + maxColLineWidth + shiftCell;
            float sy = trueRowLinePos[r] + (maxRowLineWidth * 2);

            if (sx < 0 || sy < 0 || sx + tw >= cimg.cols || sy + th >= cimg.rows)
            {
                val.append(0);
                continue;
            }

            Mat subMat;
            Rect2f srect(sx, sy, tw, th);
            cimg(srect).copyTo(subMat);

            //过滤噪点
            Mat kernel = getStructuringElement(MORPH_RECT, Size(2, 2));
            erode(subMat, subMat, kernel);

            float area = subMat.cols*subMat.rows;
            float fact = countNonZero(subMat) / area;
            val.append(fact);
        }
        result.append(val);
    }
    isCorrectInput = true;
}
