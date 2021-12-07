#ifndef IMAGEMAT_H
#define IMAGEMAT_H


class ImageMat
{
public:
    ImageMat();

    bool saveImage(string filename, SettingContent* sc, Mat& mat);
};

#endif // IMAGEMAT_H
