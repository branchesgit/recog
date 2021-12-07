#include "imagemat.h"
#include "../items/settingcontent.h"


ImageMat::ImageMat()
{

}


bool ImageMat::saveImage(string filename, SettingContent* sc, Mat& mat)
{
    if (sc->saveImageScale <= 0) sc->saveImageScale = 1.0f;
    if (sc->saveImageScale == 1.0f)
    {
        if (sc->isThreshBinary)
        {
            threshold(mat, mat, sc->threshBinaryValue, 255, THRESH_BINARY);
        }
        return imwrite(filename, mat, sc->saveParams);
    }
    else
    {
        int w = sc->saveImageScale*mat.cols;
        int h = sc->saveImageScale*mat.rows;

        Mat tempMat;
        resize(mat, tempMat, Size(w, h));
        if (sc->isThreshBinary)
        {
            threshold(tempMat, tempMat, sc->threshBinaryValue, 255, THRESH_BINARY);
        }
        return imwrite(filename, tempMat, sc->saveParams);
    }

    return false;
}
