#include "subjectiveitem.h"
#include "subjectiveitem.h"
#include "../utils/imagemat.h"


void SubjectiveItem::Do(Mat& img)
{
    if (!isSaveImage) return;

    Rect cRect(rect.x, rect.y, rect.width, rect.height);
    if (cRect.x < 0 || cRect.y < 0 || cRect.x + cRect.width >= img.cols || cRect.y + cRect.height >= img.rows)
    {
        isCorrectInput = false;
        result = "The rect is outof the image.";
        return;
    }

    Mat cimg;
    img(cRect).copyTo(cimg);

    string format = ".jpg";
    if (settingContent->saveImageFormat == Format_PNG)
        format = ".png";
    string filename = settingContent->imageSavePath + id + format;
    ImageMat imageMat;

    if (imageMat.saveImage(filename, settingContent, cimg))
    {
        isCorrectInput = true;
        saveFileUrl = filename;
    }
    else
    {
        isCorrectInput = false;
        result = "Can Not Saved";
    }
    cimg.release();
}
